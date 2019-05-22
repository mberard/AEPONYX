#include "DubinsPath.hpp"
#include <math.h>
#include "ldata.h"

//// PRIVATE METHODS ////

//update the position of the circle used to compute the best path
LStatus DubinsPath::UpdateCircleCenter(){
    DPoint Dcenter;
    LPoint Lcenter;
    LCoord radius = this->radius;

    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() - M_PI/2.0);
    Lcenter.x = round(Dcenter.x);
    Lcenter.y = round(Dcenter.y);
    this->centerStartRightCircle = Lcenter;

    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() + M_PI/2.0);
    Lcenter.x = round(Dcenter.x);
    Lcenter.y = round(Dcenter.y);
    this->centerStartLeftCircle = Lcenter;

    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() - M_PI/2.0);
    Lcenter.x = round(Dcenter.x);
    Lcenter.y = round(Dcenter.y);
    this->centerEndRightCircle = Lcenter;

    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() + M_PI/2.0);
    Lcenter.x = round(Dcenter.x);
    Lcenter.y = round(Dcenter.y);
    this->centerEndLeftCircle = Lcenter;
    
    return LStatusOK;
}

//// PUBLIC METHODS ////

//constructor
DubinsPath::DubinsPath(){}

//setters
LStatus DubinsPath::SetGuideWidth(float width){
    this->guideWidth = LFile_MicronsToIntU( this->file, width );
    return LStatusOK;
}

LStatus DubinsPath::SetStartPoint(DubinsPoint startPoint){
    this->startPoint.SetPointInInternalUnits( startPoint.GetPoint().x , startPoint.GetPoint().y );
    this->startPoint.SetAngleRadian(startPoint.GetAngleRadian());
    return this->UpdateCircleCenter();
}

LStatus DubinsPath::SetEndPoint(DubinsPoint endPoint){
    this->endPoint.SetPointInInternalUnits( endPoint.GetPoint().x , endPoint.GetPoint().y );
    this->endPoint.SetAngleRadian(endPoint.GetAngleRadian());
    return this->UpdateCircleCenter();
}

LStatus DubinsPath::SetDistance(float distance){
    this->distance = distance;
    return LStatusOK;
}

LStatus DubinsPath::SetRadius(float radius){

    radius = LFile_MicronsToIntU( this->file, radius );

    this->radius = radius;
    return this->UpdateCircleCenter();
}

LStatus DubinsPath::SetFile(LFile file){
    this->file = file;
    return LStatusOK;
}

LStatus DubinsPath::SetCell(LCell cell){
    this->cell = cell;
    return LStatusOK;
}

LStatus DubinsPath::SetLayer(LLayer layer){
    this->layer = layer;
    return LStatusOK;
}

LStatus DubinsPath::SetOffsetCurveIsSelected(bool choice){
    this->offsetCurveIsSelected = choice;
    return this->UpdateCircleCenter();
}

LStatus DubinsPath::SetOffsetValue(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->offsetValue = value;
    return this->UpdateCircleCenter();
}

LStatus DubinsPath::SetOxideSizeValue(double value){
    value = LFile_MicronsToIntU( this->file, value );
    this->oxideSizeValue = value;
    return LStatusOK;
}

LStatus DubinsPath::SetOxideLayer(LLayer layer){
    this->oxideLayer = layer;
    return LStatusOK;
}

LStatus DubinsPath::SetParamBezier(double value){
    this->paramBezier = value;
    return LStatusOK;
}

//getters
LFile DubinsPath::GetFile(){
    return this->file;
}

LCell DubinsPath::GetCell(){
    return this->cell;
}

LLayer DubinsPath::GetLayer(){
    return this->layer;
}

double DubinsPath::GetOxideSizeValue(){
    return LFile_IntUtoMicrons(this->file, this->oxideSizeValue);
}

//compute the lenght of the different type
float DubinsPath::ComputeRSRLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartRightCircle, this->startPoint.GetLPoint(), this->startTangent, false);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndRightCircle, this->endTangent, this->endPoint.GetLPoint(), false);
    return returnDistance;
}

float DubinsPath::ComputeLSLLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartLeftCircle, this->startPoint.GetLPoint(), this->startTangent, true);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndLeftCircle, this->endTangent, this->endPoint.GetLPoint(), true);
    return returnDistance;
}

float DubinsPath::ComputeRSLLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartRightCircle, this->startPoint.GetLPoint(), this->startTangent, false);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndLeftCircle, this->endTangent, this->endPoint.GetLPoint(), true);
    return returnDistance;
}

float DubinsPath::ComputeLSRLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartLeftCircle, this->startPoint.GetLPoint(), this->startTangent, true);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndRightCircle, this->endTangent, this->endPoint.GetLPoint(), false);
    return returnDistance;
}

float DubinsPath::ComputeRLRLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartRightCircle, this->startPoint.GetLPoint(), this->startTangent, false);
    returnDistance += this->GetArcLength(this->centerMiddleCircle, this->startTangent, this->endTangent, true);
    returnDistance += this->GetArcLength(this->centerEndRightCircle, this->endTangent, this->endPoint.GetLPoint(), false);
    return returnDistance;
}

float DubinsPath::ComputeLRLLength()
{
    float returnDistance;
    
    returnDistance = this->GetArcLength(this->centerStartLeftCircle, this->startPoint.GetLPoint(), this->startTangent, true);
    returnDistance += this->GetArcLength(this->centerMiddleCircle, this->startTangent, this->endTangent, false);
    returnDistance += this->GetArcLength(this->centerEndLeftCircle, this->endTangent, this->endPoint.GetLPoint(), true);
    return returnDistance;
}

//compute the tangent points for the different type of path
void DubinsPath::GetLSLorRSRTangent(LPoint startCircleCenter, LPoint endCircleCenter, bool isBottom)
{
    LCoord xStart, xEnd, yStart, yEnd;
    float dx, dy, theta;

    xStart = startCircleCenter.x;
    xEnd = endCircleCenter.x;
    yStart = startCircleCenter.y;
    yEnd = endCircleCenter.y;

    dx = xEnd - xStart;
    dy = yEnd - yStart;
    
    theta = M_PI/2;
    theta += atan2(dy, dx);
    if(isBottom)
        theta += M_PI;
    
    theta = fmod( theta, 2*M_PI );
    if(theta <0)
        theta = theta + 2*M_PI;

    this->startTangent.x = xStart + this->radius * cos(theta);
    this->startTangent.y = yStart + this->radius * sin(theta);

    this->endTangent.x = this->startTangent.x + dx;
    this->endTangent.y = this->startTangent.y + dy;
}

void DubinsPath::GetRSLorLSRTangent(LPoint startCircleCenter, LPoint endCircleCenter, bool isBottom)
{
    float circleDistance, theta;
    LCoord xStart, xEnd, yStart, yEnd;
    LPoint tmp;
    float dx, dy;

    xStart = startCircleCenter.x;
    xEnd = endCircleCenter.x;
    yStart = startCircleCenter.y;
    yEnd = endCircleCenter.y;

    dx = xEnd - xStart;
    dy = yEnd - yStart;

    circleDistance = PointDistance(startCircleCenter, endCircleCenter);

    theta = acos((2.0 * this->radius)/circleDistance);

    if(isBottom)
        theta = theta * (-1.0);

    theta += atan2(dy, dx);

    this->startTangent.x = xStart + this->radius * cos(theta);
    this->startTangent.y = yStart + this->radius * sin(theta);

    tmp.x = xStart + 2.0 * this->radius * cos(theta);
    tmp.y = yStart + 2.0 * this->radius * sin(theta);

    dx = xEnd - tmp.x;
    dy = yEnd - tmp.y;

    this->endTangent.x = this->startTangent.x + dx;
    this->endTangent.y = this->startTangent.y + dy;
}

