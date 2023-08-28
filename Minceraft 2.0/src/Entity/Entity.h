#pragma once

#include "../Include.h"
#include "../Physics/Shape.h"
#include "../World/ChunkManager.h"
#include "../Debug/Console.h"

class Entity {
public:
	enum class Type {
		NONE,
		PLAYER,
		ENEMY,
		NPC
	};
	Entity(glm::vec3 pos);
	virtual void update(float delta_time);
	void addPos(glm::vec3 offset);
	void setPos(glm::vec3 new_pos);
	void move(glm::vec3 direction);
	void jump();
	void run();
	glm::vec3& getPos();
	glm::vec3 getSize();
	glm::vec3 getVelocity();
	Type getType();
	Rectangle rigid_body;
protected:
	void checkCollision(glm::vec3 distance);
	glm::vec3 pos;
	glm::vec3 size;
	glm::vec3 velocity;
	glm::vec3 direction;
	Type type;
	bool input_jump;
	bool running;
	float max_speed;
	bool on_ground;

	const float GRAVITY_ACCEL = 9.907;
};