#pragma once

#include "core.h"

class DOF {
private:
	float value;
	float min;
	float max;

public:
	DOF();
	void SetValue(float newValue);
	float GetValue();
	void SetMinMax(float newMin, float newMax);
	float GetMin();
	float GetMax();
};