#ifndef DUBINSPATH_H
#define DUBINSPATH_H

#include <ldata.h>
#include "DubinsPoint.hpp"

#define MAX_POLYGON_SIZE 5000

enum PathType { RSR, LSL, RSL, LSR, RLR, LRL };

LCoord nLastx;
LCoord nLasty;

class DubinsPath{
private:
    LObject torusStart;
    LObject torusEnd;
    LObject line;
    LObject torusMiddle;
    PathType type;
    float guideWidth;
    
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

    LPoint point_arr[MAX_POLYGON_SIZE];
    int nbPoints;

    LCell cell;
    LFile file;
    LLayer layer;

    LStatus UpdateCircleCenter();

public:
    DubinsPath();
    LStatus SetGuideWidth(float width);
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
    void DrawLine();

    float ComputeRSRLength();
    float ComputeLSLLength();
    float ComputeRSLLength();
    float ComputeLSRLength();
    float ComputeRLRLength();
    float ComputeLRLLength();

    void StoreRSRPath();
    void StoreLSLPath();
    void StoreRSLPath();
    void StoreLSRPath();
    void StoreRLRPath();
    void StoreLRLPath();

    void GetLSLorRSRTangent(LPoint startCenter, LPoint endCenter, bool isBottom);
    void GetRSLorLSRTangent(LPoint startCenter, LPoint endCenter, bool isBottom);
    void GetRLRorLRLTangent(LPoint startCenter, LPoint endCenter, bool isRLR);

    float GetArcLength(LPoint circleCenter, LPoint point, LPoint tangent, bool isLeftCircle);

    void RasterizePath();
    void Add( double x, double y);

};

double PointDistance(LPoint start, LPoint end);
double RoundAngle(double value);
LCoord Round(double d);

#endif