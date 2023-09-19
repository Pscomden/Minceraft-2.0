#pragma once

#include "../Include.h"
#include "Chunk.h"
#include "WorldBuilder.h"
#include "../Debug/Console.h"
#include "../Render/ChunkMeshBuilder.h"
#include "../ThreadPool.h"

// https://github.com/Hopson97/MineCraft-One-Week-Challenge/blob/master/Source/Maths/Vector2XZ.h

namespace ChunkManager {
	bool init();
	void update(glm::ivec3 player_pos);
	void setRange(int horizontal_range, int vertical_range);
	void setLimit(glm::ivec3 start_limit, glm::ivec3 end_limit);
	bool inWorldLimit(glm::ivec3 chunk_pos);
	void setSeed(int seed);
	void setWorldDirectory(std::string world_directory);
	void setCubicChunks(bool cubic_chunks);
	glm::ivec3 posToChunk(glm::ivec3 pos);
	glm::ivec3 chunkRelative(glm::ivec3 pos);
	std::shared_ptr<Chunk> getChunk(glm::ivec3 pos);
	std::array<std::shared_ptr<Chunk>, 6> getAdjChunks(glm::ivec3 pos);
	Block* getBlock(glm::ivec3 pos);
	bool chunkExists(glm::ivec3 pos);
	void findEmptyChunks(glm::vec3 center_pos);
	robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* getChunks();
	void serialize(std::shared_ptr<Chunk> chunk);
	bool deserialize(std::shared_ptr<Chunk> chunk);
	// TODO: add place and break block
	Block changeBlock(glm::ivec3 pos, int id);
	void flushChunks();
	bool deletingChunks();
	bool isUpdating();
}