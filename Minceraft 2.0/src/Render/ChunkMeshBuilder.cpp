#include "ChunkMeshBuilder.h"

namespace ChunkMeshBuilder {

	static robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks;
	static constexpr float face_size = 1.0f;

	void addFace(std::shared_ptr<Chunk> chunk, glm::ivec3 pos, std::array<glm::vec2, 2> uvs, pc::CardinalDirection face, bool trans) {
		
		//trans = false;

		static constexpr float west_light = 0.95f;
		static constexpr float north_light = 0.9f;
		static constexpr float bottom_light = 0.85f;

		int num_vertices = 0;
		float neg_x = pos.x;
		float pos_x = pos.x + face_size;
		float neg_y = pos.y;
		float pos_y = pos.y + face_size;
		float neg_z = pos.z;
		float pos_z = pos.z + face_size;
		
		auto mesh = (trans) ? &chunk->trans_mesh : &chunk->mesh;

		switch (face) {
		case pc::CardinalDirection::FORWARD:
		{
			float vertices[] = {
				neg_x, neg_y, neg_z, uvs[0].x, uvs[1].y, north_light, // bottom left
				pos_x, neg_y, neg_z, uvs[1].x, uvs[1].y, north_light, // bottom right
				pos_x, pos_y, neg_z, uvs[1].x, uvs[0].y, north_light, // top right
				neg_x, pos_y, neg_z, uvs[0].x, uvs[0].y, north_light  // top left
			};
			num_vertices = mesh->vertices.size() / 6;
			int indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			mesh->vertices.insert(mesh->vertices.begin(), std::begin(vertices), std::end(vertices));
			mesh->indices.insert(mesh->indices.begin(), std::begin(indices), std::end(indices));
		}
			break;
		case pc::CardinalDirection::BACKWARD:
		{
			float vertices[] = {
				pos_x, neg_y, pos_z, uvs[0].x, uvs[1].y, north_light, // bottom left
				neg_x, neg_y, pos_z, uvs[1].x, uvs[1].y, north_light, // bottom right
				neg_x, pos_y, pos_z, uvs[1].x, uvs[0].y, north_light, // top right
				pos_x, pos_y, pos_z, uvs[0].x, uvs[0].y, north_light  // top left
			};
			num_vertices = mesh->vertices.size() / 6;
			int indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			mesh->vertices.insert(mesh->vertices.begin(), std::begin(vertices), std::end(vertices));
			mesh->indices.insert(mesh->indices.begin(), std::begin(indices), std::end(indices));
		}
			break;
		case pc::CardinalDirection::LEFT:
		{
			float vertices[] = {
				neg_x, neg_y, pos_z, uvs[0].x, uvs[1].y, west_light, // bottom left
				neg_x, neg_y, neg_z, uvs[1].x, uvs[1].y, west_light, // bottom right
				neg_x, pos_y, neg_z, uvs[1].x, uvs[0].y, west_light, // top right
				neg_x, pos_y, pos_z, uvs[0].x, uvs[0].y, west_light  // top left
			};
			num_vertices = mesh->vertices.size() / 6;
			int indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			mesh->vertices.insert(mesh->vertices.begin(), std::begin(vertices), std::end(vertices));
			mesh->indices.insert(mesh->indices.begin(), std::begin(indices), std::end(indices));
		}
			break;
		case pc::CardinalDirection::RIGHT:
		{
			float vertices[] = {
				pos_x, neg_y, neg_z, uvs[0].x, uvs[1].y, west_light, // bottom left
				pos_x, neg_y, pos_z, uvs[1].x, uvs[1].y, west_light, // bottom right
				pos_x, pos_y, pos_z, uvs[1].x, uvs[0].y, west_light, // top right
				pos_x, pos_y, neg_z, uvs[0].x, uvs[0].y, west_light // top left
			};
			num_vertices = mesh->vertices.size() / 6;
			int indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			mesh->vertices.insert(mesh->vertices.begin(), std::begin(vertices), std::end(vertices));
			mesh->indices.insert(mesh->indices.begin(), std::begin(indices), std::end(indices));
		}
			break;
		case pc::CardinalDirection::UP:
		{
			float vertices[] = {
				neg_x, pos_y, neg_z, uvs[0].x, uvs[1].y, 1.0f, // bottom left
				pos_x, pos_y, neg_z, uvs[1].x, uvs[1].y, 1.0f, // bottom right
				pos_x, pos_y, pos_z, uvs[1].x, uvs[0].y, 1.0f, // top right
				neg_x, pos_y, pos_z, uvs[0].x, uvs[0].y, 1.0f // top left
			};
			num_vertices = mesh->vertices.size() / 6;
			int indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			mesh->vertices.insert(mesh->vertices.begin(), std::begin(vertices), std::end(vertices));
			mesh->indices.insert(mesh->indices.begin(), std::begin(indices), std::end(indices));
		}
			break;
		case pc::CardinalDirection::DOWN:
		{
			float vertices[] = {
				neg_x, neg_y, pos_z, uvs[0].x, uvs[1].y, bottom_light, // bottom left
				pos_x, neg_y, pos_z, uvs[1].x, uvs[1].y, bottom_light, // bottom right
				pos_x, neg_y, neg_z, uvs[1].x, uvs[0].y, bottom_light, // top right
				neg_x, neg_y, neg_z, uvs[0].x, uvs[0].y, bottom_light  // top left
			};
			num_vertices = mesh->vertices.size() / 6;
			int indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			mesh->vertices.insert(mesh->vertices.begin(), std::begin(vertices), std::end(vertices));
			mesh->indices.insert(mesh->indices.begin(), std::begin(indices), std::end(indices));
		}
			break;
		}
	}

