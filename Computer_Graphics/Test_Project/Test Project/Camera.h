#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 15.0f;
const float SENSITIVTY = 0.1f;
const float ZOOM = 45.0f;

class Camera 
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 Target;
	glm::vec3 CameraDirection;
	float			CameraFOV;
	float			CameraOrthoHeight;
	float			CameraNear;
	float			CameraFar;


	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH);

	Camera(float posX,
		   float posY,
		   float posZ,
		   float upX,
		   float upY,
		   float upZ,
		   float yaw,
		   float pitch);

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	void updateCameraVectors();



};


#endif
