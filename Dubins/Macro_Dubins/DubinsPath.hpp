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
    LPoint centerMiddleCircle;

    LPoint startTangent;
    LPoint endTangent;


    LCell cell;
    LFile file;
    LLayer layer;
    
    LStatus UpdateCircleCenter();

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

    void ComputeDubinsPaths();

    float ComputeRSRLength();
    float ComputeLSLLength();
    float ComputeRSLLength();
    float ComputeLSRLength();
    float ComputeRLRLength();
    float ComputeLRLLength();

    void GetLSLorRSRTangent(LPoint startCenter, LPoint endCenter, bool isBottom);
    void GetRSLorLSRTangent(LPoint startCenter, LPoint endCenter, bool isBottom);
    void GetRLRorLRLTangent(LPoint startCenter, LPoint endCenter, bool isRLR);

    float GetArcLength(LPoint circleCenter, LPoint point, LPoint tangent, bool isLeftCircle);

};

double PointDistance(LPoint start, LPoint end);

#endif