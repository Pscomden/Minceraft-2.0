#pragma once

#include "../../Include.h"
#include "Mesh.h"

struct BasicVertex {
	BasicVertex(float x, float y, float u, float v) 
		: x(x), y(y), u(u), v(v)
	{

	}
	float x, y;
	float u, v;
};

class BasicMesh : public Mesh<BasicVertex> {
public:
	~BasicMesh() {
		Mesh::~Mesh();
	}
	void addRect(glm::vec2 pos, glm::vec2 size, std::array<glm::vec2, 2> uv);
	void generateBuffers();
};