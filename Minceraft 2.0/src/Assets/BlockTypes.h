#pragma once

#include "../Include.h"
#include "Item.h"
#include "../Window/TextureAtlas.h"

namespace BlockTypes {

	struct BlockAttribute {
		std::string name = "null";
		int id = 0;
		// use in item_to_id to get drop id
		std::string drops = "null";
		// 0 is not solid and can replace with another tile
		// 1 is completely solid
		// 2 is not solid and can't replace with another tile
		int solid = 0;
		short harvest = 0; // 0 is disabled harvesting
		unsigned int toughness = 1; // how long to break in milliseconds by hand
	};

	bool init(std::string asset_directory);
	std::array<std::array<glm::vec2, 2>, 6> getBlockUVs(int id);
	std::array<glm::vec2, 2> getFaceUVs(std::string name);
	BlockAttribute getBlockType(int id);
	int numBlocks();
	int nameToID(std::string name);
}