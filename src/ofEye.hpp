//
//  Camera.hpp
//  Greyout
//
//  Created by András Zalavári on 2018. 04. 30..
//

#pragma once
#include "ofMain.h"
#include "ps3eye.h"
class ofEye{
public:
    void setup();
    void update();
    shared_ptr<ofImage> buffer;
    
    // properties
    bool getAutogain() const;
    void setAutogain(bool val);
    bool getAutoWhiteBalance() const;
    void setAutoWhiteBalance(bool val);
    uint8_t getGain() const;
    void setGain(uint8_t val);
    uint8_t getExposure() const;
    void setExposure(uint8_t val);
    uint8_t getSharpness() const;
    void setSharpness(uint8_t val);
    uint8_t getContrast() const;
    void setContrast(uint8_t val);
    uint8_t getBrightness() const;
    void setBrightness(uint8_t val);
    uint8_t getHue() const;
    void setHue(uint8_t val);
    uint8_t getRedBalance() const;
    void setRedBalance(uint8_t val);
    uint8_t getGreenBalance() const;
    void setGreenBalance(uint8_t val);
    uint8_t getBlueBalance() const;
    void setBlueBalance(uint8_t val);
    bool getFlipH() const;
    bool getFlipV() const;
    void setFlip(bool horizontal=false, bool vertical = false);
private:
    shared_ptr<ofImage> camera_image = make_shared<ofImage>();
    ps3eye::PS3EYECam::PS3EYERef device;
};

