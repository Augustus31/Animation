#pragma once

#include "core.h"
#include <vector>
#include "DOF.h"
#include "Tokenizer.h"

class Joint {
private:
	glm::mat4 localmat;
	glm::mat4 worldmat;
	std::vector<DOF> dofs;
	std::vector<Joint*> children;
	glm::vec3 offset;
	glm::vec3 boxmin;
	glm::vec3 boxmax;
	Joint* parente;

public:
	Joint(Joint* parente);
	void Update();
	void Load(Tokenizer &t);
	void Draw();
	void AddChild(Joint* child);
	glm::mat4 GetWorldMatrix();
	~Joint();
};