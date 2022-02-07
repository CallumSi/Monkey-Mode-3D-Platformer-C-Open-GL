#pragma once
#include <engine.h>
class enemy
{
	enum class state
	{
		idle,
		on_guard,
		running,
		shooting,
		jumping,
		chasing
	};
public:
	enemy();
	~enemy();
	// set the parameters for the enemy
	void initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward);
	// update the enemy
	void on_update(const engine::timestep& time_step, const glm::vec3& player_position);
	// methods controlling the enemy’s behaviour in a certain state
	void patrol(const engine::timestep& time_step);
	void face_player(const engine::timestep& time_step, const glm::vec3&
		player_position);
	void run_from_player(const engine::timestep& time_step, const glm::vec3&
		player_position);
	void chase_player(const engine::timestep& time_step, const glm::vec3&
		player_position);
	void throw_at_player(const engine::timestep& time_step, const glm::vec3&
		player_position);
	bool is_shooting() { return m_shooting; }
	void shootdone() { m_shooting = false; }
	void jump();
	// game object bound to the enemy
	engine::ref<engine::game_object> object() const { return m_object; }
private:
	
	// enemy's speed
	float m_speed{ 0.2f };
	// timer controlling the direction switch and the reset value for this timer
	float m_default_time{ 4.f };
	float m_switch_direction_timer = m_default_time;
	// threshold distances
	float m_detection_radius{ 3.f };
	float m_trigger_radius{ 2.f };
	float m_attack_radius{ 1.f };
	// game object bound to the enemy
	engine::ref< engine::game_object> m_object;
	//current state of the enemy's state machine
	state m_state = state::idle;
	bool m_shooting = false;
	bool m_jumping = false;
	float m_up_speed{ 0.f };
	float m_gravity{ -5.f };
};
