#include "TsuhgiAIO.h"
#include "EventManager.h"
#include "lpputils.h"
#include <algorithm>


TsuhgiAIO::TsuhgiAIO(IPluginSDK *sdk)
{
	this->sdk = sdk;
	auto player = sdk->GetEntityList()->Player();

	std::string champName = player->ChampionName();

	if (champName == "Ezreal")
	{
		this->ezreal();
	}
	if (champName == "Lucian")
	{
		this->lucian();
	}
}

void TsuhgiAIO::ezreal()
{
	this->menu = this->sdk->AddMenu("Tsuhgi Ezreal");
	static auto comboMenu = this->menu->AddMenu("Combo Options");
	static auto useQCombo = comboMenu->CheckBox("Use Q", true);
	static auto useWCombo = comboMenu->CheckBox("Use W", true);

	static auto harassMenu = this->menu->AddMenu("Harass Options");
	static auto useQHarass = harassMenu->CheckBox("Use Q", true);
	static auto useWHarass = harassMenu->CheckBox("Use W", true);

	static auto miscMenu = this->menu->AddMenu("Misc Options");
	static auto ksQ = miscMenu->CheckBox("KS with Q", true);
	static auto useUlt = miscMenu->AddKey("Use Ult", 'V');

	static auto drawMenu = this->menu->AddMenu("Draw Options");
	static auto drawQRange = drawMenu->CheckBox("Draw Q Range", false);
	static auto qRangeColor = drawMenu->AddColor("Q Range Color", 0.f, 0.f, 255.f, 255.f);
	static auto drawWRange = drawMenu->CheckBox("Draw W Range", false);
	static auto wRangeColor = drawMenu->AddColor("W Range Color", 0.f, 0.f, 255.f, 255.f);

	static auto game = this->sdk->GetGame();
	static auto entityList = this->sdk->GetEntityList();
	static auto player = entityList->Player();
	static auto orbwalking = this->sdk->GetOrbwalking();
	static auto spellDataReader = this->sdk->GetSpellDataReader();
	static auto damageCalc = this->sdk->GetDamage();
	static auto renderer = this->sdk->GetRenderer();

	static auto q = this->sdk->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithMinions | kCollidesWithYasuoWall));
	q->SetSkillshot(0.25f, 60.f, 2000.f, 1200.f);

	static auto w = this->sdk->CreateSpell2(kSlotW, kLineCast, true, true, kCollidesWithYasuoWall);
	w->SetSkillshot(0.25f, 80.f, 1600.f, 800.f);

	static auto e = this->sdk->CreateSpell2(kSlotE, kLineCast, false, false, kCollidesWithNothing);
	e->SetOverrideRange(475.f);

	static auto r = this->sdk->CreateSpell2(kSlotR, kLineCast, true, true, kCollidesWithYasuoWall);
	r->SetSkillshot(1.0f, 160.f, 2000.f, FLT_MAX);

	eventmanager::GameEventManager::RegisterUpdateEvent([&](event_id_t id) -> void {
		auto mode = orbwalking->GetOrbwalkingMode();

		if (mode == kModeCombo)
		{
			if (useQCombo->Enabled() && q->IsReady() && orbwalking->CanMove())
			{
				auto target = this->findTarget(q->Range(), PhysicalDamage);

				if (target != nullptr && target->IsValidTarget())
				{
					q->CastOnTarget(target);
				}
			}

			if (useWCombo->Enabled() && w->IsReady() && orbwalking->CanMove(w->GetDelay()) && LPPUtils::CountEnemiesInAttackRange(this->sdk) <= 0)
			{
				auto target = this->findTarget(w->Range(), SpellDamage);

				if (target != nullptr && target->IsValidTarget())
				{
					w->CastOnTarget(target);
				}
			}
		}
		else if (mode == kModeMixed)
		{
			if (useQHarass->Enabled() && q->IsReady() && orbwalking->CanMove())
			{
				auto target = this->findTarget(q->Range(), PhysicalDamage);

				if (target != nullptr && target->IsValidTarget())
				{
					q->CastOnTarget(target);
				}
			}

			if (useWHarass->Enabled() && w->IsReady() && orbwalking->CanMove(w->GetDelay()) && LPPUtils::CountEnemiesInAttackRange(this->sdk) <= 0)
			{
				auto target = this->findTarget(w->Range(), SpellDamage);

				if (target != nullptr && target->IsValidTarget())
				{
					w->CastOnTarget(target);
				}
			}
		}

		if (LPPUtils::IsKeyDown(useUlt) && r->IsReady())
		{
			auto mousePos = game->CursorPosition();

			auto lowest = std::pair<float, IUnit *>(FLT_MAX, nullptr);
			for (auto hero : entityList->GetAllHeros(false, true))
			{
				if (hero->IsValidTarget())
				{
					auto distance = (mousePos - hero->ServerPosition()).Length2D();

					if (distance <= 350 && distance < lowest.first)
					{
						lowest = std::pair<float, IUnit *>(distance, hero);
					}
				}
			}


			if (lowest.first <= 350 && lowest.second != nullptr)
			{
				r->CastOnTarget(lowest.second, kHitChanceVeryHigh);
			}
		}

		if (ksQ->Enabled())
		{
			for (auto hero : entityList->GetAllHeros(false, true))
			{
				if (player->IsValidTarget(hero, q->Range()) && damageCalc->GetSpellDamage(player, hero, kSlotQ) >= hero->GetHealth())
				{
					q->CastOnTarget(hero);
				}
			}
		}
	});

	eventmanager::UnitEventManager::RegisterDoCastEvent([&](event_id_t id, CastedSpell const &spell) -> void {
		if (spellDataReader->IsAutoAttack(spell.Data_) && spellDataReader->GetCaster(spell.Data_)->GetNetworkId() == player->GetNetworkId())
		{
			auto target = spellDataReader->GetTarget(spell.Data_);

			if (target != nullptr && target->IsHero() && target->GetHealth() > damageCalc->GetAutoAttackDamage(player, target, true))
			{
				auto mode = orbwalking->GetOrbwalkingMode();

				if (mode == kModeCombo)
				{
					if (q->IsReady() && useQCombo->Enabled())
					{
						auto qTarget = this->findTarget(q->Range(), PhysicalDamage);

						if (qTarget != nullptr && qTarget->IsValidTarget())
						{
							LPPUtils::RepeatUntil(this->sdk, [&, qTarget]() -> void {
								q->CastOnTarget(qTarget);
							}, [&]() -> bool {
								return !q->IsReady();
							}, 25);
						}
					}
					else if (w->IsReady() && useWCombo->Enabled() && (w->GetDelay() <= (player->WindupTime() * 2)))
					{
						auto wTarget = this->findTarget(w->Range(), SpellDamage);

						if (wTarget != nullptr && wTarget->IsValidTarget())
						{
							w->CastOnTarget(wTarget, kHitChanceMedium);
						}
					}
				}
				else if (mode == kModeMixed)
				{
					if (q->IsReady() && useQHarass->Enabled())
					{
						auto qTarget = this->findTarget(q->Range(), PhysicalDamage);

						if (qTarget != nullptr && qTarget->IsValidTarget())
						{
							LPPUtils::RepeatUntil(this->sdk, [&, qTarget]() -> void {
								q->CastOnTarget(qTarget);
							}, [&]() -> bool {
								return !q->IsReady();
							}, 25);
						}
					}
					else if (w->IsReady() && useWHarass->Enabled() && (w->GetDelay() <= (player->WindupTime() * 2)))
					{
						auto wTarget = this->findTarget(w->Range(), SpellDamage);

						if (wTarget != nullptr && wTarget->IsValidTarget())
						{
							w->CastOnTarget(wTarget, kHitChanceMedium);
						}
					}
				}
			}
		}
	});

	eventmanager::DrawEventManager::RegisterRenderEvent([&](event_id_t id) -> void {
		if (LPPUtils::IsKeyDown(useUlt) && r->IsReady())
		{
			static auto color = Vec4(255, 0, 0, 255);

			renderer->DrawOutlinedCircle(game->CursorPosition(), color, 350.f);
		}

		auto pos = player->GetPosition();

		if (drawQRange->Enabled())
		{
			Vec4 color;
			qRangeColor->GetColor(&color);

			renderer->DrawOutlinedCircle(pos, color, q->Range());
		}

		if (drawWRange->Enabled())
		{
			Vec4 color;
			wRangeColor->GetColor(&color);

			renderer->DrawOutlinedCircle(pos, color, w->Range());
		}
	});
}

