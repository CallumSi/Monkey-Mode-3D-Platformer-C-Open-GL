#include "example_layer.h"
#include "platform/opengl/gl_shader.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include "pickup.h"
#include "banana.h"
#include "giantbanana.h"
#include "powerup.h"
#include <string>



example_layer::example_layer()
	:m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())
{
	/////////////
	//CONSTRUCTOR
	/////////////
	
	// Initialise the shaders, materials and lights
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	// Hide the mouse and lock it inside the window
	engine::application::window().hide_mouse_cursor();

	//////////////////////
	//Load Audio
	//////////////////////

	// Initialise audio and play background music
	m_freq_high = 0.f;
	m_freq_low = 20000.f;
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	
	//audio found at https://orangefreesounds.com/rainforest-sounds/
	m_audio_manager->load_sound("assets/audio/Rainforest.mp3", engine::sound_type::track, "rainforest");
	//audio found at https://mixkit.co/free-sound-effects/monkey/
	m_audio_manager->load_sound("assets/audio/Monkey.wav", engine::sound_type::spatialised, "monkey"); // Royalty free sound from freesound.org
	//audio found at https://mixkit.co/free-sound-effects/monkey/
	m_audio_manager->load_sound("assets/audio/Monkey2.wav", engine::sound_type::spatialised, "monkey2"); // Royalty free sound from freesound.org
	//audio found at https://mixkit.co/free-sound-effects/eat/
	m_audio_manager->load_sound("assets/audio/Eat.wav", engine::sound_type::spatialised, "eat"); // Royalty free sound from freesound.org

	m_audio_manager->play("rainforest");
	m_audio_manager->create_high_pass_filter();
	m_audio_manager->set_high_pass_filter(m_freq_high);
	m_audio_manager->create_low_pass_filter();
	m_audio_manager->set_low_pass_filter(m_freq_low);

	//////////////////////
	//Load Lighting and fog 
	//////////////////////
	
	//Initalise fog in the scene
	m_fog = true;
	glm::vec3 m_fog_colour = glm::vec3(0.5f);
	int m_fog_factor_type = 2;

	//Set the direcitonal light
	m_directionalLight.Color = glm::vec3(1.0f, 0.0f, 0.0f);
	m_directionalLight.AmbientIntensity = 0.4f;
	m_directionalLight.DiffuseIntensity = 0.3f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
	//add a point light (send light all around equal)
	m_pointLight.Color = glm::vec3(0.0f, 1.0f, 0.0f);
	m_pointLight.AmbientIntensity = 0.6f;
	m_pointLight.DiffuseIntensity = 0.2f;
	m_pointLight.Position = glm::vec3(3.f, 1.f, 6.f);
	//add spot light (send light only infront). Used for star.
	m_spotLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_spotLight.AmbientIntensity = 0.85f;
	m_spotLight.DiffuseIntensity = 0.6f;
	m_spotLight.Position = glm::vec3(7.f, 1.2f, 12.f);
	m_spotLight.Direction = glm::normalize(glm::vec3(0.0f, -1.0f, -1.f));
	m_spotLight.Cutoff = 0.5f;
	m_spotLight.Attenuation.Constant = 0.8f;
	m_spotLight.Attenuation.Linear = 0.2f;
	m_spotLight.Attenuation.Exp = 0.03f;

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("fog_colour", m_fog_colour);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("fog_factor_type", m_fog_factor_type);
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
			(float)engine::application::window().height()));

	//////////////////////
	//Load Materials
	//////////////////////

	//Declare the materials 
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_monkey_material = engine::material::create(1.0f, glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_platform_material = engine::material::create(0.2f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_pickup_material = engine::material::create(1.f, glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	////////////////////////
	//Load Terrain + Skybox
	////////////////////////

	//Create the skybox
	//Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/Skybox2/skybox_front.jpg", true),
		  engine::texture_2d::create("assets/textures/Skybox2/skybox_right.jpg", true),
		  engine::texture_2d::create("assets/textures/Skybox2/skybox_back.jpg", true),
		  engine::texture_2d::create("assets/textures/Skybox2/skybox_left.jpg", true),
		  engine::texture_2d::create("assets/textures/Skybox2/skybox_top.jpg", true),
		  engine::texture_2d::create("assets/textures/Skybox2/skybox_bottom.jpg.bmp", true)
		});

	//Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	// Grass texture from https://assetstore.unity.com/packages/2d/textures-materials/glass/stylized-grass-texture-153153
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Sand 4.png", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	//introduce the heightmap
	m_heightmap = engine::heightmap::create("assets/textures/heightmap.bmp", "assets/textures/grass.png", 100.f, 100.f, glm::vec3(0.f, 0.f, 0.f), 10.f);
	engine::game_object_properties terrain_props;
	//terrain_props.meshes = { terrain_shape->mesh() };
	//terrain_props.textures = terrain_textures;
	terrain_props.meshes = { m_heightmap->mesh() };
	terrain_props.textures = { m_heightmap->texture() };
	terrain_props.is_static = true;
	terrain_props.type = 0;
	//terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.bounding_shape = glm::vec3(m_heightmap->terrain_size().x, m_physical_terrain_height, m_heightmap->terrain_size().y);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);


	//////////////////////
	//Load Animated Models
	//////////////////////
	
	//Load the player model
	// model from https://open3dmodel.com/3d-models/3d-model-animated-chimpanzee-rig_43177.html
	//rigged using mixamo
	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/chimp/Idle.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/Run.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/ForwardJump.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/Walking.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/Breakdance.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/Jump Backward.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/DyingBackwards.fbx");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/chimp/GetBanana.fbx");
	m_skinned_mesh->switch_root_movement(false);
	engine::game_object_properties monkey_props;
	monkey_props.animated_mesh = m_skinned_mesh;
	monkey_props.textures = m_skinned_mesh->textures();
	monkey_props.scale = glm::vec3(0.1f);
	monkey_props.position = glm::vec3(3.0f, 0.5f, -5.0f);
	monkey_props.type = 0;
	monkey_props.bounding_shape = glm::vec3(m_skinned_mesh->size().x / 2.f, m_skinned_mesh->size().y * 3.f, m_skinned_mesh->size().x / 2.f);
	m_monkey = engine::game_object::create(monkey_props);
	m_player.initialise(m_monkey);
	//Added boxes for collison
	m_player_box.set_box(monkey_props.bounding_shape.x * monkey_props.scale.x , monkey_props.bounding_shape.y * monkey_props.scale.y, monkey_props.bounding_shape.z * monkey_props.scale.z,
		glm::vec3{ monkey_props.position.x +1.f, monkey_props.position.y , monkey_props.position.z });
	m_player_feet_box.set_box(monkey_props.bounding_shape.x * monkey_props.scale.x, monkey_props.bounding_shape.y * monkey_props.scale.y/8, monkey_props.bounding_shape.z * monkey_props.scale.z,
		glm::vec3{ monkey_props.position.x, monkey_props.position.y , monkey_props.position.z });

	////////////////////
	//Load Static Models
	////////////////////

	//Load the enemy model
	// model from https://open3dmodel.com/3d-models/3d-model-animated-chimpanzee-rig_43177.html
	engine::ref <engine::model> angry_monkey_model = engine::model::create("assets/models/static/monkered.3ds");
	engine::game_object_properties angry_monkey_props;
	angry_monkey_props.meshes = angry_monkey_model->meshes();
	angry_monkey_props.textures = angry_monkey_model->textures();
	float angry_monkey_scale = 1.f / glm::max(angry_monkey_model->size().x, glm::max(angry_monkey_model->size().y, angry_monkey_model->size().z));
	angry_monkey_props.position = { 4.f,0.5f, 3.f };
	angry_monkey_props.scale = glm::vec3(0.1f);
	angry_monkey_props.bounding_shape = angry_monkey_model->size();
	m_angry_monkey = engine::game_object::create(angry_monkey_props);
	m_angry_monkey2 = engine::game_object::create(angry_monkey_props);
	m_angry_monkey3 = engine::game_object::create(angry_monkey_props);
	m_angry_monkey4 = engine::game_object::create(angry_monkey_props);
	m_angry_monkey5 = engine::game_object::create(angry_monkey_props);
	m_enemy.initialise(m_angry_monkey, angry_monkey_props.position, glm::vec3(1.f, 0.f, 0.f));
	m_enemy2.initialise(m_angry_monkey2, glm::vec3(1.f, 0.5f, 7.f), glm::vec3(1.f, 0.f, 0.f));
	m_enemy3.initialise(m_angry_monkey3, glm::vec3(7.f, 0.5f, 10.f), glm::vec3(1.f, 0.f, 0.f));
	m_enemy4.initialise(m_angry_monkey4, glm::vec3(7.5f, 0.5f, 4.f), glm::vec3(1.f, 0.f, 0.f));
	m_enemy5.initialise(m_angry_monkey5, glm::vec3(4.f, 0.5f, 5.f), glm::vec3(1.f, 0.f, 0.f));

	m_angry_monkey->set_offset(angry_monkey_model->offset());
	m_angry_monkey_box.set_box(angry_monkey_props.bounding_shape.x* angry_monkey_props.scale.x, angry_monkey_props.bounding_shape.y* angry_monkey_props.scale.y,
		angry_monkey_props.bounding_shape.z* angry_monkey_props.scale.z, glm::vec3{ angry_monkey_props.position.x + 1.f, angry_monkey_props.position.y , angry_monkey_props.position.z });
	m_angry_monkey_box2.set_box(angry_monkey_props.bounding_shape.x* angry_monkey_props.scale.x, angry_monkey_props.bounding_shape.y* angry_monkey_props.scale.y,
		angry_monkey_props.bounding_shape.z* angry_monkey_props.scale.z, glm::vec3{ angry_monkey_props.position.x + 1.f, angry_monkey_props.position.y , angry_monkey_props.position.z });
	m_angry_monkey_box3.set_box(angry_monkey_props.bounding_shape.x* angry_monkey_props.scale.x, angry_monkey_props.bounding_shape.y* angry_monkey_props.scale.y,
		angry_monkey_props.bounding_shape.z* angry_monkey_props.scale.z, glm::vec3{ angry_monkey_props.position.x + 1.f, angry_monkey_props.position.y , angry_monkey_props.position.z });
	m_angry_monkey_box4.set_box(angry_monkey_props.bounding_shape.x* angry_monkey_props.scale.x, angry_monkey_props.bounding_shape.y* angry_monkey_props.scale.y,
		angry_monkey_props.bounding_shape.z* angry_monkey_props.scale.z, glm::vec3{ angry_monkey_props.position.x + 1.f, angry_monkey_props.position.y , angry_monkey_props.position.z });
	m_angry_monkey_box5.set_box(angry_monkey_props.bounding_shape.x* angry_monkey_props.scale.x, angry_monkey_props.bounding_shape.y* angry_monkey_props.scale.y,
		angry_monkey_props.bounding_shape.z* angry_monkey_props.scale.z, glm::vec3{ angry_monkey_props.position.x + 1.f, angry_monkey_props.position.y , angry_monkey_props.position.z });


	//model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the sign model. Create a sign object. Set its properties
	engine::ref <engine::model> sign_model = engine::model::create("assets/models/static/Building/Sign_Right.fbx");
	engine::game_object_properties sign_props;
	sign_props.meshes = sign_model->meshes();
	sign_props.textures = sign_model->textures();
	float sign_scale = 1.f / glm::max(sign_model->size().x, glm::max(sign_model->size().y, sign_model->size().z));
	sign_props.position = { -1.f, 0.2f, 4.f };
	sign_props.rotation_axis = { 1.f, 0.f, 0.f };
	sign_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	sign_props.scale = glm::vec3(sign_scale);
	sign_props.bounding_shape = sign_model->size() / 2.f * sign_scale;
	m_sign = engine::game_object::create(sign_props);

	//model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the star model. Create a star object. Set its properties
	engine::ref <engine::model> star_model = engine::model::create("assets/models/static/Building/star.fbx");
	engine::game_object_properties star_props;
	star_props.meshes = star_model->meshes();
	star_props.textures = star_model->textures();
	float star_scale = 1.f / glm::max(star_model->size().x, glm::max(star_model->size().y, star_model->size().z));
	star_props.position = { 7.f, 1.2f, 12.f };
	star_props.rotation_axis = { 1.f, 0.f, 0.f };
	star_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	star_props.scale = glm::vec3(star_scale);
	star_props.bounding_shape = star_model->size() / 2.f * star_scale;
	m_star = engine::game_object::create(star_props);

	// model from https://free3d.com/3d-model/guard-tower-304393.html
	// Load the tower model. Create a tower object. Set its properties
	engine::ref <engine::model> tower_model = engine::model::create("assets/models/static/WatchTower/tower.fbx");
	engine::game_object_properties tower_props;
	tower_props.meshes = tower_model->meshes();
	tower_props.textures = tower_model->textures();
	float tower_scale = 3.f / glm::max(tower_model->size().x, glm::max(tower_model->size().y, tower_model->size().z));
	tower_props.position = { 3.2f, 0.5f, 2.5f };
	tower_props.scale = glm::vec3(tower_scale);
	tower_props.bounding_shape = tower_model->size();
	m_tower = engine::game_object::create(tower_props);
	m_tower->set_offset(tower_model->offset());
	//Added boxes for collison
	m_tower_box.set_box(tower_props.bounding_shape.x* tower_scale, tower_props.bounding_shape.y* tower_scale, tower_props.bounding_shape.z* tower_scale,
		glm::vec3{ tower_props.position.x, tower_props.position.y, tower_props.position.z });
	m_tower_box2.set_box(tower_props.bounding_shape.x* tower_scale, 0.01f, tower_props.bounding_shape.z* tower_scale,
		glm::vec3{ tower_props.position.x, tower_props.position.y + tower_props.bounding_shape.y * tower_scale , tower_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the dirt_platform model. Create a dirt_platform object. Set its properties
	engine::ref <engine::model> dirt_platform_model = engine::model::create("assets/models/static/Building/Platform_CenterMiddle.fbx");
	engine::game_object_properties dirt_platform_props;
	dirt_platform_props.meshes = dirt_platform_model->meshes();
	dirt_platform_props.textures = dirt_platform_model->textures();
	float dirt_platform_scale = 1.f / glm::max(dirt_platform_model->size().x, glm::max(dirt_platform_model->size().y, dirt_platform_model->size().z));
	dirt_platform_props.position = { 2.5f, 1.3f, 6.f };
	dirt_platform_props.rotation_axis = { 1.f, 0.f, 0.f };
	dirt_platform_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	dirt_platform_props.scale = glm::vec3(dirt_platform_scale);
	dirt_platform_props.bounding_shape = dirt_platform_model->size();
	m_dirt_platform = engine::game_object::create(dirt_platform_props);
	m_dirt_platform->set_offset(dirt_platform_model->offset());
	//Added boxes for collison
	m_dirt_platform_box.set_box(dirt_platform_props.bounding_shape.x* dirt_platform_scale , dirt_platform_props.bounding_shape.z* dirt_platform_scale,
		dirt_platform_props.bounding_shape.y* dirt_platform_scale ,
		glm::vec3{ dirt_platform_props.position.x, dirt_platform_props.position.y, dirt_platform_props.position.z});
	m_dirt_platform_box2.set_box(dirt_platform_props.bounding_shape.x * dirt_platform_scale, 0.01f, dirt_platform_props.bounding_shape.y * dirt_platform_scale,
		glm::vec3{ dirt_platform_props.position.x, dirt_platform_props.position.y + dirt_platform_props.bounding_shape.z * dirt_platform_scale , dirt_platform_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the dirt_platform2 model. Create a dirt_platform2 object. Set its properties
	engine::ref <engine::model> dirt_platform2_model = engine::model::create("assets/models/static/Building/Platform_TopLeft.fbx");
	engine::game_object_properties dirt_platform2_props;
	dirt_platform2_props.meshes = dirt_platform2_model->meshes();
	dirt_platform2_props.textures = dirt_platform2_model->textures();
	float dirt_platform2_scale = 1.f / glm::max(dirt_platform2_model->size().x, glm::max(dirt_platform2_model->size().y, dirt_platform2_model->size().z));
	dirt_platform2_props.position = { 3.5f, 2.f, 5.f };
	dirt_platform2_props.rotation_axis = { 1.f, 0.f, 0.f };
	dirt_platform2_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	dirt_platform2_props.scale = glm::vec3(dirt_platform2_scale);
	dirt_platform2_props.bounding_shape = dirt_platform2_model->size();
	m_dirt_platform2 = engine::game_object::create(dirt_platform2_props);
	m_dirt_platform2->set_offset(dirt_platform2_model->offset());
	//Added boxes for collison
	m_dirt_platform2_box.set_box(dirt_platform2_props.bounding_shape.x* dirt_platform2_scale, dirt_platform2_props.bounding_shape.z* dirt_platform2_scale,
		dirt_platform2_props.bounding_shape.y* dirt_platform2_scale,
		glm::vec3{ dirt_platform2_props.position.x, dirt_platform2_props.position.y, dirt_platform2_props.position.z });
	m_dirt_platform2_box2.set_box(dirt_platform2_props.bounding_shape.x* dirt_platform2_scale, 0.01f, dirt_platform2_props.bounding_shape.y* dirt_platform2_scale,
		glm::vec3{ dirt_platform2_props.position.x, dirt_platform2_props.position.y + dirt_platform2_props.bounding_shape.z * dirt_platform2_scale , dirt_platform2_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-modelmoving_platform
	// Load the dirt_platform3 model. Create a dirt_platform3 object. Set its properties
	engine::ref <engine::model> dirt_platform3_model = engine::model::create("assets/models/static/Building/Platform_TopRight.fbx");
	engine::game_object_properties dirt_platform3_props;
	dirt_platform3_props.meshes = dirt_platform3_model->meshes();
	dirt_platform3_props.textures = dirt_platform3_model->textures();
	float dirt_platform3_scale = 1.f / glm::max(dirt_platform3_model->size().x, glm::max(dirt_platform3_model->size().y, dirt_platform3_model->size().z));
	dirt_platform3_props.position = { 4.f, 2.3f, 4.f };
	dirt_platform3_props.rotation_axis = { 1.f, 0.f, 0.f };
	dirt_platform3_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	dirt_platform3_props.scale = glm::vec3(dirt_platform3_scale);
	dirt_platform3_props.bounding_shape = dirt_platform3_model->size();
	m_dirt_platform3 = engine::game_object::create(dirt_platform3_props);
	m_dirt_platform3->set_offset(dirt_platform3_model->offset());
	//Added boxes for collison
	m_dirt_platform3_box.set_box(dirt_platform3_props.bounding_shape.x* dirt_platform3_scale, dirt_platform3_props.bounding_shape.z* dirt_platform3_scale,
		dirt_platform3_props.bounding_shape.y* dirt_platform3_scale,
		glm::vec3{ dirt_platform3_props.position.x, dirt_platform3_props.position.y, dirt_platform3_props.position.z });
	m_dirt_platform3_box2.set_box(dirt_platform3_props.bounding_shape.x* dirt_platform3_scale, 0.01f, dirt_platform3_props.bounding_shape.y* dirt_platform3_scale,
		glm::vec3{ dirt_platform3_props.position.x, dirt_platform3_props.position.y + dirt_platform3_props.bounding_shape.z * dirt_platform3_scale , dirt_platform3_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the dirt_platform4 model. Create a dirt_platform4 object. Set its properties
	engine::ref <engine::model> dirt_platform4_model = engine::model::create("assets/models/static/Building/Platform_TopLeft.fbx");
	engine::game_object_properties dirt_platform4_props;
	dirt_platform4_props.meshes = dirt_platform4_model->meshes();
	dirt_platform4_props.textures = dirt_platform4_model->textures();
	float dirt_platform4_scale = 1.f / glm::max(dirt_platform4_model->size().x, glm::max(dirt_platform4_model->size().y, dirt_platform4_model->size().z));
	dirt_platform4_props.position = { -2.f, 1.8f, 5.f };
	dirt_platform4_props.rotation_axis = { 1.f, 0.f, 0.f };
	dirt_platform4_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	dirt_platform4_props.scale = glm::vec3(dirt_platform4_scale);
	dirt_platform4_props.bounding_shape = dirt_platform4_model->size();
	m_dirt_platform4 = engine::game_object::create(dirt_platform4_props);
	m_dirt_platform4->set_offset(dirt_platform4_model->offset());
	//Added boxes for collison
	m_dirt_platform4_box.set_box(dirt_platform4_props.bounding_shape.x* dirt_platform4_scale, dirt_platform4_props.bounding_shape.z* dirt_platform4_scale,
		dirt_platform4_props.bounding_shape.y* dirt_platform4_scale,
		glm::vec3{ dirt_platform4_props.position.x, dirt_platform4_props.position.y, dirt_platform4_props.position.z });
	m_dirt_platform4_box2.set_box(dirt_platform4_props.bounding_shape.x* dirt_platform4_scale, 0.01f, dirt_platform4_props.bounding_shape.y* dirt_platform4_scale,
		glm::vec3{ dirt_platform4_props.position.x, dirt_platform4_props.position.y + dirt_platform4_props.bounding_shape.z * dirt_platform4_scale , dirt_platform4_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the dirt_platform model. Create a dirt_platform5 object. Set its properties
	engine::ref <engine::model> dirt_platform5_model = engine::model::create("assets/models/static/Building/Platform_CenterMiddle.fbx");
	engine::game_object_properties dirt_platform5_props;
	dirt_platform5_props.meshes = dirt_platform5_model->meshes();
	dirt_platform5_props.textures = dirt_platform5_model->textures();
	float dirt_platform5_scale = 1.f / glm::max(dirt_platform5_model->size().x, glm::max(dirt_platform5_model->size().y, dirt_platform5_model->size().z));
	dirt_platform5_props.position = { -2.f, 2.f, 7.f };
	dirt_platform5_props.rotation_axis = { 1.f, 0.f, 0.f };
	dirt_platform5_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	dirt_platform5_props.scale = glm::vec3(dirt_platform5_scale);
	dirt_platform5_props.bounding_shape = dirt_platform5_model->size();
	m_dirt_platform5 = engine::game_object::create(dirt_platform5_props);
	m_dirt_platform5->set_offset(dirt_platform5_model->offset());
	//Added boxes for collison
	m_dirt_platform5_box.set_box(dirt_platform5_props.bounding_shape.x* dirt_platform5_scale, dirt_platform5_props.bounding_shape.z* dirt_platform5_scale,
		dirt_platform5_props.bounding_shape.y* dirt_platform5_scale,
		glm::vec3{ dirt_platform5_props.position.x, dirt_platform5_props.position.y, dirt_platform5_props.position.z });
	m_dirt_platform5_box2.set_box(dirt_platform5_props.bounding_shape.x* dirt_platform5_scale, 0.01f, dirt_platform5_props.bounding_shape.y* dirt_platform5_scale,
		glm::vec3{ dirt_platform5_props.position.x, dirt_platform5_props.position.y + dirt_platform5_props.bounding_shape.z * dirt_platform5_scale , dirt_platform5_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	engine::ref <engine::model> moving_platform_model = engine::model::create("assets/models/static/Building/YellowBox.fbx");
	engine::game_object_properties moving_platform_props;
	moving_platform_props.meshes = moving_platform_model->meshes();
	moving_platform_props.textures = moving_platform_model->textures();
	float moving_platform_scale = 0.3f / glm::max(moving_platform_model->size().x, glm::max(moving_platform_model->size().y, moving_platform_model->size().z));
	moving_platform_props.position = { -1.f, 2.3f, 3.1f };
	moving_platform_props.rotation_axis = { 1.f, 0.f, 0.f };
	moving_platform_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	moving_platform_props.scale = glm::vec3(moving_platform_scale);
	moving_platform_props.bounding_shape = moving_platform_model->size();
	m_moving_platform = engine::game_object::create(moving_platform_props);
	//Added boxes for collison
	m_moving_platform_box.set_box(moving_platform_props.bounding_shape.x* moving_platform_scale*3.2, moving_platform_props.bounding_shape.y* moving_platform_scale *3.2,
		moving_platform_props.bounding_shape.z* moving_platform_scale*3.2,
		glm::vec3{ moving_platform_props.position.x, moving_platform_props.position.y + moving_platform_props.bounding_shape.y, moving_platform_props.position.z });
	m_moving_platform_box2.set_box(moving_platform_props.bounding_shape.x* moving_platform_scale*3.2, 0.01f, moving_platform_props.bounding_shape.z* moving_platform_scale*3.2,
		glm::vec3{ moving_platform_props.position.x, moving_platform_props.position.y , moving_platform_props.position.z });

	// model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	engine::ref <engine::model> moving_platform2_model = engine::model::create("assets/models/static/Building/YellowBox.fbx");
	engine::game_object_properties moving_platform2_props;
	moving_platform2_props.meshes = moving_platform_model->meshes();
	moving_platform2_props.textures = moving_platform2_model->textures();
	float moving_platform2_scale = 0.3f / glm::max(moving_platform2_model->size().x, glm::max(moving_platform2_model->size().y, moving_platform2_model->size().z));
	moving_platform2_props.position = { -2.f, 2.3f, 6.1f };
	moving_platform2_props.rotation_axis = { 1.f, 0.f, 0.f };
	moving_platform2_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	moving_platform2_props.scale = glm::vec3(moving_platform2_scale);
	moving_platform2_props.bounding_shape = moving_platform2_model->size();
	m_moving_platform2 = engine::game_object::create(moving_platform2_props);
	//Added boxes for collison
	m_moving_platform2_box.set_box(moving_platform2_props.bounding_shape.x* moving_platform2_scale * 3.2, moving_platform2_props.bounding_shape.y* moving_platform2_scale * 3.2,
		moving_platform2_props.bounding_shape.z* moving_platform2_scale * 3.2,
		glm::vec3{ moving_platform2_props.position.x, moving_platform2_props.position.y + moving_platform2_props.bounding_shape.y,moving_platform2_props.position.z });
	m_moving_platform2_box2.set_box(moving_platform2_props.bounding_shape.x* moving_platform2_scale * 3.2, 0.01f, moving_platform2_props.bounding_shape.z* moving_platform2_scale * 3.2,
		glm::vec3{ moving_platform2_props.position.x, moving_platform2_props.position.y , moving_platform2_props.position.z });
	//Add Electricity Effect to box
	for (uint32_t i = 0; i < 3; i++) {
		m_electricitys.push_back(electricity::create(moving_platform2_props.position, glm::vec3(0.f, 0.f, 1.f), 2.0f));
	}

	//load the pickup model
	//texture from https://www.textures.com/download/Fruit0017/20858
	engine::ref<engine::cuboid> pickup_shape = engine::cuboid::create(glm::vec3(0.1f), false);
	engine::ref<engine::texture_2d> pickup_texture = engine::texture_2d::create("assets/textures/melon.jpg", true);
	engine::game_object_properties pickup_props;
	pickup_props.position = { 7.f, 1.f, 11.f };
	pickup_props.meshes = { pickup_shape->mesh() };
	pickup_props.textures = { pickup_texture };
	m_pickup = pickup::create(pickup_props);
	m_pickup->init();

	//model from https://www.turbosquid.com/3d-models/banana-cartoon-3d-model-1495166
	// Load the banana model. Create a banana object. Set its properties
	engine::ref <engine::model> banana_model = engine::model::create("assets/models/static/Banana/Banana.FBX");
	engine::game_object_properties banana_props;
	banana_props.meshes = banana_model->meshes();
	banana_props.textures = banana_model->textures();
	float banana_scale = 0.2f / glm::max(banana_model->size().x, glm::max(banana_model->size().y, banana_model->size().z));
	banana_props.position = { 6.f, 0.8f, 1.f };
	banana_props.scale = glm::vec3(banana_scale);
	banana_props.bounding_shape = banana_model->size() / 2.f * banana_scale;
	m_banana = banana::create(banana_props);
	//Added boxes for collison
	m_banana_box.set_box(banana_props.bounding_shape.x * banana_scale, banana_props.bounding_shape.z * banana_scale, banana_props.bounding_shape.y * banana_scale,
		glm::vec3{ banana_props.position.x,banana_props.position.y,banana_props.position.z });
	m_banana->init();

	//model from https://www.turbosquid.com/3d-models/banana-cartoon-3d-model-1495166
	// Load the giantbanana model. Create a giantbanana object. Set its properties
	engine::ref <engine::model> giantbanana_model = engine::model::create("assets/models/static/banana/banana.FBX");
	engine::game_object_properties giantbanana_props;
	giantbanana_props.meshes = giantbanana_model->meshes();
	giantbanana_props.textures = giantbanana_model->textures();
	float giantbanana_scale = 0.4f / glm::max(giantbanana_model->size().x, glm::max(giantbanana_model->size().y, giantbanana_model->size().z));
	giantbanana_props.position = { -2.f, 5.3f, 6.1f };
	giantbanana_props.scale = glm::vec3(giantbanana_scale);
	giantbanana_props.bounding_shape = giantbanana_model->size() / 2.f * giantbanana_scale;
	m_giantbanana = giantbanana::create(giantbanana_props);
	//Added boxes for collison
	m_giantbanana_box.set_box(giantbanana_props.bounding_shape.x * giantbanana_scale, giantbanana_props.bounding_shape.z * giantbanana_scale, giantbanana_props.bounding_shape.y * giantbanana_scale,
		glm::vec3{ giantbanana_props.position.x,giantbanana_props.position.y,giantbanana_props.position.z });
	m_giantbanana->init();

	//model from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	// Load the powerup model. Create a powerup object. Set its properties
	engine::ref <engine::model> powerup_model = engine::model::create("assets/models/static/mushroom.fbx");
	engine::game_object_properties powerup_props;
	powerup_props.meshes = powerup_model->meshes();
	powerup_props.textures = powerup_model->textures();
	float powerup_scale = 0.2f / glm::max(powerup_model->size().x, glm::max(powerup_model->size().y, powerup_model->size().z));
	powerup_props.position = { 2.f, 0.8f, 1.f };
	powerup_props.scale = glm::vec3(powerup_scale);
	powerup_props.bounding_shape = powerup_model->size() / 2.f * powerup_scale;
	m_powerup = powerup::create(powerup_props);
	//Added boxes for collison
	m_powerup_box.set_box(powerup_props.bounding_shape.x * powerup_scale, powerup_props.bounding_shape.z * powerup_scale, powerup_props.bounding_shape.y * powerup_scale,
		glm::vec3{ powerup_props.position.x,powerup_props.position.y,powerup_props.position.z });
	m_powerup->init();

	//texture from https://creazilla.com/nodes/1403702-platformer-pack-3d-model
	engine::ref <engine::model> banana_ball_model = engine::model::create("assets/models/static/BananaBall.3ds");
	engine::game_object_properties banana_ball_props;
	banana_ball_props.position = {-20.f, 0.5f, 5.f };
	banana_ball_props.meshes = banana_ball_model->meshes();
	banana_ball_props.textures = banana_model->textures();
	float banana_ball_scale = 0.2f / glm::max(banana_model->size().x, glm::max(banana_model->size().y, banana_model->size().z));
	banana_ball_props.type = 1;
	banana_ball_props.bounding_shape = banana_ball_model->size() / 2.0f;
	banana_ball_props.restitution = 0.92f;
	banana_ball_props.mass = 0.45f;
	banana_ball_props.scale = glm::vec3(banana_scale);
	banana_ball_props.rolling_friction = 0.1f;
	m_angry_monkey_projectile = engine::game_object::create(banana_ball_props);
	m_angry_monkey_projectile2 = engine::game_object::create(banana_ball_props);
	m_angry_monkey_projectile3 = engine::game_object::create(banana_ball_props);
	m_angry_monkey_projectile4 = engine::game_object::create(banana_ball_props);
	m_angry_monkey_projectile5 = engine::game_object::create(banana_ball_props);
	//Added boxes for collison
	m_banana_ball_box.set_box(1.f,1.f,1.f, banana_ball_props.position);
	m_banana_ball_box2.set_box(1.f, 1.f, 1.f, banana_ball_props.position);
	m_banana_ball_box3.set_box(1.f, 1.f, 1.f, banana_ball_props.position);
	m_banana_ball_box4.set_box(1.f, 1.f, 1.f, banana_ball_props.position);
	m_banana_ball_box5.set_box(1.f, 1.f, 1.f, banana_ball_props.position);
	//initalise the projectile
	m_projectile.initialise(m_angry_monkey_projectile);
	m_projectile2.initialise(m_angry_monkey_projectile2);
	m_projectile3.initialise(m_angry_monkey_projectile3);
	m_projectile4.initialise(m_angry_monkey_projectile4);
	m_projectile5.initialise(m_angry_monkey_projectile5);


	//model https://www.turbosquid.com/3d-models/3d-wooden-box-model-1304417
	// Load the crate model. Create a crate object. Set its properties
	engine::ref <engine::model> crate_model = engine::model::create("assets/models/static/Crate/crate4.fbx");
	engine::game_object_properties crate_props;
	crate_props.meshes = crate_model->meshes();
	crate_props.textures = crate_model->textures();
	float crate_scale = 1.f / glm::max(crate_model->size().x, glm::max(crate_model->size().y, crate_model->size().z));
	crate_props.position = { 1.f, 0.2f, 6.f };
	crate_props.scale = glm::vec3(crate_scale);
	crate_props.bounding_shape = crate_model->size();
	m_crate= engine::game_object::create(crate_props);
	m_crate->set_offset(crate_model->offset());
	//Added boxes for collison
	m_crate_box.set_box(crate_props.bounding_shape.x* crate_scale, crate_props.bounding_shape.y* crate_scale, crate_props.bounding_shape.z* crate_scale, crate_props.position);
	m_crate_box2.set_box(crate_props.bounding_shape.x* crate_scale, 0.01f, crate_props.bounding_shape.y* crate_scale,
		glm::vec3{ crate_props.position.x, crate_props.position.y + crate_props.bounding_shape.z * crate_scale , crate_props.position.z });

	// model https://free3d.com/3d-model/castle-walls-37690.html
	// Load the castle model. Create a castle object. Set its properties
	engine::ref <engine::model> castle_model = engine::model::create("assets/models/static/Castle/castle5.fbx");
	engine::game_object_properties castle_props;
	castle_props.meshes = castle_model->meshes();
	castle_props.textures = castle_model->textures();
	float castle_scale = 15.f / glm::max(castle_model->size().x, glm::max(castle_model->size().y, castle_model->size().z));
	castle_props.rotation_axis = { 1.f, 0.f, 0.f };
	castle_props.rotation_amount = { -glm::pi<float>() / 2.0f };
	castle_props.position = { 3.f, 0.8f, 16.f };
	castle_props.scale = glm::vec3(castle_scale);
	castle_props.bounding_shape = castle_model->size();
	m_castle = engine::game_object::create(castle_props);
	//Added boxes for collison(one for each wall)
	m_castle->set_offset(castle_model->offset());
	m_castlewall1.set_box(15.f, 20.f, 3.f, glm::vec3(3.f, 0.f, -1.1f));//pink //back
	m_castlewall2.set_box(15.f, 20.f, 3.f, glm::vec3(3.f, 0.f, 13.4f));//green //front
	m_castlewall3.set_box(3.f, 20.f, 15.f, glm::vec3(-4.75f, 0.f, 6.f));//blue //left
	m_castlewall4.set_box(3.f, 20.f, 15.f, glm::vec3(10.8f, 0.f, 6.f));//white //right

	////////////
	//Primatives
	////////////

	//set log model vertices
	std::vector<glm::vec3> triangular_prism_vertices;
	triangular_prism_vertices.push_back(glm::vec3(0.f, 0.2f, 0.f)); //0
	triangular_prism_vertices.push_back(glm::vec3(0.f, 0.f, 0.1f)); //1
	triangular_prism_vertices.push_back(glm::vec3(0.f, 0.f, -0.1f)); //2
	triangular_prism_vertices.push_back(glm::vec3(1.2f, 0.2f, 0.f)); //3
	triangular_prism_vertices.push_back(glm::vec3(1.2f, 0.f, 0.1f)); //4
	triangular_prism_vertices.push_back(glm::vec3(1.2f, 0.f, -0.1f)); //5
	//create log model(primative)
	engine::ref<engine::triangular_prism> triangular_prism_shape =
	engine::triangular_prism::create(triangular_prism_vertices);
	engine::game_object_properties triangular_prism_props;
	triangular_prism_props.position = { 6.f, 0.5f, 8.f };
	triangular_prism_props.meshes = { triangular_prism_shape->mesh() };
	//texture from https://opengameart.org/content/wood-aged-seamless-handcrafted-texture-holz-dark-seamlessjpg
	std::vector<engine::ref<engine::texture_2d>> triangular_prism_textures = { engine::texture_2d::create("assets/textures/bark.jpg", false) };
	triangular_prism_props.textures = triangular_prism_textures;
	m_triangular_prism = engine::game_object::create(triangular_prism_props);

	//set square pyramid model vertices
	std::vector<glm::vec3> square_pyramid_vertices;
	square_pyramid_vertices.push_back(glm::vec3(0.5f, 1.f, 0.5f)); //0
	square_pyramid_vertices.push_back(glm::vec3(0.f, 0.f, 1.f)); //1
	square_pyramid_vertices.push_back(glm::vec3(0.f, 0.f, 0.0f)); //2
	square_pyramid_vertices.push_back(glm::vec3(1.f, 0.f, 0.f)); //3
	square_pyramid_vertices.push_back(glm::vec3(1.f, 0.f, 1.f)); //4
	//create square pyramid model (primative)
	engine::ref<engine::square_pyramid> square_pyramid_shape =
	engine::square_pyramid::create(square_pyramid_vertices);
	engine::game_object_properties square_pyramid_props;
	square_pyramid_props.position = { 6.f, 0.2f, 10.f };
	square_pyramid_props.meshes = { square_pyramid_shape->mesh() };
	std::vector<engine::ref<engine::texture_2d>> square_pyramid_textures = { engine::texture_2d::create("assets/textures/bark.jpg", false) };
	// texture from https ://opengameart.org/content/wood-aged-seamless-handcrafted-texture-holz-dark-seamlessjpg
	square_pyramid_props.textures = square_pyramid_textures;
	m_square_pyramid = engine::game_object::create(square_pyramid_props);

	//set isosceles_triangular_prism  vertices
	std::vector<glm::vec3> isosceles_triangular_prism_vertices;
	isosceles_triangular_prism_vertices.push_back(glm::vec3(0.f, 1.f, 0.f)); //0
	isosceles_triangular_prism_vertices.push_back(glm::vec3(0.f, 0.f, 1.f)); //1
	isosceles_triangular_prism_vertices.push_back(glm::vec3(0.f, 0.f, 0.f)); //2
	isosceles_triangular_prism_vertices.push_back(glm::vec3(1.f, 1.f, 0.f)); //3
	isosceles_triangular_prism_vertices.push_back(glm::vec3(1.f, 0.f, 1.f)); //4
	isosceles_triangular_prism_vertices.push_back(glm::vec3(1.f, 0.f, 0.f)); //5
	//create isosceles_triangular_prism model(primative)
	engine::ref<engine::isosceles_triangular_prism> isosceles_triangular_prism_shape =
		engine::isosceles_triangular_prism::create(isosceles_triangular_prism_vertices);
	engine::game_object_properties isosceles_triangular_prism_props;
	isosceles_triangular_prism_props.position = { 6.f, 0.2f, 12.f };
	isosceles_triangular_prism_props.meshes = { isosceles_triangular_prism_shape->mesh() };
	std::vector<engine::ref<engine::texture_2d>> isosceles_triangular_prism_textures = { engine::texture_2d::create("assets/textures/bark.jpg", false) };
	// texture from https ://opengameart.org/content/wood-aged-seamless-handcrafted-texture-holz-dark-seamlessjpg
	isosceles_triangular_prism_props.textures = isosceles_triangular_prism_textures;
	isosceles_triangular_prism_props.rotation_axis = { 0.f, 1.f, 0.f };
	isosceles_triangular_prism_props.rotation_amount = { glm::pi<float>() /2};
	m_isosceles_triangular_prism = engine::game_object::create(isosceles_triangular_prism_props);

	//////////////
	//Miscelaneous
	//////////////

	//added objects that are using bullet to m_game_objects
	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_angry_monkey_projectile);
	m_game_objects.push_back(m_angry_monkey_projectile2);
	m_game_objects.push_back(m_angry_monkey_projectile3);
	m_game_objects.push_back(m_angry_monkey_projectile4);
	m_game_objects.push_back(m_angry_monkey_projectile5);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	//create text manager for hud 
	m_text_manager = engine::text_manager::create();

	//create menu
	//menu background from https://opengameart.org/content/the-war-over-a-melon-field-main-menu-background
	m_menu = menu::create("assets/textures/menu.bmp", 2.0f, 1.6f, 0.9f);
	//create death screen
	m_deathscreen = deathscreen::create("assets/textures/die.bmp", 2.0f, 1.6f, 0.9f);
	//create hud
	m_hudfade = hudfade::create("assets/textures/hud.bmp", 2.0f, 0.9f, 0.06f);
	//create hp bar
	m_hpfade = hpfade::create("assets/textures/dmg.bmp", 2.0f, 0.8f, 0.2f);
	//create damage screen overlay
	m_damagefade = damagefade::create("assets/textures/dmg.bmp", 2.0f, 1.6f, 0.9f);
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
	
	//////////////////////////////
	//store old positions and HP
	//////////////////////////////

	//old player position 
	glm::vec3 oldpos = m_player.object()->position();
	//old enemy position 
	glm::vec3 enemy_oldpos = m_enemy.object()->position();
	glm::vec3 enemy_oldpos2 = m_enemy2.object()->position();
	glm::vec3 enemy_oldpos3 = m_enemy3.object()->position();
	glm::vec3 enemy_oldpos4 = m_enemy4.object()->position();
	glm::vec3 enemy_oldpos5 = m_enemy5.object()->position();
	//old camera position 
	glm::vec3 campos = m_3d_camera.position();
	//store old hp
	int old_hp = m_player.get_hp();

	///////////////////
	//update positions
	//////////////////
	
	//update player
	m_player.on_update(time_step);
	m_player_box.on_update(m_player.object()->position());
	m_player_feet_box.on_update(m_player.object()->position());

	//update npc
	m_enemy.on_update(time_step, m_player.object()->position());
	m_angry_monkey_box.on_update(m_enemy.object()->position());
	m_enemy2.on_update(time_step, m_player.object()->position());
	m_angry_monkey_box2.on_update(m_enemy2.object()->position());
	m_enemy3.on_update(time_step, m_player.object()->position());
	m_angry_monkey_box3.on_update(m_enemy3.object()->position());
	m_enemy4.on_update(time_step, m_player.object()->position());
	m_angry_monkey_box4.on_update(m_enemy4.object()->position());
	m_enemy5.on_update(time_step, m_player.object()->position());
	m_angry_monkey_box5.on_update(m_enemy5.object()->position());

	//update moving platform
	m_moving_platform_box.on_update({ m_moving_platform->position().x + m_move_amount,m_moving_platform->position().y - 0.5f, m_moving_platform->position().z });
	m_moving_platform_box2.on_update({ m_moving_platform->position().x + m_move_amount,m_moving_platform->position().y + 0.47f, m_moving_platform->position().z });
	m_moving_platform2_box.on_update({ m_moving_platform2->position().x,m_moving_platform2->position().y - 0.5f + m_move_amount, m_moving_platform2->position().z });
	m_moving_platform2_box2.on_update({ m_moving_platform2->position().x,m_moving_platform2->position().y + 0.49f + m_move_amount, m_moving_platform2->position().z });

	//update projectile
	m_banana_ball_box.on_update(m_projectile.object()->position());
	m_banana_ball_box2.on_update(m_projectile2.object()->position());
	m_banana_ball_box3.on_update(m_projectile3.object()->position());
	m_banana_ball_box4.on_update(m_projectile4.object()->position());
	m_banana_ball_box5.on_update(m_projectile5.object()->position());

	if (m_enemy.is_shooting() == true)
	{
		m_projectile.throw_ball(m_angry_monkey->forward(), 40.0f, { m_angry_monkey->position().x, m_angry_monkey->position().y+0.3f, m_angry_monkey->position().z });
		m_enemy.shootdone();
	}
	if (m_enemy2.is_shooting() == true)
	{
		m_projectile2.throw_ball(m_angry_monkey2->forward(), 40.0f, { m_angry_monkey2->position().x, m_angry_monkey2->position().y + 0.3f, m_angry_monkey2->position().z });
		m_enemy2.shootdone();
	}
	if (m_enemy3.is_shooting() == true)
	{
		m_projectile3.throw_ball(m_angry_monkey3->forward(), 40.0f, { m_angry_monkey3->position().x, m_angry_monkey3->position().y + 0.3f, m_angry_monkey3->position().z });
		m_enemy3.shootdone();
	}
	if (m_enemy4.is_shooting() == true)
	{
		m_projectile4.throw_ball(m_angry_monkey4->forward(), 40.0f, { m_angry_monkey4->position().x, m_angry_monkey4->position().y + 0.3f, m_angry_monkey4->position().z });
		m_enemy4.shootdone();
	}
	if (m_enemy5.is_shooting() == true)
	{
		m_projectile5.throw_ball(m_angry_monkey5->forward(), 40.0f, { m_angry_monkey5->position().x, m_angry_monkey5->position().y + 0.3f, m_angry_monkey5->position().z });
		m_enemy5.shootdone();
	}
	
	//update bullter physics and audio
	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));
	m_audio_manager->update_with_camera(m_3d_camera);

	//activate powerups 
	m_pickup->update(m_player.object()->position() , time_step);
	m_banana->update(m_player.object()->position() , time_step);
	m_powerup->update(m_player.object()->position(), time_step);
	m_giantbanana->update(m_player.object()->position(), time_step);

	////////////
	//collisions
	////////////

	//check for player collision with walls
	if (m_castlewall1.collision(m_player_box) | m_castlewall2.collision(m_player_box) | m_castlewall3.collision(m_player_box) | m_castlewall4.collision(m_player_box))
	{
		m_player.object()->set_position(oldpos);
		m_player.bounceback();
		m_player.set_grounded(false);
	}
	if (m_castlewall1.collision(m_angry_monkey_box) | m_castlewall2.collision(m_angry_monkey_box) | m_castlewall3.collision(m_angry_monkey_box) | m_castlewall4.collision(m_angry_monkey_box) | m_crate_box.collision(m_angry_monkey_box))
	{
		m_enemy.object()->set_position(enemy_oldpos);
		glm::vec3 old_direction = m_enemy.object()->forward();
		m_enemy.object()->set_forward(old_direction*-1.f);

	}
	if (m_castlewall1.collision(m_angry_monkey_box2) | m_castlewall2.collision(m_angry_monkey_box2) | m_castlewall3.collision(m_angry_monkey_box2) | m_castlewall4.collision(m_angry_monkey_box2) | m_crate_box.collision(m_angry_monkey_box2))
	{
		m_enemy2.object()->set_position(enemy_oldpos2);
		glm::vec3 old_direction2 = m_enemy2.object()->forward();
		m_enemy2.object()->set_forward(old_direction2 * -1.f);

	}
	if (m_castlewall1.collision(m_angry_monkey_box3) | m_castlewall2.collision(m_angry_monkey_box3) | m_castlewall3.collision(m_angry_monkey_box3) | m_castlewall4.collision(m_angry_monkey_box3) | m_crate_box.collision(m_angry_monkey_box3))
	{
		m_enemy3.object()->set_position(enemy_oldpos3);
		glm::vec3 old_direction3 = m_enemy3.object()->forward();
		m_enemy3.object()->set_forward(old_direction3 * -1.f);

	}
	if (m_castlewall1.collision(m_angry_monkey_box4) | m_castlewall2.collision(m_angry_monkey_box4) | m_castlewall3.collision(m_angry_monkey_box4) | m_castlewall4.collision(m_angry_monkey_box4) | m_crate_box.collision(m_angry_monkey_box4))
	{
		m_enemy4.object()->set_position(enemy_oldpos4);
		glm::vec3 old_direction4 = m_enemy4.object()->forward();
		m_enemy4.object()->set_forward(old_direction4 * -1.f);

	}
	if (m_castlewall1.collision(m_angry_monkey_box5) | m_castlewall2.collision(m_angry_monkey_box5) | m_castlewall3.collision(m_angry_monkey_box5) | m_castlewall4.collision(m_angry_monkey_box5) | m_crate_box.collision(m_angry_monkey_box5))
	{
		m_enemy5.object()->set_position(enemy_oldpos5);
		glm::vec3 old_direction5 = m_enemy5.object()->forward();
		m_enemy5.object()->set_forward(old_direction5 * -1.f);

	}

	// check for collision with projectile
	if (m_player_box.collision(m_banana_ball_box)) {
		m_player.bounceback();
	}
	if (m_player_box.collision(m_banana_ball_box2)) {
		m_player.bounceback();
	}
	if (m_player_box.collision(m_banana_ball_box3)) {
		m_player.bounceback();
	}
	if (m_player_box.collision(m_banana_ball_box4)) {
		m_player.bounceback();
	}
	if (m_player_box.collision(m_banana_ball_box5)) {
		m_player.bounceback();
	}

	//check for collisions with platforms
	if (m_player_feet_box.collision(m_dirt_platform_box2) | (m_player_feet_box.collision(m_crate_box2)) | (m_player_feet_box.collision(m_dirt_platform2_box2)) | (m_player_feet_box.collision(m_dirt_platform3_box2)) | (m_player_feet_box.collision(m_dirt_platform4_box2)) | (m_player_feet_box.collision(m_dirt_platform5_box2)) | (m_player_feet_box.collision(m_tower_box2)) | (m_player_feet_box.collision(m_moving_platform_box2)) | (m_player_feet_box.collision(m_moving_platform2_box2)))
	{
		//store new position
		glm::vec3 newpos = m_player.object()->position();
		//check for collision with moving platforms
		if (m_player_feet_box.collision(m_moving_platform_box2) | m_player_feet_box.collision(m_moving_platform2_box2))
		{
			if (m_player_feet_box.collision(m_moving_platform_box2))
			{
				//Move the platform with the player
				if (moveforward == true)
				{
					m_player.object()->set_position({ newpos.x + 0.01f, oldpos.y, newpos.z });
				}
				else
				{
					m_player.object()->set_position({ newpos.x - 0.01f, oldpos.y, newpos.z });

				}
			}
			if (m_player_feet_box.collision(m_moving_platform2_box2))
			{
				//Move the platform with the player
				if (moveforward == true)
				{
					m_player.object()->set_position({ newpos.x , oldpos.y + 0.01f, newpos.z });
				}
				else
				{
					m_player.object()->set_position({ newpos.x , oldpos.y - 0.01f, newpos.z });
				}
			}
		}
		else {
			m_player.object()->set_position({ newpos.x, oldpos.y, newpos.z });
		}
		m_player.set_gravity(0.f);
		m_player.set_grounded(true);
		m_player_box.on_update(m_player.object()->position());
		m_player_feet_box.on_update(m_player.object()->position());
	}
	else
	{
		//check for side collisions
		if (m_player_box.collision(m_dirt_platform_box) | m_player_box.collision(m_crate_box) | m_player_box.collision(m_dirt_platform2_box) | m_player_box.collision(m_dirt_platform3_box) | m_player_box.collision(m_dirt_platform4_box) | m_player_box.collision(m_dirt_platform5_box) | (m_player_feet_box.collision(m_tower_box)) | (m_player_feet_box.collision(m_moving_platform_box)) | (m_player_feet_box.collision(m_moving_platform2_box)))
		{
			m_player.object()->set_position(oldpos);
			m_player.bounceback();
			m_player.set_grounded(false);
		}
		m_player.set_gravity(-5.f);
		
		//check if on floor
		if (m_player.object()->position().y > 0.55f) {
			m_player.set_grounded(false);
		}
		else {
			m_player.set_grounded(true);
		}
	}

	//check for collision with npc
	if (m_angry_monkey_box.collision(m_player_box))
	{
		m_player.object()->set_position(oldpos);
		m_enemy.object()->set_position(enemy_oldpos);
		m_player.remove_hp(50);
		
	}
	//check for collision with npc
	if (m_angry_monkey_box2.collision(m_player_box))
	{
		m_player.object()->set_position(oldpos);
		m_enemy2.object()->set_position(enemy_oldpos2);
		m_player.remove_hp(50);

	}
	//check for collision with npc
	if (m_angry_monkey_box3.collision(m_player_box))
	{
		m_player.object()->set_position(oldpos);
		m_enemy3.object()->set_position(enemy_oldpos3);
		m_player.remove_hp(50);

	}
	//check for collision with npc
	if (m_angry_monkey_box4.collision(m_player_box))
	{
		m_player.object()->set_position(oldpos);
		m_enemy4.object()->set_position(enemy_oldpos4);
		m_player.remove_hp(50);

	}
	//check for collision with npc
	if (m_angry_monkey_box5.collision(m_player_box))
	{
		m_player.object()->set_position(oldpos);
		m_enemy5.object()->set_position(enemy_oldpos5);
		m_player.remove_hp(50);

	}

	/////////////
	//special fx
	/////////////
	
	//check for damage
	int new_hp = m_player.get_hp();
	if (startofgame == false && m_game_ended==false){
		// activate hud
		m_hudfade->activate();
		
		m_hpfade = hpfade::create("assets/textures/dmg.bmp", 2.0f, 0.005f * new_hp/2, 0.02f);
		m_hpfade->set_position(0.005f * new_hp/2);
		m_hpfade->activate();
	}
	
	
	if (new_hp< old_hp)
	{
		m_damagefade->activate();
	}

	//check for gamewon
	
	if (m_game_won==true)
	{
		m_damagefade->setlayer(-0.1f);
		m_damagefade->activate();
	}

	//activate electricity
	for (uint32_t i = 0; i < m_electricitys.size(); i++) {
		m_electricitys.at(i)->on_update(time_step);
	}

	/////////////////////
	//main menu and endgame
	/////////////////////

	gametime += (float)time_step;
	if (gametime > m_maxtime) {
		m_player.remove_hp(500);
	}

	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	if (startofgame == true)
	{
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
		m_menu->activate();
		m_menu->on_update(time_step);
		m_menu->update_camera(m_3d_camera);

	}
	else {
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
		if (m_play_cutscene == true) {
			if (m_moved_camera == false) {
				m_3d_camera.position({ -1.f, 3.f, 12.f });
				m_3d_camera.forward(0.f, 0.f, 1.f);
				m_moved_camera = true;
			}

			m_3d_camera.on_update(time_step);
		}
		else {
			m_player.update_camera(m_3d_camera);
		}
		if (m_player.get_isdead() == true && m_game_won == false)
		{
			//m_audio_manager->pause("music");
			std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", false);
			m_deathscreen->activate();
			m_deathscreen->on_update(time_step);
			m_deathscreen->update_camera(m_3d_camera);
		}
	}
		
} 

void example_layer::on_render() 
{
    engine::render_command::clear_color({0.2f, 0.3f, 0.3f, 1.0f}); 
    engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	//add fog
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("fog_on", m_fog);
	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);
	engine::renderer::submit(mesh_shader, m_terrain);

	//////////
	//Lighting
	//////////

	//point light soruce
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gNumPointLights", (int)num_point_lights);
	m_pointLight.submit(mesh_shader, 0);
	m_pointLight.Position.x= m_player.object()->position().x;
	m_pointLight.Position.z = m_player.object()->position().z;

	//spot light source
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gNumSpotLights", (int)num_spot_lights);
	m_spotLight.submit(mesh_shader, 0);

	///////////////////////
	//Render Static Objects
	///////////////////////

	//render enemy
	glm::mat4 angry_monkey_transform(1.0f);
	angry_monkey_transform = glm::translate(angry_monkey_transform, m_angry_monkey->position());
	angry_monkey_transform = glm::rotate(angry_monkey_transform, m_angry_monkey->rotation_amount(), m_angry_monkey->rotation_axis());
	angry_monkey_transform = glm::scale(angry_monkey_transform, m_angry_monkey->scale());
	engine::renderer::submit(mesh_shader, angry_monkey_transform, m_angry_monkey);
	glm::mat4 angry_monkey_transform2(1.0f);
	angry_monkey_transform2 = glm::translate(angry_monkey_transform2, m_angry_monkey2->position());
	angry_monkey_transform2 = glm::rotate(angry_monkey_transform2, m_angry_monkey2->rotation_amount(), m_angry_monkey2->rotation_axis());
	angry_monkey_transform2 = glm::scale(angry_monkey_transform2, m_angry_monkey2->scale());
	engine::renderer::submit(mesh_shader, angry_monkey_transform2, m_angry_monkey2);
	glm::mat4 angry_monkey_transform3(1.0f);
	angry_monkey_transform3 = glm::translate(angry_monkey_transform3, m_angry_monkey3->position());
	angry_monkey_transform3 = glm::rotate(angry_monkey_transform3, m_angry_monkey3->rotation_amount(), m_angry_monkey3->rotation_axis());
	angry_monkey_transform3 = glm::scale(angry_monkey_transform3, m_angry_monkey3->scale());
	engine::renderer::submit(mesh_shader, angry_monkey_transform3, m_angry_monkey3);
	glm::mat4 angry_monkey_transform4(1.0f);
	angry_monkey_transform4 = glm::translate(angry_monkey_transform4, m_angry_monkey4->position());
	angry_monkey_transform4 = glm::rotate(angry_monkey_transform4, m_angry_monkey4->rotation_amount(), m_angry_monkey4->rotation_axis());
	angry_monkey_transform4 = glm::scale(angry_monkey_transform4, m_angry_monkey4->scale());
	engine::renderer::submit(mesh_shader, angry_monkey_transform4, m_angry_monkey4);
	glm::mat4 angry_monkey_transform5(1.0f);
	angry_monkey_transform5 = glm::translate(angry_monkey_transform5, m_angry_monkey5->position());
	angry_monkey_transform5 = glm::rotate(angry_monkey_transform5,m_angry_monkey5->rotation_amount(), m_angry_monkey->rotation_axis());
	angry_monkey_transform5 = glm::scale(angry_monkey_transform5, m_angry_monkey5->scale());
	engine::renderer::submit(mesh_shader, angry_monkey_transform5, m_angry_monkey5);

	//render moving_platforms
	//Calculate the movement
	if (m_turn_on_platforms == true) {

		if (moveforward == true) {
			m_move_amount += 0.01f;
			if (m_move_amount > 3)
			{
				moveforward = false;
			}
		}
		else
		{
			m_move_amount -= 0.01f;
			if (m_move_amount < -1)
			{
				moveforward = true;
			}
		}
	}

	m_platform_material->submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
	m_moving_platform->textures().at(0)->bind();
	glm::mat4 moving_platform_transform(1.0f);
	moving_platform_transform = glm::translate(moving_platform_transform, { m_moving_platform->position().x + m_move_amount , m_moving_platform->position().y, m_moving_platform->position().z });
	moving_platform_transform = glm::rotate(moving_platform_transform, m_moving_platform->rotation_amount(), m_moving_platform->rotation_axis());
	engine::renderer::submit(mesh_shader, m_moving_platform->meshes().at(0), moving_platform_transform);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
	m_moving_platform2->textures().at(0)->bind();
	glm::mat4 moving_platform2_transform(1.0f);
	moving_platform2_transform = glm::translate(moving_platform2_transform, { m_moving_platform2->position().x, m_moving_platform2->position().y + m_move_amount, m_moving_platform2->position().z });
	moving_platform2_transform = glm::rotate(moving_platform2_transform, m_moving_platform2->rotation_amount(), m_moving_platform2->rotation_axis());
	engine::renderer::submit(mesh_shader, m_moving_platform2->meshes().at(0), moving_platform2_transform);

	//render tower
	glm::mat4 tower_transform(1.0f);
	tower_transform = glm::translate(tower_transform, m_tower->position() - glm::vec3(m_tower->offset().x, -1.f, m_tower->offset().z) * m_tower->scale().x);
	tower_transform = glm::rotate(tower_transform, m_tower->rotation_amount(), m_tower->rotation_axis());
	tower_transform = glm::scale(tower_transform, m_tower->scale());
	engine::renderer::submit(mesh_shader, tower_transform, m_tower);

	//render castle
	glm::mat4 castle_transform(1.0f);
	castle_transform = glm::translate(castle_transform, m_castle->position() - glm::vec3(m_castle->offset().x, 0.f, m_castle->offset().z) * m_castle->scale().x);
	castle_transform = glm::rotate(castle_transform, m_castle->rotation_amount(), m_castle->rotation_axis());
	castle_transform = glm::scale(castle_transform, m_castle->scale());
	engine::renderer::submit(mesh_shader, castle_transform, m_castle);

	//render dirt platform 1
	glm::mat4 dirt_platform_transform(1.0f);
	dirt_platform_transform = glm::translate(dirt_platform_transform, m_dirt_platform->position() - glm::vec3(m_dirt_platform->offset().x,-1.f, m_dirt_platform->offset().z) * m_dirt_platform->scale().x);
	dirt_platform_transform = glm::rotate(dirt_platform_transform, m_dirt_platform->rotation_amount(), m_dirt_platform->rotation_axis());
	dirt_platform_transform = glm::scale(dirt_platform_transform, m_dirt_platform->scale());

	engine::renderer::submit(mesh_shader, dirt_platform_transform, m_dirt_platform);

	//render dirt platform 2
	glm::mat4 dirt_platform2_transform(1.0f);
	dirt_platform2_transform = glm::translate(dirt_platform2_transform, m_dirt_platform2->position() - glm::vec3(m_dirt_platform2->offset().x, -1.f, m_dirt_platform2->offset().z) * m_dirt_platform2->scale().x);
	dirt_platform2_transform = glm::rotate(dirt_platform2_transform, m_dirt_platform2->rotation_amount(), m_dirt_platform2->rotation_axis());
	dirt_platform2_transform = glm::scale(dirt_platform2_transform, m_dirt_platform2->scale());
	engine::renderer::submit(mesh_shader, dirt_platform2_transform, m_dirt_platform2);

	//render dirt platform 3
	glm::mat4 dirt_platform3_transform(1.0f);
	dirt_platform3_transform = glm::translate(dirt_platform3_transform, m_dirt_platform3->position() - glm::vec3(m_dirt_platform3->offset().x, -1.f, m_dirt_platform3->offset().z) * m_dirt_platform3->scale().x);
	dirt_platform3_transform = glm::rotate(dirt_platform3_transform, m_dirt_platform3->rotation_amount(), m_dirt_platform3->rotation_axis());
	dirt_platform3_transform = glm::scale(dirt_platform3_transform, m_dirt_platform3->scale());
	engine::renderer::submit(mesh_shader, dirt_platform3_transform, m_dirt_platform3);

	//render dirt platform 4
	glm::mat4 dirt_platform4_transform(1.0f);
	dirt_platform4_transform = glm::translate(dirt_platform4_transform, m_dirt_platform4->position() - glm::vec3(m_dirt_platform4->offset().x, -1.f, m_dirt_platform4->offset().z) * m_dirt_platform4->scale().x);
	dirt_platform4_transform = glm::rotate(dirt_platform4_transform, m_dirt_platform4->rotation_amount(), m_dirt_platform4->rotation_axis());
	dirt_platform4_transform = glm::scale(dirt_platform4_transform, m_dirt_platform4->scale());
	engine::renderer::submit(mesh_shader, dirt_platform4_transform, m_dirt_platform4);

	//render dirt platform 5
	glm::mat4 dirt_platform5_transform(1.0f);
	dirt_platform5_transform = glm::translate(dirt_platform5_transform, m_dirt_platform5->position() - glm::vec3(m_dirt_platform5->offset().x, -1.f, m_dirt_platform5->offset().z) * m_dirt_platform5->scale().x);
	dirt_platform5_transform = glm::rotate(dirt_platform5_transform, m_dirt_platform5->rotation_amount(), m_dirt_platform5->rotation_axis());
	dirt_platform5_transform = glm::scale(dirt_platform5_transform, m_dirt_platform5->scale());
	engine::renderer::submit(mesh_shader, dirt_platform5_transform, m_dirt_platform5);

	//render primatives 
	engine::renderer::submit(mesh_shader, m_triangular_prism);
	engine::renderer::submit(mesh_shader, m_square_pyramid);
	engine::renderer::submit(mesh_shader, m_isosceles_triangular_prism);

	//render crate
	glm::mat4 crate_transform(1.0f);
	crate_transform = glm::translate(crate_transform, m_crate->position() - glm::vec3(m_crate->offset().x, -15.f, m_crate->offset().z) * m_crate->scale().x);
	crate_transform = glm::rotate(crate_transform, m_crate->rotation_amount(), m_crate->rotation_axis());
	crate_transform = glm::scale(crate_transform, m_crate->scale());
	engine::renderer::submit(mesh_shader, crate_transform, m_crate);

	//render sign
	glm::mat4 sign_transform(1.0f);
	sign_transform = glm::translate(sign_transform, m_sign->position());
	sign_transform = glm::rotate(sign_transform, m_sign->rotation_amount(), m_sign->rotation_axis());
	sign_transform = glm::scale(sign_transform, m_sign->scale());
	engine::renderer::submit(mesh_shader, sign_transform, m_sign);

	//render star
	glm::mat4 star_transform(1.0f);
	star_transform = glm::translate(star_transform, m_star->position());
	star_transform = glm::rotate(star_transform, m_star->rotation_amount(), m_star->rotation_axis());
	star_transform = glm::scale(star_transform, m_sign->scale());
	engine::renderer::submit(mesh_shader, star_transform, m_star);

	////////////////////////////////////////
	//Render the items and check for effects
	////////////////////////////////////////

	m_pickup_material->submit(mesh_shader);
	//render pickup if active
	if (m_pickup->active())
	{
		std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("has_texture", true);
		m_pickup->textures().at(0)->bind();
		glm::mat4 pickup_transform(1.0f);
		pickup_transform = glm::translate(pickup_transform, m_pickup->position());
		pickup_transform = glm::rotate(pickup_transform, m_pickup->rotation_amount(), m_pickup->rotation_axis());
		engine::renderer::submit(mesh_shader, m_pickup->meshes().at(0), pickup_transform);
	}
	else
	{
		//apply the pickup effect
		m_play_cutscene = true;
		m_turn_on_platforms = true;
		m_cutscene_timer++;
		if (m_cutscene_timer > 60)
		{
			m_play_cutscene = false;
			
		}
		//render fx
		for (uint32_t i = 0; i < m_electricitys.size(); i++) {
			m_electricitys.at(i)->on_render(mesh_shader);
		}
		for (uint32_t i = 0; i < m_electricitys.size(); i++) {
			m_electricitys.at(i)->activate({ m_moving_platform->position().x + m_move_amount, m_moving_platform->position().y, m_moving_platform->position().z }, glm::vec3(-1.f, 0.f, 0.f));
		}
		m_fog = false;
	}

	//render banana
	if (m_banana->active())
	{
		//render banana
		glm::mat4 banana_transform(1.0f);
		banana_transform = glm::translate(banana_transform, m_banana->position());
		banana_transform = glm::rotate(banana_transform, m_banana->rotation_amount(), m_banana->rotation_axis());
		banana_transform = glm::scale(banana_transform, m_banana->scale());
		engine::renderer::submit(mesh_shader, banana_transform, m_banana);
	}
	else
	{
		if (m_banana->is_banana_eaten() == false) {
			m_player.eatbanana();
			m_banana->set_banana_eaten(true);
			m_audio_manager->play_spatialised_sound("monkey", m_3d_camera.position(), m_angry_monkey->position());
		}
	}

	//render giantbanana
	if (m_giantbanana->active())
	{
		//render giantbanana
		glm::mat4 giantbanana_transform(1.0f);
		giantbanana_transform = glm::translate(giantbanana_transform, m_giantbanana->position());
		giantbanana_transform = glm::rotate(giantbanana_transform, m_giantbanana->rotation_amount(), m_giantbanana->rotation_axis());
		giantbanana_transform = glm::scale(giantbanana_transform, m_giantbanana->scale());
		engine::renderer::submit(mesh_shader, giantbanana_transform, m_giantbanana);
		
	}
	else
	{
		//check if player has eaten giant banana
		if (m_giantbanana->is_giantbanana_eaten() == false) {
			m_player.eatgiantbanana();
			m_giantbanana->set_giantbanana_eaten(true);
			m_game_won = true;
			//create win screen overlay
			m_damagefade = damagefade::create("assets/textures/win.bmp", 2.0f, 1.6f, 0.9f);
		
		}
	}

	//render powerup
	if (m_powerup->active())
	{
		//render powerup
		glm::mat4 powerup_transform(1.0f);
		powerup_transform = glm::translate(powerup_transform, m_powerup->position());
		powerup_transform = glm::rotate(powerup_transform, m_powerup->rotation_amount(), m_powerup->rotation_axis());
		powerup_transform = glm::scale(powerup_transform, m_powerup->scale());
		engine::renderer::submit(mesh_shader, powerup_transform, m_powerup);
	}
	else
	{
		//apply the powerups effects
		if (m_powerup->is_powerup_eaten() == false) {
			m_player.eatbanana();
			m_powerup->set_powerup_eaten(true);
			m_player.set_speed_multiplier(0.2f);
			m_audio_manager->play_spatialised_sound("eat", m_3d_camera.position(), m_monkey->position());
		}
	}

	////////////////////////////////////////
	//display hitbox outlines(for debugging)
	////////////////////////////////////////
	
	/*m_angry_monkey_box.on_render(2.5f, 0.f, 0.f, mesh_shader);
	m_angry_monkey_box2.on_render(2.5f, 0.f, 0.f, mesh_shader);
	m_angry_monkey_box3.on_render(2.5f, 0.f, 0.f, mesh_shader);
	m_angry_monkey_box4.on_render(2.5f, 0.f, 0.f, mesh_shader);
	m_angry_monkey_box5.on_render(2.5f, 0.f, 0.f, mesh_shader);*/
	//m_banana_ball_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	//m_moving_platform_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	//m_moving_platform_box2.on_render(0.f, 0.f, 2.5f, mesh_shader);
	//m_moving_platform2_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	//m_moving_platform2_box2.on_render(0.f, 0.f, 2.5f, mesh_shader);
	//m_tower_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	//m_tower_box2.on_render(0.f, 0.f, 2.5f, mesh_shader);*/
	//m_dirt_platform_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	//m_dirt_platform_box2.on_render(0.f, 0.f, 2.5f, mesh_shader);
	/*m_crate_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	m_crate_box2.on_render(0.f, 0.f, 2.5f, mesh_shader);*/
	//m_player_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);
	//m_player_feet_box.on_render(2.5f, 2.5f, 2.5f, mesh_shader);
	//m_castlewall1.on_render(5.f, 2.f, 10.f, mesh_shader);
	//m_castlewall2.on_render(0.f, 2.5f, 0.f, mesh_shader);
	//m_castlewall3.on_render(0.f, 0.f, 2.5f, mesh_shader);
	//m_castlewall4.on_render(2.5f, 2.5f, 2.5f, mesh_shader);
	//m_camera_box.on_render(2.5f, 0.f, 2.5f, mesh_shader);*/

	/////////////////
	/// Mesh Shaders
	////////////////

	m_projectile.on_render(mesh_shader);
	m_projectile2.on_render(mesh_shader);
	m_projectile3.on_render(mesh_shader);
	m_projectile4.on_render(mesh_shader);
	m_projectile5.on_render(mesh_shader);
	m_monkey_material->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, m_player.object());

	m_menu->on_render(mesh_shader);
	m_deathscreen->on_render(mesh_shader);
    engine::renderer::end_scene();

	engine::renderer::begin_scene(m_2d_camera, mesh_shader);
	m_damagefade->on_render(mesh_shader, m_player.object()->position(), m_player.object()->forward(), m_player.object()->rotation_amount(), m_player.object()->rotation_axis());
	m_hpfade->on_render(mesh_shader, m_player.object()->position(), m_player.object()->forward(), m_player.object()->rotation_amount(), m_player.object()->rotation_axis());
	m_hudfade->on_render(mesh_shader, m_player.object()->position(), m_player.object()->forward(), m_player.object()->rotation_amount(), m_player.object()->rotation_axis());
	engine::renderer::end_scene();


	//////////////
	// Render text
	//////////////

	if (m_player.get_isdead() == false && m_game_won == false && startofgame==false)
	{
		std::string s;

		if (m_player.get_hp() > 0 && gametime < m_maxtime) {
			s = std::to_string(m_player.get_hp());


			const auto text_shader2 = engine::renderer::shaders_library()->get("text_2D");
			m_text_manager->render_text(text_shader2, std::to_string((int)(m_maxtime - gametime)), 135.f, (float)engine::application::window().height() - 20.f, 0.5f, glm::vec4(0.f, 1.0f, 0.f, 1.f));
		}
		
		
	}
} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }
		if (e.key_code() == engine::key_codes::KEY_1)
		{
			//initiate easymode
			startofgame = false;
			m_player.set_hp(150);
		
		}
		if (e.key_code() == engine::key_codes::KEY_2)
		{
			//initate hard mode
			startofgame = false;
			m_player.set_hp(50);
			m_maxtime = 100;
			m_audio_manager->play_spatialised_sound("monkey2", m_3d_camera.position(), m_monkey->position());
		}

		//cheats if needed during demonstration
		if (e.key_code() == engine::key_codes::KEY_3)
		{
			
			m_monkey->set_position(glm::vec3(-2.f, 3.3f, 7.0f));
		}
		if (e.key_code() == engine::key_codes::KEY_4)
		{

			m_monkey->set_position(glm::vec3(-2.f, 5.3f, 6.1f));
		}
		
	
    } 
}


