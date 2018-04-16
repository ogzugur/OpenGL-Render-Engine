#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "application.h"


const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
int main()
{
	Application app;
	app.OpenWindow(SCR_WIDTH, SCR_HEIGHT, false, false);
	app.GameLoop();

	return 0;
}

