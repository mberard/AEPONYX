#include "BezierCurve.hpp"
#include <math.h>
#include "ldata.h"

//MEMBER METHODS

//constructor
BezierCurve::BezierCurve(){}

//setters
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

LStatus BezierCurve::SetWGGROW003SizeValueBezier(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->WGGROW003SizeValue = value;
    return LStatusOK;
}
LStatus BezierCurve::SetWGOVL010SizeValueBezier(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->WGOVL010SizeValue = value;
    return LStatusOK;
}
LStatus BezierCurve::SetWGOVLHOLESizeValueBezier(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->WGOVLHOLESizeValue = value;
    return LStatusOK;
}

LStatus BezierCurve::SetOxideSizeValueBezier(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->oxideSizeValue = value;
    return LStatusOK;
}

LStatus BezierCurve::SetWGGROW003LayerBezier(LLayer layer){
    this->WGGROW003Layer = layer;
    return LStatusOK;
}
LStatus BezierCurve::SetWGOVL010LayerBezier(LLayer layer){
    this->WGOVL010Layer = layer;
    return LStatusOK;
}
LStatus BezierCurve::SetWGOVLHOLELayerBezier(LLayer layer){
    this->WGOVLHOLELayer = layer;
    return LStatusOK;
}			
			
LStatus BezierCurve::SetOxideLayerBezier(LLayer layer){
    this->oxideLayer = layer;
    return LStatusOK;
}

LStatus BezierCurve::SetParamBezier(double value){
    this->paramBezier = value;
    return LStatusOK;
}

//getters
double BezierCurve::GetWGLength(){
	return this->WGLength;
}
void BezierCurve::ComputeBezierCurveCall(){
	ComputeBezierCurve();
	
	LLayer savedLayer;
	double savedGuideWidth, savedSize;
    //create the path of oxide if necessary
    if(this->oxideSizeValue != 0){
        savedLayer = this->layer;
        savedGuideWidth = this->guideWidth;
        savedSize = this->oxideSizeValue;

        this->layer = this->oxideLayer;
        this->guideWidth = this->oxideSizeValue;
        this->oxideSizeValue = 0;

        this->ComputeBezierCurve();

        this->layer = savedLayer;
        this->guideWidth = savedGuideWidth;
        this->oxideSizeValue = savedSize;
    }
	
	//WGGROW003Layer
    if(this->WGGROW003SizeValue != 0){
        savedLayer = this->layer;
        savedGuideWidth = this->guideWidth;
        savedSize = this->WGGROW003SizeValue;

        this->layer = this->WGGROW003Layer;
        this->guideWidth = this->WGGROW003SizeValue;
        this->WGGROW003SizeValue = 0;

        this->ComputeBezierCurve();

        this->layer = savedLayer;
        this->guideWidth = savedGuideWidth;
        this->WGGROW003SizeValue = savedSize;
    }
	
	//WGOVL010Layer
    if(this->WGOVL010SizeValue != 0){
        savedLayer = this->layer;
        savedGuideWidth = this->guideWidth;
        savedSize = this->WGOVL010SizeValue;

        this->layer = this->WGOVL010Layer;
        this->guideWidth = this->WGOVL010SizeValue;
        this->WGOVL010SizeValue = 0;

        this->ComputeBezierCurve();

        this->layer = savedLayer;
        this->guideWidth = savedGuideWidth;
        this->WGOVL010SizeValue = savedSize;
    }
	
	//WGOVLHOLELayer
    if(this->WGOVLHOLESizeValue != 0){
        savedLayer = this->layer;
        savedGuideWidth = this->guideWidth;
        savedSize = this->WGOVLHOLESizeValue;

        this->layer = this->WGOVLHOLELayer;
        this->guideWidth = this->WGOVLHOLESizeValue;
        this->WGOVLHOLESizeValue = 0;

        this->ComputeBezierCurve();

        this->layer = savedLayer;
        this->guideWidth = savedGuideWidth;
        this->WGOVLHOLESizeValue = savedSize;
    }
}

