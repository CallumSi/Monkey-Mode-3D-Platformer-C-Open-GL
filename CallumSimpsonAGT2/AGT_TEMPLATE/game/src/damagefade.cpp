#include "pch.h"
#include "damagefade.h"
#include "quad.h"

damagefade::damagefade(const std::string& path, float max_time, float width, float height)
{
	m_max_time = max_time;
	//create the texture
	m_texture = engine::texture_2d::create(path, true);

	//create the quad mesh
	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

damagefade::~damagefade()
{}

void damagefade::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;
	//add a timer 
	m_timer += (float)time_step;
	if (m_timer > m_max_time)
		s_active = false;
}

void damagefade::on_render(engine::ref<engine::shader> shader, glm::vec3 playerpos, glm::vec3 playerforward, float rotation_amount, glm::vec3 rotation_axis)
{
	if (!s_active)
		return;

	//move the damagefade infront of the camera
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, layer));

	//set the transparence and texture
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 0.2f);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void damagefade::activate()
{
	s_active = true;
	
	m_timer = 0.0f;
}



bool damagefade::is_active()
{
	return s_active;
}

engine::ref<damagefade> damagefade::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<damagefade>(path, max_time, width, height);
}
