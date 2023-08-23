#include "SkyboxRenderer.h"

namespace SkyboxRenderer {

	static CubeMap* sky_box;

	bool init() {
		std::array<std::string, 6> textures = {
			"Assets/Textures/skybox/skybox_px.jpg",
			"Assets/Textures/skybox/skybox_nx.jpg",
			"Assets/Textures/skybox/skybox_py.jpg",
			"Assets/Textures/skybox/skybox_ny.jpg",
			"Assets/Textures/skybox/skybox_pz.jpg",
			"Assets/Textures/skybox/skybox_nz.jpg"
		};

		sky_box = new CubeMap(textures);
		
		return true;
	}

	void render() {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		Shaders::useCubeMap();
		glm::mat4 projection = Camera::getProjectionMatrix();
		// view matrix must be at 0, 0, 0
		glm::mat4 view = glm::mat4(glm::mat3(Camera::getViewMatrix()));
		glm::mat4 mvp = projection * view;
		Shaders::cubeMapShader()->setMat4("MVP", mvp);
		sky_box->render();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}
}
