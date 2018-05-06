//
//  Camera.cpp
//  Greyout
//
//  Created by András Zalavári on 2018. 04. 30..
//

#include "ofEye.hpp"
void ofEye::setup(){
    buffer = make_shared<ofTexture>();
}
void ofEye::update(){
    if(!device){
        // search for ps3eye
        if(ps3eye::PS3EYECam::getDevices(true).size()>0){
            device = ps3eye::PS3EYECam::getDevices().at(0);
            device->init(320, 240, 60, ps3eye::PS3EYECam::EOutputFormat::RGB);
            device->start();
            device->setAutogain(true);
        }else{
            // if no camera found yet, show a noisy image
            unsigned char data[320 * 240 * 3];
            for(int i=0; i<320 * 240 * 3; i+=3)
                data[i+0] = data[i+1] = data[i+2] = rand()%255;
            
            buffer->loadData(data, 320, 240, GL_RGB);
            
        }
    }
    
    if (device){
        // capture camera to image
        unsigned char * data = new unsigned char[device->getWidth()*device->getHeight()*3];
        device->getFrame(data);
        buffer->loadData(data, 320, 240, GL_RGB);
        delete[] data;
    }
}

/* Properties */
bool ofEye::getAutogain() const{
    return device ? device->getAutogain() : false;
}
void ofEye::setAutogain(bool val){
    if(device)
        device->setAutogain(val);
}
bool ofEye::getAutoWhiteBalance() const{
    return device ? device->getAutoWhiteBalance() : 0;
}
void ofEye::setAutoWhiteBalance(bool val){
    if(device)
        device->setAutoWhiteBalance(val);
}
uint8_t ofEye::getGain() const{
    return device ? device->getGain() : 0;
}
void ofEye::setGain(uint8_t val){
    if(device)
        device->setGain(val);
}
uint8_t ofEye::getExposure() const{
    return device ? device->getExposure() : 0;
}
void ofEye::setExposure(uint8_t val){
    if(device)
        device->setExposure(val);
}
uint8_t ofEye::getSharpness() const{
    return device ? device->getSharpness() : 0;
}
void ofEye::setSharpness(uint8_t val){
    if(device)
        device->setSharpness(val);
}
uint8_t ofEye::getContrast() const{
    return device ? device->getSharpness() : 0;
}
void ofEye::setContrast(uint8_t val){
    if(device)
        device->setContrast(val);
}
uint8_t ofEye::getBrightness() const{
    return device ? device->getBrightness() : 0;
}
void ofEye::setBrightness(uint8_t val){
    if(device)
        device->setBrightness(val);
}
uint8_t ofEye::getHue() const{
    return device ? device->getHue() : 0;
}
void ofEye::setHue(uint8_t val){
    if(device)
        device->setHue(val);
}
uint8_t ofEye::getRedBalance() const{
    return device ? device->getRedBalance() : 0;
}
void ofEye::setRedBalance(uint8_t val){
    if(device)
        device->setRedBalance(val);
}
uint8_t ofEye::getGreenBalance() const{
    return device ? device->getGreenBalance() : 0;
}
void ofEye::setGreenBalance(uint8_t val){
    if(device)
        device->setGreenBalance(val);
}
uint8_t ofEye::getBlueBalance() const{
    return device ? device->getBlueBalance() : 0;
}
void ofEye::setBlueBalance(uint8_t val){
    if(device)
        device->setBlueBalance(val);
}

bool ofEye::getFlipH() const{
    return device ? device->getFlipH() : false;
}
bool ofEye::getFlipV() const{
    return device ? device->getFlipV() : false;
}
void ofEye::setFlip(bool horizontal, bool vertical){
    if(device)
        device->setFlip(horizontal, vertical);
}

