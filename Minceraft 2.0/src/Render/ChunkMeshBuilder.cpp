#include "ChunkMeshBuilder.h"

namespace ChunkMeshBuilder {

	static robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks;
	static constexpr float face_size = 1.0f;

	void addFace(std::shared_ptr<Chunk> chunk, glm::ivec3 pos, std::array<glm::vec2, 2> uvs, pc::CardinalDirection face) {
		
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
		
		switch (face) {
		case pc::CardinalDirection::FORWARD:
		{
			float front[] = {
				neg_x, neg_y, neg_z, uvs[0].x, uvs[1].y, north_light, // bottom left
				pos_x, neg_y, neg_z, uvs[1].x, uvs[1].y, north_light, // bottom right
				pos_x, pos_y, neg_z, uvs[1].x, uvs[0].y, north_light, // top right
				neg_x, pos_y, neg_z, uvs[0].x, uvs[0].y, north_light  // top left
			};
			num_vertices = chunk->mesh.vertices.size() / 6;
			int front_indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			chunk->mesh.vertices.insert(chunk->mesh.vertices.begin(), std::begin(front), std::end(front));
			chunk->mesh.indices.insert(chunk->mesh.indices.begin(), std::begin(front_indices), std::end(front_indices));
		}
			break;
		case pc::CardinalDirection::BACKWARD:
		{
			float back[] = {
				pos_x, neg_y, pos_z, uvs[0].x, uvs[1].y, north_light, // bottom left
				neg_x, neg_y, pos_z, uvs[1].x, uvs[1].y, north_light, // bottom right
				neg_x, pos_y, pos_z, uvs[1].x, uvs[0].y, north_light, // top right
				pos_x, pos_y, pos_z, uvs[0].x, uvs[0].y, north_light  // top left
			};
			num_vertices = chunk->mesh.vertices.size() / 6;
			int back_indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			chunk->mesh.vertices.insert(chunk->mesh.vertices.begin(), std::begin(back), std::end(back));
			chunk->mesh.indices.insert(chunk->mesh.indices.begin(), std::begin(back_indices), std::end(back_indices));
		}
			break;
		case pc::CardinalDirection::LEFT:
		{
			float left[] = {
				neg_x, neg_y, pos_z, uvs[0].x, uvs[1].y, west_light, // bottom left
				neg_x, neg_y, neg_z, uvs[1].x, uvs[1].y, west_light, // bottom right
				neg_x, pos_y, neg_z, uvs[1].x, uvs[0].y, west_light, // top right
				neg_x, pos_y, pos_z, uvs[0].x, uvs[0].y, west_light  // top left
			};
			num_vertices = chunk->mesh.vertices.size() / 6;
			int left_indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			chunk->mesh.vertices.insert(chunk->mesh.vertices.begin(), std::begin(left), std::end(left));
			chunk->mesh.indices.insert(chunk->mesh.indices.begin(), std::begin(left_indices), std::end(left_indices));
		}
			break;
		case pc::CardinalDirection::RIGHT:
		{
			float right[] = {
				pos_x, neg_y, neg_z, uvs[0].x, uvs[1].y, west_light, // bottom left
				pos_x, neg_y, pos_z, uvs[1].x, uvs[1].y, west_light, // bottom right
				pos_x, pos_y, pos_z, uvs[1].x, uvs[0].y, west_light, // top right
				pos_x, pos_y, neg_z, uvs[0].x, uvs[0].y, west_light // top left
			};
			num_vertices = chunk->mesh.vertices.size() / 6;
			int right_indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			chunk->mesh.vertices.insert(chunk->mesh.vertices.begin(), std::begin(right), std::end(right));
			chunk->mesh.indices.insert(chunk->mesh.indices.begin(), std::begin(right_indices), std::end(right_indices));
		}
			break;
		case pc::CardinalDirection::UP:
		{
			float top[] = {
				neg_x, pos_y, neg_z, uvs[0].x, uvs[1].y, 1.0f, // bottom left
				pos_x, pos_y, neg_z, uvs[1].x, uvs[1].y, 1.0f, // bottom right
				pos_x, pos_y, pos_z, uvs[1].x, uvs[0].y, 1.0f, // top right
				neg_x, pos_y, pos_z, uvs[0].x, uvs[0].y, 1.0f // top left
			};
			num_vertices = chunk->mesh.vertices.size() / 6;
			int top_indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			chunk->mesh.vertices.insert(chunk->mesh.vertices.begin(), std::begin(top), std::end(top));
			chunk->mesh.indices.insert(chunk->mesh.indices.begin(), std::begin(top_indices), std::end(top_indices));
		}
			break;
		case pc::CardinalDirection::DOWN:
		{
			float bottom[] = {
				neg_x, neg_y, pos_z, uvs[0].x, uvs[1].y, bottom_light, // bottom left
				pos_x, neg_y, pos_z, uvs[1].x, uvs[1].y, bottom_light, // bottom right
				pos_x, neg_y, neg_z, uvs[1].x, uvs[0].y, bottom_light, // top right
				neg_x, neg_y, neg_z, uvs[0].x, uvs[0].y, bottom_light  // top left
			};
			num_vertices = chunk->mesh.vertices.size() / 6;
			int bottom_indices[] = {
				num_vertices, num_vertices + 1, num_vertices + 2,
				num_vertices, num_vertices + 2, num_vertices + 3
			};
			chunk->mesh.vertices.insert(chunk->mesh.vertices.begin(), std::begin(bottom), std::end(bottom));
			chunk->mesh.indices.insert(chunk->mesh.indices.begin(), std::begin(bottom_indices), std::end(bottom_indices));
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
			offset[2] = pc::c_length;
			break;
		case pc::CardinalDirection::UP:
			max_i = pc::c_length;
			max_j = pc::c_width;
			offset[2] = pc::c_height;
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
					if (chunk->blocks[i][j][0].id != 0
						&& adj_chunk->blocks[i][j][pc::c_width - 1].id == 0)
					{
						addFace(chunk, glm::vec3(i, j, 0), block_uvs[0], pc::CardinalDirection::FORWARD);
					}
				}
					break;
				case pc::CardinalDirection::BACKWARD:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][j][pc::c_width - 1].id);
					if (chunk->blocks[i][j][pc::c_width - 1].id != 0
						&& adj_chunk->blocks[i][j][0].id == 0)
					{
						addFace(chunk, glm::vec3(i, j, pc::c_width - 1), block_uvs[1], pc::CardinalDirection::BACKWARD);
					}
				}
					break;
				case pc::CardinalDirection::LEFT:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[0][j][i].id);
					if (chunk->blocks[0][j][i].id != 0
						&& adj_chunk->blocks[pc::c_length - 1][j][i].id == 0)
					{
						addFace(chunk, glm::vec3(0, j, i), block_uvs[2], pc::CardinalDirection::LEFT);
					}
				}
					break;
				case pc::CardinalDirection::RIGHT:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[pc::c_length - 1][j][i].id);
					if (chunk->blocks[pc::c_length - 1][j][i].id != 0
						&& adj_chunk->blocks[0][j][i].id == 0)
					{
						addFace(chunk, glm::vec3(pc::c_length - 1, j, i), block_uvs[3], pc::CardinalDirection::RIGHT);
					}
				}
					break;
				case pc::CardinalDirection::UP:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][pc::c_height - 1][j].id);
					if (chunk->blocks[i][pc::c_height - 1][j].id != 0
						&& adj_chunk->blocks[i][0][j].id == 0)
					{
						addFace(chunk, glm::vec3(i, pc::c_height - 1, j), block_uvs[4], pc::CardinalDirection::UP);
					}
				}
					break;
				case pc::CardinalDirection::DOWN:
				{
					std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[i][0][j].id);
					if (chunk->blocks[i][0][j].id != 0
						&& adj_chunk->blocks[i][pc::c_height - 1][j].id == 0)
					{
						addFace(chunk, glm::vec3(i, 0, j), block_uvs[5], pc::CardinalDirection::DOWN);
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
		ChunkMesh* mesh = &chunk->mesh;
		glm::ivec3 chunk_pos = { chunk->pos.x * pc::c_length, chunk->pos.y * pc::c_height, chunk->pos.z * pc::c_width };
		bool full = true;
		bool prev_full = true;
		for (int y = 0; y < pc::c_height; y++) {
			prev_full = full;
			full = true;
			for (int x = 0; x < pc::c_length; x++) {
				for (int z = 0; z < pc::c_width; z++) {
					if (chunk->blocks[x][y][z].id != 0) {
						//glm::ivec3 pos = glm::ivec3(chunk_pos.x + x, chunk_pos.y + y, chunk_pos.z + z);
						glm::ivec3 pos = glm::ivec3(x, y, z);
						std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[x][y][z].id);
						// front
						if ((z > 0 && chunk->blocks[x][y][z - 1].id == 0)) {
							addFace(chunk, pos, block_uvs[0], pc::CardinalDirection::FORWARD);
						}
						// back
						if ((z < (pc::c_width - 1) && chunk->blocks[x][y][z + 1].id == 0)) {
							addFace(chunk, pos, block_uvs[1], pc::CardinalDirection::BACKWARD);
						}
						// left
						if ((x > 0 && chunk->blocks[x - 1][y][z].id == 0)) {
							addFace(chunk, pos, block_uvs[2], pc::CardinalDirection::LEFT);
						}
						// right
						if ((x < (pc::c_length - 1) && chunk->blocks[x + 1][y][z].id == 0)) {
							addFace(chunk, pos, block_uvs[3], pc::CardinalDirection::RIGHT);
						}
						// top
						if ((y < (pc::c_height - 1) && chunk->blocks[x][y + 1][z].id == 0)) {
							addFace(chunk, pos, block_uvs[4], pc::CardinalDirection::UP);
						}
						// bottom
						if (!prev_full && (y > 0 && chunk->blocks[x][y - 1][z].id == 0)) {
							addFace(chunk, pos, block_uvs[5], pc::CardinalDirection::DOWN);
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
		//chunk->state = Chunk::State::MESH;
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

	void buildMeshEdges(std::shared_ptr<Chunk> chunk, bool gen_adj_edges) {
		
	}

	void regenChunk(std::shared_ptr<Chunk> chunk, bool gen_adj_edges) {
		ChunkMeshBuilder::buildChunkMesh(chunk);
		buildMeshEdges(chunk, gen_adj_edges);
	}

	void setChunks(robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks) {
		ChunkMeshBuilder::chunks = chunks;
	}

	void addSingleFace(std::shared_ptr<Chunk> chunk, glm::ivec3 indices, pc::CardinalDirection edge) {
		std::array<std::array<glm::vec2, 2>, 6> block_uvs = BlockTypes::getBlockUVs(chunk->blocks[indices.x][indices.y][indices.z].id);
		// TODO: organize cardinal direction and make this block_uvs[edge]
		switch (edge) {
		case pc::CardinalDirection::FORWARD:
			addFace(chunk, indices, block_uvs[0], pc::CardinalDirection::FORWARD);
			break;
		case pc::CardinalDirection::BACKWARD:
			addFace(chunk, indices, block_uvs[1], pc::CardinalDirection::BACKWARD);
			break;
		case pc::CardinalDirection::LEFT:
			addFace(chunk, indices, block_uvs[2], pc::CardinalDirection::LEFT);
			break;
		case pc::CardinalDirection::RIGHT:
			addFace(chunk, indices, block_uvs[3], pc::CardinalDirection::RIGHT);
			break;
		case pc::CardinalDirection::UP:
			addFace(chunk, indices, block_uvs[4], pc::CardinalDirection::UP);
			break;
		case pc::CardinalDirection::DOWN:
			addFace(chunk, indices, block_uvs[5], pc::CardinalDirection::DOWN);
			break;
		}
	}
}