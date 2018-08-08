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
using raytrace_t = trace_t(__cdecl*)(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask);
using anglevec_t = void(__cdecl*)(vector angles, vector forward, vector right, vector up);
using vecangle_t = void(__cdecl*)(vector &vec, vector &angles );
using firebfg_t = void(__cdecl*)(edict_t* owner, vec3_t start, vec3_t dir, int dmg, int speed, float dmg_radius);

namespace util
{
	extern printf_t com_printf;
	extern finditem_t find_item;
	extern isvisible_t is_visible;
	extern findbyclass_t find_by_classname;
	extern edictfree_t free_entity;
	extern anglevec_t angle_vectors;
	extern vecangle_t vector_angle;
	extern raytrace_t trace;
	
	extern firebfg_t fire_bfg;
	extern startsound_t playsound;

	extern addtext_t addtext;
	extern executebuf_t executebuf;

	extern void draw_string(int x, int y, int background, bool is_white, const char* string, ...);
	extern void dump_edicts(int amt);
	extern void hook_iat(const char* iat_module_name, const char* import_module_name, const char* fn_name, void* new_fn);
	extern int draw_line(vector org, vector target, int a1 = 4, int a2 = 25);

	//provides memory patching capabilities
	template<typename T>
	void apply_patch(void* address, T value)
	{
		DWORD mem_flags;
		VirtualProtect(address, sizeof value, PAGE_READWRITE, &mem_flags);
		*reinterpret_cast<T*>(address) = value;
		VirtualProtect(address, sizeof value, mem_flags, &mem_flags);
	};

	extern edict_t* get_cast(int index);
}

namespace globalvars {
	extern game_export_t* game_api;
	extern refexport_t* ref_api;

	extern game_import_t* game_import;
	extern refimport_t* ref_import;
	
	extern edict_t* local_player;
	extern edict_t* target;

	extern connstate_t* connstate;
	extern refdef_t* refdef;

	extern level_locals_t* locals;

	extern int cast_amount;
	extern edict_t** cast_list;
}