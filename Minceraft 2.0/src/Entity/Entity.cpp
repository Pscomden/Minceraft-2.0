#include "Entity.h"

Entity::Entity(glm::dvec3 pos) : rigid_body(glm::vec3(0.0f), glm::vec3(0.0f)) {
	this->pos = pos;
	size = glm::vec3(1.0f, 1.0f, 1.0f);
	direction = glm::vec3(0.0f);
	velocity = glm::vec3(0.0f);
	rigid_body.changePos(pos);
	rigid_body.changeSize(size);
	type = Type::NONE;
	input_jump = false;
	running = false;
}

void Entity::update(double delta_time) {
	
	checkCollision(velocity * delta_time);
}

void Entity::addPos(glm::dvec3 offset) {
	pos += offset;
}

void Entity::setPos(glm::dvec3 new_pos) {
	pos = new_pos;
}

void Entity::jump() {
	input_jump = true;
}

glm::dvec3& Entity::getPos() {
	return pos;
}

void Entity::run() {
	running = true;
}

glm::dvec3 Entity::getSize() {
	return size;
}

glm::dvec3 Entity::getVelocity() {
	return glm::vec3();
}

Entity::Type Entity::getType() {
	return type;
}

void Entity::move(glm::dvec3 direction) {
	this->direction += direction;
}

void Entity::checkCollision(glm::dvec3 distance) {
	on_ground = false;
	glm::dvec3 new_pos = pos + distance;
	rigid_body.changePos(new_pos);
	std::vector<Rect> cube_hitboxes;
	glm::ivec3 start = glm::round(new_pos - (size / 2.0) - 0.5);
	glm::ivec3 end = glm::round(new_pos + (size / 2.0) - 0.5);
	glm::ivec3 index{};
	for (index.x = start.x; index.x <= end.x; index.x++) {
		for (index.y = start.y; index.y <= end.y; index.y++) {
			for (index.z = start.z; index.z <= end.z; index.z++) {
				Block* block = ChunkManager::getBlock(index);
				if (block != nullptr && block->id > 0) {
					cube_hitboxes.push_back(Rect((glm::vec3)index + glm::vec3(0.5f)));
				}
			}
		}
	}
	if (cube_hitboxes.size() == 0) {
		pos = new_pos;
		return;
	}
	new_pos -= distance;
	// X
	new_pos.x += distance.x;
	rigid_body.changePos(new_pos);
	for (const Rect& rect : cube_hitboxes) {
		if (rigid_body.rectIntersects(rect)) {
			new_pos.x = pos.x;
			break;
		}
	}
	// Y
	new_pos.y += distance.y;
	rigid_body.changePos(new_pos);

	// Notes; this is true because top condition which is just vector + velocity stops on_ground
	on_ground = false;
	//std::cout << "start: " << on_ground << "\n";
	for (const Rect& rect : cube_hitboxes) {
		if (rigid_body.rectIntersects(rect)) {
			new_pos.y = pos.y;
			if (rect.pos.y < new_pos.y) {
				on_ground = true;
				//Console::printVector("caadr", pos);
			}
			break;
		}
	}
	//std::cout << "end: " << on_ground << "\n";
	// Z
	new_pos.z += distance.z;
	rigid_body.changePos(new_pos);
	for (const Rect& rect : cube_hitboxes) {
		if (rigid_body.rectIntersects(rect)) {
			new_pos.z = pos.z;
			break;
		}
	}
	
	pos = new_pos;
}