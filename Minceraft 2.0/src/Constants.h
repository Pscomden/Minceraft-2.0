#pragma once

#include "Include.h"

namespace pc {
	// chunk size
	constexpr short c_length = 16; // x
	constexpr short c_height = 128; // y
	constexpr short c_width = 16; // z
	// -x, x, -y, y, -z, z
	enum class CardinalDirection : int {
		LEFT, RIGHT, DOWN, UP, FORWARD, BACKWARD
	};
}