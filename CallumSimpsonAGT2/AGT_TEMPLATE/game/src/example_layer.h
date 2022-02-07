#pragma once
#include <engine.h>
#include "player.h"
#include "menu.h"
#include "deathscreen.h"
#include "damagefade.h"
#include "hpfade.h"
#include "hudfade.h"
#include <engine/entities/shapes/heightmap.h>
#include "engine/entities/bounding_box.h"
#include "electricity.h"
#include "Enemy.h"
#include "projectile.h"

class pickup;
class banana;
class powerup;
class giantbanana;

class example_layer : public engine::layer
{
public:
    example_layer();
	~example_layer();
	//uses high resolution timer 
    void on_update(const engine::timestep& time_step) override;
    void on_render() override; 
    void on_event(engine::event& event) override;

private:
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_angry_monkey{};
	engine::ref<engine::game_object>	m_angry_monkey2{};
	engine::ref<engine::game_object>	m_angry_monkey3{};
	engine::ref<engine::game_object>	m_angry_monkey4{};
	engine::ref<engine::game_object>	m_angry_monkey5{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_monkey{};
	engine::ref<engine::game_object>	m_cabin{};
	engine::ref<engine::game_object>	m_tower{};
	engine::ref<engine::game_object>	m_dirt_platform{};
	engine::ref<engine::game_object>	m_dirt_platform2{};
	engine::ref<engine::game_object>	m_dirt_platform3{};
	engine::ref<engine::game_object>	m_dirt_platform4{};
	engine::ref<engine::game_object>	m_dirt_platform5{};
	engine::ref<engine::game_object>	m_castle{};
	engine::ref<engine::game_object>	m_cartoon_tree{};
	engine::ref<engine::game_object>	m_triangular_prism{};
	engine::ref<engine::game_object>	m_square_pyramid{};
	engine::ref<engine::game_object>	m_isosceles_triangular_prism{};
	engine::ref<engine::game_object>	m_crate;
	engine::ref<engine::game_object>	m_sign;
	engine::ref<engine::game_object>	m_star;
	engine::ref<engine::game_object>	m_sign2;
	engine::ref<menu>					m_menu{};
	engine::ref<deathscreen>			m_deathscreen{};
	engine::ref<damagefade>				m_damagefade{};
	engine::ref<hpfade>					m_hpfade{};
	engine::ref<hudfade>					m_hudfade{};
	std::vector<engine::ref<electricity>>		m_electricitys{};
	engine::ref<engine::game_object>	m_moving_platform{};
	engine::ref<engine::game_object>	m_moving_platform2{};
	engine::ref<pickup>				    m_pickup{};
	engine::ref<banana>					m_banana{};
	engine::ref<giantbanana>			m_giantbanana{};
	engine::ref<powerup>				m_powerup{};
	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_monkey_material{};
	engine::ref<engine::material>		m_angry_monkey_material{};
	engine::ref<engine::material>		m_platform_material{};
	engine::ref<engine::material>		m_pickup_material{};
	engine::ref<engine::game_object>	m_angry_monkey_projectile{};
	engine::ref<engine::game_object>	m_angry_monkey_projectile2{};
	engine::ref<engine::game_object>	m_angry_monkey_projectile3{};
	engine::ref<engine::game_object>	m_angry_monkey_projectile4{};
	engine::ref<engine::game_object>	m_angry_monkey_projectile5{};
	player								m_player{};
	enemy 								m_enemy{};
	enemy 								m_enemy2{};
	enemy 								m_enemy3{};
	enemy 								m_enemy4{};
	enemy 								m_enemy5{};
	projectile m_projectile;
	projectile m_projectile2;
	projectile m_projectile3;
	projectile m_projectile4;
	projectile m_projectile5;
	engine::DirectionalLight            m_directionalLight;
	engine::bounding_box				m_castlewall1;
	engine::bounding_box				m_castlewall2;
	engine::bounding_box				m_castlewall3;
	engine::bounding_box				m_castlewall4;
	engine::bounding_box				m_camera_box;
	engine::bounding_box				m_player_box;
	engine::bounding_box				m_player_feet_box;
	engine::bounding_box				m_crate_box;
	engine::bounding_box				m_crate_box2;
	engine::bounding_box				m_tower_box;
	engine::bounding_box				m_tower_box2;
	engine::bounding_box				m_banana_box;
	engine::bounding_box				m_giantbanana_box;
	engine::bounding_box				m_powerup_box;
	engine::bounding_box				m_banana_ball_box;
	engine::bounding_box				m_banana_ball_box2;
	engine::bounding_box				m_banana_ball_box3;
	engine::bounding_box				m_banana_ball_box4;
	engine::bounding_box				m_banana_ball_box5;
	engine::bounding_box				m_dirt_platform_box;
	engine::bounding_box				m_dirt_platform_box2;
	engine::bounding_box				m_dirt_platform2_box;
	engine::bounding_box				m_dirt_platform2_box2;
	engine::bounding_box				m_dirt_platform3_box;
	engine::bounding_box				m_dirt_platform3_box2;
	engine::bounding_box				m_dirt_platform4_box;
	engine::bounding_box				m_dirt_platform4_box2;
	engine::bounding_box				m_dirt_platform5_box;
	engine::bounding_box				m_dirt_platform5_box2;
	engine::bounding_box				m_moving_platform_box;
	engine::bounding_box				m_moving_platform_box2;
	engine::bounding_box				m_moving_platform2_box;
	engine::bounding_box				m_moving_platform2_box2;
	engine::bounding_box				m_angry_monkey_box;
	engine::bounding_box				m_angry_monkey_box2;
	engine::bounding_box				m_angry_monkey_box3;
	engine::bounding_box				m_angry_monkey_box4;
	engine::bounding_box				m_angry_monkey_box5;
	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

    engine::orthographic_camera       m_2d_camera; 
    engine::perspective_camera        m_3d_camera;

	engine::ref<engine::heightmap>	  m_heightmap;

	float							  m_physical_terrain_height = 0.5f;
	float							  m_move_amount = 0.1f;
	float gametime = 0.f;
	float m_maxtime = 180.f;
	bool startofgame = true;
	bool moveforward = true;
	bool m_game_ended = false;
	bool m_game_won = false;
	bool m_fog = true;
	bool m_play_cutscene = false;
	bool m_moved_camera = false;
	int  m_cutscene_timer = 0;
	bool m_turn_on_platforms = false;
	float m_freq_high;
	float m_freq_low;
	engine::PointLight m_pointLight;
	uint32_t num_point_lights = 1;
	engine::SpotLight m_spotLight;
	uint32_t num_spot_lights = 1;
};
