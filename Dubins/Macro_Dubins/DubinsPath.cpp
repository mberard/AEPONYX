#include "DubinsPath.hpp"
#include <math.h>
#include "ldata.h"

//// PRIVATE METHODS ////

LStatus DubinsPath::UpdateCircleCenter(){
    DPoint Dcenter;
    LPoint Lcenter;
    LCoord radius = this->radius;

    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() - M_PI/2.0);
//    Lcenter.x = Dcenter.x + this->offsetValue * cos(startPoint.GetAngleRadian() );
//    Lcenter.y = Dcenter.y - this->offsetValue * sin(startPoint.GetAngleRadian() );
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerStartRightCircle = Lcenter;

    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() + M_PI/2.0);
//    Lcenter.x = Dcenter.x - this->offsetValue * sin(startPoint.GetAngleRadian() );
//    Lcenter.y = Dcenter.y + this->offsetValue * cos(startPoint.GetAngleRadian() );
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerStartLeftCircle = Lcenter;

    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() - M_PI/2.0);
//    Lcenter.x = Dcenter.x + this->offsetValue * sin(endPoint.GetAngleRadian() );
//    Lcenter.y = Dcenter.y - this->offsetValue * cos(endPoint.GetAngleRadian() );
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerEndRightCircle = Lcenter;

    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() + M_PI/2.0);
//    Lcenter.x = Dcenter.x - this->offsetValue * cos(endPoint.GetAngleRadian() );
//    Lcenter.y = Dcenter.y + this->offsetValue * sin(endPoint.GetAngleRadian() );
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerEndLeftCircle = Lcenter;
    
    return LStatusOK;
}

//// PUBLIC METHODS ////

DubinsPath::DubinsPath(){}

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

LFile DubinsPath::GetFile(){
    return this->file;
}

LCell DubinsPath::GetCell(){
    return this->cell;
}

LLayer DubinsPath::GetLayer(){
    return this->layer;
}


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


