#include "DOF.h"

DOF::DOF() {
	min = -INFINITY;
	max = INFINITY;
	value = 0;
}

void DOF::SetValue(float newValue) {
	value = newValue;
	if (value < min) {
		value = min;
	}

	if (value > max) {
		value = max;
	}
}

float DOF::GetValue() {
	return value;
}

void DOF::SetMinMax(float newMin, float newMax) {
	if (newMin <= newMax) {
		min = newMin;
		max = newMax;

		if (value < min) {
			value = min;
		}

		if (value > max) {
			value = max;
		}
	}
}

float DOF::GetMin() {
	return min;
}

float DOF::GetMax() {
	return max;
}