void TsuhgiAIO::lucian()
{
	this->menu = this->sdk->AddMenu("Tsuhgi Lucian");

	static auto comboMenu = this->menu->AddMenu("Combo Settings");
	static auto useQCombo = comboMenu->CheckBox("Use Q", true);
	static auto useWCombo = comboMenu->CheckBox("Use W", true);
	static auto useECombo = comboMenu->CheckBox("Use E", true);

	static auto harassMenu = this->menu->AddMenu("Harass Settings");
	static auto useQHarass = harassMenu->CheckBox("Use Q", true);
	static auto useWHarass = harassMenu->CheckBox("Use W", true);
	static auto useEHarass = harassMenu->CheckBox("Use E", true);
	static auto minManaHarass = harassMenu->AddFloat("Min Mana", 0.f, 100.f, 30.f);

	static auto laneClearMenu = this->menu->AddMenu("Lane Clear Settings");
	static auto useQLaneClear = laneClearMenu->CheckBox("Use Q", false);
	static auto minQCountLaneClear = laneClearMenu->AddInteger("Min Q Count", 1, 5, 3);

	static auto jungleClearMenu = this->menu->AddMenu("Jungle Clear Settings");
	static auto useQJungleClear = jungleClearMenu->CheckBox("Use Q", true);
	static auto useWJungleClear = jungleClearMenu->CheckBox("Use W", true);
	static auto useEJungleClear = jungleClearMenu->CheckBox("Use E", true);

	static auto miscMenu = this->menu->AddMenu("Misc Settings");
	static auto useQAutoHarass = miscMenu->CheckBox("Auto Extended Q", true);
	static auto minManaQAutoHarass = miscMenu->AddFloat("Min Mana For Extended Q", 0.f, 100.f, 30.f);
	static auto eMode = comboMenu->AddSelection("E Mode", 0, { "Side", "Mouse Position", "Target Position" });

	static auto drawMenu = this->menu->AddMenu("Draw Settings");
	static auto drawQRange = drawMenu->CheckBox("Draw Q Range", false);
	static auto qRangeColor = drawMenu->AddColor("Q Range Color", 0.f, 0.f, 255.f, 255.f);
	static auto drawExtendedQRange = drawMenu->CheckBox("Draw Extended Q Range", false);
	static auto extendedQRangeColor = drawMenu->AddColor("Extended Q Range Color", 0.f, 0.f, 255.f, 255.f);
	static auto drawWRange = drawMenu->CheckBox("Draw W Range", false);
	static auto wRangeColor = drawMenu->AddColor("W Range Color", 0.f, 0.f, 255.f, 255.f);
	static auto drawRRange = drawMenu->CheckBox("Draw R Range", false);
	static auto rRangeColor = drawMenu->AddColor("R Range Color", 0.f, 0.f, 255.f, 255.f);

	static auto game = this->sdk->GetGame();
	static auto entityList = this->sdk->GetEntityList();
	static auto player = entityList->Player();
	static auto orbwalking = this->sdk->GetOrbwalking();
	static auto damage = this->sdk->GetDamage();
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

					AdvPredictionOutput output;
					if (extendedQ->RunPrediction(target, false, kCollidesWithNothing, &output))
					{
						if (output.HitChance >= kHitChanceMedium)
						{
							auto targetPos = LPPUtils::Extend(myPos, output.TargetPosition, extendedRange);

							for (auto minion : entityList->GetAllMinions(false, true, true))
							{
								if (player->IsValidTarget(minion, range))
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

	static auto renderer = this->sdk->GetRenderer();

	eventmanager::DrawEventManager::RegisterRenderEvent([&](event_id_t id) -> void {
		auto pos = player->GetPosition();
		
		if (drawQRange->Enabled())
		{
			Vec4 color;
			qRangeColor->GetColor(&color);

			renderer->DrawOutlinedCircle(pos, color, q->Range());
		}

		if (drawExtendedQRange->Enabled())
		{
			Vec4 color;
			extendedQRangeColor->GetColor(&color);

			renderer->DrawOutlinedCircle(pos, color, extendedQ->Range());
		}

		if (drawWRange->Enabled())
		{
			Vec4 color;
			wRangeColor->GetColor(&color);

			renderer->DrawOutlinedCircle(pos, color, w->Range());
		}

		if (drawRRange->Enabled())
		{
			Vec4 color;
			rRangeColor->GetColor(&color);

			renderer->DrawOutlinedCircle(pos, color, r->Range());
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