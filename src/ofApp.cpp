#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(100);
    /*-------OSC--------*/
    cout << "listening for osc messages on port " << PORT << "\n";
    receiver.setup(PORT);
    current_msg_string = 0;
    
    /*-------DMX--------*/
    dmx.connect("tty.usbserial-EN189608");
    /*if(dmx.connect(0)){
     cout<<"OK"<<endl;
     }else{
     cout<<"Disconnect"<<endl;
     }*/
    smoke_chan1.set("Channel 1", 0,0,255);
    smoke_chan2.set("Channel 2", 0,0,255);
    smoke_chan3.set("Channel 3", 0,0,255);
    smoke_chan4.set("Channel 4", 0,0,255);
    
    panel.setName("Panel");
    panel.setup();
    panel.add(smoke_chan1);
    panel.add(smoke_chan2);
    panel.add(smoke_chan3);
    panel.add(smoke_chan4);
    
    for(int i=0;i<SMOKE_NUM;i++){
        tag_index[i]=0;
        if(XML.loadFile("../../../xml/smoke"+ofToString(i+1)+".xml")){
            cout<<"read file -> smoke"<<i+1<<".xml"<<endl;
        }
        startTime[i] = XML.getValue("smoke:scene:startTime",0,tag_index[i]);
        endTime[i]=startTime[i]+XML.getValue("smoke:scene:length", 0,tag_index[i]);
        
        int getNum = XML.getNumTags("smoke:scene");
        XML.pushTag("smoke",getNum-1);
        sceneNum[i] = XML.getNumTags("scene");
//        cout<<"sceneNum : "<<sceneNum[i]<<endl;
        XML.popTag();
    }
    threshold = 100;
    bAutoSetting = true;
    
    newScene = false;
    
    
    sTime = 0;
    eTime = 5000;
}

//--------------------------------------------------------------
void ofApp::update(){
    /*--------OSC--------*/
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
        }
    }
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(m.getAddress() == "/sound/time"){
            sceneName = m.getArgAsString(0);
            sceneTime = m.getArgAsInt32(1);
            sequenceAbsoluteTime = m.getArgAsInt32(2);
        }else if(m.getAddress() == "/sound/beat"){
            sceneName = m.getArgAsString(0);
            beatTime = m.getArgAsInt32(1);
        }
        if(temp_sceneName!=sceneName)newScene=true;
        temp_sceneName=sceneName;
    }
    
    
    /*--------XML-----------*/
