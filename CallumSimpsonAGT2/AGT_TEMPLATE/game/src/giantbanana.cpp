#include "giantbanana.h"
giantbanana::giantbanana(const engine::game_object_properties props) : engine::game_object(props)
{}
giantbanana::~giantbanana()
{}
void giantbanana::init()
{
	//initialise with active true by default
	m_is_active = true;
}
void giantbanana::update(glm::vec3 c, float dt)
{
	//rotate the banana
	set_rotation_amount(rotation_amount() + dt * 1.5f);
	glm::vec3 d = position() - c;
	if (glm::length(d) < 0.5f)
	{
		//if picked up set it to false
		m_is_active = false;

	}

}

engine::ref<giantbanana> giantbanana::create(const engine::game_object_properties& props)
{
	return std::make_shared<giantbanana>(props);
}
