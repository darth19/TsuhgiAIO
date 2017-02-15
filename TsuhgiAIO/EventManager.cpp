#include "EventManager.h"

namespace eventmanager
{
	/**
	* OrbwalkerEventManager
	*/
	std::vector<std::function<void(IUnit *)>> OrbwalkerEventManager::beforeAttackHandlers;
	std::vector<std::function<void(IUnit *, IUnit *)>> OrbwalkerEventManager::attackHandlers;
	std::vector<std::function<void(IUnit *, IUnit *)>> OrbwalkerEventManager::afterAttackHandlers;
	std::vector<std::function<void(IUnit *, IUnit *)>> OrbwalkerEventManager::newTargetHandlers;
	std::vector<std::function<void(IUnit *)>> OrbwalkerEventManager::nonKillableMinionHandlers;
	std::vector<std::function<IUnit *()>> OrbwalkerEventManager::findTargetHandlers;

	void OrbwalkerEventManager::RegisterBeforeAttackEvent(std::function<void(IUnit *)> func)
	{
		OrbwalkerEventManager::beforeAttackHandlers.push_back(func);
	}

	void OrbwalkerEventManager::RegisterAttackEvent(std::function<void(IUnit *, IUnit *)> func)
	{
		OrbwalkerEventManager::attackHandlers.push_back(func);
	}

	void OrbwalkerEventManager::RegisterAfterAttackEvent(std::function<void(IUnit *, IUnit *)> func)
	{
		OrbwalkerEventManager::afterAttackHandlers.push_back(func);
	}

	void OrbwalkerEventManager::RegisterNewTargetEvent(std::function<void(IUnit *, IUnit *)> func)
	{
		OrbwalkerEventManager::newTargetHandlers.push_back(func);
	}

	void OrbwalkerEventManager::RegisterNonKillableMinionEvent(std::function<void(IUnit *)> func)
	{
		OrbwalkerEventManager::nonKillableMinionHandlers.push_back(func);
	}

	void OrbwalkerEventManager::RegisterFindTargetEvent(std::function<IUnit *()> func)
	{
		OrbwalkerEventManager::findTargetHandlers.push_back(func);
	}

	/**
	* GameEventManager
	*/
	std::vector<std::function<void()>> GameEventManager::updateHandlers;
	std::vector<std::function<void()>> GameEventManager::endHandlers;
	std::vector<std::function<bool(HWND Wnd, UINT Message, WPARAM wParam, LPARAM lParam)>> GameEventManager::wndProcHandlers;

	void GameEventManager::RegisterUpdateEvent(std::function<void()> func)
	{
		GameEventManager::updateHandlers.push_back(func);
	}

	void GameEventManager::RegisterEndEvent(std::function<void()> func)
	{
		GameEventManager::endHandlers.push_back(func);
	}

	void GameEventManager::RegisterWndProcEvent(std::function<bool(HWND Wnd, UINT Message, WPARAM wParam, LPARAM lParam)> func)
	{
		GameEventManager::wndProcHandlers.push_back(func);
	}

	/**
	* DrawEventManager
	*/
	std::vector<std::function<void()>> DrawEventManager::renderHandlers;
	std::vector<std::function<void()>> DrawEventManager::renderBehindHudEventHandlers;
	std::vector<std::function<void()>> DrawEventManager::D3DPresentEventHandlers;
	std::vector<std::function<void()>> DrawEventManager::D3DPreResetEventHandlers;
	std::vector<std::function<void()>> DrawEventManager::D3DPostResetEventHandlers;

	void DrawEventManager::RegisterRenderEvent(std::function<void()> func)
	{
		DrawEventManager::renderHandlers.push_back(func);
	}

	void DrawEventManager::RegisterRenderBehindHudEvent(std::function<void()> func)
	{
		DrawEventManager::renderBehindHudEventHandlers.push_back(func);
	}

	void DrawEventManager::RegisterD3DPresentEvent(std::function<void()> func)
	{
		DrawEventManager::D3DPresentEventHandlers.push_back(func);
	}

	void DrawEventManager::RegisterD3DPreResetEvent(std::function<void()> func)
	{
		DrawEventManager::D3DPreResetEventHandlers.push_back(func);
	}

	void DrawEventManager::RegisterD3DPostResetEvent(std::function<void()> func)
	{
		DrawEventManager::D3DPostResetEventHandlers.push_back(func);
	}

