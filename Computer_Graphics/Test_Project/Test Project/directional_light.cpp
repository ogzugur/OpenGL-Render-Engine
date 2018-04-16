#include "directional_light.h"

void directional_light::set_uniform_data(GLuint shader_id,std::string light_variable_name)
{
	int iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name+".color").c_str());
	glUniform3f(iUniform, light_color.x, light_color.y, light_color.z);

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".direction").c_str());
	glUniform3fv(iUniform, 1, glm::value_ptr(light_direction));

	iUniform = glGetUniformLocation((GLuint)shader_id, (light_variable_name + ".ambient").c_str());
	glUniform1f(iUniform, ambient_color);

}

directional_light::directional_light()
{
	light_color = ImVec4(1.0f, 1.0f, 1.0f,1.0f);
	light_direction = glm::vec3(-1.0f, -1.0f, 0.0f);

	ambient_color = 0.25f;
}

directional_light::directional_light(ImVec4 light_color, glm::vec3 light_direction, float ambient_color)
{
	this->light_color = light_color;
	this->light_direction = light_direction;
	this->ambient_color = ambient_color;
}

