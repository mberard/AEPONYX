#include "BezierCurve.hpp"
#include <math.h>
#include "ldata.h"

//MEMBER METHODS

BezierCurve::BezierCurve(){}

LStatus BezierCurve::SetFile(LFile file){
    this->file = file;
    return LStatusOK;
}

LStatus BezierCurve::SetCell(LCell cell){
    this->cell = cell;
    return LStatusOK;
}

LStatus BezierCurve::SetLayer(LLayer layer){
    this->layer = layer;
    return LStatusOK;
}

LStatus BezierCurve::SetStartPoint(DubinsPoint point){
    this->startPoint = point;
    return LStatusOK;
}

LStatus BezierCurve::SetEndPoint(DubinsPoint point){
    this->endPoint = point;
    return LStatusOK;
}

LStatus BezierCurve::SetGuideWidth(double width){
    width = LFile_MicronsToIntU( this->file, width );
    this->guideWidth = width;
    return LStatusOK;
}


LStatus BezierCurve::SetParamBezier(double value){
    this->paramBezier = value;
    return LStatusOK;
}



void BezierCurve::ComputeBezierCurve()
{
    char strLayer[MAX_LAYER_NAME];

    double xStart, yStart, xEnd, yEnd;
    double angleStart, angleEnd;
    double coef;
    double distX, distY;

    this->nbPointsCurve = 0;
    this->nbPoints = 0;

    double t;
    int i = 0;
    int j = 0;
    double x, y;
    double angle, angle1, angle2;
    LPoint save1, save2, save3, save4;

    xStart = this->startPoint.GetPoint().x;
    yStart = this->startPoint.GetPoint().y;
    xEnd = this->endPoint.GetPoint().x;
    yEnd = this->endPoint.GetPoint().y;
    angleStart = this->startPoint.GetAngleRadian();
    angleEnd = this->endPoint.GetAngleRadian();

    coef = 1 - this->paramBezier;


LUpi_LogMessage(LFormat("BEGIN CREATING BEZIER CURVE\n"));

    distX = xEnd - xStart;
    distY = yEnd - yStart;
    if(distX<0)
        distX = -distX;
    if(distY<0)
        distY = -distY;

    this->controlStart.x = (LCoord) ( xStart + distX * coef * cos(angleStart) );
    this->controlStart.y = (LCoord) ( yStart + distY * coef * sin(angleStart) );
    this->controlEnd.x = (LCoord) ( xEnd + distX * coef * cos(angleEnd + M_PI) );
    this->controlEnd.y = (LCoord) ( yEnd + distY * coef * sin(angleEnd + M_PI) );

    //construct the curve
    this->curve_arr[this->nbPointsCurve] = LPoint_Set( xStart, yStart );
    this->nbPointsCurve = this->nbPointsCurve + 1;
    for(t=0.0005; t<1; t=t+0.0005)
    {
        x = xStart*pow((1-t),3) + 3*this->controlStart.x*pow((1-t),2)*t + 3*this->controlEnd.x*(1-t)*pow(t,2) + xEnd*pow(t,3);
        y = yStart*pow((1-t),3) + 3*this->controlStart.y*pow((1-t),2)*t + 3*this->controlEnd.y*(1-t)*pow(t,2) + yEnd*pow(t,3);
        this->curve_arr[this->nbPointsCurve] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
        this->nbPointsCurve = this->nbPointsCurve + 1;
    }
    this->curve_arr[this->nbPointsCurve] = LPoint_Set( xEnd, yEnd );
    this->nbPointsCurve = this->nbPointsCurve + 1;

    //construct the guide from the curve
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)(xStart + sin(angleStart) * this->guideWidth / 2.0) , (LCoord)(yStart - cos(angleStart) * this->guideWidth / 2.0));
    save1 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;
    for(i=1; i<this->nbPointsCurve-1; i++)
    {
        angle = atan2( this->curve_arr[i+1].y-this->curve_arr[i].y , this->curve_arr[i+1].x-this->curve_arr[i].x );
        this->point_arr[this->nbPoints] = LPoint_Set((LCoord)(this->curve_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)(this->curve_arr[i].y - cos(angle) * this->guideWidth / 2.0));
        this->nbPoints = this->nbPoints + 1;
    }
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)(xEnd + sin(angleEnd) * this->guideWidth / 2.0) , (LCoord)(yEnd - cos(angleEnd) * this->guideWidth / 2.0));
    save2 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;

    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)(xEnd + sin(angleEnd + M_PI) * this->guideWidth / 2.0) , (LCoord)(yEnd - cos(angleEnd + M_PI) * this->guideWidth / 2.0));
    save3 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;
    for(i=this->nbPointsCurve-2; i>=1; i--)
    {
        angle = atan2( this->curve_arr[i-1].y-this->curve_arr[i].y , this->curve_arr[i-1].x-this->curve_arr[i].x );
        this->point_arr[this->nbPoints] = LPoint_Set((LCoord)(this->curve_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)(this->curve_arr[i].y - cos(angle) * this->guideWidth / 2.0));
        this->nbPoints = this->nbPoints + 1;
    }
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)(xStart + sin(angleStart + M_PI) * this->guideWidth / 2.0) , (LCoord)(yStart - cos(angleStart + M_PI) * this->guideWidth / 2.0));
    save4 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;

    LUpi_LogMessage(LFormat("nbPoints %d\n",this->nbPoints));

    //delete the points that intersect with the polygon
    j=1;
    while(j != 0)
    {
        j=0;
        for(i=0; i<this->nbPoints; i++)
        {
            if((save1.x==this->point_arr[i].x && save1.y==this->point_arr[i].y) || (save2.x==this->point_arr[i].x && save2.y==this->point_arr[i].y) || (save3.x==this->point_arr[i].x && save3.y==this->point_arr[i].y) || (save4.x==this->point_arr[i].x && save4.y==this->point_arr[i].y))
                continue;
            if(i==0)
                angle1 = atan2(this->point_arr[0].y-this->point_arr[this->nbPoints-1].y,this->point_arr[0].x-this->point_arr[this->nbPoints-1].x) - M_PI;
            else
                angle1 = atan2(this->point_arr[i].y-this->point_arr[i-1].y,this->point_arr[i].x-this->point_arr[i-1].x) - M_PI;
            if(i==nbPoints-1)
                angle2 = atan2(this->point_arr[this->nbPoints-1].y-this->point_arr[0].y,this->point_arr[this->nbPoints-1].x-this->point_arr[0].x);
            else
                angle2 = atan2(this->point_arr[i].y-this->point_arr[i+1].y,this->point_arr[i].x-this->point_arr[i+1].x);
            angle = angle2 - angle1;
            angle = fmod(angle, 2*M_PI);
            while(angle < 0)
                angle = angle + 2*M_PI;
//LUpi_LogMessage(LFormat("i %d\n",i));
            if( (angle > M_PI - ANGLE_LIMIT && angle < M_PI +ANGLE_LIMIT) ) //if not in the limit range
            {
                //point_arr[i]=point_arr[(i+1)%nbPoints];
                //point_arr[(i+1)%nbPoints]=point_arr[i];
                for(j=i; j<this->nbPoints; j++)
                    this->point_arr[j]=this->point_arr[(j+1)%this->nbPoints];
                this->nbPoints = this->nbPoints - 1;
                j=1;
            }                
        }
    }
    
    LUpi_LogMessage(LFormat("nbPoints %d\n",this->nbPoints));

    LObject obj;
    obj = LPolygon_New( this->cell, this->layer, this->point_arr, this->nbPoints );
    double dist = LFile_IntUtoMicrons(this->file, ArrayDistance(this->curve_arr, this->nbPointsCurve));
    LEntity_AssignProperty( (LEntity)obj, "PathLength", L_real, &dist);
}





//NON CLASS METHODS

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
    if(value > 0)
        return (long)(value+0.5);
    else
        return (long)(value-0.5);
}

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}

double ArrayDistance(LPoint* arr, int nbPoints)
{
    double dist = 0;
    int i = 0;
    for(i=0; i<nbPoints-1; i++)
    {
        dist += PointDistance(arr[i], arr[i+1]);
    }
    return dist;
}