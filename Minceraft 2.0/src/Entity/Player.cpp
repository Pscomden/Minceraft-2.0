#include "Player.h"

Player::Player(glm::vec3 pos) : inv(36), cursor_item(0, 0), Entity(pos) {
	size = glm::vec3(0.75f, 1.8f, 0.75f);
	//size = glm::vec3(1.5f, 3.6f, 1.5f);
	ray = nullptr;
	facing_block = false;
	inside_block = false;
	inv_open = true;
	rigid_body.changeSize(size);
	type = Type::PLAYER;
	mode = Mode::GOD;
	looking_blocks = { glm::ivec3(0) };
	// Test items
	for (int i = 1; i <= BlockTypes::numBlocks(); i++) {
		inv.add(Item(i, 100));
	}
	selected_slot = 0;
}

Player::~Player() {
	
}

void Player::update(double delta_time) {
	switch (mode) {
	case Mode::SURVIVAL:
		moveSurvival(delta_time);
		break;
	case Mode::GOD:
		moveGod();
		break;
	case Mode::GHOST:
		moveGhost();
		break;
	}
	if (mode != Mode::GHOST) {
		Entity::checkCollision(velocity * delta_time);
	}
	else {
		pos += velocity * delta_time;
	}
	if (ray != nullptr) {
		delete ray;
	}
	ray = new Ray(Camera::getPosition(), 5.0f, Camera::getPitch(), Camera::getYaw());
	glm::ivec3 initial_pos = ray->blockPos();
	Block* block = ChunkManager::getBlock(initial_pos);
	inside_block = false;
	facing_block = false;
	if (block != nullptr && block->id != 0) {
		inside_block = true;
	}
	if (ray->calcTwoPositions(looking_blocks)) {
		facing_block = true;
	}
	// reset inputs
	direction = glm::vec3(0.0f);
	input_jump = false;
	running = false;
}

void Player::tryBreakBlock() {
	if ((inside_block || facing_block) && !inv_open) {
		Block old_block;
		if (inside_block) {
			old_block = ChunkManager::changeBlock(glm::floor(pos), 0);
		}
		else {
			old_block = ChunkManager::changeBlock(looking_blocks[0], 0);
		}
		Item item = Item(ItemTypes::nameToID(BlockTypes::getBlockType(old_block.id).drops), 1);
		if (item.id != 0)
			inv.add(item);
	}
}

void Player::tryPlaceBlock() {
	if (facing_block && !inside_block && !inv_open) {
		Item item = inv.getItems()->at(selected_slot);
		if (item.id == 0)
			return;
		inv.remove(selected_slot, 1);
		// TODO: change to item dropping specific block
		Block new_block = Block(BlockTypes::nameToID(ItemTypes::getType(item.id).block));
		ChunkManager::changeBlock(looking_blocks[1], new_block.id);
	}
}

Inventory* Player::getInventory() {
	return &inv;
}

Item Player::getCursorItem() {
	return cursor_item;
}

bool Player::isInvOpen() {
	return inv_open;
}

void Player::toggleInv() {
	inv_open = 1 - inv_open;
}

void Player::leftClickInv(glm::ivec2 mouse) {
	if (inv_open) {
		inv.leftClickInv(mouse, cursor_item);
	}
}

void Player::rightClickInv(glm::ivec2 mouse) {
	if (inv_open) {
		inv.rightClickInv(mouse, cursor_item);
	}
}

void Player::addSelectedSlot(int val) {
	selected_slot += val;
	if (selected_slot < 0) {
		selected_slot = 8;
	}
	if (selected_slot > 8) {
		selected_slot = 0;
	}
}

void Player::setSelectedSlot(int val) {
	selected_slot = val;
	if (selected_slot < 0) {
		selected_slot = 8;
	}
	if (selected_slot > 8) {
		selected_slot = 0;
	}
}

int Player::getSelectedSlot() {
	return selected_slot;
}

void Player::setMode(Mode mode) {
	this->mode = mode;
}

Player::Mode Player::getMode() {
	return mode;
}

void Player::moveSurvival(float delta_time) {
	const float INITIAL_JUMP = 9.907;
	if (direction.y > 0 && on_ground) {
		velocity.y = INITIAL_JUMP;
		std::cout << "here";
	}

	velocity.x += direction.x * delta_time * 2.0f;
	velocity.z += direction.z * delta_time * 2.0f;
	// do fall
	velocity.y -= GRAVITY_ACCEL * delta_time;
	if (on_ground) {
		velocity.y = 0.0f;
	}
}

void Player::moveGod() {
	const double INITIAL_SPEED = 5.0f;
	const double INITIAL_JUMP = 10.0f;
	const double ACCELERATION = 10.0f;
	const double GRAVITY = 9.8f;

	if (running) {
		max_speed = 100.0f;
	}
	else {
		max_speed = 10.0f;
	}
	double velocity_distance = glm::distance(glm::dvec3(0.0f), velocity);
	if (direction != glm::dvec3(0.0f)) {
		if (velocity == glm::dvec3(0.0f)) {
			velocity = direction * INITIAL_SPEED;
		}
		else {
			velocity = velocity_distance * direction;
			velocity += direction * ACCELERATION;
		}
	}
	else {
		velocity = glm::vec3(0.0f);
	}
	velocity_distance = glm::distance(glm::dvec3(0.0f), velocity);
	if (velocity_distance > max_speed) {
		velocity = glm::normalize(velocity) * max_speed;
	}
}

void Player::moveGhost() {
	const double INITIAL_SPEED = 5.0f;
	const double INITIAL_JUMP = 10.0f;
	const double ACCELERATION = 10.0f;
	const double GRAVITY = 9.8f;

	if (running) {
		max_speed = 100.0f;
	}
	else {
		max_speed = 10.0f;
	}
	double velocity_distance = glm::distance(glm::dvec3(0.0f), velocity);
	if (direction != glm::dvec3(0.0f)) {
		if (velocity == glm::dvec3(0.0f)) {
			velocity = direction * INITIAL_SPEED;
		}
		else {
			velocity = velocity_distance * direction;
			velocity += direction * ACCELERATION;
		}
	}
	else {
		velocity = glm::vec3(0.0f);
	}
	velocity_distance = glm::distance(glm::dvec3(0.0f), velocity);
	if (velocity_distance > max_speed) {
		velocity = glm::normalize(velocity) * max_speed;
	}
}