#include "pch.h"
#include "hpfade.h"
#include "quad.h"

hpfade::hpfade(const std::string& path, float max_time, float width, float height)
{
	m_max_time = max_time;
	//add the teture
	m_texture = engine::texture_2d::create(path, true);

	//render the quad
	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

hpfade::~hpfade()
{}

void hpfade::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;
	//incriment the timer
	m_timer += (float)time_step;
	if (m_timer > m_max_time)
		s_active = false;
}

void hpfade::on_render(engine::ref<engine::shader> shader, glm::vec3 playerpos, glm::vec3 playerforward, float rotation_amount, glm::vec3 rotation_axis)
{
	if (!s_active)
		return;

	//transform the quad infront of the player
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(position-1.25f, 0.82f, -0.1f));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void hpfade::activate()
{
	s_active = true;

	m_timer = 0.0f;
}



bool hpfade::is_active()
{
	return s_active;
}

engine::ref<hpfade> hpfade::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<hpfade>(path, max_time, width, height);
}