	/**
	 * UnitEventManager
	 */
	std::vector<std::function<void(IUnit *)>> UnitEventManager::createHandlers;
	std::vector<std::function<void(IUnit *)>> UnitEventManager::destroyHandlers;
	std::vector<std::function<void(IUnit *)>> UnitEventManager::deathHandlers;
	std::vector<std::function<bool(IUnit *, int, Vec3 *, IUnit *)>> UnitEventManager::issueOrderHandlers;
	std::vector<std::function<bool(int, IUnit *, Vec3 *, Vec3 *)>> UnitEventManager::preCastHandlers;
	std::vector<std::function<void(int, Vec3 *, bool *, bool *)>> UnitEventManager::updateChargedSpellHandlers;
	std::vector<std::function<void(CastedSpell const &)>> UnitEventManager::processSpellCastHandlers;
	std::vector<std::function<void(CastedSpell const &)>> UnitEventManager::doCastHandlers;
	std::vector<std::function<void(InterruptibleSpell const &)>> UnitEventManager::processInterruptibleSpellHandlers;
	std::vector<std::function<void(GapCloserSpell const &)>> UnitEventManager::processGapCloserSpellHandlers;
	std::vector<std::function<void(IUnit *, void *)>> UnitEventManager::buffAddHandlers;
	std::vector<std::function<void(IUnit *, void *)>> UnitEventManager::buffRemoveHandlers;
	std::vector<std::function<void(IUnit *, int)>> UnitEventManager::levelUpHandlers;
	std::vector<std::function<void(UnitDash *)>> UnitEventManager::dashHandlers;
	std::vector<std::function<void(IUnit *)>> UnitEventManager::enterVisibilityHandlers;
	std::vector<std::function<void(IUnit *)>> UnitEventManager::exitVisibilityHandlers;
	std::vector<std::function<bool(IUnit *, std::string const)>> UnitEventManager::playAnimationHandlers;


	void UnitEventManager::RegisterCreateEvent(std::function<void(IUnit *)> func)
	{
		UnitEventManager::createHandlers.push_back(func);
	}

	void UnitEventManager::RegisterDestroyEvent(std::function<void(IUnit *)> func)
	{
		UnitEventManager::destroyHandlers.push_back(func);
	}

	void UnitEventManager::RegisterDeathEvent(std::function<void(IUnit *)> func)
	{
		UnitEventManager::deathHandlers.push_back(func);
	}

	void UnitEventManager::RegisterIssueOrderEvent(std::function<bool(IUnit *, int, Vec3 *, IUnit *)> func)
	{
		UnitEventManager::issueOrderHandlers.push_back(func);
	}

	void UnitEventManager::RegisterPreCastEvent(std::function<bool(int, IUnit *, Vec3 *, Vec3 *)> func)
	{
		UnitEventManager::preCastHandlers.push_back(func);
	}

	void UnitEventManager::RegisterUpdateChargedSpellEvent(std::function<void(int, Vec3 *, bool *, bool *)> func)
	{
		UnitEventManager::updateChargedSpellHandlers.push_back(func);
	}

	void UnitEventManager::RegisterProcessSpellCastEvent(std::function<void(CastedSpell const &)> func)
	{
		UnitEventManager::processSpellCastHandlers.push_back(func);
	}

	void UnitEventManager::RegisterDoCastEvent(std::function<void(CastedSpell const &)> func)
	{
		UnitEventManager::doCastHandlers.push_back(func);
	}

	void UnitEventManager::RegisterProcessInterruptibleSpellEvent(std::function<void(InterruptibleSpell const &)> func)
	{
		UnitEventManager::processInterruptibleSpellHandlers.push_back(func);
	}

	void UnitEventManager::RegisterProcessGapCloserSpellEvent(std::function<void(GapCloserSpell const &)> func)
	{
		UnitEventManager::processGapCloserSpellHandlers.push_back(func);
	}

	void UnitEventManager::RegisterBuffAddEvent(std::function<void(IUnit *, void *)> func)
	{
		UnitEventManager::buffAddHandlers.push_back(func);
	}

	void UnitEventManager::RegisterBuffRemoveEvent(std::function<void(IUnit *, void *)> func)
	{
		UnitEventManager::buffRemoveHandlers.push_back(func);
	}

	void UnitEventManager::RegisterLevelUpEvent(std::function<void(IUnit *, int)> func)
	{
		UnitEventManager::levelUpHandlers.push_back(func);
	}

	void UnitEventManager::RegisterDashEvent(std::function<void(UnitDash *)> func)
	{
		UnitEventManager::dashHandlers.push_back(func);
	}

