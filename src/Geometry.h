/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GL\glew.h>
#include "Material.h"
#include "Shader.h"

/*!
 * Stores all data for a geometry object
 */
struct GeometryData {
	/*!
	 * Vertex positions
	 */
	std::vector<glm::vec3> positions;
	/*!
	 * Geometry indices
	 */
	std::vector<unsigned int> indices;
	/*!
	 * Vertex normals
	 */
	std::vector<glm::vec3> normals;
	/*!
	 * Vertex UV coordinates
	 */
	std::vector<glm::vec2> uvs;
};


class Geometry
{
protected:
	/*!
	 * Vertex array object
	 */
	GLuint _vao;
	/*!
	 * Vertex buffer object that stores the vertex positions
	 */
	GLuint _vboPositions;
	/*!
	 * Vertex buffer object that stores the vertex normals
	 */
	GLuint _vboNormals;
	/*!
	 * Vertex buffer object that stores the vertex UV coordinates
	 */
	GLuint _vboUVs;
	/*!
	 * Vertex buffer object that stores the indices
	 */
	GLuint _vboIndices;

	/*!
	 * Number of elements to be rendered
	 */
	unsigned int _elements;

	/*!
	 * Material of the geometry object
	 */
	std::shared_ptr<Material> _material;

	/*!
	 * Model matrix of the object
	 */
	glm::mat4 _modelMatrix;

public:
	/*!
	 * Geometry object constructor
	 * Creates VAO and VBOs and binds them
	 * @param modelMatrix: model matrix of the object
	 * @param data: data for the geometry object
	 * @param material: material of the geometry object
	 */
	Geometry(glm::mat4 modelMatrix, GeometryData& data, std::shared_ptr<Material> material);
	~Geometry();

	/*!
	 * Draws the object
	 * Uses the shader, sets the uniform and issues a draw call
	 */
	void draw();

	/*!
	 * Transforms the object, i.e. updates the model matrix
	 * @param transformation: the transformation matrix to be applied to the object
	 */
	void transform(glm::mat4 transformation);

	///*!
	// * @return the modelMatrix of the Geometry
	// */
	glm::mat4 getModelMatrix() { return _modelMatrix; }

	/*!
	 * Resets the model matrix to the identity matrix
	 */
	void resetModelMatrix();

	/*!
	 * Creates a cube geometry
	 * @param width: width of the cube
	 * @param height: height of the cube
	 * @param depth: depth of the cube
	 * @return all cube data
	 */
	static GeometryData createCubeGeometry(float width, float height, float depth);
	/*!
	 * Creates a cylinder geometry
	 * @param segments: number of segments of the cylinder
	 * @param height: height of the cylinder
	 * @param radius: radius of the cylinder
	 * @return all cylinder data
	 */
	static GeometryData createCylinderGeometry(unsigned int segments, float height, float radius);
	/*!
	 * Creates a sphere geometry
	 * @param longitudeSegments: number of longitude segments of the sphere
	 * @param latitudeSegments: number of latitude segments of the sphere
	 * @param radius: radius of the sphere
	 * @return all sphere data
	 */
	static GeometryData createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius);

	static GeometryData createOBJGeometry(const char * path) {

		GeometryData data;
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< glm::vec3 > temp_vertices;
		std::vector< glm::vec2 > temp_uvs;
		std::vector< glm::vec3 > temp_normals;
		FILE * file = fopen(path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
		}
		else {
			while (1) {

				char lineHeader[128];
				// read the first word of the line
				int res = fscanf(file, "%s", lineHeader);
				if (res == EOF) {
					break; // EOF = End Of File. Quit the loop.
				}
				else {
					if (strcmp(lineHeader, "v") == 0) {
						glm::vec3 vertex;
						fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
						temp_vertices.push_back(vertex);
					}
					else if (strcmp(lineHeader, "vt") == 0) {
						glm::vec2 uv;
						fscanf(file, "%f %f\n", &uv.x, &uv.y);
						temp_uvs.push_back(uv);
					}
					else if (strcmp(lineHeader, "vn") == 0) {
						glm::vec3 normal;
						fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
						temp_normals.push_back(normal);
					}
					else if (strcmp(lineHeader, "f") == 0) {
						std::string vertex1, vertex2, vertex3;
						unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
						int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
						if (matches != 9) {
							printf("File can't be read by our simple parser : ( Try exporting with other options\n");
						}
						vertexIndices.push_back(vertexIndex[0]);
						vertexIndices.push_back(vertexIndex[1]);
						vertexIndices.push_back(vertexIndex[2]);
						uvIndices.push_back(uvIndex[0]);
						uvIndices.push_back(uvIndex[1]);
						uvIndices.push_back(uvIndex[2]);
						normalIndices.push_back(normalIndex[0]);
						normalIndices.push_back(normalIndex[1]);
						normalIndices.push_back(normalIndex[2]);


					}
				}
			}
			for (unsigned int i = 0; i < vertexIndices.size(); i++) {
				unsigned int vertexIndex = vertexIndices[i];
				glm::vec3 vertex = temp_vertices[vertexIndex - 1];
				data.positions.push_back(vertex);
				data.indices.push_back(i);

			}
			for (unsigned int i = 0; i < uvIndices.size(); i++) {
				unsigned int uvIndex = uvIndices[i];
				glm::vec2 uv = temp_uvs[uvIndex - 1];
				data.uvs.push_back(uv);

			}
			for (unsigned int i = 0; i < normalIndices.size(); i++) {
				unsigned int normalIndex = normalIndices[i];
				glm::vec3 normal = temp_normals[normalIndex - 1];
				data.normals.push_back(normal);

				return std::move(data);
			}
		}
	}
};
