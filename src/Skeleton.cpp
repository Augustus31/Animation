#include "core.h"
#include <vector>
#include "DOF.h"
#include "Joint.h"
#include <Tokenizer.h>
#include "Skeleton.h"

void Skeleton::Load(const char* filename) {
	Tokenizer token;
	token.Open(filename);
	token.FindToken("balljoint");
	// Parse tree
	root = new Joint(nullptr);
	root->Load(token);
	// Finish
	token.Close();

}

void Skeleton::Update() {
	root->Update();
}

void Skeleton::Draw() {
	root->Draw();
}

Skeleton::~Skeleton(){
	delete root;
}