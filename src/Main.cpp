/*
* Copyright 2018 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


#include "Utils.h"
#include <sstream>
#include "Camera.h"
#include "Shader.h"
#include "Geometry.h"
#include "Material.h"
#include "Light.h"
#include "Texture.h"
#include"PxPhysicsAPI.h"

#include <iostream>
#include "glm/ext.hpp"

// MY includes
#include <string>


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, PointLight& pointL);


/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool _wireframe = false;
static bool _culling = true;
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 6.0f;
static bool _accalerate = false;
static bool _accalerateNegative = false;
static bool _rotateForward = false;
static bool _rotateBackward = false;
static bool _rotateLeft = false;
static bool _rotateRight = false;
static bool _spinRight = false;
static bool _spinLeft = false;
static bool _reset = false;
static int _camera = 2;
static bool _coutINFO = false;
int INFO_count = 0;


/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

using namespace std;

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	INIReader reader("assets/settings.ini");

	int window_width = reader.GetInteger("window", "width", 800);
	int window_height = reader.GetInteger("window", "height", 800);
	int refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	bool fullscreen = reader.GetBoolean("window", "fullscreen", false);
	std::string window_title = reader.Get("window", "title", "ECG");
	float fov = float(reader.GetReal("camera", "fov", 60.0f));
	float nearZ = float(reader.GetReal("camera", "near", 0.1f));
	float farZ = float(reader.GetReal("camera", "far", 100.0f));

	/* --------------------------------------------- */
	// Create context
	/* --------------------------------------------- */

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW")
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	GLFWmonitor* monitor = nullptr;

	if (fullscreen)
		monitor = glfwGetPrimaryMonitor();

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title.c_str(), monitor, nullptr);

	if (!window) {
		glfwTerminate();
		EXIT_WITH_ERROR("Failed to create window")
	}

	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();

	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		EXIT_WITH_ERROR("Failed to init GLEW")
	}

	// Debug callback
	if (glDebugMessageCallback != NULL) {
		// Register your callback function.

		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework")
	}

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set GL defaults
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{
		// Load shader(s)
		std::shared_ptr<Shader> textureShader = std::make_shared<Shader>("texture.vert", "texture.frag");

		// Create textures
		std::shared_ptr<Texture> woodTexture = std::make_shared<Texture>("wood_texture.dds");
		std::shared_ptr<Texture> brickTexture = std::make_shared<Texture>("bricks_diffuse.dds");

		// Create materials
		std::shared_ptr<Material> woodTextureMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
		std::shared_ptr<Material> brickTextureMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, brickTexture);

		// Create geometry
		Geometry cube = Geometry(glm::mat4(1.0f), Geometry::createCubeGeometry(1.5f, 1.5f, 2.5f), woodTextureMaterial);
		Geometry cylinder = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -5.0f)), Geometry::createCylinderGeometry(32, 1.3f, 1.0f), brickTextureMaterial);
		Geometry sphere = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, -5.0f)), Geometry::createSphereGeometry(64, 32, 1.0f), brickTextureMaterial);
		Geometry userShip = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)), Geometry::createOBJGeometry("assets/objects/testship.obj"), woodTextureMaterial);
		// Initialize camera
		Camera camera(fov, float(window_width) / float(window_height), nearZ, farZ);
		camera.insertValues(fov, window_height, window_width, float(window_width) / float(window_height), nearZ, farZ);
		// Initialize lights
		DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(0.0f, -1.0f, -1.0f));
		PointLight pointL(glm::vec3(1.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.4f, 0.1f));
		PointLight pointL2(glm::vec3(-1.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.4f, 0.1f));

		//Initialize text overlay

		// Render loop
		float t = float(glfwGetTime());
		float dt = 0.0f;
		float t_sum = 0.0f;
		double mouse_x, mouse_y;

		while (!glfwWindowShouldClose(window)) {
			// Clear backbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Poll events
			glfwPollEvents();


			// print cameraPosition to console
			glm::vec3 cameraPosition = camera.getPosition();
			cout << "cameraPosition\n";
			cout << glm::to_string(cameraPosition) << std::endl;
			cout << "\n\n";

			// print cubeMatrix to console
			glm::mat4 cubeMatrix = cube.getModelMatrix();
			cout << "cubeMatrix\n";
			//cout << glm::to_string(cubeMatrix) << std::endl;
			cout << "\n\n";

			// Update Objects
			if (_accalerateNegative) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				cube.transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.01f)));
				camera.positionUpdate(glm::vec3(0.0f, 0.0f, 0.01f));
			}
			if (_accalerate) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				cube.transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.01f)));
				camera.positionUpdate(glm::vec3(0.0f, 0.0f, -0.01f));
			}
			if (_rotateForward) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				//cubePosition
				glm::vec3 cubePosition = cubeMatrix[3];

				//worldCenterPosition
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cubePosition;

				cube.transform(glm::translate(glm::mat4(1.0f), worldCenterPosition));
				cube.transform(glm::rotate(-0.0005f, glm::vec3(1.0f, 0.0f, 0.0f)));
				cube.transform(glm::translate(glm::mat4(1.0f), cubePosition));
			}
			if (_rotateBackward) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				//cubePosition
				glm::vec3 cubePosition = cubeMatrix[3];

				//worldCenterPosition
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cubePosition;

				cube.transform(glm::translate(glm::mat4(1.0f), worldCenterPosition));
				cube.transform(glm::rotate(0.0005f, glm::vec3(1.0f, 0.0f, 0.0f)));
				cube.transform(glm::translate(glm::mat4(1.0f), cubePosition));
			}
			if (_rotateRight) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				//cubePosition
				glm::vec3 cubePosition = cubeMatrix[3];

				//worldCenterPosition
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cubePosition;

				cube.transform(glm::translate(glm::mat4(1.0f), worldCenterPosition));
				cube.transform(glm::rotate(-0.0005f, glm::vec3(0.0f, 1.0f, 0.0f)));
				cube.transform(glm::translate(glm::mat4(1.0f), cubePosition));
			}
			if (_rotateLeft) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				//cubePosition
				glm::vec3 cubePosition = cubeMatrix[3];

				//worldCenterPosition
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cubePosition;

				cube.transform(glm::translate(glm::mat4(1.0f), worldCenterPosition));
				cube.transform(glm::rotate(0.0005f, glm::vec3(0.0f, 1.0f, 0.0f)));
				cube.transform(glm::translate(glm::mat4(1.0f), cubePosition));
			}
			if (_spinRight) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				//cubePosition
				glm::vec3 cubePosition = cubeMatrix[3];

				//worldCenterPosition
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cubePosition;

				cube.transform(glm::translate(glm::mat4(1.0f), worldCenterPosition));
				cube.transform(glm::rotate(-0.0005f, glm::vec3(0.0f, 0.0f, 1.0f)));
				cube.transform(glm::translate(glm::mat4(1.0f), cubePosition));
			}
			if (_spinLeft) {
				// cubeMatrix
				glm::mat4 cubeMatrix = cube.getModelMatrix();

				//cubePosition
				glm::vec3 cubePosition = cubeMatrix[3];

				//worldCenterPosition
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cubePosition;

				cube.transform(glm::translate(glm::mat4(1.0f), worldCenterPosition));
				cube.transform(glm::rotate(0.0005f, glm::vec3(0.0f, 0.0f, 1.0f)));
				cube.transform(glm::translate(glm::mat4(1.0f), cubePosition));
			}
			if (_reset) {
				// cameraPosition
				/*glm::vec3 cameraPosition = camera.getPosition();
				glm::vec3 worldCenterPosition = glm::vec3(0.0f) - cameraPosition;
				camera.positionUpdate(worldCenterPosition);*/

				cube.resetModelMatrix();
			}

			//show object info
			if (_coutINFO) {
				// print cameraPosition to console
					glm::vec3 cameraPosition = camera.getPosition();
					cout << "cameraPosition\n";
					cout << glm::to_string(cameraPosition) << std::endl;
					cout << "\n\n";

				// print cubeMatrix to console
					glm::mat4 cubeMatrix = cube.getModelMatrix();
					cout << "cubeMatrix\n";
					cout << glm::to_string(cubeMatrix) << std::endl;
					cout << "\n\n";

				// print cubePosition to console
					glm::vec3 cubePosition = glm::vec3(cubeMatrix[3][0], cubeMatrix[3][1], cubeMatrix[3][2]);
					cout << "cubePosition\n";
					cout << glm::to_string(cubePosition) << std::endl;
					cout << "\n\n";


				/*printCameraPosition(camera);
				printCubeMatrix(cube);
				printCubePosition(cube);*/
			}

			// Update camera
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			if (_camera == 2) {
				camera.updates(int(mouse_x), int(mouse_y), _zoom, _dragging, _strafing);
			}
			camera.updates(int(mouse_x), int(mouse_y), _zoom, _dragging, _strafing);
			if (_accalerate) {
				cube.transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.01f)));
				camera.positionUpdate(glm::vec3(0.0f, 0.0f, -0.01f));
			}
      
			// Set per-frame uniforms
			setPerFrameUniforms(textureShader.get(), camera, dirL, pointL);

			// Render
			cube.draw();
			cylinder.draw();
			sphere.draw();
			userShip.draw();

			// Compute frame time

			dt = t;
			t = float(glfwGetTime());
			dt = t - dt;
			t_sum += dt;

			// Swap buffers
			glfwSwapBuffers(window);

		}
	}


	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	return EXIT_SUCCESS;
}

