#include "PluginSDK.h"
#include "EventManager.h"
#include "TsuhgiAIO.h"

IEventManager *eventManager = nullptr;
TsuhgiAIO *implementation = nullptr;


PLUGIN_API void OnLoad(IPluginSDK *pluginSDK)
{
	eventManager = pluginSDK->GetEventManager();
	eventmanager::RegisterEvents(eventManager);

	implementation = new TsuhgiAIO(pluginSDK);
}

PLUGIN_API void OnUnload()
{
	eventmanager::UnregisterEvents(eventManager);

	delete implementation;
}
