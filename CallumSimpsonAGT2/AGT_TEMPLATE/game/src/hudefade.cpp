#include "pch.h"
#include "hudfade.h"
#include "quad.h"

hudfade::hudfade(const std::string& path, float max_time, float width, float height)
{
	m_max_time = max_time;
	m_texture = engine::texture_2d::create(path, true);

	//create the quad 
	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

hudfade::~hudfade()
{}

void hudfade::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;
	//increment the timer
	m_timer += (float)time_step;



	if (m_timer > m_max_time)
		s_active = false;
}

void hudfade::on_render(engine::ref<engine::shader> shader, glm::vec3 playerpos, glm::vec3 playerforward, float rotation_amount, glm::vec3 rotation_axis)
{
	if (!s_active)
		return;

	//transfrom the quad infront of the camera.
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(-0.7f, +0.85f, -0.2f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void hudfade::activate()
{
	s_active = true;

	m_timer = 0.0f;
}



bool hudfade::is_active()
{
	return s_active;
}

engine::ref<hudfade> hudfade::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<hudfade>(path, max_time, width, height);
}
