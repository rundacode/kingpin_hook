#include "kingpin_hooked.h"
#include "mem_manager.h"
#include "aimbot.h"
#include "visuals.h"
#include "menu.h"
#include "vars.h"
#include "globals.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma warning (disable : 4477)

//actually works lmao
gitem_s* itemlist = (gitem_s*)0x200F35E8;
#define	ITEM_INDEX(x) ((x)-itemlist)

double m_Projection[16];
int	m_Viewport[4];
matrix4x4_s &m_ModelView = *reinterpret_cast<matrix4x4_s*>(MODEL_VIEW);

namespace original
{
	//Game

	firebullet_t o_firebullet;
	fireshotgun_t o_fireshotgun;
	firelead_t o_firelead;
	basemove_t o_basemove;
	runframe_t o_runframe;
	calcviewangles_t o_calcviewangles;
	finishmove_t o_finishmove;
	findindex_t o_findindex;
	clientthink_t o_clientthink;
	parselaser_t o_parselaser;
	sendcmd_t o_sendcmd;
	voicespecific_t o_voicespecific;
	//Renderer
	
	glimp_endframe_t o_endframe;
	renderframe_t o_renderframe;
	cl_deltaentity_t o_deltaentity;
}

namespace cheat
{
	void SetupGameLibHooks()
	{
		original::o_firebullet = (firebullet_t)DetourFunction((PBYTE)0x20088F57, (PBYTE)cheat::hooked_FireBullet);
		original::o_fireshotgun = (fireshotgun_t)DetourFunction((PBYTE)0x2008A44D, (PBYTE)cheat::hooked_FireShotgun);

		original::o_clientthink = (clientthink_t)DetourFunction((PBYTE)0x20096EFA, (PBYTE)cheat::hooked_ClientThink);
		original::o_runframe = (runframe_t)DetourFunction((PBYTE)0x2006E499, (PBYTE)cheat::hooked_RunFrame);

		original::o_voicespecific = (voicespecific_t)DetourFunction((PBYTE)0x200AD50E, (PBYTE)cheat::hooked_VoiceSpecific);
	}

	void hooked_FireBullet(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod)
	{
		//checking for classname isn't really needed, since player always has index 1
		if (self->s.number == 1 && std::strstr(self->classname, "player"))
		{
			if (vars.nospread)
				hspread = vspread = 0;

			if (vars.highdamage)
				damage = 1337;

			//kick = INT_MAX;

			//printf("[kingpin_hook] bullet: spread %d, %d / mod %d / damage %d\n", hspread, vspread, mod, damage);

			if (vars.silent && globalvars::target)
			{
				//how this works:
				//we get aim angles from aimbot, but do not substract delta angles
				//so that anglevectors rotates stuff properly, otherwise we'll shoot
				//somewhere else
				
				util::angle_vectors(globalvars::v_aim_angles, aimdir, 0, 0);
			}

			//uff ja $
			if (static_cast<int>(vars.bfg_charges) >= 1)
			{
				switch (static_cast<int>(vars.bfg_charges))
				{
				case 1:
					util::fire_bfg(self, start, aimdir, 13337, 250, 5000);
					break;
				case 2:
					util::fire_rail(self, start, aimdir, 1000, INT_MAX);
					break;
				case 3:
					util::fire_blaster(self, start, aimdir, 1000, 3500, EF_HYPERBLASTER, 1);
				}
				
				return;
			}
		}

		original::o_firebullet(self, start, aimdir, damage, kick, hspread, vspread, mod);

		if (self->s.number == 1 && vars.norecoil)
		{
			self->client->kick_origin[0] = 0.f;
			self->client->kick_origin[1] = 0.f;
			self->client->kick_origin[2] = 0.f;

			self->client->kick_angles[0] = 0.f;
			self->client->kick_angles[1] = 0.f;
			self->client->kick_angles[2] = 0.f;
		}
	}

	void hooked_SendCmd()
	{

		original::o_sendcmd();
	}

