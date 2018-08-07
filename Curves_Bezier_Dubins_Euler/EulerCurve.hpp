#ifndef EULERCURVE_H
#define EULERCURVE_H

#include <ldata.h>
#include "DubinsPoint.hpp"

#define EXCLUDE_LEDIT_LEGACY_UPI //This statement make the C language macros, which are now superseded by C++ functions, unavailable.

#define MAX_POLYGON_SIZE_EULER 15000

class EulerCurve{
private:
    LCell cell;
    LFile file;
    LLayer layer;

    DubinsPoint startPoint;
    DubinsPoint endPoint;

    LPoint curve_arr[MAX_POLYGON_SIZE_EULER];
    int nbPointsCurve;
    LPoint point_arr[MAX_POLYGON_SIZE_EULER];
    int nbPoints;

    double guideWidth;

    double oxideSizeValue;
    LLayer oxideLayer;

    double paramEuler;

public:
    EulerCurve();
    LStatus SetFile(LFile file);
    LStatus SetCell(LCell cell);
    LStatus SetLayer(LLayer layer);
    LStatus SetStartPoint(DubinsPoint point);
    LStatus SetEndPoint(DubinsPoint point);
    LStatus SetGuideWidth(double width);
    LStatus SetOxideSizeValueEuler(double value);
    LStatus SetOxideLayerEuler(LLayer layer);
    LStatus SetParamEuler(double paramEuler);

    void ComputeEulerCurve();

};


#endif