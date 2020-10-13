#pragma once
#include "common.h"
#include "engine.h"
#include "q2_shared.h"
#include <iostream>

using printf_t = void(__cdecl*)(const char* fmt, ...);
using finditem_t = gitem_t * (__cdecl*)(const char* pickup_name);
using findbyclass_t = gitem_t * (__cdecl*)(const char* classname);
using addtext_t = void(__cdecl*)(const char* cmd);
using executebuf_t = void(__cdecl*)();
using startsound_t = void(__cdecl*)(const char*);
using isvisible_t = bool(__cdecl*)(edict_t* self, edict_t* ent);
using edictfree_t = void(__cdecl*)(edict_t* ent);
using avoident_t = void(__cdecl*)(edict_t*);
using raytrace_t = trace_t(__cdecl*)(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask);
using anglevec_t = void(__cdecl*)(vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
using vecangle_t = void(__cdecl*)(vector &vec, vector &angles );
using vectorma_t = void(_cdecl*)(vec3_t veca, float scale, vec3_t vecb, vec3_t vecc);
using firebfg_t = void(__cdecl*)(edict_t* owner, vec3_t start, vec3_t dir, int dmg, int speed, float dmg_radius);
using firerail_t = void(__cdecl*)(edict_t* owner, vec3_t start, vec3_t dir, int dmg, int kick);
using fireblaster_t = void(__cdecl*)(edict_t* owner, vec3_t start, vec3_t dir, int dmg, int speed, int effect, BOOL is_hyper);
using projsrc_t = void(__cdecl*)(gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
using dmgthrualpha_t = BOOL(__cdecl*)(trace_t trace, edict_t* targ, edict_t* inflictor, vec3_t dst);
using candamage_t = BOOL(__cdecl*)(edict_t* target, edict_t* attacker);

namespace util
{
	extern printf_t com_printf;
	extern finditem_t find_item;
	extern isvisible_t is_visible;
	extern avoident_t avoid_ent;
	extern findbyclass_t find_by_classname;
	extern edictfree_t free_entity;
	extern anglevec_t angle_vectors;
	extern vecangle_t vector_angle;
	extern raytrace_t trace;
	extern dmgthrualpha_t candamagethrualpha;
	extern candamage_t candamage;
	extern vectorma_t vector_ma;

	extern firebfg_t fire_bfg;
	extern firerail_t fire_rail;
	extern fireblaster_t fire_blaster;
	extern startsound_t playsound;

	extern addtext_t addtext;
	extern executebuf_t executebuf;

	extern void draw_string(int x, int y, int background, bool is_white, const char* string, ...);
	extern void dump_edicts(int amt);
	extern void hook_iat(const char* iat_module_name, const char* import_module_name, const char* fn_name, void* new_fn);
	extern int draw_line(vector org, vector target, int a1 = 4, int a2 = 25);
	extern bool _trace(edict_t* target, bool check_transparent);

	extern void randomize_skin();

	//provides memory patching capabilities
	template<typename T>
	void apply_patch(void* address, T value)
	{
		DWORD mem_flags;
		VirtualProtect(address, sizeof value, PAGE_READWRITE, &mem_flags);
		*reinterpret_cast<T*>(address) = value;
		VirtualProtect(address, sizeof value, mem_flags, &mem_flags);
	};

	extern const char* rand_skin;

	extern edict_t* get_cast(int index);
}

namespace globalvars {
	extern game_export_t* game_api;
	extern refexport_t* ref_api;

	extern game_import_t* game_import;
	extern refimport_t* ref_import;
	
	extern edict_t* local_player;
	extern edict_t* target;
	extern edict_t* edicts;

	extern connstate_t* connstate;
	extern refdef_t* refdef;

	extern level_locals_t* locals;

	extern int cast_amount;
	extern edict_t** cast_list;

	extern vector* viewangles;
	extern vector aim_angles;
	extern vec3_t v_aim_angles;
	extern int* cmdnum;
	extern usercmd_t* usercmds;

	extern object_bounds_t* objbounds;

	extern usercmd_t* get_usercmd(int num);

}