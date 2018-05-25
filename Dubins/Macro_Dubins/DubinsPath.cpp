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
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerStartRightCircle = Lcenter;

    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() + M_PI/2.0);
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerStartLeftCircle = Lcenter;

    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() - M_PI/2.0);
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerEndRightCircle = Lcenter;

    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() + M_PI/2.0);
    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;
    this->centerEndLeftCircle = Lcenter;
    
    return LStatusOK;
}

//// PUBLIC METHODS ////

DubinsPath::DubinsPath(){}

LStatus DubinsPath::SetGuideWidth(LCoord width){
    this->guideWidth = LFile_MicronsToIntU( this->file, width );
    return LStatusOK;
}

LStatus DubinsPath::SetStartPoint(DubinsPoint startPoint){
    this->startPoint.SetPoint(LPoint_Set(LFile_MicronsToIntU( this->file, startPoint.GetPoint().x ),LFile_MicronsToIntU( this->file, startPoint.GetPoint().y )));
    this->startPoint.SetAngleRadian(startPoint.GetAngleRadian());
    return this->UpdateCircleCenter();
}

LStatus DubinsPath::SetEndPoint(DubinsPoint endPoint){
    this->endPoint.SetPoint(LPoint_Set(LFile_MicronsToIntU( this->file, endPoint.GetPoint().x ),LFile_MicronsToIntU( this->file, endPoint.GetPoint().y )));
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

    returnDistance = this->GetArcLength(this->centerStartRightCircle, this->startPoint.GetPoint(), this->startTangent, false);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndRightCircle, this->endTangent, this->endPoint.GetPoint(), false);

    return returnDistance;
}

float DubinsPath::ComputeLSLLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartLeftCircle, this->startPoint.GetPoint(), this->startTangent, true);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndLeftCircle, this->endTangent, this->endPoint.GetPoint(), true);

    return returnDistance;
}

float DubinsPath::ComputeRSLLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartRightCircle, this->startPoint.GetPoint(), this->startTangent, false);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndLeftCircle, this->endTangent, this->endPoint.GetPoint(), true);

    return returnDistance;
}

float DubinsPath::ComputeLSRLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartLeftCircle, this->startPoint.GetPoint(), this->startTangent, true);
    returnDistance += PointDistance(this->startTangent, this->endTangent);
    returnDistance += this->GetArcLength(this->centerEndRightCircle, this->endTangent, this->endPoint.GetPoint(), false);

    return returnDistance;
}

float DubinsPath::ComputeRLRLength()
{
    float returnDistance;

    returnDistance = this->GetArcLength(this->centerStartRightCircle, this->startPoint.GetPoint(), this->startTangent, false);
    returnDistance += this->GetArcLength(this->centerMiddleCircle, this->startTangent, this->endTangent, true);
    returnDistance += this->GetArcLength(this->centerEndRightCircle, this->endTangent, this->endPoint.GetPoint(), false);

    return returnDistance;
}

float DubinsPath::ComputeLRLLength()
{
    float returnDistance;
    
    returnDistance = this->GetArcLength(this->centerStartLeftCircle, this->startPoint.GetPoint(), this->startTangent, true);
    returnDistance += this->GetArcLength(this->centerMiddleCircle, this->startTangent, this->endTangent, false);
    returnDistance += this->GetArcLength(this->centerEndLeftCircle, this->endTangent, this->endPoint.GetPoint(), true);
    
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
        //Lenght
        returnDistance = this->ComputeRSRLength();

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

        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = LSL;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
        }
    }

    float circleDistanceSqr = (xEnd-xStart)*(xEnd-xStart)+(yEnd-yStart)*(yEnd-yStart);
    float comparaisonDistanceSqr = (2*this->radius)*(2*this->radius);

    //RSL
    if( circleDistanceSqr > comparaisonDistanceSqr ) //circle don't intersect
    {
        this->GetRSLorLSRTangent(this->centerStartRightCircle, this->centerEndLeftCircle, false);

        returnDistance = this->ComputeRSLLength();

        if(returnDistance < shortestDistance){
            shortestDistance = returnDistance;
            shortestType = RSL;
            shortestStartTangent = this->startTangent;
            shortestEndTangent = this->endTangent;
        }
    }    

    //LSR
    if( circleDistanceSqr > comparaisonDistanceSqr ) //circle don't intersect
    {
        this->GetRSLorLSRTangent(this->centerStartLeftCircle, this->centerEndRightCircle, true);
         
        returnDistance = this->ComputeLSRLength();

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
        this->GetRLRorLRLTangent(this->centerStartRightCircle, this->centerEndRightCircle, true);
        
        returnDistance = this->ComputeRLRLength();
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
        this->GetRLRorLRLTangent(this->centerStartLeftCircle, this->centerEndLeftCircle, false);

        returnDistance = this->ComputeLRLLength();
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
    if( shortestType == LRL || shortestType == RLR )
        this->centerMiddleCircle = shortestMiddleCenter;
    else
        this->centerMiddleCircle = LPoint_Set(0,0);



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



void DubinsPath::StoreRSRPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetPoint().y - this->centerStartRightCircle.y , this->startPoint.GetPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    
    params.dStartAngle = angleTorusTangent;
    params.dStopAngle = angleTorusPoint;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);

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
    angleTorusPoint = atan2( this->endPoint.GetPoint().y - this->centerEndRightCircle.y , this->endPoint.GetPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;
    
    params.dStartAngle = angleTorusPoint;
    params.dStopAngle = angleTorusTangent;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
}

