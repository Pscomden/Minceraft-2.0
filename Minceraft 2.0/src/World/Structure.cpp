#include "Structure.h"

Structure::Structure() {
	start = glm::ivec3(0);
	end = glm::ivec3(0);
	// for now, assume start chunk exists
	num_existing_chunks = 1;
}

Structure::~Structure() {
	delete[] blocks;
}

void Structure::setRandomTree(glm::ivec3 pos, int seed) {
	static const int RADIUS = 2;
	start = pos - glm::ivec3(RADIUS, 0, RADIUS);
	int height = (rand() % 4) + 5;
	end = pos + glm::ivec3(RADIUS, height + RADIUS, RADIUS);
	size = end - start + glm::ivec3(1);
	makeChunkOverlaps();
	blocks = new Block[size.x * size.y * size.z];
	// trunk
	glm::ivec3 index = glm::ivec3(size.x / 2, 0, size.z / 2);
	for (index.y = 0; index.y < height; index.y++) {
		blocks[Structure::index(index)].id = 4;
	}
	for (index.x = 0; index.x < (2 * RADIUS) + 1; index.x++) {
		for (index.y = size.y - 1; index.y > (size.y - 1) - ((2 * RADIUS) + 1); index.y--) {
			for (index.z = 0; index.z < (2 * RADIUS) + 1; index.z++) {
				//std::cout << index.x << " " << index.y << " " << index.z << "\n";
				if (blocks[Structure::index(index)].id == 0) {
					blocks[Structure::index(index)].id = 5;
				}
			}
		}
	}
}

bool Structure::place(std::shared_ptr<Chunk> chunk) {
	glm::ivec3 chunk_start = chunk->pos * glm::ivec3(pc::c_length, pc::c_height, pc::c_width);
	glm::ivec3 chunk_end = chunk_start + glm::ivec3(pc::c_length, pc::c_height, pc::c_width);
	glm::ivec3 index = glm::ivec3(0);
	glm::ivec3 chunk_index{};
	for (index.x = 0; index.x < size.x; index.x++) {
		chunk_index.x = (start.x + index.x);
		if (chunk_index.x >= chunk_start.x && chunk_index.x < chunk_end.x) {
			for (index.y = 0; index.y < size.y; index.y++) {
				chunk_index.y = (start.y + index.y);
				if (chunk_index.y >= chunk_start.y && chunk_index.y < chunk_end.y) {
					for (index.z = 0; index.z < size.z; index.z++) {
						chunk_index.z = (start.z + index.z);
						if (chunk_index.z >= chunk_start.z && chunk_index.z < chunk_end.z) {
							chunk_index = ChunkManager::chunkRelative(chunk_index);
							if (blocks[Structure::index(index)].id != 0) {
								if (chunk->blocks[chunk_index.x][chunk_index.y][chunk_index.z].id == 0) {
									chunk->blocks[chunk_index.x][chunk_index.y][chunk_index.z] =
										blocks[Structure::index(index)];
								}
								else {

								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool Structure::place(glm::ivec3 chunk_pos) {

	return false;
}

void Structure::addChunkCount() {
	num_existing_chunks++;
}

bool Structure::decChunkCount() {
	num_existing_chunks--;

	return num_existing_chunks < 1;
}

std::vector<glm::ivec3> Structure::getChunkOverlaps() {
	return chunk_overlaps;
}

int Structure::index(glm::ivec3 index) {
	return (index.x * size.y * size.z) + (index.y * size.z) + index.z;
}

void Structure::makeChunkOverlaps() {
	glm::ivec3 chunk_start = ChunkManager::posToChunk(start);
	glm::ivec3 chunk_end = ChunkManager::posToChunk(end);
	for (int x = chunk_start.x; x <= chunk_end.x; x++) {
		for (int y = chunk_start.y; y <= chunk_end.y; y++) {
			for (int z = chunk_start.z; z <= chunk_end.z; z++) {
				chunk_overlaps.push_back(glm::ivec3(x, y, z));
			}
		}
	}
}
