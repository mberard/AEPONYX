#include "DubinsPoint.hpp"
#include <math.h>

DubinsPoint::DubinsPoint(){}

DubinsPoint::DubinsPoint(double x, double y, float angle, LFile pFile){
    angle = angle * M_PI / 180.0;
    this->point.x = LFile_MicronsToIntU(pFile, x);
    this->point.y = LFile_MicronsToIntU(pFile, y);
    this->angle = angle;
}

LStatus DubinsPoint::SetPoint(double x, double y, LFile pFile){
    this->point.x = LFile_MicronsToIntU(pFile, x);
    this->point.y = LFile_MicronsToIntU(pFile, y);
    return LStatusOK;
}

LStatus DubinsPoint::SetPointInInternalUnits( double x , double y ){
    this->point.x = x;
    this->point.y = y;
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

DPoint DubinsPoint::GetPoint(){
    return this->point;
}

LPoint DubinsPoint::GetLPoint(){
    LPoint returnValue;
    returnValue.x = (LCoord)this->point.x;
    returnValue.y = (LCoord)this->point.y;
    return returnValue;
}

float DubinsPoint::GetAngleRadian(){
    return this->angle;
}

float DubinsPoint::GetAngleDegre(){
    return this->angle*180.0/M_PI;
}