#pragma once

class Keyframe {
public:
	Keyframe(float time, float value, char ruleIn, char ruleOut);
	void SetTanIn(float t);
	void SetTanOut(float t);
	void Debug();

	float time;
	float value;
	float tanIn;
	float tanOut;
	char ruleIn;
	char ruleOut;
	float a, b, c, d;
};