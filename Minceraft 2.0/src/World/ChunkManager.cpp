#include "ChunkManager.h"

namespace ChunkManager {

	static robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> chunks; // normal chunks
	static robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> pool_chunks; // chunks being generated in the pool
	static std::mutex chunk_lock; // for deleting chunks on main thread
	static std::queue<glm::ivec3> delete_chunks; // chunks to be deleted
	static std::queue<glm::ivec3> empty_chunks; // chunks to be created
	static glm::ivec3 player_chunk_pos;
	static int horizontal_range;
	static int vertical_range;
	static glm::ivec3 start_limit;
	static glm::ivec3 end_limit;
	static int seed;
	static std::string world_directory;
	static bool first_frame;
	static bool deleting_chunks;
	static bool updating;
	static bool cubic_chunks;
	static int chunk_update_tick;

	bool poolChunkExists(glm::ivec3 pos);

	bool init() {
		horizontal_range = 0;
		vertical_range = 3;
		seed = 10;
		world_directory = "";
		first_frame = true;
		player_chunk_pos = glm::ivec3(0);
		chunk_update_tick = 10;
		
		deleting_chunks = false;
		updating = false;
		cubic_chunks = false;

		if (!WorldBuilder::init()) {
			return false;
		}

		WorldBuilder::setSeed(seed);
		findEmptyChunks(glm::ivec3(0, 0, 0));

		return true;
	}

	void deleteChunk(std::shared_ptr<Chunk> chunk) {
		if (chunk->is_modified) {
			serialize(chunk);
		}
		chunk->state = Chunk::State::DELETING;
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
		return !(player_pos.x < (chunk_pos.x - (horizontal_range / 2)) ||
			player_pos.x > (chunk_pos.x + (horizontal_range / 2)) ||
			player_pos.y < (chunk_pos.y - (vertical_range / 2)) ||
			player_pos.y > (chunk_pos.y + (vertical_range / 2)) ||
			player_pos.z < (chunk_pos.z - (horizontal_range / 2)) ||
			player_pos.z > (chunk_pos.z + (horizontal_range / 2)));
	}

	void deleteEmptyChunks(glm::ivec3 chunk_pos) {
		deleting_chunks = true;
		if (chunk_lock.try_lock()) {
			for (auto& pair : chunks) {
				if (pair.second == nullptr) {
					chunks.erase(pair.first);
				}
				else if (pair.second->state == Chunk::State::DELETING) {
					if (pair.second->lock.try_lock()) {
						pair.second->lock.unlock();
						chunks.erase(pair.first);
					}
				}
				else if (!inRange(pair.first, chunk_pos)) {
					std::shared_ptr<Chunk> chunk = pair.second;
					std::function<void()> func = [chunk]() {deleteChunk(chunk); };
					pool.QueueJob(func);
				}
			}
			chunk_lock.unlock();
		}
		deleting_chunks = false;
	}

	void generateChunkBuffers() {
		// only once per frame
		// building meshes on main thread is a lot of work
		// TODO: see if locks are necessary
		if (chunk_lock.try_lock()) {
			for (auto& pair : chunks) {
				if (pair.second) {
					if (pair.second->lock.try_lock()) {
						if (pair.second->state == Chunk::State::FULL_MESH) {
							pair.second->mesh.generateBuffers();
							pair.second->state = Chunk::State::BUFFERS;
						}
						else if (pair.second->state == Chunk::State::INNER_MESH) {
							ChunkMeshBuilder::buildMeshEdges(pair.second, true);
							pair.second->mesh.generateBuffers();
							pair.second->state = Chunk::State::BUFFERS;
						}
						pair.second->lock.unlock();
					}
				}
			}
			chunk_lock.unlock();
		}
	}

	void createChunk(std::shared_ptr<Chunk> chunk) {
		// Don't create a chunk when player moved out of range
		if (!inRange(chunk->pos, player_chunk_pos)) {
			chunk->state = Chunk::State::DELETING;
			return;
		}
		if (!deserialize(chunk)) {
			WorldBuilder::buildChunk(chunk);
		}
		ChunkMeshBuilder::buildChunkMesh(chunk, false);
	}

	void createChunks() {
		if (!empty_chunks.empty()) {
			glm::ivec3 new_chunk_pos = empty_chunks.front();
			if (!poolChunkExists(new_chunk_pos)) {
				std::shared_ptr<Chunk> new_chunk = std::shared_ptr<Chunk>(new Chunk(new_chunk_pos));
				//std::cout << "New Chunk at " << new_chunk->toString() << "\n";
				empty_chunks.pop();
				std::function<void()> func = [new_chunk]() {createChunk(new_chunk); };
				pool.QueueJob(func);
				pool_chunks.emplace(new_chunk->pos, new_chunk);
			}
		}
	}

