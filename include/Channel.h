#pragma once

#include <Tokenizer.h>
#include <core.h>
#include <vector>
#include <Keyframe.h>

class Channel {
public:
	float Evaluate(float time);
	float EvaluateSpan(float time);
	int SearchSpan(float time);
	void Precompute();
	std::vector<Keyframe> keys;
	char extrapolate[2];

};