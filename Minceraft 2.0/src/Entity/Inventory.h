#pragma once

#include "../Include.h"
#include "../Assets/Item.h"
#include "../Window/Window.h"

class Inventory {
public:
	Inventory(int size);
	Item add(Item item);
	Item remove(int index);
	Item remove(int index, int count);
	void move(Inventory& other, int index);
	void swap(Item& other, int index);
	Item takeHalf(int index);
	Item tryPlace(Item item, int index);
	void leftClickInv(glm::ivec2 mouse, Item& cursor_item);
	void rightClickInv(glm::ivec2 mouse, Item& cursor_item);
	std::vector<Item>* getItems();
	float getSlotSize();
	void resetSlotSize();
	glm::vec2 getOffset();
	glm::ivec2 getSlot(glm::ivec2 mouse_pos);
	size_t size();
private:
	std::vector<Item> items;
	int slot_size;
};