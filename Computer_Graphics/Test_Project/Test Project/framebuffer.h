#pragma once
#include <glad/glad.h> 
#include "texture.h"
class FrameBuffer {

public:
	 FrameBuffer();
	 ~FrameBuffer();
	 void attach_texture(CTexture &texture);
	 unsigned int get_buffer();
	 void bind_buffer();
	 void unbind_buffer();
private:
	unsigned int FBO;
};