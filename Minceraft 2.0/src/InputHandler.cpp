#include "InputHandler.h"

namespace InputHandler {

	static std::shared_ptr<Player> player;
	static bool actions_locked = true;
	static bool game_paused = false;

	void movePlayerForward(double delta_time, float offset) {
		//player->addPos(Camera::getForward() * (float)delta_time * offset);
		player->move(Camera::getForward());
	}

	void movePlayerBack(double delta_time, float offset) {
		//player->addPos(-Camera::getForward() * (float)delta_time * offset);
		player->move(-Camera::getForward());
	}

	void movePlayerLeft(double delta_time, float offset) {
		//player->addPos(-Camera::getRight() * (float)delta_time * offset);
		player->move(-Camera::getRight());
	}

	void movePlayerRight(double delta_time, float offset) {
		//player->addPos(Camera::getRight() * (float)delta_time * offset);
		player->move(Camera::getRight());
	}

	void movePlayerUp(double delta_time, float offset) {
		player->move(glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void movePlayerDown(double delta_time, float offset) {
		player->move(glm::vec3(0.0f, -1.0f, 0.0f));
	}

	void updatePlayerMovement(double delta_time) {
		float velocity = 7.0f;
		const float MAX_FOV = 110.0f;
		const float DEFAULT_FOV = 90.0f;
		static float fov = DEFAULT_FOV;
		if (Input::keyHold(GLFW_KEY_LEFT_CONTROL)) {
			player->run();
			fov += 220.0f * delta_time;
			if (fov > MAX_FOV) {
				fov = MAX_FOV;
			}
		}
		else {
			fov -= 220.0f * delta_time;
			if (fov < DEFAULT_FOV) {
				fov = DEFAULT_FOV;
			}
		}
		if (Input::keyHold(GLFW_KEY_C)) {
			fov = 30.0f;
		}
		Camera::setFOV(fov);
		if (Input::keyHold(GLFW_KEY_W)) {
			movePlayerForward(delta_time, velocity);
		}
		if (Input::keyHold(GLFW_KEY_S)) {
			movePlayerBack(delta_time, velocity);
		}
		if (Input::keyHold(GLFW_KEY_A)) {
			movePlayerLeft(delta_time, velocity);
		}
		if (Input::keyHold(GLFW_KEY_D)) {
			movePlayerRight(delta_time, velocity);
		}
		if (Input::keyHold(GLFW_KEY_SPACE)) {
			movePlayerUp(delta_time, velocity);
		}
		if (Input::keyHold(GLFW_KEY_LEFT_SHIFT)) {
			movePlayerDown(delta_time, velocity);
		}
		if (Input::keyPress(GLFW_KEY_F)) {
			
		}
		if (Input::keyPress(GLFW_KEY_F3)) {
			DebugRenderer::toggleStats();
		}
		if (Input::keyPress(GLFW_KEY_R)) {
			DebugRenderer::toggleRigidBodies();
		}
		if (Input::keyPress(GLFW_KEY_F7)) {
			DebugRenderer::toggleChunkBorders();
		}
		if (Input::keyPress(GLFW_KEY_F8)) {
			DebugRenderer::toggleRangeBorders();
		}
		if (Input::keyPress(GLFW_KEY_F9)) {
			DebugRenderer::toggleLimitBorders();
		}
		if (Input::keyPress(GLFW_KEY_F10)) {
			ChunkRenderer::printOrder();
		}
		if (Input::mousePress(GLFW_MOUSE_BUTTON_LEFT)) {
			player->tryBreakBlock();
		}
		if (Input::mousePress(GLFW_MOUSE_BUTTON_RIGHT)) {
			player->tryPlaceBlock();
		}
	}

	void updateCamera(float delta_time) {
		if (Camera::getMode() == Camera::Mode::DETACHED) {
			float camera_speed = 5.0f;
			if (Input::keyHold(GLFW_KEY_J)) {
				Camera::moveLeft(camera_speed * delta_time);
			}
			if (Input::keyHold(GLFW_KEY_L)) {
				Camera::moveRight(camera_speed * delta_time);
			}
			if (Input::keyHold(GLFW_KEY_I)) {
				Camera::moveForward(camera_speed * delta_time);
			}
			if (Input::keyHold(GLFW_KEY_K)) {
				Camera::moveBack(camera_speed * delta_time);
			}
			if (Input::keyHold(GLFW_KEY_U)) {
				Camera::moveDown(camera_speed * delta_time);
			}
			if (Input::keyHold(GLFW_KEY_O)) {
				Camera::moveUp(camera_speed * delta_time);
			}
		}
		if (!game_paused && !actions_locked) {
			if (Input::keyPress(GLFW_KEY_F5)) {
				Camera::Mode mode = Camera::getMode();
				switch (mode) {
				case Camera::Mode::FIRST_PERSON:
					Camera::setMode(Camera::Mode::THIRD_PERSON);
					break;
				case Camera::Mode::THIRD_PERSON:
					Camera::setMode(Camera::Mode::FIRST_PERSON);
					break;
				}
			}
		}
	}

	void hotbarInputs() {
		if (!player->isInvOpen()) {
			if (Input::getScroll().y > 0) {
				player->addSelectedSlot(-1);
			}
			if (Input::getScroll().y < 0) {
				player->addSelectedSlot(1);
			}
			if (Input::keyPress(GLFW_KEY_1)) {
				player->setSelectedSlot(0);
			}
			if (Input::keyPress(GLFW_KEY_2)) {
				player->setSelectedSlot(1);
			}
			if (Input::keyPress(GLFW_KEY_3)) {
				player->setSelectedSlot(2);
			}
			if (Input::keyPress(GLFW_KEY_4)) {
				player->setSelectedSlot(3);
			}
			if (Input::keyPress(GLFW_KEY_5)) {
				player->setSelectedSlot(4);
			}
			if (Input::keyPress(GLFW_KEY_6)) {
				player->setSelectedSlot(5);
			}
			if (Input::keyPress(GLFW_KEY_7)) {
				player->setSelectedSlot(6);
			}
			if (Input::keyPress(GLFW_KEY_8)) {
				player->setSelectedSlot(7);
			}
			if (Input::keyPress(GLFW_KEY_9)) {
				player->setSelectedSlot(8);
			}
		}
	}

	void updateCommand() {
		if (!player->isInvOpen()) {
			if (Command::isOpen()) {
				if (Input::keyPress(GLFW_KEY_ENTER)) {
					Command::execute();
					actions_locked = false;
				}
				else {
					char text_input = Input::getText();
					if ((int)text_input == 3) {
						Command::removeLastChar();
					}
					else if (text_input != '\0') {
						Command::addChar(text_input);
					}
				}
				if (Input::keyPress(GLFW_KEY_UP)) {
					Command::arrowUp();
				}
				if (Input::keyPress(GLFW_KEY_DOWN)) {
					Command::arrowDown();
				}
			}
			if (Input::keyPress(GLFW_KEY_SLASH)) {
				if (!Command::isOpen()) {
					Command::open();
					actions_locked = true;
				}
			}
		}
	}

	void inventoryInputs() {
		if (!Command::isOpen()) {
			if (Input::keyPress(GLFW_KEY_E)) {
				player->toggleInv();
				if (player->isInvOpen()) {
					Window::unlockMouse();
					actions_locked = true;
				}
				else {
					Window::lockMouse();
					actions_locked = false;
				}
			}
			if (Input::mousePress(GLFW_MOUSE_BUTTON_LEFT)) {
				player->leftClickInv(Input::getMouse());
			}
			if (Input::mousePress(GLFW_MOUSE_BUTTON_RIGHT)) {
				player->rightClickInv(Input::getMouse());
			}
		}
	}

	void pauseMenuInputs() {
		if (Input::keyPress(GLFW_KEY_ESCAPE)) {
			// check if in inventory or command first
			if (player->isInvOpen()) {
				player->toggleInv();
				Window::lockMouse();
			}
			else if (Command::isOpen()) {
				Command::close();
				actions_locked = false;
			}
			else {
				// pause
				if (!game_paused) {
					Window::unlockMouse();
					actions_locked = true;
					game_paused = true;
				}
				// unpause
				else {
					Window::lockMouse();
					actions_locked = false;
					game_paused = false;
				}
			}
		}
	}

	void update(double delta_time) {
		if (player != nullptr) {
			if (!game_paused) {
				if (!actions_locked) {
					updateCamera(delta_time);
					updatePlayerMovement(delta_time);
					hotbarInputs();
				}
				inventoryInputs();
				updateCommand();
			}
			pauseMenuInputs();
		}
	}

	void setPlayer(std::shared_ptr<Player> player) {
		InputHandler::player = player;
	}
}