void DubinsPath::GetRLRorLRLTangent(LPoint startCircleCenter, LPoint endCircleCenter, bool isLRL)
{
    float circleDistance, theta;
    LCoord xStart, xEnd, yStart, yEnd;
    float xNormalised, yNormalised;
    float dx, dy;

    xStart = startCircleCenter.x;
    xEnd = endCircleCenter.x;
    yStart = startCircleCenter.y;
    yEnd = endCircleCenter.y;

    dx = xEnd - xStart;
    dy = yEnd - yStart;

    circleDistance = PointDistance(startCircleCenter, endCircleCenter);

    theta = acos(circleDistance / (4.0 * this->radius));

    if(isLRL)
        theta = atan2(dy,dx) + theta;
    else
        theta = atan2(dy,dx) - theta;

    this->centerMiddleCircle.x = xStart + 2.0 * this->radius * cos(theta);
    this->centerMiddleCircle.y = yStart + 2.0 * this->radius * sin(theta);
    
    xNormalised = (xStart - this->centerMiddleCircle.x) / PointDistance(startCircleCenter, this->centerMiddleCircle);
    yNormalised = (yStart - this->centerMiddleCircle.y) / PointDistance(startCircleCenter, this->centerMiddleCircle);
    this->startTangent.x = this->centerMiddleCircle.x + xNormalised * this->radius;
    this->startTangent.y = this->centerMiddleCircle.y + yNormalised * this->radius;

    xNormalised = (xEnd - this->centerMiddleCircle.x) / PointDistance(endCircleCenter, this->centerMiddleCircle);
    yNormalised = (yEnd - this->centerMiddleCircle.y) / PointDistance(endCircleCenter, this->centerMiddleCircle);
    this->endTangent.x = this->centerMiddleCircle.x + xNormalised * this->radius;
    this->endTangent.y = this->centerMiddleCircle.y + yNormalised * this->radius;
}

//get the length of an arc
float DubinsPath::GetArcLength(LPoint centerCircle, LPoint startPoint, LPoint tangent, bool isLeftCircle)
{
    float theta, returnDistance;
    LCoord dxStart, dyStart, dxTangent, dyTangent;

    dxStart = startPoint.x - centerCircle.x;
    dyStart = startPoint.y - centerCircle.y;
    dxTangent = tangent.x - centerCircle.x;
    dyTangent = tangent.y - centerCircle.y;

    theta = atan2(dyTangent, dxTangent) - atan2(dyStart, dxStart);

    if(theta < 0.0 && isLeftCircle)
        theta += 2.0*M_PI;
    
    else if(theta > 0.0 && !isLeftCircle)
        theta -= 2.0*M_PI;

    returnDistance = theta * this->radius;

    if(returnDistance<0.0)
        return -returnDistance;
    else
        return returnDistance;
}

//function to find the best path and call the appropriate functions to create it
void DubinsPath::ComputeDubinsPaths(){

    float xStart, xEnd, yStart, yEnd;

    xStart = this->startPoint.GetPoint().x;
    xEnd = this->endPoint.GetPoint().x;
    yStart = this->startPoint.GetPoint().y;
    yEnd = this->endPoint.GetPoint().y;

    float returnDistance, shortestDistance;
    PathType shortestType;
    LPoint shortestStartTangent, shortestEndTangent, shortestMiddleCenter;

    shortestDistance = 9999999999999999.9999;

    //RSR
    if( ! (xStart == xEnd && yStart == yEnd) ) //not the same circle == start and endpoint are different
    {
        ////compute the RSR length
        //find the tangent
        this->GetLSLorRSRTangent(this->centerStartRightCircle, this->centerEndRightCircle, false);
        
        returnDistance = this->ComputeRSRLength();
        LUpi_LogMessage( LFormat("RSR distance %f\n",returnDistance) );
        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = RSR;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
        }
    }

    //LSL
    if( ! (xStart == xEnd && yStart == yEnd) ) //not the same circle == start and endpoint are different
    {
        
        this->GetLSLorRSRTangent(this->centerStartLeftCircle, this->centerEndLeftCircle, true);
        
        returnDistance = this->ComputeLSLLength();
        LUpi_LogMessage( LFormat("LSL distance %f\n",returnDistance) );
        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = LSL;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
        }
    }


    double circleDistanceSqr = PointDistance(this->centerEndLeftCircle, this->centerStartRightCircle) * PointDistance(this->centerEndLeftCircle, this->centerStartRightCircle);
    double comparaisonDistanceSqr = (2*this->radius)*(2*this->radius);

    //RSL
    if( circleDistanceSqr >= comparaisonDistanceSqr ) //circle don't intersect
    {
        this->GetRSLorLSRTangent(this->centerStartRightCircle, this->centerEndLeftCircle, false);

        returnDistance = this->ComputeRSLLength();
        LUpi_LogMessage( LFormat("RSL distance %lf\n",returnDistance) );
        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = RSL;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
        }
    } 

    circleDistanceSqr = PointDistance(this->centerEndRightCircle, this->centerStartLeftCircle) * PointDistance(this->centerEndRightCircle, this->centerStartLeftCircle);
    //LSR
    if( circleDistanceSqr >= comparaisonDistanceSqr ) //circle don't intersect
    {
        this->GetRSLorLSRTangent(this->centerStartLeftCircle, this->centerEndRightCircle, true);
         
        returnDistance = this->ComputeLSRLength();
        LUpi_LogMessage( LFormat("LSR distance %f\n",returnDistance) );
        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = LSR;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
        }
    }

    comparaisonDistanceSqr = (4*this->radius)*(4*this->radius);
    
    //RLR
    if( circleDistanceSqr < comparaisonDistanceSqr ) //circle don't intersect
    {
        this->GetRLRorLRLTangent(this->centerStartRightCircle, this->centerEndRightCircle, false);

        returnDistance = this->ComputeRLRLength();
        LUpi_LogMessage( LFormat("RLR distance %f\n",returnDistance) );
        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = RLR;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
            shortestMiddleCenter = this->centerMiddleCircle;
        }
    } 

    //LRL
    if( circleDistanceSqr < comparaisonDistanceSqr ) //circle don't intersect
    {
        this->GetRLRorLRLTangent(this->centerStartLeftCircle, this->centerEndLeftCircle, true);

        returnDistance = this->ComputeLRLLength();
        LUpi_LogMessage( LFormat("LRL distance %f\n",returnDistance) );
        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = LRL;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
            shortestMiddleCenter = this->centerMiddleCircle;
        }
    }

    this->startTangent = shortestStartTangent;
    this->endTangent = shortestEndTangent;
    this->distance = shortestDistance;
    this->type = shortestType;
    if( shortestType == LRL || shortestType == RLR )
        this->centerMiddleCircle = shortestMiddleCenter;
    else //initialisation of the Middle torus to know to not draw this
    {
        LTorusParams params;
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    //call the appropriate function
    switch(shortestType)
    {
        case RSR:
            this->StoreRSRPath();
            break;
        case LSL:
            this->StoreLSLPath();
            break;
        case RSL:
            this->StoreRSLPath();
            break;
        case LSR:
            this->StoreLSRPath();
            break;
        case RLR:
            this->StoreRLRPath();
            break;
        case LRL:
            this->StoreLRLPath();
            break;
        default:
            LDialog_AlertBox(LFormat("Path error"));
    }
}


