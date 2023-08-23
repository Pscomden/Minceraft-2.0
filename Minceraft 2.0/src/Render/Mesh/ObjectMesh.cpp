#include "ObjectMesh.h"

void ObjectMesh::addRect(glm::vec3 pos1, glm::vec3 pos2, std::array<glm::vec2, 2> uv) {
	ObjectVertex new_mesh[] = {
			ObjectVertex(pos1.x, pos1.y, pos2.z, uv[0].x, uv[1].y), // bottom left
			ObjectVertex(pos2.x, pos2.y, pos2.z, uv[1].x, uv[1].y), // bottom right
			ObjectVertex(pos2.x, pos2.y, pos1.z, uv[1].x, uv[0].y), // top right
			ObjectVertex(pos1.x, pos1.y, pos1.z, uv[0].x, uv[0].y) // top left
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

void ObjectMesh::addPrism(glm::vec3 pos, glm::vec3 size, std::array<std::array<glm::vec2, 2>, 6> uvs) {
	size /= 2.0f;
	glm::vec3 base_vertices[] = {
		pos - size,												   // 0
		glm::vec3(pos.x + size.x, pos.y - size.y, pos.z - size.z), // 1
		glm::vec3(pos.x + size.x, pos.y + size.y, pos.z - size.z), // 2
		glm::vec3(pos.x - size.x, pos.y + size.y, pos.z - size.z), // 3
		glm::vec3(pos.x + size.x, pos.y - size.y, pos.z + size.z), // 4
		glm::vec3(pos.x - size.x, pos.y - size.y, pos.z + size.z), // 5
		glm::vec3(pos.x - size.x, pos.y + size.y, pos.z + size.z), // 6
		pos + size												   // 7
	};
	ObjectVertex new_vertices[] = {
		// front
		ObjectVertex(base_vertices[0], glm::vec2(uvs[0][0].x, uvs[0][1].y)), // bottom left
		ObjectVertex(base_vertices[1], uvs[0][1]), // bottom right
		ObjectVertex(base_vertices[2], glm::vec2(uvs[0][1].x, uvs[0][0].y)), // top right
		ObjectVertex(base_vertices[3], uvs[0][0]), // top left
		// back
		ObjectVertex(base_vertices[4], glm::vec2(uvs[1][0].x, uvs[1][1].y)), // bottom left
		ObjectVertex(base_vertices[5], uvs[1][1]), // bottom right
		ObjectVertex(base_vertices[6], glm::vec2(uvs[1][1].x, uvs[1][0].y)), // top right
		ObjectVertex(base_vertices[7], uvs[1][0]),  // top left
		// left
		ObjectVertex(base_vertices[5], glm::vec2(uvs[2][0].x, uvs[2][1].y)), // bottom left
		ObjectVertex(base_vertices[0], uvs[2][1]), // bottom right
		ObjectVertex(base_vertices[3], glm::vec2(uvs[2][1].x, uvs[2][0].y)), // top right
		ObjectVertex(base_vertices[6], uvs[2][0]),  // top left
		// right
		ObjectVertex(base_vertices[1], glm::vec2(uvs[3][0].x, uvs[3][1].y)), // bottom left
		ObjectVertex(base_vertices[4], uvs[3][1]), // bottom right
		ObjectVertex(base_vertices[7], glm::vec2(uvs[3][1].x, uvs[3][0].y)), // top right
		ObjectVertex(base_vertices[2], uvs[3][0]), // top left
		// top
		ObjectVertex(base_vertices[3], glm::vec2(uvs[4][0].x, uvs[4][1].y)), // bottom left
		ObjectVertex(base_vertices[2], uvs[4][1]),  // bottom right
		ObjectVertex(base_vertices[7], glm::vec2(uvs[4][1].x, uvs[4][0].y)), // top right
		ObjectVertex(base_vertices[6], uvs[4][0]), // top left
		// bottom
		ObjectVertex(base_vertices[5], glm::vec2(uvs[5][0].x, uvs[5][1].y)), // bottom left
		ObjectVertex(base_vertices[4], uvs[5][1]), // bottom right
		ObjectVertex(base_vertices[1], glm::vec2(uvs[5][1].x, uvs[5][0].y)), // top right
		ObjectVertex(base_vertices[0], uvs[5][0])  // top left
	};
	int index[] = {
		// front
		0, 1, 2,
		0, 2, 3,
		// back
		4, 5, 6,
		4, 6, 7,
		// left
		8, 9, 10,
		8, 10, 11,
		// right
		12, 13, 14,
		12, 14, 15,
		// top
		16, 17, 18,
		16, 18, 19,
		// back
		20, 21, 22,
		20, 22, 23
	};
	for (int i = 0; i < 6; i++) {
		index[i] += vertices.size();
	}
	vertices.insert(vertices.end(), std::begin(new_vertices), std::end(new_vertices));
	indices.insert(indices.end(), std::begin(index), std::end(index));
}

void ObjectMesh::generateBuffers() {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 5, vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		5 * sizeof(GLfloat),  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		5 * sizeof(float),                // stride
		(void*)(3 * sizeof(GLfloat))        // array buffer offset
	);
	glEnableVertexAttribArray(1);

	num_vertices = indices.size();
	buffers_generated = true;
}