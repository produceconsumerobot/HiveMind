#pragma once

#include "ofMain.h"
#include "ofxOpenBciWifi.h"
#include "ofxOscilloscope.h"

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
		//void openBciWifiTcp(string computerIp, int tcpPort, string openBciWifiIp);
		//void openBciWifiStart(string openBciWifiIp);
		//void openBciWifiStop(string openBciWifiIp);
		//void openBciWifiSquareWaveOn(string openBciWifiIp);
		//void openBciWifiAnalogDataOn(string openBciWifiIp);


		ofxOpenBciWifi openBci;

		int nHeadsets;
		int nChan;
		vector<ofxMultiScope> scopeWins;
		vector<ofxMultiScope> scopeFftWins;
		vector<string> stringData;

		int nFftBins;

		vector<uint64_t> lastFftMillis;
		vector<float> fftDelay;

		float xGap;
		float yTop;

		bool isPaused;
		int selectedScope;

		bool debugLoggingEnabled;
		LoggerThread debugLogger;

		int tcpPort;
		string computerIp;
		vector<string> openBciIps;
};
