#pragma once
#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"

class player
{
public:
	player();
	~player();
	void initialise(engine::ref<engine::game_object> object);
	void on_update(const engine::timestep& time_step);
	engine::ref<engine::game_object> object() const { return m_object; }
	void turn(float angle);
	void update_camera(engine::perspective_camera& camera);
	void jump();
	void run();
	void walk();
	void dance();
	void die();
	void eatbanana();
	void eatgiantbanana();
	void bounceback();
	bool get_isdead();
	void set_speed_multiplier(float multiplier) { m_speed_multiplier = multiplier; }
	void set_gravity(float gravity) { m_gravity = gravity; }
	void set_grounded(bool grounded) { m_grounded = grounded; }
	void set_isbouncingback(bool isbouncingback) { m_isbouncingback = isbouncingback; }
	int get_hp(){return m_hp;}
	void set_hp(int hp) { m_hp =hp; }
	void remove_hp(int hp) { m_hp -= hp; }
private:
	float m_speed{ 0.f };
	float m_up_speed{ 0.f };
	float m_gravity{ -5.f };
	float terrain_height{ 0.5f };
	int m_hp;
	float m_timer;
	bool m_moving;
	bool m_grounded;
	bool m_isbouncingback = false;
	bool m_playerdead = false;
	bool m_dying = false;
	bool m_eating = false;
	float m_speed_multiplier{ 1.f };
	engine::ref< engine::game_object> m_object;
};
