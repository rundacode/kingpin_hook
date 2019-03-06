#include "menu.h"
#include "globals.h"

namespace kingpin_menu
{
	void menu::add_item(std::string name, std::string desc, float* val, float min = 0, float max = 1)
	{
		item element = item(name, desc, val, min, max);

		this->items.push_back(element);
	}

	void menu::increase()
	{
		item element = items[selecteditem];

		if (element.get_float() < element.m_fmax)
		{
			*element.m_pfvalue += 1.f;
			util::playsound("world/switches/wall switch.wav");
		}		
	}

	void menu::decrease()
	{
		item element = items[selecteditem];

		if (element.get_float() > element.m_fmin)
		{
			*element.m_pfvalue -= 1.f;
			util::playsound("world/switches/wall switch.wav");
		}
	}

	void menu::up()
	{
		if (this->selecteditem > 0)
			this->selecteditem--;
		else
			this->selecteditem = items.size() - 1;

		util::playsound("misc/menu1.wav");
	}

	void menu::down()
	{
		if (this->selecteditem < items.size() - 1)
			this->selecteditem++;
		else
			this->selecteditem = 0;

		util::playsound("misc/menu1.wav");
	}

	void menu::init()
	{
		x = 20;
		y = 250;
		w = 220;
		bgcolor = 70;
		spacing = 12;

		menu_title = "kingpin hook";

		add_item("aimbot", "locks onto hostile enemies", &vars.aimbot);
		add_item("silent aim", "invisible aiming at enemies", &vars.silent);
		add_item("autoshoot", "starts shooting if there's a valid enemy", &vars.autoshoot);
		add_item("norecoil", "removes gun recoil", &vars.norecoil);
		add_item("nospread", "forces bullet weapons to have no spread", &vars.nospread);
		add_item("bunnyhop", "forces player to jump and gain momentum", &vars.bhop);
		add_item("wallhack", "renders entities through walls", &vars.wallhack);
		add_item("chams", "npcs are drawn in solid bright color", &vars.chams);
		add_item("chams color", "0 - red, 1 - green, 2 - blue", &vars.chams_clr, 0.f, 2.f);
		add_item("godmode", "infinte health and armor", &vars.godmode);
		add_item("infinite money", "buy all the shit", &vars.money);
		add_item("high damage", "rips enemies a new one in one shot", &vars.highdamage);
		add_item("bfg projectiles", "pistol and tommygun will shoot bfg charges", &vars.bfg_charges);
		add_item("shotgun pellet amount", "self-explainatory", &vars.shotgun_pellet_amt, 1.f, 20.f);
		add_item("infinite ammo", "self-explainatory", &vars.inf_ammo);
		add_item("unlimited mods", "weapon mods never break", &vars.inf_mods);
		add_item("rapid fire", "weapons always ready to shoot", &vars.rapidfire);
		add_item("randomize skins", "spawned npcs will look different instead of using default skins", &vars.rand_skin);

		add_item("debug info", "display debug information", &vars.debug);

		h = items.size() * spacing;

		printf("[kingpin_hook] menu initialized with %d item(s)\n", items.size());
	}

	void menu::draw()
	{
		util::draw_string(x, y - 14, bgcolor, true, "[kingpin_hook]");
		globalvars::ref_api->DrawFill(x - 1, y - 1, w + 2, h + 2, 251);
		globalvars::ref_api->DrawFill(x, y, w, h, bgcolor);

		for (size_t i = 0; i < items.size(); i++)
		{
			item element = items[i];
			int y_offset = i * spacing;

			if (i == selecteditem)
				globalvars::ref_api->DrawFill(x, y + y_offset, w, 8, 111);

			util::draw_string(x, y + y_offset, 0, i == selecteditem, "%s", element.get_name());
			util::draw_string(x + 200, y + y_offset, 0, i == selecteditem, "%.0f", element.get_float());
		}

		item element = items[selecteditem];

		util::draw_string(x, y + h + spacing, 39, true, "%s", element.get_desc());
	}

	menu g_menu;
}