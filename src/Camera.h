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
	int _mouseXFirstP, _mouseYFirstP;
	float _yaw, _pitch;
	float _yawFirstP, _pitchFirstP;
	glm::vec3 _position;
	glm::vec3 _front;
	glm::vec3 _up;
	float _fov;
	float _aspect;
	float _window_height;
	float _window_width;
	float _near;
	float _far;
	bool _firstMouse;
	glm::vec3 _strafe;
	bool _inactive = true;

public:
	/*!
	 * Camera constructor
	 * @param fov: field of view, in degrees
	 * @param aspect: aspect ratio
	 * @param near: near plane
	 * @param far: far plane
	 */
	Camera(float fov,float aspect, float near, float far);
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
	void insertValues(float fov, int window_height, int window_width, float aspect, float zNear, float zFar) {
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
		_window_height = window_height;
		_window_width = window_width;
	}
	void update(int x, int y, float zoom, bool dragging, bool strafing);

	void updates(int x, int y, float zoom, bool dragging, bool strafing);
	void updatesArcball(int x, int y, float zoom, bool dragging, bool strafing);
	void positionUpdate(glm::vec3 newPosition);
	void positionUpdateStrafe(float speed);
	//myPositionUpdate(glm::vec3 newPosition);
	void myPositionUpdate(glm::vec3 newPosition);
	void myUpdates(glm::vec3 newPosition, glm::vec3 newFront);


private:
	glm::vec3 get_arcball_vector(int x, int y);

};

	/*!
	* move camera
	*/
