#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// setup the server to listen on 3000
	port = 3000;
	TCP.setup(port);
	// optionally set the delimiter to something else.  The delimiter in the client and the server have to be the same, default being [/TCP]
	TCP.setMessageDelimiter("\r\n");

	updateOscilloscope = true;
	drawOscilloscope = true;
	logData = true;
	
	//ofAddListener(httpUtils.newResponseEvent, this, &ofApp::newResponse);
	//httpUtils.start();

	baseAddr = "http://10.0.0.3/";

	Fs = 250;
	fftBufferSize = 250;
	d = 0;
	notchFilterFreq = 60;

	// ** Oscilloscope Stuff ** 
	ofTrueTypeFont legendFont;
	legendFont.load("verdana.ttf", 7, true, true);

	std::vector<ofColor> oscColors = {
		ofColor(255, 0, 0),
		ofColor(196, 196, 196),
		ofColor(255, 255, 0),
		ofColor(128, 128, 128),
		ofColor(255, 0, 255),
		ofColor(255, 140, 0),
		ofColor(196, 0, 196),
		ofColor(139, 69, 19)
	};

	std::vector<string> oscNames = { "ch1", "ch2", "ch3", "ch4", "ch5", "ch6", "ch7", "ch8"};

	nScopes = oscColors.size();
	float timeWindow = 5.f; // seconds
	float yScale = 0.004f; // yScale multiplier
	float yOffset = 0.f; // yOffset from the center of the scope window
	ofPoint winSize = ofGetWindowSize();
	ofRectangle scopeArea = ofRectangle(ofPoint(0, winSize.y / 4), ofPoint(winSize.x * 2 / 3, winSize.y));
	scopeWin1 = ofxMultiScope(nScopes, scopeArea, legendFont); // Setup the multiScope panel
	for (int ch = 0; ch < oscColors.size(); ch++)
	{
		scopeWin1.scopes.at(ch).setup(timeWindow, Fs, oscNames.at(ch), oscColors.at(ch), yScale, yOffset); // Setup each oscilloscope panel
	}


	// Allocate space for new data in form data[nVariables][nPoints]
	data.resize(oscNames.size(), vector<float>(fftBufferSize, 0));
	
	// ** Data Logger Stuff **
	logger.setDirPath(ofToDataPath(""));
	logger.setFilename(ofGetTimestampString("%Y-%m-%d-%H-%M-%S") + ".log");
	logger.startThread();

	// ** FFT Stuff **
	fft = ofxFft::create(fftBufferSize, OF_FFT_WINDOW_HAMMING);
	int nBins = fft->getBinFromFrequency(70, Fs);
	scopeArea = ofRectangle(ofPoint(winSize.x * 2 / 3, winSize.y / 4), ofPoint(winSize.x, winSize.y));
	scopeWinFFT1 = ofxMultiScope(nScopes, scopeArea, legendFont); // Setup the multiScope panel
	fftData.resize(oscNames.size());
	for (int ch = 0; ch < oscNames.size(); ch++)
	{
		scopeWinFFT1.scopes.at(ch).setup(nBins, 1, oscNames.at(ch), oscColors.at(ch), 40, -1200); // Setup each oscilloscope panel
		fftData.at(ch).resize(nBins);
		fftData.at(ch).assign(nBins, 0);
	}
	//scopeWin.scopes.at(nChan).setup(1, nBins, oscNames, oscColors, 32, -1200); // Setup each oscilloscope panel
	lastBufferFillMillis = ofGetElapsedTimeMillis();

	// ** Filtering Stuff ** 
	filterHP.resize(oscNames.size());
	for (int ch = 0; ch < oscNames.size(); ch++)
	{
		// 1Hz Butterworth Highpass
		filterHP.at(ch) = ofxBiquadFilter1f(OFX_BIQUAD_TYPE_HIGHPASS, 1.f / Fs, 0.7071);
	}
	filterLP.resize(oscNames.size());
	for (int ch = 0; ch < oscNames.size(); ch++)
	{
		// 40Hz Butterworth Lowpass
		//filterLP.at(ch) = ofxBiquadFilter1f(OFX_BIQUAD_TYPE_LOWPASS, 0.16, 0.7071);
		filterLP.at(ch) = ofxBiquadFilter1f(OFX_BIQUAD_TYPE_LOWPASS, 35.f / Fs, 0.90);
	}
	filterNotch.resize(oscNames.size());
	for (int ch = 0; ch < oscNames.size(); ch++)
	{
		// Notch filter
		filterNotch.at(ch) = ofxBiquadFilter1f(OFX_BIQUAD_TYPE_NOTCH, notchFilterFreq / Fs, 0.7071);
	}
}

