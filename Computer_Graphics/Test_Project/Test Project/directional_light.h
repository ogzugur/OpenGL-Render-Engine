#pragma once
#include "common.h"

class directional_light
{
public:
	ImVec4 light_color;
	glm::vec3 light_direction;

	float ambient_color;

	void set_uniform_data(GLuint shader_id, string light_variable_name);

	directional_light();
	directional_light(ImVec4 light_color, glm::vec3 light_direction, float ambient_color);
		
};