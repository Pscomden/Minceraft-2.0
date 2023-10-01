#pragma once

#include "Include.h"

namespace pc {
	// chunk size
	constexpr short c_length = 32; // x
	constexpr short c_height = 32; // y
	constexpr short c_width = 32; // z
	// -x, x, -y, y, -z, z
	enum class CardinalDirection : int {
		LEFT, RIGHT, DOWN, UP, FORWARD, BACKWARD
	};
}