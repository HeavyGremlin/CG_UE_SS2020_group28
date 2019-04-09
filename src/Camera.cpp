#include "Camera.h";

void Camera::positionUpdate(glm::vec3 newPosition) {
	_position += newPosition;
	_viewMatrix = glm::lookAt(_position, _front + _position, glm::vec3(0.0, 1.0, 0.0));

}