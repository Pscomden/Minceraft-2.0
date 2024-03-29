#pragma once

#include "../Include.h"

class Console {
public:
	static void printVector(std::string prefix, glm::vec3 vector) {
		std::cout << prefix << " " << vector.x << " " << vector.y << " " << vector.z << "\n";
	}
	
	static void num(std::string prefix, double num) {
		std::cout << prefix << " " << num << "\n";
	}
};