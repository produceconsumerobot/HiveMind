//
//  ofxTimerMillis.h
//
//  Simple openFrameworks addOn to make tracking time a little less cumbersome
//
//  Created by Sean Montgomery on 10/10/17.
//
//  This work is licensed under the MIT License
//

#pragma once
#include "ofMain.h"

class ofxTimerMillis
{
public:
	ofxTimerMillis();
	ofxTimerMillis(uint64_t milliseconds);
	void set(uint64_t milliseconds);
	void start();
	bool isElapsed();
	bool isElapsed(uint64_t milliseconds);
	uint64_t elapsedTime();
private:
	uint64_t mStartTime;
	uint64_t mSetTime;
};

class ofxTimerMicros
{
public:
	ofxTimerMicros();
	ofxTimerMicros(uint64_t microseconds);
	void set(uint64_t microseconds);
	void start();
	bool isElapsed();
	bool isElapsed(uint64_t microseconds);
	uint64_t elapsedTime();
private:
	uint64_t mStartTime;
	uint64_t mSetTime;
};