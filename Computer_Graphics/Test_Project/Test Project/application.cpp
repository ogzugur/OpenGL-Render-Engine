#include "application.h"
#include "rendering_context.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"


Application* Application::s_pApplication = NULL;

Application::Application()
{
	s_pApplication = this;
}

Application::~Application()
{
}

bool Application::OpenWindow(size_t iWidth, size_t iHeight, bool bFullscreen, bool bResizeable)
{

	int ret = glfwInit();

	if (!ret) {
		printf("glfwInit failed\n");
		exit(1);
	}

	m_bFullscreen = bFullscreen;


	m_iWindowWidth = iWidth;
	m_iWindowHeight = iHeight;
	firstMouse = true;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	if (!(window = glfwCreateWindow(iWidth, iHeight, "Window 1", NULL, NULL)))
	{
		glfwTerminate();
		return false;
	}
	ImGui_ImplGlfwGL3_Init(window, true);

	float lastX = iWidth / 2.0f;
	float lastY = iHeight / 2.0f;
	bool firstMouse = true;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);

	glfwSetTime(0.0);



	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -1;
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glLineWidth(1.0);

	glDisable(GL_CULL_FACE);

	m_bIsOpen = true;

	m_pRenderer = CreateRenderer();
	m_pRenderer->Initialize();

	main_rc = new RenderingContext(m_pRenderer);



	return true;
}

void Application::SetMouseCursorEnabled(bool bEnabled)
{
	if (bEnabled)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	m_bMouseEnabled = bEnabled;
}

void Application::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
	getApplication()->m_iWindowHeight = height;
	getApplication()->m_iWindowWidth = width;
}

void Application::mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (Get()->IsMouseRightDown())
	{
		Get()->mouse_pos(xpos, ypos);
	}
	if (!Get()->IsMouseRightDown())
	{
		Get()->firstMouse = true;
	}
}

void Application::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			getApplication()->keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			getApplication()->keys[key] = false;
			getApplication()->keysPressed[key] = false;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (getApplication()->polygon_mode == GL_FILL)
			getApplication()->polygon_mode = GL_LINE;
		else if (getApplication()->polygon_mode == GL_LINE)
			getApplication()->polygon_mode = GL_FILL;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		getApplication()->debug_box_draw = !getApplication()->debug_box_draw;
	}
}

void Application::mouse_pos(double x, double y)
{
	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

	lastX = x;
	lastY = y;

	GetRenderer()->camera->ProcessMouseMovement(xoffset, yoffset);
}

bool Application::WindowShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Application::processInput(float dt)
{
	if (keys[GLFW_KEY_W])
		GetRenderer()->camera->ProcessKeyboard(FORWARD, dt);
	if (keys[GLFW_KEY_S])
		GetRenderer()->camera->ProcessKeyboard(BACKWARD, dt);
	if (keys[GLFW_KEY_A])
		GetRenderer()->camera->ProcessKeyboard(LEFT, dt);
	if (keys[GLFW_KEY_D])
		GetRenderer()->camera->ProcessKeyboard(RIGHT, dt);
	if (keys[GLFW_KEY_E])
		GetRenderer()->camera->ProcessKeyboard(UP, dt);
	if (keys[GLFW_KEY_Q])
		GetRenderer()->camera->ProcessKeyboard(DOWN, dt);

}

float Application::GetTime()
{
	return (float)glfwGetTime();
}

void Application::GameLoop()
{
	float flPreviousTime = 0;
	float flCurrentTime = getApplication()->GetTime();
	init();

	while (!Application::Get()->WindowShouldClose())
	{
		glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
		glfwPollEvents();

		flPreviousTime = flCurrentTime;
		flCurrentTime = getApplication()->GetTime();

		float dt = flCurrentTime - flPreviousTime;

		if (dt > 0.15f)
			dt = 0.15f;

		processInput(dt);
		Update(dt);
		Draw();
		glfwSwapBuffers(window);
	}
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();


}

void Application::init()
{
	bloor_downsample = 8;

	selected_model = models[0];
	models[1].rotate(-90,glm::vec3(1.0, 0.0,0.0));


	GetRenderer()->light_position.x = GetRenderer()->p_light.light_position.x;
	GetRenderer()->light_position.y = GetRenderer()->p_light.light_position.y;
	GetRenderer()->light_position.z = GetRenderer()->p_light.light_position.z;


	for (int i = 0; i<4 ; i++)
	{
		point_lights[i] = point_light(light_color, lightPositions[i]);
	}
	


	InitHdrBuffers();
	InitPBR();
	InitGBuffer();

}
void Application::Update(float dt)
{

	selected_model.rotate(model_rotation_speed * dt, glm::vec3(0.0, 1.0, 0.0));

	for (int i = 0; i < 4 ; i++)
	{
		point_lights[i].light_color = light_color;
	}

	if (!is_vsync_updated)
	{
		glfwSwapInterval(is_vsync_on);
		is_vsync_updated = true;
	}
	frame_frustum = Frustum(main_rc->rc_projection*main_rc->rc_view);
}

