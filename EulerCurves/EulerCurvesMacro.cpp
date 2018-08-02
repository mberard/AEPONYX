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

#define MAX_POLYGON_SIZE 30000

extern "C" {
    double PointDistance(LPoint, LPoint);
    void EulerCurvesMacro(void);
	int UPI_Entry_Point(void);
}

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}

void EulerCurvesMacro()
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer = LLayer_Find(pFile, "WGUIDE");

    LPoint startPoint = LPoint_Set(0,0);
    LPoint endPoint = LPoint_Set(200000,200000);
    LPoint center = LPoint_Set(0,200000);
    double startAngle = 0;
    double endAngle = 90;

    double diffAngle = (endAngle - startAngle)*M_PI/180.0;
    double delta;

    LPoint curve_arr[MAX_POLYGON_SIZE];
    int numberPointsCurveArr = 0;

    double dThetaStep = 0;
    LGrid_v16_30 grid;
	LFile_GetGrid_v16_30( pFile, &grid );

    dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / PointDistance(startPoint, center) / 20);
    
    startAngle = startAngle*M_PI/180.0 - M_PI/2.0;
    endAngle = endAngle*M_PI/180.0 - M_PI/2.0;

    curve_arr[numberPointsCurveArr] = startPoint;
    numberPointsCurveArr = numberPointsCurveArr + 1;
    for (double dTheta = startAngle; dTheta < endAngle; dTheta += dThetaStep )
    {
LUpi_LogMessage(LFormat("Dans le for %lf\n", dTheta));
        delta = (fabs(-diffAngle/2.0 - dTheta) - diffAngle/2.0)*(-2.0)/diffAngle;
        curve_arr[numberPointsCurveArr] = LPoint_Set( center.x + PointDistance(startPoint, center)*cos(dTheta) + delta*cos(dTheta)*50000*sin(dTheta/2.0) , center.y + PointDistance(startPoint, center)*sin(dTheta) + delta*sin(dTheta)*50000*cos(dTheta/2.0) );
        numberPointsCurveArr = numberPointsCurveArr + 1;
    }
    curve_arr[numberPointsCurveArr] = endPoint;
    numberPointsCurveArr = numberPointsCurveArr + 1;
LUpi_LogMessage(LFormat("Affichage polygone\n"));

curve_arr[numberPointsCurveArr] = center;
numberPointsCurveArr = numberPointsCurveArr + 1;

    LPolygon_New( pCell, pLayer, curve_arr, numberPointsCurveArr );
LUpi_LogMessage(LFormat("fin de macro\n"));
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F8" /*hotkey*/, "Euler Curves Macro", "EulerCurvesMacro", NULL /*hotkey category*/);
	return 1;
}