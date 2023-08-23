#include "Renderer.h"

namespace Renderer {

	void render() {
		SkyboxRenderer::render();
		Shaders::useWorld();
		glm::mat4 matrix = Camera::getMatrix();
		Shaders::worldShader()->setMat4("MVP", matrix);
		Atlas::getAtlas()->Use();
		ChunkRenderer::render(World::getPlayer()->getPos());
		
		Shaders::useBasic();
		Shaders::basicShader()->setMat4("MVP", matrix);
		EntityRenderer::render();

		Shaders::useBasic();
		glm::mat4 ortho_matrix = glm::ortho(0.0f, (float)Window::getSize().x, (float)Window::getSize().y, 0.0f);
		Shaders::basicShader()->setMat4("MVP", ortho_matrix);
		UIRenderer::render();
		Shaders::useColor();
		Shaders::colorShader()->setMat4("MVP", matrix);
		DebugRenderer::render();
	}
}