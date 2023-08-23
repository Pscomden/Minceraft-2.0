#pragma once

#include "../../Include.h"

class CubeMap {
public:
	CubeMap(std::array<std::string, 6> texture_faces);
	~CubeMap();
	void render();
private:
	GLuint texture_id;
	GLuint VAO, VBO;
};