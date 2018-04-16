#pragma once
#include <glad/glad.h> 
#include "stb_image.h"
#include <iostream>
enum texture_type {
	HDR
};

class CTexture {
public:
	CTexture(char *path);
	CTexture(char *path, texture_type t_type);
	CTexture(unsigned int WIDTH, unsigned int HEIGHT);
	void bind_texture();
	unsigned int get_texture_id();
private:
	unsigned int texture;
};