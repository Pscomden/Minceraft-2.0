#pragma once

#include "../Include.h"
#include "Block.h"
#include "Chunk.h"
#include "ChunkManager.h"

class Structure {
public:
	Structure();
	~Structure();
	void setRandomTree(glm::ivec3 pos, int seed);
	bool place(std::shared_ptr<Chunk> chunk);
	bool place(glm::ivec3 chunk_pos);
	void addChunkCount();
	bool decChunkCount();
	std::vector<glm::ivec3> getChunkOverlaps();
private:
	int index(glm::ivec3 index);
	void makeChunkOverlaps();

	glm::ivec3 start, end, size;
	Block* blocks;
	int num_existing_chunks; // number of existing chunks this structure is inside of
	std::vector<glm::ivec3> chunk_overlaps;
};