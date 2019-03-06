#pragma once
#include "detours.h"
#include "q2_shared.h"
#include "globals.h"
#include "engine.h"

//game functions
using firebullet_t = void(__cdecl*)(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
using fireshotgun_t = void(__cdecl*)(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod);
using firelead_t = void(__cdecl*)(int kick, int damage, edict_t* self, vec3_t start, vec3_t aimdir, temp_event_t type, int hspread, int vspread, int mod);
using basemove_t = int(__cdecl*)(usercmd_t* cmd);
using runframe_t = void(__cdecl*)();
using calcviewangles_t = void(__cdecl*)(); //00406C70
using parselaser_t = void(__cdecl*)(int colors);
using finishmove_t = int(__cdecl*)(usercmd_t* cmd);
using clientthink_t = void(__cdecl*)(edict_t* client, usercmd_t* cmd);
using findindex_t = int(__cdecl*)(const char* name, int start, int max, bool create);
using drawline_t = void(__cdecl*)(vector*, vector*); //200AF190
using sendcmd_t = void(__cdecl*)();

//renderer
using glimp_endframe_t = void(__cdecl*)();
using renderframe_t = void(__cdecl*)(refdef_t* rd);
using cl_deltaentity_t = void(__cdecl*)(frame_t* frame, int newnum, entity_state_t* old, int bits);

namespace original
{
	extern firebullet_t o_firebullet;
	extern fireshotgun_t o_fireshotgun;
	extern firelead_t o_firelead;
	extern basemove_t o_basemove;
	extern calcviewangles_t o_calcviewangles;
	extern finishmove_t o_finishmove;
	extern findindex_t o_findindex;
	extern runframe_t o_runframe;
	extern clientthink_t o_clientthink;
	extern parselaser_t o_parselaser;
	extern sendcmd_t o_sendcmd;

	extern glimp_endframe_t o_endframe;
	extern renderframe_t o_renderframe;
	extern cl_deltaentity_t o_deltaentity;
}

namespace cheat
{
	void hooked_FireBullet(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
	void hooked_FireShotgun(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod);
	void hooked_FireLead(int kick, int damage, edict_t* self, vec3_t start, vec3_t aimdir, temp_event_t type, int hspread, int vspread, int mod);
	void hooked_RunFrame();
	void hooked_CalcViewAngles();
	void hooked_ParseLaser(int colors);
	void hooked_ClientThink(edict_t* client, usercmd_t* cmd);
	void hooked_SendCmd();
	HMODULE __stdcall hooked_LoadLibrary(LPCSTR module_name);
	int hooked_BaseMove(usercmd_t* cmd);
	int hooked_FinishMove(usercmd_t* cmd);
	int hooked_FindIndex(const char* name, int start, int max, bool create);

	void hooked_EndFrame();
	void hooked_DeltaEntity(frame_t* frame, int newnum, entity_state_t* old, int bits);
	void hooked_RenderFrame(refdef_t* rd);

	void SetupGameLibHooks();
}

