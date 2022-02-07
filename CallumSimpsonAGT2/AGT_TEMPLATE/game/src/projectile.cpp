#include "pch.h"
#include "projectile.h"

projectile::projectile()
{}

projectile::~projectile()
{}

void projectile::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_rotational_inertia = (2.f / 3.f) * object->mass() * (m_object->bounding_shape().y / 2.f) * (m_object->bounding_shape().y / 2.f);
	m_contact_time = 0.05f;
}

//throw the ball from the AI's front vector towards the player
void projectile::throw_ball(glm::vec3 front_vector, float throw_ball, glm::vec3 position_vector)
{
	//set the balls properties
	m_object->set_velocity(glm::vec3(0.f));
	m_object->set_acceleration(glm::vec3(0.f, 0.f, 0.f));
	m_object->set_torque(glm::vec3(0.f));
	m_object->set_rotation_amount(0.f);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_angular_velocity(glm::vec3(1.f));
	m_contact_time = 0.0f;

	// Set the ball to the current camera position
	m_object->set_position(position_vector);
	m_object->set_acceleration(3.0f * throw_ball * front_vector);

	// Determine rotation angles of camera (from Lab 4)
	m_theta = engine::PI / 2.f - acos(front_vector.y);
	m_phi = atan2(front_vector.x, front_vector.z);
}

void projectile::on_update(const engine::timestep& time_step)
{
	// Update physical quanitities
	m_last_position = m_object->position();
}

void projectile::on_render(const engine::ref<engine::shader>& shader)
{
	//render the ball
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_object->position());
	//transform = glm::rotate(transform, -m_theta, glm::vec3(1.f, 0.f, 0.f));
	//transform = glm::rotate(transform, m_phi, glm::vec3(0.f, 1.f, 0.f));
	transform = glm::rotate(transform, m_object->rotation_amount(), m_object->rotation_axis());
	engine::renderer::submit(shader, transform, m_object);
}

bool projectile::collision_detection(float y_plane)
{
	// Check for collision with the ground by looking at the y value of the ball's position
	if (m_object->position().y - m_object->bounding_shape().y < y_plane && m_object->velocity().y < 0) {
		return true;
	}
	return false;
}

void projectile::collision_response(float y_plane)
{
	float convergenceThreshold = 0.5f;
	if (glm::length(m_object->velocity()) > convergenceThreshold) {
		// The ball has bounced!  Implement a bounce by flipping the y velocity
		m_object->set_velocity(glm::vec3(m_object->velocity().x, -m_object->velocity().y, m_object->velocity().z));
	}
	else {
		// Velocity of the ball is below a threshold.  Stop the ball. 
		m_object->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
		m_object->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
		m_object->set_position(glm::vec3(m_object->position().x, m_object->bounding_shape().y + y_plane, m_object->position().z));
		m_object->set_angular_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}
