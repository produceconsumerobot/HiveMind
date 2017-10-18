//
//  HiveMindi.h
//
//  OpenFrameworks code for the Hive Mind neuro art performance
//
//  Created by Sean Montgomery on 10/06/17.
//
//  This work is licensed under the MIT License
//

#pragma once

#include "ofMain.h"
#include "ofxOpenBciWifi.h"

class HiveMind : public ofThread
{
private:
	//float smoothTheta;
	//float baselineTheta;
	//float smoothAlpha;
	//float baselineAlpha;
	//float smoothBeta;
	//float baselineBeta;

	ofxOpenBciWifi _openBci;

	int _nHeadsets = 2;

	vector<vector<int>> _eegChannels;					// Headset x Channels
	vector<vector<float>> _eegBands;					// Headset x EEG Bands x Freq Min/Max
	vector<vector<vector<float>>> _eegBandData;			// Headset x Channels x Bands
	vector<vector<vector<float>>> _eegBaselineBandData;	// Headset x Channels x Bands
	vector<vector<vector<float>>> _eegData;

	vector<float> _maxNormBandValue;
	int _dominantBand1;
	int _dominantBand2;

	float _smoothingWeight;
	float _baselineWeight;

	vector<uint64_t> _lastFftMillis;
	vector<float> _fftDelay;
	float _fftDelay1;
	float _fftDelay2;

	void threadedFunction();

	bool _dataReset;
	bool _dataIsReset;

public:

	HiveMind(int tcpPort = 3000);
	~HiveMind();

	//float getFftDelay(int headset);
	float getFftDelay(int headset);

	void setEegChannels(int headset, vector<int> channels);
	void setEegBands(vector<vector<float>> eegBands);

	int getDominantBand(int headset);
	int getFrameCount(int headset, int frameRate);

	vector<float> getEegBandData(int headset);
	vector<float> getEegBaselineBandData(int headset);

	vector<vector<float>> getData(string ipAddress);
	vector<vector<float>> HiveMind::getData(int headset);

	void setTcpPort(int port);

	void resetBandData();
};
