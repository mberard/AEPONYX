#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include <string>

#include <ldata.h>
#include <unistd.h> //getcwd

#define EXCLUDE_LEDIT_LEGACY_UPI //This statement make the C language macros, which are now superseded by C++ functions, unavailable.

#define BEZIER_PARAM 0.45

extern "C" {
    #include "DubinsPoint.hpp"

    #include "DubinsPoint.cpp"

    void BezierMacro(void);
	int UPI_Entry_Point(void);
}

void BezierMacro()
{
    DubinsPoint start, end;
    LPoint controlStart, controlEnd;

    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;

    double xStart, yStart, xEnd, yEnd;
    double angleStart, angleEnd;
    double coef;
    double distX, distY;

    start.SetPoint(0,0,pFile);
    start.SetAngleDegre(90);
    end.SetPoint(1,1,pFile);
    end.SetAngleDegre(90);

    xStart = start.GetPoint().x;
    yStart = start.GetPoint().y;
    xEnd = end.GetPoint().x;
    yEnd = end.GetPoint().y;
    angleStart = start.GetAngleRadian();
    angleEnd = end.GetAngleRadian();

    coef = 1 - BEZIER_PARAM;

    distX = xEnd - xStart;
    distY = yEnd - yStart;

    controlStart.x = (LCoord) ( xStart + distX * coef * cos(angleStart) );
    controlStart.y =  (LCoord) (yStart + distY * coef * sin(angleStart) );
    controlEnd.x = (LCoord) ( xEnd + distX * coef * cos(angleEnd + M_PI) );
    controlEnd.y = (LCoord) ( yEnd + distY * coef * sin(angleEnd + M_PI) );

    LUpi_LogMessage(LFormat("controlStart %ld %ld\n", controlStart.x, controlStart.y));
    LUpi_LogMessage(LFormat("controlEnd %ld %ld\n", controlEnd.x, controlEnd.y));
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Bezier Macro", "BezierMacro", NULL /*hotkey category*/);
	return 1;
}