#pragma once
#include "PluginSDK.h"

class TsuhgiAIO
{
public:
	explicit TsuhgiAIO(IPluginSDK *sdk);
	~TsuhgiAIO();
private:
//	void ashe();
//	void ezreal();
//	void graves();
//	void jhin();
//	void kalista();
//	void kogmaw();
//	void leesin();
	void lucian();
//	void sivir();
//	void tristana();
//	void twitch();


	IUnit *findTarget(float range, eDamageType damage) const;

	IPluginSDK *sdk;
	IMenu *menu;
};

