#ifndef DUBINSPOINT_H
#define DUBINSPOINT_H

#include <ldata.h>

class DubinsPoint{
private:
    LPoint point;
    float angle;

public:
    DubinsPoint();
    DubinsPoint(LPoint point, float angle);
    LStatus SetPoint(LPoint point);
    LStatus SetAngleDegre(float angle);
    LStatus SetAngleRadian(float angle);
    LPoint GetPoint();
    float GetAngleDegre();
    float GetAngleRadian();

};


#endif