	void UnitEventManager::RegisterEnterVisibilityEvent(std::function<void(IUnit *)> func)
	{
		UnitEventManager::enterVisibilityHandlers.push_back(func);
	}

	void UnitEventManager::RegisterExitVisibilityEvent(std::function<void(IUnit *)> func)
	{
		UnitEventManager::exitVisibilityHandlers.push_back(func);
	}

	void UnitEventManager::RegisterPlayAnimationEvent(std::function<bool(IUnit*, std::string const)> func)
	{
		UnitEventManager::playAnimationHandlers.push_back(func);
	}

	PLUGIN_EVENT(void) OnOrbwalkBeforeAttack(IUnit *target)
	{
		for (auto function : OrbwalkerEventManager::beforeAttackHandlers)
		{
			function(target);
		}
	}

	PLUGIN_EVENT(void) OnOrbwalkAttack(IUnit *source, IUnit *target)
	{
		for (auto function : OrbwalkerEventManager::attackHandlers)
		{
			function(source, target);
		}
	}

	PLUGIN_EVENT(void) OnOrbwalkAfterAttack(IUnit *source, IUnit *target)
	{
		for (auto function : OrbwalkerEventManager::afterAttackHandlers)
		{
			function(source, target);
		}
	}

	PLUGIN_EVENT(void) OnOrbwalkTargetChange(IUnit *oldTarget, IUnit *newTarget)
	{
		for (auto function : OrbwalkerEventManager::newTargetHandlers)
		{
			function(oldTarget, newTarget);
		}
	}

	PLUGIN_EVENT(void) OnOrbwalkOrbwalkNonKillableMinion(IUnit *nonKillableMinion)
	{
		for (auto function : OrbwalkerEventManager::nonKillableMinionHandlers)
		{
			function(nonKillableMinion);
		}
	}

	PLUGIN_EVENT(IUnit *) OnOrbwalkFindTarget()
	{
		IUnit *target = nullptr;

		for (auto function : OrbwalkerEventManager::findTargetHandlers)
		{
			auto ret = function();

			if (ret != nullptr && target == nullptr)
			{
				target = nullptr;
			}
		}

		return target;
	}

