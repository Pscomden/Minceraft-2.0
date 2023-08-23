#include "Ray.h"

Ray::Ray(glm::vec3 pos, float max_distance, float pitch, float yaw)
	: max_distance(max_distance)
{
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction = glm::normalize(direction);
	this->pos = pos;
	this->prev_pos = pos;
	cur_distance = 0.0f;
}

void Ray::step(float distance) {
	//std::cout << cur_distance << " " << max_distance << "\n";
	prev_pos = pos;
	if (cur_distance < max_distance) {
		pos += direction * distance;
		cur_distance += distance;
	}
}

glm::ivec3 Ray::blockPos() {
	return glm::floor(pos);
}

pc::CardinalDirection Ray::blockFace() {
	// Assumes blockPos(prev_pos) != blockPos(pos)
	// which means blockFace() must be called after blockPos() returns
	// a solid block from somewhere else.
	glm::ivec3 direction = glm::floor(prev_pos) - glm::floor(pos);
	if (direction.x < 0) {
		return pc::CardinalDirection::LEFT;
	}
	else if (direction.x > 0) {
		return pc::CardinalDirection::RIGHT;
	}
	else if (direction.y < 0) {
		return pc::CardinalDirection::DOWN;
	}
	else if (direction.y > 0) {
		return pc::CardinalDirection::UP;
	}
	else if (direction.z < 0) {
		return pc::CardinalDirection::BACKWARD;
	}
	else if (direction.z > 0) {
		return pc::CardinalDirection::FORWARD;
	}
}

bool Ray::calcTwoPositions(std::array<glm::ivec3, 2>& blocks) {
	static const float distance = 0.1f;
	glm::ivec3 block_pos = glm::floor(pos);
	while (cur_distance < max_distance) {
		step(distance);
		if ((glm::ivec3)glm::floor(pos) != block_pos) {
			block_pos = glm::floor(pos);
			Block* block = ChunkManager::getBlock(block_pos);
			if (block != nullptr && block->id != 0) {
				blocks[0] = block_pos;
				// step back
				pos -= direction * distance;
				blocks[1] = glm::floor(pos);
				return true;
			}
		}
	}
	blocks = { glm::floor(pos), glm::floor(pos) };
	return false;
}

bool Ray::isMoving() {
	return cur_distance < max_distance;
}
