#include "DebugRenderer.h"

namespace DebugRenderer {

	static std::shared_ptr<Player> player;
	static bool rigid_bodies = false;
	static bool stats = false;
	static bool chunk_borders = false;
	static bool range_borders = false;

	void renderRigidBodies() {
		if (player != nullptr) {
			LineMesh mesh;
			mesh.addRect(Rectangle(player->getPos(), player->getSize()), glm::vec3(1.0f));
			mesh.generateBuffers();
			mesh.render();
		}
	}

	void renderChunkBorders() {
		LineMesh mesh;
		glm::ivec3 pos = ChunkManager::posToChunk(glm::round(player->getPos()));
		glm::ivec3 index{};
		glm::ivec3 chunk_size = glm::ivec3(pc::c_length, pc::c_height, pc::c_width);
		mesh.addRect(Rectangle(pos * chunk_size + (chunk_size / 2), chunk_size), glm::vec3(1.0f, 0.0f, 0.0f));
		for (index.x = pos.x - 1; index.x <= pos.x + 1; index.x++) {
			for (index.y = pos.y - 1; index.y <= pos.y + 1; index.y++) {
				for (index.z = pos.z - 1; index.z <= pos.z + 1; index.z++) {
					//mesh.addRect(Rectangle(index * chunk_size, chunk_size), glm::vec3(1.0f, 0.0f, 0.0f));
				}
			}
		}
		mesh.generateBuffers();
		mesh.render();
	}

	void renderRangeBorders() {
		//std::cout << "here\n";
		LineMesh mesh;
		glm::ivec3 pos = ChunkManager::posToChunk(glm::round(player->getPos()));
		glm::ivec3 index{};
		glm::ivec3 chunk_size = glm::ivec3(pc::c_length, pc::c_height, pc::c_width);
		glm::ivec3 range = World::getRange() * chunk_size;
		mesh.addRect(Rectangle(pos * chunk_size + (chunk_size / 2), range), glm::vec3(0.0f, 0.0f, 1.0f));
		mesh.generateBuffers();
		mesh.render();
	}

	void renderStats() {
		glm::ivec3 chunk_pos = ChunkManager::posToChunk(round(player->getPos()));
		glm::ivec3 relative = ChunkManager::chunkRelative(round(player->getPos()));
		std::shared_ptr<Chunk> chunk = ChunkManager::getChunk(chunk_pos);
		std::stringstream stats;
		stats << std::fixed << "Position: " << std::setprecision(5) << player->getPos().x << " " << std::setprecision(5) << player->getPos().y << " " << std::setprecision(5) << player->getPos().z <<
			" Chunk: " << chunk_pos.x << " " << chunk_pos.y << " " << chunk_pos.z <<
			" Relative: " << relative.x << " " << relative.y << " " << relative.z;
		TextRenderer::renderText(stats.str(), 32, Window::getSize().y - 32, 24.0f / 48.0f, glm::vec3(1.0f));
	}

	void renderCommand() {
		std::string command = '/' + Command::getInput();
		TextRenderer::renderText(command, 32, 32, 48.0f / 48.0f, glm::vec3(1.0f));
	}

	void render() {
		if (rigid_bodies)
			renderRigidBodies();
		if (chunk_borders)
			renderChunkBorders();
		if (range_borders)
			renderRangeBorders();
		if (stats)
			renderStats();
		if (Command::isOpen())
			renderCommand();
	}

	void toggleRigidBodies(){
		rigid_bodies = 1 - rigid_bodies;
	}

	void toggleStats() {
		stats = 1 - stats;
	}

	void toggleChunkBorders() {
		chunk_borders = 1 - chunk_borders;
	}

	void toggleRangeBorders() {
		range_borders = 1 - range_borders;
	}

	void setPlayer(std::shared_ptr<Player> player) {
		DebugRenderer::player = player;
	}
}