void DubinsPath::DrawLine() //only when the dubin path is a unique line
{
    LPoint point_arr[4];
    float dx, dy;
    double dist;
    dx = this->endPoint.GetPoint().x - this->startPoint.GetPoint().x;
    dy = this->endPoint.GetPoint().y - this->startPoint.GetPoint().y;
    point_arr[0] = LPoint_Set( this->startPoint.GetPoint().x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->startPoint.GetPoint().y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[1] = LPoint_Set( this->endPoint.GetPoint().x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->endPoint.GetPoint().y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[2] = LPoint_Set( this->endPoint.GetPoint().x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->endPoint.GetPoint().y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[3] = LPoint_Set( this->startPoint.GetPoint().x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->startPoint.GetPoint().y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);
    dist = LFile_IntUtoMicrons(this->file, (LCoord)PointDistance(this->startPoint.GetLPoint(),this->endPoint.GetLPoint()));
    LEntity_AssignProperty( (LEntity)this->line, "PathLength", L_real, &dist );
}





void DubinsPath::StoreRSRPath()
{ 
    double angleTorusPoint, angleTorusTangent, angleTorusTangentRadian;

    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->startPoint.GetAngleDegre()+90;
    angleTorusTangentRadian = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x );

    params.dStartAngle = RoundAngle(angleTorusTangent);
    params.dStopAngle = RoundAngle(angleTorusPoint);
    
    //create the start torus
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    //compute the first two points of the line
    LPoint point_arr[4];
    point_arr[0] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    point_arr[1] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );

    params.ptCenter = this->centerEndRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->endPoint.GetAngleDegre()+90;
    angleTorusTangentRadian = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x );

    params.dStartAngle = RoundAngle(angleTorusPoint);
    params.dStopAngle = RoundAngle(angleTorusTangent);
    
    //create end torus
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    //compute 2 last points of the line
    point_arr[2] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );
    point_arr[3] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    //create the line
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);

}

//same methods as RSR but with a different ordre
void DubinsPath::StoreLSLPath()
{
    double angleTorusPoint, angleTorusTangent, angleTorusTangentRadian;

    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->startPoint.GetAngleDegre()-90;
    angleTorusTangentRadian = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x );

    params.dStartAngle = RoundAngle(angleTorusPoint);
    params.dStopAngle = RoundAngle(angleTorusTangent);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }


    LPoint point_arr[4];
    point_arr[0] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    point_arr[1] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius, params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );
    

    params.ptCenter = this->centerEndLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->endPoint.GetAngleDegre()-90;
    angleTorusTangentRadian = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x );

    params.dStartAngle = RoundAngle(angleTorusTangent);
    params.dStopAngle = RoundAngle(angleTorusPoint);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    }
    
    
    point_arr[2] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius, params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );
    point_arr[3] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);
}

//same methods as RSR but with a different ordre
void DubinsPath::StoreRSLPath()
{
    double angleTorusPoint, angleTorusTangent, angleTorusTangentRadian;

    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->startPoint.GetAngleDegre()+90;
    angleTorusTangentRadian = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x );

    params.dStartAngle = RoundAngle(angleTorusTangent);
    params.dStopAngle = RoundAngle(angleTorusPoint);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    LPoint point_arr[4];
    point_arr[0] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    point_arr[1] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius, params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );

    params.ptCenter = this->centerEndLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->endPoint.GetAngleDegre()-90;
    angleTorusTangentRadian = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x );

    params.dStartAngle = RoundAngle(angleTorusTangent);
    params.dStopAngle = RoundAngle(angleTorusPoint);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    point_arr[2] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    point_arr[3] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius, params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);
}

//same methods as RSR but with a different ordre
void DubinsPath::StoreLSRPath()
{
    double angleTorusPoint, angleTorusTangent, angleTorusTangentRadian;

    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->startPoint.GetAngleDegre()-90;
    angleTorusTangentRadian = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x );

    params.dStartAngle = RoundAngle(angleTorusPoint);
    params.dStopAngle = RoundAngle(angleTorusTangent);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    LPoint point_arr[4];
    point_arr[0] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    point_arr[1] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius, params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );


    params.ptCenter = this->centerEndRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = this->endPoint.GetAngleDegre()+90;
    angleTorusTangentRadian = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x );

    params.dStartAngle = RoundAngle(angleTorusPoint);
    params.dStopAngle = RoundAngle(angleTorusTangent);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    }

    point_arr[2] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nOuterRadius , params.ptCenter.y + sin(angleTorusTangentRadian)*params.nOuterRadius );
    point_arr[3] = LPoint_Set( params.ptCenter.x + cos(angleTorusTangentRadian)*params.nInnerRadius, params.ptCenter.y + sin(angleTorusTangentRadian)*params.nInnerRadius );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);
}

//for RLR and LRL, same methods but with 3 torus (no line)
void DubinsPath::StoreRLRPath()
{
    double angleTorusPointStart, angleTorusTangentStart, angleTorusPointEnd, angleTorusTangentEnd;

    angleTorusTangentStart = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPointStart = atan2( this->startPoint.GetLPoint().y - this->centerStartRightCircle.y , this->startPoint.GetLPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusTangentEnd = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPointEnd = atan2( this->endPoint.GetLPoint().y - this->centerEndRightCircle.y , this->endPoint.GetLPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;
    
    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    params.dStartAngle = RoundAngle(angleTorusTangentStart);
    params.dStopAngle = RoundAngle(angleTorusPointStart);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }


    params.ptCenter = this->centerMiddleCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    params.dStartAngle = RoundAngle( fmod(angleTorusTangentStart + 180.0, 360.0) );
    params.dStopAngle =  RoundAngle( fmod(angleTorusTangentEnd + 180.0, 360.0) );
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusMiddle = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusMiddle = LTorus_CreateNew(this->cell, this->layer, &params);
    }


    params.ptCenter = this->centerEndRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    params.dStartAngle = RoundAngle(angleTorusPointEnd);
    params.dStopAngle = RoundAngle(angleTorusTangentEnd);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    }
}

//for RLR and LRL, same methods but with 3 torus (no line)
void DubinsPath::StoreLRLPath()
{
    double angleTorusPointStart, angleTorusTangentStart, angleTorusPointEnd, angleTorusTangentEnd;

    angleTorusTangentStart = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPointStart = atan2( this->startPoint.GetLPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetLPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusTangentEnd = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPointEnd = atan2( this->endPoint.GetLPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetLPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    
    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    params.dStartAngle = RoundAngle(angleTorusPointStart);
    params.dStopAngle = RoundAngle(angleTorusTangentStart);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);
    }


    params.ptCenter = this->centerMiddleCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    params.dStopAngle = RoundAngle( fmod(angleTorusTangentStart + 180.0, 360.0) );
    params.dStartAngle =  RoundAngle( fmod(angleTorusTangentEnd + 180.0, 360.0) );
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusMiddle = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusMiddle = LTorus_CreateNew(this->cell, this->layer, &params);
    }


    params.ptCenter = this->centerEndLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0 - this->offsetValue;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0 - this->offsetValue;
    params.dStartAngle = RoundAngle(angleTorusTangentEnd);
    params.dStopAngle = RoundAngle(angleTorusPointEnd);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    else
    {
        params.ptCenter = LPoint_Set(0,0);
        params.nInnerRadius = 0.001;
        params.nOuterRadius = 0.002;
        params.dStartAngle = 0;
        params.dStopAngle = 0;
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
    }
}