	void hooked_FireShotgun(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod)
	{
		if (self->s.number == 1 && std::strstr(self->classname, "player"))
		{
			//still, it is a shotgun after all :)
			if(vars.nospread)
				hspread = vspread = 50;

			if(vars.highdamage)
				damage = 1337;

			//custom amount of pellets
			count = (int)vars.shotgun_pellet_amt;
			//for lulz
			kick = INT_MAX;
			//printf("[kingpin_hook] shotgun: spread %d, %d / mod %d / damage %d\n", hspread, vspread, mod, damage * count);

			if (vars.silent && globalvars::target)
			{
				util::angle_vectors(globalvars::v_aim_angles, aimdir, 0, 0);
			}
		}
		original::o_fireshotgun(self, start, aimdir, damage, kick, hspread, vspread, count, mod);

		if (self->s.number == 1 && vars.norecoil)
		{
			self->client->kick_origin[0] = 0.f;
			self->client->kick_origin[1] = 0.f;
			self->client->kick_origin[2] = 0.f;

			self->client->kick_angles[0] = 0.f;
			self->client->kick_angles[1] = 0.f;
			self->client->kick_angles[2] = 0.f;
		}
	}

	void hooked_FireLead(edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int te_impact, int hspread, int vspread, int mod)
	{
		printf("[kingpin_hook] log: hspread %d, vspread %d\n", hspread, vspread);

		original::o_firelead(self, start, aimdir, damage, kick, te_impact, hspread, vspread, mod);
	}

	void hooked_BaseMove(usercmd_t* cmd)
	{
		 original::o_basemove(cmd);
	}

	void hooked_FinishMove(usercmd_t* cmd)
	{
		if (vars.bhop && globalvars::local_player)
		{
			if (!(globalvars::local_player->client->ps.pmove.pm_flags & PMF_ON_GROUND) && globalvars::local_player->client->ps.pmove.pm_flags & PMF_JUMP_HELD)
				globalvars::local_player->client->ps.pmove.pm_flags &= ~PMF_JUMP_HELD;
		}

		//Instead of having SendCommand memed, you can modify this as well
		//bigger msec = faster movement

			//I've decided to go with this instead of SendCommand hook, since I "lag out" for some unknown reason
			//plus this way it's much smoother I think

		original::o_finishmove(cmd);

		if (GetAsyncKeyState(VK_XBUTTON1))
			cmd->msec = static_cast<int>(vars.cmd_msec);
	}

	int hooked_FindIndex(const char* name, int start, int max, bool create)
	{

		return original::o_findindex(name, start, max, create);
	}

	/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

	//blindly sets a nice flag to every entity that goes thru this func
	//using classnames will be a good filter
	void hooked_DeltaEntity(frame_t* frame, int newnum, entity_state_t* old, int bits)
	{
		static bool did_print = false;
		if (!did_print)
		{
			printf("[kingpin_hook] frame: 0x%X\n", frame);
			did_print = true;
		}

		if (GetModuleHandle("gamex86.dll"))
		{
			if (globalvars::game_api)
			{
				auto ent = globalvars::game_api->GetEntity(old->number);
				if (globalvars::local_player && ent)
				{
					if (ent == globalvars::local_player)
					{
						old->effects |= EF_COLOR_SHELL;
						old->renderfx |= RF_SHELL_RED;
					}

					if (vars.chams)
					{
						if (ent->classname && strstr(ent->classname, "cast_")) 
						{
							old->renderfx = 0;
							switch (static_cast<int>(vars.chams_clr))
							{
							case 0:
								old->renderfx |= RF_SHELL_RED;
							case 1:
								old->renderfx |= RF_SHELL_GREEN;
							case 2:
								old->renderfx |= RF_SHELL_BLUE;
							}
						}
					}
					else
					{
						if (ent && ent->classname && strstr(ent->classname, "cast_"))
						{
							old->renderfx &= ~RF_SHELL_RED;
							old->renderfx &= ~RF_SHELL_GREEN;
							old->renderfx &= ~RF_SHELL_BLUE;
						}
					}

					if (vars.wallhack)
					{
						old->renderfx |= RF_DEPTHHACK;

						if (ent->classname && strstr(ent->classname, "item_")) //make items highlighted, sort of.
							old->effects |= (1 << 4); //if you wonder why crates suddenly "explode" or vent bits have fires on them, this is why
					}
					else
					{
						old->renderfx &= ~RF_DEPTHHACK;
						if (ent->classname && strstr(ent->classname, "item_"))
							old->effects &= ~(1 << 4);
					}
				}
			}
		}


		//effect_num can be changed here as well
		//if set, different kind of effect will be attached
		//to the entity (effectnum are flags!)

		original::o_deltaentity(frame, newnum, old, bits);
	}

