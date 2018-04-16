#include "framebuffer.h"

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &FBO);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteBuffers(1, &FBO);
}

void FrameBuffer::attach_texture(CTexture &texture)
 {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,texture.get_texture_id(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::get_buffer()
{
	return FBO;
}
void FrameBuffer::bind_buffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::unbind_buffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




