#include "pch.h"
#include "menu.h"
#include "quad.h"

menu::menu(const std::string& path, float max_time, float width, float height)
{
	//create the texture and quad
	m_texture = engine::texture_2d::create(path, true);
	m_transparency = 0.0f;
	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

menu::~menu()
{}

void menu::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;
	//set the transparency
	m_transparency = 1.f;

}

void menu::on_render(engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	//transform the quad infront of the camera.
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, -20.f, 0.f));

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", 1.0f);
}

void menu::activate()
{
	s_active = true;
	m_transparency = 1.0f;
	m_timer = 0.0f;
}

engine::ref<menu> menu::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<menu>(path, max_time, width, height);
}

void menu::update_camera(engine::perspective_camera& camera)
{
	//move the camera under the map to look at menu on start
	glm::vec3 cam_pos = { 0.f,-20.f,2.15f }; 
	glm::vec3 cam_look_at = { 0.f,-20.f,0.f }; 
	camera.set_view_matrix(cam_pos, cam_look_at);
}
