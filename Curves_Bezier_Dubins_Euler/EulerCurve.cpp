#include "EulerCurve.hpp"
#include <math.h>
#include "ldata.h"

//MEMBER METHODS

EulerCurve::EulerCurve(){}

LStatus EulerCurve::SetFile(LFile file){
    this->file = file;
    return LStatusOK;
}

LStatus EulerCurve::SetCell(LCell cell){
    this->cell = cell;
    return LStatusOK;
}

LStatus EulerCurve::SetLayer(LLayer layer){
    this->layer = layer;
    return LStatusOK;
}

LStatus EulerCurve::SetStartPoint(DubinsPoint point){
    this->startPoint = point;
    return LStatusOK;
}

LStatus EulerCurve::SetEndPoint(DubinsPoint point){
    this->endPoint = point;
    return LStatusOK;
}

LStatus EulerCurve::SetGuideWidth(double width){
    width = LFile_MicronsToIntU( this->file, width );
    this->guideWidth = width;
    return LStatusOK;
}

LStatus EulerCurve::SetOxideSizeValueEuler(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->oxideSizeValue = value;
    return LStatusOK;
}

LStatus EulerCurve::SetOxideLayerEuler(LLayer layer){
    this->oxideLayer = layer;
    return LStatusOK;
}

LStatus EulerCurve::SetParamEuler(double value){
    this->paramEuler = value;
    return LStatusOK;
}