	//Sometimes somehow I end up crashing in Voice_Specific due to self being null (can happen on "Lizzie's Problem")
	//Since original doesn't handle that, well, here I am
	void hooked_VoiceSpecific(edict_t* self, edict_t* other, void* voice_table, int entry)
	{
		if (!self || !other || !voice_table)
		{
			//Notify ourselves about this nigger shit happening, just so we know this didn't go in vain
			printf("[ kingpin_hook ] game tried calling \"Voice_Specific\" with an edict being nullptr\n");
			return;
		}

		original::o_voicespecific(self, other, voice_table, entry);
	}

	void hooked_EndFrame()
	{

		original::o_endframe();
	}

	void hooked_RunFrame()
	{
		//Advances the game by 0.1. Just that.

		globalvars::local_player = util::get_cast(0);

		if (globalvars::local_player)
		{
			//supposed to make you wear cigar and a hat
			globalvars::local_player->count |= 1 | 2;

			if (vars.money)
			{
				//so that we still could collect dosh :-)
				if (globalvars::local_player->client->pers.currentcash < 5000)
					globalvars::local_player->client->pers.currentcash = 5000;
			}

			if (vars.rapidfire)
			{
				globalvars::local_player->client->ps.gunframe = 0;
				globalvars::local_player->client->weaponstate = weaponstate_t::WEAPON_READY;
			}

			if (vars.godmode)
			{
				//setting flags? too easy
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Jacket Armor Heavy"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Legs Armor Heavy"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Helmet Armor Heavy"))] = 999;

				globalvars::local_player->health = 999;
				globalvars::local_player->max_health = 999;
			}

			if (vars.inf_ammo)
			{
				//fixed RL and flamer's ammo
				globalvars::local_player->client->pers.weapon_clip[globalvars::local_player->client->clip_index] = 999;
				globalvars::local_player->client->pers.inventory[globalvars::local_player->client->ammo_index] = 999;

				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("308cal"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Rockets"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Gas"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Shells"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Bullets"))] = 999;
				globalvars::local_player->client->pers.inventory[ITEM_INDEX(util::find_item("Grenades"))] = 999;
			}

			//weapon mods do not wear down
			if (static_cast<int>(vars.inf_mods))
			{
				globalvars::local_player->client->pers.hmg_shots = 99;
				globalvars::local_player->client->pers.silencer_shots = 99;
			}

