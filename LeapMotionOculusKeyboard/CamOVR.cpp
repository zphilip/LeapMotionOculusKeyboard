//
//  CamOVR.cpp
//  LeapMotionOculusKeyboard
//
//  Created by Grin on 14.01.14.
//  Copyright (c) 2014 Fachbereich Informatik. All rights reserved.
//

#include "CamOVR.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <glm/gtx/transform.hpp>
#include <iostream>

CamOVR::CamOVR(){
    eye=glm::vec3(0.0f,0.17f, 0.1f); //initial value, player is 1m 80cm tall and stands 1m away from origin
    center=glm::vec3(0.0f,0.0f,0.0f);
    up=glm::vec3(0.0f,1.0f,0.0f);
    hv=glm::vec2(180.0f,90.0f);//and he is looking straingt forward, slightly down
    r=1.0f;
    
    rot_cw=glm::rotate(90.0f, 0.0f, 1.0f, 0.0f);
    rot_ccw=glm::rotate(-90.0f, 0.0f, 1.0f, 0.0f);
    
    fix=hv;
    first=true;
    std::cout << ".";
}
CamOVR::~CamOVR(){
}

void CamOVR::move(glm::vec3 in){
    eye+=in;
}
void CamOVR::ovrInput(glm::vec3 yawPitchRoll){
    if (first){
        oculusZero=yawPitchRoll;
        first=false;
    }
    //std::cout << yawPitchRoll.x << std::endl;
    yawPitchRoll-=oculusZero;
    yawPitchRoll.x*=(-1.0);
    yawPitchRoll.z*=(-1.0);
    hv.x=fix.x+yawPitchRoll.x;
    hv.y=fix.y+yawPitchRoll.y;
    
    float tmp = yawPitchRoll.z;
    static float lastZ;
    yawPitchRoll.z-=lastZ;
    lastZ=tmp;
    glm::mat4 rMat=glm::rotate(yawPitchRoll.z, 0.0f, 0.0f, 1.0f);
    
    glm::vec4 tUp(up.x,up.y,up.z,0.0f);
    tUp=rMat*tUp;
    up=glm::vec3(tUp.x,tUp.y,tUp.z);
    
}
void CamOVR::orientation(glm::vec2 in){
    hv+=in;
    hv=glm::vec2(std::fmodf(hv.x, 360.0f),std::fmodf(hv.y, 360.0f));
    
    fix+=in;
    fix=glm::vec2(std::fmodf(fix.x, 360.0f),std::fmodf(fix.y, 360.0f));
}
void CamOVR::mouseMove(glm::vec2 in){
    in.x=in.x*(-1);
    float sensitivity = 3.0f;
    in.x=in.x/sensitivity;
    in.y=in.y/sensitivity;
    
    if (oldMPos!=glm::vec2(0.0f,0.0f)) {
        orientation(glm::vec2(in.x-oldMPos.x,in.y-oldMPos.y));
        oldMPos=in;
    }else{
        oldMPos=in;
    }
}
void CamOVR::yaw(float in){
    
}
void CamOVR::setIPD(float in){
    IPD=in;
}

glm::vec3 CamOVR::getEye(){
    return eye;
}
glm::vec3 CamOVR::getCenter(){
    calculateCenter();
    return center+eye;
}
glm::vec3 CamOVR::getUp(){
    return up;
}
glm::mat4 CamOVR::getLookAt(){
    return glm::lookAt(getEye(), getCenter(), getUp());
}
glm::mat4 CamOVR::getLookAtL(){
    glm::vec4 direction(center.x,0.0f,center.z,0.0f);
    glm::vec4 offset=rot_ccw*direction;
    float scale=((IPD*0.5)/sqrtf(offset.x*offset.x+offset.z*offset.z));
    offset=glm::scale(scale, 0.0f, scale)*offset;
    
    return glm::translate(offset.x, offset.y, offset.z)*getLookAt();
}
glm::mat4 CamOVR::getLookAtR(){
    glm::vec4 direction(center.x,0.0f,center.z,0.0f);
    glm::vec4 offset=rot_cw*direction;
    float scale=((IPD*0.5)/sqrtf(offset.x*offset.x+offset.z*offset.z));
    offset=glm::scale(scale, 0.0f, scale)*offset;
    
    return glm::translate(offset.x, offset.y, offset.z)*getLookAt();
}
void CamOVR::calculateCenter(void){
    center.x=std::sinf(hv.y/180.0*M_PI)*std::sinf(hv.x/180*M_PI)*r;
    center.y=std::cosf(hv.y/180.0*M_PI)*r;
    center.z=std::sinf(hv.y/180.0*M_PI)*std::cosf(hv.x/180*M_PI)*r;
}

std::string CamOVR::getData(){
    std::stringstream output;
    
    output << "CamOVR: h: " << hv.x << " v: " << hv.y << " eye.x: " << eye.x << " eye.y: " << eye.y << " eye.z: " << eye.z;
    return output.str();
}

void CamOVR::setMouseDown(bool in){
    mouseIsDown=in;
    if (in==false) {
        oldMPos=glm::vec2(0.0f,0.0f);
    }
}