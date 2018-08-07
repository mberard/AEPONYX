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
    LPoint FindCenter(LPoint, LPoint, LPoint, LPoint);
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

LPoint FindCenter(LPoint left, double leftAngle , LPoint right, double rightAngle)
{
    LPoint perpendiculaireLeft, perpendiculaireRight, center;
    perpendiculaireLeft = LPoint_Set(left.x + 1000*cos(leftAngle + M_PI/2.0), left.y + 1000*sin(leftAngle + M_PI/2.0));
    perpendiculaireRight = LPoint_Set(right.x + 1000*cos(rightAngle + M_PI/2.0), right.y + 1000*sin(rightAngle + M_PI/2.0));

    //source link: https://www.developpez.net/forums/d369370/applications/developpement-2d-3d-jeux/algo-intersection-2-segments/
    double Ax = left.x;
	double Ay = left.y;
	double Bx = perpendiculaireLeft.x;
	double By = perpendiculaireLeft.y;
	double Cx = right.x;
	double Cy = right.y;
	double Dx = perpendiculaireRight.x;
	double Dy = perpendiculaireRight.y;
    double Sx;
	double Sy;
	if(Ax==Bx)
	{
		if(Cx==Dx)
        {
            return LPoint_Set( (Ax+Cx)/2.0 , (Ay+Cy)/2.0 );
        }
		else
		{
			double pCD = (Cy-Dy)/(Cx-Dx);
			Sx = Ax;
			Sy = pCD*(Ax-Cx)+Cy;
		}
	}
	else
	{
		if(Cx==Dx)
		{
			double pAB = (Ay-By)/(Ax-Bx);
			Sx = Cx;
			Sy = pAB*(Cx-Ax)+Ay;
		}
		else
		{
			double pCD = (Cy-Dy)/(Cx-Dx);
			double pAB = (Ay-By)/(Ax-Bx);
			double oCD = Cy-pCD*Cx;
			double oAB = Ay-pAB*Ax;
			Sx = (oAB-oCD)/(pCD-pAB);
			Sy = pCD*Sx+oCD;
		}
	}
    center.x = (LCoord)Sx;
    center.y = (LCoord)Sy;

    return center;
}

void EulerCurvesMacro()
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer = LLayer_Find(pFile, "WGUIDE");

LUpi_LogMessage(LFormat("\n\n\n\n\n"));

    LPoint startPoint = LPoint_Set(0,0);
    LPoint endPoint = LPoint_Set(400000,250000);
    LPoint center;
    double startAngle = 0;
    double endAngle = 90;
    LArcDirection dir;

    double x,y;

    startAngle = startAngle*M_PI/180.0;
    endAngle = endAngle*M_PI/180.0;
    center = FindCenter(startPoint , startAngle , endPoint , endAngle );

    //find if the arc direction is CC or CCW
    if(startAngle > 0 && startAngle < M_PI)
    {
        if(center.x<startPoint.x)
            dir = CCW;
        else
            dir = CW;
    }
    else if(startAngle > M_PI && startAngle < 2*M_PI)
    {
        if(center.x>startPoint.x)
            dir = CCW;
        else
            dir = CW;
    }
    else if(startAngle == M_PI)
    {
        if(center.y>startPoint.y)
            dir = CW;
        else
            dir = CCW;
    }
    else if(startAngle == 0 || startAngle == 2*M_PI)
    {
        if(center.y>startPoint.y)
            dir = CCW;
        else
            dir = CW;
    }

    double radius = PointDistance(startPoint, center);
    double delta = 8000;

    LPoint movedCenter = center;

    LPoint curve_arr[MAX_POLYGON_SIZE];
    int numberPointsCurveArr = 0;

    double coef;

    double dThetaStep = 0;
    LGrid_v16_30 grid;
	LFile_GetGrid_v16_30( pFile, &grid );

    dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 20);
    
    if(dir == CCW)
    {
        startAngle = startAngle - M_PI/2.0;
        endAngle = endAngle - M_PI/2.0;
    }
    else
    {
        startAngle = startAngle + M_PI/2.0;
        endAngle = endAngle + M_PI/2.0;
    }

    double coefX = 1;
    double coefY = 1;
    double coefXY = 1;
    double incrementCoefXY;
    double currentCoefXY = 1;
    /*
    if(endPoint.x != center.x)
    {
        if(center.x + radius*cos(endAngle) < endPoint.x )
            coefX = (endPoint.x - center.x)/(radius*cos(endAngle));
        else
            coefX = (radius*cos(endAngle))/(endPoint.x - center.x);
    }
    if(endPoint.y != center.y)
    {
        if(center.y + radius*sin(endAngle) > endPoint.y )
            coefY = (endPoint.y - center.y)/(radius*sin(endAngle));
        else
            coefY = (radius*sin(endAngle))/(endPoint.y - center.y);
    }

    if(coefX<0)
        coefX = 1/coefX;
    if(coefY<0)
        coefY = 1/coefY;

LUpi_LogMessage(LFormat("coefX %lf coefY %lf\n", coefX,coefY));
*/
    coefXY = PointDistance(endPoint, center)/(radius);
