#pragma once

#include "../Include.h"

struct Block {
	Block() {
		
	}

	Block(int id) {
		this->id = id;
	}
	unsigned short id = 0;
	//unsigned short sun_light = 0;
	//unsigned short other_light = 0;
};