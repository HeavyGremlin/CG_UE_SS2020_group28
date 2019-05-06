#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "Utils.h"
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
class FontCharacter {
public:
	void initialize();
};