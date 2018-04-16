#pragma once
#include<string>
#include <iostream>
#include "texture.h"

class CMaterial
{
public:
	CMaterial(std::string path, std::string extension);
	~CMaterial();

public:
	void use_material();
private:
	CTexture *albedo_texture;
	CTexture *normal_texture;
	CTexture *metallic_texture;
	CTexture *roughness_texture;
	CTexture *ao_texture;
};