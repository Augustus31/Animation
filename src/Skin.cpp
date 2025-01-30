#include <Tokenizer.h>
#include <Skin.h>
#include <iostream>
#include <string>
#include <Skeleton.h>
#include <Window.h>

extern int mode;

void Skin::Load(const char* filename) {
	Tokenizer token;
	token.Open(filename);

	token.FindToken("positions");
	int posNum = token.GetInt();
	token.FindToken("{");
	positions = std::vector<glm::vec3>();
	for (int i = 0; i < posNum; i++) {
		glm::vec3 pos(0.0, 0.0, 0.0);
		pos.x = token.GetFloat();
		pos.y = token.GetFloat();
		pos.z = token.GetFloat();
		positions.push_back(pos);
	}

	token.FindToken("normals");
	int normNum = token.GetInt();
	token.FindToken("{");
	normals = std::vector<glm::vec3>();
	for (int i = 0; i < normNum; i++) {
		glm::vec3 norm(0.0, 0.0, 0.0);
		norm.x = token.GetFloat();
		norm.y = token.GetFloat();
		norm.z = token.GetFloat();
		normals.push_back(norm);
	}

	token.FindToken("skinweights");
	int weightsNum = token.GetInt();
	token.FindToken("{");
	weights = std::vector<std::unordered_map<int, float>>();
	for (int i = 0; i < weightsNum; i++) {
		int inNum = token.GetInt();
		std::unordered_map<int, float> mapa = std::unordered_map<int, float>();
		for (int i = 0; i < inNum; i++) {
			int joint = token.GetInt();
			float weight = token.GetFloat();
			mapa[joint] = weight;
		}
		weights.push_back(mapa);
	}

	token.FindToken("triangles");
	int trianglesNum = token.GetInt();
	token.FindToken("{");
	triangles = std::vector<int>();
	for (int i = 0; i < trianglesNum * 3; i++) {
		int index = token.GetInt();
		triangles.push_back(index);
	}

	token.FindToken("bindings");
	int jointsNum = token.GetInt();
	token.FindToken("{");
	bindings = std::vector<glm::mat4>();
	for (int i = 0; i < jointsNum; i++) {
		token.FindToken("{");
		glm::mat4 bind(1.0);
		for (int i = 0; i < 4; i++) {
			float val = token.GetFloat();
			bind[i][0] = val;

			val = token.GetFloat();
			bind[i][1] = val;

			val = token.GetFloat();
			bind[i][2] = val;
		}
		bindings.push_back(bind);
	}

	token.Close();

	//DEBUG

	/*std::cout << "Positions: \n" << std::endl;

	for(int i = 0; i < positions.size(); i++) {
		std::cout << positions[i].x << " " << positions[i].y << " " << positions[i].z << std::endl;
	}

	std::cout << "Normals: \n" << std::endl;

	for (int i = 0; i < normals.size(); i++) {
		std::cout << normals[i].x << " " << normals[i].y << " " << normals[i].z << std::endl;
	}

	std::cout << "Triangles: \n" << std::endl;

	for (int i = 0; i < triangles.size()/3; i++) {
		std::cout << triangles[i*3] << " " << triangles[i*3 + 1] << " " << triangles[i*3 + 2] << std::endl;
	}

	std::cout << "Weights: \n" << std::endl;

	for (int i = 0; i < weights.size(); i++) {
		for (auto j : weights[i]) {
			std::cout << j.first << " " << j.second << std::endl;
		}
		std::cout << "---------end of weight set------" << std::endl;
	}

	std::cout << "Binding Matrices: \n" << std::endl;

	for (int i = 0; i < bindings.size(); i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				std::cout << (bindings[i])[j][k];
			}
			std::cout << std::endl;
		}
		std::cout << "\n" << std::endl;
	}*/
}

void Skin::Update() {
	if (mode != 1) {
		for (int i = 0; i < Skeleton::joints.size(); i++) {
			Skeleton::joints[i]->Precompute(bindings[i]);
		}
		adjPositions.clear();
		adjNormals.clear();
		for (int i = 0; i < positions.size(); i++) {
			glm::vec4 runningSum(0.0, 0.0, 0.0, 0.0);
			glm::vec4 runningSumNormals(0.0, 0.0, 0.0, 0.0);
			for (auto w : weights[i]) {
				glm::vec4 homopos(positions[i].x, positions[i].y, positions[i].z, 1.0f);
				glm::vec4 homonorm(normals[i].x, normals[i].y, normals[i].z, 1.0f);
				runningSum += (w.second * Skeleton::joints[w.first]->GetPrecomputedMatrix()) * homopos;
				runningSumNormals += (w.second * Skeleton::joints[w.first]->GetPrecomputedMatrixNormal()) * homonorm;
			}
			adjPositions.push_back(glm::vec3(runningSum));
			adjNormals.push_back(glm::vec3(runningSumNormals));
		}
	}
	else {
		adjPositions.clear();
		adjNormals.clear();
		for (int i = 0; i < positions.size(); i++) {
			adjPositions.push_back(positions[i]);
			adjNormals.push_back(normals[i]);
		}
	}
	

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * adjPositions.size(), adjPositions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * adjNormals.size(), adjNormals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	model = glm::mat4(1.0);

	color = glm::vec3(0.5f, 0.5f, 0.5f);
}

void Skin::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
	// actiavte the shader program
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void Skin::BeginDraw() {
	Window::skin = this;
	Window::skinFlag = true;
}

Skin::~Skin() {
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}