void Application::Draw()
{
	PBRDraw();
	draw_imgui();
}

void Application::GbufferDebugDraw()
{
	GeometryPass();
	debug_Gbuffer();
}
void Application::PBRDraw()
{
	main_rc->ClearDepth();
	main_rc->ClearColor();

	GetRenderer()->StartRendering(main_rc);



	GeometryPass();
	
	beginPBRRender();
	endPBRRender();
	
	
	

	//BlurPass();

	PostProcessPass();
	

	if (draw_background)
	{
		DrawBackground();
	}
	
}
void Application::PhongDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	main_rc->ClearDepth();
	main_rc->ClearColor();

	GetRenderer()->StartRendering(main_rc);



	main_rc->UseProgram("testShader");
	setTestShaderUniforms();
	RenderScene();


	main_rc->UseProgram("lightSource");
	setLightShaderUniforms();
	models[1].Draw(main_rc->m_iProgram);


	if (debug_box_draw)
	{
		main_rc->UseProgram("boundingBox");
		setBBUniforms();
		models[0].debug_box_draw(main_rc->m_iProgram);
	}

	BlurPass();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glViewport(0, 0, m_iWindowWidth, m_iWindowHeight);
	RenderFullScreenQuad();
}
void Application::DrawBackground()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, 0, 0, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	main_rc->UseProgram("backgroundShader");

	main_rc->SetUniform("projection", main_rc->rc_projection);
	main_rc->SetUniform("view", main_rc->rc_view);
	main_rc->SetUniform("mipLevel", envMipLevel);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	renderCube();
}
void Application::RenderScene()
{
	drawed_meshes = 0;

	for (unsigned int i = 0; i < models[0].meshes.size(); i++)
	{
		if (frame_frustum.boxIntersection(models[0].meshes[i].mesh_bounding_box))
		{
			drawed_meshes++;
			models[0].meshes[i].Draw(main_rc->m_iProgram, models[0].b_load_texture);
		}
	}
}
void Application::RenderFullScreenQuad()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	main_rc->UseProgram("bloomFinal");
	glActiveTexture(GL_TEXTURE0);
	main_rc->SetUniform("scene", 0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]); //colorBuffers[0]

	glActiveTexture(GL_TEXTURE1);
	main_rc->SetUniform("bloomBlur", 1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[horizontal]);


	main_rc->SetUniform("bloom", bloom);
	main_rc->SetUniform("exposure", exposure);
	renderQuad();
}

