#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

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

	// Midi
	sendMidi = true;
	midiChannel = 1;
	midiValue = 100;
	midiout.listPorts();
	midiout.openPort(1);
	
	hiveMind.startThread();
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
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

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
			if (sendMidi)
			{
				midiout.sendNoteOn(midiChannel, midiId.at(h), midiValue);
			}
		}
		else
		{
			ofSetColor(0, 0, 0);
			if (sendMidi)
			{
				midiout.sendNoteOff(midiChannel, midiId.at(h), midiValue);
			}
		}
		ofDrawRectangle((float)h * ofGetWidth() / nHeadsets, 0.f,
			(float)ofGetWidth() / nHeadsets, ofGetHeight());
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
		if (drawOn.at(0) && drawOn.at(1))
		{
			drawOn.at(0) = true;
			drawOn.at(1) = false;
		}
		else if (drawOn.at(0))
		{
			drawOn.at(0) = false;
			drawOn.at(1) = true;
		}
		else if (drawOn.at(1))
		{
			drawOn.at(0) = true;
			drawOn.at(1) = true;
		}
	}

	if (((char)key) == OF_KEY_BACKSPACE) {
		sendMidi = !sendMidi;
	}
	if (((char)key) == '1') {
		midiout.sendControlChange(midiChannel, midiId.at(0), midiValue);
		printf("1");
	}
	if (((char)key) == '2') {
		midiout.sendControlChange(midiChannel, midiId.at(0) + 1, midiValue);
		printf("2");
	}
	if (((char)key) == '3') {
		midiout.sendNoteOn(midiChannel, midiId.at(0) + 2, midiValue);
		printf("3");
	}
	if (((char)key) == '4') {
		midiout.sendNoteOff(midiChannel, midiId.at(0) + 3, midiValue);
		printf("4");
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