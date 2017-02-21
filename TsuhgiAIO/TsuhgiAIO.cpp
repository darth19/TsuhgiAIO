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

//	if (champName == "ezreal")
//	{
//		this->ezreal();
//	}
	if (champName == "lucian")
	{
		this->lucian();
	}
}

void TsuhgiAIO::ezreal()
{
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

	static auto laneClearMenu = this->menu->AddMenu("Lane Clear Settings");
	static auto useQLaneClear = laneClearMenu->CheckBox("Use Q", false);
	static auto minQCountLaneClear = laneClearMenu->AddInteger("Min Q Count", 1, 5, 3);

	static auto jungleClearMenu = this->menu->AddMenu("Jungle Clear Settings");
	static auto useQJungleClear = jungleClearMenu->CheckBox("Use Q", true);
	static auto useWJungleClear = jungleClearMenu->CheckBox("Use W", true);
	static auto useEJungleClear = jungleClearMenu->CheckBox("Use E", true);

	static auto minManaHarass = harassMenu->AddFloat("Min Mana", 0.f, 100.f, 30.f);
	static auto miscMenu = this->menu->AddMenu("Misc Settings");
	static auto useQAutoHarass = miscMenu->CheckBox("Auto Extended Q", true);
	static auto minManaQAutoHarass = miscMenu->AddFloat("Min Mana For Extended Q", 0.f, 100.f, 30.f);
	static auto eMode = comboMenu->AddInteger("E position (0 = side, 1 = mouse, 2 = target) ", 0, 2, 0);

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

	//ty based hoola
	auto deviation = [&](Vec2 p1, Vec2 p2, double angle) -> Vec2 {
		auto rads = static_cast<float>(angle * (M_PI / 180.0));

		auto diff = (p2 - p1);
		return Vec2((diff.x * cos(rads) - diff.y * sin(rads)) / 4.f, (diff.x * sin(rads) + diff.y * cos(rads)) / 4.f) + p1;
	};

	eventmanager::GameEventManager::RegisterUpdateEvent([&](event_id_t id) -> void {
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

					for (auto minion : entityList->GetAllMinions(false, true, true))
					{
						if (player->IsValidTarget(minion, range) && !minion->IsWard())
						{
							auto minionPos = minion->GetPosition();
							auto pos = LPPUtils::Extend(myPos, minionPos, extendedRange);

							if ((targetPos - pos).Length2D() <= extendedQ->Radius())
							{
								q->CastOnUnit(minion);
							}
						}
					}
				}
			}
		}

		if (mode == kModeLaneClear && useQLaneClear->Enabled())
		{
			auto minQCount = minQCountLaneClear->GetInteger();

			if (minQCount > 0)
			{
				auto range = q->Range();
				auto extendedRange = extendedQ->Range();
				auto myPos = player->GetPosition();
				auto qRadius = extendedQ->Radius();

				auto enemyMinions = entityList->GetAllMinions(false, true, false);

				for (auto minion : enemyMinions)
				{
					if (player->IsValidTarget(minion, range) && minion->IsCreep())
					{
						auto minionPos = minion->GetPosition();
						auto pos = LPPUtils::Extend(myPos, minionPos, extendedRange);

						auto networkId = minion->GetNetworkId();

						auto count = std::count_if(enemyMinions.begin(), enemyMinions.end(),
							[&](IUnit *unit) -> bool {
							if (player->IsValidTarget(unit, extendedRange) && unit->IsCreep() && unit->GetNetworkId() != networkId)
							{
								auto targetPos = LPPUtils::Extend(myPos, unit->GetPosition(), extendedRange);

								return (pos - targetPos).Length2D() <= qRadius;
							}

							return false;
						}) + 1;

						if (count >= minQCount)
						{
							q->CastOnUnit(minion);
							break;
						}
					}
				}
			}
		}
	});

	eventmanager::UnitEventManager::RegisterDoCastEvent([&](event_id_t id, CastedSpell const &spell) -> void {
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
						Vec3 pos;
						
						auto m = eMode->GetInteger();

						if (m == 0)
						{
							pos = LPPUtils::To3D(this->sdk, deviation(LPPUtils::To2D(player->GetPosition()), LPPUtils::To2D(target->GetPosition()), 65));
						}
						else if (m == 1)
						{
							pos = game->CursorPosition();
						}
						else
						{
							pos = LPPUtils::Extend(player->GetPosition(), target->GetPosition(), 50.f);
						}

						LPPUtils::RepeatUntil(this->sdk, [&, pos]() -> void {
							e->CastOnPosition(pos);
						}, [&]() -> bool {
							return !e->IsReady();
						}, 25);
					}
					else if (q->IsReady() && useQCombo->Enabled())
					{
						auto qTarget = this->findTarget(q->Range(), PhysicalDamage);

						if (qTarget != nullptr && qTarget->IsValidTarget())
						{
							LPPUtils::RepeatUntil(this->sdk, [&, qTarget]() -> void {
								q->CastOnUnit(qTarget);
							}, [&]() -> bool {
								return !q->IsReady();
							}, 25);
						}
					}
					else if (w->IsReady() && useWCombo->Enabled())
					{
						auto wTarget = this->findTarget(w->Range(), SpellDamage);

						if (wTarget != nullptr && wTarget->IsValidTarget())
						{
							LPPUtils::RepeatUntil(this->sdk, [&, wTarget]() -> void {
								w->CastOnTarget(wTarget);
							}, [&]() -> bool {
								return !w->IsReady();
							}, 25);
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
						Vec3 pos;

						auto m = eMode->GetInteger();

						if (m == 0)
						{
							pos = LPPUtils::To3D(this->sdk, deviation(LPPUtils::To2D(player->GetPosition()), LPPUtils::To2D(target->GetPosition()), 65));
						}
						else if (m == 1)
						{
							pos = game->CursorPosition();
						}
						else
						{
							pos = LPPUtils::Extend(player->GetPosition(), target->GetPosition(), 50.f);
						}

						LPPUtils::RepeatUntil(this->sdk, [&, pos]() -> void {
							e->CastOnPosition(pos);
						}, [&]() -> bool {
							return !e->IsReady();
						}, 25);
					}
					else if (q->IsReady() && useQHarass->Enabled())
					{
						auto qTarget = this->findTarget(q->Range(), PhysicalDamage);

						if (qTarget != nullptr && qTarget->IsValidTarget())
						{
							LPPUtils::RepeatUntil(this->sdk, [&, qTarget]() -> void {
								q->CastOnUnit(qTarget);
							}, [&]() -> bool {
								return !q->IsReady();
							}, 25);
						}
					}
					else if (w->IsReady() && useWHarass->Enabled())
					{
						auto wTarget = this->findTarget(w->Range(), SpellDamage);

						if (wTarget != nullptr && wTarget->IsValidTarget())
						{
							LPPUtils::RepeatUntil(this->sdk, [&, wTarget]() -> void {
								w->CastOnTarget(wTarget);
							}, [&]() -> bool {
								return !w->IsReady();
							}, 25);
						}
					}
				}
			}
			else if (mode == kModeLaneClear && player->ManaPercent() >= minManaHarass->GetFloat())
			{
				auto target = spellDataReader->GetTarget(spell.Data_);

				if (target != nullptr && target->IsJungleCreep() && target->IsValidTarget() && damage->GetAutoAttackDamage(player, target, true) < target->GetHealth())
				{
					if (e->IsReady() && useEJungleClear->Enabled())
					{
						auto pos = LPPUtils::To3D(this->sdk, deviation(LPPUtils::To2D(player->GetPosition()), LPPUtils::To2D(target->GetPosition()), 65));
						LPPUtils::RepeatUntil(this->sdk, [&, pos]() -> void {
							e->CastOnPosition(pos);
						}, [&]() -> bool {
							return !e->IsReady();
						}, 25);
					}
					else if (q->IsReady() && useQJungleClear->Enabled())
					{
						LPPUtils::RepeatUntil(this->sdk, [&, target]() -> void {
							q->CastOnUnit(target);
						}, [&]() -> bool {
							return !q->IsReady();
						}, 25);
					}
					else if (w->IsReady() && useWJungleClear->Enabled())
					{
						LPPUtils::RepeatUntil(this->sdk, [&, target]() -> void {
							w->CastOnTarget(target);
						}, [&]() -> bool {
							return !w->IsReady();
						}, 25);
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