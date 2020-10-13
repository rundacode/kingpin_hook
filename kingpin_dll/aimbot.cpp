#include "aimbot.h"
#include "vars.h"

const std::vector <std::string> valid_weapons = { 
	"weapon_spistol",
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
		//I've commented this out becase this can cause overflows
		//since bfg laser is a temp entity which writes into a datagram, etc etc w/e
		//util::draw_line(globalvars::local_player->s.origin, target->s.origin, svc_temp_entity, TE_BFG_LASER);

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

		//Might wanna adjust height for crouching enemies
		vector temp = target->s.origin;
		if (strstr(target->classname, "cast_dog"))
		{
			//sort of fix for dogs
			temp.z -= (target->viewheight + 12.f);
		}
		else
		{
			if (target->maxs[2] < target->cast_info.standing_max_z)
				temp.z = 24.f;
		}

		vector direction =  temp - globalvars::local_player->s.origin;
		direction.normalize();

		float pitch = -(float)atan2(direction.z, direction.length2d());
		float yaw = (float)atan2(direction.y, direction.x);

		static vector* viewangles = (vector*)0x01353274;
		float delta_pitch = SHORT2ANGLE(*(short*)0x0134FBFC);
		float delta_yaw = SHORT2ANGLE(*(short*)0x0134FBFE);

		if (vars.silent)
		{
			globalvars::v_aim_angles[0] = pitch * (180 / M_PI);
			globalvars::v_aim_angles[1] = yaw * (180 / M_PI); //preserve rotation
		}
		else
		{
			viewangles->x = pitch * (180 / M_PI) - delta_pitch;
			viewangles->y = yaw * (180 / M_PI) - delta_yaw;
		}

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

		for (int i = 0; i < MAX_CHARACTERS; i++)
		{
			edict_t* ent = globalvars::locals->characters[i];
			if (!ent || !ent->classname || ent->health <= 0 || ent->movetype != MOVETYPE_STEP)
				continue;

			if (strstr(ent->classname, "cast_") && !strstr(ent->classname, "cast_bum_sit")) //bums dindu nuffin
			{				
				//Blunt has enormous amount of health (~150k) and is not godmodded
				//But I'll put this check anyway
				if (ent->flags & FL_GODMODE)
					continue;

				if (globalvars::local_player)
				{
					if(!util::_trace(ent, true))
						continue;

					//This guy here isn't really good :-)
					//But it's "CanDamageThroughAlpha" method is!
					/*if (!util::candamage(ent, globalvars::local_player))
						continue;*/
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

	//todo: find out why this shit traces god knows where
	//do we really need this?
	void triggerbot()
	{
		if (!globalvars::refdef)
			return;

		vec3_t start, forward, right, end;
		trace_t trace;
		
		util::angle_vectors(globalvars::local_player->client->v_angle, forward, right, 0);

		trace = globalvars::game_import->trace(globalvars::refdef->vieworg, vec3_origin, vec3_origin, forward, globalvars::local_player, MASK_SHOT);
		if (trace.ent && trace.fraction >= 0.97f)
		{
			if (strstr(trace.ent->classname, "cast_"))
			{
				printf("[kingpin_hook] triggerbot caught entity\n");
				__asm
				{
					mov eax, 0x40b7e0
					call eax
					mov eax, 0x40b7f0
					call eax
				}
			}
		}
	}
}