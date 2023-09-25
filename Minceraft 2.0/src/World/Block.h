#pragma once

#include "../Include.h"

struct Block {
	Block() {
		
	}

	Block(int id) {
		this->id = id;
	}

	//void setSunLight(unsigned short level) {
	//	light |= ((level << 4) & 0b11110000);
	//}

	//void setExtLight(unsigned short level) {
	//	light |= (level & 0b00001111);
	//}

	//unsigned short getSunLight() {
	//	return (light & 0b11110000) >> 4;
	//}

	//unsigned short getExtLight() {
	//	return (light & 0b00001111);
	//}

	//unsigned char light = 0b00000000;
	unsigned short id = 0;
};