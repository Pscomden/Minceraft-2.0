#pragma once

#include "Include.h"

namespace pc {
	// chunk size
	constexpr short c_length = 48; // x
	constexpr short c_height = 48; // y
	constexpr short c_width = 48; // z

	enum class CardinalDirection {
		UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD
	};
}