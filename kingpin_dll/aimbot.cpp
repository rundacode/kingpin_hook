#include "aimbot.h"
#include "vars.h"

std::vector <std::string> valid_weapons = { 
	"weapon_pistol",
	"weapon_tommygun",
	"weapon_heavymachinegun",
	"weapon_shotgun"
};

//game bug: rapid kills cause overflows

namespace aimbot
{
	__declspec (noinline) void aim()
	{
		if (*globalvars::connstate != connstate_t::ca_active)
			return;

		//we still get target even if we don't have proper weapon equipped
		auto target = get_target();
		if (!target || !globalvars::local_player)
			return;

		//would be suitable for ESP or some shit
		//laser colors can be changed in hooked_ParseLaser
		util::draw_line(globalvars::local_player->s.origin, target->s.origin, svc_temp_entity, TE_BFG_LASER);

		//can aim, but we can't shoot with bare hands!
		if (!globalvars::local_player->client->pers.weapon)
			return;
		
		//check if we have our inventory thing init'd
		if (globalvars::local_player->client)
		{
			for (size_t i = 0; i < valid_weapons.size(); i++)
			{
				//check if we have a valid gun to aimbot with, if yes, break out and do the aiming stuff
				if (strstr(globalvars::local_player->client->pers.weapon->classname, valid_weapons[i].c_str()))
					break;

				//our weapon didn't match valid ones, abort
				if (i == valid_weapons.size() - 1)
					return;
			}
		}

		vector temp = target->s.origin;
		if (strstr(target->classname, "cast_dog"))
		{
			//sort of fix for dogs
			temp.z -= (target->viewheight + 12.f);
		}

		vector direction =  temp - globalvars::local_player->s.origin;
		direction.normalize();

		float pitch = -(float)atan2(direction.z, direction.length2d());
		float yaw = (float)atan2(direction.y, direction.x);

		static vector* viewangles = (vector*)0x01353274;
		float delta_pitch = SHORT2ANGLE(*(short*)0x0134FBFC);
		float delta_yaw = SHORT2ANGLE(*(short*)0x0134FBFE);

		viewangles->x = (pitch) * (180 / M_PI) - delta_pitch;
		viewangles->y = yaw * (180 / M_PI) - delta_yaw;

		if (vars.autoshoot)
		{
			//(c) ghetto codez industries
			/*using key = void(__cdecl*)();

			static key kdown = (key)0x40B7E0;
			static key kup = (key)0x40B7F0;

			kdown();
			kup(); */

			__asm
			{
				mov eax, 0x40b7e0
				call eax
				mov eax, 0x40b7f0
				call eax
			}
		}
	}

	__declspec (noinline) edict_t* get_target()
	{
		edict_t* temp_target = nullptr;
		float distance = 0.f;

		for (int i = 1; i <= globalvars::cast_amount; i++)
		{
			edict_t* ent = util::get_cast(i);
			if (!ent || !ent->classname || ent->health <= 0 || ent->movetype != MOVETYPE_STEP)
				continue;

			if (strstr(ent->classname, "cast_") && !strstr(ent->classname, "cast_bum_sit")) //bums dindu nuffin
			{				
				if (globalvars::local_player)
				{
					//fails to check thru some transparent AND shootable surfaces
					if (!util::is_visible(globalvars::local_player, ent)) 
						continue;
				}

				float player_distance = globalvars::local_player->s.origin.distance(ent->s.origin);
				if (distance == 0.f || player_distance < distance)
				{
					distance = player_distance;
					temp_target = ent;
				}
			}
		}

		globalvars::target = temp_target;
		return temp_target;
	}
}