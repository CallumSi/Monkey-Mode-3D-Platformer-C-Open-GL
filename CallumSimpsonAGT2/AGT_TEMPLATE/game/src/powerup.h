#pragma once
#include <engine.h>

//extends engine::powerup class
class powerup : public engine::game_object {
public:
	//constructor 
	powerup(const engine::game_object_properties props);
	//destructor 
	~powerup();
	//methods
	void init();
	void update(glm::vec3 c, float dt);
	bool active() { return m_is_active; }
	static engine::ref<powerup> create(const engine::game_object_properties& props);
	void set_powerup_eaten(bool eaten) { m_powerup_eaten = eaten; };
	bool is_powerup_eaten() { return m_powerup_eaten; };
	//private member variables
private:
	bool m_is_active; // Indicates if powerup is active
	bool m_powerup_eaten = false;
};
#pragma once
