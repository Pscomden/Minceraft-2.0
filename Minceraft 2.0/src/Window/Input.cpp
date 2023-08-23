#include "Input.h"

namespace Input {

	static bool key_states[348];
	static bool prev_key_states[348];
	static bool mouse_states[5];
	static bool prev_mouse_states[5];
	static glm::dvec2 scroll;
	static glm::ivec2 mouse_pos;
	static glm::ivec2 prev_mouse_pos;
	static bool mouse_locked;
	static char text_input;

	bool init() {
		glfwSetKeyCallback(Window::getWindow(), keyCallBack);
		glfwSetMouseButtonCallback(Window::getWindow(), mouseCallBack);
		glfwSetScrollCallback(Window::getWindow(), scrollCallBack);
		glfwSetCursorPosCallback(Window::getWindow(), mousePosCallBack);
		glfwSetCharCallback(Window::getWindow(), characterCallback);
		Window::setMousePos(&mouse_pos, &prev_mouse_pos);
		mouse_locked = true;

		return true;
	}

	void update() {
		for (int i = 0; i < 348; i++) {
			prev_key_states[i] = key_states[i];
		}
		for (int i = 0; i < 3; i++) {
			prev_mouse_states[i] = mouse_states[i];
		}
		if (mouse_locked)
			prev_mouse_pos = mouse_pos;
		else {
			prev_mouse_pos = Window::getSize() / 2;
		}
		scroll = glm::dvec2(0.0);
		text_input = '\0';
	}

	bool keyPress(int key) {
		return key_states[key] && !prev_key_states[key];
	}

	bool keyHold(int key) {
		return key_states[key];
	}

	bool mousePress(int key) {
		return mouse_states[key] && !prev_mouse_states[key];
	}

	bool mouseHold(int key) {
		return mouse_states[key];
	}

	char getText() {
		return text_input;
	}

	glm::dvec2 getScroll() {
		return scroll;
	}

	glm::ivec2 getMouse() {
		return mouse_pos;
	}

	glm::ivec2 getPrevMouse() {
		return prev_mouse_pos;
	}

	void resetPrevMouse() {
		prev_mouse_pos = Window::getSize() / 2;
	}

	void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS) {
			key_states[key] = true;
			if (key == GLFW_KEY_BACKSPACE) {
				text_input = GLFW_KEY_BACKSPACE;
			}
		}
		else if (action == GLFW_RELEASE) {
			key_states[key] = false;
		}
		else if (action == GLFW_REPEAT) {
			if (key == GLFW_KEY_BACKSPACE) {
				text_input = GLFW_KEY_BACKSPACE;
			}
		}
	}

	void mouseCallBack(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS) {
			mouse_states[button] = true;
		}
		else if (action == GLFW_RELEASE) {
			mouse_states[button] = false;
		}
	}

	void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
		scroll.x = xoffset;
		scroll.y = yoffset;
	}

	void Input::mousePosCallBack(GLFWwindow* window, double xpos, double ypos) {
		mouse_pos.x = xpos;
		mouse_pos.y = ypos;
	}

	void characterCallback(GLFWwindow* window, unsigned int codepoint) {
		if (!text_input)
			text_input = codepoint;
	}
}