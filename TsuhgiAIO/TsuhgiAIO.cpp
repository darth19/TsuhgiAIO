#include "TsuhgiAIO.h"
#include <boost/algorithm/string.hpp>
#include "EventManager.h"
#include "lpputils.h"


TsuhgiAIO::TsuhgiAIO(IPluginSDK *sdk)
{
	this->sdk = sdk;
	auto player = sdk->GetEntityList()->Player();

	std::string champName = player->ChampionName();
	boost::algorithm::to_lower(champName);

	if (champName == "lucian")
	{
		this->lucian();
	}
}

void TsuhgiAIO::lucian()
{
	this->menu = this->sdk->AddMenu("Lucian");
	static auto comboMenu = this->menu->AddMenu("Combo Settings");
	static auto useQCombo = comboMenu->CheckBox("Use Q", true);
	static auto useWCombo = comboMenu->CheckBox("Use W", true);
	static auto useECombo = comboMenu->CheckBox("Use E", true);

	static auto harassMenu = this->menu->AddMenu("Harass Settings");
	static auto useQHarass = harassMenu->CheckBox("Use Q", true);
	static auto useWHarass = harassMenu->CheckBox("Use W", true);
	static auto useEHarass = harassMenu->CheckBox("Use E", true);
	static auto minManaHarass = harassMenu->AddFloat("Min Mana", 0.f, 100.f, 30.f);

	static auto miscMenu = this->menu->AddMenu("Misc Settings");
	static auto useQAutoHarass = miscMenu->CheckBox("Auto Extended Q", true);
	static auto minManaQAutoHarass = miscMenu->AddFloat("Min Mana For Extended Q", 0.f, 100.f, 30.f);

	static auto game = this->sdk->GetGame();
	static auto entityList = this->sdk->GetEntityList();
	static auto player = entityList->Player();
	static auto orbwalking = this->sdk->GetOrbwalking();
	static auto damage = this->sdk->GetDamage();
	static auto prediction = this->sdk->GetPrediction();
	static auto spellDataReader = this->sdk->GetSpellDataReader();

	static auto q = this->sdk->CreateSpell2(kSlotQ, kTargetCast, true, false, kCollidesWithYasuoWall);
	q->SetOverrideRange(675.f);

	static auto extendedQ = this->sdk->CreateSpell2(kSlotQ, kLineCast, true, false, kCollidesWithYasuoWall);
	extendedQ->SetSkillshot(0.25f, 50.f, FLT_MAX, 1000.f);

	static auto w = this->sdk->CreateSpell2(kSlotW, kLineCast, true, false, kCollidesWithYasuoWall);
	w->SetSkillshot(0.30f, 80.f, 1600.f, 1100.f);

	static auto e = this->sdk->CreateSpell2(kSlotE, kLineCast, false, false, kCollidesWithNothing);
	e->SetOverrideRange(475.f);

	static auto r = this->sdk->CreateSpell2(kSlotR, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithHeroes | kCollidesWithMinions | kCollidesWithYasuoWall));
	r->SetSkillshot(0.20f, 110.f, 2500.f, 1400.f);

	eventmanager::GameEventManager::RegisterUpdateEvent([&]() -> void {
		auto mode = orbwalking->GetOrbwalkingMode();

		if ((mode == kModeCombo && useQCombo->Enabled())
			|| (mode == kModeMixed && useQHarass->Enabled() && player->ManaPercent() > minManaHarass->GetFloat())
			|| (useQAutoHarass->Enabled() && player->ManaPercent() >= minManaQAutoHarass->GetFloat()))
		{
			auto qDelay = q->GetDelay();

			if (q->IsReady() && orbwalking->CanMove(qDelay) && !player->HasBuff("LucianPassiveBuff"))
			{
				auto myPos = player->GetPosition();
				auto extendedRange = extendedQ->Range();

				auto target = this->findTarget(extendedRange, PhysicalDamage);

				if (target != nullptr && target->IsValidTarget() && (target->GetPosition() - myPos).Length2D() > player->GetRealAutoAttackRange(target))
				{
					auto range = q->Range();

					Vec3 predictedPos;
					prediction->GetFutureUnitPosition(target, qDelay, true, predictedPos);
					auto targetPos = LPPUtils::Extend(myPos, predictedPos, extendedRange);
					auto targetPos2D = Vec2(targetPos.x, targetPos.z);

					for (auto minion : entityList->GetAllMinions(false, true, true))
					{
						if (player->IsValidTarget(minion, range))
						{
							auto minionPos = minion->GetPosition();
							auto pos = LPPUtils::Extend(myPos, minionPos, extendedRange);

							auto extendedPos2D = Vec2(pos.x, pos.z);

							auto distance = (targetPos2D - extendedPos2D).Length();

							if (distance <= extendedQ->Radius())
							{
								q->CastOnUnit(minion);
							}
						}
					}
				}
			}
		}
	});

	eventmanager::UnitEventManager::RegisterDoCastEvent([&](CastedSpell const &spell) -> void {
		if (spellDataReader->IsAutoAttack(spell.Data_)
			&& spellDataReader->GetCaster(spell.Data_)->GetNetworkId() == player->GetNetworkId())
		{
			auto mode = orbwalking->GetOrbwalkingMode();

			if (mode == kModeCombo)
			{
				auto target = spellDataReader->GetTarget(spell.Data_);

				if (target != nullptr && target->IsHero() && target->IsValidTarget() && damage->GetAutoAttackDamage(player, target, true) < target->GetHealth())
				{
					if (e->IsReady() && useECombo->Enabled())
					{
						e->CastOnPosition(game->CursorPosition());
					}
					else if (q->IsReady() && useQCombo->Enabled())
					{
						auto qTarget = this->findTarget(q->Range(), PhysicalDamage);

						if (qTarget != nullptr && qTarget->IsValidTarget())
						{
							q->CastOnUnit(target);
						}
					}
					else if (w->IsReady() && useWCombo->Enabled())
					{
						auto wTarget = this->findTarget(w->Range(), SpellDamage);

						if (wTarget != nullptr && wTarget->IsValidTarget())
						{
							w->CastOnTarget(wTarget);
						}
					}
				}
			}
			else if (mode == kModeMixed && player->ManaPercent() >= minManaHarass->GetFloat())
			{
				auto target = spellDataReader->GetTarget(spell.Data_);

				if (target != nullptr && target->IsHero() && target->IsValidTarget() && damage->GetAutoAttackDamage(player, target, true) < target->GetHealth())
				{
					if (e->IsReady() && useEHarass->Enabled())
					{
						e->CastOnPosition(game->CursorPosition());
					}
					else if (q->IsReady() && useQHarass->Enabled())
					{
						auto qTarget = this->findTarget(q->Range(), PhysicalDamage);

						if (qTarget != nullptr && qTarget->IsValidTarget())
						{
							q->CastOnUnit(target);
						}
					}
					else if (w->IsReady() && useWHarass->Enabled())
					{
						auto wTarget = this->findTarget(w->Range(), SpellDamage);

						if (wTarget != nullptr && wTarget->IsValidTarget())
						{
							w->CastOnTarget(wTarget);
						}
					}
				}
			}
		}
	});
}

IUnit *TsuhgiAIO::findTarget(float range, eDamageType damage) const
{
	static auto targetselector = this->sdk->CreateTargetSelector();

	return targetselector->FindTarget(QuickestKill, damage, range);
}

TsuhgiAIO::~TsuhgiAIO()
{
	this->menu->Remove();
}
