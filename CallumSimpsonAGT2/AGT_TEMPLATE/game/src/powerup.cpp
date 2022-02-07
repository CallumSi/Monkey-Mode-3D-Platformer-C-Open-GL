#include "powerup.h"
powerup::powerup(const engine::game_object_properties props) : engine::game_object(props)
{}
powerup::~powerup()
{}
void powerup::init()
{
	//initialise with active true by default
	m_is_active = true;
}
void powerup::update(glm::vec3 c, float dt)
{
	//rotate the powerup
	set_rotation_amount(rotation_amount() + dt * 1.5f);
	glm::vec3 d = position() - c;
	//if player gets too close become inactive
	if (glm::length(d) < 0.5f)
	{
		m_is_active = false;

	}



}

engine::ref<powerup> powerup::create(const engine::game_object_properties& props)
{
	return std::make_shared<powerup>(props);
}
