#pragma once

#include "../Include.h"
#include "../Entity/Player.h"
#include "../World/World.h"
#include "../World/WorldBuilder.h"

namespace Command {
	void open();
	void close();
	bool isOpen();
	bool inputExists();
	void execute();
	std::string getInput();
	void addChar(char character);
	void removeLastChar();
	void arrowUp();
	void arrowDown();
	void setPlayer(std::shared_ptr<Player> player);
}