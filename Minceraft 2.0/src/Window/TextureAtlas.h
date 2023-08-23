#pragma once

#include "../Include.h"

class TextureAtlas {
public:
	TextureAtlas();
	TextureAtlas(std::string file_name);
	void Load(std::string file_name);
	glm::ivec2 addTexture(std::string file_name);
	int GetTileSize();
	void SetTileSize(int tile_size);
	int GetTextureSize();
	void SetTextureSize(int texture_size);
	glm::vec2 GetSize();
	glm::vec2 GetUVCoords(int x, int y);
	void Use();
	glm::ivec2 GetNextPosition(glm::ivec2 cur_pos);
	glm::vec2 GetUVSize();
private:
	int tile_size;
	int texture_size;
	glm::vec2 uv_size;
	GLuint texture_id;
	glm::ivec2 dimensions;
	glm::ivec2 cur_pos;
};

namespace Atlas {
	bool init();
	TextureAtlas* getAtlas();
}