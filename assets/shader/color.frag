#version 430 core
/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

in VertexData {
	vec3 position_world;
} vert;

uniform vec3 diffuseColor;

out vec4 color;

void main() {	
	color = vec4(diffuseColor, 1);
}