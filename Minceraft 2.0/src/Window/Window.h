#pragma once

#include "../Include.h"

namespace Window {
	bool init();
	bool update();
	void setWindowTitle(std::string title);
	void lockMouse();
	void unlockMouse();
	bool isMouseLocked();
	void setMousePos(glm::ivec2* mouse_pos, glm::ivec2* prev_mouse_pos);
	void frameBuffSizeCallback(GLFWwindow* window, int width, int height);
	glm::ivec2 getSize();
	GLFWwindow* getWindow();
}