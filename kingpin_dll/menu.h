#pragma once
#include "common.h"
#include "engine.h"
#include "vars.h"

//proudly copypasted from Q2Bot
namespace kingpin_menu
{
	class item
	{
	public:
		float *m_pfvalue, m_fmin, m_fmax;

		std::string m_szname;
		std::string m_szdesc;


		item(std::string name, std::string desc, float* var, float min, float max)
		{
			m_szname = name;
			m_szdesc = desc;
			m_pfvalue = var;
			m_fmin = min;
			m_fmax = max;
		}

		bool get_bool()
		{
			return *m_pfvalue != 0.f;
		}

		float get_float()
		{
			return *m_pfvalue;
		}

		const char* get_desc()
		{
			return m_szdesc.c_str();
		}

		const char* get_name()
		{
			return m_szname.c_str();
		}

		int get_int()
		{
			return *reinterpret_cast<int*>(m_pfvalue);
		}

	};

	class menu
	{
	public:

		menu()
		{
			x = y = w = h = 0;
			bgcolor = spacing = 0;
			selecteditem = 1;
			is_menu_visible = false;
		}

		std::vector<item> items;
		std::string menu_title;
		int x, y, w, h, bgcolor, spacing, selecteditem;
		bool is_menu_visible;

		void init();
		void increase();
		void decrease();

		void up();
		void down();

		void draw();

		void add_item(std::string name, std::string desc, float* var, float min, float max);
	};

	extern menu g_menu;
}