void DubinsPath::StoreLSLPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    
    params.dStartAngle = angleTorusPoint;
    params.dStopAngle = angleTorusTangent;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);

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
    angleTorusPoint = atan2( this->endPoint.GetPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    
    params.dStartAngle = angleTorusTangent;
    params.dStopAngle = angleTorusPoint;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
}

void DubinsPath::StoreRSLPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetPoint().y - this->centerStartRightCircle.y , this->startPoint.GetPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    
    params.dStartAngle = angleTorusTangent;
    params.dStopAngle = angleTorusPoint;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);

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
    angleTorusPoint = atan2( this->endPoint.GetPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;

    params.dStartAngle = angleTorusTangent;
    params.dStopAngle = angleTorusPoint;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
}

void DubinsPath::StoreLSRPath()
{
    double angleTorusPoint, angleTorusTangent;

    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    angleTorusTangent = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPoint = atan2( this->startPoint.GetPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    
    params.dStartAngle = angleTorusPoint;
    params.dStopAngle = angleTorusTangent;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);

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
    angleTorusPoint = atan2( this->endPoint.GetPoint().y - this->centerEndRightCircle.y , this->endPoint.GetPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;

    params.dStartAngle = angleTorusPoint;
    params.dStopAngle = angleTorusTangent;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
}

void DubinsPath::StoreRLRPath()
{
    double angleTorusPointStart, angleTorusTangentStart, angleTorusPointEnd, angleTorusTangentEnd;

    angleTorusTangentStart = atan2( this->startTangent.y - this->centerStartRightCircle.y , this->startTangent.x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusPointStart = atan2( this->startPoint.GetPoint().y - this->centerStartRightCircle.y , this->startPoint.GetPoint().x - this->centerStartRightCircle.x ) *180.0/M_PI;
    angleTorusTangentEnd = atan2( this->endTangent.y - this->centerEndRightCircle.y , this->endTangent.x - this->centerEndRightCircle.x ) *180.0/M_PI;
    angleTorusPointEnd = atan2( this->endPoint.GetPoint().y - this->centerEndRightCircle.y , this->endPoint.GetPoint().x - this->centerEndRightCircle.x ) *180.0/M_PI;
    
    LTorusParams params;
    params.ptCenter = this->centerStartRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    params.dStartAngle = angleTorusTangentStart;
    params.dStopAngle = angleTorusPointStart;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);


    params.ptCenter = this->centerMiddleCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    params.dStartAngle = std::fmod(angleTorusTangentStart + 180.0, 360.0);
    params.dStopAngle =  std::fmod(angleTorusTangentEnd + 180.0, 360.0);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusMiddle = LTorus_CreateNew(this->cell, this->layer, &params);


    params.ptCenter = this->centerEndRightCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    params.dStartAngle = angleTorusPointEnd;
    params.dStopAngle = angleTorusTangentEnd;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
}

void DubinsPath::StoreLRLPath()
{
    double angleTorusPointStart, angleTorusTangentStart, angleTorusPointEnd, angleTorusTangentEnd;

    angleTorusTangentStart = atan2( this->startTangent.y - this->centerStartLeftCircle.y , this->startTangent.x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusPointStart = atan2( this->startPoint.GetPoint().y - this->centerStartLeftCircle.y , this->startPoint.GetPoint().x - this->centerStartLeftCircle.x ) *180.0/M_PI;
    angleTorusTangentEnd = atan2( this->endTangent.y - this->centerEndLeftCircle.y , this->endTangent.x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    angleTorusPointEnd = atan2( this->endPoint.GetPoint().y - this->centerEndLeftCircle.y , this->endPoint.GetPoint().x - this->centerEndLeftCircle.x ) *180.0/M_PI;
    
    LTorusParams params;
    params.ptCenter = this->centerStartLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    params.dStartAngle = angleTorusPointStart;
    params.dStopAngle = angleTorusTangentStart;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusStart = LTorus_CreateNew(this->cell, this->layer, &params);


    params.ptCenter = this->centerMiddleCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    params.dStopAngle = std::fmod(angleTorusTangentStart + 180.0, 360.0);
    params.dStartAngle =  std::fmod(angleTorusTangentEnd + 180.0, 360.0);
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusMiddle = LTorus_CreateNew(this->cell, this->layer, &params);


    params.ptCenter = this->centerEndLeftCircle;
    params.nInnerRadius = this->radius - this->guideWidth / 2.0;
    params.nOuterRadius = this->radius + this->guideWidth / 2.0;
    params.dStartAngle = angleTorusTangentEnd;
    params.dStopAngle = angleTorusPointEnd;
    
    if(params.dStartAngle != params.dStopAngle)
        this->torusEnd = LTorus_CreateNew(this->cell, this->layer, &params);
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
