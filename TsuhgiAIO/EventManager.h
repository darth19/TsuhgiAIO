#pragma once
#include <functional>
#include <vector>
#include "PluginData.h"
#include "PluginSDK.h"

namespace eventmanager {
	class OrbwalkerEventManager
	{
	public:
		static void RegisterBeforeAttackEvent(std::function<void(IUnit *)> func);
		static void RegisterAttackEvent(std::function<void(IUnit *, IUnit *)> func);
		static void RegisterAfterAttackEvent(std::function<void(IUnit *, IUnit *)> func);
		static void RegisterNewTargetEvent(std::function<void(IUnit *, IUnit *)> func);
		static void RegisterNonKillableMinionEvent(std::function<void(IUnit *)> func);
		static void RegisterFindTargetEvent(std::function<IUnit *()> func);

		static std::vector<std::function<void(IUnit *)>> beforeAttackHandlers;
		static std::vector<std::function<void(IUnit *, IUnit *)>> attackHandlers;
		static std::vector<std::function<void(IUnit *, IUnit *)>> afterAttackHandlers;
		static std::vector<std::function<void(IUnit *, IUnit *)>> newTargetHandlers;
		static std::vector<std::function<void(IUnit *)>> nonKillableMinionHandlers;
		static std::vector<std::function<IUnit *()>> findTargetHandlers;

	private:
		OrbwalkerEventManager() {}
	};

	class GameEventManager
	{
	public:
		static void RegisterUpdateEvent(std::function<void()> func);
		static void RegisterEndEvent(std::function<void()> func);
		static void RegisterWndProcEvent(std::function<bool(HWND Wnd, UINT Message, WPARAM wParam, LPARAM lParam)> func);

		static std::vector<std::function<void()>> updateHandlers;
		static std::vector<std::function<void()>> endHandlers;
		static std::vector<std::function<bool(HWND Wnd, UINT Message, WPARAM wParam, LPARAM lParam)>> wndProcHandlers;

	private:
		GameEventManager() {}
	};

	class DrawEventManager
	{
	public:
		static void RegisterRenderEvent(std::function<void()> func);
		static void RegisterRenderBehindHudEvent(std::function<void()> func);
		static void RegisterD3DPresentEvent(std::function<void()> func);
		static void RegisterD3DPreResetEvent(std::function<void()> func);
		static void RegisterD3DPostResetEvent(std::function<void()> func);

		static std::vector<std::function<void()>> renderHandlers;
		static std::vector<std::function<void()>> renderBehindHudEventHandlers;
		static std::vector<std::function<void()>> D3DPresentEventHandlers;
		static std::vector<std::function<void()>> D3DPreResetEventHandlers;
		static std::vector<std::function<void()>> D3DPostResetEventHandlers;

	private:
		DrawEventManager() {}
	};

	class UnitEventManager
	{
	public:
		static void RegisterCreateEvent(std::function<void(IUnit *)> func);
		static void RegisterDestroyEvent(std::function<void(IUnit *)> func);
		static void RegisterDeathEvent(std::function<void(IUnit *)> func);
		static void RegisterIssueOrderEvent(std::function<bool(IUnit *, int, Vec3 *, IUnit *)> func);
		static void RegisterPreCastEvent(std::function<bool(int, IUnit *, Vec3 *, Vec3 *)> func);
		static void RegisterUpdateChargedSpellEvent(std::function<void(int, Vec3 *, bool *, bool *)> func);
		static void RegisterProcessSpellCastEvent(std::function<void(CastedSpell const &)> func);
		static void RegisterDoCastEvent(std::function<void(CastedSpell const &)> func);
		static void RegisterProcessInterruptibleSpellEvent(std::function<void(InterruptibleSpell const &)> func);
		static void RegisterProcessGapCloserSpellEvent(std::function<void(GapCloserSpell const &)> func);
		static void RegisterBuffAddEvent(std::function<void(IUnit *, void *)> func);
		static void RegisterBuffRemoveEvent(std::function<void(IUnit *, void *)> func);
		static void RegisterLevelUpEvent(std::function<void(IUnit *, int)> func);
		static void RegisterDashEvent(std::function<void(UnitDash *)> func);
		static void RegisterEnterVisibilityEvent(std::function<void(IUnit *)> func);
		static void RegisterExitVisibilityEvent(std::function<void(IUnit *)> func);
		static void RegisterPlayAnimationEvent(std::function<bool(IUnit *, std::string const)> func);

		static std::vector<std::function<void(IUnit *)>> createHandlers;
		static std::vector<std::function<void(IUnit *)>> destroyHandlers;
		static std::vector<std::function<void(IUnit *)>> deathHandlers;
		static std::vector<std::function<bool(IUnit *, int, Vec3 *, IUnit *)>> issueOrderHandlers;
		static std::vector<std::function<bool(int, IUnit *, Vec3 *, Vec3 *)>> preCastHandlers;
		static std::vector<std::function<void(int, Vec3 *, bool *, bool *)>> updateChargedSpellHandlers;
		static std::vector<std::function<void(CastedSpell const &)>> processSpellCastHandlers;
		static std::vector<std::function<void(CastedSpell const &)>> doCastHandlers;
		static std::vector<std::function<void(InterruptibleSpell const &)>> processInterruptibleSpellHandlers;
		static std::vector<std::function<void(GapCloserSpell const &)>> processGapCloserSpellHandlers;
		static std::vector<std::function<void(IUnit *, void *)>> buffAddHandlers;
		static std::vector<std::function<void(IUnit *, void *)>> buffRemoveHandlers;
		static std::vector<std::function<void(IUnit *, int)>> levelUpHandlers;
		static std::vector<std::function<void(UnitDash *)>> dashHandlers;
		static std::vector<std::function<void(IUnit *)>> enterVisibilityHandlers;
		static std::vector<std::function<void(IUnit *)>> exitVisibilityHandlers;
		static std::vector<std::function<bool(IUnit *, std::string const)>> playAnimationHandlers;

	private:
		UnitEventManager() {}
	};

	void RegisterEvents(IEventManager *eventManager);
	void UnregisterEvents(IEventManager *eventManager);
}
