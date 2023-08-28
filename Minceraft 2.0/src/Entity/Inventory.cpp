#include "Inventory.h"

Inventory::Inventory(int size) {
	Item null_item = Item(0, 0);
	items = std::vector<Item>(size, null_item);
	slot_size = 75 * 2;
}

Item Inventory::add(Item item) {
	for (int i = 0; i < items.size(); i++) {
		if (items[i].id == 0 || items[i].id == item.id) {
			items[i].id = item.id;
			item.count = items[i].add(item.count);
			if (item.count == 0) {
				return Item(0, 0);
			}
		}
	}
	return item;
}

Item Inventory::remove(int index) {
	Item item = items[index];
	items[index] = Item(0, 0);
	return item;
}

Item Inventory::remove(int index, int count) {
	Item item = items[index];
	item.count = count;
	items[index].remove(count);
	return item;
}

void Inventory::move(Inventory& other, int index) {
	items[index] = other.add(items[index]);
}

void Inventory::swap(Item& other, int index) {
	if (other.id == items[index].id) {
		// simply swap
		if (items[index].count == Item::MAX_COUNT) {
			int temp = other.count;
			other.count = items[index].count;
			items[index].count = temp;
		}
		// compact items
		else {
			other.count = items[index].add(other.count);
			if (other.count == 0) {
				other = Item(0, 0);
			}
		}
	}
	else {
		Item temp = other;
		other = items[index];
		items[index] = temp;
	}
}

Item Inventory::takeHalf(int index) {
	Item half = items[index];
	half.count = ceil(half.count / 2.0f);
	items[index].remove(half.count);
	return half;
}

Item Inventory::tryPlace(Item item, int index) {
	if (items[index].id == 0) {
		items[index] = item;
		return Item(0, 0);
	}
	else if (items[index].id == item.id) {
		int remainder = items[index].add(item.count);
		if (remainder > 0) {
			return Item(item.id, remainder);
		}
		return Item(0, 0);
	}
	return item;
}

void Inventory::leftClickInv(glm::ivec2 mouse, Item& cursor_item) {
	glm::ivec2 slot_index = getSlot(mouse);
	if (slot_index.x != -1) {
		swap(cursor_item, slot_index.y * 9 + slot_index.x);
	}
}

void Inventory::rightClickInv(glm::ivec2 mouse, Item& cursor_item) {
	glm::ivec2 slot_index = getSlot(mouse);
	if (slot_index.x != -1) {
		if (cursor_item.id == 0) {
			cursor_item = takeHalf(slot_index.y * 9 + slot_index.x);
		}
		else {
			Item item = Item(cursor_item.id, 1);
			if (tryPlace(item, slot_index.y * 9 + slot_index.x).count == 0) {
				cursor_item.remove(1);
			}
		}
	}
}

std::vector<Item>* Inventory::getItems() {
	return &items;
}

float Inventory::getSlotSize() {
	return slot_size;
}

glm::vec2 Inventory::getOffset() {
	return glm::ivec2(Window::getSize() / 2 - glm::ivec2(slot_size * 4.5f, slot_size * 2.0f));;
}

glm::ivec2 Inventory::getSlot(glm::ivec2 mouse_pos) {
	glm::vec2 offset = getOffset();
	if (mouse_pos.x >= offset.x && mouse_pos.y >= offset.y &&
		mouse_pos.x < offset.x + (slot_size * 9) && mouse_pos.y < offset.y + (slot_size * 4))
	{
		glm::ivec2 slot = (mouse_pos - (glm::ivec2)offset) / slot_size;
		// inventory is inverted
		slot.y = 3 - slot.y;
		return slot;
	}
	return glm::ivec2(-1);
}

size_t Inventory::size() {
	return items.size();
}
