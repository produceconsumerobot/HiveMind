//
//  ofxTimerMillis.cpp
//
//  Simple openFrameworks addOn to make tracking time a little less cumbersome
//
//  Created by Sean Montgomery on 10/10/17.
//
//  This work is licensed under the MIT License
//

#pragma once
#include "ofxTimer.h"

ofxTimerMillis::ofxTimerMillis()
{
	mStartTime = ofGetElapsedTimeMillis();
}

ofxTimerMillis::ofxTimerMillis(uint64_t milliseconds)
{
	set(milliseconds);
	mStartTime = ofGetElapsedTimeMillis();
}

void ofxTimerMillis::set(uint64_t milliseconds)
{
	mSetTime = milliseconds;
}

void ofxTimerMillis::start()
{
	isStarted = true;
	mStartTime = ofGetElapsedTimeMillis();
}

void ofxTimerMillis::stop()
{
	isStarted = false;
}

bool ofxTimerMillis::isElapsed()
{
	return isElapsed(mSetTime);
}

bool ofxTimerMillis::isElapsed(uint64_t milliseconds)
{
	if (isStarted && elapsedTime() > mSetTime)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint64_t ofxTimerMillis::elapsedTime()
{
	return ofGetElapsedTimeMillis() - mStartTime;
}


// ******************************************************* //

ofxTimerMicros::ofxTimerMicros()
{
	mStartTime = ofGetElapsedTimeMicros();
}

ofxTimerMicros::ofxTimerMicros(uint64_t microseconds)
{
	set(microseconds);
	mStartTime = ofGetElapsedTimeMicros();
}

void ofxTimerMicros::set(uint64_t microseconds)
{
	mSetTime = microseconds;
}

void ofxTimerMicros::start()
{
	isStarted = true;
	mStartTime = ofGetElapsedTimeMicros();
}

void ofxTimerMicros::stop()
{
	isStarted = false;
}

bool ofxTimerMicros::isElapsed()
{
	return isElapsed(mSetTime);
}

bool ofxTimerMicros::isElapsed(uint64_t microseconds)
{
	if (isStarted && elapsedTime() > mSetTime)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint64_t ofxTimerMicros::elapsedTime()
{
	return ofGetElapsedTimeMicros() - mStartTime;
}

