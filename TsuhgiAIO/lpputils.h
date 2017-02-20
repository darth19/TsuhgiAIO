#pragma once
#include "PluginSDK.h"
#include <boost/algorithm/string.hpp>

#define M_PI 3.14159265358979323846

class LPPUtils
{
public:
	static void CreateConsoleWindow()
	{
		AllocConsole();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		SetConsoleTitleA("Debug Console");
	}

	static void LogConsole(char* Fmt, ...)
	{
		DWORD dwBytes = 0;
		char szBuffer[1024] = { 0 };

		va_list va;
		va_start(va, Fmt);
		vsnprintf_s(szBuffer, sizeof(szBuffer), Fmt, va);
		va_end(va);

		strcat_s(szBuffer, "\n");

		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), szBuffer, strlen(szBuffer), &dwBytes, nullptr);
	}

	static int CountEnemiesNearMe(IPluginSDK* sdk, float range)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllHeros(false, true))
		{
			if (player->IsValidTarget(unit, range))
			{
				count++;
			}
		}

		return count;
	}

	static int CountEnemiesInAttackRange(IPluginSDK* sdk)
	{
		static auto entityList = sdk->GetEntityList();
		static auto player = entityList->Player();

		auto count = 0;

		for (auto unit : entityList->GetAllHeros(false, true))
		{
			if (player->IsValidTarget(unit, player->GetRealAutoAttackRange(unit)))
			{
				count++;
			}
		}

		return count;
	}

	static bool InFountain(IUnit *unit)
	{
		//TODO: Implement
		return unit->HasBuff("kappachino");
	}

	static bool IsTeleporting(IUnit *unit, IPluginSDK *sdk)
	{
		static auto buffReader = sdk->GetBuffDataReader();

		std::vector<void *> dataVector;
		unit->GetAllBuffsData(dataVector);

		for (auto data: dataVector)
		{
			auto name = std::string(buffReader->GetBuffName(data));
			boost::algorithm::to_lower(name);

			if (name.find("recall") != std::string::npos || name.find("teleport") != std::string::npos)
			{
				return true;
			}
		}

		return false;
	}

	static Vec2 To2D(Vec3 vec)
	{
		return Vec2(vec.x, vec.z);
	}

	static Vec2 V2E(Vec3 from, Vec3 direction, float distance)
	{
		return LPPUtils::To2D(from) + LPPUtils::To2D(distance * (direction - from).VectorNormalize());
	}

	static Vec3 Extend(const Vec3 &from, const Vec3 &to, float distance)
	{
		return from + (distance * (to - from).VectorNormalize());
	}

	static Vec3 To3D(IPluginSDK *sdk, Vec2 v)
	{
		static auto player = sdk->GetEntityList()->Player();

		return Vec3(v.x, player->GetPosition().y, v.y);
	}

	static void DrawHpBarDamage(IRender *renderer, IUnit *hero, float damage, Vec4 &color)
	{
		//Credit to xSalice
		static auto font = renderer->CreateFontW("monospace", 12.f, kFontWeightThin);

		const auto xOffset = 10;
		const auto yOffset = 20;
		const auto width = 103;
		const auto height = 8;

		Vec2 barPos;
		if (hero->GetHPBarPosition(barPos))
		{
			auto hp = hero->GetHealth();
			auto maxHp = hero->GetMaxHealth();
			auto percentHealthAfterDamage = max(0, hp - damage) / maxHp;
			auto yPos = barPos.y + yOffset;
			auto xPosDamage = barPos.x + xOffset + (width * percentHealthAfterDamage);
			auto xPosCurrentHp = barPos.y + xOffset + ((width * hp) / maxHp);

			if (damage > hp)
			{
				font->Render((barPos.x + xOffset), (barPos.y + yOffset - 13.f), "Killable: %d", (hp - damage));
			}

			renderer->DrawLine(Vec2(xPosDamage, yPos), Vec2(xPosDamage, yPos + height), color);

			auto differenceInHp = xPosCurrentHp - xPosDamage;
			auto pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

			for (auto i = 0; i < differenceInHp; i++)
			{
				renderer->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + height), color);
			}
		}
	}

	static void DrawLFCircle(IRender *renderer, IGame *game, Vec3 &position, Vec4 &color, float radius, int quality = 30)
	{
		//Creits to L#.Common

		auto points = std::vector<Vec3>();
		for (auto i = 0; i < quality; i++)
		{
			auto angle = i * M_PI * 2 / quality;
			points.push_back(
				Vec3(
					position.x + static_cast<float>(radius * cos(angle)),
					position.y,
					position.z + static_cast<float>(radius * sin(angle))
				)
			);
		}

		for (size_t i = 0; i < points.size(); i++)
		{
			auto a = points[i];
			auto b = points[i == points.size() - 1 ? 0 : i + 1];

			Vec2 aonScreen, bonScreen;
			game->Projection(a, &aonScreen);
			game->Projection(b, &bonScreen);

			renderer->DrawLine(aonScreen, bonScreen, color);
		}
	}
	static void DrawCircleMinimap(IRender *renderer, IGame *game, Vec3 &position, Vec4 &color, float radius, int quality = 30)
	{
		//Creits to L#.Common
		auto points = std::vector<Vec3>();
		for (auto i = 0; i < quality; i++)
		{
			auto angle = i * M_PI * 2 / quality;
			points.push_back(
				Vec3(
					position.x + static_cast<float>(radius * cos(angle)),
					position.y,
					position.z + static_cast<float>(radius * sin(angle))
				)
			);
		}

		for (size_t i = 0; i < points.size(); i++)
		{
			auto a = points[i];
			auto b = points[i == points.size() - 1 ? 0 : i + 1];

			Vec2 aonScreen, bonScreen;
			game->WorldToMinimap(a, aonScreen);
			game->WorldToMinimap(b, bonScreen);

			renderer->DrawLine(aonScreen, bonScreen, color);
		}
	}

	static bool IsKeyDown(IMenuOption *menuOption)
	{
		return GetAsyncKeyState(menuOption->GetInteger()) & 0x8000;
	}
};