//// print cameraPosition to console
//void printCameraPosition(Camera camera) {
//	glm::vec3 cameraPosition = camera.getPosition();
//	cout << "cameraPosition\n";
//	cout << glm::to_string(cameraPosition) << std::endl;
//	cout << "\n\n";
//}
//
//// print cubeMatrix to console
//void printCubeMatrix(Geometry cube) {
//	glm::mat4 cubeMatrix = cube.getModelMatrix();
//	cout << "cubeMatrix\n";
//	cout << glm::to_string(cubeMatrix) << std::endl;
//	cout << "\n\n";
//}
//
//// print cubePosition to console
//void printCubePosition(Geometry cube) {
//	glm::mat4 cubeMatrix = cube.getModelMatrix();
//	glm::vec3 cubePosition = glm::vec3(cubeMatrix[3][0], cubeMatrix[3][1], cubeMatrix[3][2]);
//	cout << "cubePosition\n";
//	cout << glm::to_string(cubePosition) << std::endl;
//	cout << "\n\n";
//}


void setPerFrameUniforms(Shader* shader, Camera& camera, DirectionalLight& dirL, PointLight& pointL)
{
	shader->use();
	shader->setUniform("viewProjMatrix", camera.getViewProjectionMatrix());
	shader->setUniform("camera_world", camera.getPosition());

	shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);
	shader->setUniform("pointL.color", pointL.color);
	shader->setUniform("pointL.position", pointL.position);
	shader->setUniform("pointL.attenuation", pointL.attenuation);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	_zoom -= float(yoffset) * 0.5f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Culling
	// Esc - Exit

	//if (action != GLFW_RELEASE) return;

	switch (key)
	{
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_RELEASE) return;
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_F1:
			if (action == GLFW_RELEASE) return;
			_wireframe = !_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
			break;
		case GLFW_KEY_F2:
			if (action == GLFW_RELEASE) return;
			_culling = !_culling;
			if (_culling) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
			break;
		case GLFW_KEY_SPACE:
			if (action == GLFW_RELEASE) _accalerate = false;
			else _accalerate = true;
			break;
		case GLFW_KEY_B:
			if (action == GLFW_RELEASE) _accalerateNegative = false;
			else _accalerateNegative = true;
			break;
		case GLFW_KEY_W:
			if (action == GLFW_RELEASE) _rotateForward = false;
			else _rotateForward = true;
			break;
		case GLFW_KEY_S:
			if (action == GLFW_RELEASE) _rotateBackward = false;
			else _rotateBackward = true;
			break;
		case GLFW_KEY_D:
			if (action == GLFW_RELEASE) _rotateRight = false;
			else _rotateRight = true;
			break;
		case GLFW_KEY_A:
			if (action == GLFW_RELEASE) _rotateLeft = false;
			else _rotateLeft = true;
			break;
		case GLFW_KEY_E:
			if (action == GLFW_RELEASE) _spinRight = false;
			else _spinRight = true;
			break;
		case GLFW_KEY_Q:
			if (action == GLFW_RELEASE) _spinLeft = false;
			else _spinLeft = true;
			break;
		case GLFW_KEY_X:
			if (action == GLFW_RELEASE) _reset = false;
			else _reset = true;
			break;
		case GLFW_KEY_I:
			if (action == GLFW_RELEASE) {
				INFO_count = 0;
				_coutINFO = false;
			} else if (INFO_count == 0) {
				INFO_count = 1;
				_coutINFO = true; 
			} else {
				_coutINFO = false;
			}
			break;
	}
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}