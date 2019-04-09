/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include "glm/ext.hpp"

/*!
 * Arc ball camera, modified by mouse input
 */
class Camera
{
protected:

	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	int _mouseX, _mouseY;
	float _yaw, _pitch;
	glm::vec3 _position;
	glm::vec3 _front;
	glm::vec3 _up;
	float _fov;
	float _aspect;
	float _near;
	float _far;
	bool _firstMouse;
	glm::vec3 _strafe;

public:
	/*!
	 * Camera constructor
	 * @param fov: field of view, in degrees
	 * @param aspect: aspect ratio
	 * @param near: near plane
	 * @param far: far plane
	 */
	Camera(float fov, float aspect, float near, float far);
	~Camera();

	/*!
	 * @return the current position of the camera
	 */
	glm::vec3 getPosition();

	/*!
	 * @return the view-projection matrix
	 */
	glm::mat4 getViewProjectionMatrix();

	/*!
	 * Updates the camera's position and view matrix according to the input
	 * @param x: current mouse x position
	 * @param y: current mouse x position
	 * @param zoom: zoom multiplier
	 * @param dragging: is the cammera dragging
	 * @param strafing: is the cammera strafing
	 */
	void insertValues(float fov, float aspect, float zNear, float zFar) {
		_position = glm::vec3(0.0f, 0.0f, 6.0f);
		_front = glm::vec3(0.0f, 0.0f, -1.0f);
		_fov = fov;
		_aspect = aspect;
		_near = zNear;
		_far = zFar;
		_yaw = -90.0f;
		_pitch = 0.0f;
		_firstMouse = true;
		_viewMatrix = glm::lookAt(_position, _front, glm::vec3(0.0f, 1.0f, 0.0f));


	}
	void update(int x, int y, float zoom, bool dragging, bool strafing);

	void updates(int x, int y, float zoom, bool dragging, bool strafing) {
		if (dragging) {
			if (_firstMouse) {
				_mouseX = x;
				_mouseY = y;
				_firstMouse = false;
			}
			float xOffset = x - _mouseX;
			float yOffset = _mouseY - y;
			_mouseX = x;
			_mouseY = y;

			float sensitivity = 0.5f;
			xOffset *= sensitivity;
			yOffset *= sensitivity;

			_yaw += xOffset;
			_pitch += yOffset;

			if (_pitch > 89.0f)
				_pitch = 89.0f;
			if (_pitch < -89.0f)
				_pitch = -89.0f;
			glm::vec3 front;
			front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
			front.y = sin(glm::radians(_pitch));
			front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
			_front = glm::normalize(front);

		}

		_viewMatrix = glm::lookAt(_position, _front+_position, glm::vec3(0.0, 1.0, 0.0));
		_projMatrix = glm::perspective((1 + zoom*0.05f)*glm::radians(_fov), _aspect, _near, _far);
	}

	/*!
	* move camera
	*/
	void Camera::positionUpdate(glm::vec3 newPosition) {
		_position += newPosition;
		_viewMatrix = glm::lookAt(_position, _front+_position, glm::vec3(0.0, 1.0, 0.0));

	}
};