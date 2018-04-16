#define _CRT_SECURE_NO_WARNINGS
#include "rendering_context.h"
#include "renderer.h"
#include <vector>


std::vector<RenderingContext::RenderContext> RenderingContext::s_aContexts;

RenderingContext::RenderingContext(Renderer * pRenderer, bool bInherit)
{
	
		m_pRenderer = pRenderer;

		s_aContexts.push_back(RenderContext());

		if (bInherit && s_aContexts.size() > 1)
		{
			RenderContext& oLastContext = s_aContexts[s_aContexts.size() - 2];
			RenderContext& oThisContext = GetContext();

			oThisContext.m_mProjection = oLastContext.m_mProjection;
			oThisContext.m_mView = oLastContext.m_mView;
			oThisContext.m_mTransformations = oLastContext.m_mTransformations;

			strncpy(oThisContext.m_szProgram, oLastContext.m_szProgram, PROGRAM_LEN);
			oThisContext.m_pShader = oLastContext.m_pShader;

			oThisContext.m_iViewportX = oLastContext.m_iViewportX;
			oThisContext.m_iViewportY = oLastContext.m_iViewportY;
			oThisContext.m_iViewportWidth = oLastContext.m_iViewportWidth;
			oThisContext.m_iViewportHeight = oLastContext.m_iViewportHeight;
			oThisContext.m_eBlend = oLastContext.m_eBlend;
			oThisContext.m_flAlpha = oLastContext.m_flAlpha;
			oThisContext.m_bDepthMask = oLastContext.m_bDepthMask;
			oThisContext.m_bDepthTest = oLastContext.m_bDepthTest;
			oThisContext.m_eDepthFunction = oLastContext.m_eDepthFunction;
			oThisContext.m_bCull = oLastContext.m_bCull;
			oThisContext.m_bWinding = oLastContext.m_bWinding;

			m_pShader = oThisContext.m_pShader;

			if (m_pShader)
				m_iProgram = m_pShader->ID;
			else
				m_iProgram = 0;
		}
		else
		{
			m_pShader = NULL;

			BindTexture(0);
			UseProgram("");

			SetViewport(0, 0, getApplication()->GetWindowWidth(), getApplication()->GetWindowHeight());
			SetDepthMask(true);
			SetDepthTest(true);
			SetDepthFunction(DF_LEQUAL);
			SetBackCulling(false);
			SetWinding(true);
		}
}
void RenderingContext::BindFrameBuffer(GLuint buffer_name)
{
	glBindFramebuffer(GL_FRAMEBUFFER, buffer_name);
	this->ClearColor();
	this->ClearDepth();
}
void RenderingContext::UnbindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void RenderingContext::UseProgram(std::string shaderName)
{
	RenderContext& oContext = GetContext();
	std::vector<Shader>shaderList = getApplication()->GetRenderer()->shaderList;

	for (int i = 0; i < shaderList.size(); i++)
	{
		if (shaderName.compare(shaderList[i].shaderName) == 0)
		{
			oContext.m_pShader = m_pShader = &shaderList[i];
			m_iProgram = m_pShader->ID;
			m_pShader->use();
		}
	}
}
void RenderingContext::SetUniform(const std::string &pszName, const glm::vec3 mValue)
{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName.c_str());
	glUniform3fv(iUniform, 1,glm::value_ptr(mValue));
}
void RenderingContext::SetUniform(const std::string &pszName, const glm::vec2 mValue)
{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName.c_str());
	glUniform2fv(iUniform, 1, glm::value_ptr(mValue));
}
void RenderingContext::SetUniform(const std::string &pszName, const ImVec4 mValue)
{
	glm::vec3 value = glm::vec3(mValue.x, mValue.y, mValue.z);
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName.c_str());
	glUniform3fv(iUniform, 1, glm::value_ptr(value));
}
void RenderingContext::SetUniform(const char* pszName, const glm::vec3 mValue)
{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName);
	glUniform3fv(iUniform, 1, glm::value_ptr(mValue));
}

void RenderingContext::SetUniform(const std::string &pszName, const glm::mat4 mValue)

{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName.c_str());
	glUniformMatrix4fv(iUniform, 1, false, glm::value_ptr(mValue));
}
void RenderingContext::SetUniform(const char* pszName, const bool mValue)

{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName);
	glUniform1i(iUniform, mValue);
}

void RenderingContext::SetUniform(const char* pszName, const int mValue)

{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName);
	glUniform1i(iUniform, mValue);
}
void RenderingContext::SetUniform(const char* pszName, float mValue)

{
	int iUniform = glGetUniformLocation((GLuint)m_iProgram, pszName);
	glUniform1f(iUniform, mValue);
}

void RenderingContext::SetDepthMask(bool bDepthMask)
{
	glDepthMask(bDepthMask);
	GetContext().m_bDepthMask = bDepthMask;
}

void RenderingContext::SetDepthTest(bool bDepthTest)
{
	if (bDepthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	GetContext().m_bDepthTest = bDepthTest;
}

void RenderingContext::SetDepthFunction(depth_function_t eDepthFunction)
{
	if (eDepthFunction == DF_LEQUAL)
		glDepthFunc(GL_LEQUAL);
	else if (eDepthFunction == DF_LESS)
		glDepthFunc(GL_LESS);

	GetContext().m_eDepthFunction = eDepthFunction;
}

void RenderingContext::SetBackCulling(bool bCull)
{
	if (bCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	GetContext().m_bCull = bCull;
}

void RenderingContext::SetWinding(bool bWinding)
{
	GetContext().m_bWinding = bWinding;
	glFrontFace(bWinding ? GL_CCW : GL_CW);
}

void RenderingContext::BindTexture(size_t iTexture, int iChannel, bool bMultisample)
{
	glActiveTexture(GL_TEXTURE0 + iChannel);

	if (bMultisample)
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, (GLuint)iTexture);
	else
		glBindTexture(GL_TEXTURE_2D, (GLuint)iTexture);
}

void RenderingContext::SetViewport(int x, int y, int w, int h)
{
	RenderContext& oContext = GetContext();

	oContext.m_iViewportX = x;
	oContext.m_iViewportY = y;
	oContext.m_iViewportWidth = w;
	oContext.m_iViewportHeight = h;
}

void RenderingContext::SetProjection(const glm::mat4 & m)
{
	rc_projection = m;
	is_projection_updated = false;
}

void RenderingContext::SetView(const glm::mat4 & m)
{
	rc_view = m;
	is_view_updated = false;
}

void RenderingContext::ClearColor(glm::vec4 clrClear)
{
	glClearColor(clrClear.x/255, clrClear.y/255, clrClear.z/255, clrClear.w/255 );
	glClear(GL_COLOR_BUFFER_BIT);
}
void RenderingContext::ClearColor()
{
	glClearColor(clear_color.x , clear_color.y , clear_color.z , clear_color.w );
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderingContext::ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}


RenderingContext::RenderContext::RenderContext()
{
	m_bProjectionUpdated = false;
	m_bViewUpdated = false;
	m_bTransformUpdated = false;
}
