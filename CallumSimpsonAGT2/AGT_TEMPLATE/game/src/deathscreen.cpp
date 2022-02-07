#include "pch.h"
#include "deathscreen.h"
#include "quad.h"

deathscreen::deathscreen(const std::string& path, float max_time, float width, float height)
{
	//add the texture
	m_texture = engine::texture_2d::create(path, true);
	//set the transparency 
	m_transparency = 0.0f;
	//create the quad 
	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

deathscreen::~deathscreen()
{}

void deathscreen::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;
	//change the transparency
	m_transparency = 1.f;

}

void deathscreen::on_render(engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	//transform the quad into the right place and add the texture
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, -20.f, 10.f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void deathscreen::activate()
{
	
	s_active = true;
	m_transparency = 1.0f;
	m_timer = 0.0f;
}

engine::ref<deathscreen> deathscreen::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<deathscreen>(path, max_time, width, height);
}

void deathscreen::update_camera(engine::perspective_camera& camera)
{
	//move the camera under the map to look at deathscreen on start
	glm::vec3 cam_pos = { 0.f,-20.f, 12.15f };
	glm::vec3 cam_look_at = { 0.f,-20.f,10.f };
	camera.set_view_matrix(cam_pos, cam_look_at);
}
