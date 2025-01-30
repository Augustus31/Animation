#pragma once

#include "core.h"
#include <vector>
#include "DOF.h"
#include "Joint.h"
#include <Tokenizer.h>
#include <unordered_map>

class Skin {
private:
	std::vector<glm::mat4> bindings;
	std::vector<int> triangles;
	std::vector<std::unordered_map<int, float>> weights;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> adjPositions;
	std::vector<glm::vec3> adjNormals;
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;
	glm::mat4 model;
	glm::vec3 color;

public:
	void Load(const char* filename);
	void Update();
	void BeginDraw();
	void Draw(const glm::mat4& viewProjMtx, GLuint shader);
	~Skin();
};
