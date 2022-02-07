#include "banana.h"
banana::banana(const engine::game_object_properties props) : engine::game_object(props)
{}
banana::~banana()
{}
void banana::init()
{
	//initialise with active true by default
	m_is_active = true;
}
void banana::update(glm::vec3 c, float dt)
{
	//rotate the object
	set_rotation_amount(rotation_amount() + dt * 1.5f);
	glm::vec3 d = position() - c;
	if (glm::length(d) < 0.5f)
	{
		//set the banana inactive
		m_is_active = false;
		
	}
	
		

}

engine::ref<banana> banana::create(const engine::game_object_properties& props)
{
	//create the banana
	return std::make_shared<banana>(props);
}
