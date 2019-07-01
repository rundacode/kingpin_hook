#include "mem_manager.h"
#include "globals.h"

void c_mem_manager::check_edicts()
{
	if (*globalvars::connstate != ca_active)
		return;

	for (size_t i = 0; i < MAX_CHARACTERS; i++)
	{
		auto edict = globalvars::locals->characters[i];
		if (!edict || edict->health > 0)
			continue;

		if (strstr(edict->classname, "cast_"))
		{
			printf("[kingpin_hook] freeing edict 0x%p, classname %s\n", edict, edict->classname);
			globalvars::locals->num_characters--;
			util::free_entity(edict);
		}
	}
}