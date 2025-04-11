#pragma once

#include "Keyframe.h"
#include <iostream>

Keyframe::Keyframe(float time, float value, char ruleIn, char ruleOut) {
	this->time = time;
	this->value = value;
	this->ruleIn = ruleIn;
	this->ruleOut = ruleOut;
	this->tanIn = 984399;
	this->tanOut = 984399; //filler value to indicate uninitialized tangent
}
void Keyframe::SetTanIn(float t) {
	this->tanIn = t;
}
void Keyframe::SetTanOut(float t) {
	this->tanOut = t;
}

void Keyframe::Debug() {
	std::cout << time << " " << value << " " << ruleIn << " " << ruleOut << std::endl;
}