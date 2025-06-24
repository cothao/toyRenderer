#ifndef  TEXTRENDERER_H
#define TEXTRENDERER_H
#define GL_CLAMP_TO_EDGE 0x812F

#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <ft2build.h>
#include "shader.h"
#include <memory>
#include FT_FREETYPE_H

struct Character
{

	unsigned int TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;

};

class TextRenderer
{
public:

	TextRenderer();

	TextRenderer(std::shared_ptr<Shader> Shader);

	std::map<char, Character> Characters;
	std::shared_ptr<Shader> Shader_;
	unsigned int VAO, VBO;

	void InitTextRenderer();
	void RenderText(std::string Text, float x, float y, float Scale, glm::vec3 Color, glm::mat4 ProjectionMatrix);
};

#endif // ! TEXT_H
