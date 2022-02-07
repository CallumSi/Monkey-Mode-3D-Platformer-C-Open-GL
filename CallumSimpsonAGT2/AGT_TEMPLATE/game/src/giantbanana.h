#pragma once
#include <engine.h>

//extends engine::giantbanana class
class giantbanana : public engine::game_object {
public:
	//constructor 
	giantbanana(const engine::game_object_properties props);
	//destructor 
	~giantbanana();
	//methods
	void init();
	void update(glm::vec3 c, float dt);
	bool active() { return m_is_active; }
	static engine::ref<giantbanana> create(const engine::game_object_properties& props);
	void set_giantbanana_eaten(bool eaten) { m_giantbanana_eaten = eaten; };
	bool is_giantbanana_eaten() { return m_giantbanana_eaten; };
	//private member variables
private:
	bool m_is_active; // Indicates if giantbanana is active
	bool m_giantbanana_eaten = false;
};
