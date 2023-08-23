#pragma once

#include "../Include.h"
#include "../Window/Shader.h"

namespace TextRenderer {
	bool init();
	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
}