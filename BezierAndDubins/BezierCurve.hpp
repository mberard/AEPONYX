#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <ldata.h>
#include "DubinsPoint.hpp"

#define EXCLUDE_LEDIT_LEGACY_UPI //This statement make the C language macros, which are now superseded by C++ functions, unavailable.

#define WIDTH 0.2
#define ANGLE_LIMIT 1.5
#define MAX_POLYGON_SIZE_BEZIER 10000

class BezierCurve{
private:
    LCell cell;
    LFile file;
    LLayer layer;

    DubinsPoint startPoint; //position and angle
    DubinsPoint endPoint; //position and angle
    LPoint controlStart; //compute
    LPoint controlEnd; //compute

    LPoint curve_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPointsCurve;
    LPoint point_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPoints;

    double guideWidth;

    double oxideSizeValue;
    LLayer oxideLayer;

    double paramBezier;

public:
    BezierCurve();
    LStatus SetFile(LFile file);
    LStatus SetCell(LCell cell);
    LStatus SetLayer(LLayer layer);
    LStatus SetStartPoint(DubinsPoint point);
    LStatus SetEndPoint(DubinsPoint point);
    LStatus SetGuideWidth(double width);
    LStatus SetOxideSizeValueBezier(double value);
    LStatus SetOxideLayerBezier(LLayer layer);
    LStatus SetParamBezier(double paramBezier);

    void ComputeBezierCurve();

};

long Round0or5ToLong(double val);
long RoundToLong(double value);
double PointDistance(LPoint start, LPoint end);
double ArrayDistance(LPoint* arr, int nbPoints);

#endif