//rasterize for every type of path
void DubinsPath::RasterizePath(bool needToRasterize)
{
    bool startTorusExist, endTorusExist;
    LPoint ptCenter;
    LCoord nRadius;
    this->nbPoints = 0;

    if(needToRasterize)
    {
        if(this->type == RSL)
        {
            
            LTorusParams startTorusParams, endTorusParams;
            LTorus_GetParams(this->torusStart, &startTorusParams);
            LTorus_GetParams(this->torusEnd, &endTorusParams);
            double dStartAngleStartTorus = startTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleStartTorus = startTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleEndTorus = endTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleEndTorus = endTorusParams.dStopAngle * M_PI / 180.0;

            if(startTorusParams.dStartAngle == 0.001 && startTorusParams.dStopAngle == 0.002)
                startTorusExist = false;
            else
                startTorusExist = true;
            if(endTorusParams.dStartAngle == 0.001 && endTorusParams.dStopAngle == 0.002)
                endTorusExist = false;
            else
                endTorusExist = true;

            while (dStopAngleStartTorus < dStartAngleStartTorus)
                dStopAngleStartTorus += 2.0 * M_PI;
            while (dStopAngleEndTorus < dStartAngleEndTorus)
                dStopAngleEndTorus += 2.0 * M_PI;

            //small radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nInnerRadius;
                this->Add( ptCenter.x + nRadius * cos( dStartAngleStartTorus ), ptCenter.y + nRadius * sin( dStartAngleStartTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, true);
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }

            //long radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nOuterRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, false);
                this->Add( ptCenter.x + nRadius * cos( dStartAngleStartTorus ), ptCenter.y + nRadius * sin( dStartAngleStartTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }
            
            //small radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nInnerRadius;
                this->Add( ptCenter.x + nRadius * cos( dStartAngleEndTorus ), ptCenter.y + nRadius * sin( dStartAngleEndTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
                this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            //long radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nOuterRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, false);
                this->Add( ptCenter.x + nRadius * cos( dStartAngleEndTorus ), ptCenter.y + nRadius * sin( dStartAngleEndTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            LUpi_LogMessage( "RSL path\n" );
        }
        else if(this->type == LSR)
        {

            LTorusParams startTorusParams, endTorusParams;
            LTorus_GetParams(this->torusStart, &startTorusParams);
            LTorus_GetParams(this->torusEnd, &endTorusParams);
            double dStartAngleStartTorus = startTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleStartTorus = startTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleEndTorus = endTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleEndTorus = endTorusParams.dStopAngle * M_PI / 180.0;

            if(startTorusParams.dStartAngle == 0.001 && startTorusParams.dStopAngle == 0.002)
                startTorusExist = false;
            else
                startTorusExist = true;
            if(endTorusParams.dStartAngle == 0.001 && endTorusParams.dStopAngle == 0.002)
                endTorusExist = false;
            else
                endTorusExist = true;

            while (dStopAngleStartTorus < dStartAngleStartTorus)
                dStopAngleStartTorus += 2.0 * M_PI;
            while (dStopAngleEndTorus < dStartAngleEndTorus)
                dStopAngleEndTorus += 2.0 * M_PI;

            //small radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nInnerRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, true);
                this->Add( ptCenter.x + nRadius * cos( dStopAngleStartTorus ), ptCenter.y + nRadius * sin( dStopAngleStartTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }


            //long radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nOuterRadius;
                this->Add( ptCenter.x + nRadius * cos( dStopAngleEndTorus ), ptCenter.y + nRadius * sin( dStopAngleEndTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, false);
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            
            
            //small radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nInnerRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, true);
                this->Add( ptCenter.x + nRadius * cos( dStopAngleEndTorus ), ptCenter.y + nRadius * sin( dStopAngleEndTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            //long radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nOuterRadius;
                this->Add( ptCenter.x + nRadius * cos( dStopAngleStartTorus ), ptCenter.y + nRadius * sin( dStopAngleStartTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, false);
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }
            

            LUpi_LogMessage( "LSR path\n" );
        }
        else if(this->type == LSL)
        {
            LTorusParams startTorusParams, endTorusParams;
            LTorus_GetParams(this->torusStart, &startTorusParams);
            LTorus_GetParams(this->torusEnd, &endTorusParams);
            double dStartAngleStartTorus = startTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleStartTorus = startTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleEndTorus = endTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleEndTorus = endTorusParams.dStopAngle * M_PI / 180.0;

            if(startTorusParams.dStartAngle == 0.001 && startTorusParams.dStopAngle == 0.002)
                startTorusExist = false;
            else
                startTorusExist = true;
            if(endTorusParams.dStartAngle == 0.001 && endTorusParams.dStopAngle == 0.002)
                endTorusExist = false;
            else
                endTorusExist = true;

            while (dStopAngleStartTorus < dStartAngleStartTorus)
                dStopAngleStartTorus += 2.0 * M_PI;
            while (dStopAngleEndTorus < dStartAngleEndTorus)
                dStopAngleEndTorus += 2.0 * M_PI;

            //small radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nInnerRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, true);
                this->Add( ptCenter.x + nRadius * cos( dStopAngleStartTorus ), ptCenter.y + nRadius * sin( dStopAngleStartTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }
            

            //small radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nInnerRadius;
                this->Add( ptCenter.x + nRadius * cos( dStartAngleEndTorus ), ptCenter.y + nRadius * sin( dStartAngleEndTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, true);
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            //long radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nOuterRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, false);
                this->Add( ptCenter.x + nRadius * cos( dStartAngleEndTorus ), ptCenter.y + nRadius * sin( dStartAngleEndTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            //long radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nOuterRadius;
                this->Add( ptCenter.x + nRadius * cos( dStopAngleStartTorus ), ptCenter.y + nRadius * sin( dStopAngleStartTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, false);
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }
            

            LUpi_LogMessage( "LSL path\n" );
        }
        else if(this->type == RSR)
        {
            LTorusParams startTorusParams, endTorusParams;
            LTorus_GetParams(this->torusStart, &startTorusParams);
            LTorus_GetParams(this->torusEnd, &endTorusParams);
            double dStartAngleStartTorus = startTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleStartTorus = startTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleEndTorus = endTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleEndTorus = endTorusParams.dStopAngle * M_PI / 180.0;

            if(startTorusParams.dStartAngle == 0.001 && startTorusParams.dStopAngle == 0.002)
                startTorusExist = false;
            else
                startTorusExist = true;
            if(endTorusParams.dStartAngle == 0.001 && endTorusParams.dStopAngle == 0.002)
                endTorusExist = false;
            else
                endTorusExist = true;

            while (dStopAngleStartTorus < dStartAngleStartTorus)
                dStopAngleStartTorus += 2.0 * M_PI;
            while (dStopAngleEndTorus < dStartAngleEndTorus)
                dStopAngleEndTorus += 2.0 * M_PI;

            //small radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nInnerRadius;
                this->Add( ptCenter.x + nRadius * cos( dStartAngleStartTorus ), ptCenter.y + nRadius * sin( dStartAngleStartTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, true);
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }
            

            //long radius, start torus
            if(startTorusExist)
            {
                ptCenter = startTorusParams.ptCenter;
                nRadius = startTorusParams.nOuterRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, false);
                this->Add( ptCenter.x + nRadius * cos( dStartAngleStartTorus ), ptCenter.y + nRadius * sin( dStartAngleStartTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("Start radius does not exist"));
            }
            
            
            //long radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nOuterRadius;
                this->Add( ptCenter.x + nRadius * cos( dStopAngleEndTorus ), ptCenter.y + nRadius * sin( dStopAngleEndTorus ) );
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, false);
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            //small radius, end torus
            if(endTorusExist)
            {
                ptCenter = endTorusParams.ptCenter;
                nRadius = endTorusParams.nInnerRadius;
                //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
                this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, true);
                this->Add( ptCenter.x + nRadius * cos( dStopAngleEndTorus ), ptCenter.y + nRadius * sin( dStopAngleEndTorus ) );
            }
            else
            {
                LUpi_LogMessage(LFormat("End radius does not exist"));
            }
            

            LUpi_LogMessage( "RSR path\n" );
        }
        else if(this->type == LRL)
        {
            LTorusParams startTorusParams, endTorusParams, middleTorusParams;
            LTorus_GetParams(this->torusStart, &startTorusParams);
            LTorus_GetParams(this->torusEnd, &endTorusParams);
            LTorus_GetParams(this->torusMiddle, &middleTorusParams);
            double dStartAngleStartTorus = startTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleStartTorus = startTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleEndTorus = endTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleEndTorus = endTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleMiddleTorus = middleTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleMiddleTorus = middleTorusParams.dStopAngle * M_PI / 180.0;
            while (dStopAngleStartTorus < dStartAngleStartTorus)
                dStopAngleStartTorus += 2.0 * M_PI;
            while (dStopAngleEndTorus < dStartAngleEndTorus)
                dStopAngleEndTorus += 2.0 * M_PI;
            while (dStopAngleMiddleTorus < dStartAngleMiddleTorus)
                dStopAngleMiddleTorus += 2.0 * M_PI;

            //small radius, start torus
            ptCenter = startTorusParams.ptCenter;
            nRadius = startTorusParams.nInnerRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
            this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, true);

            //long radius, middle torus
            ptCenter = middleTorusParams.ptCenter;
            nRadius = middleTorusParams.nOuterRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, false);
            this->DrawArc(ptCenter, nRadius, dStartAngleMiddleTorus, dStopAngleMiddleTorus, false);

            //small radius, end torus
            ptCenter = endTorusParams.ptCenter;
            nRadius = endTorusParams.nInnerRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
            this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, true);

            //long radius, end torus
            ptCenter = endTorusParams.ptCenter;
            nRadius = endTorusParams.nOuterRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
            this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, false);

            //small radius, middle torus
            ptCenter = middleTorusParams.ptCenter;
            nRadius = middleTorusParams.nInnerRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, true);
            this->DrawArc(ptCenter, nRadius, dStartAngleMiddleTorus, dStopAngleMiddleTorus, true);

            //long radius, start torus
            ptCenter = startTorusParams.ptCenter;
            nRadius = startTorusParams.nOuterRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
            this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, false);

            LUpi_LogMessage( "LRL path\n" );
        }
        else if(this->type == RLR)
        {
            LTorusParams startTorusParams, endTorusParams, middleTorusParams;
            LTorus_GetParams(this->torusStart, &startTorusParams);
            LTorus_GetParams(this->torusEnd, &endTorusParams);
            LTorus_GetParams(this->torusMiddle, &middleTorusParams);
            double dStartAngleStartTorus = startTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleStartTorus = startTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleEndTorus = endTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleEndTorus = endTorusParams.dStopAngle * M_PI / 180.0;
            double dStartAngleMiddleTorus = middleTorusParams.dStartAngle * M_PI / 180.0;
            double dStopAngleMiddleTorus = middleTorusParams.dStopAngle * M_PI / 180.0;
            while (dStopAngleStartTorus < dStartAngleStartTorus)
                dStopAngleStartTorus += 2.0 * M_PI;
            while (dStopAngleEndTorus < dStartAngleEndTorus)
                dStopAngleEndTorus += 2.0 * M_PI;
            while (dStopAngleMiddleTorus < dStartAngleMiddleTorus)
                dStopAngleMiddleTorus += 2.0 * M_PI;

            //small radius, start torus
            ptCenter = startTorusParams.ptCenter;
            nRadius = startTorusParams.nInnerRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
            this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, false);

            //long radius, middle torus
            ptCenter = middleTorusParams.ptCenter;
            nRadius = middleTorusParams.nOuterRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, true);
            this->DrawArc(ptCenter, nRadius, dStartAngleMiddleTorus, dStopAngleMiddleTorus, true);

            //small radius, end torus
            ptCenter = endTorusParams.ptCenter;
            nRadius = endTorusParams.nInnerRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
            this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, false);

            //long radius, end torus
            ptCenter = endTorusParams.ptCenter;
            nRadius = endTorusParams.nOuterRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
            this->DrawArc(ptCenter, nRadius, dStartAngleEndTorus, dStopAngleEndTorus, true);

            //small radius, middle torus
            ptCenter = middleTorusParams.ptCenter;
            nRadius = middleTorusParams.nInnerRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, false);
            this->DrawArc(ptCenter, nRadius, dStartAngleMiddleTorus, dStopAngleMiddleTorus, false);

            //long radius, start torus
            ptCenter = startTorusParams.ptCenter;
            nRadius = startTorusParams.nOuterRadius;
            //this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
            this->DrawArc(ptCenter, nRadius, dStartAngleStartTorus, dStopAngleStartTorus, true);

            LUpi_LogMessage( "RLR path\n" );
        }

        if(this->nbPoints > 0)
        {
            LObject obj;
            obj = LPolygon_New(this->cell, this->layer, this->point_arr, this->nbPoints);
            double dist = LFile_IntUtoMicrons(this->file, this->distance);
            LEntity_AssignProperty( (LEntity)obj, "PathLength", L_real, &dist);
        }
    }
    
    //test for the oxide
    if(this->oxideSizeValue != 0)
    {
        LLayer savedLayer;
        double savedWidth;
        double savedOxideSize;
        savedLayer = this->layer;
        savedWidth = this->guideWidth;
        savedOxideSize = this->oxideSizeValue;
        this->guideWidth = this->oxideSizeValue;
        this->layer = this->oxideLayer;
        if(!needToRasterize) //if no rasterizer, save the polygons
        {
            this->torusStart_saved = this->torusStart;
            this->torusEnd_saved = this->torusEnd;
            this->line_saved = this->line;
            this->torusMiddle_saved = this->torusMiddle;
        }
        else
        {
            LObject_Delete( this->cell, this->torusStart );
            LObject_Delete( this->cell, this->torusEnd );
            LObject_Delete( this->cell, this->torusMiddle );
            LObject_Delete( this->cell, this->line );
        }
        switch(this->type)
        {
            case RSR:
                this->StoreRSRPath();
                break;
            case LSL:
                this->StoreLSLPath();
                break;
            case RSL:
                this->StoreRSLPath();
                break;
            case LSR:
                this->StoreLSRPath();
                break;
            case RLR:
                this->StoreRLRPath();
                break;
            case LRL:
                this->StoreLRLPath();
                break;
            default:
                LDialog_AlertBox(LFormat("Path error"));
        }
        //to not have an infinite loop
        this->oxideSizeValue = 0;
        //put back the previous parameter
        this->guideWidth = savedWidth;
        this->layer = savedLayer;
        this->oxideSizeValue = savedOxideSize;
    }
    else
    {
        if(!needToRasterize)
        {
            this->torusStart_saved = this->torusStart;
            this->torusEnd_saved = this->torusEnd;
            this->line_saved = this->line;
            this->torusMiddle_saved = this->torusMiddle;
        }
        else
        {
            LObject_Delete( this->cell, this->torusStart );
            LObject_Delete( this->cell, this->torusEnd );
            LObject_Delete( this->cell, this->torusMiddle );
            LObject_Delete( this->cell, this->line );
        }
        
    }
}


void DubinsPath::DrawArc(LPoint center, LCoord radius, double startAngle, double stopAngle, bool isCCW)
{
    double dThetaStep = 0;
    LGrid_v16_30 grid;
	LFile_GetGrid_v16_30( this->file, &grid );
    
    if(isCCW)
    {
        this->Add( center.x + radius * cos( startAngle ), center.y + radius * sin( startAngle ) );
		//dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 10);
        dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 8);
		for (double dTheta = startAngle; dTheta < stopAngle; dTheta += dThetaStep )
			this->Add( center.x + radius * cos( dTheta ), center.y + radius * sin( dTheta ) );
		this->Add( center.x + radius * cos( stopAngle ), center.y + radius * sin( stopAngle ) );
    }
    else
    {
        this->Add( center.x + radius * cos( stopAngle ), center.y + radius * sin( stopAngle ) );
		//dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 10);
        dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 8);
		for (double dTheta = stopAngle; dTheta > startAngle; dTheta -= dThetaStep )
			this->Add( center.x + radius * cos( dTheta ), center.y + radius * sin( dTheta ) );
		this->Add( center.x + radius * cos( startAngle ), center.y + radius * sin( startAngle ) );
    }
}

