#include "ChunkRenderer.h"

namespace ChunkRenderer {

	static robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks;
	static std::vector<std::pair<float, std::shared_ptr<Chunk>>> render_order;

	static bool is_rendering = false;

	void setChunks(robin_hood::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>* chunks) {
		ChunkRenderer::chunks = chunks;
	}

	void render(glm::vec3 cam) {
		// fog :)
		// the camera is used for calculating distance of point to camera
		float fogStart = (World::getRange().x * pc::c_length) / 2 - (pc::c_length);
		float fogEnd = (World::getRange().x * pc::c_length) / 2;

		//fogEnd = 100000000.0f;

		Shaders::worldShader()->setFloat("fogStart", fogStart);
		Shaders::worldShader()->setFloat("fogEnd", fogEnd);

		//static auto old_chunk_pos = glm::ivec3(0);
		//static int old_num_chunks = 0;
		//glm::ivec3 chunk_pos = ChunkManager::posToChunk(glm::round(cam));
		//if (chunk_pos != old_chunk_pos || old_num_chunks != chunks->size()) {
		//	old_chunk_pos = chunk_pos;
		//	old_num_chunks = chunks->size();
		//	render_order.clear();
		//	for (auto it = chunks->begin(); it != chunks->end(); it++) {
		//		float distance = glm::distance(cam, (glm::vec3)it->first * glm::vec3(pc::c_length, pc::c_height, pc::c_width));
		//		// find spot
		//		bool spot_found = false;
		//		for (int i = 0; i < render_order.size(); i++) {
		//			if (distance < render_order[i].first) {
		//				spot_found = true;
		//				render_order.insert(render_order.begin() + i, std::pair<float, std::shared_ptr<Chunk>>(distance, it->second));
		//				break;
		//			}
		//		}
		//		if (!spot_found) {
		//			render_order.push_back(std::pair<float, std::shared_ptr<Chunk>>(distance, it->second));
		//		}
		//	}
		//}

		//is_rendering = true;
		//for (auto& chunk : render_order) {
		//	if (chunk.second == nullptr) {
		//		std::cout << "FUCKFUCKFUCK " << chunk.second->pos.x << " " << chunk.second->pos.z << "\n";
		//		//assert(false);
		//		continue;
		//	}
		//	if (chunk.second->state != Chunk::State::DELETING) {
		//		if (chunk.second->mesh.indices.size() > 0) {
		//			Shaders::worldShader()->setIvec3("chunk_pos", chunk.second->pos);
		//			chunk.second->mesh.render();
		//		}
		//	}
		//}
		for (auto& chunk : *chunks) {
			if (chunk.second == nullptr) {
				std::cout << "FUCKFUCKFUCK " << chunk.first.x << " " << chunk.first.z << "\n";
				//assert(false);
				continue;
			}
			if (chunk.second->state != Chunk::State::DELETING) {
				if (chunk.second->mesh.indices.size() > 0) {
					Shaders::worldShader()->setIvec3("chunk_pos", chunk.second->pos);
					chunk.second->mesh.render();
				}
			}
		}
		is_rendering = false;
	}

	void printOrder() {
		std::cout << "[";
		for (auto it = render_order.begin(); it != render_order.end(); it++) {
			std::cout << "(" << it->first << ", " << it->second->pos.x << " " << it->second->pos.y << " " << it->second->pos.z << "), ";
		}
		std::cout << "]\n";
	}

	bool rendering() {
		return is_rendering;
	}
}