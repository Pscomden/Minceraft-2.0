#include "BasicMesh.h"

void BasicMesh::addRect(glm::vec2 pos, glm::vec2 size, std::array<glm::vec2, 2> uv) {
	BasicVertex new_mesh[] = {
			BasicVertex(pos.x, pos.y + size.y, uv[0].x, uv[1].y), // bottom left
			BasicVertex(pos.x + size.x, pos.y + size.y, uv[1].x, uv[1].y), // bottom right
			BasicVertex(pos.x + size.x, pos.y, uv[1].x, uv[0].y), // top right
			BasicVertex(pos.x, pos.y, uv[0].x, uv[0].y) // top left
	};
	int index[] = {
		0, 1, 2,
		0, 2, 3
	};
	for (int i = 0; i < 6; i++) {
		index[i] += vertices.size();
	}
	vertices.insert(vertices.end(), std::begin(new_mesh), std::end(new_mesh));
	indices.insert(indices.end(), std::begin(index), std::end(index));
}

void BasicMesh::generateBuffers() {
	if (indices.size() == 0) {
		empty = true;
	}
	else {
		empty = false;
	}
	if (!first_time) {
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
	first_time = false;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 4, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		4 * sizeof(GLfloat),  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		4 * sizeof(float),                // stride
		(void*)(2 * sizeof(GLfloat))        // array buffer offset
	);
	glEnableVertexAttribArray(1);

	num_vertices = indices.size();
	buffers_generated = true;
}