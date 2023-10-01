#pragma once

#include "../Include.h"
#include "ChunkManager.h"
#include "../Entity/Entity.h"
#include "../Entity/Player.h"

namespace World {
	void start(std::string world_directory);
	void exit();
	void update(double delta_time);
	std::shared_ptr<Player> getPlayer();
	std::vector<std::shared_ptr<Entity>>* getEntities();
	void setHorizontalRange(int range);
	void setVerticalRange(int range);
	void setLimit(glm::ivec3 start_limit, glm::ivec3 end_limit);
	std::pair<glm::ivec3, glm::ivec3> getLimit();
	glm::ivec3 getRange();
}