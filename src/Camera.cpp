#include "Camera.h";

void Camera::positionUpdate(glm::vec3 newPosition) {
	//_position += newPosition;
	_position = newPosition;
	_viewMatrix = glm::lookAt(_position, _front + _position, glm::vec3(0.0, 1.0, 0.0));

}

void Camera::updates(int x, int y, float zoom, bool dragging, bool strafing) {
	if (dragging) {
		if (_firstMouse) {
			_mouseXFirstP = x;
			_mouseYFirstP = y;
			_firstMouse = false;
		}
		float xOffset = x - _mouseXFirstP;
		float yOffset = _mouseYFirstP - y;
		_mouseXFirstP = x;
		_mouseYFirstP = y;

		float sensitivity = 0.5f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		_yawFirstP += xOffset;
		_pitchFirstP += yOffset;

		if (_pitchFirstP > 89.0f)
			_pitchFirstP = 89.0f;
		if (_pitchFirstP < -89.0f)
			_pitchFirstP = -89.0f;
		glm::vec3 front;
		front.x = cos(glm::radians(_yawFirstP)) * cos(glm::radians(_pitchFirstP));
		front.y = sin(glm::radians(_pitchFirstP));
		front.z = sin(glm::radians(_yawFirstP)) * cos(glm::radians(_pitchFirstP));
		_front = glm::normalize(front);

	}

	_viewMatrix = glm::lookAt(_position, _front + _position, glm::vec3(0.0, 1.0, 0.0));
	_projMatrix = glm::perspective((1 + zoom * 0.05f)*glm::radians(_fov), _aspect, _near, _far);
}
void Camera::updatesArcball(int x, int y, float zoom, bool dragging, bool strafing) {

}
glm::vec3 Camera::get_arcball_vector(int x, int y) {
	glm::vec3 P = glm::vec3(1.0*x / _window_width * 2 - 1.0,
		1.0*y / _window_height * 2 - 1.0,
		0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1 * 1)
		P.z = sqrt(1 * 1 - OP_squared);  // Pythagoras
	else
		P = glm::normalize(P);  // nearest point
	return P;
}