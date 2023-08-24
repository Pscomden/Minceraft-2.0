#include "Command.h"

namespace Command {

	static std::shared_ptr<Player> player;
	static std::string command_input;
	static std::vector<std::string> input_history;
	static int current_history;
	static bool is_open = false;
	static const short MAX_HISTORY = 50;

	void open() {
		is_open = true;
		current_history = -1;
	}

	void close() {
		is_open = false;
		command_input.clear();
	}

	bool isOpen() {
		return is_open;
	}

	bool inputExists() {
		return command_input.size() > 0;
	}

	std::string getInput() {
		return command_input;
	}

	void teleport(std::shared_ptr<Entity> entity, glm::vec3 new_pos, bool relative) {
		if (relative) {
			entity->addPos(new_pos);
		}
		else {
			entity->setPos(new_pos);
		}
	}
	// https://www.techiedelight.com/split-string-cpp-using-delimiter/
	void splitString(std::string const& str, const char delim,
		std::vector<std::string>& out) 
	{
		size_t start;
		size_t end = 0;
		while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
		{
			end = str.find(delim, start);
			out.push_back(str.substr(start, end - start));
		}
	}

	void placeStructure(glm::ivec3 pos, std::string structure) {
		if (structure == "tree") {
			std::shared_ptr<Chunk> chunk = ChunkManager::getChunk(ChunkManager::posToChunk(pos));
			Structure tree = Structure();
			tree.setRandomTree(pos, 123);
			tree.place(chunk);
			chunk->clearMesh();
			ChunkMeshBuilder::buildMeshEdges(chunk, false);
			pool.QueueJob([chunk]() {ChunkMeshBuilder::buildChunkMesh(chunk, false); });
		}
	}

	void parseInput() {
		std::vector<std::string> split_str;
		splitString(command_input, ' ', split_str);
		if (split_str[0] == "tp") {
			std::shared_ptr<Entity> entity;
			size_t offset = 0;
			if (split_str.size() == 4) {
				entity = player;
				offset = 0;
			}
			else if (split_str.size() == 5) {
				// TODO: find player from
				offset = 1;
			}
			if (split_str[1 + offset].at(0) == '~') {
				split_str[1 + offset].erase(split_str[1 + offset].begin());
				split_str[2 + offset].erase(split_str[2 + offset].begin());
				split_str[3 + offset].erase(split_str[3 + offset].begin());
				if (split_str[1 + offset] == "") {
					split_str[1 + offset] = "0";
				}
				if (split_str[2 + offset] == "") {
					split_str[2 + offset] = "1";
				}
				if (split_str[3 + offset] == "") {
					split_str[3 + offset] = "2";
				}
				teleport(player, glm::vec3(std::stof(split_str[1 + offset]),
					std::stof(split_str[2 + offset]),
					std::stof(split_str[3 + offset])), true);
			}
			else {
				teleport(player, glm::vec3(std::stof(split_str[1 + offset]),
					std::stof(split_str[2 + offset]),
					std::stof(split_str[3 + offset])), false);
			}
		}
		if (split_str[0] == "give") {
			if (split_str.size() == 2) {
				player->getInventory()->add(Item(std::stoi(split_str[1]), 1));
			}
			if (split_str.size() == 3) {
				player->getInventory()->add(Item(std::stoi(split_str[1]), std::stoi(split_str[2])));
			}
		}
		if (split_str[0] == "mode") {
			if (split_str.size() == 2) {
				if (split_str[1] == "survival" || split_str[1] == "0") {
					player->setMode(Player::Mode::SURVIVAL);
				}
				else if (split_str[1] == "god" || split_str[1] == "1") {
					player->setMode(Player::Mode::GOD);
				}
				else if (split_str[1] == "ghost" || split_str[1] == "2") {
					player->setMode(Player::Mode::GHOST);
				}
			}
			else if (split_str.size() == 3) {
				// find player to set game mode
			}
		}
		if (split_str[0] == "range") {
			if (split_str.size() == 3) {
				World::setHorizontalRange(std::stoi(split_str[1]));
				World::setVerticalRange(std::stoi(split_str[2]));
			}
		}
		if (split_str[0] == "place") {
			if (split_str.size() == 2) {
				placeStructure(player->getPos(), split_str[1]);
			}
		}
	}

	void execute() {
		if (command_input.size() > 0) {
			parseInput();
		}
		input_history.push_back(command_input);
		if (input_history.size() > MAX_HISTORY) {
			input_history.erase(input_history.begin());
		}
		close();
	}

	void addChar(char character) {
		command_input += character;
		current_history = -1;
	}

	void removeLastChar() {
		if (command_input.size() > 0) {
			command_input.erase(command_input.end() - 1);
		}
		current_history = -1;
	}

	void arrowUp() {
		if (input_history.size() > 0) {
			if (current_history == -1) {
				current_history = input_history.size() - 1;
			}
			else {
				current_history--;
				if (current_history < 0) {
					current_history = 0;
				}
			}
			command_input = input_history[current_history];
		}
	}

	void arrowDown() {
		if (input_history.size() > 0) {
			if (current_history == -1) {
				current_history = input_history.size() - 1;
			}
			else {
				current_history++;
				if (current_history > input_history.size() - 1) {
					current_history = input_history.size() - 1;
				}
			}
			command_input = input_history[current_history];
		}
	}

	void Command::setPlayer(std::shared_ptr<Player> player) {
		Command::player = player;
	}
}