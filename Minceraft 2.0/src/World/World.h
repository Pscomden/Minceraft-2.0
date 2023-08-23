#pragma once

#include "../Include.h"
#include "ChunkManager.h"
#include "../Entity/Entity.h"
#include "../Entity/Player.h"

namespace World {
	void start(std::string world_directory);
	void exit();
	void update(float delta_time);
	std::shared_ptr<Player> getPlayer();
	std::vector<std::shared_ptr<Entity>>* getEntities();
	void setHorizontalRange(int range);
	void setVerticalRange(int range);
	glm::ivec3 getRange();
}