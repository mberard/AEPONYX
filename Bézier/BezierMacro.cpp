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
#define MAX_POLYGON_SIZE 5000
#define WIDTH 0.05

extern "C" {
    #include "DubinsPoint.hpp"

    #include "DubinsPoint.cpp"

    long RoundToLong(double);
    void BezierMacro(void);
	int UPI_Entry_Point(void);
}

long Round0or5ToLong(double val)
{
	double tmpFloat = val;
	long tmpInt = 0;
	if(val >= 0)
		tmpFloat = (double)(tmpFloat + 2.5)/5.0;
	else
		tmpFloat = (double)(tmpFloat - 2.5)/5.0;
	tmpInt = (long)tmpFloat; //delete the digits after the '.'
	tmpInt = tmpInt*5;
	return tmpInt;
}

long RoundToLong(double value)
{
    return (long)value+0.5;
}

void BezierMacro()
{
    DubinsPoint start, end;
    LPoint controlStart, controlEnd;

    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer = LLayer_Find(pFile, "TEST");

    double xStart, yStart, xEnd, yEnd;
    double xStartChanged, yStartChanged, xEndChanged, yEndChanged;
    double angleStart, angleEnd;
    double coef;
    double distX, distY;

    LPoint point_arr[MAX_POLYGON_SIZE];
    int nbPoints = 0;
    LPoint curve_arr[MAX_POLYGON_SIZE];
    int nbPointsCurve = 0;

    double t;
    double x, y;
    double angle;

    double guideWidth = LFile_MicronsToIntU(pFile, WIDTH);

    start.SetPoint(0,0,pFile);
    start.SetAngleDegre(0);
    end.SetPoint(10,10,pFile);
    end.SetAngleDegre(45);

    xStart = start.GetPoint().x;
    yStart = start.GetPoint().y;
    xEnd = end.GetPoint().x;
    yEnd = end.GetPoint().y;
    angleStart = start.GetAngleRadian();
    angleEnd = end.GetAngleRadian();

    coef = 1 - BEZIER_PARAM;

LUpi_LogMessage(LFormat("BEGIN CREATING CURVE\n"));

    xStartChanged = xStart+(sin(angleStart)*guideWidth/2.0);
    yStartChanged = yStart-(cos(angleStart)*guideWidth/2.0);
    xEndChanged = xEnd+(sin(angleEnd)*guideWidth/2.0);
    yEndChanged = yEnd-(cos(angleEnd)*guideWidth/2.0);

    distX = xEndChanged - xStartChanged;
    distY = yEndChanged - yStartChanged;

    controlStart.x = (LCoord) ( xStartChanged + distX * coef * cos(angleStart) );
    controlStart.y = (LCoord) ( yStartChanged + distY * coef * sin(angleStart) );
    controlEnd.x = (LCoord) ( xEndChanged + distX * coef * cos(angleEnd + M_PI) );
    controlEnd.y = (LCoord) ( yEndChanged + distY * coef * sin(angleEnd + M_PI) );

    curve_arr[nbPointsCurve] = LPoint_Set( Round0or5ToLong(xStartChanged), Round0or5ToLong(yStartChanged) );
    nbPointsCurve = nbPointsCurve + 1;
    for(t=0.0005; t<1; t=t+0.0005)
    {
        x = xStartChanged*pow((1-t),3) + 3*controlStart.x*pow((1-t),2)*t + 3*controlEnd.x*(1-t)*pow(t,2) + xEndChanged*pow(t,3);
        y = yStartChanged*pow((1-t),3) + 3*controlStart.y*pow((1-t),2)*t + 3*controlEnd.y*(1-t)*pow(t,2) + yEndChanged*pow(t,3);
        curve_arr[nbPointsCurve] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
        nbPointsCurve = nbPointsCurve + 1;
    }
    curve_arr[nbPointsCurve] = LPoint_Set( Round0or5ToLong(xEndChanged), Round0or5ToLong(yEndChanged) );
    nbPointsCurve = nbPointsCurve + 1;

    xStartChanged = xEnd+(sin(angleEnd + M_PI)*guideWidth/2.0);
    yStartChanged = yEnd-(cos(angleEnd + M_PI)*guideWidth/2.0);
    xEndChanged = xStart+(sin(angleStart + M_PI)*guideWidth/2.0);
    yEndChanged = yStart-(cos(angleStart + M_PI)*guideWidth/2.0);

    distX = xEndChanged - xStartChanged;
    distY = yEndChanged - yStartChanged;

    controlStart.x = (LCoord) ( xStartChanged + distX * coef * cos(angleEnd) );
    controlStart.y = (LCoord) ( yStartChanged + distY * coef * sin(angleEnd) );
    controlEnd.x = (LCoord) ( xEndChanged + distX * coef * cos(angleStart + M_PI) );
    controlEnd.y = (LCoord) ( yEndChanged + distY * coef * sin(angleStart + M_PI) );

    curve_arr[nbPointsCurve] = LPoint_Set( Round0or5ToLong(xStartChanged), Round0or5ToLong(yStartChanged) );
    nbPointsCurve = nbPointsCurve + 1;
    for(t=0.0005; t<1; t=t+0.0005)
    {
        x = xStartChanged*pow((1-t),3) + 3*controlStart.x*pow((1-t),2)*t + 3*controlEnd.x*(1-t)*pow(t,2) + xEndChanged*pow(t,3);
        y = yStartChanged*pow((1-t),3) + 3*controlStart.y*pow((1-t),2)*t + 3*controlEnd.y*(1-t)*pow(t,2) + yEndChanged*pow(t,3);
        curve_arr[nbPointsCurve] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
        nbPointsCurve = nbPointsCurve + 1;
    }
    curve_arr[nbPointsCurve] = LPoint_Set( Round0or5ToLong(xEndChanged), Round0or5ToLong(yEndChanged) );
    nbPointsCurve = nbPointsCurve + 1;

LUpi_LogMessage(LFormat("nbPoints %d\n",nbPointsCurve));

    LPolygon_New( pCell, pLayer, curve_arr, nbPointsCurve );

}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Bezier Macro", "BezierMacro", NULL /*hotkey category*/);
	return 1;
}