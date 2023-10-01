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
	Entity(glm::dvec3 pos);
	virtual void update(double delta_time);
	void addPos(glm::dvec3 offset);
	void setPos(glm::dvec3 new_pos);
	void move(glm::dvec3 direction);
	void jump();
	void run();
	glm::dvec3& getPos();
	glm::dvec3 getSize();
	glm::dvec3 getVelocity();
	Type getType();
	Rect rigid_body;
protected:
	void checkCollision(glm::dvec3 distance);
	glm::dvec3 pos;
	glm::dvec3 size;
	glm::dvec3 velocity;
	glm::dvec3 direction;
	Type type;
	bool input_jump;
	bool running;
	double max_speed;
	bool on_ground;

	const float GRAVITY_ACCEL = 9.907;
};