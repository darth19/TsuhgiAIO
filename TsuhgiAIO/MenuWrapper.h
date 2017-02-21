#pragma once
#include "PluginSDK.h"
#include <unordered_map>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include "lpputils.h"
#include "EventManager.h"

class Menu;

class KeyBind
{
public:
	enum struct KeyBindType
	{
		Press,
		Toggle
	};

	explicit KeyBind(char key, KeyBind::KeyBindType type, bool defaultValue = false) : key(key), type(type), defaultValue(defaultValue)
	{
	}

	char getKey() const
	{
		return key;
	}

	KeyBind::KeyBindType getType() const
	{
		return type;
	}

	bool getDefault() const
	{
		return this->defaultValue;
	}

private:
	char key;
	KeyBind::KeyBindType type;
	bool defaultValue;
};

template<typename T>
class Slider
{
	static_assert((std::is_base_of<int, T>::value) || (std::is_base_of<float, T>::value), "T must be int or float");

public:
	explicit Slider(T minValue, T maxValue, T defaultValue)
		: minValue(minValue), maxValue(maxValue), defaultValue(defaultValue)
	{
	}

	T getMin() const
	{
		return minValue;
	}

	T getMax() const
	{
		return maxValue;
	}

	T getDefault() const
	{
		return defaultValue;
	}
private:
	T minValue;
	T maxValue;
	T defaultValue;
};

class MenuElement
{
public:
	enum struct MenuElementType : int
	{
		Toggle = 0,
		Integer,
		Float,
		KeyBind,
		Color,
		Menu
	};

	explicit MenuElement(std::string name, boost::variant<bool, Slider<int>, Slider<float>, KeyBind, Vec4, Menu> value)
		: name(name), type(static_cast<MenuElement::MenuElementType>(value.which())), value(value)
	{
		auto test = eventmanager::GameEventManager::RegisterUpdateEvent([&]() -> void {
			switch (type)
			{
			case MenuElement::MenuElementType::Toggle:
				int op = this->option;
			case MenuElement::MenuElementType::Integer:
				auto intSlider = boost::get<Slider<int>>(this->value);
				this->option = parent->AddInteger(this->name.c_str(), intSlider.getMin(), intSlider.getMax(), intSlider.getDefault());
			case MenuElement::MenuElementType::Float:
				auto floatSlider = boost::get<Slider<float>>(this->value);
				this->option = parent->AddFloat(this->name.c_str(), floatSlider.getMin(), floatSlider.getMax(), floatSlider.getDefault());
			case MenuElement::MenuElementType::KeyBind:
				auto keyBind = boost::get<KeyBind>(this->value);
				auto KeyBindType = keyBind.getType();

				if (KeyBindType == KeyBind::KeyBindType::Press)
				{
					this->option = parent->AddKey(this->name.c_str(), keyBind.getKey());
				}
				else if (KeyBindType == KeyBind::KeyBindType::Toggle)
				{
					this->option = LPPUtils::AddKeyBindToggle(parent, this->name, keyBind.getKey(), keyBind.getDefault());
				}
			case MenuElement::MenuElementType::Color:
				auto color = boost::get<Vec4>(this->value);
				this->option = parent->AddColor(this->name.c_str(), color.x, color.y, color.z, color.w);
			case MenuElement::MenuElementType::Menu:
				this->option = parent->AddMenu(this->name.c_str());
			}
		});
	}

	~MenuElement()
	{
		
	};

	boost::variant<IMenuOption *, IMenu *> addOption(IMenu *parent)
	{
		switch (type)
		{
		case MenuElement::MenuElementType::Toggle:
			this->option = parent->CheckBox(this->name.c_str(), boost::get<bool>(this->value));
		case MenuElement::MenuElementType::Integer:
			auto intSlider = boost::get<Slider<int>>(this->value);
			this->option = parent->AddInteger(this->name.c_str(), intSlider.getMin(), intSlider.getMax(), intSlider.getDefault());
		case MenuElement::MenuElementType::Float:
			auto floatSlider = boost::get<Slider<float>>(this->value);
			this->option = parent->AddFloat(this->name.c_str(), floatSlider.getMin(), floatSlider.getMax(), floatSlider.getDefault());
		case MenuElement::MenuElementType::KeyBind:
			auto keyBind = boost::get<KeyBind>(this->value);
			auto KeyBindType = keyBind.getType();

			if (KeyBindType == KeyBind::KeyBindType::Press)
			{
				this->option = parent->AddKey(this->name.c_str(), keyBind.getKey());
			}
			else if (KeyBindType == KeyBind::KeyBindType::Toggle)
			{
				this->option = LPPUtils::AddKeyBindToggle(parent, this->name, keyBind.getKey(), keyBind.getDefault());
			}
		case MenuElement::MenuElementType::Color:
			auto color = boost::get<Vec4>(this->value);
			this->option = parent->AddColor(this->name.c_str(), color.x, color.y, color.z, color.w);
		case MenuElement::MenuElementType::Menu:
			this->option = parent->AddMenu(this->name.c_str());
		}

		return this->option;
	}

	boost::variant<bool, Slider<int>, Slider<float>, KeyBind, Vec4, Menu> getValue()
	{
		

		return nullptr;
	}
private:
	std::string name;
	MenuElement::MenuElementType type;
	boost::variant<bool, Slider<int>, Slider<float>, KeyBind, Vec4, Menu> value;

	boost::variant<IMenuOption *, IMenu *> option = nullptr;
};

class Menu
{
public:
	explicit Menu(IPluginSDK *sdk, std::string name) : name(name)
	{
		this->element = sdk->AddMenu(name.c_str());
	}

	void addMenuElement(MenuElement )
	{
		
	}

	boost::optional<MenuElement> &operator[](std::string string)
	{
		for (auto element : this->menuElements)
		{
			if (element.uniqueName() == string)
			{
				auto op = boost::optional<MenuElement>(element);
				return op;
			}
		}

		auto op = boost::optional<MenuElement>();
		return op;
	}
private:
	IMenu *element;
	std::string name;
	//Detuks: use radix tries
	std::vector<MenuElement> menuElements;
};