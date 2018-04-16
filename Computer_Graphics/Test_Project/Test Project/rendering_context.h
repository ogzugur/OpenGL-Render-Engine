#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "imgui.h"
#include "shader.h"
#include "model.h"
#include "render_common.h"

#define PROGRAM_LEN 32

class RenderingContext
{
protected:
	class RenderContext
	{
	public:
		RenderContext();


	public:
		glm::mat4			m_mProjection;
		glm::mat4			m_mView;
		glm::mat4			m_mTransformations;

		bool                m_bProjectionUpdated;
		bool                m_bViewUpdated;
		bool                m_bTransformUpdated;

		const class FrameBuffer*	m_pFrameBuffer;
		char				m_szProgram[PROGRAM_LEN];	
		class Shader*		m_pShader;

		int                 m_iViewportX;
		int                 m_iViewportY;
		int                 m_iViewportWidth;
		int                 m_iViewportHeight;

		blendtype_t			m_eBlend;
		float				m_flAlpha;
		bool				m_bDepthMask;
		bool				m_bDepthTest;
		depth_function_t	m_eDepthFunction;
		bool				m_bCull;
		bool				m_bWinding;
	};

	
protected:
	inline RenderContext&	GetContext() { return s_aContexts.back(); }

public:
	RenderingContext(class Renderer * pRenderer = nullptr, bool bInherit = false);
	void BindFrameBuffer(GLuint buffer_name);
	void UnbindFrameBuffer();
	void UseProgram(std::string shaderName);
	void SetUniform(const std::string & pszName, const glm::vec3 mValue);
	void SetUniform(const std::string & pszName, const glm::vec2 mValue);
	void SetUniform(const std::string & pszName, const ImVec4 mValue);
	void SetUniform(const char * pszName, const glm::vec3 mValue);
	void SetUniform(const std::string &pszName, const glm::mat4 mValue);
	void SetUniform(const char * pszName, const bool mValue);
	void SetUniform(const char * pszName, const int mValue);
	void SetUniform(const char * pszName, float mValue);
	void SetDepthMask(bool bDepthMask);
	void SetDepthTest(bool bDepthTest);
	void SetDepthFunction(depth_function_t eDepthFunction);
	void SetBackCulling(bool bCull);
	void SetWinding(bool bWinding);
	void BindTexture(size_t iTexture, int iChannel = 0, bool bMultisample = false);
	void SetViewport(int x, int y, int w, int h);


	void					SetProjection(const glm::mat4& m);
	void					SetView(const glm::mat4& m);

	glm::mat4				GetProjection() { return GetContext().m_mProjection; }
	glm::mat4				GetView() { return GetContext().m_mView; }


	void					ClearColor(glm::vec4 clrClear);
	void					ClearColor();
	void					ClearDepth();

	ImVec4					clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
	FrameBuffer             *depth_buffer;

	glm::mat4				rc_projection;
	bool					is_projection_updated;

	glm::mat4				rc_view;
	bool					is_view_updated;

	class Renderer*			m_pRenderer;
	class Shader*			m_pShader;

	size_t					m_iProgram;

	static std::vector<RenderContext> s_aContexts;
	class Model* model;
};