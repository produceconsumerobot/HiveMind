#include "ofApp.h"
#include "openBciWifiHttpUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetLogLevel(OF_LOG_NOTICE);

	tcpPort = 3000;
	computerIp = "192.168.1.100";
	openBciIps.push_back("192.168.1.101");
	openBciIps.push_back("192.168.1.102");

	//hiveMind.setTcpPort(tcpPort);
	reTcpDelay = 10 * 1000;
	resetBandDataTimer.set(5000);

	nHeadsets = 2;
	swapHeadsets = false;

	imageYOffset = 0;
	imageScaler2 = 1;

	printRates = true;

	ofSetFrameRate(targetFrameRate);
	ofSetVerticalSync(true);
	ofSetFullscreen(false);

	printFrameRateMillis = ofGetElapsedTimeMillis();
	lastFrameMicros = ofGetElapsedTimeMicros();

	uint64_t lastFrameMicros;
	uint64_t printFrameRateMillis;
	float frameDelay;

	changeFrameCountMin = 9 * 1000000;	// microseconds
	changeFrameCountMax = 14 * 1000000;	// microseconds

	int midiId0 = 60;
	midiId.resize(nHeadsets);
	for (int h = 0; h < nHeadsets; h++)
	{
		lastDrawMicros.push_back(ofGetElapsedTimeMicros());
		drawDelay.push_back(0);
		frameCount.push_back(0);
		targetFrameCount.push_back(4);
		changeFrameCountMicros.push_back(ofGetElapsedTimeMicros());
		changeFrameCountInterval.push_back(ofRandom(changeFrameCountMin, changeFrameCountMax));
		drawWhiteOn.push_back(true);
		drawOn.push_back(true);
		dominantBand.push_back(0);
		for (int b = 0; b < hiveMind.getNumEegBands(); b++)
		{
			midiId.at(h).push_back(midiId0 + b * nHeadsets + h);
		}
	}

	// Static Screens
	titleScreen.load("screens/TitleScreen.png");
	riskScreen.load("screens/RiskScreen.png");
	descriptionScreen.load("screens/DescriptionScreen.png");
	creditsScreen.load("screens/CreditsScreen.png");

	introTimers.push_back(0.5 * 60 * 1000);
	introTimers.push_back(0.5 * 60 * 1000);

	flashState0 = 3;
	flashTimers.push_back(1.5 * 60 *1000);	// Left (Sean)
	flashTimers.push_back(0* 60 * 1000);	// Both
	flashTimers.push_back(1.5 * 60 * 1000);	// Right (Diego)
	flashTimers.push_back(0 * 60 * 1000);	// Both
	flashTimers.push_back(1 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(0.25 * 60 * 1000);// Both
	flashTimers.push_back(2 * 60 * 1000);	// Right (Diego)
	flashTimers.push_back(.5 * 60 * 1000);	// Both
	flashTimers.push_back(2 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(1 * 60 * 1000);	// Both
	flashTimers.push_back(0.25 * 60 * 1000);// Right (Diego)
	flashTimers.push_back(0.5 * 60 * 1000);	// Both
	flashTimers.push_back(0.5 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(2 * 60 * 1000);	// Both
	flashTimers.push_back(1.f / 6 * 60 * 1000);	// Black

	//flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(0 * 60 * 1000);	// Both
	//flashTimers.push_back(.1 * 60 * 1000);	// Right (Diego)
	//flashTimers.push_back(0 * 60 * 1000);	// Both
	//flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(0.1 * 60 * 1000);// Both
	//flashTimers.push_back(.1 * 60 * 1000);	// Right (Diego)
	//flashTimers.push_back(.1 * 60 * 1000);	// Both
	//flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(.1 * 60 * 1000);	// Both
	//flashTimers.push_back(.1 * 60 * 1000);// Right (Diego)
	//flashTimers.push_back(.1 * 60 * 1000);	// Both
	//flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(.1 * 60 * 1000);	// Both
	//flashTimers.push_back(0.1 * 60 * 1000);	// Black

	drawOn.at(0) = false;
	drawOn.at(1) = false;

	// ** Midi **
	sendMidi = true;
	midiChannel = 1;
	midiValue = 100;
	midiout.listPorts();
	midiout.openPort(1);

	// ** Oscilloscopes **
	drawOscilloscopes = true;
	oscilloscopesOn = true;
	drawScopesAtEnd = true;
	overlayAlpha = 128;
	isPaused = false;
	int Fs = 250;
	ofTrueTypeFont legendFont;
	legendFont.load("verdana.ttf", 7, true, true);
	ofColor scopeColor(128, 128, 128);
	std::vector<ofColor> oscColors;
	nChan = 8;
	for (int ch = 0; ch < nChan; ch++)
	{
		oscColors.push_back(scopeColor);
	}
	std::vector<string> oscNames = { "ch1", "ch2", "ch3", "ch4", "ch5", "ch6", "ch7", "ch8" };
	float timeWindow = 5.f; // seconds
	float yScale = 0.004f; // yScale multiplier
	float yOffset = 0.f; // yOffset from the center of the scope window
	winSize = ofGetWindowSize();
	yTop = 30;
	xGap = 20;
	scopeWins.resize(nHeadsets);
	ofRectangle scopeArea;
	scopeArea = ofRectangle(ofPoint(0, yTop), ofPoint(winSize.x / 2 - xGap, winSize.y));
	scopeWins.at(0) = ofxMultiScope(nChan, scopeArea, legendFont, 40); // Setup the multiScope panel
	scopeWins.at(0).setOutlineColor(ofColor(0, 0, 0));
	scopeArea = ofRectangle(ofPoint(winSize.x / 2 + xGap, yTop), ofPoint(winSize.x / 2 + winSize.x / 2, winSize.y));
	scopeWins.at(1) = ofxMultiScope(nChan, scopeArea, legendFont, 40); // Setup the multiScope panel
	scopeWins.at(1).setOutlineColor(ofColor(0, 0, 0));
	for (int h = 0; h < nHeadsets; h++)
	{
		for (int ch = 0; ch < oscColors.size(); ch++)
		{
			scopeWins.at(h).scopes.at(ch).setup(timeWindow, Fs, oscNames.at(ch), oscColors.at(ch), yScale, yOffset); // Setup each oscilloscope panel
		}
	}
	
	hiveMind.startThread();

	keyReleased('t');

	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){
	for (int h = 0; h < nHeadsets; h++)
	{
		if (ofGetElapsedTimeMicros() - changeFrameCountMicros.at(h) > changeFrameCountInterval.at(h))
		{
			int temp = hiveMind.getFrameCount(h, targetFrameRate);
			if (temp >= 0)
			{
				targetFrameCount.at(h) = temp;
				midiout.sendNoteOff(midiChannel, midiId.at(h).at(dominantBand.at(h)), midiValue);
				ofLogVerbose() << "sendNoteOff midiId=" << midiId.at(h).at(dominantBand.at(h));
				dominantBand.at(h) = hiveMind.getDominantBand(h);
				cout << "Set frame count h" << h << " = " << temp << endl;
				vector<float> eegBandData = hiveMind.getEegBandData(h);
				vector<float> eegBaselineBandData = hiveMind.getEegBaselineBandData(h);
				cout << "h" << h << " bands: ";
				for (int b = 0; b < eegBandData.size(); b++)
				{
					cout << eegBandData.at(b) / eegBaselineBandData.at(b) << ", ";
				}
				cout << endl;
				cout << "h" << h << " bands: ";
				for (int b = 0; b < eegBandData.size(); b++)
				{
					cout << (int) eegBandData.at(b) << " / " << (int) eegBaselineBandData.at(b) << ", ";
				}
				cout << endl;
			}
			changeFrameCountMicros.at(h) = ofGetElapsedTimeMicros();
		}

		if (drawOscilloscopes)
		{
			int hDraw = h;
			if (swapHeadsets)
			{
				hDraw = (h + 1) % 2;
			}
			vector<vector<float>> tempData = hiveMind.getData(hDraw);
			for (int ch = 0; ch < tempData.size() && ch < nChan; ch++)
			{
				if (!isPaused)
				{
					scopeWins.at(h).scopes.at(ch).updateData(tempData.at(ch));
				}
			}
		}

		if (hiveMind.getFftDelay(h) > reTcpDelay)
		{
			// reestablish TCP link if lost for too long
			openBciWifiTcp(computerIp, tcpPort, openBciIps.at(h));
			openBciWifiStart(openBciIps.at(h));
			resetBandDataTimer.start();
		}
		if (resetBandDataTimer.isElapsed())
		{
			// Renormalize FFT
			hiveMind.resetBandData();
			resetBandDataTimer.stop();
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	// ** Handle Oscilloscopes **
	if (drawOscilloscopes && oscilloscopesOn)
	{
		for (int h = 0; h < nHeadsets; h++)
		{
			int hDraw = h;
			if (swapHeadsets)
			{
				hDraw = (h + 1) % 2;
			}
			ofPushMatrix();
			ofDrawBitmapString("OpenBCI Headset " + ofToString(hDraw) + ": " + openBciIps.at(hDraw), ofGetWindowSize().x / 2 * h + 10, yTop - 20);
			ofScale(ofGetWindowSize().x / winSize.x, ofGetWindowSize().y / winSize.y);
			scopeWins.at(hDraw).plot();
			ofPopMatrix();
		}
	}

	// ** Handle App States ** 
	if (states.state == 0)
	{
		// Title screen
		ofSetColor(0, 0, 0, overlayAlpha);
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight() * imageScaler2;
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		ofSetColor(255, 255, 255);
		titleScreen.draw(imageXOffset / imageScaler, imageYOffset);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}
	else if (states.state == 1)
	{
		// risk screen
		ofSetColor(0, 0, 0, overlayAlpha);
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight() * imageScaler2;
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		ofSetColor(255, 255, 255);
		riskScreen.draw(imageXOffset / imageScaler, imageYOffset);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}
	else if (states.state == 2)
	{
		// description screen
		ofSetColor(0, 0, 0, overlayAlpha);
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight() * imageScaler2;
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		ofSetColor(255, 255, 255);
		descriptionScreen.draw(imageXOffset / imageScaler, imageYOffset);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}
	else if (states.state == flashState0)
	{
		// flashing
		drawOscilloscopes = false;
		drawOn.at(0) = true;
		drawOn.at(1) = false;
		if (flashTimers.at(0).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(1).start();
		}
	}
	else if (states.state == flashState0 + 1)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(1).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(2).start();
		}
	}
	else if (states.state == flashState0 + 2)
	{
		// flashing
		drawOn.at(0) = false;
		drawOn.at(1) = true;
		if (flashTimers.at(2).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(3).start();
		}
	}
	else if (states.state == flashState0 + 3)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(3).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(4).start();
		}
	}
	else if (states.state == flashState0 + 4)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = false;
		if (flashTimers.at(4).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(5).start();
		}
	}
	else if (states.state == flashState0 + 5)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(5).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(6).start();
		}
	}
	else if (states.state == flashState0 + 6)
	{
		// flashing
		drawOn.at(0) = false;
		drawOn.at(1) = true;
		if (flashTimers.at(6).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(7).start();
		}
	}
	else if (states.state == flashState0 + 7)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(7).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(8).start();
		}
	}
	else if (states.state == flashState0 + 8)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = false;
		if (flashTimers.at(8).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(9).start();
		}
	}
	else if (states.state == flashState0 + 9)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(9).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(10).start();
		}
	}
	else if (states.state == flashState0 + 10)
	{
		// flashing
		drawOn.at(0) = false;
		drawOn.at(1) = true;
		if (flashTimers.at(10).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(11).start();
		}
	}
	else if (states.state == flashState0 + 11)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(11).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(12).start();
		}
	}
	else if (states.state == flashState0 + 12)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = false;
		if (flashTimers.at(12).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(13).start();
		}
	}
	else if (states.state == flashState0 + 13)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = true;
		if (flashTimers.at(13).isElapsed())
		{
			states.next();
			allNotesOff();
			flashTimers.at(14).start();
		}
	}
	else if (states.state == flashState0 + 14)
	{
		// black screen
		ofPushStyle();
		ofSetColor(0, 0, 0, overlayAlpha);
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		ofPopStyle();
		drawOscilloscopes = drawScopesAtEnd;
		drawOn.at(0) = false;
		drawOn.at(1) = false;
		if (flashTimers.at(14).isElapsed())
		{
			states.next();
			allNotesOff();
		}
	}
	else if (states.state == flashState0 + 15)
	{
		// credits screen
		ofPushStyle();
		ofSetColor(0, 0, 0, overlayAlpha);
		ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		ofPopStyle();
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight() * imageScaler2;
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofSetColor(255, 255, 255);
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		creditsScreen.draw(imageXOffset / imageScaler, imageYOffset);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}

	// ** Handle flashing ** 
	for (int h = 0; h < nHeadsets; h++)
	{
		int hDraw = h;
		if (swapHeadsets)
		{
			hDraw = (h + 1) % 2;
		}
		if (frameCount.at(h) >= targetFrameCount.at(h))
		{
			frameCount.at(h) = 0;
			drawWhiteOn.at(h) = !drawWhiteOn.at(h);
			if (drawWhiteOn.at(h) && drawOn.at(h)) // white screen!
			{
				if (sendMidi)
				{
					//for (int b = 0; b < hiveMind.getNumEegBands(); b++) {
						//midiout.sendNoteOff(midiChannel, midiId.at(hDraw).at(b), midiValue);
						//ofLogVerbose() << "sendNoteOff midiId=" << midiId.at(hDraw).at(b) << endl;
					//}
					midiout.sendNoteOn(midiChannel, midiId.at(hDraw).at(dominantBand.at(h)), midiValue);
					ofLogVerbose() << "sendNoteOn midiId=" << midiId.at(hDraw).at(dominantBand.at(h));
				}
			} 
			else
			{
				if (sendMidi)
				{
					//for (int b = 0; b < hiveMind.getNumEegBands(); b++) {
						//midiout.sendNoteOff(midiChannel, midiId.at(hDraw).at(b), midiValue);
						//ofLogVerbose() << "sendNoteOff midiId=" << midiId.at(hDraw).at(b);
					//}
					midiout.sendNoteOff(midiChannel, midiId.at(hDraw).at(dominantBand.at(h)), midiValue);
					ofLogVerbose() << "sendNoteOff midiId=" << midiId.at(hDraw).at(dominantBand.at(h));
				}
			}
			if (printRates)
			{
				drawDelay.at(h) = ofGetElapsedTimeMicros() - lastDrawMicros.at(h);
				lastDrawMicros.at(h) = ofGetElapsedTimeMicros();
			}
		}
		else
		{
			frameCount.at(h)++;
		}
		if (drawWhiteOn.at(h) && drawOn.at(h))
		{
			ofSetColor(255, 255, 255);
			ofDrawRectangle((float)hDraw * ofGetWidth() / nHeadsets, 0.f,
				(float)ofGetWidth() / nHeadsets, ofGetHeight());
		}
		else
		{
			//ofSetColor(0, 0, 0);
		}
	}

	if (printRates)
	{
		frameDelay = ofGetElapsedTimeMicros() - lastFrameMicros;
		lastFrameMicros = ofGetElapsedTimeMicros();

		if (ofGetElapsedTimeMillis() - printFrameRateMillis > 2000)
		{
			printFrameRateMillis = ofGetElapsedTimeMillis();
			cout << "Frame Rate: " << 1000000.f / frameDelay;
			for (int i = 0; i < nHeadsets; i++)
			{
				cout << ", Rate" << i << ": " << 1000000.f / drawDelay.at(i) / 2;
			}
			cout << endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	if (key == 'u')
	{
		targetFrameCount.at(0) = 4;
	}
	if (key == 'i')
	{
		targetFrameCount.at(0) = 2;
	}
	if (key == 'o')
	{
		targetFrameCount.at(0) = 1;
	}
	if (key == 'j')
	{
		targetFrameCount.at(1) = 4;
	}
	if (key == 'k')
	{
		targetFrameCount.at(1) = 2;
	}
	if (key == 'l')
	{
		targetFrameCount.at(1) = 1;
	}
	if (key == 'f')
	{
		ofToggleFullscreen();
	}
	if (key == 'r')
	{
		hiveMind.resetBandData();
	}
	if (key == ' ')
	{
		states.next();
		allNotesOff();
		if (states.state == 1)
		{
			hiveMind.resetBandData();
		}
		if (states.state == flashState0)
		{
			flashTimers.at(0).start();
		}
	}

	if (((char)key) == OF_KEY_BACKSPACE) {
		sendMidi = !sendMidi;
	}
	if (((char)key) == '1') {
		states.state = 0;	// Title Screen
	}
	if (((char)key) == '2') {
		states.state = 1;	// Risk Screen
	}
	if (((char)key) == '3') {
		states.state = 2;	// Description Screen
	}
	if (((char)key) == '4') {
		states.state = flashState0;	// Flashing
		flashTimers.at(0).start();
	}
	if (((char)key) == '5') {
		states.state = flashState0 + flashTimers.size() - 1;	// Black -> Credits
	}
	if (key == 't')
	{
		// Setup TCP connection with openBci headsets
		for each (string ip in openBciIps)
		{
			openBciWifiTcp(computerIp, tcpPort, ip);
			Sleep(3000);
		}
		resetBandDataTimer.start();
	}
	if (key == 's')
	{
		// Start streaming data
		for each (string ip in openBciIps)
		{
			openBciWifiStart(ip);
		}
	}
	if (key == 'S')
	{
		// Stop streaming data
		for each (string ip in openBciIps)
		{
			openBciWifiStop(ip);
		}
	}
	if (key == 'd')
	{
		// Turn on raw data sending from openBci Cyton boards
		for each (string ip in openBciIps)
		{
			openBciWifiAnalogDataOn(ip);
		}
	}
	if (key == '[')
	{
		// Turn on square wave data sending from openBci Cyton boards
		for each (string ip in openBciIps)
		{
			openBciWifiSquareWaveOn(ip);
		}
	}
	if (key == 'h')
	{
		swapHeadsets = !swapHeadsets;
		cout << "swapHeadsets=" << swapHeadsets << ": ";
		for (int h = 0; h < openBciIps.size(); h++)
		{
			cout << openBciIps.at(h) << ", ";
		}
		cout << endl;
		//if (openBciIps.size() == 2)
		//{
		//	vector<string> openBciIpsTemp = openBciIps;
		//	openBciIps.at(0) = openBciIpsTemp.at(1);
		//	openBciIps.at(1) = openBciIpsTemp.at(0);
		//}
	}

	// ** Oscilloscope scaling **
	// Increment the yScale
	if (key == '+') {
		for (int h = 0; h < nHeadsets; h++)
		{
			scopeWins.at(h).incrementYScale();
		}
	}
	// Decrement the yScale
	if ((key == '-') || (key == '_')) {
		for (int h = 0; h < nHeadsets; h++)
		{
			scopeWins.at(h).decrementYScale();
		}
	}
	// Increment the yOffset
	if (key == 357) { // Up Arrow
		for (int h = 0; h < nHeadsets; h++)
		{
			scopeWins.at(h).incrementYOffset();
		}
	}
	// Decrement the yOffset
	if (key == 359) { // Down Arrow
		for (int h = 0; h < nHeadsets; h++)
		{
			scopeWins.at(h).decrementYOffset();
		}
	}
	// Increment the timeWindow
	if (key == 358) { // Right Arrow
		for (int h = 0; h < nHeadsets; h++)
		{
			scopeWins.at(h).incrementTimeWindow();
		}
	}

	// Decrement the timeWindow
	if (key == 356) { // Left Arrow
		for (int h = 0; h < nHeadsets; h++)
		{
			scopeWins.at(h).decrementTimeWindow();
		}
	}
	// Increment the overlayAlpha
	if (key == 'A') {
		overlayAlpha += 5;
	}
	// Decrement the overlayAlpha
	if (key == 'a') {
		overlayAlpha -= 5;
	}

	if (key == 'o')
	{
		oscilloscopesOn = !oscilloscopesOn;
	}
	// Make images smaller
	if (key == '<') {
		imageScaler2 = imageScaler2 / 1.1;
	}
	// Make images larger
	if (key == '>') {
		imageScaler2 = imageScaler2 * 1.1;
	}
	// Move images up
	if (key == '.') {
		imageYOffset = imageYOffset - 10;
	}
	// Move images Down
	if (key == ',') {
		imageYOffset = imageYOffset + 10;
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

void ofApp::exit()
{
	if (midiout.isOpen())
	{
		for (int h = 0; h < nHeadsets; h++)
		{
			for (int b = 0; b < hiveMind.getNumEegBands(); b++) {
				midiout.sendNoteOff(midiChannel, midiId.at(h).at(b), midiValue);
			}
		}
		midiout.closePort();
	}

	//hiveMind.waitForThread(true);
}

void ofApp::allNotesOff()
{
	if (sendMidi)
	{
		for (int h = 0; h < nHeadsets; h++)
		{
			for (int b = 0; b < hiveMind.getNumEegBands(); b++) {
				midiout.sendNoteOff(midiChannel, midiId.at(h).at(b), midiValue);
				ofLogVerbose() << "sendNoteOff midiId=" << midiId.at(h).at(b) << endl;
			}
		}
	}
}