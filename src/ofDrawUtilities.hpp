//
//  ofDrawUtilities.hpp
//  ofGO_1_0_eyeclass
//
//  Created by András Zalavári on 2017. 12. 08..
//

#pragma once
#include "ofMain.h"

// extend OpenFrameworks functions
void ofDrawMeshString(const ofTrueTypeFont & font, string text, float x, float y, float z);

// stripes
void draw_test_stripes(shared_ptr<ofBaseGLRenderer> gl, ofVec2f size, float deg);
shared_ptr<ofImage> make_image(ofVec2f size, const std::function<void(int w, int h)>& f);
float compare_images(const ofImage & A, const ofImage & B);
float compare_brightness(const ofImage & A, const ofImage & B);
float compare_brightness_to_grey(const ofImage & A);

//
ofVec2f bw_size(size_t length);
void draw_bw(int* data, size_t length, float x, float y, float w, float h);
ofVec2f rgb_size(size_t length, size_t channels);
void draw_rgb(int* data, size_t length, float x, float y, float w, float h);