	PLUGIN_EVENT(void) OnGameUpdate()
	{
		for (auto function : GameEventManager::updateHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(void) OnGameEnd()
	{
		for (auto function : GameEventManager::endHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(bool) OnGameWndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto ret = true;

		for (auto function : GameEventManager::wndProcHandlers)
		{
			auto r = function(wnd, message, wParam, lParam);

			if (!r && ret)
			{
				ret = false;
			}
		}

		return ret;
	}

	PLUGIN_EVENT(void) OnDrawRender()
	{
		for (auto function : DrawEventManager::renderHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(void) OnDrawRenderBehindHud()
	{
		for (auto function : DrawEventManager::renderBehindHudEventHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(void) OnDrawRenderD3DPresent()
	{
		for (auto function : DrawEventManager::D3DPresentEventHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(void) OnDrawRenderD3DPreReset()
	{
		for (auto function : DrawEventManager::D3DPreResetEventHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(void) OnDrawRenderD3DPostReset()
	{
		for (auto function : DrawEventManager::D3DPostResetEventHandlers)
		{
			function();
		}
	}

	PLUGIN_EVENT(void) OnUnitCreate(IUnit *unit)
	{
		for (auto function : UnitEventManager::createHandlers)
		{
			function(unit);
		}
	}

	PLUGIN_EVENT(void) OnUnitDestroy(IUnit *unit)
	{
		for (auto function : UnitEventManager::destroyHandlers)
		{
			function(unit);
		}
	}

	PLUGIN_EVENT(void) OnUnitDeath(IUnit *unit)
	{
		for (auto function : UnitEventManager::deathHandlers)
		{
			function(unit);
		}
	}

	PLUGIN_EVENT(bool) OnUnitIssueOrder(IUnit *source, int orderId, Vec3 *pos, IUnit *target)
	{
		auto process = true;

		for (auto function : UnitEventManager::issueOrderHandlers)
		{
			auto ret = function(source, orderId, pos, target);

			if (!ret && process)
			{
				process = false;
			}
		}

		return process;
	}

	PLUGIN_EVENT(bool) OnUnitPreCast(int slot, IUnit *target, Vec3 *start, Vec3 *end)
	{
		auto process = true;

		for (auto function : UnitEventManager::preCastHandlers)
		{
			auto ret = function(slot, target, start, end);

			if (!ret && process)
			{
				process = false;
			}
		}

		return process;
	}

	PLUGIN_EVENT(void) OnUnitUpdateChargedSpell(int slot, Vec3 *position, bool *releaseCast, bool *triggerEvent)
	{
		for (auto function : UnitEventManager::updateChargedSpellHandlers)
		{
			function(slot, position, releaseCast, triggerEvent);
		}
	}

	PLUGIN_EVENT(void) OnUnitProcessSpellCast(CastedSpell const &spell)
	{
		for (auto function : UnitEventManager::processSpellCastHandlers)
		{
			function(spell);
		}
	}

	PLUGIN_EVENT(void) OnUnitDoCast(CastedSpell const &spell)
	{
		for (auto function : UnitEventManager::doCastHandlers)
		{
			function(spell);
		}
	}

	PLUGIN_EVENT(void) OnUnitProcessInterruptableSpellCast(InterruptibleSpell const &spell)
	{
		for (auto function : UnitEventManager::processInterruptibleSpellHandlers)
		{
			function(spell);
		}
	}

	PLUGIN_EVENT(void) OnUnitProcessGapCloserSpellCast(GapCloserSpell const &spell)
	{
		for (auto function : UnitEventManager::processGapCloserSpellHandlers)
		{
			function(spell);
		}
	}

	PLUGIN_EVENT(void) OnUnitBuffAdd(IUnit *source, void *buffData)
	{
		for (auto function : UnitEventManager::buffAddHandlers)
		{
			function(source, buffData);
		}
	}

	PLUGIN_EVENT(void) OnUnitBuffRemove(IUnit *source, void *buffData)
	{
		for (auto function : UnitEventManager::buffRemoveHandlers)
		{
			function(source, buffData);
		}
	}

	PLUGIN_EVENT(void) OnUnitLevelUp(IUnit *source, int newLevel)
	{
		for (auto function : UnitEventManager::levelUpHandlers)
		{
			function(source, newLevel);
		}
	}

	PLUGIN_EVENT(void) OnUnitDash(UnitDash *dashArgs)
	{
		for (auto function : UnitEventManager::dashHandlers)
		{
			function(dashArgs);
		}
	}

	PLUGIN_EVENT(void) OnUnitEnterVisibility(IUnit *unit)
	{
		for (auto function : UnitEventManager::enterVisibilityHandlers)
		{
			function(unit);
		}
	}

	PLUGIN_EVENT(void) OnUnitExitVisibility(IUnit *unit)
	{
		for (auto function : UnitEventManager::exitVisibilityHandlers)
		{
			function(unit);
		}
	}

	PLUGIN_EVENT(bool) OnUnitPlayAnimation(IUnit *source, std::string const animationId)
	{
		auto process = true;

		for (auto function : UnitEventManager::playAnimationHandlers)
		{
			auto ret = function(source, animationId);

			if (!ret && process)
			{
				process = false;
			}
		}

		return process;
	}

	void RegisterEvents(IEventManager *eventManager)
	{
		//Orbwalker Events
		eventManager->AddEventHandler(kEventOrbwalkBeforeAttack, OnOrbwalkBeforeAttack);
		eventManager->AddEventHandler(kEventOrbwalkOnAttack, OnOrbwalkAttack);
		eventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
		eventManager->AddEventHandler(kEventOrbwalkTargetChange, OnOrbwalkAfterAttack);
		eventManager->AddEventHandler(kEventOrbwalkNonKillableMinion, OnOrbwalkOrbwalkNonKillableMinion);
		eventManager->AddEventHandler(kEventOrbwalkFindTarget, OnOrbwalkFindTarget);

		//Game Events
		eventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
		eventManager->AddEventHandler(kEventOnGameEnd, OnGameEnd);
		eventManager->AddEventHandler(kEventOnWndProc, OnGameWndProc);

		//Draw Events
		eventManager->AddEventHandler(kEventOnRender, OnDrawRender);
		eventManager->AddEventHandler(kEventOnRenderBehindHud, OnDrawRenderBehindHud);
		eventManager->AddEventHandler(kEventOnD3DPresent, OnDrawRenderD3DPresent);
		eventManager->AddEventHandler(kEventOnD3DPreReset, OnDrawRenderD3DPreReset);
		eventManager->AddEventHandler(kEventOnD3DPostReset, OnDrawRenderD3DPostReset);

		//Unit Events
		eventManager->AddEventHandler(kEventOnCreateObject, OnUnitCreate);
		eventManager->AddEventHandler(kEventOnDestroyObject, OnUnitDestroy);
		eventManager->AddEventHandler(kEventOnUnitDeath, OnUnitDeath);
		eventManager->AddEventHandler(kEventOnIssueOrder, OnUnitIssueOrder);
		eventManager->AddEventHandler(kEventOnPreCast, OnUnitPreCast);
		eventManager->AddEventHandler(kEventOnUpdateChargedSpell, OnUnitUpdateChargedSpell);
		eventManager->AddEventHandler(kEventOnSpellCast, OnUnitProcessSpellCast);
		eventManager->AddEventHandler(kEventOnDoCast, OnUnitDoCast);
		eventManager->AddEventHandler(kEventOnInterruptible, OnUnitProcessInterruptableSpellCast);
		eventManager->AddEventHandler(kEventOnGapCloser, OnUnitProcessGapCloserSpellCast);
		eventManager->AddEventHandler(kEventOnBuffAdd, OnUnitBuffAdd);
		eventManager->AddEventHandler(kEventOnBuffRemove, OnUnitBuffRemove);
		eventManager->AddEventHandler(kEventOnLevelUp, OnUnitLevelUp);
		eventManager->AddEventHandler(kEventOnDash, OnUnitDash);
		eventManager->AddEventHandler(kEventOnEnterVisible, OnUnitEnterVisibility);
		eventManager->AddEventHandler(kEventOnExitVisible, OnUnitExitVisibility);
		eventManager->AddEventHandler(kEventOnPlayAnimation, OnUnitPlayAnimation);
	}

	void UnregisterEvents(IEventManager *eventManager)
	{
		//Orbwalker Events
		eventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, OnOrbwalkBeforeAttack);
		eventManager->RemoveEventHandler(kEventOrbwalkOnAttack, OnOrbwalkAttack);
		eventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
		eventManager->RemoveEventHandler(kEventOrbwalkTargetChange, OnOrbwalkAfterAttack);
		eventManager->RemoveEventHandler(kEventOrbwalkNonKillableMinion, OnOrbwalkOrbwalkNonKillableMinion);
		eventManager->RemoveEventHandler(kEventOrbwalkFindTarget, OnOrbwalkFindTarget);

		//Game Events
		eventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
		eventManager->RemoveEventHandler(kEventOnGameEnd, OnGameEnd);
		eventManager->RemoveEventHandler(kEventOnWndProc, OnGameWndProc);

		//Draw Events
		eventManager->RemoveEventHandler(kEventOnRender, OnDrawRender);
		eventManager->RemoveEventHandler(kEventOnRenderBehindHud, OnDrawRenderBehindHud);
		eventManager->RemoveEventHandler(kEventOnD3DPresent, OnDrawRenderD3DPresent);
		eventManager->RemoveEventHandler(kEventOnD3DPreReset, OnDrawRenderD3DPreReset);
		eventManager->RemoveEventHandler(kEventOnD3DPostReset, OnDrawRenderD3DPostReset);

		//Unit Events
		eventManager->RemoveEventHandler(kEventOnCreateObject, OnUnitCreate);
		eventManager->RemoveEventHandler(kEventOnDestroyObject, OnUnitDestroy);
		eventManager->RemoveEventHandler(kEventOnUnitDeath, OnUnitDeath);
		eventManager->RemoveEventHandler(kEventOnIssueOrder, OnUnitIssueOrder);
		eventManager->RemoveEventHandler(kEventOnPreCast, OnUnitPreCast);
		eventManager->RemoveEventHandler(kEventOnUpdateChargedSpell, OnUnitUpdateChargedSpell);
		eventManager->RemoveEventHandler(kEventOnSpellCast, OnUnitProcessSpellCast);
		eventManager->RemoveEventHandler(kEventOnDoCast, OnUnitDoCast);
		eventManager->RemoveEventHandler(kEventOnInterruptible, OnUnitProcessInterruptableSpellCast);
		eventManager->RemoveEventHandler(kEventOnGapCloser, OnUnitProcessGapCloserSpellCast);
		eventManager->RemoveEventHandler(kEventOnBuffAdd, OnUnitBuffAdd);
		eventManager->RemoveEventHandler(kEventOnBuffRemove, OnUnitBuffRemove);
		eventManager->RemoveEventHandler(kEventOnLevelUp, OnUnitLevelUp);
		eventManager->RemoveEventHandler(kEventOnDash, OnUnitDash);
		eventManager->RemoveEventHandler(kEventOnEnterVisible, OnUnitEnterVisibility);
		eventManager->RemoveEventHandler(kEventOnExitVisible, OnUnitExitVisibility);
		eventManager->RemoveEventHandler(kEventOnPlayAnimation, OnUnitPlayAnimation);
	}
}