void BezierCurve::ComputeBezierCurve(){
    char strLayer[MAX_LAYER_NAME];

    double xStart, yStart, xEnd, yEnd;
    double angleStart, angleEnd;
    double distX, distY;

    this->nbPointsCurve = 0;
    this->nbPoints = 0;

    int i = 0;
    int j = 1;
    double x, y;
    double angle, angle1, angle2;
    LPoint save1, save2, save3, save4;

    xStart = this->startPoint.GetPoint().x;
    yStart = this->startPoint.GetPoint().y;
    xEnd = this->endPoint.GetPoint().x;
    yEnd = this->endPoint.GetPoint().y;
    angleStart = this->startPoint.GetAngleRadian();
    angleEnd = this->endPoint.GetAngleRadian();

    double coef = 1 - this->paramBezier;


LUpi_LogMessage(LFormat("BEGIN CREATING BEZIER CURVE\n"));

    distX = xEnd - xStart;
    distY = yEnd - yStart;
    if(distX<0)
        distX = -distX;
    if(distY<0)
        distY = -distY;

    this->controlStart.x = (LCoord)round ( xStart + distX * coef * cos(angleStart) );
    this->controlStart.y = (LCoord)round ( yStart + distY * coef * sin(angleStart) );
    this->controlEnd.x = (LCoord)round ( xEnd + distX * coef * cos(angleEnd + M_PI) );
    this->controlEnd.y = (LCoord)round ( yEnd + distY * coef * sin(angleEnd + M_PI) );

    //construct the curve
    this->curve_arr[this->nbPointsCurve] = LPoint_Set( xStart, yStart );
    this->nbPointsCurve = this->nbPointsCurve + 1;
    for(double t=0.0005; t<1; t=t+0.0005)
    {
        x = xStart*pow((1-t),3) + 3*this->controlStart.x*pow((1-t),2)*t + 3*this->controlEnd.x*(1-t)*pow(t,2) + xEnd*pow(t,3);
        y = yStart*pow((1-t),3) + 3*this->controlStart.y*pow((1-t),2)*t + 3*this->controlEnd.y*(1-t)*pow(t,2) + yEnd*pow(t,3);
        this->curve_arr[this->nbPointsCurve] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
        this->nbPointsCurve = this->nbPointsCurve + 1;
    }
    this->curve_arr[this->nbPointsCurve] = LPoint_Set( xEnd, yEnd );
    this->nbPointsCurve = this->nbPointsCurve + 1;

    //construct the guide from the curve
    
    //right side of the curve
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xStart + sin(angleStart) * this->guideWidth / 2.0) , (LCoord)round(yStart - cos(angleStart) * this->guideWidth / 2.0));
    save1 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;
    for(i=1; i<this->nbPointsCurve-1; i++)
    {
        angle = atan2( this->curve_arr[i+1].y-this->curve_arr[i].y , this->curve_arr[i+1].x-this->curve_arr[i].x );
        this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(this->curve_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(this->curve_arr[i].y - cos(angle) * this->guideWidth / 2.0));
        this->nbPoints = this->nbPoints + 1;
    }
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xEnd + sin(angleEnd) * this->guideWidth / 2.0) , (LCoord)round(yEnd - cos(angleEnd) * this->guideWidth / 2.0));
    save2 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;

    //left side of the curve
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xEnd + sin(angleEnd + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yEnd - cos(angleEnd + M_PI) * this->guideWidth / 2.0));
    save3 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;
    for(i=this->nbPointsCurve-2; i>=1; i--)
    {
        angle = atan2( this->curve_arr[i-1].y-this->curve_arr[i].y , this->curve_arr[i-1].x-this->curve_arr[i].x );
        this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(this->curve_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(this->curve_arr[i].y - cos(angle) * this->guideWidth / 2.0));
        this->nbPoints = this->nbPoints + 1;
    }
    this->point_arr[this->nbPoints] = LPoint_Set((LCoord)round(xStart + sin(angleStart + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yStart - cos(angleStart + M_PI) * this->guideWidth / 2.0));
    save4 = this->point_arr[this->nbPoints];
    this->nbPoints = this->nbPoints + 1;

    LUpi_LogMessage(LFormat("nbPoints %d\n",this->nbPoints));

    //delete the points that intersect with the polygon
    while(j != 0){
        j=0;
        for(i=0; i<this->nbPoints; i++){
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
            
            if( (angle > M_PI - ANGLE_LIMIT && angle < M_PI +ANGLE_LIMIT) ) //if not in the limit range
            {
                for(j=i; j<this->nbPoints; j++)
                    this->point_arr[j]=this->point_arr[(j+1)%this->nbPoints];
                this->nbPoints = this->nbPoints - 1;
                j=1;
            }                
        }
    }
    
    LUpi_LogMessage(LFormat("nbPoints %d\n",this->nbPoints));

    //create the path
    LObject obj = LPolygon_New( this->cell, this->layer, this->point_arr, this->nbPoints );
	//calculate the length of the waveguide
    double dist = LFile_IntUtoMicrons(this->file, ArrayDistance(this->curve_arr, this->nbPointsCurve));
    LEntity_AssignProperty( (LEntity)obj, "PathLength", L_real, &dist);
	this->WGLength = dist;
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

double PointDistance(LPoint start, LPoint end){
    double dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    return sqrt(dist);
}

double ArrayDistance(LPoint* arr, int nbPoints){
    double dist = 0;
    for(int i=0; i<nbPoints-1; i++){
        dist += PointDistance(arr[i], arr[i+1]);
    }
    return dist;
}