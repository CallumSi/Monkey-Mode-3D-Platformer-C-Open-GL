#pragma once
#include <engine.h>

class quad;

class menu
{
public:
	menu(const std::string& path, float max_time, float width, float height);
	~menu();
	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void activate();
	void update_camera(engine::perspective_camera& camera);

	static engine::ref<menu> create(const std::string& path, float max_time, float width, float height);

private:
	bool s_active;

	float m_timer;
	float m_max_time;

	engine::ref<engine::texture_2d> m_texture;
	float m_transparency;
	engine::ref<quad> m_quad;
	engine::ref< engine::game_object> m_object;
};
