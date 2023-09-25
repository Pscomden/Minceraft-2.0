#include "BlockTypes.h"

namespace BlockTypes {
	
	static std::string asset_directory;
	static std::unordered_map<std::string, std::array<glm::vec2, 2>> face_uvs;
	static std::vector<std::array<std::array<glm::vec2, 2>, 6>> block_uvs;
	static std::unordered_map<std::string, int> name_to_id;
	static std::vector<BlockAttribute> block_attributes;
	static int max_blocks = 0;

	bool loadAttributes() {
		std::ifstream file = std::ifstream();
		file.open(asset_directory + "/BlockTypes/attributes.txt");
		if (!file.good()) {
			printf("ERROR: Couldn't open block attributes file!\n");
		}
		bool finished = true;
		std::string next_block;
		std::string line;
		// air block
		block_attributes.push_back(BlockAttribute());
		while (std::getline(file, line)) {
			if (line[0] == ';') {
				finished = true;
				max_blocks++;
				continue;
			}
			if (finished) {
				finished = false;
				next_block = line;
				block_attributes.push_back(BlockAttribute());
				block_attributes.at(block_attributes.size() - 1).name = next_block;
				std::cout << "Loading Block " << next_block << "\n";
				continue;
			}
			std::string str = "";
			while (line[0] != ' ' && line.size() > 1) {
				str += line[0];
				line.erase(line.begin() + 0);
			}
			line.erase(line.begin() + 0);
			if (str == "id") {
				name_to_id[next_block] = std::stoi(line);
				block_attributes.at(block_attributes.size() - 1).id = std::stoi(line);
			}
			else if (str == "drops") {
				block_attributes.at(block_attributes.size() - 1).drops = line;
			}
			else if (str == "solid") {
				block_attributes.at(block_attributes.size() - 1).solid = std::stoi(line);
			}
			else if (str == "harvest") {
				block_attributes.at(block_attributes.size() - 1).harvest = std::stoi(line);
			}
			else if (str == "toughness") {
				block_attributes.at(block_attributes.size() - 1).toughness = std::stoi(line);
			}
		}
		file.close();
		
		return true;
	}

	bool loadUVFaces() {
		std::ifstream file = std::ifstream();
		std::string block_directory = asset_directory + "Textures/Blocks/";
		file.open(block_directory + "list.txt");
		if (!file.good()) {
			printf("ERROR: Couldn't open block texture list!\n");
			return false;
		}
		std::string line;
		while (std::getline(file, line)) {
			glm::vec2 pos = Atlas::getAtlas()->addTexture(block_directory + line + ".png");
			glm::vec2 uv_pos = pos / Atlas::getAtlas()->GetSize();
			std::array<glm::vec2, 2> uv = { uv_pos, uv_pos + Atlas::getAtlas()->GetUVSize() };
			face_uvs.emplace(std::pair(line, uv));
		}
		file.close();

		return true;
	}

	bool loadBlockUVMappings() {
		// block uvs are added out of order
		for (int i = 0; i < max_blocks + 1; i++) {
			block_uvs.push_back(std::array<std::array<glm::vec2, 2>, 6>());
		}
		std::ifstream file = std::ifstream();
		std::string file_name = asset_directory + "BlockTypes/faces.txt";
		file.open(file_name);
		if (!file.good()) {
			printf("ERROR: Couldn't open face to block file!\n");
			return false;
		}
		bool finished = true;
		std::string next_block;
		std::string line;
		int index = 0;
		while (std::getline(file, line)) {
			if (line == ";") {
				finished = true;
				continue;
			}
			if (finished) {
				finished = false;
				next_block = line;
				index = 0;
				continue;
			}
			block_uvs[name_to_id[next_block]].at(index) = face_uvs[line];
			index++;
		}

		return true;
	}

	bool loadBlockItems() {
		for (auto& block : block_attributes) {
			std::array<glm::vec2, 2> uvs = block_uvs[block.id][0];
			ItemTypes::createBlockItem(
				ItemFormat(block.name, block.id, block.name, 1, uvs)
			);
		}

		return true;
	}
	
	bool init(std::string asset_directory) {
		BlockTypes::asset_directory = asset_directory;
		// load block attributes
		if (!loadAttributes()) {
			return false;
		}
		// load uv faces
		if (!loadUVFaces()) {
			return false;
		}
		// load mappings of faces to blocks
		if (!loadBlockUVMappings()) {
			return false;
		}
		if (!loadBlockItems()) {
			return false;
		}

		return true;
	}

	std::array<std::array<glm::vec2, 2>, 6> getBlockUVs(int id) {
		return block_uvs[id];
	}

	std::array<glm::vec2, 2> getFaceUVs(std::string name) {
		return face_uvs[name];
	}

	BlockAttribute getBlockType(int id) {
		return block_attributes[id];
	}

	int numBlocks() {
		return max_blocks;
	}

	int nameToID(std::string name) {
		return name_to_id[name];
	}

	bool isTrans(int id) {
		return block_attributes[id].solid == 0;
	}
}