#pragma once

#include "../Include.h"
#include "../Window/Input.h"
#include "../Window/Window.h"

namespace Camera {
	enum class Mode {
		FIRST_PERSON,
		THIRD_PERSON,
		DETACHED
	};
	bool init();
	glm::mat4 getMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();
	glm::mat4 getOriginViewMatrix();
	glm::vec3 getDirection();
	glm::vec3 getRight();
	glm::vec3 getForward();
	void moveForward(float distance);
	void moveBack(float distance);
	void moveLeft(float distance);
	void moveRight(float distance);
	void strafeForward(float distance);
	void strafeBack(float distance);
	void moveUp(float distance);
	void moveDown(float distance);
	void addYaw(float distance);
	void addPitch(float distance);
	float getYaw();
	float getPitch();
	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();
	void setFOV(float fov);
	void setMode(Mode mode);
	Mode getMode();
	void update(glm::vec3 player_pos);
}