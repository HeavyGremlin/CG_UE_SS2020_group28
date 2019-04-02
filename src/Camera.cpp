#include "Camera.h";

void Camera::positionUpdate(glm::vec3 newPosition){
	Camera::_position = newPosition;
}