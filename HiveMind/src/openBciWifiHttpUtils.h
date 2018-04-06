//
//  openBciWifiHttpUtils.h
//
//  Helper functions to send http requests to openBciWifi module
//
//  Created by Sean Montgomery on 10/15/17.
//
//  This work is licensed under the MIT License
//


#pragma once

#include "ofMain.h"

// ToDo: Integrate http methods into ofxOpenBciWifi

static void openBciWifiTcp(string computerIp, int tcpPort, string openBciWifiIp)
{
	string pyCmd = "";
#if defined(TARGET_WIN32) 
	pyCmd = pyCmd + "start "; 
#endif
		pyCmd = pyCmd + "python " + ofToDataPath("httpPostJson.py") + " http://" + openBciWifiIp + "/tcp"
		+ " {"
		+ "\\\"ip\\\":\\\"" + computerIp + "\\\","
		+ "\\\"port\\\":" + ofToString(tcpPort) + ","
		+ "\\\"output\\\":\\\"raw\\\","
		+ "\\\"delimiter\\\":true,"
		+ "\\\"latency\\\":20000,"
		+ "\\\"timestamps\\\":false,"
		+ "\\\"sample_numbers\\\":true"
		+ "}";
#if defined(TARGET_OSX) || defined(TARGET_LINUX) 
		pyCmd = pyCmd + " &";
#endif
	cout << pyCmd << endl;
	system(pyCmd.c_str());
}

static void openBciWifiStart(string openBciWifiIp)
{
	// Start streaming data
	string pyCmd = "";
#if defined(TARGET_WIN32) 
	pyCmd = pyCmd + "start ";
#endif
	pyCmd = pyCmd + "python " + ofToDataPath("httpGet.py") + " http://" + openBciWifiIp + "/stream/start";
#if defined(TARGET_OSX) || defined(TARGET_LINUX) 
	pyCmd = pyCmd + " &";
#endif
	cout << pyCmd << endl;
	system(pyCmd.c_str());
}
static void openBciWifiStop(string openBciWifiIp)
{
	// Stop streaming data
	string pyCmd = "";
#if defined(TARGET_WIN32) 
	pyCmd = pyCmd + "start ";
#endif
	pyCmd = pyCmd + "python " + ofToDataPath("httpGet.py") + " http://" + openBciWifiIp + "/stream/stop";
#if defined(TARGET_OSX) || defined(TARGET_LINUX) 
	pyCmd = pyCmd + " &";
#endif
	cout << pyCmd << endl;
	system(pyCmd.c_str());
}
static void openBciWifiSquareWaveOn(string openBciWifiIp)
{
	// Turn on raw data sending from openBci Cyton boards
	// {'command': '-'}
	string pyCmd = "";
#if defined(TARGET_WIN32) 
	pyCmd = pyCmd + "start ";
#endif
	pyCmd = pyCmd + "python " + ofToDataPath("httpPostJson.py") + " http://" + openBciWifiIp + "/command"
		+ " {"
		+ "\\\"command\\\":\\\"-\\\""
		+ "}";
#if defined(TARGET_OSX) || defined(TARGET_LINUX) 
	pyCmd = pyCmd + " &";
#endif
	cout << pyCmd << endl;
	system(pyCmd.c_str());
}
static void openBciWifiAnalogDataOn(string openBciWifiIp)
{
	// Turn on square wave data sending from openBci Cyton boards
	// {'command': 'd'}
	string pyCmd = "";
#if defined(TARGET_WIN32) 
	pyCmd = pyCmd + "start ";
#endif
	pyCmd = pyCmd + "python " + ofToDataPath("httpPostJson.py") + " http://" + openBciWifiIp + "/command"
		+ " {"
		+ "\\\"command\\\":\\\"d\\\""
		+ "}";
#if defined(TARGET_OSX) || defined(TARGET_LINUX) 
	pyCmd = pyCmd + " &";
#endif
	cout << pyCmd << endl;
	system(pyCmd.c_str());
}