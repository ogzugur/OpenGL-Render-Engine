#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include "application.h"
#include "rendering_context.h"
#include "Camera.h"
#include <vector>
#include "directional_light.h"
#include "point_light.h"


class Renderer
{
	friend class RenderingContext;
	
public:
	Renderer(int width, int height);

	std::vector<Shader> shaderList;

	Camera* camera;
	directional_light dir_light;
	point_light		  p_light;

	

	ImVec4            light_direction;
	ImVec4            light_position;

	void			SetCameraPosition(glm::vec3 vecCameraPosition) { camera->Position = vecCameraPosition; };
	void			SetCameraDirection(glm::vec3 vecCameraDirection) { m_vecCameraDirection = vecCameraDirection; };
	void			SetCameraUp(glm::vec3 vecCameraUp) { m_vecCameraUp = vecCameraUp; };
	void			SetCameraFOV(float flFOV) { m_flCameraFOV = flFOV; };
	void			SetCameraOrthoHeight(float flOrthoHeight) { m_flCameraOrthoHeight = flOrthoHeight; };
	void			SetCameraNear(float flNear) { m_flCameraNear = flNear; };
	void			SetCameraFar(float flFar) { m_flCameraFar = flFar; };
	void			SetRenderOrthographic(bool bRenderOrtho) { m_bRenderOrthographic = bRenderOrtho; }
	void			StartRendering(class RenderingContext* pContext);
	void			StartShadowRendering(RenderingContext * pContext);
	bool			ShouldRenderOrthographic() { return m_bRenderOrthographic; }

	void			SetSize(int w, int h);

	void Initialize();

	void SetCamera();

	void LoadShaders();
	void LoadModels();

	void WindowResize(int w, int h);

	static Renderer* s_pRenderer;

public:
	size_t			m_iWidth;
	size_t			m_iHeight;

	glm::vec3		m_vecCameraPosition;
	glm::vec3		m_vecCameraDirection;
	glm::vec3		m_vecCameraUp;
	float			m_flCameraFOV;
	float			m_flCameraOrthoHeight;
	float			m_flCameraNear;
	float			m_flCameraFar;
	bool			m_bRenderOrthographic;

	double			m_aflModelView[16];
	double			m_aflProjection[16];
	int				m_aiViewport[4];

	uint32_t        m_default_vao;
	uint32_t        m_dynamic_mesh_vbo;

	bool			m_bDrawBackground;

	int				m_iScreenSamples;
	bool			m_bUseMultisampleTextures;
};