#pragma once

#include "ofMain.h"
#include "HiveMind.h"
#include "ofxMidi.h"
#include "ofxStateMachine.h"

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

		HiveMind hiveMind;
		int nHeadsets;

		int targetFrameRate = 60;

		uint64_t lastFrameMicros;
		uint64_t printFrameRateMillis;
		float frameDelay;

		vector<uint64_t> lastDrawMicros;
		vector<float> drawDelay;
		vector<uint64_t> changeFrameCountMicros;
		vector<uint64_t> changeFrameCountInterval;
		uint64_t changeFrameCountMin;
		uint64_t changeFrameCountMax;

		vector<bool> drawOn;
		vector<int> frameCount;
		vector<int> targetFrameCount;
		vector<bool> drawWhiteOn;

		//vector<ofRectangle> drawRect;

		bool printRates;

		// MIDI variables
		ofxMidiOut midiout;
		bool sendMidi;
		int midiChannel;
		vector<int> midiId;
		int midiValue;

		StateMachine states;
		ofImage titleScreen;
		ofImage riskScreen;
		ofImage descriptionScreen;
		ofImage creditsScreen;

		vector<ofxTimerMillis> flashTimers;

		int flashState0;

		string computerIp;
		vector<string> openBciIps;
		int tcpPort;

		float reTcpDelay;

		ofxTimerMillis resetBandDataTimer;
};
