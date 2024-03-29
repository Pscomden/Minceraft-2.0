#pragma once

#include "../Include.h"

class Shader {
public:
	Shader();
	~Shader();
	Shader(std::string vertex_path, std::string frag_path);
	bool load(std::string vertex_path, std::string frag_path);
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w);
	void setIvec3(const std::string& name, const glm::ivec3& value) const;
	void setIvec3(const std::string& name, int x, int y, int z) const;
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
	bool checkCompileErrors(unsigned int shader, std::string type);
	GLuint ID;

	static void addUniformLocations(GLuint shader_id);
	static GLuint getUniformLocation(GLuint shader_id, std::string);
};

namespace Shaders {
	bool init();
	Shader* basicShader();
	Shader* colorShader();
	Shader* textShader();
	Shader* worldShader();
	Shader* cubeMapShader();
	void useBasic();
	void useColor();
	void useText();
	void useWorld();
	void useCubeMap();
}