void Application::BlurPass()
{
	GLboolean first_iteration = true;
	GLuint amount = 10;
	main_rc->UseProgram("blurShader");
	for (GLuint i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glViewport(0, 0, m_iWindowWidth / bloor_downsample, m_iWindowHeight / bloor_downsample);
		main_rc->SetUniform("horizontal", horizontal);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		renderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glViewport(0, 0, m_iWindowWidth, m_iWindowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Application::PostProcessPass()
{
	main_rc->UseProgram("postProcess");
	main_rc->SetUniform("screenTexture", 0);
	main_rc->SetUniform("bloomBlur", 1);
	main_rc->SetUniform("bloom", bloom);
	main_rc->SetUniform("vignette", vignette);
	main_rc->SetUniform("chromatic_aberration", chromatic_aberration);
	main_rc->SetUniform("exposure", exposure);
	main_rc->SetUniform("screenTextureSize", glm::vec2(1 / getApplication()->m_iWindowWidth, 1 / getApplication()->m_iWindowHeight));
	main_rc->SetUniform("ca_offset", ca_offset);
	main_rc->SetUniform("ca_strength", ca_strength);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1]);
	renderQuad();
}

void Application::beginPBRRender()
{
	main_rc->UseProgram("pbrShader");
	main_rc->SetUniform("bloom_threshold", bloom_threshold);
	main_rc->SetUniform("gBufferView", gBufferView);
	main_rc->SetUniform("gPosition", 0);
	main_rc->SetUniform("gAlbedo", 1);
	main_rc->SetUniform("gNormal", 2);
	main_rc->SetUniform("gEffects", 3);
	main_rc->SetUniform("inverseView", glm::transpose(main_rc->rc_view));
	main_rc->SetUniform("inverseView", glm::transpose(main_rc->rc_projection));

	main_rc->SetUniform("projection", main_rc->rc_projection);
	main_rc->SetUniform("view", main_rc->rc_view);
	main_rc->SetUniform("model", models[0].get_model_matrix());
	main_rc->SetUniform("camPos", GetRenderer()->camera->Position);

	for (int i = 0; i<4 ; i++)
	{
		main_rc->SetUniform("lightPositions[" + std::to_string(i) + "]", point_lights[i].light_position);
		main_rc->SetUniform("lightColors[" + std::to_string(i) + "]", glm::vec3(point_lights[i].light_color.x*light_density, point_lights[i].light_color.y*light_density, point_lights[i].light_color.z*light_density));
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gEffects);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
}
void Application::endPBRRender()
{
	main_rc->BindFrameBuffer(hdrFBO);
	renderQuad();
	main_rc->UnbindFrameBuffer();
}
void Application::setTestShaderUniforms()
{
	GetRenderer()->dir_light.set_uniform_data(getApplication()->main_rc->m_iProgram, "sun_light");
	GetRenderer()->p_light.set_uniform_data(getApplication()->main_rc->m_iProgram, "point_light");
	main_rc->SetUniform("model", models[0].get_model_matrix());
	main_rc->SetUniform("projection", main_rc->rc_projection);
	main_rc->SetUniform("view", main_rc->rc_view);
	main_rc->SetUniform("viewPos", GetRenderer()->camera->Position);
	main_rc->SetUniform("is_directional_light_on", is_directional_light_on);
	main_rc->SetUniform("texture_channel", getApplication()->texture_channel);
	main_rc->SetUniform("bloom_threshold", bloom_threshold);
}
void Application::setLightShaderUniforms()
{
	main_rc->SetUniform("bloom_threshold", bloom_threshold);
	main_rc->SetUniform("model", models[1].get_model_matrix());
	main_rc->SetUniform("projection", main_rc->rc_projection);
	main_rc->SetUniform("view", main_rc->rc_view);
	main_rc->SetUniform("color", glm::vec3(GetRenderer()->p_light.light_color.x * 2, GetRenderer()->p_light.light_color.y * 2, GetRenderer()->p_light.light_color.z * 2));
}
void::Application::setBBUniforms()
{
	main_rc->SetUniform("model", models[0].get_model_matrix());
	main_rc->SetUniform("projection", main_rc->rc_projection);
	main_rc->SetUniform("view", main_rc->rc_view);
}



void Application::renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
																  // front face
			 -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			 -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
																 // left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
																 // right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
																 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
void Application::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}



