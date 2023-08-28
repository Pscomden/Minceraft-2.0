#include "Game.h"

namespace Game {

	void exit() {
		World::exit();
	}

	bool init() {
		if (!loadResources())
			return false;

		double delta_time = 0.0f;
		while (true) {
			auto start_time = std::chrono::high_resolution_clock::now();
			if (!gameLoop(delta_time))
				break;
			auto end_time = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
			delta_time = (double)((duration.count()) / 1000000.0);
			// calculate FPS
			static int count = 0;
			static float total_time = 0;
			total_time += delta_time;
			// display FPS once per second
			count++;
			if (total_time > 1.0) {
				total_time = 0;
				std::string title = "FPS: " + std::to_string(count);
				Window::setWindowTitle(title);
				count = 0;
			}
		}

		exit();

		return true;
	}

	bool loadResources() {
		if (!Window::init()) {
			printf("ERROR: The window couldn't be initialized!\n");
		}
		if (!Input::init()) {
			printf("ERROR: The input couldn't be initialized!\n");
		}
		if (!Shaders::init()) {
			printf("ERROR: The shaders couldn't be initialized!\n");
		}
		if (!TextRenderer::init()) {
			printf("ERROR: The text renderer couldn't be initialized!\n");
		}
		if (!SkyboxRenderer::init()) {
			printf("ERROR: The skybox renderer couldn't be initialized!\n");
		}
		if (!Camera::init()) {
			printf("ERROR: The camera couldn't be initialized!\n");
		}
		if (!Atlas::init()) {
			printf("ERROR: The atlas couldn't be initialized!\n");
		}
		if (!BlockTypes::init("Assets/")) {
			printf("ERROR: The block types couldn't be initialized!\n");
		}
		if (!ItemTypes::init()) {
			printf("ERROR: The item types couldn't be initialized!\n");
		}
		if (!ChunkManager::init()) {
			printf("ERROR: The chunk manager couldn't be initialized!\n");
		}
		// TODO: move to world selection screen (and also set nullptr when exiting)
		World::start("saves/World/");
		InputHandler::setPlayer(World::getPlayer());
		UIRenderer::setPlayer(World::getPlayer());
		DebugRenderer::setPlayer(World::getPlayer());
		Command::setPlayer(World::getPlayer());
		ChunkRenderer::setChunks(ChunkManager::getChunks());
		EntityRenderer::setEntities(World::getEntities());
		pool.Start();

		// Testing Zone

		

		return true;
	}

	bool gameLoop(double delta_time) {
		// resets previous inputs
		// must be done before event polling
		Input::update();
		if (!Window::update()) {
			return false;
		}
		InputHandler::update(delta_time);

		World::update(delta_time);
		// set camera position now
		Camera::update(World::getPlayer()->getPos());
		
		Renderer::render();

		return true;
	}
}