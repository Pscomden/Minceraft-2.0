#pragma once

#include "../Include.h"
#include "../Constants.h"
#include "../World/Chunk.h"

namespace ChunkMeshBuilder {
	void buildMeshEdge(std::shared_ptr<Chunk> chunk, std::shared_ptr<Chunk> adj_chunk, pc::CardinalDirection edge);
	void buildChunkMesh(std::shared_ptr<Chunk> chunk, bool reset = true);
	void buildMeshEdges(std::shared_ptr<Chunk> chunk, bool gen_adj_edges);
	void regenChunk(std::shared_ptr<Chunk> chunk, bool gen_adj_edges);
	void setChunks(robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks);
	void addSingleFace(std::shared_ptr<Chunk> chunk, glm::ivec3 indices, pc::CardinalDirection edge);
}