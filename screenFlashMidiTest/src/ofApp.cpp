#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(targetFrameRate);
	ofSetVerticalSync(true);
	ofSetFullscreen(false);

	printFrameRateMillis = ofGetElapsedTimeMillis();
	lastDrawMicros = ofGetElapsedTimeMicros();
	lastDrawMicros1 = ofGetElapsedTimeMicros();
	lastDrawMicros2 = ofGetElapsedTimeMicros();
	frameCount1 = 0;
	targetFrameCount1 = 1;
	drawWhiteOn1 = true;

	frameCount2 = 0;
	targetFrameCount2 = 1;
	drawWhiteOn2 = true;

	draw1 = true;
	draw2 = false;

	midiId0 = 60;
	midiId1 = midiId0 + 1;

	// ** Midi **
	sendMidi = true;
	midiChannel = 1;
	midiValue = 100;
	midiout.listPorts();
	midiout.openPort(1);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	// 1
	if (frameCount1 >= targetFrameCount1)
	{
		frameCount1 = 0;
		drawWhiteOn1 = !drawWhiteOn1;
		if (printRates)
		{
			drawDelay1 = ofGetElapsedTimeMicros() - lastDrawMicros1;
			lastDrawMicros1 = ofGetElapsedTimeMicros();
		}
	}
	else
	{
		frameCount1++;
	}
	if (drawWhiteOn1 && draw1)
	{
		ofSetColor(255, 255, 255);
		if (sendMidi)
		{
			midiout.sendNoteOn(midiChannel, midiId0, midiValue);
		}
	}
	else
	{
		ofSetColor(0, 0, 0);
		if (sendMidi)
		{
			midiout.sendNoteOff(midiChannel, midiId0, midiValue);
		}
	}
	ofDrawRectangle(0, 0, ofGetWidth() / 2, ofGetHeight());

	// 2
	if (frameCount2 >= targetFrameCount2)
	{
		frameCount2 = 0;
		drawWhiteOn2 = !drawWhiteOn2;
		if (printRates)
		{
			drawDelay2 = ofGetElapsedTimeMicros() - lastDrawMicros2;
			lastDrawMicros2 = ofGetElapsedTimeMicros();
		}
	}
	else
	{
		frameCount2++;
	}
	if (drawWhiteOn2 && draw2)
	{
		ofSetColor(255, 255, 255);
		if (sendMidi)
		{
			midiout.sendNoteOn(midiChannel, midiId1, midiValue);
		}
	}
	else
	{
		ofSetColor(0, 0, 0);
		if (sendMidi)
		{
			midiout.sendNoteOff(midiChannel, midiId1, midiValue);
		}
	}
	ofDrawRectangle(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight());

	if (printRates)
	{
		drawDelay = ofGetElapsedTimeMicros() - lastDrawMicros;
		lastDrawMicros = ofGetElapsedTimeMicros();

		if (ofGetElapsedTimeMillis() - printFrameRateMillis > 1000)
		{
			printFrameRateMillis = ofGetElapsedTimeMillis();
			cout << "Frame Rate: " << 1000000 / drawDelay
				<< ", Rate1: " << 1000000 / drawDelay1 / 2
				<< ", Rate2: " << 1000000 / drawDelay2 / 2
				<< endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == '1')
	{
		targetFrameCount1 = 9;		// Delta 3Hz
	}
	if (key == '2')
	{
		targetFrameCount1 = 4;		// Theta 6Hz
	}
	if (key == '3')
	{
		targetFrameCount1 = 2;		// Alpha 10Hz
	}
	if (key == '4')
	{
		targetFrameCount1 = 1;		// Beta 15Hz
	}
	if (key == '5')
	{
		targetFrameCount1 = 0;		// Gamma 30Hz
	}
	if (key == 'q')
	{
		targetFrameCount2 = 9;		// Delta 3Hz
	}
	if (key == 'w')
	{
		targetFrameCount2 = 4;		// Theta 6Hz
	}
	if (key == 'e')
	{
		targetFrameCount2 = 2;		// Alpha 10Hz
	}
	if (key == 'r')
	{
		targetFrameCount2 = 1;		// Beta 15Hz
	}
	if (key == 't')
	{
		targetFrameCount2 = 0;		// Gamma 30Hz
	}
	if (key == 'f')
	{
		ofToggleFullscreen();
	}
	if (key == ' ')
	{
		if (draw1 && draw2)
		{
			draw1 = true;
			draw2 = false;
		}
		else if (draw1)
		{
			draw1 = false;
			draw2 = true;
		}
		else if (draw2)
		{
			draw1 = true;
			draw2 = true;
		}
	}
	if (((char)key) == OF_KEY_BACKSPACE) {
		sendMidi = !sendMidi;
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
		midiout.sendNoteOff(midiChannel, midiId0, midiValue);
		midiout.sendNoteOff(midiChannel, midiId1, midiValue);
		midiout.closePort();
	}

	//hiveMind.waitForThread(true);
}
