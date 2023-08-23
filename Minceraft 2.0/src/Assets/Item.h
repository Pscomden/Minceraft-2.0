#pragma once

#include "../Include.h"

struct Item {
	Item(int id, int count) : id(id), count(count)
	{

	}

	int add(int count) {
		this->count += count;
		if (this->count > MAX_COUNT) {
			int remainder = this->count - MAX_COUNT;
			this->count -= remainder;
			return remainder;
		}
		return 0;
	}

	int remove(int amount) {
		int remainder = amount - count;
		count -= amount;
		if (count < 1) {
			count = 0;
			id = 0;
		}
		if (remainder > 0)
			return remainder;
		return 0;
	}

	bool operator==(Item other) {
		return this->id == other.id;
	}

	int id;
	int count;
	static int MAX_COUNT;
};

struct ItemFormat {
	ItemFormat(std::string name, int id, std::string block, int harvest, std::array<glm::vec2, 2> uv_coords)
		: name(name), id(id), block(block), harvest(harvest), uv_coords(uv_coords)
	{

	}
	std::string name = "null";
	int id = 0;
	std::string block = "null";
	int harvest = 1; // all items can break blocks
	std::array<glm::vec2, 2> uv_coords;
};

namespace ItemTypes {
	bool init();
	void createBlockItem(ItemFormat block);
	ItemFormat getType(std::string name);
	ItemFormat getType(int id);
	int nameToID(std::string name);
}