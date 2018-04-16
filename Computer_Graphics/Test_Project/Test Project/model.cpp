#include "model.h"

Model::Model()
{

}
Model::Model(string path, bool b_load_textures)
{
	this->b_load_texture = b_load_textures;
	load_model(path,b_load_textures);
}

void Model::Draw(size_t shader_id)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader_id,b_load_texture);
	}
		
}

void Model::debug_box_draw(size_t shader_id)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].mesh_bounding_box.Draw();
	}
}

glm::mat4 Model::get_model_matrix()
{
	return transformation_matrix;
}

void Model::set_position(float* position)
{
	position_matrix = glm::mat4(1.0f);
	position_matrix = glm::translate(position_matrix, glm::vec3(position[0],position[1],position[2]));
	transformation_matrix = position_matrix * scale_matrix* rotation_matrix;
}
void Model::set_rotation(float* position, float rotation_angle)
{
	rotation_matrix = glm::mat4(1.0f);
	rotation_matrix = glm::rotate(rotation_matrix,glm::radians(rotation_angle),glm::vec3(position[0], position[1], position[2]));
	transformation_matrix = position_matrix * scale_matrix* rotation_matrix;
}

void Model::set_scale(float scale_amount)
{
	scale_matrix = glm::mat4(1.0f);
	scale_matrix = glm::scale(scale_matrix, glm::vec3(scale_amount, scale_amount, scale_amount));
	transformation_matrix = position_matrix * scale_matrix* rotation_matrix;
}

void Model::translate(float x, float y, float z)
{
	this->transformation_matrix = glm::translate(transformation_matrix, glm::vec3(x,y,z));
}

void Model::translate(glm::vec3 translation)
{
	this->transformation_matrix = glm::translate(transformation_matrix, translation);
}

void Model::rotate(float angle, glm::vec3 rotate_value)
{
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(angle), rotate_value);
}

void Model::scale(float x, float y, float z)
{
	transformation_matrix = glm::scale(transformation_matrix, glm::vec3(x,y,z));
}

void Model::reset_model_matrix()
{
	scale_matrix = glm::mat4(1.0f);
	rotation_matrix = glm::mat4(1.0f);
	position_matrix = glm::mat4(1.0f);
	transformation_matrix = glm::mat4(1.0f);
}

void Model::load_model(string path, bool b_load_textures)
{
	Assimp::Importer import;
	cout << path << " loading...";
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	process_node(scene->mRootNode, scene, b_load_textures);
	cout <<" OK" << endl;
}

void Model::process_node(aiNode * node, const aiScene * scene, bool b_load_textures)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene, b_load_textures));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene,b_load_textures);
	}
}

Mesh Model::process_mesh(aiMesh * mesh, const aiScene * scene, bool b_load_textures)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Bounding_Box b_box;
	float min_x = 99999;
	float min_y = 99999;
	float min_z = 99999;

	float max_x = -99999;
	float max_y = -99999;
	float max_z = -99999;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		if (vector.x > max_x)
			max_x = vector.x;
		else if (vector.x < min_x)
			min_x = vector.x;
		if (vector.y > max_y)
			max_y = vector.y;
		else if (vector.y < min_y)
			min_y = vector.y;
		if (vector.z > max_z)
			max_z = vector.z;
		else if (vector.z < min_z)
			min_z = vector.z;
		
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) 
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		if (mesh->HasTangentsAndBitangents())
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;

		}
		vertices.push_back(vertex);
	}
	b_box = Bounding_Box(glm::vec3(min_x, max_y, min_z),
		glm::vec3(max_x, max_y, min_z),
		glm::vec3(min_x, max_y, max_z),
		glm::vec3(max_x, max_y, max_z),
		glm::vec3(min_x, min_y, min_z),
		glm::vec3(max_x, min_y, min_z),
		glm::vec3(min_x, min_y, max_z),
		glm::vec3(max_x, min_y, max_z));


	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (b_load_textures)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 1. diffuse maps
		vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	

	return Mesh(vertices, indices, textures,b_box);
}

vector<Texture> Model::load_material_textures(aiMaterial * mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; 
				break;
			}
		}
		if (!skip)
		{  
			Texture texture;
			texture.id = texture_from_file(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  
		}
	}
	return textures;
}

unsigned int Model::texture_from_file(const char *path, const string &directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


