#pragma once
#include <engine.h>
#include "player.h"
class quad;

class hpfade
{
public:
	hpfade(const std::string& path, float max_time, float width, float height);
	~hpfade();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader, glm::vec3 playerpos, glm::vec3 playerforward, float rotation_amount, glm::vec3 rotation_axis);
	void activate();
	static engine::ref<hpfade> create(const std::string& path, float max_time, float width, float height);
	bool is_active();
	void set_position(float pos) { position = pos; }
private:
	bool s_active;
	float position = 0.07f;
	
	float m_timer;
	float m_max_time;

	engine::ref<engine::texture_2d> m_texture;
	engine::ref<quad> m_quad;
};

