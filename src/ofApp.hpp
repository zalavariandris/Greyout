#pragma once

#include "ofMain.h"
#define BOOST_NO_CXX17_STD_INVOKE
#define BOOST_NO_CXX14_STD_EXCHANGE
#include <boost/coroutine2/all.hpp>
#include "ofxImGui.h"
#include "ofEye.hpp"
#include "GA.hpp"
using namespace boost::coroutines2;
class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void onGui();
    
    // Algorithms
    // a) GA-camera
    void step_GACamera_coroutine(coroutine<void>::pull_type &VSYNC);
    coroutine<void>::push_type step_GACamera {bind(&ofApp::step_GACamera_coroutine, this, placeholders::_1)};
    
    // b) GA-image
    void step_GAImage_coroutine(coroutine<void>::pull_type &VSYNC);
    coroutine<void>::push_type step_GAImage {bind(&ofApp::step_GAImage_coroutine, this, placeholders::_1)};
    
    // Ps3eye
    shared_ptr<ofEye> camera;
    
    // Font
    ofTrueTypeFont font;
    
    // Camera Sync Test
    void camera_sync_test();
    
    // ImGui
    ofxImGui::Gui gui;
    
    // GA
    GA::Population pop;
    shared_ptr<GA::Candidate> best_candidate;
    bool play;
};


