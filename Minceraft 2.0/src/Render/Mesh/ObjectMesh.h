#pragma once

#include "../../Include.h"
#include "Mesh.h"

struct ObjectVertex {
	ObjectVertex(float x, float y, float z, float u, float v)
		: x(x), y(y), z(z), u(u), v(v)
	{

	}
	ObjectVertex(glm::vec3 pos, glm::vec2 uv)
		: x(pos.x), y(pos.y), z(pos.z), u(uv.x), v(uv.y)
	{

	}
	float x, y, z;
	float u, v;
};

class ObjectMesh : public Mesh<ObjectVertex> {
public:
	void addRect(glm::vec3 pos1, glm::vec3 pos2, std::array<glm::vec2, 2> uv);
	void addPrism(glm::vec3 pos, glm::vec3 size, std::array<std::array<glm::vec2, 2>, 6> uvs);
	void generateBuffers();
};