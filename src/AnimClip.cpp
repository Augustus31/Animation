#include "AnimClip.h"
#include <Tokenizer.h>
#include <string.h>
#include <Channel.h>
#include <iostream>
#include <Skeleton.h>

bool AnimClip::Load(const char* filename) {
	Tokenizer token;
	token.Open(filename);

	token.FindToken("animation");
	token.FindToken("{");
	token.FindToken("range");
	float start = token.GetFloat();
	float end = token.GetFloat();
	this->start = start;
	this->end = end;

	token.FindToken("numchannels");
	int numchannels = token.GetInt();
	for (int i = 0; i < numchannels; i++) {
		token.FindToken("channel");
		token.FindToken("{");

		Channel* channel = new Channel();

		token.FindToken("extrapolate");
		for (int j = 0; j < 2; j++) {
			char ext[256];
			token.GetToken(ext);
			if (strcmp(ext, "constant") == 0) {
				channel->extrapolate[j] = 'c';
			}
			else if (strcmp(ext, "linear") == 0) {
				channel->extrapolate[j] = 'l';
			}
			else if (strcmp(ext, "cycle") == 0) {
				channel->extrapolate[j] = 'y';
			}
			else if (strcmp(ext, "cycle_offset") == 0) {
				channel->extrapolate[j] = 'o';
			}
			else if (strcmp(ext, "bounce") == 0) {
				channel->extrapolate[j] = 'b';
			}
		}

		token.FindToken("keys");
		int numKeys = token.GetInt();
		token.FindToken("{");

		for (int k = 0; k < numKeys; k++) {
			float time = token.GetFloat();
			float value = token.GetFloat();
			char codes[2];
			float tangents[2]{ 100001, 100001 };
			for (int j = 0; j < 2; j++) {
				char code[256];
				token.GetToken(code);
				if (strcmp(code, "flat") == 0) {
					codes[j] = 'f';
				}
				else if (strcmp(code, "linear") == 0) {
					codes[j] = 'l';
				}
				else if (strcmp(code, "smooth") == 0) {
					codes[j] = 's';
				}
				else {
					codes[j] = 'x';
					tangents[j] = atof(code);
				}
			}

			Keyframe frame = Keyframe(time, value, codes[0], codes[1]);
			for (int tc = 0; tc < 2; tc++) {
				if (abs(tangents[tc] - 100001) > 0.1) {
					if (tc == 0) {
						frame.SetTanIn(tangents[tc]);
					}
					else {
						frame.SetTanOut(tangents[tc]);
					}
				}
			}
			channel->keys.push_back(frame);
			
		}
		channels.push_back(channel);

		

	}

	//DEBUG

	std::cout << "Channels and Keyframes: \n" << std::endl;

	for(int i = 0; i < channels.size(); i++) {
		std::cout << channels[i]->extrapolate[0] << " " << channels[i]->extrapolate[1] << " " << std::endl;
		for (int j = 0; j < channels[i]->keys.size(); j++) {
			channels[i]->keys[j].Debug();
		}
		std::cout << "\n---------------------------------\n" << std::endl;
	}

	token.Close();
	return true;
}

void AnimClip::Evaluate(float time) {
	glm::vec3 nOffset;
	for (int i = 0; i < 3; i++) {
		float offval = channels[i]->Evaluate(time);
		nOffset[i] = offval;
	}
	Skeleton::joints[0]->SetOffset(nOffset);

	for (int i = 3; i < channels.size(); i++) {
		float dofval = channels[i]->Evaluate(time);
		Skeleton::DOFs[i-3]->SetValue(dofval);
	}
}

void AnimClip::PrecomputeChannels() {
	for (int i = 0; i < channels.size(); i++) {
		std::cout << "precomputing channel " << i << std::endl;
		channels[i]->Precompute();
	}
}

void AnimClip::Debug() {
	//to be called after Load() and PrecomputeChannels()
	for (int i = 0; i < channels[0]->keys.size(); i++) {
		std::cout << "Hermite Coefficients: " << channels[0]->keys[i].a << " " << channels[0]->keys[i].b << " " << channels[0]->keys[i].c << " " << channels[0]->keys[i].d << "\n" << std::endl;
	}
	
	std::cout << "--------------------" << std::endl;

	float test1 = channels[65]->Evaluate(2);
	float test2 = channels[65]->Evaluate(2.5);
	float test3 = channels[65]->Evaluate(3);
	float test4 = channels[65]->Evaluate(3.5);

	/*float tI1 = channels[0]->keys[1].tanIn;
	float tO1 = channels[0]->keys[1].tanOut;
	float tI0 = channels[0]->keys[0].tanIn;
	float tO0 = channels[0]->keys[0].tanOut;
	float tI2 = channels[0]->keys[2].tanIn;
	float tO2 = channels[0]->keys[2].tanOut;*/

	std::cout << test1 << " " << test2 << " " << test3 << " " << test4 << std::endl;
	//std::cout << tI0 << " " << tO0 << " " << tI1 << " " << tO1 << " " << tI2 << " " << tO2 << std::endl;
}