#ifndef DUBINSPATH_H
#define DUBINSPATH_H

#include <ldata.h>
#include "DubinsPoint.hpp"
#include "BezierCurve.hpp"

#define MAX_POLYGON_SIZE 10000
#define ANGLE_LIMIT 1.5

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

    LObject torusStart_saved;
    LObject torusEnd_saved;
    LObject line_saved;
    LObject torusMiddle_saved;
    
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

    bool offsetCurveIsSelected;
    double offsetValue;
	
	double WGGROW003SizeValue;
	double WGOVL010SizeValue;
	double WGOVLHOLESizeValue;
	
    double oxideSizeValue;
    LLayer oxideLayer;

    LPoint point_arr[MAX_POLYGON_SIZE];
    int nbPoints;

    double paramBezier;

    LCell cell;
    LFile file;
    LLayer layer;
	
	LLayer WGGROW003Layer, WGOVL010Layer, WGOVLHOLELayer;

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
	LStatus SetWGGROW003Layer(LLayer layer);
	LStatus SetWGOVL010Layer(LLayer layer);
	LStatus SetWGOVLHOLELayer(LLayer layer);
    LStatus SetOffsetCurveIsSelected(bool choice);
    LStatus SetOffsetValue(double value);
	LStatus SetWGGROW003SizeValue(double value);
	LStatus SetWGOVL010SizeValue(double value);
	LStatus SetWGOVLHOLESizeValue(double value);
    LStatus SetOxideSizeValue(double value);
    LStatus SetOxideLayer(LLayer layer);
    LStatus SetParamBezier(double paramBezier);

    double GetOxideSizeValue();

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

    void RasterizePath(bool needToRasterize);
    void DrawArc(LPoint center, LCoord radius, double startAngle, double stopAngle, bool isCCW);
    void Add( double x, double y);

    void DubinsPathWithBezierCurves();
	void DubinsPathWithBezierCurvesCall();

};

double RoundAngle(double value);
double Round0or5(double val);

#endif