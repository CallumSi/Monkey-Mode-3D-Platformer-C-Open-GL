#pragma once
#include <engine.h>

//extends engine::banana class
class banana : public engine::game_object {
public:
	//constructor 
	banana(const engine::game_object_properties props);
	//destructor 
	~banana();
	//methods
	void init();
	void update(glm::vec3 c, float dt);
	bool active() { return m_is_active; }
	static engine::ref<banana> create(const engine::game_object_properties& props);
	void set_banana_eaten(bool eaten) { m_banana_eaten = eaten; };
	bool is_banana_eaten() { return m_banana_eaten; };
	//private member variables
private:
	bool m_is_active; // Indicates if banana is active
	bool m_banana_eaten = false;
};
