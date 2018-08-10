#ifndef DUBINSPOINT_H
#define DUBINSPOINT_H

#include <ldata.h>

class DubinsPoint{
private:
    DPoint point;
    float angleRadian;
    float angleDegre;

public:
    DubinsPoint();
    DubinsPoint(double x, double y, float angle, LFile pFile);
    LStatus SetPoint(double x, double y, LFile pFile);
    LStatus SetPointInInternalUnits( double x , double y );
    LStatus SetAngleDegre(float angle);
    LStatus SetAngleRadian(float angle);
    DPoint GetPoint();
    LPoint GetLPoint();
    float GetAngleDegre();
    float GetAngleRadian();

};


#endif