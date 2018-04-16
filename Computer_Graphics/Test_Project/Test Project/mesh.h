#ifndef MESH_H
#define MESH_H
#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "bounding_box.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;


struct Vertex {

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};


struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	Mesh(vector<Vertex> vertices, vector <unsigned int> indices, vector<Texture> textures, Bounding_Box mesh_bounding_box);

public:

	void Draw(size_t shader_id,bool b_load_textures);

	Bounding_Box mesh_bounding_box;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

private:
	void setup_mesh();

	unsigned int VBO, EBO;

};

#endif // !MESH_H
