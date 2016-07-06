#pragma once

#include "ofMain.h"
#include "ofxDmx.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

/*----OSC----*/
#define PORT 8000
#define NUM_MSG_STRINGS 20
#define SMOKE_NUM 4

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
    
    ofxDmx dmx;
    int level;
    
    ofxPanel panel;
    ofParameter<int> smoke_chan1;
    ofParameter<int> smoke_chan2;
    ofParameter<int> smoke_chan3;
    ofParameter<int> smoke_chan4;    

    ofxOscReceiver receiver;
    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
		
    string sceneName;
    int sceneTime;
    int sequenceAbsoluteTime;
    int beatTime;
    
    ofxXmlSettings XML;
    int numTags;
    int tag_index[SMOKE_NUM];
    int startTime[SMOKE_NUM];
    int endTime[SMOKE_NUM];
    int next_endTime[SMOKE_NUM];
    int power[SMOKE_NUM];
    int threshold;
    bool bAutoSetting;
    
    bool newScene;
    int sceneNum[SMOKE_NUM];
    int startScene,endScene;
    string temp_sceneName;
    vector<int> startTimeArray_1;
    vector<int> startTimeArray_2;
    vector<int> startTimeArray_3;
    vector<int> startTimeArray_4;
    int index;
    
    int nowTimePos;
    int sTime,eTime;
    int drawStartEffect[SMOKE_NUM];
    int drawEndEffect[SMOKE_NUM];
    
};
