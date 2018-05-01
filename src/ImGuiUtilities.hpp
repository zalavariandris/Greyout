//
//  ImGuiUtilities.hpp
//  ofGO_1_0_eyeclass
//
//  Created by András Zalavári on 2017. 12. 08..
//

#pragma once

#include "ofxImGui.h"
#include "ofMain.h"
#include "ofEye.hpp"
//#include "Profiler.hpp"
#include "GA.hpp"
#include "ofApp.hpp"
namespace ImGui{
    void App(ofApp & app);
    void Eye(ofEye& eye);
//    void Pro(Profiler & profiler);
    void Pop(GA::Population & pop);
}

