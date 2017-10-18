//
//  HiveMindi.cpp
//
//  OpenFrameworks code for the Hive Mind neuro art performance
//
//  Created by Sean Montgomery on 10/06/17.
//
//  This work is licensed under the MIT License
//

#include "HiveMind.h"

HiveMind::HiveMind(int tcpPort)
{
	_openBci.setTcpPort(tcpPort);
	//_openBci.enableDataLogging(ofToDataPath(ofGetTimestampString("%Y-%m-%d-%H-%M-%S") + ".log"));

	_openBci.enableLPFilter(30);

	_nHeadsets = 2;

	_eegChannels = {	// Headset x Channels
		{4},
		{4}
	};

	_eegBands = {		// Bands x Frequencies
		{4, 7},
		{8, 15},
		{16, 31}
	};

	_eegData.resize(_nHeadsets);
	_eegBandData.resize(_nHeadsets);
	_eegBaselineBandData.resize(_nHeadsets);
	for (int h = 0; h < _nHeadsets; h++)
	{
		_eegBandData.at(h).resize(_eegChannels.at(h).size());
		_eegBaselineBandData.at(h).resize(_eegChannels.at(h).size());
		for (int ch = 0; ch < _eegChannels.at(h).size(); ch++)
		{
			_eegBandData.at(h).at(ch).resize(_eegBands.size());
			_eegBaselineBandData.at(h).at(ch).resize(_eegBands.size());
		}
	}
		
	_maxNormBandValue.resize(_nHeadsets);

	_dominantBand1 = -1;
	_dominantBand2 = -1;

	_dataReset = false;
	_dataIsReset = false;

	_smoothingWeight = 0.052f;	//1- 10^(log10(0.2) / (2*15))
	_baselineWeight = 0.0026f;	//1- 10^(log10(0.2) / (2*60*5))

	_fftDelay.resize(_nHeadsets);
	for (int h = 0; h < _nHeadsets; h++)
	{
		_lastFftMillis.push_back(ofGetElapsedTimeMillis());
		_fftDelay.at(h) = -1;
	}
}

HiveMind::~HiveMind() {
	// Stop the thread if it's still running
	waitForThread(true);
}

void HiveMind::setEegChannels(int headset, vector<int> channels)
{
	if (headset < _nHeadsets)
	{
		_eegChannels.at(headset) = channels;
	}
}

void HiveMind::setEegBands(vector<vector<float>> eegBands)
{
	_eegBands = eegBands;
}

