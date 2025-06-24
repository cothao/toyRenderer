#include "textRenderer.h"

TextRenderer::TextRenderer()
{};

TextRenderer::TextRenderer(std::shared_ptr<Shader> Shader)
	:Shader_(Shader)
{};

void TextRenderer::InitTextRenderer()
{
	
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader_->Use();

	Shader_->SetMat4("Projection", glm::ortho(0.0f, 800.f, 0.0f, 600.f));

	FT_Library FT;

	if (FT_Init_FreeType(&FT)) std::cout << "ERROR: Could not Init FreeType Library";

	FT_Face Face;

	if (FT_New_Face(FT, "C:/Windows/Fonts/arial.ttf", 0, &Face))
	{

		std::cout << "ERROR: Failed to load font";

	}
	else
	{




		FT_Set_Pixel_Sizes(Face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++)
		{

			if (FT_Load_Char(Face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR: FAILED TO LOAD GLYPH";
				continue;
			}

			unsigned int Texture;

			glGenTextures(1, &Texture);

			glBindTexture(GL_TEXTURE_2D, Texture);

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				Face->glyph->bitmap.width,
				Face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				Face->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character Character_ =
			{
			Texture,
			glm::ivec2(Face->glyph->bitmap.width, Face->glyph->bitmap.rows),
			glm::ivec2(Face->glyph->bitmap_left, Face->glyph->bitmap_top),
			static_cast<unsigned int>(Face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, Character_));

		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	FT_Done_Face(Face);
	FT_Done_FreeType(FT);

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void TextRenderer::RenderText(std::string Text, float x, float y, float Scale, glm::vec3 Color, glm::mat4 ProjectionMatrix)
{

	Shader_->Use();

	Shader_->SetVec3("TextColor", Color);

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);

	std::string::const_iterator c;

	for (c = Text.begin(); c != Text.end(); c++)
	{

		Character Ch = Characters[*c];

		float Xpos = x + Ch.Bearing.x * Scale;

		float Ypos = y - (Ch.Size.y - Ch.Bearing.y) * Scale;

		float W = Ch.Size.x * Scale;
		float H = Ch.Size.y * Scale;

		float Vertices[6][4] = {
		{ Xpos,     Ypos + H,   0.0f, 0.0f },
		{ Xpos,     Ypos,       0.0f, 1.0f },
		{ Xpos + W, Ypos,       1.0f, 1.0f },

		{ Xpos,     Ypos + H,   0.0f, 0.0f },
		{ Xpos + W, Ypos,       1.0f, 1.0f },
		{ Xpos + W, Ypos + H,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, Ch.TextureID);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES,0, 6);

		x += (Ch.Advance >> 6) * Scale;

	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}