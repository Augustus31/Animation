#include "Joint.h"
#include "Tokenizer.h"
#include <iostream>
#include <Cube.h>
#include <Window.h>
#include <Skeleton.h>

Joint::Joint(Joint* parente) {
	this->parente = parente;
	dofs.push_back(DOF());
	dofs.push_back(DOF());
	dofs.push_back(DOF());

	offset = glm::vec3(0.0, 0.0, 0.0);
	boxmin = glm::vec3(-0.1, -0.1, -0.1);
	boxmax = glm::vec3(0.1, 0.1, 0.1);
}

void Joint::Load(Tokenizer& t) {
	std::cout << "new call to Joint::Load" << std::endl;
	Skeleton::joints.push_back(this);
	std::unordered_map<int, char> dofmap({ {0, 'x'}, {1, 'y'}, {2, 'z'} });
	char name[256];
	t.GetToken(name);
	std::string strname = name;
	for (int i = 0; i < 3; i++) {
		dofs[i].SetName(strname + " " + dofmap[i]);
		Skeleton::DOFs.push_back(&(dofs[i]));
	}
	t.FindToken("{");
	while (1) {
		char temp[256];
		t.GetToken(temp);
		if (strcmp(temp, "offset") == 0) {
			offset.x = t.GetFloat();
			offset.y = t.GetFloat();
			offset.z = t.GetFloat();
			std::cout << "Offset: " << offset.x << " " << offset.y << " " << offset.z << std::endl;
		}
		else if (strcmp(temp, "boxmin") == 0) {
			boxmin.x = t.GetFloat();
			boxmin.y = t.GetFloat();
			boxmin.z = t.GetFloat();
			std::cout << "Boxmin: " << boxmin.x << " " << boxmin.y << " " << boxmin.z << std::endl;
		}
		else if (strcmp(temp, "boxmax") == 0) {
			boxmax.x = t.GetFloat();
			boxmax.y = t.GetFloat();
			boxmax.z = t.GetFloat();
			std::cout << "Boxmax: " << boxmax.x << " " << boxmax.y << " " << boxmax.z << std::endl;
		}
		else if (strcmp(temp, "rotxlimit") == 0) {
			float nmin = t.GetFloat();
			float nmax = t.GetFloat();
			dofs[0].SetMinMax(nmin, nmax);
			std::cout << "Min, Max: " << dofs[0].GetMin() << " " << dofs[0].GetMax() << std::endl;
		}
		else if (strcmp(temp, "rotylimit") == 0) {
			float nmin = t.GetFloat();
			float nmax = t.GetFloat();
			dofs[1].SetMinMax(nmin, nmax);
			std::cout << "Min, Max: " << dofs[1].GetMin() << " " << dofs[1].GetMax() << std::endl;
		}
		else if (strcmp(temp, "rotzlimit") == 0) {
			float nmin = t.GetFloat();
			float nmax = t.GetFloat();
			dofs[2].SetMinMax(nmin, nmax);
			std::cout << "Min, Max: " << dofs[2].GetMin() << " " << dofs[2].GetMax() << std::endl;
		}
		else if (strcmp(temp, "pose") == 0) {
			dofs[0].SetValue(t.GetFloat());
			dofs[1].SetValue(t.GetFloat());
			dofs[2].SetValue(t.GetFloat());

			std::cout << "Values: " << dofs[0].GetValue() << " " << dofs[1].GetValue() << " " << dofs[2].GetValue() << std::endl;
		}
		else if (strcmp(temp, "balljoint") == 0) {
			Joint* child = new Joint(this);
			child->Load(t);
			AddChild(child);
		}
		else if (strcmp(temp, "}") == 0) {
			std::cout << "returning" << std::endl;
			return;
		}
		else t.SkipLine(); // Unrecognized token
	}
}

void Joint::Update() {

	//set z rotation matrix
	glm::mat4 zrot(1.0);
	zrot[0][0] = cos(dofs[2].GetValue());
	zrot[0][1] = sin(dofs[2].GetValue());
	zrot[1][0] = -sin(dofs[2].GetValue());
	zrot[1][1] = cos(dofs[2].GetValue());

	//set y rotation matrix
	glm::mat4 yrot(1.0);
	yrot[0][0] = cos(dofs[1].GetValue());
	yrot[0][2] = -sin(dofs[1].GetValue());
	yrot[2][0] = sin(dofs[1].GetValue());
	yrot[2][2] = cos(dofs[1].GetValue());

	//set x rotation matrix
	glm::mat4 xrot(1.0);
	xrot[1][1] = cos(dofs[0].GetValue());
	xrot[1][2] = sin(dofs[0].GetValue());
	xrot[2][1] = -sin(dofs[0].GetValue());
	xrot[2][2] = cos(dofs[0].GetValue());

	localmat = zrot * yrot * xrot;
	localmat[3] = glm::vec4(offset, 1.0);

	if (parente == nullptr) {
		worldmat = localmat;
	}
	else {
		worldmat = parente->GetWorldMatrix() * localmat;
	}
	for (int i = 0; i < children.size(); i++) {
		children[i]->Update();
	}
}

void Joint::Draw() {
	Cube* bone = new Cube(boxmin, boxmax);
	bone->ObjToWorld(&worldmat);
	Window::cubes.push_back(bone);
	for (int i = 0; i < children.size(); i++) {
		children[i]->Draw();
	}
}

void Joint::AddChild(Joint* child) {
	children.push_back(child);
}

glm::mat4 Joint::GetWorldMatrix(){
	return worldmat;
}

void Joint::Precompute(glm::mat4 bind) {
	precomputemat = worldmat * glm::inverse(bind);
	precomputematnormal = glm::transpose(glm::inverse(worldmat * glm::inverse(bind)));
}

glm::mat4 Joint::GetPrecomputedMatrix() {
	return precomputemat;
}

glm::mat4 Joint::GetPrecomputedMatrixNormal() {
	return precomputematnormal;
}

void Joint::SetOffset(glm::vec3 off) {
	offset = off;
}

Joint::~Joint() {
	for (int i = 0; i < children.size(); i++) {
		delete children[i];
	}
}