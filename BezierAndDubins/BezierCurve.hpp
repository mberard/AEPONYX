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

    DubinsPoint startPoint;
    DubinsPoint endPoint;
    LPoint controlStart;
    LPoint controlEnd;

    LPoint curve_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPointsCurve;
    LPoint point_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPoints;

    double guideWidth;

    double paramBezier;

public:
    BezierCurve();
    LStatus SetFile(LFile file);
    LStatus SetCell(LCell cell);
    LStatus SetLayer(LLayer layer);
    LStatus SetStartPoint(DubinsPoint point);
    LStatus SetEndPoint(DubinsPoint point);
    LStatus SetGuideWidth(double width);
    LStatus SetParamBezier(double paramBezier);

    void ComputeBezierCurve();

};

long Round0or5ToLong(double val);
long RoundToLong(double value);
double PointDistance(LPoint start, LPoint end);
double ArrayDistance(LPoint* arr, int nbPoints);

#endif