void EulerCurve::ComputeEulerCurve()
{
    LCell pCell	= this->cell;
	LFile pFile	= this->file;
    LLayer pLayer = this->layer;


    double xStart, yStart, xEnd, yEnd;
    double angleStart, angleEnd;

    this->nbPointsCurve = 0;
    this->nbPoints = 0;

    int i = 0;
    double angle;

    xStart = this->startPoint.GetPoint().x;
    yStart = this->startPoint.GetPoint().y;
    xEnd = this->endPoint.GetPoint().x;
    yEnd = this->endPoint.GetPoint().y;
    angleStart = this->startPoint.GetAngleRadian();
    angleEnd = this->endPoint.GetAngleRadian();



LUpi_LogMessage(LFormat("\n\n\n\n\n"));

    LPoint startPoint = this->startPoint.GetLPoint();
    LPoint endPoint = this->endPoint.GetLPoint();
    LPoint center;
    double startAngle = this->startPoint.GetAngleRadian();
    double endAngle = this->endPoint.GetAngleRadian();
    LArcDirection dir;

    double x,y;

    center = FindCenter(startPoint , startAngle , endPoint , endAngle );

    //find if the arc direction is CC or CCW
    if(startAngle > 0 && startAngle < M_PI)
    {
        if(endPoint.x<startPoint.x)
            dir = CCW;
        else
            dir = CW;
    }
    else if(startAngle > M_PI && startAngle < 2*M_PI)
    {
        if(endPoint.x>startPoint.x)
            dir = CCW;
        else
            dir = CW;
    }
    else if(startAngle == M_PI)
    {
        if(endPoint.y>startPoint.y)
            dir = CW;
        else
            dir = CCW;
    }
    else if(startAngle == 0 || startAngle == 2*M_PI)
    {
        if(endPoint.y>startPoint.y)
            dir = CCW;
        else
            dir = CW;
    }

    double radius = PointDistanceEuler(startPoint, center);
    double delta = this->paramEuler;

    LPoint movedCenter = center;

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

    double coefXY = 1;
    double incrementCoefXY;
    double currentCoefXY = 1;

    coefXY = PointDistanceEuler(endPoint, center)/(radius);

    incrementCoefXY = (coefXY-1)/(fabs((endAngle-startAngle)/dThetaStep));

    this->curve_arr[this->nbPointsCurve] = startPoint;
    this->nbPointsCurve = this->nbPointsCurve + 1;

    if(dir == CCW)
    {
        for (double dTheta = startAngle; dTheta < endAngle; dTheta += dThetaStep )
        {
            LUpi_LogMessage(LFormat("%lf out of %lf\n", dTheta, endAngle));

            coef = 2*M_PI*(dTheta-startAngle)/(endAngle-startAngle);
            coef = fabs(cos(coef)-1);

            x = ( center.x + (radius + delta*coef)*cos(dTheta)*currentCoefXY );
            y = ( center.y + (radius + delta*coef)*sin(dTheta)*currentCoefXY );

            this->curve_arr[this->nbPointsCurve] = LPoint_Set( x , y );
            this->nbPointsCurve = this->nbPointsCurve + 1;

            currentCoefXY = currentCoefXY + incrementCoefXY*coef;
            if(coefXY > 1 && currentCoefXY>coefXY)
                currentCoefXY = coefXY;
            if(coefXY < 1 && currentCoefXY<coefXY)
                currentCoefXY = coefXY;
        }
    }
    else if(dir == CW)
    {
        for(double dTheta = startAngle; dTheta > endAngle; dTheta -= dThetaStep )
        {
            LUpi_LogMessage(LFormat("%lf out of %lf\n", dTheta, endAngle));

            coef = 2*M_PI*(dTheta-startAngle)/(endAngle-startAngle);
            coef = fabs(cos(coef)-1);

            x = ( center.x + (radius + delta*coef)*cos(dTheta)*currentCoefXY );
            y = ( center.y + (radius + delta*coef)*sin(dTheta)*currentCoefXY );

            this->curve_arr[this->nbPointsCurve] = LPoint_Set( x , y );
            this->nbPointsCurve = this->nbPointsCurve + 1;
        }
    }

    this->curve_arr[this->nbPointsCurve] = endPoint;
    this->nbPointsCurve = this->nbPointsCurve + 1;


    //construct the guide from the curve
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xStart + sin(angleStart) * this->guideWidth / 2.0) , (LCoord)round(yStart - cos(angleStart) * this->guideWidth / 2.0));
    this->nbPoints = this->nbPoints + 1;
    for(i=1; i<this->nbPointsCurve-1; i++)
    {
        angle = atan2( this->curve_arr[i+1].y-this->curve_arr[i].y , this->curve_arr[i+1].x-this->curve_arr[i].x );
        this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(this->curve_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(this->curve_arr[i].y - cos(angle) * this->guideWidth / 2.0));
        this->nbPoints = this->nbPoints + 1;
    }
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xEnd + sin(angleEnd) * this->guideWidth / 2.0) , (LCoord)round(yEnd - cos(angleEnd) * this->guideWidth / 2.0));
    this->nbPoints = this->nbPoints + 1;

    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xEnd + sin(angleEnd + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yEnd - cos(angleEnd + M_PI) * this->guideWidth / 2.0));
    this->nbPoints = this->nbPoints + 1;
    for(i=this->nbPointsCurve-2; i>=1; i--)
    {
        angle = atan2( this->curve_arr[i-1].y-this->curve_arr[i].y , this->curve_arr[i-1].x-this->curve_arr[i].x );
        this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(this->curve_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(this->curve_arr[i].y - cos(angle) * this->guideWidth / 2.0));
        this->nbPoints = this->nbPoints + 1;
    }
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xStart + sin(angleStart + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yStart - cos(angleStart + M_PI) * this->guideWidth / 2.0));
    this->nbPoints = this->nbPoints + 1;

LUpi_LogMessage(LFormat("nbPoints %d\n",this->nbPoints));
LUpi_LogMessage(LFormat("Display polygon\n"));

    LObject obj;
    obj = LPolygon_New( this->cell, this->layer, this->point_arr, this->nbPoints );

//    double dist = LFile_IntUtoMicrons(this->file, ArrayDistance(this->curve_arr, this->nbPointsCurve));
//    LEntity_AssignProperty( (LEntity)obj, "PathLength", L_real, &dist);

    if(this->oxideSizeValue != 0)
    {
        LLayer savedLayer = this->layer;
        double savedGuideWidth = this->guideWidth;
        double savedOxideSize = this->oxideSizeValue;

        this->layer = this->oxideLayer;
        this->guideWidth = this->oxideSizeValue;
        this->oxideSizeValue = 0;

        this->ComputeEulerCurve();

        this->layer = savedLayer;
        this->guideWidth = savedGuideWidth;
        this->oxideSizeValue = savedOxideSize;
    }
}



//NON CLASS METHOD

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

double PointDistanceEuler(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}