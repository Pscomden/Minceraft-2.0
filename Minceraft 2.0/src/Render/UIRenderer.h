#pragma once

#include "../Include.h"
#include "TextRenderer.h"
#include "Mesh/BasicMesh.h"
#include "../Assets/BlockTypes.h"
#include "../Entity/Player.h"
#include "../Window/Input.h"
#include "../Window/Shader.h"
#include "../Window/Window.h"

namespace UIRenderer {
	void render();
	void setPlayer(std::shared_ptr<Player> player);
}