#include "ChunkMesh.h"

void ChunkMesh::generateBuffers() {
	if (indices.size() == 0) {
		empty = true;
	}
	else {
		empty = false;
	}
	// added this because if you delete it before it exists, it may end up
	// deleting another buffer we don't want deleted
	// this effect was exaggerated in release mode because things happen so 
	// fast that its a lot easier to create a buffer and immediately delete it
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
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
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		6 * sizeof(float),                // stride
		(void*)(3 * sizeof(GLfloat))        // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		1,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		6 * sizeof(float),                // stride
		(void*)(5 * sizeof(GLfloat))        // array buffer offset
	);
	glEnableVertexAttribArray(2);

	num_vertices = indices.size();
	buffers_generated = true;
}