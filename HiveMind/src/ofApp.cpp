#include "ofApp.h"
#include "openBciWifiHttpUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){

	tcpPort = 3000;
	computerIp = "192.168.1.100";
	openBciIps.push_back("192.168.1.101");
	openBciIps.push_back("192.168.1.102");

	hiveMind.setTcpPort(tcpPort);
	reTcpDelay = 10 * 10000;
	resetBandDataTimer.set(5000);

	nHeadsets = 2;

	printRates = true;

	ofSetFrameRate(targetFrameRate);
	ofSetVerticalSync(true);
	ofSetFullscreen(false);

	printFrameRateMillis = ofGetElapsedTimeMillis();
	lastFrameMicros = ofGetElapsedTimeMicros();

	uint64_t lastFrameMicros;
	uint64_t printFrameRateMillis;
	float frameDelay;

	changeFrameCountMin = 3 * 1000000;
	changeFrameCountMax = 3 * 1000000;

	int midiId0 = 60;
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
		midiId.push_back(midiId0 + h);
	}

	// Static Screens
	titleScreen.load("screens/TitleScreen.png");
	riskScreen.load("screens/RiskScreen.png");
	descriptionScreen.load("screens/DescriptionScreen.png");
	creditsScreen.load("screens/CreditsScreen.png");

	flashState0 = 3;
	//flashTimers.push_back(1.5 * 60 *1000);	// Left (Sean)
	//flashTimers.push_back(0* 60 * 1000);	// Both
	//flashTimers.push_back(2 * 60 * 1000);	// Right (Diego)
	//flashTimers.push_back(0 * 60 * 1000);	// Both
	//flashTimers.push_back(1 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(0.25 * 60 * 1000);// Both
	//flashTimers.push_back(2 * 60 * 1000);	// Right (Diego)
	//flashTimers.push_back(.5 * 60 * 1000);	// Both
	//flashTimers.push_back(2 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(0.5 * 60 * 1000);	// Both
	//flashTimers.push_back(0.25 * 60 * 1000);// Right (Diego)
	//flashTimers.push_back(0.5 * 60 * 1000);	// Both
	//flashTimers.push_back(0.5 * 60 * 1000);	// Left (Sean)
	//flashTimers.push_back(2 * 60 * 1000);	// Both
	//flashTimers.push_back(0.5 * 60 * 1000);	// Black

	flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(0 * 60 * 1000);	// Both
	flashTimers.push_back(.1 * 60 * 1000);	// Right (Diego)
	flashTimers.push_back(0 * 60 * 1000);	// Both
	flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(0.1 * 60 * 1000);// Both
	flashTimers.push_back(.1 * 60 * 1000);	// Right (Diego)
	flashTimers.push_back(.1 * 60 * 1000);	// Both
	flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(.1 * 60 * 1000);	// Both
	flashTimers.push_back(.1 * 60 * 1000);// Right (Diego)
	flashTimers.push_back(.1 * 60 * 1000);	// Both
	flashTimers.push_back(.1 * 60 * 1000);	// Left (Sean)
	flashTimers.push_back(.1 * 60 * 1000);	// Both
	flashTimers.push_back(0.1 * 60 * 1000);	// Black

	drawOn.at(0) = false;
	drawOn.at(1) = false;

	// Midi
	sendMidi = true;
	midiChannel = 1;
	midiValue = 100;
	midiout.listPorts();
	midiout.openPort(1);
	
	hiveMind.startThread();

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
					cout << eegBandData.at(b) << " / " << eegBaselineBandData.at(b) << ", ";
				}
				cout << endl;
			}
			changeFrameCountMicros.at(h) = ofGetElapsedTimeMicros();
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
			hiveMind.resetBandData();
		}
	}

}

//--------------------------------------------------------------
void ofApp::draw(){

	if (states.state == 0)
	{
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight();
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		ofSetColor(255, 255, 255);
		titleScreen.draw(imageXOffset,0);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}
	else if (states.state == 1)
	{
		// risk screen
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight();
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		ofSetColor(255, 255, 255);
		riskScreen.draw(imageXOffset, 0);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}
	else if (states.state == 2)
	{
		// description screen
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight();
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		ofSetColor(255, 255, 255);
		descriptionScreen.draw(imageXOffset, 0);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}
	else if (states.state == flashState0)
	{
		// flashing
		drawOn.at(0) = true;
		drawOn.at(1) = false;
		if (flashTimers.at(0).isElapsed())
		{
			states.next();
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
			flashTimers.at(14).start();
		}
	}
	else if (states.state == flashState0 + 14)
	{
		// black screen
		drawOn.at(0) = false;
		drawOn.at(1) = false;
		if (flashTimers.at(14).isElapsed())
		{
			states.next();
		}
	}
	else if (states.state == flashState0 + 15)
	{
		// credits screen
		float imageScaler = ofGetWindowHeight() / titleScreen.getHeight();
		float imageXOffset = (ofGetWindowWidth() - titleScreen.getWidth() * imageScaler) / 2;
		ofPushMatrix();
		ofScale(imageScaler, imageScaler);
		creditsScreen.draw(imageXOffset, 0);
		ofPopMatrix();
		drawOn.at(0) = false;
		drawOn.at(1) = false;
	}

	// Handle flashing
	for (int h = 0; h < nHeadsets; h++)
	{
		if (frameCount.at(h) >= targetFrameCount.at(h))
		{
			frameCount.at(h) = 0;
			drawWhiteOn.at(h) = !drawWhiteOn.at(h);
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
			ofDrawRectangle((float)h * ofGetWidth() / nHeadsets, 0.f,
				(float)ofGetWidth() / nHeadsets, ofGetHeight());
			if (sendMidi)
			{
				midiout.sendNoteOn(midiChannel, midiId.at(h), midiValue);
			}
		}
		else
		{
			//ofSetColor(0, 0, 0);
			if (sendMidi)
			{
				midiout.sendNoteOff(midiChannel, midiId.at(h), midiValue);
			}
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
void ofApp::keyReleased(int key){
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
		states.state = flashState0 + + flashTimers.size() - 1;	// Black -> Credits
	}
	if (key == 't')
	{
		for each (string ip in openBciIps)
		{
			openBciWifiTcp(computerIp, tcpPort, ip);
			Sleep(2000);
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
		if (openBciIps.size() == 2)
		{
			vector<string> openBciIpsTemp = openBciIps;
			openBciIps.at(0) = openBciIpsTemp.at(1);
			openBciIps.at(1) = openBciIpsTemp.at(0);
		}
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
			midiout.sendNoteOff(midiChannel, midiId.at(h), midiValue);
		}
		midiout.closePort();
	}

	hiveMind.waitForThread(true);
}