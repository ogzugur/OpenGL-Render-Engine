#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H


#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
//#include "application.h"


class Bounding_Box
{
public:
	Bounding_Box();
	Bounding_Box(glm::vec3 v0,
		glm::vec3 v1,
		glm::vec3 v2,
		glm::vec3 v3,
		glm::vec3 v4,
		glm::vec3 v5,
		glm::vec3 v6,
		glm::vec3 v7);

public:
	void Draw();
	std::vector<glm::vec3>vertices;

private:
	std::vector<unsigned int> v_indices;

	unsigned int VAO,VBO, EBO;
	void setup_buffers();

};

#endif