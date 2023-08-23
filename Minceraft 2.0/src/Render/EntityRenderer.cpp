#include "EntityRenderer.h"

namespace EntityRenderer {

	static std::vector<std::shared_ptr<Entity>>* entities;

	void setEntities(std::vector<std::shared_ptr<Entity>>* entities) {
		EntityRenderer::entities = entities;
	}

	void render() {
		if (entities != nullptr) {
			for (auto& entity : *entities) {
				if (entity->getType() != Entity::Type::PLAYER) {
					ObjectMesh mesh;
					std::array<std::array<glm::vec2, 2>, 6> uvs = BlockTypes::getBlockUVs(3);
					mesh.addPrism(entity->getPos(), entity->getSize(), uvs);
					mesh.generateBuffers();
					mesh.render();
				}
			}
		}
	}
}
