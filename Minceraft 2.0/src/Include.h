#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <future>
#include <thread>
#include <stdio.h>
#include <random>
#include <queue>
#include <list>
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <iterator>
#include <tchar.h>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "SFML/Graphics.hpp"

#include <stb_image.h>

#include "FastNoise/FastNoise.h"

#include "robin_hood.h"

namespace robin_hood {
	template <> struct hash<glm::ivec3> {
		size_t operator()(const glm::ivec3& vect) const noexcept
		{
			std::hash<decltype(vect.x)> hasher;

			auto hash1 = hasher(vect.x);
			auto hash2 = hasher(vect.y);
			auto hash3 = hasher(vect.z);

			return std::hash<decltype(vect.x)>{}(
				(hash1 ^ (hash2 << hash3) ^ hash3));
		}
	};
}