void DubinsPath::DrawLine()
{
    LPoint point_arr[4];
    float dx, dy;
    dx = this->endPoint.GetPoint().x - this->startPoint.GetPoint().x;
    dy = this->endPoint.GetPoint().y - this->startPoint.GetPoint().y;
    point_arr[0] = LPoint_Set( this->startPoint.GetPoint().x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->startPoint.GetPoint().y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[1] = LPoint_Set( this->endPoint.GetPoint().x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->endPoint.GetPoint().y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[2] = LPoint_Set( this->endPoint.GetPoint().x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->endPoint.GetPoint().y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[3] = LPoint_Set( this->startPoint.GetPoint().x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->startPoint.GetPoint().y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);
}





void DubinsPath::StoreRSRPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetLPoint().y - this->centerStartRightCircle.y , this->startPoint.GetLPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    
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
    float dx, dy;
    dx = this->endTangent.x - this->startTangent.x;
    dy = this->endTangent.y - this->startTangent.y;
    point_arr[0] = LPoint_Set( this->startTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[1] = LPoint_Set( this->endTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[2] = LPoint_Set( this->endTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[3] = LPoint_Set( this->startTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);

    params.ptCenter = this->centerEndRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->endPoint.GetLPoint().y - this->centerEndRightCircle.y , this->endPoint.GetLPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;
    
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
}

void DubinsPath::StoreLSLPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetLPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetLPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    
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
    float dx, dy;
    dx = this->endTangent.x - this->startTangent.x;
    dy = this->endTangent.y - this->startTangent.y;
    point_arr[0] = LPoint_Set( this->startTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[1] = LPoint_Set( this->endTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[2] = LPoint_Set( this->endTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[3] = LPoint_Set( this->startTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);

    params.ptCenter = this->centerEndLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->endPoint.GetLPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetLPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    
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
}

void DubinsPath::StoreRSLPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetLPoint().y - this->centerStartRightCircle.y , this->startPoint.GetLPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    
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
    float dx, dy;
    dx = this->endTangent.x - this->startTangent.x;
    dy = this->endTangent.y - this->startTangent.y;
    point_arr[0] = LPoint_Set( this->startTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[1] = LPoint_Set( this->endTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[2] = LPoint_Set( this->endTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[3] = LPoint_Set( this->startTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);

    params.ptCenter = this->centerEndLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->endPoint.GetLPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetLPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;

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
}

void DubinsPath::StoreLSRPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetLPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetLPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;

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
    float dx, dy;
    dx = this->endTangent.x - this->startTangent.x;
    dy = this->endTangent.y - this->startTangent.y;
    point_arr[0] = LPoint_Set( this->startTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[1] = LPoint_Set( this->endTangent.x+(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y-(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[2] = LPoint_Set( this->endTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->endTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    point_arr[3] = LPoint_Set( this->startTangent.x-(sin(atan2(dy,dx))*this->guideWidth/2) , this->startTangent.y+(cos(atan2(dy,dx))*this->guideWidth/2) );
    this->line = LPolygon_New(this->cell, this->layer, point_arr, 4);

    params.ptCenter = this->centerEndRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->endPoint.GetLPoint().y - this->centerEndRightCircle.y , this->endPoint.GetLPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;

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
}

void DubinsPath::StoreRLRPath()
{
    double angleTorusPointStart, angleTorusTangentStart, angleTorusPointEnd, angleTorusTangentEnd;

    angleTorusTangentStart = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPointStart = atan2( this->startPoint.GetLPoint().y - this->centerStartRightCircle.y , this->startPoint.GetLPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusTangentEnd = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPointEnd = atan2( this->endPoint.GetLPoint().y - this->centerEndRightCircle.y , this->endPoint.GetLPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;
    
    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
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
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
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
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
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

void DubinsPath::StoreLRLPath()
{
    double angleTorusPointStart, angleTorusTangentStart, angleTorusPointEnd, angleTorusTangentEnd;

    angleTorusTangentStart = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPointStart = atan2( this->startPoint.GetLPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetLPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusTangentEnd = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPointEnd = atan2( this->endPoint.GetLPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetLPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    
    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
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
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
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
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
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

void DubinsPath::RasterizePath()
{
    bool startTorusExist, endTorusExist;
    LPoint ptCenter;
    LCoord nRadius;
    this->nbPoints = 0;

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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);
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
            this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);
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
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);

        //long radius, middle torus
        ptCenter = middleTorusParams.ptCenter;
		nRadius = middleTorusParams.nOuterRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, false);

        //small radius, end torus
        ptCenter = endTorusParams.ptCenter;
		nRadius = endTorusParams.nInnerRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);

        //long radius, end torus
        ptCenter = endTorusParams.ptCenter;
		nRadius = endTorusParams.nOuterRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);

        //small radius, middle torus
        ptCenter = middleTorusParams.ptCenter;
		nRadius = middleTorusParams.nInnerRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, true);

        //long radius, start torus
        ptCenter = startTorusParams.ptCenter;
		nRadius = startTorusParams.nOuterRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);

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
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, false);

        //long radius, middle torus
        ptCenter = middleTorusParams.ptCenter;
		nRadius = middleTorusParams.nOuterRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, true);

        //small radius, end torus
        ptCenter = endTorusParams.ptCenter;
		nRadius = endTorusParams.nInnerRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, false);

        //long radius, end torus
        ptCenter = endTorusParams.ptCenter;
		nRadius = endTorusParams.nOuterRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleEndTorus, dStopAngleEndTorus, true);

        //small radius, middle torus
        ptCenter = middleTorusParams.ptCenter;
		nRadius = middleTorusParams.nInnerRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleMiddleTorus, dStopAngleMiddleTorus, false);

        //long radius, start torus
        ptCenter = startTorusParams.ptCenter;
		nRadius = startTorusParams.nOuterRadius;
        this->DrawArc(ptCenter, nRadius-this->offsetValue, dStartAngleStartTorus, dStopAngleStartTorus, true);

        LUpi_LogMessage( "RLR path\n" );
    }

    if(this->nbPoints > 0)
        LPolygon_New(this->cell, this->layer, this->point_arr, this->nbPoints);

    LObject_Delete( this->cell, this->torusStart );
    LObject_Delete( this->cell, this->torusEnd );
    LObject_Delete( this->cell, this->torusMiddle );
    LObject_Delete( this->cell, this->line );


}


void DubinsPath::DrawArc(LPoint center, LCoord radius, double startAngle, double stopAngle, bool isCCW)
{
    double dThetaStep = 0;
    LGrid_v16_30 grid;
	LFile_GetGrid_v16_30( this->file, &grid );
    
    if(isCCW)
    {
        this->Add( center.x + radius * cos( startAngle ), center.y + radius * sin( startAngle ) );
		dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 10);
		for (double dTheta = startAngle; dTheta < stopAngle; dTheta += dThetaStep )
			this->Add( center.x + radius * cos( dTheta ), center.y + radius * sin( dTheta ) );
		this->Add( center.x + radius * cos( stopAngle ), center.y + radius * sin( stopAngle ) );
    }
    else
    {
        this->Add( center.x + radius * cos( stopAngle ), center.y + radius * sin( stopAngle ) );
		dThetaStep = 2*acos(1 - (double)grid.manufacturing_grid_size / radius / 10);
		for (double dTheta = stopAngle; dTheta > startAngle; dTheta -= dThetaStep )
			this->Add( center.x + radius * cos( dTheta ), center.y + radius * sin( dTheta ) );
		this->Add( center.x + radius * cos( startAngle ), center.y + radius * sin( startAngle ) );
    }
}


void DubinsPath::Add( double x, double y )
{
	LCoord nx = Round( x );
	LCoord ny = Round( y );

	if ( this->nbPoints != 0 && nx == nLastx && ny == nLasty )
		return; // do not duplicate vertex

	this->point_arr[this->nbPoints] = LPoint_Set(nx, ny);
    this->nbPoints = this->nbPoints + 1;
	
	nLastx = nx;
	nLasty = ny;

    //LUpi_LogMessage( LFormat("nbPoints: %d\n", this->nbPoints) );
}



//NON CLASS FUNCTIONS

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}

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

LCoord Round(double d)
{
	if (d >= 0)
		return d+0.5;
	else
		return d-0.5;
}