void HiveMind::threadedFunction()
{
	while (isThreadRunning()) {

		_openBci.update();

		lock();
		vector<string> ipAddresses = _openBci.getHeadsetIpAddresses();
		for (int h = 0; h < ipAddresses.size() && h < _nHeadsets; h++)
		{
			//cout << _openBci.getStringData(ipAddresses.at(h));
			// ToDo: buffer _eegData
			//_eegData.at(h) = _openBci.getData(ipAddresses.at(h));
			vector<vector<float>> tempData = _openBci.getData(ipAddresses.at(h));
			if (_eegData.at(h).size() < tempData.size())
			{
				_eegData.at(h).resize(tempData.size());
			}
			for (int ch = 0; ch < tempData.size(); ch++)
			{
				for (int n = 0; n < tempData.at(ch).size(); n++)
				{
					// ToDo: Make this less horribly inefficient
					_eegData.at(h).at(ch).push_back(tempData.at(ch).at(n));
				}
			}

			vector<vector<float>> tempFftData;
			vector<int> tempDominantBand;
			tempDominantBand.resize(_nHeadsets);
			if (_openBci.isFftNew(ipAddresses.at(h)))
			{
				tempFftData = _openBci.getLatestFft(ipAddresses.at(h));

				_maxNormBandValue.at(h) = -1;
				tempDominantBand.at(h) = -1;
				if (_dataReset)
				{
					_dataIsReset = true;
				}
				for (int b = 0; b < _eegBands.size(); b++)
				{
					for (int ch = 0; ch < _eegChannels.at(h).size(); ch++)
					{
						int f1 = _openBci.getFftBinFromFrequency(_eegBands.at(b).at(0));
						int f2 = _openBci.getFftBinFromFrequency(_eegBands.at(b).at(1));

						float sum = 0;
						for (int f = f1; f <= f2; f++)
						{
							sum = sum + tempFftData.at(ch).at(f);
						}
						float avg = sum / (1.f + f2 - f1);
						
						// ToDo: Deal with averaging across channels
						if (_dataReset)
						{
							_eegBandData.at(h).at(ch).at(b) = avg;
							_eegBaselineBandData.at(h).at(ch).at(b) = avg;
						}
						else
						{
							_eegBandData.at(h).at(ch).at(b) =
								ofxOpenBciWifi::smooth(avg, _eegBandData.at(h).at(ch).at(b), _smoothingWeight);
							_eegBaselineBandData.at(h).at(ch).at(b) =
								ofxOpenBciWifi::smooth(avg, _eegBaselineBandData.at(h).at(ch).at(b), _baselineWeight);
						}
						float normalizedData = _eegBandData.at(h).at(ch).at(b) / _eegBaselineBandData.at(h).at(ch).at(b);
						if (normalizedData > _maxNormBandValue.at(h))
						{
							_maxNormBandValue.at(h) = normalizedData;
							tempDominantBand.at(h) = b;
						}
					}
				}

				if (h == 0)
				{
					_dominantBand1 = tempDominantBand.at(h);
				}
				if (h == 1)
				{
					_dominantBand2 = tempDominantBand.at(h);
				}

				_fftDelay.at(h) = ofGetElapsedTimeMillis() - _lastFftMillis.at(h);
				_lastFftMillis.at(h) = ofGetElapsedTimeMillis();
				if (_fftDelay.size() > 1)
				{
					_fftDelay1 = _fftDelay.at(0);
					_fftDelay2 = _fftDelay.at(1);
				}
			}
		}

		if (_dataIsReset)
		{
			_dataReset = false;
		}

		unlock();
		sleep(1);
	}
}

float HiveMind::getFftDelay(int headset)
{
	if (headset == 0)
	{
		return _fftDelay1;
	}
	if (headset == 1)
	{
		return _fftDelay2;
	}
	return -1;
}

int HiveMind::getDominantBand(int headset)
{
	if (headset == 0)
	{
		return _dominantBand1;
	}
	if (headset == 1)
	{
		return _dominantBand2;
	}
	return -1;
}

int HiveMind::getFrameCount(int headset, int frameRate)
{
	if (frameRate == 60)
	{
		if (getDominantBand(headset) == 0)	return 4; // Theta
		if (getDominantBand(headset) == 1)	return 2; // Alpha
		if (getDominantBand(headset) == 2)	return 1; // Beta
	}
}

vector<float> HiveMind::getEegBandData(int headset)
{
	lock();
	vector<float> temp = _eegBandData.at(headset).at(0);
	unlock();
	return temp;
}

vector<float> HiveMind::getEegBaselineBandData(int headset)
{
	lock();
	vector<float> temp = _eegBaselineBandData.at(headset).at(0);
	unlock();
	return temp;
}

void HiveMind::resetBandData()
{
	_dataReset = true;
	_dataIsReset = false;
}

vector<vector<float>> HiveMind::getData(int headset)
{
	// ToDo: clear _eegData buffer
	vector<vector<float>> returnData;
	lock();
	returnData = _eegData.at(headset);
	_eegData.at(headset).clear();
	unlock();
	return returnData;
}

vector<vector<float>> HiveMind::getData(string ipAddress)
{
	// ToDo: clear _eegData buffer
	vector<vector<float>> returnData;
	lock();
	vector<string> ipAddresses = _openBci.getHeadsetIpAddresses();
	for (int h = 0; h < ipAddresses.size(); h++)
	{
		if (ipAddress.compare(ipAddresses.at(h)) == 0)
		{
			returnData = _eegData.at(h);
			_eegData.at(h).clear();
		}
	}
	unlock();
	return returnData;
}

void HiveMind::setTcpPort(int port)
{
	_openBci.setTcpPort(port);
}


//vector<vector<int>> goodEegChannels;
//
//void threadedFunction();
//
//public:
//
//	HiveMind();
//	~HiveMind();
//
//	int getDominantBand(int headset);