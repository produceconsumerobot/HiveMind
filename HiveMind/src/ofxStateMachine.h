//
//  ofxStateMachine.h
//
//  Simple openFrameworks state machine addon
//
//  Created by Sean Montgomery on 10/10/17.
//
//  This work is licensed under the MIT License
//


#pragma once
#include "ofxTimer.h"

class StateMachine
{
public:
	// ToDo: make vars private and use _T * var = addVar(Type _T);
	vector<ofxTimerMillis> timers;
	vector<int> ints;
	vector<bool> bools;
	vector<float> floats;

	int state;
	bool on;
	bool loop;

	StateMachine(int _numStates = 10)
	{
		setup(_numStates);
	}

	void next()
	{
		if (loop)
			state = (state + 1) % numStates;
		else
			state++;
	}
private:
	void setup(int _numStates)
	{
		int nVars = 10;
		timers.resize(nVars);
		ints.resize(nVars);
		bools.resize(nVars);
		floats.resize(nVars);
		state = 0;
		on = true;
		loop = false;
		numStates = _numStates;
	}

	int numStates;
};