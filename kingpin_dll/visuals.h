#pragma once
#include "common.h"
#include "globals.h"
#include "engine.h"
#include "q2_shared.h"

namespace visuals
{
	//Check if this particular point is on your screen
	//So that you can draw stuff at that point
	bool w2s(vec3_t point, float &x, float &y);
	//ESP for NPCs
	//TODO: maybe add boxes? idk, ogl seems gay
	void draw_cast();
	//ESP for items (weapons, ammo etc.)
	void draw_items();
	//little pieces of shit
	void draw_rats();
}
