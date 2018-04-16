#include "point_light.h"

void point_light::set_uniform_data(GLuint shader_id, string light_variable_name)
{
	int iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".color").c_str());
	glUniform3f(iUniform, light_color.x, light_color.y, light_color.z);

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".position").c_str());
	glUniform3fv(iUniform, 1, glm::value_ptr(light_position));

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".ambient").c_str());
	glUniform1f(iUniform, ambient_color);

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".ConstantAtt").c_str());
	glUniform1f(iUniform, ConstantAtt);

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".LinearAtt").c_str());
	glUniform1f(iUniform, LinearAtt);

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".QuadraticAtt").c_str());
	glUniform1f(iUniform, QuadraticAtt);
}

point_light::point_light()
{
	light_color = ImVec4(1.5f, 1.5f, 1.5f, 0.0f);
	light_position = glm::vec3(0.0f, 250.0f, 0.0f);
	ambient_color = 0.25f;
	ConstantAtt = 1.0f;
	LinearAtt = 0.0014;
	QuadraticAtt = 0.000007;
}

point_light::point_light(ImVec4 light_color, glm::vec3 light_position, float ambient_color,float ConstantAtt, float LinearAtt, float QuadraticAtt)
{
	this->light_color = light_color;
	this->light_position = light_position;
	this->ambient_color = ambient_color;
	this->ConstantAtt = ConstantAtt;
	this->LinearAtt = LinearAtt;
	this->QuadraticAtt = QuadraticAtt;
}

point_light::point_light(ImVec4 light_color, glm::vec3 light_position)
{
	this->light_color = light_color;
	this->light_position = light_position;
}
