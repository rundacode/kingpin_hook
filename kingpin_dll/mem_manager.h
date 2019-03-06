#pragma once

#include "common.h"

//If you try to spawn many NPCs, you'll eventually reach
//max edicts limit of 64. The game doesn't free unused NPCs,
//so here's a little attempt at helping the game
//(Because I want to spawn & kill :o)

//Memory manager uses game's native functions to
//free the edicts
class c_mem_manager
{
public:
	//Checks if any edicts need to be freed
	void check_edicts();
	//Free an edict at given pointer
	void free_edict(void* ptr);
};