//when we select bezier curves instead of circle
void DubinsPath::DubinsPathWithBezierCurves()
{
    double xStartCurve1, yStartCurve1, xEndCurve1, yEndCurve1;
    double xStartCurve2, yStartCurve2, xEndCurve2, yEndCurve2;

    double angleStartCurve1, angleEndCurve1;
    double angleStartCurve2, angleEndCurve2;

    double coef;

    double distXCurve1, distYCurve1;
    double distXCurve2, distYCurve2;

    LPoint controlStartCurve1, controlEndCurve1, controlStartCurve2, controlEndCurve2; 

    LPoint curve1_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPointsCurve1 = 0;
    LPoint curve2_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPointsCurve2 = 0;
    LPoint point_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPoints = 0;
    LPoint middle_arr[MAX_POLYGON_SIZE_BEZIER];
    int nbPointsMiddle = 0;

    LPoint save1, save2, save3, save4;

    LCoord x, y;
    double dx, dy;
    double dist1, dist2;

    double angle, angle1, angle2;

    int i = 0;
    int j = 0;

    double t = 0;

    xStartCurve1 = this->startPoint.GetPoint().x;
    yStartCurve1 = this->startPoint.GetPoint().y;
    xEndCurve1 = this->startTangent.x;
    yEndCurve1 = this->startTangent.y;

    xStartCurve2 = this->endTangent.x;
    yStartCurve2 = this->endTangent.y;
    xEndCurve2 = this->endPoint.GetPoint().x;
    yEndCurve2 = this->endPoint.GetPoint().y;

    distXCurve1 = xEndCurve1 - xStartCurve1;
    distYCurve1 = yEndCurve1 - yStartCurve1;
    distXCurve2 = xEndCurve2 - xStartCurve2;
    distYCurve2 = yEndCurve2 - yStartCurve2;
    if(distXCurve1<0)
        distXCurve1=-distXCurve1;
    if(distYCurve1<0)
        distYCurve1=-distYCurve1;
    if(distXCurve2<0)
        distXCurve2=-distXCurve2;
    if(distYCurve2<0)
        distYCurve2=-distYCurve2;

    dx = xStartCurve2 - xEndCurve1;
    dy = yStartCurve2 - yEndCurve1;

    angleStartCurve1 = this->startPoint.GetAngleRadian();
    angleEndCurve1 = atan2(dy, dx);
    angleStartCurve2 = angleEndCurve1;
    angleEndCurve2 = this->endPoint.GetAngleRadian();

    coef = 1 - this->paramBezier;

    if(this->type == RSR || this->type == LSL || this->type== RSL || this->type == LSR) //only 2 bezier in those cases
    {
        dist1 = PointDistance(this->startPoint.GetLPoint(), this->startTangent)/1.3;
        dist2 = PointDistance(this->endTangent, this->endPoint.GetLPoint())/1.3;

        //compute controle points
        controlStartCurve1.x = (LCoord)round ( xStartCurve1 + dist1 * coef * cos(angleStartCurve1) );
        controlStartCurve1.y = (LCoord)round ( yStartCurve1 + dist1 * coef * sin(angleStartCurve1) );
        controlEndCurve1.x = (LCoord)round ( xEndCurve1 + dist1 * coef * cos(angleEndCurve1 + M_PI) );
        controlEndCurve1.y = (LCoord)round ( yEndCurve1 + dist1 * coef * sin(angleEndCurve1 + M_PI) );
        controlStartCurve2.x = (LCoord)round ( xStartCurve2 + dist2 * coef * cos(angleStartCurve2) );
        controlStartCurve2.y = (LCoord)round ( yStartCurve2 + dist2 * coef * sin(angleStartCurve2) );
        controlEndCurve2.x = (LCoord)round ( xEndCurve2 + dist2 * coef * cos(angleEndCurve2 + M_PI) );
        controlEndCurve2.y = (LCoord)round ( yEndCurve2 + dist2 * coef * sin(angleEndCurve2 + M_PI) );

        //construct the curve1
        curve1_arr[nbPointsCurve1] = LPoint_Set( xStartCurve1, yStartCurve1 );
        nbPointsCurve1 = nbPointsCurve1 + 1;
        for(t=0.0005; t<1; t=t+0.0005)
        {
            x = xStartCurve1*pow((1-t),3) + 3*controlStartCurve1.x*pow((1-t),2)*t + 3*controlEndCurve1.x*(1-t)*pow(t,2) + xEndCurve1*pow(t,3);
            y = yStartCurve1*pow((1-t),3) + 3*controlStartCurve1.y*pow((1-t),2)*t + 3*controlEndCurve1.y*(1-t)*pow(t,2) + yEndCurve1*pow(t,3);
            curve1_arr[nbPointsCurve1] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
            nbPointsCurve1 = nbPointsCurve1 + 1;
        }
        curve1_arr[nbPointsCurve1] = LPoint_Set( xEndCurve1, yEndCurve1 );
        nbPointsCurve1 = nbPointsCurve1 + 1;

        //construct the curve2
        curve2_arr[nbPointsCurve2] = LPoint_Set( xStartCurve2, yStartCurve2 );
        nbPointsCurve2 = nbPointsCurve2 + 1;
        for(t=0.0005; t<1; t=t+0.0005)
        {
            x = xStartCurve2*pow((1-t),3) + 3*controlStartCurve2.x*pow((1-t),2)*t + 3*controlEndCurve2.x*(1-t)*pow(t,2) + xEndCurve2*pow(t,3);
            y = yStartCurve2*pow((1-t),3) + 3*controlStartCurve2.y*pow((1-t),2)*t + 3*controlEndCurve2.y*(1-t)*pow(t,2) + yEndCurve2*pow(t,3);
            curve2_arr[nbPointsCurve2] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
            nbPointsCurve2 = nbPointsCurve2 + 1;
        }
        curve2_arr[nbPointsCurve2] = LPoint_Set( xEndCurve1, yEndCurve1 );
        nbPointsCurve2 = nbPointsCurve2 + 1;


        //right side of curve 1
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve1 + sin(angleStartCurve1) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve1 - cos(angleStartCurve1) * this->guideWidth / 2.0));
        save1 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;
        for(i=1; i<nbPointsCurve1-1; i++)
        {
            angle = atan2( curve1_arr[i+1].y-curve1_arr[i].y , curve1_arr[i+1].x-curve1_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve1_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve1_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve1 + sin(angleEndCurve1) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve1 - cos(angleEndCurve1) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;

        //right side of curve 2
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve2 + sin(angleStartCurve2) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve2 - cos(angleStartCurve2) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;
        for(i=1; i<nbPointsCurve2-1; i++)
        {
            angle = atan2( curve2_arr[i+1].y-curve2_arr[i].y , curve2_arr[i+1].x-curve2_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve2_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve2_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve2 + sin(angleEndCurve2) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve2 - cos(angleEndCurve2) * this->guideWidth / 2.0));
        save2 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;

        //left side of curve 2
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve2 + sin(angleEndCurve2 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve2 - cos(angleEndCurve2 + M_PI) * this->guideWidth / 2.0));
        save3 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;
        for(i=nbPointsCurve2-2; i>=1; i--)
        {
            angle = atan2( curve2_arr[i-1].y-curve2_arr[i].y , curve2_arr[i-1].x-curve2_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve2_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve2_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve2 + sin(angleStartCurve2 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve2 - cos(angleStartCurve2 + M_PI) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;
        
        //left side of the curve 1
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve1 + sin(angleEndCurve1 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve1 - cos(angleEndCurve1 + M_PI) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;
        for(i=nbPointsCurve1-2; i>=1; i--)
        {
            angle = atan2( curve1_arr[i-1].y-curve1_arr[i].y , curve1_arr[i-1].x-curve1_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve1_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve1_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve1 + sin(angleStartCurve1 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve1 - cos(angleStartCurve1 + M_PI) * this->guideWidth / 2.0));
        save4 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;
    }
    else if(this->type == RLR || this->type == LRL) //with 3 torus
    {
        LPoint controlStartMiddle, controlEndMiddle; 

        dist1 = PointDistance(this->startPoint.GetLPoint(), this->startTangent)/1.3;
        dist2 = PointDistance(this->endTangent, this->endPoint.GetLPoint())/1.3;
        double distMiddle = PointDistance(this->startTangent, this->endTangent)/1.3;

        //angleStartCurve1 and angleEndCurve2 already set to the good value
        if(this->type == LRL)
        {
            angleEndCurve1 = atan2(this->startTangent.y - centerStartLeftCircle.y , this->startTangent.x - centerStartLeftCircle.x) + M_PI/2.0;
            angleStartCurve2 = atan2(this->endTangent.y - centerEndLeftCircle.y , this->endTangent.x - centerEndLeftCircle.x) + M_PI/2.0;
        }
        else if(this->type == RLR)
        {
            angleEndCurve1 = atan2(this->startTangent.y - centerStartRightCircle.y , this->startTangent.x - centerStartRightCircle.x) - M_PI/2.0;
            angleStartCurve2 = atan2(this->endTangent.y - centerEndRightCircle.y , this->endTangent.x - centerEndRightCircle.x) - M_PI/2.0;
        }
        
        //compute the control points
        controlStartCurve1.x = (LCoord)round ( xStartCurve1 + dist1 * coef * cos(angleStartCurve1) );
        controlStartCurve1.y = (LCoord)round ( yStartCurve1 + dist1 * coef * sin(angleStartCurve1) );
        controlEndCurve1.x = (LCoord)round ( xEndCurve1 + dist1 * coef * cos(angleEndCurve1 + M_PI) );
        controlEndCurve1.y = (LCoord)round ( yEndCurve1 + dist1 * coef * sin(angleEndCurve1 + M_PI) );

        controlStartMiddle.x = (LCoord)round ( xEndCurve1 + distMiddle * coef * cos(angleEndCurve1) );
        controlStartMiddle.y = (LCoord)round ( yEndCurve1 + distMiddle * coef * sin(angleEndCurve1) );
        controlEndMiddle.x = (LCoord)round ( xStartCurve2 + distMiddle * coef * cos(angleStartCurve2 + M_PI) );
        controlEndMiddle.y = (LCoord)round ( yStartCurve2 + distMiddle * coef * sin(angleStartCurve2 + M_PI) );

        controlStartCurve2.x = (LCoord)round ( xStartCurve2 + dist2 * coef * cos(angleStartCurve2) );
        controlStartCurve2.y = (LCoord)round ( yStartCurve2 + dist2 * coef * sin(angleStartCurve2) );
        controlEndCurve2.x = (LCoord)round ( xEndCurve2 + dist2 * coef * cos(angleEndCurve2 + M_PI) );
        controlEndCurve2.y = (LCoord)round ( yEndCurve2 + dist2 * coef * sin(angleEndCurve2 + M_PI) );

        //construct the curve1
        curve1_arr[nbPointsCurve1] = LPoint_Set( xStartCurve1, yStartCurve1 );
        nbPointsCurve1 = nbPointsCurve1 + 1;
        for(t=0.0005; t<1; t=t+0.0005)
        {
            x = xStartCurve1*pow((1-t),3) + 3*controlStartCurve1.x*pow((1-t),2)*t + 3*controlEndCurve1.x*(1-t)*pow(t,2) + xEndCurve1*pow(t,3);
            y = yStartCurve1*pow((1-t),3) + 3*controlStartCurve1.y*pow((1-t),2)*t + 3*controlEndCurve1.y*(1-t)*pow(t,2) + yEndCurve1*pow(t,3);
            curve1_arr[nbPointsCurve1] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
            nbPointsCurve1 = nbPointsCurve1 + 1;
        }
        curve1_arr[nbPointsCurve1] = LPoint_Set( xEndCurve1, yEndCurve1 );
        nbPointsCurve1 = nbPointsCurve1 + 1;

        //construct the middle curve
        middle_arr[nbPointsMiddle] = LPoint_Set( xEndCurve1, yEndCurve1 );
        nbPointsMiddle = nbPointsMiddle + 1;
        for(t=0.0005; t<1; t=t+0.0005)
        {
            x = xEndCurve1*pow((1-t),3) + 3*controlStartMiddle.x*pow((1-t),2)*t + 3*controlEndMiddle.x*(1-t)*pow(t,2) + xStartCurve2*pow(t,3);
            y = yEndCurve1*pow((1-t),3) + 3*controlStartMiddle.y*pow((1-t),2)*t + 3*controlEndMiddle.y*(1-t)*pow(t,2) + yStartCurve2*pow(t,3);
            middle_arr[nbPointsMiddle] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
            nbPointsMiddle = nbPointsMiddle + 1;
        }
        middle_arr[nbPointsMiddle] = LPoint_Set( xStartCurve2, yStartCurve2 );
        nbPointsMiddle = nbPointsMiddle + 1;

        //construct the curve2
        curve2_arr[nbPointsCurve2] = LPoint_Set( xStartCurve2, yStartCurve2 );
        nbPointsCurve2 = nbPointsCurve2 + 1;
        for(t=0.0005; t<1; t=t+0.0005)
        {
            x = xStartCurve2*pow((1-t),3) + 3*controlStartCurve2.x*pow((1-t),2)*t + 3*controlEndCurve2.x*(1-t)*pow(t,2) + xEndCurve2*pow(t,3);
            y = yStartCurve2*pow((1-t),3) + 3*controlStartCurve2.y*pow((1-t),2)*t + 3*controlEndCurve2.y*(1-t)*pow(t,2) + yEndCurve2*pow(t,3);
            curve2_arr[nbPointsCurve2] = LPoint_Set( RoundToLong(x), RoundToLong(y) );
            nbPointsCurve2 = nbPointsCurve2 + 1;
        }
        curve2_arr[nbPointsCurve2] = LPoint_Set( xEndCurve1, yEndCurve1 );
        nbPointsCurve2 = nbPointsCurve2 + 1;



        //right side of curve 1
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve1 + sin(angleStartCurve1) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve1 - cos(angleStartCurve1) * this->guideWidth / 2.0));
        save1 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;
        for(i=1; i<nbPointsCurve1-1; i++)
        {
            angle = atan2( curve1_arr[i+1].y-curve1_arr[i].y , curve1_arr[i+1].x-curve1_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve1_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve1_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve1 + sin(angleEndCurve1) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve1 - cos(angleEndCurve1) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;

        //right side of middle curve
        for(i=1; i<nbPointsMiddle-1; i++)
        {
            angle = atan2( middle_arr[i+1].y-middle_arr[i].y , middle_arr[i+1].x-middle_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(middle_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(middle_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }

        //right side of curve 2
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve2 + sin(angleStartCurve2) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve2 - cos(angleStartCurve2) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;
        for(i=1; i<nbPointsCurve2-1; i++)
        {
            angle = atan2( curve2_arr[i+1].y-curve2_arr[i].y , curve2_arr[i+1].x-curve2_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve2_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve2_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve2 + sin(angleEndCurve2) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve2 - cos(angleEndCurve2) * this->guideWidth / 2.0));
        save2 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;

        //left side of curve 2
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve2 + sin(angleEndCurve2 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve2 - cos(angleEndCurve2 + M_PI) * this->guideWidth / 2.0));
        save3 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;
        for(i=nbPointsCurve2-2; i>=1; i--)
        {
            angle = atan2( curve2_arr[i-1].y-curve2_arr[i].y , curve2_arr[i-1].x-curve2_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve2_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve2_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve2 + sin(angleStartCurve2 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve2 - cos(angleStartCurve2 + M_PI) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;

        //left side of middle curve
        for(i=nbPointsMiddle-2; i>=1; i--)
        {
            angle = atan2( middle_arr[i-1].y-middle_arr[i].y , middle_arr[i-1].x-middle_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(middle_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(middle_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        
        //left side of the curve 1
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xEndCurve1 + sin(angleEndCurve1 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yEndCurve1 - cos(angleEndCurve1 + M_PI) * this->guideWidth / 2.0));
        nbPoints = nbPoints + 1;
        for(i=nbPointsCurve1-2; i>=1; i--)
        {
            angle = atan2( curve1_arr[i-1].y-curve1_arr[i].y , curve1_arr[i-1].x-curve1_arr[i].x );
            point_arr[nbPoints] = LPoint_Set((LCoord)round(curve1_arr[i].x + sin(angle) * this->guideWidth / 2.0) , (LCoord)round(curve1_arr[i].y - cos(angle) * this->guideWidth / 2.0));
            nbPoints = nbPoints + 1;
        }
        point_arr[nbPoints] = LPoint_Set((LCoord)round(xStartCurve1 + sin(angleStartCurve1 + M_PI) * this->guideWidth / 2.0) , (LCoord)round(yStartCurve1 - cos(angleStartCurve1 + M_PI) * this->guideWidth / 2.0));
        save4 = point_arr[nbPoints];
        nbPoints = nbPoints + 1;
    }

    //delete the points that intersect with the polygon
    j=1;
    while(j != 0)
    {
        j=0;
        for(i=0; i<nbPoints; i++)
        {
            if((save1.x==point_arr[i].x && save1.y==point_arr[i].y) || (save2.x==point_arr[i].x && save2.y==point_arr[i].y) || (save3.x==point_arr[i].x && save3.y==point_arr[i].y) || (save4.x==point_arr[i].x && save4.y==point_arr[i].y))
                continue;
            if(i==0)
                angle1 = atan2(point_arr[0].y-point_arr[nbPoints-1].y,point_arr[0].x-point_arr[nbPoints-1].x) - M_PI;
            else
                angle1 = atan2(point_arr[i].y-point_arr[i-1].y,point_arr[i].x-point_arr[i-1].x) - M_PI;
            if(i==nbPoints-1)
                angle2 = atan2(point_arr[nbPoints-1].y-point_arr[0].y,point_arr[nbPoints-1].x-point_arr[0].x);
            else
                angle2 = atan2(point_arr[i].y-point_arr[i+1].y,point_arr[i].x-point_arr[i+1].x);
            angle = angle2 - angle1;
            angle = fmod(angle, 2*M_PI);
            while(angle < 0)
                angle = angle + 2*M_PI;
            if( (angle > M_PI - ANGLE_LIMIT && angle < M_PI +ANGLE_LIMIT) ) //if not in the limit range
            {
                for(j=i; j<nbPoints; j++)
                    point_arr[j]=point_arr[(j+1)%nbPoints];
                nbPoints = nbPoints - 1;
                j=1;
            }
        }
    }

    //create the polygon
    LObject obj;
    obj = LPolygon_New( this->cell, this->layer, point_arr, nbPoints );
    double dist;
    
    dist = LFile_IntUtoMicrons(this->file, ArrayDistance(point_arr, nbPoints/2));
    
    LEntity_AssignProperty( (LEntity)obj, "PathLength", L_real, &dist);
    
    //if we need a oxide
    if(this->oxideSizeValue != 0)
    {
        LLayer savedLayer = this->layer;
        double savedGuideWidth = this->guideWidth;
        double savedOxideSize = this->oxideSizeValue;
        this->layer = this->oxideLayer;
        this->guideWidth = this->oxideSizeValue;
        this->oxideSizeValue = 0;
        this->DubinsPathWithBezierCurves();

        this->layer = savedLayer;
        this->guideWidth = savedGuideWidth;
        this->oxideSizeValue = savedOxideSize;
    }
    else
    {
        LObject_Delete( this->cell, this->torusStart );
        LObject_Delete( this->cell, this->torusEnd );
        LObject_Delete( this->cell, this->torusMiddle );
        LObject_Delete( this->cell, this->line );
    }
    
}


void DubinsPath::Add( double x, double y )
{
	//LCoord nx = round( x );
	//LCoord ny = round( y );

    LCoord nx = (LCoord)Round0or5( x );
	LCoord ny = (LCoord)Round0or5( y );

	if ( this->nbPoints != 0 && nx == nLastx && ny == nLasty )
		return; // do not duplicate vertex

	this->point_arr[this->nbPoints] = LPoint_Set(nx, ny);
    this->nbPoints = this->nbPoints + 1;
	
	nLastx = nx;
	nLasty = ny;

}



//NON CLASS FUNCTIONS

double RoundAngle(double value)
{
    long Ltmp = 0;
    double Dtmp = value * 1000;
    double returnVal;
    if(value > 0)
        Dtmp = (double)(Dtmp + 0.5);
    else
        Dtmp = (double)(Dtmp - 0.5);
    Ltmp = (long)Dtmp;
    returnVal = Ltmp / 1000.0;
    return returnVal;
}

double Round0or5(double val)
{
	double tmpFloat = val;
	long tmpInt = 0;
	double returnedVal = 0;
	if(val >= 0)
		tmpFloat = (double)(tmpFloat + 2.5)/5.0;
	else
		tmpFloat = (double)(tmpFloat - 2.5)/5.0;
	tmpInt = (long)tmpFloat; //delete the digits after the '.'
	returnedVal = tmpInt*5.0;
	return returnedVal;
}