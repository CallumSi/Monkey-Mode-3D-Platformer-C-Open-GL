#pragma once
#include <engine.h>

//extends engine::pickup class
class pickup : public engine::game_object {
public:
	//constructor 
	pickup(const engine::game_object_properties props);
	//destructor 
	~pickup();
	//methods
	void init();
	void update(glm::vec3 c, float dt);
	bool active() { return m_is_active; }
	static engine::ref<pickup> create(const engine::game_object_properties& props);
//private member variables
private:
	bool m_is_active; // Indicates if pickup is active
};