	// https://stackoverflow.com/review/suggested-edits/1416384
	void findEmptyChunks(glm::ivec3 chunk_pos) {
		static glm::ivec3 old_pos = chunk_pos;
		if (chunk_pos != old_pos) {
			old_pos = chunk_pos;
		}
		else {
			return;
		}
		int X = horizontal_range;
		int Y = horizontal_range;
		int x, y, dx, dy;

		x = y = dx = 0;
		dy = -1;
		int t = std::max(X, Y);
		int maxI = t * t;
		for (int i = 0; i < maxI; i++) {
			if ((-X / 2 <= x) && (x <= X / 2) && (-Y / 2 <= y) && (y <= Y / 2)) {
				if (!cubic_chunks) {
					glm::ivec3 pos = glm::ivec3(x, 0, y) + chunk_pos;
					pos.y = 0;
					if (!chunkExists(pos) && !poolChunkExists(pos) && inWorldLimit(pos)) {
						empty_chunks.push(pos);
					}
				}
				else {
					int medium = chunk_pos.y;
					glm::ivec3 pos = glm::ivec3(x + chunk_pos.x, chunk_pos.y, y + chunk_pos.z);
					for (int j = 0; j < vertical_range; j++) {
						if (!chunkExists(pos) && !poolChunkExists(pos) && inWorldLimit(pos)) {
							empty_chunks.push(pos);
						}
						if (pos.y == chunk_pos.y) {
							pos.y++;
						}
						else {
							int dz = (medium - pos.y) * 2;
							if (pos.y < medium) {
								pos.y++;
							}
							pos.y += dz;
						}
					}
				}
			}
			if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
				t = dx;
				dx = -dy;
				dy = t;
			}
			x += dx;
			y += dy;
		}
	}

	void update(glm::ivec3 player_pos) {
		static int cur_tick = 1;
		bool is_update_tick = false;
		if (cur_tick == chunk_update_tick) {
			cur_tick = 1;
			is_update_tick = true;
		}
		cur_tick++;
		Console::num("Num jobs: ", pool.numJobs());
		if (!is_update_tick) {
			return;
		}
		player_chunk_pos = posToChunk(player_pos);
		// TODO: change to loop through all chunks once per frame
		generateChunkBuffers();
		deleteEmptyChunks(player_chunk_pos);
		findEmptyChunks(player_chunk_pos);
		createChunks();
		auto it = pool_chunks.begin();
		bool once = true;
		for (; it != pool_chunks.end(); it++) {
			if ((*it).second->state == Chunk::State::INNER_MESH && once) {
				chunks.emplace((*it).second->pos, (*it).second);
				pool_chunks.erase(it);
				// cap adding chunks to frame rate because otherwise there are border issues
				// TODO: solve this somehow!!! I'm too tired rn. 7/27 need to watch pbg do McDonald's Happy Meal Maker Toy
				once = false;
			}
			else if ((*it).second->state == Chunk::State::DELETING) {
				pool_chunks.erase(it);
			}
		}
	}

	void genWorld(glm::ivec3 player_pos) {
		glm::ivec3 chunk_pos = posToChunk(player_pos);
		chunk_pos.y = 0;
		glm::ivec3 pos{};
		for (pos.x = chunk_pos.x - 2; pos.x <= chunk_pos.x + 2; pos.x++) {
			for (pos.z = chunk_pos.z - 2; pos.z <= chunk_pos.z + 2; pos.z++) {
				if (!chunkExists(pos)) {
					std::shared_ptr<Chunk> new_chunk = std::shared_ptr<Chunk>(new Chunk(pos));
					if (!deserialize(new_chunk))
						WorldBuilder::buildChunk(new_chunk);
					chunks.emplace(pos, new_chunk);
				}
			}
		}
		for (auto& pair : chunks) {
			ChunkMeshBuilder::buildChunkMesh(pair.second);
			pair.second->mesh.generateBuffers();
			pair.second->state = Chunk::State::BUFFERS;
		}
	}

	void setRange(int horizontal_range, int vertical_range) {
		ChunkManager::horizontal_range = horizontal_range;
		ChunkManager::vertical_range = vertical_range;
	}

	void setLimit(glm::ivec3 start_limit, glm::ivec3 end_limit) {
		ChunkManager::start_limit = start_limit;
		ChunkManager::end_limit = end_limit;
		ChunkManager::deleteEmptyChunks(player_chunk_pos);
		ChunkManager::findEmptyChunks(player_chunk_pos);
		ChunkManager::createChunks();
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
		file_name << world_directory + "/chunks/" << chunk->pos.x << " " << chunk->pos.y << " " << chunk->pos.z << ".txt";
		FILE* file = fopen(file_name.str().c_str(), "wb");
		if (file) {
			fwrite(chunk->blocks, sizeof(short), static_cast<size_t>(pc::c_length * pc::c_height) * pc::c_width, file);
			fclose(file);
		}
		else {
			std::cout << "ERROR: couldn't save chunk to " << file_name.str() << "\n";
		}
	}

	bool deserialize(std::shared_ptr<Chunk> chunk) {
		std::stringstream file_name;
		file_name << world_directory + "/chunks/" << chunk->pos.x << " " << chunk->pos.y << " " << chunk->pos.z << ".txt";
		FILE* file = fopen(file_name.str().c_str(), "rb");
		if (file) {
			fread(chunk->blocks, sizeof(short), static_cast<size_t>(pc::c_length * pc::c_height) * pc::c_width, file);
			fclose(file);
			chunk->state = Chunk::State::GENERATED;
			return true;
		}
		return false;
	}

	void deleteChunk(glm::ivec3 pos) {
		if (chunks.find(pos) != chunks.end()) {
			delete_chunks.push(pos);
		}
	}
	
	std::shared_ptr<Chunk> getChunk(glm::ivec3 pos) {
		if (chunks.find(pos) != chunks.end()) {
			return chunks[pos];
		}
		return nullptr;
	}

	Block* getBlock(glm::ivec3 pos) {
		std::shared_ptr<Chunk> chunk = getChunk(posToChunk(pos));
		if (chunk == nullptr) {
			return nullptr;
		}
		glm::ivec3 indices = chunkRelative(pos);
		return &chunk->blocks[indices.x][indices.y][indices.z];
	}

	bool poolChunkExists(glm::ivec3 pos) {
		return pool_chunks.find(pos) != pool_chunks.end();
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

	std::array<std::shared_ptr<Chunk>, 4> findAdjacentChunks(std::shared_ptr<Chunk> chunk) {
		std::array<std::shared_ptr<Chunk>, 4> adj_chunks{ nullptr };
		if (chunk != nullptr) {
			adj_chunks[0] = getChunk(chunk->pos + glm::ivec3(-1, 0, 0));
			adj_chunks[1] = getChunk(chunk->pos + glm::ivec3(1, 0, 0));
			adj_chunks[2] = getChunk(chunk->pos + glm::ivec3(0, 0, -1));
			adj_chunks[3] = getChunk(chunk->pos + glm::ivec3(0, 0, 1));
		}
		return adj_chunks;
	}

	Block changeBlock(glm::ivec3 pos, int id) {
		Block block = Block(0);
		std::shared_ptr<Chunk> chunk = getChunk(posToChunk(pos));
		if (chunk != nullptr) {
			glm::ivec3 indices = chunkRelative(pos);
			block = chunk->blocks[indices.x][indices.y][indices.z];
			chunk->blocks[indices.x][indices.y][indices.z].id = id;
			chunk->clearMesh();
			ChunkMeshBuilder::buildMeshEdges(chunk, false);
			// don't want to regen buffers now
			chunk->state = Chunk::State::BUFFERS;
			pool.QueueJob([chunk]() {ChunkMeshBuilder::buildChunkMesh(chunk, false); });
			// on edge of chunk
			if (indices.x == 0) {
				std::shared_ptr<Chunk> adj_chunk = getChunk(chunk->pos + glm::ivec3(-1, 0, 0));
				if (adj_chunk && adj_chunk->blocks[pc::c_length - 1][indices.y][indices.z].id != 0) {
					// just add face
					if (id == 0) {
						ChunkMeshBuilder::addSingleFace(adj_chunk, glm::ivec3(pc::c_length - 1, indices.y, indices.z), pc::CardinalDirection::RIGHT);
						adj_chunk->mesh.generateBuffers();
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk, false);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {ChunkMeshBuilder::buildChunkMesh(adj_chunk, false); });
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
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk, false);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {ChunkMeshBuilder::buildChunkMesh(adj_chunk, false); });
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
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk, false);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {ChunkMeshBuilder::buildChunkMesh(adj_chunk, false); });
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
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk, false);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {ChunkMeshBuilder::buildChunkMesh(adj_chunk, false); });
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
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk, false);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {ChunkMeshBuilder::buildChunkMesh(adj_chunk, false); });
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
					}
					else {
						adj_chunk->clearMesh();
						ChunkMeshBuilder::buildMeshEdges(adj_chunk, false);
						adj_chunk->state = Chunk::State::BUFFERS;
						pool.QueueJob([adj_chunk]() {ChunkMeshBuilder::buildChunkMesh(adj_chunk, false); });
					}
				}
			}

			chunk->is_modified = true;
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
		while (!chunk_lock.try_lock()) {

		}
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
		chunk_lock.unlock();
	}

	bool deletingChunks() {
		return deleting_chunks;
	}

	bool isUpdating() {
		return updating;
	}
}