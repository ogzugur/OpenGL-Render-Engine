#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "bounding_box.h"
#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
class Model
{
public:
	Model();
	Model(string path, bool b_load_textures = true);
public:
	void Draw(size_t shader_id);
	void debug_box_draw(size_t shader_id);
	glm::mat4 get_model_matrix();
	void set_position(float* position);
	void set_rotation(float* position, float rotation_angle);
	void set_scale(float scale_amount);
	void reset_model_matrix();
	void translate(float x, float y, float z);
	void translate(glm::vec3 translation);
	void rotate(float angle, glm::vec3);
	void scale(float x, float y, float z);
	vector<Mesh> meshes;
	bool b_load_texture;

private:
	vector<Texture> textures_loaded;

	string directory;

	
	glm::mat4 transformation_matrix = glm::mat4(1.0f);
	glm::mat4 position_matrix = glm::mat4(1.0f);
	glm::mat4 rotation_matrix = glm::mat4(1.0f);
	glm::mat4 scale_matrix = glm::mat4(1.0f);
	
	void load_model(string path, bool b_load_textures);
	void process_node(aiNode *node, const aiScene *scene, bool b_load_textures);
	Mesh process_mesh(aiMesh *mesh, const aiScene *scene, bool b_load_textures);
	vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, string typeName);
	unsigned int texture_from_file(const char * path, const string & directory);
};
#endif // !MODEL_H
