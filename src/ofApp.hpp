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
    void decode(shared_ptr<GA::Candidate> candidate);
    void evaluate(shared_ptr<GA::Candidate> candidate, shared_ptr<ofTexture> environment);
    
    // properties
    enum Mutation:int {MutationUniform, MutationTweak};
    enum Crossover:int {CrossoverUniform};
    enum Decode:int {DecodeColor, DecodeGreyscale, DecodeIndex};
    enum Evaluate:int {EvaluateCompareColor, EvaluateCompareGreyscale, EvaluateCaptureGreyness};
    
    int mutationMethod{MutationUniform};
    int crossoverMethod{CrossoverUniform};
    int decodeMethod{DecodeGreyscale};
    int evaluateMethod{EvaluateCompareGreyscale};
    
    int cameraLatency{4};
    
    vector<float> best_history;
    ofRectangle camera_clip{0.0f,0.0f,320.0f,240.0f};
    ofRectangle projection_rect{0.0f,0.0f,320.0f, 240.0f};
};


