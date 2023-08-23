#pragma once

#include "../Include.h"
#include "Chunk.h"
#include "Structure.h"

namespace WorldBuilder {
	bool init();
	void buildChunk(std::shared_ptr<Chunk> chunk);
	void removeChunkStructPair(glm::ivec3 chunk_pos);
	void setSeed(int seed);
}