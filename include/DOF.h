#pragma once

#include "core.h"
#include <string>

class DOF {
private:
	float value;
	float min;
	float max;
	std::string name;

public:
	DOF();
	void SetValue(float newValue);
	float GetValue();
	void SetMinMax(float newMin, float newMax);
	float GetMin();
	float GetMax();
	std::string GetName();
	void SetName(std::string nomen);
};