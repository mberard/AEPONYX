#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <ldata.h>
#include "DubinsPoint.hpp"

#define EXCLUDE_LEDIT_LEGACY_UPI //This statement make the C language macros, which are now superseded by C++ functions, unavailable.

#define BEZIER_PARAM 0.3
#define WIDTH 0.2
#define ANGLE_LIMIT 1.5

class BezierCurve{
private:
    LCell cell;
    LFile file;
    LLayer layer;

    DubinsPoint startPoint;
    DubinsPoint endPoint;
    LPoint controlStart;
    LPoint controlEnd;

    LPoint curve_arr[MAX_POLYGON_SIZE];
    int nbPointsCurve;
    LPoint point_arr[MAX_POLYGON_SIZE];
    int nbPoints;

    double guideWidth;

public:
    BezierCurve();
    LStatus SetFile(LFile file);
    LStatus SetCell(LCell cell);
    LStatus SetLayer(LLayer layer);
    LStatus SetStartPoint(DubinsPoint point);
    LStatus SetEndPoint(DubinsPoint point);
    LStatus SetGuideWidth(double width);

    void ComputeBezierCurve();

};

long Round0or5ToLong(double val);
long RoundToLong(double value);

#endif