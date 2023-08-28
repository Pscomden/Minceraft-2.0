#pragma once

#include "Entity.h"
#include "Inventory.h"
#include "../Assets/BlockTypes.h"
#include "../Physics/Ray.h"
#include "../Render/Camera.h"
#include "../World/ChunkManager.h"

class Player : public Entity {
public:
	enum class Mode : short {
		SURVIVAL,
		GOD,
		GHOST
	};

	Player(glm::vec3 pos);
	~Player();
	void update(float delta_time);
	// TODO: change to frame 1
	void tryBreakBlock();
	void tryPlaceBlock();
	Inventory* getInventory();
	Item getCursorItem();
	bool isInvOpen();
	void toggleInv();
	void leftClickInv(glm::ivec2 mouse);
	void rightClickInv(glm::ivec2 mouse);
	void addSelectedSlot(int val);
	void setSelectedSlot(int val);
	int getSelectedSlot();
	void setMode(Mode mode);
	Mode getMode();
private:
	void moveSurvival(float delta_time);
	void moveGod();
	void moveGhost();
	Ray* ray;
	bool facing_block;
	bool inside_block;
	Inventory inv;
	Item cursor_item;
	int selected_slot;
	bool inv_open;
	std::array<glm::ivec3, 2> looking_blocks;
	Mode mode;
};