#pragma once

#include "../Include.h"
#include "TextRenderer.h"
#include "Mesh/BasicMesh.h"
#include "Mesh/LineMesh.h"
#include "../Entity/Player.h"
#include "../Debug/Command.h"
#include "../World/World.h"

namespace DebugRenderer {
	void render();
	void toggleRigidBodies();
	void toggleStats();
	void toggleChunkBorders();
	void toggleRangeBorders();
	void setPlayer(std::shared_ptr<Player> player);
}