void Application::InitHdrBuffers()
{
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	glGenTextures(2, colorBuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(2, attachments);
	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getApplication()->m_iWindowWidth / bloor_downsample, getApplication()->m_iWindowHeight / bloor_downsample, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// Also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Application::InitPBR()
{
	set_pbr_texture_uniforms();
	load_pbr_materials();
	InitPbrBuffers();
	load_hdr_envmap_textures();
	setup_cubemap(*hdrCanyonTexture);
	create_LUT_from_BRDF();

}
void Application::load_pbr_materials()
{
	ball_material = new CMaterial("resources/textures/ball/","tga");
	gold_material = new CMaterial("resources/textures/gold/","png");
	plastic_material = new CMaterial("resources/textures/plastic/","png");
	metal_material = new CMaterial("resources/textures/metal/","png");
	cerberus_material = new CMaterial("resources/textures/cerberus/","png");

	object_material = ball_material;
}
void Application::load_hdr_envmap_textures()
{
	hdrCanyonTexture = new CTexture("resources/textures/hdr/canyon.hdr", HDR);
	hdrAppartEnvmapTexture = new CTexture("resources/textures/hdr/appart.hdr", HDR);
	hdrPortEnvmapTexture = new CTexture("resources/textures/hdr/newport_loft.hdr", HDR);
	hdrUffiziEnvmapTexture = new CTexture("resources/textures/hdr/uffizi_large.hdr", HDR);
	hdrNoonGrassEnvmapTexture = new CTexture("resources/textures/hdr/pond_bridge_night_4k.hdr", HDR);


	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}
void Application::set_pbr_texture_uniforms()
{

	main_rc->UseProgram("pbrShader");
	main_rc->UseProgram("backgroundShader");
	main_rc->SetUniform("environmentMap", 0);
	main_rc->SetUniform("prefilterMap", 1);

	main_rc->UseProgram("pbrShader");
	main_rc->SetUniform("irradianceMap", 4);
	main_rc->SetUniform("prefilterMap", 5);
	main_rc->SetUniform("brdfLUT", 6);


}
void Application::create_irradiance_cubemap()
{
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	main_rc->UseProgram("irradianceShader");
	main_rc->SetUniform("environmentMap", 0);
	main_rc->SetUniform("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
							  //glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		main_rc->SetUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
void Application::create_prefilter_cubemap()
{
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	main_rc->UseProgram("prefilterShader");
	main_rc->SetUniform("environmentMap", 0);
	main_rc->SetUniform("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		main_rc->SetUniform("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			main_rc->SetUniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Application::create_LUT_from_BRDF()
{
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	main_rc->UseProgram("brdfShader");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Application::equirectangular_to_cubemap(CTexture envmap_texture)
{
	main_rc->UseProgram("equirectangularToCubemapShader");
	main_rc->SetUniform("equirectangularMap", 0);
	main_rc->SetUniform("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, envmap_texture.get_texture_id());

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		main_rc->SetUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
void Application::setup_cubemap(CTexture envmap_texture)
{
	InitPbrBuffers();
	equirectangular_to_cubemap(envmap_texture);
	create_irradiance_cubemap();
	create_prefilter_cubemap();
}
void Application::InitPbrBuffers()
{
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
}

void Application::InitGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// Position
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// Albedo + Roughness
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo, 0);

	// Normals + Metalness
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

	// Effects (AO + Velocity)
	glGenTextures(1, &gEffects);
	glBindTexture(GL_TEXTURE_2D, gEffects);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEffects, 0);

	// Define the COLOR_ATTACHMENTS for the G-Buffer
	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// Z-Buffer
	glGenRenderbuffers(1, &zBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getApplication()->m_iWindowWidth, getApplication()->m_iWindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

	// Check if the framebuffer is complete before continuing
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete !" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}
void Application::GeometryPass()
{
	main_rc->BindFrameBuffer(gBuffer);


	// Model(s) rendering
	main_rc->UseProgram("gbufferSetupShader");

	main_rc->SetUniform("projection",main_rc->rc_projection);
	main_rc->SetUniform("view", main_rc->rc_view);



	projViewModel = main_rc->rc_projection * main_rc->rc_view * selected_model.get_model_matrix();

	main_rc->SetUniform("projViewModel", projViewModel);
	main_rc->SetUniform("prevProjViewModel", prevProjViewModel);
	main_rc->SetUniform("model", selected_model.get_model_matrix());


	object_material->use_material();

	set_gbuffer_uniforms();

	selected_model.Draw(main_rc->m_iProgram);

	main_rc->UnbindFrameBuffer();

	prevProjViewModel = projViewModel;
}
void Application::set_gbuffer_uniforms()
{
	main_rc->UseProgram("gbufferSetupShader");
	main_rc->SetUniform("texAlbedo", 0);
	main_rc->SetUniform("texNormal", 1);
	main_rc->SetUniform("texMetalness", 2);
	main_rc->SetUniform("texRoughness", 3);
	main_rc->SetUniform("texAO", 4);
}
void Application::debug_Gbuffer()
{

	main_rc->ClearDepth();
	main_rc->ClearColor();

	GetRenderer()->StartRendering(main_rc);

	main_rc->UseProgram("gbufferDebugShader");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gEffects);

	main_rc->SetUniform("gPosition", 0);
	main_rc->SetUniform("gAlbedo", 1);
	main_rc->SetUniform("gNormal", 2);
	main_rc->SetUniform("gEffects", 3);

	main_rc->SetUniform("gBufferView", gBufferView);
	renderQuad();
}

void Application::draw_imgui()
{
	ImGui_ImplGlfwGL3_NewFrame();

	static int selected_model = -1;
	ImGui::Begin("Test Window", &show_ogz_window);
	if (ImGui::TreeNode("G-Buffer"))
	{
		ImGui::RadioButton("final", &gBufferView, 1);
		ImGui::RadioButton("position", &gBufferView, 2);
		ImGui::RadioButton("normal", &gBufferView, 3);
		ImGui::RadioButton("albedo", &gBufferView, 4);
		ImGui::RadioButton("roughness", &gBufferView, 5);
		ImGui::RadioButton("metallness", &gBufferView, 6);
		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Model"))
	{
		if (show_model_inspector)
		{
			ImGui::Begin("Model Inspector", &show_model_inspector);
			if (ImGui::DragFloat3("Position", position, 0.5f, -350.0f, 350.0f))
			{
				getApplication()->selected_model.set_position(position);
			}
			if (ImGui::DragFloat4("Rotation", rotation_axis, 0.2, -360.0, 360.0));
			{
				getApplication()->selected_model.set_rotation(rotation_axis, rotation_axis[3]);
			}
			if (ImGui::DragFloat("Scale", &scale_amount, 0.1, 0.1, 360.0));
			{
				getApplication()->selected_model.set_scale(scale_amount);
			}
			if (ImGui::Button("Reset Position"))
			{	
				memcpy(position, default_position, sizeof(position));
				memcpy(rotation_axis, default_rotation, sizeof(rotation_axis));
				scale_amount = 1.0f;
				getApplication()->selected_model.reset_model_matrix();
			}
			ImGui::End();
		}
		if (ImGui::Button("Model Inspector")) show_model_inspector ^= 1;
		if (ImGui::Combo("Model", &selected_model_id, "Ball\0Cerberus\0\0"))
		{
			if (selected_model_id == 0)
				getApplication()->selected_model = models[0];
			else if(selected_model_id == 1)
				getApplication()->selected_model = models[1];
		}

		ImGui::Text("Rotation Speed");
		ImGui::DragFloat("R", &model_rotation_speed, 0.5f);
		if (ImGui::Combo("Material", &selected_material, "Ball\0Gold\0Plastic\0Metal\0Cerberus\0\0"))
		{

			if (selected_material == 0)
			{
				object_material = ball_material;
			}
			else if (selected_material == 1)
			{
				object_material = gold_material;
			}

			else if (selected_material == 2)
			{
				object_material = plastic_material;
			}

			else if (selected_material == 3)
			{
				object_material = metal_material;
			}
			else if (selected_material == 4)
			{
				object_material = cerberus_material;
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Environment Maps"))
	{
		ImGui::SliderInt("Mip Level", &envMipLevel,0,4);
		if (ImGui::Combo("Env Maps", &selected_envmap, "Canyon\0Appart\0Port\0Uffizi\0NoonGrass\0"))
		{
			if (selected_envmap == 0)
			{
				setup_cubemap(*hdrCanyonTexture);
			}
			else if (selected_envmap == 1)
			{
				setup_cubemap(*hdrAppartEnvmapTexture);
			}
			else if (selected_envmap == 2)
			{
				setup_cubemap(*hdrPortEnvmapTexture);
			}
			else if (selected_envmap == 3)
			{
				setup_cubemap(*hdrUffiziEnvmapTexture);
			}
			else if (selected_envmap == 4)
				setup_cubemap(*hdrNoonGrassEnvmapTexture);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Light"))
	{
		ImGui::ColorEdit3("Light Color", (float*)&light_color);
		ImGui::DragFloat("Light Density", &light_density, 10.0f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Camera"))
	{
		ImGui::Text("Camera Position");
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "%.2f    ", GetRenderer()->camera->Position.x); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%.2f    ", GetRenderer()->camera->Position.y); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "%.2f    ", GetRenderer()->camera->Position.z);
		ImGui::DragFloat("Camera Speed", &GetRenderer()->camera->MovementSpeed, 0.05f);
		ImGui::DragFloat("Camera FOV", &GetRenderer()->camera->CameraFOV, 1.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Post Process"))
	{
		if (ImGui::TreeNode("Bloom"))
		{
			ImGui::Checkbox("Bloom", &bloom);
			ImGui::SliderFloat("Exposure", &(float)exposure, 0.0f, 5.0f);
			ImGui::SliderFloat("BloomThreshold", &(float)bloom_threshold, 0.0f, 5.0f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Chromatic Aberration"))
		{
			ImGui::DragFloat("Offset", &ca_offset, 0.1f,-10.0f,10.0f);
			ImGui::DragFloat("Strength", &ca_strength, 0.1f,0.5f,3.0f);
			ImGui::Checkbox("Chromatic Aberration", &chromatic_aberration);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Vignette"))
		{
			ImGui::Checkbox("Vignette", &vignette);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::Checkbox("V-Sync", &is_vsync_on))
	{
		is_vsync_updated = false;
	}
	if (ImGui::Checkbox("Draw Background", &draw_background))
	{

	}

	if (!ImGui::CollapsingHeader("About"))
	{
		ImGui::Text((char*)glGetString(GL_VERSION));
		ImGui::Text((char*)glGetString(GL_RENDERER));
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Drawed Meshes:%d", drawed_meshes);

	}
	ImGui::End();

	ImGui::Render();
}

inline class Renderer * Application::CreateRenderer()
{
	return new Renderer(m_iWindowWidth, m_iWindowHeight);
}

bool Application::IsMouseRightDown()
{
	return glfwGetMouseButton(Get()->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

bool Application::IsMouseRightRelease()
{
	return glfwGetMouseButton(Get()->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE;
}
