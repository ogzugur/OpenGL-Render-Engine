#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <string>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "renderer.h"
#include "model.h"
#include "frustum.h"
#include "texture.h"
#include "framebuffer.h"
#include "Material.h"
#include "imgui.h"
#include "point_light.h"

class Application
{
public:
	Application();
	~Application();

	std::vector<Model> models;
	Model selected_model;
	Frustum                     frame_frustum;
	bool						OpenWindow(size_t width, size_t height, bool fullscreen, bool resizeable);
	void						SetMouseCursorEnabled(bool bEnabled);



	static void						framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static void						mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void						key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void							mouse_pos(double x, double y);

	bool							IsMouseRightDown();
	bool							IsMouseRightRelease();

	void InitHdrBuffers();

	void load_pbr_materials();

	void load_hdr_envmap_textures();

	bool							WindowShouldClose();

	int							GetWindowWidth() { return (int)m_iWindowWidth; };
	int							GetWindowHeight() { return (int)m_iWindowHeight; };

	void processInput(float dt);
	float GetTime();
	void GameLoop();

	void init();


	void Update(float dt);

	void Draw();

	void PBRDraw();

	void PhongDraw();

	void DrawBackground();

	void RenderScene();

	void RenderFullScreenQuad();

	void BrightPass();

	void BlurPass();

	void PostProcessPass();

	void beginPBRRender();

	void endPBRRender();

	void setTestShaderUniforms();

	void setLightShaderUniforms();

	void renderCube();

	void renderQuad();

	void draw_imgui();



	void set_pbr_texture_uniforms();

	void create_irradiance_cubemap();

	void create_prefilter_cubemap();

	void create_LUT_from_BRDF();

	void equirectangular_to_cubemap(CTexture envmap_texture);

	void setup_cubemap(CTexture envmap_texture);

	void InitPbrBuffers();

	void InitPBR();

	class Renderer*			CreateRenderer();
	class Renderer*			GetRenderer() { return m_pRenderer; }



	static Application*			Get() { return s_pApplication; };
	GLFWwindow*					window;
	GLenum						draw_mode;
	bool						show_ogz_window = true;
	bool						show_model_inspector = false;
	bool						is_texture_bind = false;
	bool						is_vsync_on = false;
	bool						is_vsync_updated = false;
	bool						is_directional_light_on = true;
	bool						debug_box_draw = false;
	bool						draw_background = true;
	bool						msaa = true;


	int							texture_channel = 0;
	int							drawed_meshes = 0;
	class RenderingContext*		main_rc;

	GLuint						hdrFBO;
	GLuint						colorBuffers[2];
	GLuint						rboDepth;
	GLenum  attachments[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	GLuint						pingpongFBO[2];
	GLuint						pingpongColorbuffers[2];
	GLboolean					horizontal = true;
	bool						bloom = false; 
	bool						chromatic_aberration = true;
	bool						vignette = true;
	GLfloat						exposure = 1.0f;
	GLfloat						bloom_threshold = 1.0f;
	GLfloat						bloor_downsample = 4;
	GLfloat						ca_strength = 1.0f;
	GLfloat						ca_offset = 2.50f;

	float model_rotation_speed = 0.0f;
	float default_position[4] = { 0.0f, 0.0f, 0.0f,0.0f };
	float default_rotation[4] = { 0.0f, 1.0f, 0.0f,0.0f };
	float position[4] = { 0.0f, 0.0f, 0.0f,0.0f};
	float rotation_axis[4] = { 0.0f, 1.0f, 0.0f,0.0 };
	float scale_amount = 1.0f;;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

public:
	//GBUFFER
	glm::mat4 projViewModel;
	glm::mat4 prevProjViewModel = projViewModel;

	GLuint gBuffer, zBuffer;
	GLuint gPosition, gNormal, gAlbedo, gEffects;
	int gBufferView = 1;

	void InitGBuffer();
	void GeometryPass();
	void set_gbuffer_uniforms();
	void debug_Gbuffer();
	void GbufferDebugDraw();


public:
	int selected_material = 0;
	int selected_model_id = 0;
	int selected_envmap = 0;
	int envMipLevel = 1;

	GLuint captureFBO;
	GLuint captureRBO;

	CTexture* hdrCanyonTexture;
	CTexture* hdrAppartEnvmapTexture;
	CTexture* hdrPortEnvmapTexture;
	CTexture* hdrUffiziEnvmapTexture;
	CTexture* hdrNoonGrassEnvmapTexture;

	GLuint brdfLUTTexture;

	GLuint irradianceMap;
	GLuint prefilterMap;
	GLuint envCubemap;

	glm::mat4 captureProjection;
	std::vector<glm::mat4> captureViews;

	CMaterial* object_material;
	CMaterial* ball_material;
	CMaterial* gold_material;
	CMaterial* plastic_material;
	CMaterial* metal_material;
	CMaterial* cerberus_material;

	glm::vec3 lightPositions[4] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	

	point_light point_lights[4];
	ImVec4 light_color = ImVec4(1.0,1.0,1.0,1.0);
	float light_density = 300;

protected:
	bool						keys[1024];
	bool						keysPressed[1024];

	size_t						m_iWindowWidth;
	size_t						m_iWindowHeight;
	bool						m_bFullscreen;
	bool						m_bIsOpen;
	GLenum						polygon_mode = GL_FILL;

	bool					firstMouse;
	double						lastX;
	double						lastY;

	bool						m_bMultisampling;
	bool						m_bMouseEnabled;

	class Renderer*			m_pRenderer;


	static Application*		s_pApplication;
	void setBBUniforms();
};

inline Application* getApplication()
{
	return Application::Get();
}

#endif