			//enemies will flee away from player, func used by grenades
			if (vars.spook)
				util::avoid_ent(globalvars::local_player);
		}

		original::o_runframe();
	}

	void hooked_ClientThink(edict_t* client, usercmd_t* cmd)
	{
		static bool did_print = false;
		globalvars::cast_amount = globalvars::locals->num_characters;

		if (!did_print)
		{
			printf("client at 0x%p\n", client);
			did_print = true;
		}

		//aimbot::triggerbot();

		if (vars.aimbot)
			aimbot::aim();
		else
		{
			if (globalvars::target)
				globalvars::target = nullptr;
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
			vars.draw_menu = !vars.draw_menu;

		//WndProc hook would be a better idea maybe?
		if (vars.draw_menu)
		{
			if (GetAsyncKeyState(VK_UP) & 1)
				kingpin_menu::g_menu.up();

			if (GetAsyncKeyState(VK_DOWN) & 1)
				kingpin_menu::g_menu.down();

			if (GetAsyncKeyState(VK_RIGHT) & 1)
				kingpin_menu::g_menu.increase();

			if (GetAsyncKeyState(VK_LEFT) & 1)
				kingpin_menu::g_menu.decrease();
		}

		if(static_cast<int>(vars.allow_memmgr))
			c_mem_manager::check_edicts();

		original::o_clientthink(client, cmd);
	}

	void hooked_RenderFrame(refdef_t* rd)
	{
		original::o_renderframe(rd);
		//globalvars::refdef = rd;

		if (globalvars::local_player->s.number == 1 && vars.norecoil)
		{
			globalvars::local_player->client->kick_origin[0] = 0.f;
			globalvars::local_player->client->kick_origin[1] = 0.f;
			globalvars::local_player->client->kick_origin[2] = 0.f;

			globalvars::local_player->client->kick_angles[0] = 0.f;
			globalvars::local_player->client->kick_angles[1] = 0.f;
			globalvars::local_player->client->kick_angles[2] = 0.f;
		}

		if (vars.esp)
		{
			//starts drawing right at the center of model (origin)
			//so you might wanna tinker with positioning of the label a little bit
			visuals::draw_cast();
			visuals::draw_rats();
		}

		if (vars.draw_menu)
		{
			kingpin_menu::g_menu.draw();
		}
		else
		{
			if (vars.debug)
			{
				util::draw_string(20, 100, 0, true, "viewangles[x %.2f / y %.2f / z %.2f]",
					rd->viewangles[0], rd->viewangles[1], rd->viewangles[2]);
				util::draw_string(20, 108, 0, true, "cast amount %d", globalvars::cast_amount);
				if (*globalvars::connstate == connstate_t::ca_active && globalvars::target)
					util::draw_string(20, 116, 0, true, "target 0x%X, %s, %d HP", globalvars::target, globalvars::target->classname, globalvars::target->health);
				if (GetModuleHandle("gamex86.dll") && globalvars::locals)
				{
					//wanna shoot in the bar? just edit this variable
					util::draw_string(20, 124, 0, true, "bar lvl: %s", globalvars::locals->bar_lvl ? "it's da bar" : "nope, not bar");
					util::draw_string(20, 132, 0, true, "locals 0x%X", globalvars::locals);
					util::draw_string(20, 140, 0, true, "current cmd num: %d", *globalvars::cmdnum);
				}
			}
		}
	}

	void hooked_ParseLaser(int colors)
	{
		//4 colors, each color -> 2 digits in hex
		colors = 0x27C1904F;
		original::o_parselaser(colors);
	}

	//big solution to a very ANNOYING """feature"""
	//inb4 hook getgameapi/getrefapi
	HMODULE __stdcall hooked_LoadLibrary(LPCSTR module_name)
	{
		//we need to (attempt to) load the lib first
		auto library = LoadLibraryA(module_name);

		//check for path (or just name && library != null)
		if (std::strstr(module_name, "main/gamex86.dll") && library)
		{
			//it's that nigger piece of shit, pop a bunch o' hooks in it's ass
			printf("[kingpin_hook] gamex86 re-hooked\n");
			SetupGameLibHooks();
		}

		//ref_gl unload happens on vid_restart or whatever
		if (std::strstr(module_name, "ref_gl.dll") && library)
		{
			original::o_renderframe = (renderframe_t)DetourFunction((PBYTE)0x10010D70, (PBYTE)cheat::hooked_RenderFrame);

			printf("[kingpin_hook] ref_gl re-hooked\n");
		}

		if (library)
			printf("[kingpin_hook] loaded module %s\n", module_name);

		return library;
	}
}