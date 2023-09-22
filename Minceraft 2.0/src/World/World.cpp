#include "World.h"

namespace World {

	// how much a player can see
	static int horizontal_range;
	static int vertical_range;
	// how far a player can explore
	static glm::ivec3 start_limit;
	static glm::ivec3 end_limit;
	static int seed;
	static std::string world_directory;
	static bool cubic_chunks;
	static std::vector<std::shared_ptr<Entity>> entities;
	static std::shared_ptr<Player> player;

	void start(std::string world_directory) {
		// TODO: load from settings
		/*horizontal_range = 50;
		vertical_range = 8;
		setLimit(glm::ivec3(-5, -2, -5), glm::ivec3(5, 2, 5));*/
		horizontal_range = 15;
		vertical_range = 9;
		setLimit(glm::ivec3(-100000, -100000, -100000), glm::ivec3(100000, 100000, 100000));
		seed = 10;
		World::world_directory = world_directory;
		cubic_chunks = true;
		// TODO: change to load player
		player = std::shared_ptr<Player>(new Player(glm::vec3(0.0f, 70.0f, 0.0f)));
		entities.push_back(player);
		//entities.push_back(std::shared_ptr<Entity>(new Entity(glm::vec3(0.0f, 70.0f, 0.0f))));
		ChunkManager::setRange(horizontal_range, vertical_range);
		ChunkManager::setSeed(seed);
		ChunkManager::setWorldDirectory(world_directory);
		ChunkManager::setCubicChunks(cubic_chunks);
		ChunkMeshBuilder::setChunks(ChunkManager::getChunks());
		Camera::setPosition(player->getPos() + glm::dvec3(0.0, 0.5, 0.0));
	}

	void exit() {
		ChunkManager::flushChunks();
	}

	void update(double delta_time) {
		for (auto& entity : entities) {
			switch (entity->getType()) {
			case Entity::Type::NONE:
				// TEST
				glm::vec3 distance = player->getPos() - entity->getPos();
				glm::vec3 direction = glm::vec3(0.0f);
				if (glm::length(distance) != 0.0f) {
					direction = distance / glm::length(distance);
				}
				entity->move(direction);
				entity->update(delta_time);
				break;
			case Entity::Type::PLAYER:
				std::dynamic_pointer_cast<Player>(entity)->update(delta_time);
				break;
			}
		}
		ChunkManager::update(player->getPos());
	}

	std::shared_ptr<Player> getPlayer(){
		return player;
	}

	std::vector<std::shared_ptr<Entity>>* getEntities() {
		return &entities;
	}

	void setHorizontalRange(int range) {
		horizontal_range = range;
		ChunkManager::setRange(horizontal_range, vertical_range);
	}

	void setVerticalRange(int range) {
		vertical_range = range;
		ChunkManager::setRange(horizontal_range, vertical_range);
	}

	void setLimit(glm::ivec3 start_limit, glm::ivec3 end_limit) {
		World::start_limit = start_limit;
		World::end_limit = end_limit;
		ChunkManager::setLimit(start_limit, end_limit);
	}

	std::pair<glm::ivec3, glm::ivec3> getLimit() {
		return std::pair<glm::ivec3, glm::ivec3>(start_limit, end_limit);
	}

	glm::ivec3 getRange() {
		return glm::ivec3(horizontal_range, vertical_range, horizontal_range);
	}
}