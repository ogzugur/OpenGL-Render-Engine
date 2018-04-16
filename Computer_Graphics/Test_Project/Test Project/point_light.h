#pragma once
#include"common.h"

class point_light
{
public:
	ImVec4 light_color;
	glm::vec3 light_position;

	float ambient_color;

	float ConstantAtt;
	float LinearAtt;
	float QuadraticAtt;

	void set_uniform_data(GLuint shader_id, string light_variable_name);

	point_light();
	point_light(ImVec4 light_color, glm::vec3 light_position, float ambient_color, float ConstantAtt, float LinearAtt, float QuadraticAtt);
	point_light(ImVec4 light_color, glm::vec3 light_position);
};