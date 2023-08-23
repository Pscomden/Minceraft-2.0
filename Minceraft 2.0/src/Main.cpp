#include "Game.h"

int main() {
	if (!Game::init()) {
		return -69;
	}
	else {
		return 0;
	}
}