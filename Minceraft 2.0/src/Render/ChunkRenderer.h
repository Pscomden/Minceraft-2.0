#pragma once

#include "../Include.h"
#include "../Render/Mesh/ChunkMesh.h"
#include "../Window/Shader.h"
#include "../World/Chunk.h"
#include "../World/ChunkManager.h"
#include "../World/World.h"
#include "Frustrum.h"

namespace ChunkRenderer {
	void setChunks(robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks);
	void render(glm::vec3 cam);
	void printOrder();
	bool rendering();
}