#include "UIRenderer.h"

namespace UIRenderer {

	static std::shared_ptr<Player> player;
	static glm::ivec2 window_size = glm::ivec2(0);

	void renderInventory() {
		BasicMesh inv_mesh;
		int size = player->getInventory()->getSlotSize();
		glm::ivec2 offset = player->getInventory()->getOffset();
		int padding = 8;
		std::array<glm::vec2, 2> slot_uvs = BlockTypes::getFaceUVs("slot");
		//inventory
		if (player->isInvOpen()) {
			for (int y = 0; y < 4; y++) {
				for (int x = 0; x < 9; x++) {
					inv_mesh.addRect(offset + glm::ivec2(size * x, size * y), glm::ivec2(size), slot_uvs);
					Item item = player->getInventory()->getItems()->at(((3 - y) * 9) + x);
					if (item.id > 0) {
						std::array<glm::vec2, 2> item_uvs = ItemTypes::getType(item.id).uv_coords;
						inv_mesh.addRect(offset + glm::ivec2(size * x, size * y) + padding, glm::ivec2(size) - (padding * 2), item_uvs);
					}
				}
			}

			Item item = player->getCursorItem();
			if (item.count > 0) {
				std::array<glm::vec2, 2> item_uvs = ItemTypes::getType(item.id).uv_coords;
				inv_mesh.addRect(Input::getMouse() - (size / 2) + padding, glm::ivec2(size) - (padding * 2), item_uvs);
			}
		}
		// hotbar
		else {
			offset.y = (((Window::getSize().y / 2) + (2 * size)) + Window::getSize().y) / 2 - (size / 2);
			std::array<glm::vec2, 2> selected_slot_uvs = BlockTypes::getFaceUVs("selected_slot");
			for (int i = 0; i < 9; i++) {
				if (i == player->getSelectedSlot())
					inv_mesh.addRect(offset + glm::ivec2(size * i, 0), glm::ivec2(size), selected_slot_uvs);
				else
					inv_mesh.addRect(offset + glm::ivec2(size * i, 0), glm::ivec2(size), slot_uvs);
				Item item = player->getInventory()->getItems()->at(i);
				if (item.id > 0) {
					std::array<glm::vec2, 2> item_uvs = ItemTypes::getType(item.id).uv_coords;
					inv_mesh.addRect(offset + glm::ivec2(size * i, 0) + padding, glm::ivec2(size) - (padding * 2), item_uvs);
				}
			}
		}
		inv_mesh.generateBuffers();
		inv_mesh.render();
	}

	void renderInventoryNumbers() {
		Shaders::useText();
		glm::mat4 ortho_matrix = glm::ortho(0.0f, (float)Window::getSize().x, 0.0f, (float)Window::getSize().y);
		Shaders::textShader()->setMat4("MVP", ortho_matrix);
		int size = player->getInventory()->getSlotSize();
		glm::vec2 offset = player->getInventory()->getOffset();
		float padding = 8.0f;
		// inventory
		if (player->isInvOpen()) {
			for (int y = 0; y < 4; y++) {
				for (int x = 0; x < 9; x++) {
					Item item = player->getInventory()->getItems()->at(((3 - y) * 9) + x);
					if (item.count > 0) {
						glm::vec2 pos = offset + glm::vec2(size * x, size * y) + padding;
						pos.y = Window::getSize().y - pos.y - (size * 0.75);
						TextRenderer::renderText(std::to_string(item.count), pos.x, pos.y, 0.75f, glm::vec3(1.0f));
					}
				}
			}

			Item item = player->getCursorItem();
			if (item.count > 0) {
				glm::vec2 pos = (glm::vec2)Input::getMouse() + (padding * 1) - (float)(size / 2);
				pos.y = Window::getSize().y - pos.y - (size * 0.75);
				TextRenderer::renderText(std::to_string(item.count), pos.x, pos.y, 0.75f, glm::vec3(1.0f));
			}
		}
		// hotbar
		else {
			offset.y = (((Window::getSize().y / 2) + (2 * size)) + Window::getSize().y) / 2 - (size / 2);
			for (int i = 0; i < 9; i++) {
				Item item = player->getInventory()->getItems()->at(i);
				if (item.count > 0) {
					glm::vec2 pos = offset + glm::vec2(size * i, 0) + padding;
					pos.y = Window::getSize().y - pos.y - (size * 0.75);
					TextRenderer::renderText(std::to_string(item.count), pos.x, pos.y, 0.75f, glm::vec3(1.0f));
				}
			}
		}
	}

	void renderAttributes() {
		if (!player->isInvOpen()) {
			BasicMesh mesh;
			// cross hair
			std::array<glm::vec2, 2> uvs = BlockTypes::getFaceUVs("cross_hair");
			int size = player->getInventory()->getSlotSize();
			mesh.addRect((Window::getSize() / 2) - glm::ivec2(size / 2), glm::ivec2(size), uvs);
			mesh.generateBuffers();
			mesh.render();
		}
	}

	void render() {
		if (player != nullptr) {
			renderInventory();
			renderAttributes();
			renderInventoryNumbers();
			// reajust ui when window resize
			if (Window::hasResized()) {
				player->getInventory()->resetSlotSize();
			}
		}
	}

	void setPlayer(std::shared_ptr<Player> player) {
		UIRenderer::player = player;
	}
}