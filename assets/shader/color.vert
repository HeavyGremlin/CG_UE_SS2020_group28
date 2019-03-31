#version 430 core
/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

layout(location = 0) in vec3 position;

out VertexData {
	vec3 position_world;
} vert;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

void main() {
	gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1);
}