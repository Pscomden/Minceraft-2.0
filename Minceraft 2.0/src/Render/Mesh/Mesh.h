#pragma once

#include "../../Include.h"
/*
* For some reason putting the code in Mesh.cpp caused some 
* template issues I don't understand. When ChunkMesh inputted 
* float or ChunkVertex for the template parameter, I think it 
* caused some sort of issue with Mesh.cpp.
*/
template <class T>
class Mesh {
public:
	Mesh() {
		buffers_generated = false;
		first_time = true;
		empty = true;
		VBO = -1;
		VAO = -1;
		EBO = -1;
	}

	~Mesh() {
		if (buffers_generated) {
			glDeleteBuffers(1, &EBO);
			glDeleteBuffers(1, &VBO);
			glDeleteVertexArrays(1, &VAO);
		}
	}

	virtual void render() {
		if (!empty) {
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, 0);
		}
	}

	void clear() {
		if (buffers_generated) {
			glDeleteBuffers(1, &EBO);
			glDeleteBuffers(1, &VBO);
			glDeleteVertexArrays(1, &VAO);
		}
		vertices.clear();
		indices.clear();
		buffers_generated = false;
	}

	bool isGenerated() {
		return buffers_generated;
	}

	virtual void generateBuffers() = 0;

	std::vector<GLuint> indices;
	std::vector<T> vertices;
protected:
	GLuint VAO, VBO, EBO;
	bool buffers_generated;
	bool first_time;
	bool empty;
	int num_vertices;
};