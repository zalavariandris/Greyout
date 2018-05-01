//
//  ofDrawUtilities.cpp
//  ofGO_1_0_eyeclass
//
//  Created by András Zalavári on 2017. 12. 08..
//

#include "ofDrawUtilities.hpp"

void ofDrawMeshString(const ofTrueTypeFont & font, string text, float x, float y, float z){
    vector<ofTTFCharacter> letters = font.getStringAsPoints(text);
    ofPushMatrix();
    ofTranslate(x, y, z);
    for(auto letter : letters){
        ofVboMesh mesh = letter.getTessellation();
        mesh.draw();
    }
    ofPopMatrix();
}

void draw_test_stripes(shared_ptr<ofBaseGLRenderer> gl, ofVec2f size, float deg){
    // draw stripes
    {
        int stripe_count = 10;
        float stripe_width = size.x/stripe_count/2;
        float stripe_height = ofVec2f(size.x, size.y).length();
        
        for(int i=0; i<stripe_count*2; i++){
            ofVec2f stripe_pos = ofVec2f(1.0 * i * size.x/stripe_count - size.x , 0);
            stripe_pos.rotate(deg);
            stripe_pos += ofVec2f(size.x/2, size.y/2);
            
            /*draw a stripe*/
            {
                ofPath stripe_path;
                stripe_path.moveTo(-1, 1);
                stripe_path.lineTo( 1, 1);
                stripe_path.lineTo( 1,-1);
                stripe_path.lineTo(-1,-1);
                stripe_path.scale(stripe_width/2, stripe_height/2);
                stripe_path.rotate(deg, ofVec3f(0, 0, 1));
                stripe_path.translate(ofPoint(stripe_pos.x, stripe_pos.y));
                gl->draw(stripe_path);
            }
        }
    }
}

shared_ptr<ofImage> make_image(ofVec2f size, const std::function<void(int w, int h)>& f){
    
    ofFbo fbo;
    fbo.allocate(size.x, size.y);
    
    fbo.begin();
        ofClear(0,255,0,255);
        f(size.x, size.y);
    fbo.end();
    
    shared_ptr<ofImage> image = make_shared<ofImage>();
    fbo.readToPixels(*image);
    image->update();
    
    return image;
}

ofVec2f bw_size(size_t length){
    size_t square = length;
    size_t width = sqrt(square);
    return ofVec2f(width);
}

void draw_bw(int * data, size_t length, float x, float y, float w, float h){
    int CHANNELS = 3;
    auto size = bw_size(length);
    
    uint8_t * data2 = new uint8_t[length];
    for(int y=0; y < size.y; y++)
        for(int x=0; x < size.x; x++){
            int rgb_index = y*size.x + x;
            data2[rgb_index + 0] = data[rgb_index+0];
        }
    
    ofImage img;
    img.allocate(size.x, size.y, OF_IMAGE_GRAYSCALE);
    img.setFromPixels(data2, size.x, size.y, OF_IMAGE_GRAYSCALE);
    img.update();
    img.draw(x, y, w, h);
    delete[] data2;
}

ofVec2f rgb_size(size_t length, size_t channels){
    size_t square = length / channels;
    size_t width = sqrt(square);
    return ofVec2f(width);
}

void draw_rgb(int * data, size_t length, float x, float y, float w, float h){
    int CHANNELS = 3;
    auto size = rgb_size(length, CHANNELS);
    
    uint8_t * data2 = new uint8_t[length];
    for(int y=0; y < size.y; y++)
        for(int x=0; x < size.x; x++){
            int rgb_index = y*size.x*CHANNELS + x*CHANNELS;
            data2[rgb_index + 0] = data[rgb_index+0];
            data2[rgb_index + 1] = data[rgb_index+1];
            data2[rgb_index + 2] = data[rgb_index+2];
        }
    
    ofImage img;
    img.allocate(size.x, size.y, OF_IMAGE_COLOR);
    img.setFromPixels(data2, size.x, size.y, OF_IMAGE_COLOR);
    img.update();
    img.draw(x, y, w, h);
    delete[] data2;
}

float compare_images(const ofImage & A, const ofImage & B){
    //    assert(A.getWidth() == B.getWidth() && A.getHeight() == B.getHeight());
    ofPixels pixelsA = A.getPixels();
    ofPixels pixelsB = B.getPixels();
    
    float diff {0};
    for(size_t x=0; x<pixelsA.getWidth(); x++){
        for(size_t y=0; y<pixelsA.getHeight(); y++){
            ofColor colorA = pixelsA.getColor(x, y);
            ofColor colorB = pixelsB.getColor(x, y);
            diff+=abs((int)colorB.r - (int)colorA.r);
            diff+=abs((int)colorB.g - (int)colorA.g);
            diff+=abs((int)colorB.b - (int)colorA.b);
        }
    }
    diff = diff / (3 * A.getWidth() * A.getHeight());
    
    return diff;
}

float compare_lightness(const ofImage & A, const ofImage & B){
    //    assert(A.getWidth() == B.getWidth() && A.getHeight() == B.getHeight());
    ofPixels pixelsA = A.getPixels();
    ofPixels pixelsB = B.getPixels();
    
    float diff {0};
    for(size_t x=0; x<pixelsA.getWidth(); x++){
        for(size_t y=0; y<pixelsA.getHeight(); y++){
            ofColor colorA = pixelsA.getColor(x, y);
            ofColor colorB = pixelsB.getColor(x, y);
            
            diff+= abs((int)colorB.getLightness() - (int)colorA.getLightness());
        }
    }
    diff = diff / (A.getWidth() * A.getHeight());
    
    return diff;
}

float compare_lightness_to_grey(const ofImage & A){
    //    assert(A.getWidth() == B.getWidth() && A.getHeight() == B.getHeight());
    ofPixels pixelsA = A.getPixels();
    
    float diff {0};
    for(size_t x=0; x<pixelsA.getWidth(); x++){
        for(size_t y=0; y<pixelsA.getHeight(); y++){
            ofColor colorA = pixelsA.getColor(x, y);
            diff += (int)colorA.getLightness();
        }
    }
    diff = diff / ( A.getWidth() * A.getHeight());
    
    return diff;
}
