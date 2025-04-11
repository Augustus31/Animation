#include <Keyframe.h>
#include <Channel.h>
#include <iostream>

void Channel::Precompute() {
	if (keys.size() == 1) {
		if (extrapolate[0] != 'l') {
			extrapolate[0] = 'c';
		}
		if (extrapolate[1] != 'l') {
			extrapolate[1] = 'c';
		}

		if (keys[0].ruleIn == 'f') {
			keys[0].tanIn = 0;
		}
		else if (keys[0].ruleIn == 'x') {
			//do nothing, tangent already set
		}
		else {
			keys[0].tanIn = 0;
		}

		if (keys[0].ruleOut == 'f') {
			keys[0].tanOut = 0;
		}
		else if (keys[0].ruleOut == 'x') {
			//do nothing, tangent already set
		}
		else {
			keys[0].tanOut = 0;
		}

	}
	else {
		for (int i = 0; i < keys.size(); i++) {
			if (keys[i].ruleIn == 'f') {
				keys[i].tanIn = 0;
			}
			else if (keys[i].ruleIn == 'l') {
				if (i != 0) {
					keys[i].tanIn = (keys[i].value - keys[i - 1].value) / (keys[i].time - keys[i - 1].time);
				}
			}
			else if (keys[i].ruleIn == 's') {
				if (i != 0 && i != keys.size() - 1) {
					keys[i].tanIn = (keys[i + 1].value - keys[i - 1].value) / (keys[i + 1].time - keys[i - 1].time);
				}
				else if (i == keys.size() - 1) {
					keys[i].tanIn = (keys[i].value - keys[i - 1].value) / (keys[i].time - keys[i - 1].time);
				}
			}

			if (keys[i].ruleOut == 'f') {
				keys[i].tanOut = 0;
			}
			else if (keys[i].ruleOut == 'l') {
				if (i != keys.size() - 1) {
					keys[i].tanOut = (keys[i + 1].value - keys[i].value) / (keys[i + 1].time - keys[i].time);
				}
			}
			else if (keys[i].ruleOut == 's') {
				if (i != 0 && i != keys.size() - 1) {
					keys[i].tanOut = (keys[i + 1].value - keys[i - 1].value) / (keys[i + 1].time - keys[i - 1].time);
				}
				else if (i == 0) {
					keys[i].tanOut = (keys[i + 1].value - keys[i].value) / (keys[i + 1].time - keys[i].time);
				}
			}


		}

		if (keys[0].tanIn == 984399) {
			keys[0].tanIn = keys[0].tanOut;
		}

		if (keys[keys.size() - 1].tanOut == 984399) {
			keys[keys.size() - 1].tanOut = keys[keys.size() - 1].tanIn;
		}
	}


	glm::mat4 hermite = { 2,-3,0,1,-2,3,0,0,1,-2,1,0,1,-1,0,0 };

	for (int i = 0; i < keys.size() - 1; i++) {
		float scale = keys[i + 1].time - keys[i].time;
		glm::vec4 mvec = { keys[i].value, keys[i + 1].value, keys[i].tanOut * scale, keys[i + 1].tanIn * scale };
		glm::vec4 params = hermite * mvec;
		keys[i].a = params[0];
		keys[i].b = params[1];
		keys[i].c = params[2];
		keys[i].d = params[3];
	}
}

float Channel::Evaluate(float time) {
	if (time >= keys[0].time && time <= keys[keys.size() - 1].time) {
		return EvaluateSpan(time);
	}
	
	//Extrapolation
	float t0 = keys[0].time;
	float t1 = keys[keys.size() - 1].time;
	float p0 = keys[0].value;
	float p1 = keys[keys.size() - 1].value;
	float nTime = 0;

	//extrap in
	if (time < keys[0].time) {
		if (extrapolate[0] == 'c') {
			return p0;
		}
		else if (extrapolate[0] == 'l') {
			return p0 - keys[0].tanIn * (t0 - time);
		}
		else if (extrapolate[0] == 'y') {
			float mult = (time - t0) / (t1 - t0);
			nTime = (mult - floor(mult)) * (t1 - t0) + t0;
			return EvaluateSpan(nTime);
		}
		else if (extrapolate[0] == 'o') {
			float mult = (time - t0) / (t1 - t0);
			nTime = (mult - floor(mult)) * (t1 - t0) + t0;
			return EvaluateSpan(nTime) + (p1 - p0) * floor(mult);
		}
		else if (extrapolate[0] == 'b') {
			float mult = (time - t0) / (t1 - t0);
			nTime = (mult - floor(mult)) * (t1 - t0) + t0;
			if ((int)floor(mult) % 2 == 1) {
				nTime = ((int)mult - mult) * (t1 - t0) + t0;
			}
			return EvaluateSpan(nTime);
		}
		else {
			std::cout << "ERROR: unrecognized extrapolate mode" << std::endl;
		}
	}
	//extrap out
	else {
		if (extrapolate[1] == 'c') {
			return p1;
		}
		else if (extrapolate[1] == 'l') {
			return p1 + keys[keys.size() - 1].tanOut * (time - t1);
		}
		else if (extrapolate[1] == 'y') {
			float mult = (time - t1) / (t1 - t0);
			nTime = (mult - floor(mult)) * (t1 - t0) + t0;
			return EvaluateSpan(nTime);
		}
		else if (extrapolate[1] == 'o') {
			float mult = (time - t1) / (t1 - t0);
			nTime = (mult - floor(mult)) * (t1 - t0) + t0;
			return EvaluateSpan(nTime) + (p1 - p0) * (floor(mult) + 1);
		}
		else if (extrapolate[1] == 'b') {
			float mult = (time - t1) / (t1 - t0);
			nTime = (mult - floor(mult)) * (t1 - t0) + t0;
			if ((int)floor(mult) % 2 == 1) {
				nTime = (ceil(mult) - mult) * (t1 - t0) + t0;
			}
			return EvaluateSpan(nTime);
		}
		else {
			std::cout << "ERROR: unrecognized extrapolate mode" << std::endl;
		}
	}
}

float Channel::EvaluateSpan(float time) { //time value is between first and last keyframe
	int keyIndex = SearchSpan(time);
	if (time == keys[keyIndex].time) {
		return keys[keyIndex].value;
	}
	else {
		float input = (time - keys[keyIndex].time) / (keys[keyIndex + 1].time - keys[keyIndex].time);
		return keys[keyIndex].d + input * (keys[keyIndex].c + input * (keys[keyIndex].b + input * keys[keyIndex].a));
	}
}

int Channel::SearchSpan(float time) {
	//replace this with binary search at later date
	for (int i = 0; i < keys.size() - 1; i++) {
		if (keys[i].time <= time && keys[i + 1].time > time) {
			return i;
		}
	}

	if (time == keys[keys.size() - 1].time) {
		return keys.size() - 1;
	}
}