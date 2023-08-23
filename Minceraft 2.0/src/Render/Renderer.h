#pragma once

#include "../Include.h"
#include "Camera.h"
#include "ChunkRenderer.h"
#include "DebugRenderer.h"
#include "EntityRenderer.h"
#include "SkyboxRenderer.h"
#include "UIRenderer.h"
#include "../Assets/BlockTypes.h"
#include "../Render/Mesh/Mesh.h"
#include "../Window/Shader.h"
#include "../Window/Window.h"
#include "../World/Chunk.h"
#include "../World/ChunkManager.h"
#include "../World/World.h"

namespace Renderer {
	void render();
}