//void ofApp::newResponse(ofxHttpResponse & response) {
//	responseStr = ofToString(response.status) + ": " + (string)response.responseBody;
//	bool happy = json.parse((string)response.responseBody);
//	if (happy)
//	{
//	}
//	else
//	{
//	}
//	ofLogNotice("json:") << json.getRawString();
//}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetHexColor(0xDDDDDD);
	ofDrawBitmapString("OpenBCI FFT WiFi Example \n\nconnect on port: " + ofToString(TCP.getPort()), 10, 20);

	ofSetHexColor(0xDDDDDD);
	ofDrawBitmapString("Frame Rate: " + ofToString(ofGetFrameRate()) + "\n\nFFT Buffer Fill Rate: " + ofToString(1000.f / bufferFillRate), 500, 20);

	ofSetHexColor(0x000000);
	ofDrawRectangle(10, 60, ofGetWidth() - 24, ofGetHeight() - 65 - 15);

	ofSetHexColor(0xDDDDDD);

	// for each connected client lets get the data being sent and lets print it to the screen
	for (unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++) {

		if (!TCP.isClientConnected(i))continue;

		// give each client its own color
		ofSetColor(255 - i * 30, 255 - i * 20, 100 + i * 40);

		// calculate where to draw the text
		int xPos = 15;
		int yPos = 80 + (12 * i * 4);

		// get the ip and port of the client
		string port = ofToString(TCP.getClientPort(i));
		string ip = TCP.getClientIP(i);
		string info = "client " + ofToString(i) + " -connected from " + ip + " on port: " + port;


		// if we don't have a string allocated yet
		// lets create one
		if (i >= storeText.size()) {
			storeText.push_back(string());
		}

		// receive all the available messages, separated by \n
		string str;
		string tmp;
		do {
			str = str + tmp;
			tmp = TCP.receive(i);
		} while (tmp != "");

		// if there was a message set it to the corresponding client
		if (str.length() > 0) {
			storeText[i] = str;

			// Split the string by "chunk"
			vector<string> result = ofSplitString(str, "{\"chunk\":");
			for (int r = 1; r < result.size(); r++)
			{
				// Add back "chunk"
				result.at(r) = "{\"chunk\":" + result.at(r);
				bool success = json.parse(result.at(r));
				if (success)
				{
					int nSamples = 0;
					try {
						nSamples = json["chunk"].size();
					}
					catch (exception e) {}
					for (int s = 0; s < nSamples; s++)
					{
						try {
							if (logData)
							{
								logger.push(json["chunk"][s]["sampleNumber"].asString() + ",");
							}
						}
						catch (exception e) {}
						try {
							if (logData)
							{
								logger.push(json["chunk"][s]["timestamp"].asString() + ",");
							}
						}
						catch (exception e) {}
						//try {
						//	logger.push(json["count"].asString() + ",");
						//}
						//catch (exception e) {}
						if (logData)
						{
							logger.push(ofToString(i) + ",");
						}
						for (int ch = 0; ch < data.size(); ch++)
						{
							try {
								data.at(ch).at(d) = (float) json["chunk"][s]["data"][ch].asDouble();
								if (logData)
								{
									logger.push(json["chunk"][s]["data"][ch].asString() + ",");
								}
							}
							catch (exception e) {
								bool debug = true;
							}
						}
						// increment the data pointer
						d++;

						if (logData)
						{
							logger.push("\n");
						}
						if (d == fftBufferSize)
						{
							d = 0;

							// if the buffer is full
							for (int ch = 0; ch < data.size(); ch++)
							{
								for (int n = 0; n < data[ch].size(); n++)
								{
									// Filter data
									data.at(ch).at(n) = filterHP.at(ch).update(data.at(ch).at(n));
									data.at(ch).at(n) = filterNotch.at(ch).update(data.at(ch).at(n));
									//data.at(ch).at(n) = filterLP.at(ch).update(data.at(ch).at(n));
								}

								fft->setSignal(data.at(ch));
								float* curFft = fft->getAmplitude();
								//memcpy(&fftData[ch][0], curFft, sizeof(float) * fftData[ch].size());
								for (int n = 0; n < fftData[ch].size(); n++)
								{
									// Smooth the FFT over time so that after X seconds only 20% "legacy" influence remains
									float newDataWeight = 1.f - pow(10, log10(0.2) / (7 * Fs / fftBufferSize));
									// Calculate the FFT power in dB for easier viewing
									if (isfinite(fftData.at(ch).at(n)))
									{
										fftData.at(ch).at(n) = smoother(10.f * log10(curFft[n]), fftData.at(ch).at(n), newDataWeight);
									}
									else
									{
										// Handle case when fftData runs off into the weeds
										fftData.at(ch).at(n) = 10.f * log10(curFft[n]);
									}
								}

								if (updateOscilloscope)
								{
									vector<vector<float>> temp;
									temp.push_back(data.at(ch));
									scopeWin1.scopes.at(ch).updateData(temp);

									vector<vector<float>> temp2;
									temp2.push_back(fftData.at(ch));
									scopeWinFFT1.scopes.at(ch).updateData(temp2);
								}
							}
							//if (updateOscilloscope)
							//{
							//	scopeWinFFT1.scopes.at(1).updateData(fftData);
							//}
							
							bufferFillRate = ofGetElapsedTimeMillis() - lastBufferFillMillis;
							lastBufferFillMillis = ofGetElapsedTimeMillis();
						}
					}
				}
			}
		}

		// draw the info text and the received text bellow it
		ofDrawBitmapString(info, xPos, yPos);
		ofDrawBitmapString(storeText[i], 25, yPos + 20);

	}
	if (drawOscilloscope)
	{
		scopeWin1.plot();
		scopeWinFFT1.plot();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == ' ')
	{
	}
	else if (key == 'S')
	{
		//action_url = baseAddr + "stream/start";
		//ofxHttpForm form;
		//form.action = action_url;
		//form.method = OFX_HTTP_GET;
		//httpUtils.addForm(form);
	}
	else if (key == 's')
	{
		//action_url = baseAddr + "stream/stop";
		//ofxHttpForm form;
		//form.action = action_url;
		//form.method = OFX_HTTP_GET;
		//httpUtils.addForm(form);
	}

	// Increment the yScale
	if (key == '+') {
		if (selectedScope == 0) {
			scopeWin1.incrementYScale();
		}
		else {
			//scopeWinFFT1.scopes.at(selectedScope - 1).incrementYScale();
			scopeWinFFT1.incrementYScale();
		}
	}

	// Decrement the yScale
	if ((key == '-') || (key == '_')) {
		if (selectedScope == 0) {
			scopeWin1.decrementYScale();
		}
		else {
			scopeWinFFT1.decrementYScale();
		}
	}

	// Increment the yOffset
	if (key == 357) { // Up Arrow
		if (selectedScope == 0) {
			scopeWin1.incrementYOffset();
		}
		else {
			scopeWinFFT1.incrementYOffset();
		}
	}

	// Decrement the yOffset
	if (key == 359) { // Down Arrow
		if (selectedScope == 0) {
			scopeWin1.decrementYOffset();
		}
		else {
			scopeWinFFT1.decrementYOffset();
		}
	}

	// Increment the timeWindow
	if (key == 358) { // Right Arrow
		if (selectedScope == 0) {
			scopeWin1.incrementTimeWindow();
		}
		else {
			scopeWinFFT1.incrementTimeWindow();
		}
	}

	// Decrement the timeWindow
	if (key == 356) { // Left Arrow
		if (selectedScope == 0) {
			scopeWin1.decrementTimeWindow();
		}
		else {
			scopeWinFFT1.decrementTimeWindow();
		}
	}
	if (key == '0') {
		selectedScope = 0;
	}
	if (key == '1') {
		selectedScope = 1;
	}
	if (key == '2') {
		selectedScope = 2;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::subtractDC(vector<float> & data, float & dc, float newDataWeight)
{
	for (int n = 0; n < data.size(); n++)
	{
		float newData = data.at(n);
		data.at(n) = data.at(n) - dc;
		dc = dc * (1.f - newDataWeight) + newData * newDataWeight;
	}
}

//--------------------------------------------------------------
float ofApp::smoother(float newData, float oldData, float newDataWeight)
{
	return newData * newDataWeight + oldData * (1.f - newDataWeight);
}

