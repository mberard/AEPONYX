#include "DubinsPoint.hpp"
#include <math.h>

DubinsPoint::DubinsPoint(){}

DubinsPoint::DubinsPoint(LPoint point, float angle){
    angle = angle * M_PI / 180.0;
    this->point = point;
    this->angle = angle;
}

LStatus DubinsPoint::SetPoint(LPoint point){
    this->point = point;
    return LStatusOK;
}

LStatus DubinsPoint::SetAngleDegre(float angle){
    if( angle >= 0 && angle <= 360 ){
        angle = angle * M_PI / 180.0;
        this->angle = angle;
        return LStatusOK;
    }
    else
        return LBadParameters;
}

LStatus DubinsPoint::SetAngleRadian(float angle){
    if( angle >= 0 && angle <= 2*M_PI ){
        this->angle = angle;
        return LStatusOK;
    }
    else
        return LBadParameters;
}

LPoint DubinsPoint::GetPoint(){
    return this->point;
}

float DubinsPoint::GetAngleRadian(){
    return this->angle;
}

float DubinsPoint::GetAngleDegre(){
    return this->angle*180.0/M_PI;
}