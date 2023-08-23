#pragma once

#include "../../Include.h"
#include "Mesh.h"
#include "../../Assets/BlockTypes.h"
#include "../../Constants.h"

struct ChunkVertex {
	ChunkVertex(float x, float y, float z, float u, float v, float light) :
		x(x), y(y), z(z), u(u), v(v), cardinal_light(light)
	{

	}
	float x, y, z;
	float u, v;
	float cardinal_light;
};

class ChunkMesh : public Mesh<float> {
public:
	~ChunkMesh() {
		Mesh::~Mesh();
	}
	void generateBuffers();
};