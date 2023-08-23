#include "Shader.h"

struct ProgramUniformPair {
    ProgramUniformPair(GLuint program_id, std::string uniform_name) {
        this->program_id = program_id;
        this->uniform_name = uniform_name;
    }
    GLuint program_id;
    std::string uniform_name;
    bool operator==(const ProgramUniformPair& other) const {
        return this->program_id == other.program_id &&
            this->uniform_name == other.uniform_name;
    }
};

struct HashUniform {
    std::size_t operator() (const ProgramUniformPair& key) const {
        return (std::hash<std::string>()(key.uniform_name) ^ std::hash<GLuint>()(key.program_id));
    }
};

static robin_hood::unordered_map < ProgramUniformPair, GLuint, HashUniform > uniform_locations =
robin_hood::unordered_map<ProgramUniformPair, GLuint, HashUniform>();

Shader::Shader() {
	ID = 0;
}

Shader::~Shader() {
	glDeleteProgram(ID);
}
/*
* https://www.youtube.com/watch?v=yrFo1_Izlk0&t=15s
*/
void Shader::addUniformLocations(GLuint shader_id) {
    int num_uniforms;
    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &num_uniforms);

    int max_char_length;
    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_char_length);
    if (num_uniforms > 0 && max_char_length > 0) {
        char* char_buffer = (char*)malloc(sizeof(char) * max_char_length);
        for (int i = 0; i < num_uniforms; i++) {
            int length, size;
            GLenum data_type;
            glGetActiveUniform(shader_id, i, max_char_length, &length, &size, &data_type, char_buffer);
            GLint uniform_location = glGetUniformLocation(shader_id, char_buffer);
            std::string uniform_name(char_buffer);
            ProgramUniformPair pair = { shader_id, uniform_name };
            uniform_locations[pair] = uniform_location;
            std::cout << "Found uniform location for shader " << shader_id << " and name "
                << uniform_name << " at " << uniform_location << "\n";
        }
        free(char_buffer);
    }
}

GLuint Shader::getUniformLocation(GLuint shader_id, std::string) {
    return GLuint();
}

Shader::Shader(std::string vertex_path, std::string frag_path) {
	load(vertex_path, frag_path);
}
/*
* Code from https://learnopengl.com/Getting-started/Shaders
*/
bool Shader::load(std::string vertex_path, std::string frag_path) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertex_path);
        fShaderFile.open(frag_path);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return false;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, "VERTEX"))
        return false;
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    addUniformLocations(ID);

	return true;
}

bool Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    return true;
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setIvec3(const std::string& name, const glm::ivec3& value) const
{
    glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setIvec3(const std::string& name, int x, int y, int z) const
{
    glUniform3i(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(uniform_locations[{ID, name}], 1, GL_FALSE, & mat[0][0]);
}

namespace Shaders {

    enum class CurrentShader {
        NONE,
        BASIC,
        COLOR,
        TEXT,
        WORLD,
        CUBE_MAP
    };

    static Shader* basic_shader = nullptr;
    static Shader* color_shader = nullptr;
    static Shader* text_shader = nullptr;
    static Shader* world_shader = nullptr;
    static Shader* cube_map_shader = nullptr;
    static CurrentShader current_shader;

    bool init() {
        std::string vert_path = "Assets/Shaders/BasicShader.vert";
        std::string frag_path = "Assets/Shaders/BasicShader.frag";
        basic_shader = new Shader(vert_path, frag_path);

        vert_path = "Assets/Shaders/ColorShader.vert";
        frag_path = "Assets/Shaders/ColorShader.frag";
        color_shader = new Shader(vert_path, frag_path);

        vert_path = "Assets/Shaders/TextShader.vert";
        frag_path = "Assets/Shaders/TextShader.frag";
        text_shader = new Shader(vert_path, frag_path);

        vert_path = "Assets/Shaders/WorldShader.vert";
        frag_path = "Assets/Shaders/WorldShader.frag";
        world_shader = new Shader(vert_path, frag_path);

        vert_path = "Assets/Shaders/CubeMapShader.vert";
        frag_path = "Assets/Shaders/CubeMapShader.frag";
        cube_map_shader = new Shader(vert_path, frag_path);

        current_shader = CurrentShader::NONE;

        return true;
    }

    Shader* basicShader() {
        return basic_shader;
    }

    Shader* colorShader() {
        return color_shader;
    }

    Shader* textShader() {
        return text_shader;
    }

    Shader* worldShader() {
        return world_shader;
    }

    Shader* cubeMapShader() {
        return cube_map_shader;
    }

    void useBasic() {
        if (current_shader != CurrentShader::BASIC) {
            current_shader = CurrentShader::BASIC;
            basic_shader->use();
        }
    }

    void useColor() {
        if (current_shader != CurrentShader::COLOR) {
            current_shader = CurrentShader::COLOR;
            color_shader->use();
        }
    }

    void useText() {
        if (current_shader != CurrentShader::TEXT) {
            current_shader = CurrentShader::TEXT;
            text_shader->use();
        }
    }

    void useWorld() {
        if (current_shader != CurrentShader::WORLD) {
            current_shader = CurrentShader::WORLD;
            world_shader->use();
        }
    }

    void useCubeMap() {
        if (current_shader != CurrentShader::CUBE_MAP) {
            current_shader = CurrentShader::CUBE_MAP;
            cube_map_shader->use();
        }
    }
}