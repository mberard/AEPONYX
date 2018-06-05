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

#define BEZIER_PARAM 0.30
#define MAX_POLYGON_SIZE 5000
#define WIDTH 0.05

extern "C" {
    #include "DubinsPoint.hpp"

    #include "DubinsPoint.cpp"

    long RoundToLong(double);
    void BezierMacro(void);
	int UPI_Entry_Point(void);
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
    double angleStart, angleEnd;
    double coef;
    double distX, distY;

    LPoint point_arr[MAX_POLYGON_SIZE];
    int nbPoints = 0;
    LPoint curve_arr[MAX_POLYGON_SIZE];
    int nbPointsCurve = 0;

    double t;
    int i;
    double x, y;
    double angle;

    double guideWidth = LFile_MicronsToIntU(pFile, WIDTH);

    start.SetPoint(0,0,pFile);
    start.SetAngleDegre(0);
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

    for(t=0; t<=1; t=t+0.001)
    {
        x = xStart*pow((1-t),3) + 3*controlStart.x*pow((1-t),2)*t + 3*controlEnd.x*(1-t)*pow(t,2) + xEnd*pow(t,3);
        y = yStart*pow((1-t),3) + 3*controlStart.y*pow((1-t),2)*t + 3*controlEnd.y*(1-t)*pow(t,2) + yEnd*pow(t,3);
        curve_arr[nbPointsCurve] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
        nbPointsCurve = nbPointsCurve + 1;
    }

LUpi_LogMessage(LFormat("BEGIN CREATING CURVE\n"));

    point_arr[nbPoints] = LPoint_Set((LCoord)(xStart + sin(angleStart) * guideWidth / 2.0) , (LCoord)(yStart - cos(angleStart) * guideWidth / 2.0));
    nbPoints = nbPoints + 1;
    for(i=1; i<nbPointsCurve-1; i++)
    {
        angle = atan2( curve_arr[i+1].y-curve_arr[i-1].y , curve_arr[i+1].x-curve_arr[i-1].x );
        point_arr[nbPoints] = LPoint_Set((LCoord)(curve_arr[i].x + sin(angle) * guideWidth / 2.0) , (LCoord)(curve_arr[i].y - cos(angle) * guideWidth / 2.0));
        nbPoints = nbPoints + 1;
    }
    point_arr[nbPoints] = LPoint_Set((LCoord)(xEnd + sin(angleEnd) * guideWidth / 2.0) , (LCoord)(yEnd - cos(angleEnd) * guideWidth / 2.0));
    nbPoints = nbPoints + 1;

    point_arr[nbPoints] = LPoint_Set((LCoord)(xEnd + sin(angleEnd + M_PI) * guideWidth / 2.0) , (LCoord)(yEnd - cos(angleEnd + M_PI) * guideWidth / 2.0));
    nbPoints = nbPoints + 1;
    for(i=nbPointsCurve-2; i>=1; i--)
    {
        angle = atan2( curve_arr[i-1].y-curve_arr[i+1].y , curve_arr[i-1].x-curve_arr[i+1].x );
        point_arr[nbPoints] = LPoint_Set((LCoord)(curve_arr[i].x + sin(angle) * guideWidth / 2.0) , (LCoord)(curve_arr[i].y - cos(angle) * guideWidth / 2.0));
        nbPoints = nbPoints + 1;
    }
    point_arr[nbPoints] = LPoint_Set((LCoord)(xStart + sin(angleStart + M_PI) * guideWidth / 2.0) , (LCoord)(yStart - cos(angleStart + M_PI) * guideWidth / 2.0));
    nbPoints = nbPoints + 1;
LUpi_LogMessage(LFormat("nbPoints %d\n",nbPoints));

    LPolygon_New( pCell, pLayer, point_arr, nbPoints );

}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Bezier Macro", "BezierMacro", NULL /*hotkey category*/);
	return 1;
}