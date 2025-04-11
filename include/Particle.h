#include "core.h"

class Particle {
public:
	void ApplyForce(glm::vec3& f);
	void Integrate(float deltaTime);
private:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;
	float mass;
};
