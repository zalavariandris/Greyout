//#define BOOST_NO_AUTO_PTR
#include "ofMain.h"
#include "ofApp.hpp"

//========================================================================
int main( ){
	ofSetupOpenGL(1100, 700, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}
