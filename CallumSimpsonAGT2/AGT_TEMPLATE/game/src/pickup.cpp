#include "pickup.h"
pickup::pickup(const engine::game_object_properties props) : engine::game_object(props)
{}
pickup::~pickup()
{}
void pickup::init()
{
	//initialise with active true by default
	m_is_active = true;
}
void pickup::update(glm::vec3 c, float dt)
{

	//rotate the pickup
	set_rotation_amount(rotation_amount() + dt * 1.5f);
	glm::vec3 d = position() - c;
	if (glm::length(d) < 0.5f)
		m_is_active = false;
		
}
engine::ref<pickup> pickup::create(const engine::game_object_properties& props)
{
	return std::make_shared<pickup>(props);
}
