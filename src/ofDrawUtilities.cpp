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

shared_ptr<ofTexture> make_texture(ofVec2f size, const std::function<void(int w, int h)>& f){
    ofFbo fbo;
    fbo.allocate(size.x, size.y, GL_RGB);
    
    fbo.begin();
        ofClear(0,255,0,255);
        f(size.x, size.y);
    fbo.end();

    return std::make_shared<ofTexture>(std::move(fbo.getTexture()));
}

ofVec2f bw_size(size_t length){
    size_t square = length;
    size_t width = sqrt(square);
    return ofVec2f(width);
}

void draw_bw(int * data, size_t length, float x, float y, float w, float h){
    int CHANNELS = 3;
    auto size = bw_size(length);
    
    unsigned char data2[length];
    for(int y=0; y < size.y; y++)
        for(int x=0; x < size.x; x++){
            int rgb_index = y*size.x + x;
            data2[rgb_index + 0] = data[rgb_index+0];
        }
    
    ofTexture tex;
    tex.loadData(data2, (int)size.x, (int)size.y, GL_LUMINANCE);
    cout << "draw greyscale" << endl;
    tex.draw(x, y, w, h);
}

ofVec2f rgb_size(size_t length, size_t channels){
    size_t square = length / channels;
    size_t width = sqrt(square);
    return ofVec2f(width);
}

void draw_rgb(int * data, size_t length, float x, float y, float w, float h){
    int CHANNELS = 3;
    auto size = rgb_size(length, CHANNELS);
    
    unsigned char data2[length];
    for(int y=0; y < size.y; y++)
        for(int x=0; x < size.x; x++){
            int rgb_index = y*size.x*CHANNELS + x*CHANNELS;
            data2[rgb_index + 0] = data[rgb_index+0];
            data2[rgb_index + 1] = data[rgb_index+1];
            data2[rgb_index + 2] = data[rgb_index+2];
        }
    
    ofTexture tex;
    tex.loadData(data2, size.x, size.y, GL_RGB);
    tex.draw(x,y,w,h);

}

float compare_textures(const ofTexture & A, const ofTexture & B){
    //    assert(A.getWidth() == B.getWidth() && A.getHeight() == B.getHeight());
    ofPixels pixelsA;
    A.readToPixels(pixelsA);
    ofPixels pixelsB;
    B.readToPixels(pixelsB);
    
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

float compare_brightness(const ofTexture & A, const ofTexture & B){
    //    assert(A.getWidth() == B.getWidth() && A.getHeight() == B.getHeight());
    ofPixels pixelsA;
    A.readToPixels(pixelsA);
    ofPixels pixelsB;
    B.readToPixels(pixelsB);
    
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

float compare_brightness_to_grey(const ofTexture & A){
    //    assert(A.getWidth() == B.getWidth() && A.getHeight() == B.getHeight());
    ofPixels pixelsA;
    A.readToPixels(pixelsA);
    
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
