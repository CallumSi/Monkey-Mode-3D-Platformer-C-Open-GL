#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"
#include "damagefade.h"

//constructor
player::player() : m_timer(0.0f), m_speed(0.f), m_moving(false), m_hp(100)
{
	
}
//destructor
player::~player()
{
}

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_object->set_forward(glm::vec3(0.f, 0.f, -1.f));
	m_object->set_position(glm::vec3(0.f, 0.5, 10.f));
	m_object->animated_mesh()->set_default_animation(0);
}

void player::on_update(const engine::timestep& time_step)
{
		//Apply gravity and speed multipler 
		m_up_speed += m_gravity * (float)time_step * m_speed_multiplier;
		//check if player isnt dead
		if (m_playerdead == false) {
			//check for pressed keys
			//ensure key press also ensures that the player is in the correct state 
			if (engine::input::key_pressed(engine::key_codes::KEY_W) && m_moving != true && m_grounded == true && m_isbouncingback == false)
			{
				if (engine::input::key_pressed(engine::key_codes::KEY_LEFT_SHIFT))
					walk();
				else
					run();
			}
			if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
				turn(2.0f * time_step);
			if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
				turn(-2.0f * time_step);
			if (engine::input::key_pressed(engine::key_codes::KEY_SPACE) && m_grounded == true && m_eating ==false)
				jump();
			if (engine::input::key_pressed(engine::key_codes::KEY_B) && m_grounded == true && m_moving != true)
				dance();

			//move the player
			glm::vec3 oldpos = m_object->position();
			m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
			m_object->set_position(m_object->position() += m_object->up() * m_up_speed * (float)time_step);
			//check if under map
			if (m_object->position().y < terrain_height)
			{
				//apply fall damage
				if (m_up_speed < -3.f) {
					m_hp -= 50;
					
				}
				//make sure player doesnt fall through map
				m_up_speed = 0;
				m_object->set_position(oldpos);
				m_grounded = true;
			}

		}
		//use a timer to make sure animations are finished
		if (m_timer > 0.0f)
		{
			//move the plaer forward 
			m_object->set_position(m_object->position() += m_object->forward() * m_speed  * (float)time_step);
			m_timer -= (float)time_step;
			if (m_timer < 0.0f)
			{
				m_object->animated_mesh()->switch_root_movement(false);
				m_object->animated_mesh()->switch_animation(m_object->animated_mesh()->default_animation());
				m_speed = 0.f;
				m_up_speed = 0.f;
				m_moving = false;
				m_dying = false;
				m_eating = false;
			}
		}
		m_object->animated_mesh()->on_update(time_step);
		if (m_grounded == true) {
			m_isbouncingback = false;
		}
		//check if the player is dead 
		if (m_hp <= 0 && m_playerdead == false) {
			//die
			m_playerdead = true;
			die();
		}
	
	
}

void player::turn(float angle)
{
	m_object->set_forward(glm::rotate(m_object->forward(), angle, glm::vec3(0.f, 1.f,
		0.f)));
}

void player::update_camera(engine::perspective_camera& camera)
{
	//update the camreas position
	float A = 1.f;
	float B = 2.f;
	float C = 4.f;
	glm::vec3 cam_pos = m_object->position() - glm::normalize(m_object->forward()) * B;
	cam_pos.y += A;
	glm::vec3 cam_look_at = m_object->position() + glm::normalize(m_object->forward()) * C;
	cam_look_at.y = 0.f;
	camera.set_view_matrix(cam_pos, cam_look_at);
}

void player::run()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(false);
	m_object->animated_mesh()->switch_animation(1);
	m_speed = 2.f ;
	m_timer = 0.4;//m_object->animated_mesh()->animations().at(1)->mDuration;
	m_moving = true;
}

void player::jump()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(2);
	m_speed = 1.0f;
	m_up_speed = 2.75f ;
	m_timer = 1.0;//m_object->animated_mesh()->animations().at(2)->mDuration;
	m_grounded = false;
}

void player::walk()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(false);
	m_object->animated_mesh()->switch_animation(3);
	m_speed = 0.5f;
	m_timer = 1;//m_object->animated_mesh()->animations().at(1)->mDuration;
	m_moving = true;
}

void player::dance()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(4);
	m_speed = 0.0f;
	m_timer = 7.0;//m_object->animated_mesh()->animations().at(2)->mDuration;
	m_moving = true;
}

void player::die()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(6);
	m_speed = 0.0f;
	m_timer = 4.0;
	//indicate the player has died
	m_dying = true;
}



void player::eatbanana()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(7);
	m_speed = 0.0f;
	m_timer = 4.3;//m_object->animated_mesh()->animations().at(2)->mDuration;
	m_moving = true;
	//increase the players HP
	m_hp += 50;
	m_eating = true;
}

void player::eatgiantbanana()
{
	//change the animation , speed and start the timer
	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(4);
	m_speed = 0.0f;
	m_timer = 7.0;//m_object->animated_mesh()->animations().at(2)->mDuration;
	m_moving = true;

}

void player::bounceback()
{
	//change the animation , speed and start the timer
	//makes the player move away from an object
	if (m_isbouncingback == false) {
		m_object->animated_mesh()->switch_root_movement(true);
		m_object->animated_mesh()->switch_animation(5);
		m_speed = -1.0f;
		m_timer = 0.8;//m_object->animated_mesh()->animations().at(2)->mDuration;
		m_isbouncingback = true;
	}
	
}

bool player::get_isdead()
{
	//check if the player is dead
	if (m_playerdead && m_dying == false) {
		return true;
	}
	else {
		return false;
	}

}

