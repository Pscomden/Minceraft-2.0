#include "ChunkManager.h"

/*
----- Chunk Cycle -----

1. If the camera has left the local chunk, get list of empty chunks
2. Also get list of chunks to delete
Pick one function per frame or tick (for now)
	---
	1. Create empty chunk in the chunks list, set state to new
	2. Queue the chunk to generate blocks, state now generated
	---
	1. Set a single delete chunk to saving status and queue serialize
	2. Delete a chunk in deleting state
	---
	1. Scan for a chunk with generated state, queue to generate inner mesh vertices, status now mesh, set find_adj true
	2. Look for chunk with find_adj true and state mesh
	3. For each vertices wall the chunk hasn't made
	4. Set adj chunk that hasn't generated relevant wall and NOT NEW/DELETING find_adj to true
	5. Generate wall vertices, set generated_edge[side] to true
	6. Once all are generated, set find_adj to false
	---
	1. Generate a chunk's buffer on main thread, no pool
	2. Set status to buffers
	---

----- Pool Cycles -----

* Should never interact with chunks outside of the single provided!!! *
* Each should use chunk lock!!! *

Generating chunk blocks:
1. Try to deserialize chunk from file
2. Else generate fresh using world builder
3. Set state from new to generated

Generating chunk vertices:
1. Clear vertices
2. Build inner vertices
3. Set state to inner_mesh, find_adj to true

Deleting chunk:
1. Serialize if modified
2. Set state to deleting
*/
namespace ChunkManager {

	static robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> chunks; // normal chunks
	// May be deprecated
	static std::mutex chunk_lock; // for deleting chunks on main thread
	static std::queue<glm::ivec3> far_chunks; // chunks to be deleted
	static std::vector<glm::ivec3> empty_chunks; // chunks to be created
	static glm::ivec3 range;
	static glm::ivec3 start_limit;
	static glm::ivec3 end_limit;
	static int seed;
	static std::string world_directory;
	static bool first_frame;
	static bool deleting_chunks;
	static bool updating;
	static bool cubic_chunks;
	static int chunk_update_tick;
	static glm::ivec3 player_pos; // reset every update
	static const int CHUNKS_PER_ITER = 100;

	void pool_genChunkBlocks(std::shared_ptr<Chunk> chunk) {
		chunk->lock.lock();
		if (chunk->state == Chunk::State::NEW) {
			if (!deserialize(chunk)) {
				WorldBuilder::buildChunk(chunk);
			}
			chunk->state = Chunk::State::GENERATED;
		}
		chunk->lock.unlock();
	}

	void pool_genChunkMesh(std::shared_ptr<Chunk> chunk) {
		chunk->lock.lock();
		if (chunk->state != Chunk::State::DELETING) {
			ChunkMeshBuilder::buildChunkMesh(chunk);
			chunk->find_adj = true;
			chunk->state = Chunk::State::MESH;
		}
		chunk->lock.unlock();
	}

	void pool_deleteChunk(std::shared_ptr<Chunk> chunk) {
		chunk->lock.lock();
		if (chunk->state != Chunk::State::DELETING) {
			if (chunk->is_modified) {
				serialize(chunk);
			}
			chunk->state = Chunk::State::DELETING;
		}
		chunk->lock.unlock();
	}

	void main_genChunkBlocks() {
		if (!empty_chunks.empty()) {
			glm::ivec3 new_chunk_pos = empty_chunks.back();
			empty_chunks.pop_back();
			std::shared_ptr<Chunk> new_chunk = std::make_shared<Chunk>(new_chunk_pos);
			chunks.emplace(new_chunk_pos, new_chunk);
			std::function<void()> func = [new_chunk]() {pool_genChunkBlocks(new_chunk); };
			//Console::printVector("Queue create", new_chunk->pos);
			pool.QueueJob(func);
		}
	}

