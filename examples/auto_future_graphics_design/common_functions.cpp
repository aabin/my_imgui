#include "common_functions.h"
#include <math.h>
#include "SOIL.h"
#include "texture.h"

/*
x'=(x-a)cos��+(y-b)sin��+a
y'=-(x-a)sin��+(y-b)cos��+b
*/

ImVec2 rotate_point_by_zaxis(ImVec2& tar, float angle, ImVec2& basePoint)
{
	ImVec2 des;
	des.x = (tar.x - basePoint.x)*cos(angle) + (tar.y - basePoint.y)*sin(angle) + basePoint.x;
	des.y = -(tar.x - basePoint.x)*sin(angle) + (tar.y - basePoint.y)*cos(angle) + basePoint.y;
	return des;
}
bool prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// ���� ���� color attachment
	colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGB, frame_width, frame_height, GL_RGB, GL_UNSIGNED_BYTE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
	// ���� depth stencil texture attachment
	depthStencilTextId = TextureHelper::makeAttachmentTexture(0, GL_DEPTH24_STENCIL8, frame_width,
		frame_height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);
	// ���������
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
/*
* ��������Color Attachment
* ͬʱ����RBO��depth stencil Attachment
*/
bool prepareFBO2(GLuint& textId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// �������� color attachment
	textId = TextureHelper::makeAttachmentTexture(0, GL_RGB, frame_width, frame_height, GL_RGB, GL_UNSIGNED_BYTE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textId, 0);
	// ���� depth stencil RBO attachment
	GLuint rboId;
	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		frame_width, frame_height); // Ԥ�����ڴ�
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}