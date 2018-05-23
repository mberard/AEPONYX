#ifndef DUBINSPATH_H
#define DUBINSPATH_H

#include <ldata.h>
#include "DubinsPoint.hpp"

enum PathType { RSR, LSL, RSL, LSR, RLR, LRL };

class DubinsPath{
private:
    LObject torusStart;
    LObject torusEnd;
    LObject line;
    PathType type;
    DubinsPoint startPoint;
    DubinsPoint endPoint;
    float distance;

    float radius;
    LPoint centerStartLeftCircle;
    LPoint centerStartRightCircle;
    LPoint centerEndLeftCircle;
    LPoint centerEndRightCircle;

    LPoint startTangent;
    LPoint endTangent;

    LStatus UpdateCircleCenter();

    LCell cell;
    LFile file;
    LLayer layer;

public:
    DubinsPath();
    LStatus SetPathType(PathType type);
    LStatus SetStartPoint(DubinsPoint startPoint);
    LStatus SetEndPoint(DubinsPoint endPoint);
    LStatus SetDistance(float distance);
    LStatus SetRadius(float radius);
    LStatus SetFile(LFile file);
    LStatus SetCell(LCell cell);
    LStatus SetLayer(LLayer layer);
    LFile GetFile();
    LCell GetCell();
    LLayer GetLayer();

};

#endif