/*    if(newScene){
        cout<<"newScene"<<endl;
        newScene=false;
        int getNum = XML.getNumTags("smoke:scene");
        XML.pushTag("smoke",getNum-1);
        for(int i=0;i<4;i++){
            bool bStart=false;
            startScene=0;
            endScene=0;
            tag_index[i]=0;
            for(int j=0;j<sceneNum[i];j++){
                string temp = XML.getValue("scene:sceneName", "",j);
                if(!bStart&&temp==sceneName&&startScene==0){
                    startScene=j;
                    bStart=true;
                }
                if(bStart&&endScene==0&&temp!=sceneName){
                    endScene=j-1;
                    bStart=false;
                }
                if(bStart){
                    int tmp=XML.getValue("scene:startTime", 0,j);
                    if(sceneTime-tmp<0&&tag_index[i]==0&&startScene!=j)tag_index[i]=j;
                    cout<<"sceneTime:"<<sceneTime<<"tmp:"<<tmp<<" sum:"<<sceneTime-tmp<<endl;
                    if(i==0)startTimeArray_1.push_back(tmp);
                    else if(i==1)startTimeArray_2.push_back(tmp);
                    else if(i==2)startTimeArray_3.push_back(tmp);
                    else if(i==3)startTimeArray_4.push_back(tmp);
                }
            }
            if(tag_index[i]==0)tag_index[i]=startScene;
            if(endScene==0)endScene=sceneNum[i]-1;
            cout<<"startScene:"<<startScene<<" endScene:"<<endScene<<" now:"<<tag_index[i]<<endl;
            startTime[i]=XML.getValue("scene:startTime", 0,tag_index[i]);
            endTime[i]=startTime[i]+XML.getValue("scene:length", 0,tag_index[i]);
            power[i] = XML.getValue("scene:power", 0,tag_index[i]);
            drawStartEffect[i]=ofMap(startTime[i], sTime, eTime, 400, 950);
            drawEndEffect[i]=ofMap(endTime[i], sTime, eTime, 400, 950);
            
        }
        XML.popTag();
    }*/
    
    //------OLD
    for(int i=0;i<SMOKE_NUM;i++){
        if(sceneTime-threshold<startTime[i]&&sceneTime+threshold>startTime[i]){
            if(XML.loadFile("../../../xml/smoke"+ofToString(i+1)+".xml")){
                cout<<"read file -> smoke"<<i+1<<".xml"<<endl;
            }
            int numTags=XML.getNumTags("smoke:scene");
            XML.pushTag("smoke",numTags-1);
            power[i] = XML.getValue("scene:power", 0,tag_index[i]);
            tag_index[i]++;
            startTime[i]=XML.getValue("scene:startTime", 0,tag_index[i]);
            next_endTime[i] = startTime[i]+XML.getValue("scene:length", 0,tag_index[i]);
            XML.popTag();
            drawStartEffect[i]=ofMap(startTime[i], sTime, eTime, 400, 950);
            drawEndEffect[i]=ofMap(endTime[i], sTime, eTime, 400, 950);
        }
        if(sceneTime-threshold<endTime[i]&&sceneTime+threshold>endTime[i]){
            power[i]=0;
            endTime[i]=next_endTime[i];
        }
    }
    if(bAutoSetting){
        smoke_chan1 = power[0];
        smoke_chan2 = power[1];
        smoke_chan3 = power[2];
        smoke_chan4 = power[3];
    }
    /*--------DMX--------*/
    dmx.setLevel(1, smoke_chan1);
    dmx.setLevel(2, smoke_chan2);
    dmx.setLevel(3, smoke_chan3);
    dmx.setLevel(4, smoke_chan4);
    dmx.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    ofDrawBitmapString("SMOKE", 300, 50);
    ofDrawBitmapString("OSC MESSAGE",20,225);
    ofDrawBitmapString("sceneName : "+sceneName, 20, 250);
    ofDrawBitmapString("sceneTime : "+ofToString(sceneTime), 20, 275);
    ofDrawBitmapString("sequenceAbsoluteTime : "+ofToString(sequenceAbsoluteTime), 20, 300);
    ofDrawBitmapString("beatTime : "+ofToString(beatTime), 20, 325);
    
    ofDrawBitmapString("index", 300, 180);
    ofDrawBitmapString("startTime",300,200);
    ofDrawBitmapString("endTime", 300, 220);
    ofDrawBitmapString("power",300,240);
    if(bAutoSetting){
        ofDrawBitmapString("AUTO", 20, 130);
    }else{
        ofDrawBitmapString("MANUAL", 20, 130);
    }
    ofDrawBitmapString("threshold:"+ofToString(threshold),20,150);
    for(int i=0;i<SMOKE_NUM;i++){
        ofDrawBitmapString(tag_index[i],450+i*150,180);
        ofDrawBitmapString(startTime[i], 450+i*150, 200);
        ofDrawBitmapString(endTime[i], 450+i*150, 220);
        ofDrawBitmapString(power[i], 450+i*150, 240);
    }
    
    ofSetColor(smoke_chan1);
    ofDrawRectangle(400,60,100,100);
    ofSetColor(smoke_chan2);
    ofDrawRectangle(550,60, 100, 100);
    ofSetColor(smoke_chan3);
    ofDrawRectangle(700,60, 100, 100);
    ofSetColor(smoke_chan4);
    ofDrawRectangle(850,60, 100, 100);
    panel.draw();
    
    ofSetColor(255);
    ofDrawRectangle(400, 270, 550, 10);
    ofDrawRectangle(400, 284, 550, 10);
    ofDrawRectangle(400, 298, 550, 10);
    ofDrawRectangle(400, 312, 550, 10);
    ofDrawBitmapString(sTime, 400, 335);
    ofDrawBitmapString(eTime, 950, 335);
    while(eTime<sceneTime){
        eTime+=5000;
        sTime+=5000;
    }
    nowTimePos = ofMap(sceneTime, sTime, eTime, 400, 950);
    ofSetColor(0);
    ofDrawRectangle(nowTimePos, 270, 5, 52);
    for(int i=0;i<SMOKE_NUM;i++){
        if(drawEndEffect[i]-drawStartEffect[i]>0){
            ofSetColor(255, 0, 0);
            ofDrawRectangle(drawStartEffect[i], 270+i*14,drawEndEffect[i]-drawStartEffect[i] , 10);
        }
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='a'){
        if(bAutoSetting)bAutoSetting=false;
        else bAutoSetting=true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
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

