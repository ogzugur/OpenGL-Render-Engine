#include "bounding_box.h"
Bounding_Box::Bounding_Box()
{

}

Bounding_Box::Bounding_Box(glm::vec3 v0, 
						   glm::vec3 v1, 
						   glm::vec3 v2, 
						   glm::vec3 v3, 
						   glm::vec3 v4, 
						   glm::vec3 v5, 
						   glm::vec3 v6, 
						   glm::vec3 v7)
{
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);
	vertices.push_back(v5);
	vertices.push_back(v6);
	vertices.push_back(v7);
	unsigned int indices[] = {0,2,3,
							  3,1,0,
							  3,7,5,
							  5,1,3,
							  5,7,6,
							  6,4,5,
							  0,4,6,
							  6,2,0,
							  2,6,7,
							  7,3,2,
							  0,1,5,
							  5,4,0 };
	for (int i = 0; i<36;i++)
	{
		v_indices.push_back(indices[i]);
	}
	setup_buffers();
}

void Bounding_Box::Draw()
{
	//getApplication()->main_rc->UseProgram("boundingBox");
	glBindVertexArray(VAO);
	glDrawElements(GL_LINE_LOOP, v_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Bounding_Box::setup_buffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, v_indices.size() * sizeof(unsigned int), &v_indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(0);
}