LUpi_LogMessage(LFormat("coefXY max %lf\n", coefXY));

    incrementCoefXY = (coefXY-1)/(fabs((endAngle-startAngle)/dThetaStep));

    curve_arr[numberPointsCurveArr] = startPoint;
    numberPointsCurveArr = numberPointsCurveArr + 1;

    if(dir == CCW)
    {
        for (double dTheta = startAngle; dTheta < endAngle; dTheta += dThetaStep )
        {
            movedCenter.x = center.x + radius*cos(dTheta);
            movedCenter.y = center.y + radius*sin(dTheta);

            coef = 2*M_PI*(dTheta-startAngle)/(endAngle-startAngle);
            coef = fabs(cos(coef)-1);

    LUpi_LogMessage(LFormat("currentCoefXY %lf\n", currentCoefXY));

            x = ( center.x + (radius + delta*coef)*cos(dTheta)*currentCoefXY );
            y = ( center.y + (radius + delta*coef)*sin(dTheta)*currentCoefXY );

            curve_arr[numberPointsCurveArr] = LPoint_Set( x , y );
            numberPointsCurveArr = numberPointsCurveArr + 1;

            currentCoefXY = currentCoefXY + incrementCoefXY*coef;
            if(coefXY > 1 && currentCoefXY>coefXY)
                currentCoefXY = coefXY;
            if(coefXY < 1 && currentCoefXY<coefXY)
                currentCoefXY = coefXY;
        }
    }
    else if(dir == CW)
    {
        for (double dTheta = startAngle; dTheta > endAngle; dTheta -= dThetaStep )
        {
            movedCenter.x = center.x + radius*cos(dTheta);
            movedCenter.y = center.y + radius*sin(dTheta);

            coef = 2*M_PI*(dTheta-startAngle)/(endAngle-startAngle);
            coef = fabs(cos(coef)-1);

            curve_arr[numberPointsCurveArr] = LPoint_Set( ( movedCenter.x + delta*coef*cos(dTheta) )*coefXY , ( movedCenter.y + delta*coef*sin(dTheta) )*coefXY );
            numberPointsCurveArr = numberPointsCurveArr + 1;
        }
    }

    curve_arr[numberPointsCurveArr] = endPoint;
    numberPointsCurveArr = numberPointsCurveArr + 1;

    LUpi_LogMessage(LFormat("Display polygon\n"));

    LPolygon_New( pCell, pLayer, curve_arr, numberPointsCurveArr );

    LUpi_LogMessage(LFormat("End macro\n"));
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F8" /*hotkey*/, "Euler Curves Macro", "EulerCurvesMacro", NULL /*hotkey category*/);
	return 1;
}