#pragma once

#include <Channel.h>

class AnimClip {
public:
	void Evaluate(float time); //static Skeleton::DOFs has pose 
	bool Load(const char* filename);
	void PrecomputeChannels();
	void Debug();
	std::vector<Channel*> channels;
private:
	float start;
	float end;
};