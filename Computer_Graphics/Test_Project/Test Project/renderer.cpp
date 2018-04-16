#include "renderer.h"


Renderer::Renderer(int width, int height)
{

	m_bRenderOrthographic = false;

	m_bUseMultisampleTextures = !!glTexImage2DMultisample;

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glGetIntegerv(GL_SAMPLES, &m_iScreenSamples);

	SetSize(width, height);

	m_bDrawBackground = true;


	//InitBunny();
}

void Renderer::StartRendering(RenderingContext * pContext)
{
	m_iWidth = getApplication()->GetWindowWidth();
	m_iHeight = getApplication()->GetWindowHeight();

	float flAspectRatio = (float)m_iWidth / (float)m_iHeight;

		pContext->SetProjection(glm::perspective(
			glm::radians(camera->CameraFOV),
			flAspectRatio,
			camera->CameraNear,
			camera->CameraFar
		));

	pContext->SetView(camera->GetViewMatrix());
	
	glViewport(0, 0, (GLsizei)m_iWidth, (GLsizei)m_iHeight);
}


void Renderer::StartShadowRendering(RenderingContext * pContext)
{

	//glBindFramebuffer(GL_FRAMEBUFFER,pContext->depth_buffer->get_buffer());
	pContext->depth_buffer->bind_buffer();

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 50.0f;
	float scale = 2;

	lightProjection = glm::ortho(-10.0f*scale, 10.0f*scale, -10.0f*scale, 10.0f*scale, near_plane, far_plane);

	//lightProjection = glm::perspective(45.0f, (GLfloat)1024 / (GLfloat)1024, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.

	lightView = glm::lookAt(glm::vec3(10.0, 20.0, 10.0), getApplication()->GetRenderer()->dir_light.light_direction, glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	pContext->UseProgram("depthShader");
	pContext->SetUniform("model", getApplication()->models[0].get_model_matrix());
	pContext->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, (GLsizei)1024, (GLsizei)1024);

}

void Renderer::SetSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}

void Renderer::Initialize()
{
	SetCamera();
	LoadShaders();
	LoadModels();
	WindowResize(m_iWidth, m_iHeight);
}
void Renderer::SetCamera()
{
	camera = new Camera(glm::vec3(-2.0f, -1.0f, 20.0f));

}

void Renderer::LoadShaders()
{
	Shader testShader = Shader("resources/shaders/test.vs", "resources/shaders/test.fs","testShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/light_source.vs", "resources/shaders/light_source.fs", "lightSource");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/bounding_box.vs", "resources/shaders/bounding_box.fs", "boundingBox");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/blur_shader.vs", "resources/shaders/blur_shader.fs", "blurShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/post_process.vs", "resources/shaders/post_process.fs", "postProcess");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/pbr_shaders/pbr.vs", "resources/shaders/pbr_shaders/pbr.fs", "pbrShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/pbr_shaders/cubemap.vs", "resources/shaders/pbr_shaders/equirectangular_to_cubemap.fs", "equirectangularToCubemapShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/pbr_shaders/cubemap.vs", "resources/shaders/pbr_shaders/irradiance_convolution.fs", "irradianceShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/pbr_shaders/cubemap.vs", "resources/shaders/pbr_shaders/prefilter.fs", "prefilterShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/pbr_shaders/brdf.vs", "resources/shaders/pbr_shaders/brdf.fs", "brdfShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/pbr_shaders/background.vs", "resources/shaders/pbr_shaders/background.fs", "backgroundShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/deferred/gbuffer.vs", "resources/shaders/deferred/gbuffer.fs", "gbufferSetupShader");
	shaderList.push_back(testShader);
	testShader = Shader("resources/shaders/deferred/gbuffer_debug.vs", "resources/shaders/deferred/gbuffer_debug.fs", "gbufferDebugShader");
	shaderList.push_back(testShader);

}

void Renderer::LoadModels()
{
	Model model = Model("resources/models/ball/export3dcoat.obj",false);
	getApplication()->models.push_back(model);
	model = Model("resources/models/cerberus/cerberus.FBX",false);
	getApplication()->models.push_back(model);
	//"resources/models/sponza_crytek/untitled.obj"
	//resources/models/ball/export3dcoat.obj
	//"resources/models/nanosuit/nanosuit.obj"

}

void Renderer::WindowResize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}