	void buildMeshEdge(std::shared_ptr<Chunk> chunk, std::shared_ptr<Chunk> adj_chunk, pc::CardinalDirection edge) {
		if (chunk == nullptr || chunk->state == Chunk::State::DELETING) {
			return;
		}
		if (chunk->generated_edge[(int)edge]) {
			return;
		}
		int max_i = 0;
		int max_j = 0;
		std::array<int, 3> offset{0, 0, 0};
		// TODO: change to have
		switch (edge) {
		case pc::CardinalDirection::FORWARD:
			max_i = pc::c_length;
			max_j = pc::c_height;
			break;
		case pc::CardinalDirection::BACKWARD:
			max_i = pc::c_length;
			max_j = pc::c_height;
			offset[2] = pc::c_width;
			break;
		case pc::CardinalDirection::LEFT:
			max_i = pc::c_width;
			max_j = pc::c_height;
			break;
		case pc::CardinalDirection::RIGHT:
			max_i = pc::c_width;
			max_j = pc::c_height;
			offset[0] = pc::c_length;
			break;
		case pc::CardinalDirection::UP:
			max_i = pc::c_length;
			max_j = pc::c_width;
			offset[1] = pc::c_height;
			break;
		case pc::CardinalDirection::DOWN:
			max_i = pc::c_length;
			max_j = pc::c_width;
			break;
		}
		for (int i = 0; i < max_i; i++) {
			for (int j = 0; j < max_j; j++) {
				switch (edge) {
				case pc::CardinalDirection::FORWARD:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][j][0].id);
					bool trans = BlockTypes::isTrans(chunk->blocks[i][j][0].id);
					if (chunk->blocks[i][j][0].id != 0
						&& BlockTypes::isTrans(adj_chunk->blocks[i][j][pc::c_width - 1].id)
						&& adj_chunk->blocks[i][j][pc::c_width - 1].id != chunk->blocks[i][j][0].id)
					{
						addFace(chunk, glm::vec3(i, j, 0), block_uvs[0], pc::CardinalDirection::FORWARD, trans);
					}
				}
					break;
				case pc::CardinalDirection::BACKWARD:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][j][pc::c_width - 1].id);
					bool trans = BlockTypes::isTrans(chunk->blocks[i][j][pc::c_width - 1].id);
					if (chunk->blocks[i][j][pc::c_width - 1].id != 0
						&& BlockTypes::isTrans(adj_chunk->blocks[i][j][0].id)
						&& adj_chunk->blocks[i][j][0].id != chunk->blocks[i][j][pc::c_width - 1].id)
					{
						addFace(chunk, glm::vec3(i, j, pc::c_width - 1), block_uvs[1], pc::CardinalDirection::BACKWARD, trans);
					}
				}
					break;
				case pc::CardinalDirection::LEFT:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[0][j][i].id);
					bool trans = BlockTypes::isTrans(chunk->blocks[0][j][i].id);
					if (chunk->blocks[0][j][i].id != 0
						&& BlockTypes::isTrans(adj_chunk->blocks[pc::c_length - 1][j][i].id)
						&& adj_chunk->blocks[pc::c_length - 1][j][i].id != chunk->blocks[0][j][i].id)
					{
						addFace(chunk, glm::vec3(0, j, i), block_uvs[2], pc::CardinalDirection::LEFT, trans);
					}
				}
					break;
				case pc::CardinalDirection::RIGHT:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[pc::c_length - 1][j][i].id);
					bool trans = BlockTypes::isTrans(chunk->blocks[pc::c_length - 1][j][i].id);
					if (chunk->blocks[pc::c_length - 1][j][i].id != 0
						&& BlockTypes::isTrans(adj_chunk->blocks[0][j][i].id)
						&& adj_chunk->blocks[0][j][i].id != chunk->blocks[pc::c_length - 1][j][i].id)
					{
						addFace(chunk, glm::vec3(pc::c_length - 1, j, i), block_uvs[3], pc::CardinalDirection::RIGHT, trans);
					}
				}
					break;
				case pc::CardinalDirection::UP:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][pc::c_height - 1][j].id);
					bool trans = BlockTypes::isTrans(chunk->blocks[i][pc::c_height - 1][j].id);
					if (chunk->blocks[i][pc::c_height - 1][j].id != 0
						&& BlockTypes::isTrans(adj_chunk->blocks[i][0][j].id)
						&& adj_chunk->blocks[i][0][j].id != chunk->blocks[i][pc::c_height - 1][j].id)
					{
						addFace(chunk, glm::vec3(i, pc::c_height - 1, j), block_uvs[4], pc::CardinalDirection::UP, trans);
					}
				}
					break;
				case pc::CardinalDirection::DOWN:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][0][j].id);
					bool trans = BlockTypes::isTrans(chunk->blocks[i][0][j].id);
					if (chunk->blocks[i][0][j].id != 0
						&& BlockTypes::isTrans(adj_chunk->blocks[i][pc::c_height - 1][j].id)
						&& adj_chunk->blocks[i][pc::c_height - 1][j].id != chunk->blocks[i][0][j].id)
					{
						addFace(chunk, glm::vec3(i, 0, j), block_uvs[5], pc::CardinalDirection::DOWN, trans);
					}
				}
					break;
				}
			}
		}
		chunk->generated_edge[(int)edge] = true;
	}

	void buildChunkMesh(std::shared_ptr<Chunk> chunk) {
		if (chunk == nullptr || chunk->state == Chunk::State::DELETING) {
			return;
		}
		glm::ivec3 chunk_pos = { chunk->pos.x * pc::c_length, chunk->pos.y * pc::c_height, chunk->pos.z * pc::c_width };
		bool full = true;
		bool prev_full = true;
		for (int y = 0; y < pc::c_height; y++) {
			prev_full = full;
			full = true;
			for (int x = 0; x < pc::c_length; x++) {
				for (int z = 0; z < pc::c_width; z++) {
					if (chunk->blocks[x][y][z].id != 0) {
						bool trans = BlockTypes::isTrans(chunk->blocks[x][y][z].id);
						//glm::ivec3 pos = glm::ivec3(chunk_pos.x + x, chunk_pos.y + y, chunk_pos.z + z);
						glm::ivec3 pos = glm::ivec3(x, y, z);
						std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[x][y][z].id);
						// front
						if ((z > 0 && BlockTypes::isTrans(chunk->blocks[x][y][z - 1].id)) &&
							chunk->blocks[x][y][z - 1].id != chunk->blocks[x][y][z].id) {
							addFace(chunk, pos, block_uvs[0], pc::CardinalDirection::FORWARD, trans);
						}
						// back
						if ((z < (pc::c_width - 1) && BlockTypes::isTrans(chunk->blocks[x][y][z + 1].id)) &&
							chunk->blocks[x][y][z + 1].id != chunk->blocks[x][y][z].id) {
							addFace(chunk, pos, block_uvs[1], pc::CardinalDirection::BACKWARD, trans);
						}
						// left
						if ((x > 0 && BlockTypes::isTrans(chunk->blocks[x - 1][y][z].id)) &&
							chunk->blocks[x - 1][y][z].id != chunk->blocks[x][y][z].id) {
							addFace(chunk, pos, block_uvs[2], pc::CardinalDirection::LEFT, trans);
						}
						// right
						if ((x < (pc::c_length - 1) && BlockTypes::isTrans(chunk->blocks[x + 1][y][z].id)) &&
							chunk->blocks[x + 1][y][z].id != chunk->blocks[x][y][z].id) {
							addFace(chunk, pos, block_uvs[3], pc::CardinalDirection::RIGHT, trans);
						}
						// top
						if ((y < (pc::c_height - 1) && BlockTypes::isTrans(chunk->blocks[x][y + 1][z].id)) &&
							chunk->blocks[x][y + 1][z].id != chunk->blocks[x][y][z].id) {
							addFace(chunk, pos, block_uvs[4], pc::CardinalDirection::UP, trans);
						}
						// bottom
						if (!prev_full && (y > 0 && BlockTypes::isTrans(chunk->blocks[x][y - 1][z].id)) &&
							chunk->blocks[x][y - 1][z].id != chunk->blocks[x][y][z].id) {
							addFace(chunk, pos, block_uvs[5], pc::CardinalDirection::DOWN, trans);
						}
					}
					else {
						full = false;
					}
				}
			}
		}
		for (int i = 0; i < 6; i++) {
			chunk->generated_edge[i] = false;
		}
	}

	std::shared_ptr<Chunk> getChunk(glm::ivec3 pos) {
		if (chunks->find(pos) != chunks->end()) {
			std::shared_ptr<Chunk> chunk = chunks->at(pos);
			if (chunk->state != Chunk::State::DELETING) {
				return chunk;
			}
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

	void buildMeshEdges(std::shared_ptr<Chunk> chunk) {
		auto adjacent_chunks = getAdjChunks(chunk->pos);
		// this may be the first time this chunk has generated any edges
		// or it was called by another chunk
		pc::CardinalDirection edges[] = {pc::CardinalDirection::LEFT, pc::CardinalDirection::RIGHT,
					pc::CardinalDirection::DOWN, pc::CardinalDirection::UP,
					pc::CardinalDirection::FORWARD, pc::CardinalDirection::BACKWARD };
		for (int i = 0; i < 6; i++) {
			if (!chunk->generated_edge[i] && adjacent_chunks[i] != nullptr && adjacent_chunks[i]->state != Chunk::State::NEW) {
				buildMeshEdge(chunk, adjacent_chunks[i], edges[i]);
			}
		}
	}
	
	void regenChunk(std::shared_ptr<Chunk> chunk) {
		ChunkMeshBuilder::buildChunkMesh(chunk);
		buildMeshEdges(chunk);
	}

	void setChunks(robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks) {
		ChunkMeshBuilder::chunks = chunks;
	}

	void addSingleFace(std::shared_ptr<Chunk> chunk, glm::ivec3 indices, pc::CardinalDirection edge) {
		std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[indices.x][indices.y][indices.z].id);
		bool trans = BlockTypes::isTrans(chunk->blocks[indices.x][indices.y][indices.z].id);
		// TODO: organize cardinal direction and make this block_uvs[edge]
		switch (edge) {
		case pc::CardinalDirection::FORWARD:
			addFace(chunk, indices, block_uvs[0], pc::CardinalDirection::FORWARD, trans);
			break;
		case pc::CardinalDirection::BACKWARD:
			addFace(chunk, indices, block_uvs[1], pc::CardinalDirection::BACKWARD, trans);
			break;
		case pc::CardinalDirection::LEFT:
			addFace(chunk, indices, block_uvs[2], pc::CardinalDirection::LEFT, trans);
			break;
		case pc::CardinalDirection::RIGHT:
			addFace(chunk, indices, block_uvs[3], pc::CardinalDirection::RIGHT, trans);
			break;
		case pc::CardinalDirection::UP:
			addFace(chunk, indices, block_uvs[4], pc::CardinalDirection::UP, trans);
			break;
		case pc::CardinalDirection::DOWN:
			addFace(chunk, indices, block_uvs[5], pc::CardinalDirection::DOWN, trans);
			break;
		}
	}
}