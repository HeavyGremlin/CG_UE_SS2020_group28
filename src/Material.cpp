/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#include "Material.h"

/* --------------------------------------------- */
// Base material
/* --------------------------------------------- */

Material::Material(std::shared_ptr<Shader> shader, glm::vec3 materialCoefficients, float specularCoefficient)
	: _shader(shader), _materialCoefficients(materialCoefficients), _alpha(specularCoefficient)
{
}

Material::~Material()
{
}

Shader* Material::getShader()
{
	return _shader.get();
}

void Material::setUniforms()
{
	_shader->setUniform("illumination", _materialCoefficients);
	_shader->setUniform("specularAlpha", _alpha);
}

TextureMaterial::TextureMaterial(std::shared_ptr<Shader> shader, glm::vec3 materialCoefficients, float specularCoefficient, const char* file)
	: Material(shader, materialCoefficients, specularCoefficient), _file(file)
{
	glGenTextures(1, &_handle);
}

void TextureMaterial::setUniforms()
{
	Material::setUniforms();
	glBindTexture(GL_TEXTURE_2D, _handle);
	DDSImage data = loadDDS(_file);
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, data.height, data.width, 0, data.size, data.image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glActiveTexture(GL_TEXTURE0 + 0);
	_shader->setUniform("diffuseTexture", 0);
}
TextureMaterial::~TextureMaterial()
{
}