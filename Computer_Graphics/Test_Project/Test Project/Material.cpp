#include "Material.h"

CMaterial::CMaterial(std::string path, std::string extension)
{
	albedo_texture = new CTexture((char*)(path + "albedo." + extension).c_str());
	normal_texture = new CTexture((char*)(path + "normal." + extension).c_str());
	metallic_texture = new CTexture((char*)(path + "metallic." + extension).c_str());
	roughness_texture = new CTexture((char*)(path + "roughness." + extension).c_str());
	ao_texture = new CTexture((char*)(path + "ao.png").c_str());
}

CMaterial::~CMaterial()
{
	delete albedo_texture;
	delete normal_texture;
	delete metallic_texture;
	delete roughness_texture;
	delete ao_texture;
}

void CMaterial::use_material()
{
	glActiveTexture(GL_TEXTURE0);
	albedo_texture->bind_texture();
	glActiveTexture(GL_TEXTURE1);
	normal_texture->bind_texture();
	glActiveTexture(GL_TEXTURE2);
	metallic_texture->bind_texture();
	glActiveTexture(GL_TEXTURE3);
	roughness_texture->bind_texture();
	glActiveTexture(GL_TEXTURE4);
	ao_texture->bind_texture();
}
