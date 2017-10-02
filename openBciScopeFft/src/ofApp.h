#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxOscilloscope.h"
//#include "ofxHttpUtils.h"
#include "ofxJSON.h"
#include "ofxThreadedLogger.h"
#include "ofxFft.h"
#include "ofxBiquadFilter.h"

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
		void calcFft();
		void subtractDC(vector<float> & data, float & dc, float newDataWeight = 0.03f);
		float ofApp::smoother(float newData, float oldData, float newDataWeight);

		// void newResponse(ofxHttpResponse & response);

		ofxTCPServer TCP;

		ofTrueTypeFont  mono;
		ofTrueTypeFont  monosm;
		int port;

		//ofxHttpUtils httpUtils;
		ofxJSONElement json;

		string responseStr;
		string requestStr;
		string action_url;
		string baseAddr;

		bool isStreaming;

		vector <string> storeText;

		// ** Oscilloscope Stuff ** 
		ofxMultiScope scopeWin1;
		int newPoints;
		int nScopes;
		std::vector<std::vector<float> > data;
		int d;
		bool zeroData;
		int counter;
		int counter2;
		ofPoint min;
		ofPoint max;
		int selectedScope;
		bool isPaused;
		bool updateOscilloscope;
		bool drawOscilloscope;
		bool filterData;

		ofxMultiScope fftScopeWin;

		// ** Data Logger Stuff **
		LoggerThread logger;
		bool logData;

		// ** FFT Stuff **
		ofxFft* fft;
		int fftBufferSize;
		uint64_t lastBufferFillMillis;
		float bufferFillRate;
		std::vector<std::vector<float> > fftData;
		int Fs;		// Sampling Frequency

		vector<float> dcLevels;

		ofxMultiScope scopeWinFFT1;

		float notchFilterFreq;
		vector<ofxBiquadFilter1f> filterHP;
		vector<ofxBiquadFilter1f> filterLP;
		vector<ofxBiquadFilter1f> filterNotch;
};
