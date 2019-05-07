#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "Utils.h"
#include "Shader.h"
struct FontCharacterData {
public:
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};


class FontCharacter {
protected:
	std::map<GLchar, FontCharacterData> Characters;
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	GLuint _VAO, _VBO;
public:
	void initialize();
	void RenderText(std::shared_ptr<Shader> &s, std::string text, GLfloat x, GLfloat y, GLfloat scale);

};