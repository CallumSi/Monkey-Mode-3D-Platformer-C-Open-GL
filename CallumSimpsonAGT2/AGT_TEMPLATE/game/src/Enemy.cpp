#include "pch.h"
#include "enemy.h"
#include <ctime>
enemy::enemy() {}

enemy::~enemy() {}

void enemy::initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward)
{
	m_object = object;
	m_object->set_forward(forward);
	m_object->set_position(position);
}
void enemy::on_update(const engine::timestep& time_step, const glm::vec3& player_position)
{
		glm::vec3 oldpos = m_object->position();
		float distance_to_player = glm::distance(m_object->position(), player_position);
		m_up_speed += m_gravity * (float)time_step ;
		
		// check which state is the enemy in, then execute the matching behaviour
		if (m_state == state::idle)
		{
			//randomly select patrol or jumping
			srand((unsigned)time(NULL));
			if ((rand() % 2) + 1 == 2)
			{
				patrol(time_step);
			}
			else {
				m_state = state::jumping;
			}
			
			// check whether the condition has been met to switch to the on_guard state
			if (distance_to_player < m_detection_radius)
				m_state = state::on_guard;
		}
		else if (m_state == state::on_guard )
		{
			face_player(time_step, player_position);
			// check whether the condition has been met to switch to shooting state
			if (player_position.y > 0.7 && player_position.y < 1) {
				m_state = state::shooting;
			}
			// check whether the condition has been met to switch to idle state
			else if (distance_to_player > m_detection_radius) {
				
					m_state = state::idle;
			}
			// check whether the condition has been met to switch to the running state
			else if (distance_to_player < m_trigger_radius)
				m_state = state::running;

		}
		else if (m_state == state::running)
		{
			run_from_player(time_step, player_position);
			// check whether the condition has been met to switch to idle state
			if (distance_to_player > m_detection_radius)
				m_state = state::idle;
			// check whether the condition has been met to switch to the chasing state
			else if (distance_to_player < m_attack_radius)
				m_state = state::chasing;
			
		}
		else if (m_state == state::chasing)
		{
			chase_player(time_step, player_position);
			// check whether the condition has been met to switch to ong guard state
			if (distance_to_player > m_attack_radius)
				m_state = state::on_guard;
			// check whether the condition has been met to switch to the running state
			else if (distance_to_player > m_trigger_radius)
				m_state = state::running;
			

		}
		else if (m_state == state::jumping)
		{
		
			jump();
			
			
			// check whether the condition has been met to switch to the on_guard state otherwise become idle
			if (distance_to_player < m_detection_radius)
				m_state = state::on_guard;
			else {
				m_state = state::idle;
			}
		}
		else if (m_state == state::shooting )
		{
			throw_at_player(time_step, player_position);
			// check whether the condition has been met to switch to the idle  state
			if (distance_to_player > m_detection_radius)
				m_state = state::idle;
			// check whether the condition has been met to switch to the running  state
			else if (distance_to_player < m_trigger_radius)
				m_state = state::running;
		}

		////move the player
		glm::vec3 newpos = m_object->position();
		m_object->set_position(m_object->position() += m_object->up() * m_up_speed * (float)time_step);
		if (m_object->position().y < 0.5f)
		{
			
		
			//make sure player doesnt fall through map
			m_up_speed = 0;
			m_object->set_position({ newpos.x, oldpos.y, newpos.z });
			m_jumping = false;
		
		}
		
}



// move forwards until the timer runs out, then switch direction to move the other way
void enemy::patrol(const engine::timestep& time_step)
{

	m_switch_direction_timer -= (float)time_step;
	if (m_switch_direction_timer < 0.f)
	{
		m_object->set_forward(m_object->forward() * -1.f);
		m_switch_direction_timer = m_default_time;
	}

	m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
	
}
// stop and look at the player
void enemy::face_player(const engine::timestep& time_step, const glm::vec3&
	player_position)
{

	m_object->set_forward(player_position - m_object->position());
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));

}

// move forwards in the direction of the player
void enemy::run_from_player(const engine::timestep& time_step, const glm::vec3&
	player_position)
{

	m_object->set_forward(player_position + m_object->position());
	m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

void enemy::chase_player(const engine::timestep& time_step, const glm::vec3&
	player_position)
{
	m_object->set_forward(player_position - m_object->position());
	m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// move forwards in the direction of the player
void enemy::throw_at_player(const engine::timestep& time_step, const glm::vec3&
	player_position)
{
		
	m_shooting = true;
	
}
//make the monkey jump by increasing its up screen
void enemy::jump(){
	if (m_jumping != true) {
		m_up_speed = 0.15f;
	}
	
}







