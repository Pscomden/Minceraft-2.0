#pragma once

#include "../Include.h"

class Rect {
public:
	Rect(glm::vec3 pos) : pos(pos), size(glm::vec3(1.0f))
	{

	}

	Rect(glm::vec3 pos, glm::vec3 size) : pos(pos), size(size)
	{

	}

	void changePos(glm::vec3 pos) {
		this->pos = pos;
	}

	void changeSize(glm::vec3 size) {
		this->size = size;
	}

    //https://gamedev.stackexchange.com/questions/60505/how-to-check-for-cube-collisions
	bool rectIntersects(const Rect& other) const {
        return (abs(pos.x - other.pos.x) < size.x / 2.0f + other.size.x / 2.0f &&
            abs(pos.y - other.pos.y) < size.y / 2.0f + other.size.y / 2.0f &&
            abs(pos.z - other.pos.z) < size.z / 2.0f + other.size.z / 2.0f);
	}
	glm::vec3 pos;
	glm::vec3 size;
};