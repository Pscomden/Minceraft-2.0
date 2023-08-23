#include "Item.h"

int Item::MAX_COUNT = 100;

namespace ItemTypes {
	
	static robin_hood::unordered_map<std::string, int> name_to_id;
	static std::vector<ItemFormat> item_types;

	bool init() {

		return true;
	}

	void createBlockItem(ItemFormat block) {
		block.id = item_types.size();
		item_types.push_back(block);
		name_to_id[block.name] = block.id;
	}

	ItemFormat getType(std::string name) {
		if (name_to_id.contains(name)) {
			return item_types[name_to_id[name]];
		}
		return ItemFormat("null", 0, "null", 0, std::array<glm::vec2, 2>());
	}

	ItemFormat getType(int id) {
		return item_types[id];
	}

	int nameToID(std::string name) {
		return name_to_id[name];
	}
}