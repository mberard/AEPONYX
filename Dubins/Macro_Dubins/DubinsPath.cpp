#include "DubinsPath.hpp"
#include <math.h>
#include "ldata.h"

DubinsPath::DubinsPath(){}

LStatus DubinsPath::SetPathType(PathType type){
    this->type = type;
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


//// PRIVATE METHODS ////

LStatus DubinsPath::UpdateCircleCenter(){
    DPoint Dcenter;
    LPoint Lcenter;
    LCoord radius = this->radius;

    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() - M_PI/2.0);

    float x = Dcenter.x;
    float y = Dcenter.y;

    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;

    LUpi_LogMessage( LFormat("START POINT : %lf %lf %f\n", startPoint.GetPoint().x, startPoint.GetPoint().y, startPoint.GetAngleRadian() ) );
    LUpi_LogMessage( LFormat("END POINT : %lf %lf %f\n", endPoint.GetPoint().x, endPoint.GetPoint().y, endPoint.GetAngleRadian() ) );

    LUpi_LogMessage( LFormat("centre cercle droit debut : %lf %lf\n", x, y ) );
    this->centerStartRightCircle = Lcenter;



    Dcenter.x = startPoint.GetPoint().x + radius*cos(startPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = startPoint.GetPoint().y + radius*sin(startPoint.GetAngleRadian() + M_PI/2.0);

    x = Dcenter.x;
    y = Dcenter.y;

    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;

    LUpi_LogMessage( LFormat("centre cercle gauche debut : %lf %lf\n", x, y ) );
    this->centerStartLeftCircle = Lcenter;



    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() - M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() - M_PI/2.0);

    x = Dcenter.x;
    y = Dcenter.y;

    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;

    LUpi_LogMessage( LFormat("centre cercle droit fin : %lf %lf\n", x, y ) );
    this->centerEndLeftCircle = Lcenter;



    Dcenter.x = endPoint.GetPoint().x + radius*cos(endPoint.GetAngleRadian() + M_PI/2.0);
    Dcenter.y = endPoint.GetPoint().y + radius*sin(endPoint.GetAngleRadian() + M_PI/2.0);

    x = Dcenter.x;
    y = Dcenter.y;

    Lcenter.x = Dcenter.x;
    Lcenter.y = Dcenter.y;

    LUpi_LogMessage( LFormat("centre cercle gauche fin: %lf %lf\n", x, y ) );
    this->centerEndLeftCircle = Lcenter;
    
    return LStatusOK;
}