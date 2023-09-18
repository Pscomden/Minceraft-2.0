#pragma once

#include "../Include.h"
#include "Block.h"
#include "../Constants.h"
#include "../Debug/Console.h"
#include "../Render/Mesh/ChunkMesh.h"

struct Chunk {

	enum class State : short {
		NEW, // fresh out of oven
		GENERATED, // all blocks no mesh
		MESH, // inner or all vertices generated, no buffer
		BUFFERS, // arbitrary vertices generated, buffer
		DELETING // to delete existing buffer, already serialized
	};

	Chunk() : Chunk(glm::ivec3(0)) {}

	Chunk(int x, int y, int z) : Chunk(glm::ivec3(x, y, z)) {}

	Chunk(glm::ivec3 pos) {
		this->pos = pos;
		state = State::NEW;
		find_adj = false;
		is_modified = false;
	}

	~Chunk() {
		mesh.clear();
	}

	void clearMesh() {
		mesh.vertices.clear();
		mesh.indices.clear();
		for (int i = 0; i < 6; i++) {
			generated_edge[i] = false;
		}
	}

	bool operator==(std::shared_ptr<Chunk> other) {
		return other->pos == pos;
	}

	bool operator==(glm::ivec3 other) {
		return other == pos;
	}

	std::string toString() {
		return std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z);
	}
	
	glm::ivec3 pos;
	Block blocks[pc::c_length][pc::c_height][pc::c_width];
	ChunkMesh mesh;
	bool is_modified;
	State state;
	std::mutex lock;
	// a chunk could generate its edge, and an adjacent one could come back
	// and accidently add another redundant edge
	bool generated_edge[6] = {false, false, false, false, false, false};
	bool find_adj;
};