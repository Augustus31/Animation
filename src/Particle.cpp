#include "Particle.h"

void Particle::ApplyForce(glm::vec3& f) { force += f; }

void Particle::Integrate(float deltaTime) {
	glm::vec3 accel = force / mass; // Apply Newton’s Second Law (f=ma)
	velocity += accel * deltaTime; // Forward Euler integration to get new velocity
	position += velocity * deltaTime; // Backward Euler integration to get new position
	force = glm::vec3(0); // Zero force out so next frame will start fresh
}