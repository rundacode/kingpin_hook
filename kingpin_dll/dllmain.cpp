/* = = = = = = = = = = = = = = = = = = = = = = = = = = */
//[kingpin_hook]
//made our of boredom
//works for both gog and steam versions
//to debug steam version, unpack it first
//credits :: IDA Pro, ReClass, Q2/Kingpin SDK, Firehawk for big(large(enormous)) menu
/* = = = = = = = = = = = = = = = = = = = = = = = = = = */
#include "common.h"
#include "globals.h"
#include "kingpin_hooked.h"
#include "menu.h"
#include "ogl.h"
#include <thread>

#pragma warning (disable : 4477)

using namespace std::chrono_literals;

//because game console's font is barely readable at all
void attach_console()
{
	AllocConsole();
	auto menu = GetSystemMenu(GetConsoleWindow(), 0);
	if (!menu)
		return;

	DeleteMenu(menu, 0xF060, 0x00000000L);
	_iobuf* data;
	freopen_s(&data, "CONIN$", "r", __acrt_iob_func(0));
	freopen_s(&data, "CONOUT$", "w", __acrt_iob_func(1));

	SetConsoleTitleA("[kingpin_hook]");
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		attach_console();

		util::com_printf("\n\nkingpin hook - %s\n\n", __DATE__);
		printf("kingpin_hook injected - %s\n\n", __DATE__);

		printf("[kingpin_hook] applying viewmodel fov patch\n");
		util::apply_patch((void*)0x44C510, 180.f);

		//if the game crashes or exits abruptly
		//it won't reset windows's mouse settings
		//and retarded accel stays on
		printf("[kingpin_hook] disabling retarded mouse accel enabler\n");
		util::apply_patch((void*)0x4533C0, 0);

		//solution to everlasting problem of idtech2 unloading and loading gamex86
		printf("[kingpin_hook] hooking loadlibrary\n");
		util::hook_iat("kingpin.exe", "KERNEL32.dll", "LoadLibraryA", cheat::hooked_LoadLibrary);

		//Try to hook if the game module is present
		if (GetModuleHandle("gamex86.dll"))
			cheat::SetupGameLibHooks();
		else
			printf("[kingpin_hook] couldn't hook because gamex86.dll isn't loaded\n");

		/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

		if (globalvars::game_api && globalvars::ref_api)
		{
			printf("\nGameAPI 0x%X | RefAPI 0x%X\n", 
				reinterpret_cast<unsigned int>(globalvars::game_api), reinterpret_cast<unsigned int>(globalvars::ref_api));

			//replace game's renderframe with ours, so we can draw stuff
			original::o_renderframe = (renderframe_t)DetourFunction((PBYTE)0x10010D70, (PBYTE)cheat::hooked_RenderFrame);

			if (*globalvars::connstate == connstate_t::ca_active)
			{
				globalvars::local_player = util::get_cast(0); /*globalvars::game_api->GetEntity(1);*/
				if (globalvars::local_player)
					printf("[!]\tgot localplayer at 0x%X, client 0x%X, classname %s, movetype %d\n", globalvars::local_player, globalvars::local_player->client, globalvars::local_player->classname, globalvars::local_player->movetype);
			}
			else
				printf("[!]\tnot in an active game, didn't fetch localplayer, state %d\n", *globalvars::connstate);

			printf("GameAPI Version: %d\n", globalvars::game_api->apiversion);
			printf("RefAPI Version: %d\n", globalvars::ref_api->api_version);

			kingpin_menu::g_menu.init();
		}

		//solving a little problem of spinning and moving around while in cheat's menu
		util::addtext("unbind r_arrow; unbind l_arrow; unbind d_arrow; unbind u_arrow");
		util::executebuf();

		printf("arrow key unbinding: executing buffer..\n");

		util::addtext("unbind f9; unbind f10; unbind f12");
		util::executebuf();

		printf("\n");

		//printf("[kingpin_hook] patching sendcommand - speedhack is bound to Mouse4\n");
		//DetourFunction((PBYTE)0x40F690, (PBYTE)cheat::hooked_SendCommand);

		//Wallhack and FX related
		original::o_deltaentity = (cl_deltaentity_t)DetourFunction((PBYTE)0x402E40, (PBYTE)cheat::hooked_DeltaEntity);

		original::o_parselaser = (parselaser_t)DetourFunction((PBYTE)0x417E40, (PBYTE)cheat::hooked_ParseLaser);

		//Not much of a use
		original::o_basemove = (basemove_t)DetourFunction((PBYTE)0x40BA80, (PBYTE)cheat::hooked_BaseMove);
		original::o_finishmove = (finishmove_t)DetourFunction((PBYTE)0x40C280, (PBYTE)cheat::hooked_FinishMove);
		original::o_findindex = (findindex_t)DetourFunction((PBYTE)0x442920, (PBYTE)cheat::hooked_FindIndex);

		original::o_sendcmd = (sendcmd_t)DetourFunction((PBYTE)0x40C630, (PBYTE)cheat::hooked_SendCmd);

		HMODULE hOpenGL = GetModuleHandle("opengl32.dll");
		original::o_glenable = (glenable_t)DetourFunction((PBYTE)GetProcAddress(hOpenGL, ("glEnable")), (PBYTE)cheat::hooked_GLEnable);

	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

