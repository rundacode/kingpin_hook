#include "common.h"
#include "globals.h"
#include "vars.h"

vec3_t vec3_origin = { 0,0,0 };

printf_t util::com_printf = (printf_t)0x42050D;
finditem_t util::find_item = (finditem_t)0x2004FD95;
findbyclass_t util::find_by_classname = (findbyclass_t)0x2004FD35;
addtext_t util::addtext = (addtext_t)0x41CD80;
startsound_t util::playsound = (startsound_t)0x43BEF0;
isvisible_t util::is_visible = (isvisible_t)0x2002DDE0;
executebuf_t util::executebuf = (executebuf_t)0x41CEF0;
raytrace_t util::trace = (raytrace_t)0x448C39;

//converts angles into vectors
anglevec_t util::angle_vectors = (anglevec_t)0x437FAC;
//converts vectors into angles
vecangle_t util::vector_angle = (vecangle_t)0x200A9400;


firebfg_t util::fire_bfg = (firebfg_t)0x2008C53E;
edictfree_t util::free_entity = (edictfree_t)0x2008822C;

namespace globalvars {

	game_export_t* game_api = (game_export_t*)0x2014E4C0;
	refexport_t* ref_api = (refexport_t*)0x482620;
	//ref_api -> dump .exe, search for "loadlibrary failed"
	//qmemcpy(&v4, &v6, 0x44u);
	//qmemcpy(&dword_482620 <- le pointer

	game_import_t* game_import = (game_import_t*)0x20150560;
	refimport_t* ref_import = nullptr;

	edict_t* local_player = nullptr;
	edict_t* target = nullptr;

	connstate_t* connstate = (connstate_t*)0x127A180;

	refdef_t* refdef = nullptr;

	level_locals_t* locals = (level_locals_t*)0x201506C0;

	int cast_amount = globalvars::locals->num_characters;
	edict_t** cast_list = globalvars::locals->characters;
}

namespace util {
	
	void draw_string(int x, int y, int background, bool is_white, const char* string, ...)
	{
		char buffer[256];
		va_list va;
		va_start(va, string);
		vsprintf_s(buffer, string, va);
		va_end(va);

		int width = 8 * strlen(buffer);

		char* text = buffer;

		if (background > 0)
			globalvars::ref_api->DrawFill(x, y, width, 8, background);

		while (*text)
		{
			globalvars::ref_api->DrawChar(x, y, *text ^ (is_white ? 0x80 : 0));
			x += 8;
			text++;
		}
	}

	//Shoots a trace. Easy.
	bool _trace(edict_t* target)
	{
		vec3_t	spot1;
		vec3_t	spot2;
		trace_t	trace;

		auto VecCopy = [&](const vector a, vec3_t b) -> void
		{
			b[0] = a.x;
			b[1] = a.y;
			b[2] = a.z;
		};

		VecCopy(globalvars::local_player->s.origin, spot1);
		spot1[2] += globalvars::local_player->viewheight;
		VecCopy(target->s.origin, spot2);
		spot2[2] += target->viewheight;
		trace = globalvars::game_import->trace(spot1, vec3_origin, vec3_origin, spot2, globalvars::local_player, 
			CONTENTS_SOLID|CONTENTS_AUX|CONTENTS_LAVA|CONTENTS_SLIME); //can you even trace thru grates? sometimes works, sometimes not

		if (trace.fraction == 1.0)
			return true;
		return false;
	}

	edict_t* get_cast(int index)
	{
		return *reinterpret_cast<edict_t**>(uintptr_t(globalvars::cast_list) + index * 0x4);
	}

	void dump_edicts(int amt)
	{
		for (int i = 0; i < amt; i++)
		{
			auto entity = globalvars::game_api->GetEntity(i);
			printf("Entity at 0x%p, classname %s\n", entity, entity->classname);
		}
	}

	//rebuild of what NAV_DrawLine does, was rebuilt to see what those integers are
	//nothing interesting, alas..
	int draw_line(vector org, vector dst, int a1, int a2)
	{
		using unk_t = void(__cdecl*)(int);
		static unk_t first_call = (unk_t)0x4424A1; //gamex86 -> 201505C8

		using unk2_t = void(__cdecl*)(vector&);
		static unk2_t second_call = (unk2_t)0x44250F; // gamex86 -> 201505DC

		using unk3_t = int(__cdecl*)(vector&, multicast_t);
		static unk3_t third_call = (unk3_t)0x445AAB;

		//writeint
		first_call(a1);
		first_call(a2);

		//writeposition
		second_call(org);
		second_call(dst);

		return third_call(org, multicast_t::MULTICAST_PHS);

	}

	//pasted from some valve anti-cheat dumper
	void hook_iat(const char* iat_module_name, const char* import_module_name, const char* fn_name, void* new_fn)
	{
		auto module = (uintptr_t)GetModuleHandleA(iat_module_name);

		auto dos_header = (PIMAGE_DOS_HEADER)module;
		auto nt_headers = (PIMAGE_NT_HEADERS)(module + dos_header->e_lfanew);

		auto imports = (PIMAGE_IMPORT_DESCRIPTOR)(module + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		for (auto import = imports; import->Name; ++import) {
			auto module_name = (const char*)(module + import->Name);
			if (std::strcmp(module_name, import_module_name))
				continue;

			auto original_first_thunk = (PIMAGE_THUNK_DATA)(module + import->OriginalFirstThunk);
			auto first_thunk = (PIMAGE_THUNK_DATA)(module + import->FirstThunk);

			for (; original_first_thunk->u1.AddressOfData; ++original_first_thunk, ++first_thunk) {
				auto name = (const char*)((PIMAGE_IMPORT_BY_NAME)(module + original_first_thunk->u1.AddressOfData))->Name;
				if (std::strcmp(name, fn_name))
					continue;

				auto fn_address = &first_thunk->u1.Function;

				DWORD old;
				VirtualProtect(fn_address, sizeof(new_fn), PAGE_READWRITE, &old);
				*fn_address = (DWORD)new_fn;
				VirtualProtect(fn_address, sizeof(new_fn), old, &old);
				break;
			}

			break;
		}
	};
}

vars_t vars;