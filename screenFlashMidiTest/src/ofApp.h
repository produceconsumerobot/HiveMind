#pragma once

#include "ofMain.h"
#include "ofxMidi.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void exit();
		
		int targetFrameRate = 60;

		uint64_t lastDrawMicros;
		uint64_t lastDrawMicros1;
		uint64_t lastDrawMicros2;
		uint64_t printFrameRateMillis;
		float drawDelay;
		float drawDelay1;
		float drawDelay2;

		bool draw1;
		int frameCount1;
		int targetFrameCount1;
		bool drawWhiteOn1;

		bool draw2;
		int frameCount2;
		int targetFrameCount2;
		bool drawWhiteOn2;

		bool printRates;

		// MIDI variables
		ofxMidiOut midiout;
		bool sendMidi;
		int midiChannel;
		int midiId0;
		int midiId1;
		int midiValue;
};
