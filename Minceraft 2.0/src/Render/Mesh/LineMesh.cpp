#include "LineMesh.h"

void LineMesh::render() {
	if (!empty) {
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, num_vertices, GL_UNSIGNED_INT, 0);
	}
}

void LineMesh::addLine(glm::vec3 start, glm::vec3 end, glm::vec3 color) {
	LineVertex new_line[] = { LineVertex(start, color), LineVertex(end, color) };
	int new_indices[] = { 0, 1 };
	for (int i = 0; i < 2; i++) {
		new_indices[i] += vertices.size();
	}
	vertices.insert(vertices.end(), std::begin(new_line), std::end(new_line));
	indices.insert(indices.end(), std::begin(new_indices), std::end(new_indices));
}

void LineMesh::addRect(const Rect& rect, glm::vec3 color) {
	glm::vec3 start = rect.pos - rect.size / 2.0f;
	glm::vec3 end = rect.pos + rect.size / 2.0f;
	addLine(start, glm::vec3(end.x, start.y, start.z), color);
	addLine(start, glm::vec3(start.x, end.y, start.z), color);
	addLine(start, glm::vec3(start.x, start.y, end.z), color);
	addLine(end, glm::vec3(start.x, end.y, end.z), color);
	addLine(end, glm::vec3(end.x, start.y, end.z), color);
	addLine(end, glm::vec3(end.x, end.y, start.z), color);
	addLine(glm::vec3(start.x, end.y, start.z), glm::vec3(end.x, end.y, start.z), color);
	addLine(glm::vec3(start.x, end.y, start.z), glm::vec3(start.x, end.y, end.z), color);
	addLine(glm::vec3(end.x, start.y, end.z), glm::vec3(start.x, start.y, end.z), color);
	addLine(glm::vec3(end.x, start.y, end.z), glm::vec3(end.x, start.y, start.z), color);
	addLine(glm::vec3(end.x, end.y, start.z), glm::vec3(end.x, start.y, start.z), color);
	addLine(glm::vec3(start.x, end.y, end.z), glm::vec3(start.x, start.y, end.z), color);
}

void LineMesh::addGridRect(const Rect& rect, float spacing, glm::vec3 color) {
	/*glm::vec3 start = rect.pos - rect.size / 2.0f;
	glm::vec3 end = rect.pos + rect.size / 2.0f;
	glm::ivec3 num_lines = glm::floor(rect.size / spacing);
	addRect(rect, color);
	for (int i = 0; i < num_lines.x; i++) {
		addLine(start + glm::vec3(spacing * i, 0, 0), glm::vec3());
	}*/
}

void LineMesh::generateBuffers() {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 6, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		6 * sizeof(GLfloat),  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		6 * sizeof(GLfloat),                // stride
		(void*)(3 * sizeof(GLfloat))        // array buffer offset
	);
	glEnableVertexAttribArray(1);

	num_vertices = indices.size();
	buffers_generated = true;
}