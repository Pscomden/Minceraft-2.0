#pragma once

#include "../Include.h"
#include "Mesh/ObjectMesh.h"
#include "../Entity/Entity.h"
#include "../Entity/Player.h"
#include "../Window/Shader.h"
#include "../Window/TextureAtlas.h"

namespace EntityRenderer {
	void setEntities(std::vector<std::shared_ptr<Entity>>* entities);
	void render();
}