	void main_deleteChunk() {
		while (!far_chunks.empty()) {
			glm::ivec3 pos = far_chunks.front();
			far_chunks.pop();
			std::shared_ptr<Chunk> deleting_chunk = getChunk(pos);
			if (deleting_chunk != nullptr && deleting_chunk->state != Chunk::State::DELETING) {
				std::function<void()> func = [deleting_chunk]() {pool_deleteChunk(deleting_chunk); };
				//Console::printVector("Queue delete", deleting_chunk->pos);
				pool.QueueJob(func);
			}
		}
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			std::shared_ptr<Chunk> chunk = it->second;
			if (chunk == nullptr) {
				chunks.erase(chunk->pos);
			}
			if (chunk->state == Chunk::State::DELETING) {
				if (chunk->lock.try_lock()) {
					chunks.erase(chunk->pos);
					chunk->lock.unlock();
				}
			}
		}
	}

	void main_genChunkMesh() {
		int mesh_count = 0;
		const int max_mesh = 3;
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			std::shared_ptr<Chunk> chunk = it->second;
			// generate inner mesh
			if (chunk->state == Chunk::State::GENERATED) {
				if (chunk->lock.try_lock()) {
					std::function<void()> func = [chunk]() {pool_genChunkMesh(chunk); };
					//Console::printVector("Queue mesh", chunk->pos);
					pool.QueueJob(func);
					chunk->lock.unlock();
					mesh_count++;
					if (mesh_count == max_mesh) {
						break;
					}
				}
			}
			// generate outer mesh
			if (chunk->state == Chunk::State::MESH ||
				chunk->state == Chunk::State::BUFFERS
				&& chunk->find_adj)
			{
				if (chunk->lock.try_lock()) {
					ChunkMeshBuilder::buildMeshEdges(chunk);
					auto adj_chunks = getAdjChunks(chunk->pos);
					// to adj_chunk, chunk is opposite direction
					const int adj_key[6] = { 1, 0, 3, 2, 5, 4 };
					for (int i = 0; i < 6; i++) {
						if (adj_chunks[i] != nullptr &&
							adj_chunks[i]->state != Chunk::State::NEW &&
							adj_chunks[i]->state != Chunk::State::DELETING)
						{
							if (!adj_chunks[i]->generated_edge[adj_key[i]]) {
								adj_chunks[i]->find_adj = true;
							}
						}
					}
					chunk->find_adj = false;
					chunk->mesh.generateBuffers();
					chunk->trans_mesh.generateBuffers();
					chunk->state = Chunk::State::BUFFERS;
					chunk->lock.unlock();
				}
			}
		}
	}

	void main_genChunkBuffers() {
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			std::shared_ptr<Chunk> chunk = it->second;
			if (chunk->state == Chunk::State::MESH) {
				if (chunk->lock.try_lock()) {
					chunk->mesh.generateBuffers();
					chunk->trans_mesh.generateBuffers();
					chunk->state = Chunk::State::BUFFERS;
					chunk->lock.unlock();
				}
			}
		}
	}

	bool init() {
		seed = 10;
		world_directory = "./saves/World/chunks";
		first_frame = true;
		chunk_update_tick = 5;
		
		deleting_chunks = false;
		updating = false;
		cubic_chunks = false;

		if (!WorldBuilder::init()) {
			return false;
		}

		WorldBuilder::setSeed(seed);
		findEmptyChunks(glm::ivec3(0, 0, 0));
		std::filesystem::create_directory(world_directory);
		
		return true;
	}

	bool inWorldLimit(glm::ivec3 chunk_pos) {
		return (chunk_pos.x >= start_limit.x &&
				chunk_pos.y >= start_limit.y &&
				chunk_pos.z >= start_limit.z &&
				chunk_pos.x <= end_limit.x &&
				chunk_pos.y <= end_limit.y &&
				chunk_pos.z <= end_limit.z);
	}

	bool inRange(glm::ivec3 chunk_pos, glm::ivec3 player_pos) {
		return !(player_pos.x < (chunk_pos.x - (range.x / 2)) ||
			player_pos.x > (chunk_pos.x + (range.x / 2)) ||
			player_pos.y < (chunk_pos.y - (range.y / 2)) ||
			player_pos.y > (chunk_pos.y + (range.y / 2)) ||
			player_pos.z < (chunk_pos.z - (range.z / 2)) ||
			player_pos.z > (chunk_pos.z + (range.z / 2)));
	}

	// TODO: replace with finding all empty chunks linearly and sorting by distance
	// https://stackoverflow.com/review/suggested-edits/1416384
	void findEmptyChunks(glm::vec3 center_pos) {
		glm::ivec3 chunk_pos = posToChunk(center_pos);
		glm::ivec3 start = chunk_pos - (range / 2);
		glm::ivec3 end = chunk_pos + (range / 2);
		glm::ivec3 pos;
		empty_chunks.clear();
		std::vector<float> distances;
		for (pos.x = start.x; pos.x <= end.x; pos.x++) {
			//for (pos.y = start.y; pos.y <= end.y; pos.y++) {
			pos.y = 0;
				for (pos.z = start.z; pos.z <= end.z; pos.z++) {
					if (inWorldLimit(pos) && !chunkExists(pos)) {
						float distance = glm::distance(center_pos, (glm::vec3)(pos * glm::ivec3(pc::c_length, pc::c_height, pc::c_width) + glm::ivec3(pc::c_length, pc::c_height, pc::c_width) / 2));
						if (distance <= (range.x * pc::c_length / 2.0f) || true) {
							empty_chunks.push_back(pos);
							distances.push_back(distance);
						}
					}
				}
			//}
		}
		for (int i = empty_chunks.size() - 1; i >= 0; i--) {
			float min = distances[i];
			int index = i;
			for (int j = i; j >= 0; j--) {
				if (distances[j] < min) {
					min = distances[j];
					index = j;
				}
			}
			if (i != index) {
				float temp_d = distances[index];
				glm::ivec3 temp_e = empty_chunks[index];
				distances[index] = distances[i];
				empty_chunks[index] = empty_chunks[i];
				distances[i] = temp_d;
				empty_chunks[i] = temp_e;
			}
		}
	}

	void findFarChunks(glm::ivec3 center_pos) {
		far_chunks = std::queue<glm::ivec3>();
		glm::ivec3 center_chunk_pos = posToChunk(center_pos);
		for (auto it = chunks.begin(); it != chunks.end(); it++) {
			std::shared_ptr<Chunk> chunk = it->second;
			if (!inRange(chunk->pos, center_chunk_pos)) {
				far_chunks.push(chunk->pos);
			}
		}
	}

	void update(glm::ivec3 new_player_pos) {
		//std::cout << chunks.size() << "\n";
		player_pos = new_player_pos;
		// check if update tick!
		static int cur_tick = 1;
		bool is_update_tick = false;
		if (cur_tick > chunk_update_tick) {
			cur_tick = 1;
			is_update_tick = true;
		}
		cur_tick++;
		int jobs = pool.numJobs();
		if (jobs > 0) {
			Console::num("Num jobs:", jobs);
		}
		if (!is_update_tick) {
			return;
		}
		
		auto start_time = std::chrono::high_resolution_clock::now();

		// see if out of chunk, get empty/far chunks
		glm::ivec3 chunk_pos = posToChunk(player_pos);
		static glm::ivec3 old_pos = chunk_pos;
		if (chunk_pos != old_pos) {
			old_pos = chunk_pos;
			findEmptyChunks(player_pos);
			findFarChunks(player_pos);
			return;
		}

		static int update_func = 0;
		//std::cout << chunks.size() << "\n";
		switch (update_func) {
		case 0:
			main_genChunkBlocks();
			break;
		case 1:
			main_deleteChunk();
			break;
		case 2:
			main_genChunkMesh();
			break;
		case 3:
			main_genChunkBuffers();
		}
		update_func++;
		if (update_func == 4) {
			update_func = 0;
		}

		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
		auto delta = (double)((duration.count()) / 1000000.0);
		//std::cout << std::fixed << delta << "\n";
	}

	void setRange(int horizontal_range, int vertical_range) {
		ChunkManager::range = glm::ivec3(horizontal_range, vertical_range, horizontal_range);
		ChunkManager::findEmptyChunks(player_pos);
		ChunkManager::findFarChunks(player_pos);
	}

	void setLimit(glm::ivec3 start_limit, glm::ivec3 end_limit) {
		ChunkManager::start_limit = start_limit;
		ChunkManager::end_limit = end_limit;
		ChunkManager::findEmptyChunks(player_pos);
		ChunkManager::findFarChunks(player_pos);
	}

	void setSeed(int seed) {
		ChunkManager::seed = seed;
		WorldBuilder::setSeed(seed);
	}

	void setWorldDirectory(std::string world_directory) {
		ChunkManager::world_directory = world_directory;
	}

	void setCubicChunks(bool cubic_chunks) {
		ChunkManager::cubic_chunks = cubic_chunks;
	}

	void serialize(std::shared_ptr<Chunk> chunk) {
		std::stringstream file_name;
		file_name << world_directory + "chunks/" << chunk->pos.x << " " << chunk->pos.y << " " << chunk->pos.z << ".bin";
		FILE* file = fopen(file_name.str().c_str(), "wb");
		if (file) {
			fwrite(chunk->blocks, sizeof(Block), static_cast<size_t>(pc::c_length * pc::c_height * pc::c_width), file);
			fclose(file);
		}
		else {
			std::cout << "ERROR: couldn't save chunk to " << file_name.str() << "\n";
		}
	}

	bool deserialize(std::shared_ptr<Chunk> chunk) {
		std::stringstream file_name;
		file_name << world_directory + "chunks/" << chunk->pos.x << " " << chunk->pos.y << " " << chunk->pos.z << ".bin";
		FILE* file = fopen(file_name.str().c_str(), "rb");
		if (file) {
			fread(chunk->blocks, sizeof(Block), static_cast<size_t>(pc::c_length * pc::c_height * pc::c_width), file);
			fclose(file);
			chunk->state = Chunk::State::GENERATED;
			return true;
		}
		return false;
	}
	
	std::shared_ptr<Chunk> getChunk(glm::ivec3 pos) {
		if (chunks.find(pos) != chunks.end()) {
			return chunks[pos];
		}
		return nullptr;
	}

	std::array<std::shared_ptr<Chunk>, 6> getAdjChunks(glm::ivec3 pos) {
		std::array<std::shared_ptr<Chunk>, 6> adj_chunks;
		adj_chunks[0] = getChunk(pos + glm::ivec3(-1, 0, 0));
		adj_chunks[1] = getChunk(pos + glm::ivec3(1, 0, 0));
		adj_chunks[2] = getChunk(pos + glm::ivec3(0, -1, 0));
		adj_chunks[3] = getChunk(pos + glm::ivec3(0, 1, 0));
		adj_chunks[4] = getChunk(pos + glm::ivec3(0, 0, -1));
		adj_chunks[5] = getChunk(pos + glm::ivec3(0, 0, 1));

		return adj_chunks;
	}

	Block* getBlock(glm::ivec3 pos) {
		std::shared_ptr<Chunk> chunk = getChunk(posToChunk(pos));
		if (chunk == nullptr) {
			return nullptr;
		}
		glm::ivec3 indices = chunkRelative(pos);
		return &chunk->blocks[indices.x][indices.y][indices.z];
	}

	bool chunkExists(glm::ivec3 pos) {
		return chunks.find(pos) != chunks.end();
	}

	std::mutex* chunkLock() {
		return &chunk_lock;
	}

	robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* getChunks() {
		return &chunks;
	}

	Block changeBlock(glm::ivec3 pos, int id) {
		Block block = Block(0);
		std::shared_ptr<Chunk> chunk = getChunk(posToChunk(pos));
		if (chunk != nullptr) {
			glm::ivec3 indices = chunkRelative(pos);
			block = chunk->blocks[indices.x][indices.y][indices.z];
			chunk->blocks[indices.x][indices.y][indices.z].id = id;
			chunk->is_modified = true;
			chunk->clearMesh();

			ChunkMeshBuilder::buildMeshEdges(chunk);
			// don't want to regen buffers now
			chunk->state = Chunk::State::BUFFERS;
			pool.QueueJob([chunk]() {pool_genChunkMesh(chunk); });
			// on edge of chunk
			if (indices.x == 0) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(-1, 0, 0));
				if (adj_chunk && adj_chunk->blocks[pc::c_length - 1][indices.y][indices.z].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(pc::c_length - 1, indices.y, indices.z), pc::CardinalDirection::RIGHT);
						adj_chunk->mesh.generateBuffers();
						adj_chunk->trans_mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {pool_genChunkMesh(adj_chunk); });
					}
				}
			}
			else if (indices.x == pc::c_length - 1) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(1, 0, 0));
				if (adj_chunk && adj_chunk->blocks[0][indices.y][indices.z].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(0, indices.y, indices.z), pc::CardinalDirection::LEFT);
						adj_chunk->mesh.generateBuffers();
						adj_chunk->trans_mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {pool_genChunkMesh(adj_chunk); });
					}
				}
			}
			if (indices.y == 0) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(0, -1, 0));
				if (adj_chunk && adj_chunk->blocks[indices.x][pc::c_height - 1][indices.z].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(indices.x, pc::c_height - 1, indices.z), pc::CardinalDirection::UP);
						adj_chunk->mesh.generateBuffers();
						adj_chunk->trans_mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {pool_genChunkMesh(adj_chunk); });
					}
				}
			}
			else if (indices.y == pc::c_height - 1) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(0, 1, 0));
				if (adj_chunk && adj_chunk->blocks[indices.x][0][indices.z].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(indices.x, 0, indices.z), pc::CardinalDirection::DOWN);
						adj_chunk->mesh.generateBuffers();
						adj_chunk->trans_mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {pool_genChunkMesh(adj_chunk); });
					}
				}
			}
			if (indices.z == 0) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(0, 0, -1));
				if (adj_chunk && adj_chunk->blocks[indices.x][indices.y][pc::c_width - 1].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(indices.x, indices.y, pc::c_width - 1), pc::CardinalDirection::BACKWARD);
						adj_chunk->mesh.generateBuffers();
						adj_chunk->trans_mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {pool_genChunkMesh(adj_chunk); });
					}
				}
			}
			else if (indices.z == pc::c_width - 1) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(0, 0, 1));
				if (adj_chunk && adj_chunk->blocks[indices.x][indices.y][0].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(indices.x, indices.y, 0), pc::CardinalDirection::FORWARD);
						adj_chunk->mesh.generateBuffers();
						adj_chunk->trans_mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {pool_genChunkMesh(adj_chunk); });
					}
				}
			}
		}

		return block;
	}

	glm::ivec3 posToChunk(glm::ivec3 pos) {
		glm::ivec3 chunk_pos{};
		chunk_pos.x = floor((float)pos.x / pc::c_length);
		chunk_pos.y = floor((float)pos.y / pc::c_height);
		chunk_pos.z = floor((float)pos.z / pc::c_width);

		return chunk_pos;
	}
	
	glm::ivec3 chunkRelative(glm::ivec3 pos) {
		glm::ivec3 relative(
			pos.x % pc::c_length,
			pos.y % pc::c_height,
			pos.z % pc::c_width
		);
		if (relative.x < 0) {
			relative.x += pc::c_length;
		}
		if (relative.y < 0) {
			relative.y += pc::c_height;
		}
		if (relative.z < 0) {
			relative.z += pc::c_width;
		}
		return relative;
	}

	void flushChunks() {
		for (auto& pair : chunks) {
			if (pair.second != nullptr) {
				while (!pair.second->lock.try_lock()) {

				}
				if (pair.second->is_modified) {
					serialize(pair.second);
					//std::cout << pair.second.toString() << "\n";
				}
				pair.second->lock.unlock();
			}
		}
		chunks.clear();
	}

	bool deletingChunks() {
		return deleting_chunks;
	}

	bool isUpdating() {
		return updating;
	}
}