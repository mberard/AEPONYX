#include <unistd.h> //getcwd

//#define EXCLUDE_LEDIT_LEGACY_UPI

#include <ldata.h>
/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */
#include <string.h>
#include <math.h>

#define MAX_POLYGON_SIZE 5000
#define ANGLE_LIMIT 0.2 //in radian, 0.523599 rad == 30 degrés, 0.785398 rad == 45 degrés, 1.5708 rad == 90 degrés
#define FILLET_VALUE 10000 //in internal units

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}


int FindRightPointWithDistance(LPoint* right, double distance, int firstPointIndex, LPoint* point_arr, int nbPointsArr, int stopPointIndex)
{
    int i = firstPointIndex;
    while(PointDistance(point_arr[firstPointIndex],point_arr[i]) < distance )
    {
        i = i + 1;
        if(i >= nbPointsArr)
            i = 0;
        if(i == stopPointIndex)
        {
            *right = point_arr[(firstPointIndex+1)%nbPointsArr];
            return stopPointIndex;
        }
            
        
LCell	pCell	=	LCell_GetVisible();
LFile	pFile	=	LCell_GetFile(pCell);
//LCircle_New(pCell, LLayer_Find ( pFile, "CIRCLE" ), point_arr[i], 1000);
    }
    *right = point_arr[i];
    return i;
}

double Vecto(LCoord x1, LCoord y1, LCoord x2, LCoord y2){
    return (x1*y2-y1*x1);
}

LPoint FindCenter(LPoint left, LPoint nextLeft , LPoint prevRight, LPoint right  )
{
    LPoint perpendiculaireLeft, perpendiculaireRight, center;
    long dx = nextLeft.x - left.x;
    long dy = nextLeft.y - left.y;
    perpendiculaireLeft = LPoint_Set(left.x + dy, left.y - dx);
    dx = right.x - prevRight.x;
    dy = right.y - prevRight.y;
    perpendiculaireRight = LPoint_Set(right.x + dy, right.y - dx);
/*
    //source link: https://openclassrooms.com/forum/sujet/calcul-du-point-d-intersection-de-deux-segments-21661
    double a = (double)Vecto(perpendiculaireLeft.x-left.x, perpendiculaireLeft.y-left.y, right.x-left.x, right.y-left.y) / PointDistance(left, perpendiculaireLeft);
    double b = (double)Vecto(perpendiculaireLeft.x-left.x, perpendiculaireLeft.y-left.y, perpendiculaireRight.x-left.x, perpendiculaireRight.y-left.y) / PointDistance(left, perpendiculaireLeft);

    double newB = PointDistance(right, perpendiculaireRight) + (PointDistance(right, perpendiculaireRight)*b)/(a-b);
    double coef = newB/PointDistance(right, perpendiculaireRight);
*/
/*
    //source link: https://www.commentcamarche.net/forum/affich-3376777-algo-detecter-deux-segments-qui-se-croisent
    double coefDirLeft, coefDirRight;
    double b1, b2;
    double xCommun;
    if(perpendiculaireLeft.x != left.x)
        coefDirLeft = (double)(perpendiculaireLeft.y - left.y)/(double)(perpendiculaireLeft.x - left.x);
    else
        coefDirLeft = 9999999.9999999;
    if(perpendiculaireRight.x != right.x)
        coefDirRight = (double)(perpendiculaireRight.y - right.y)/(double)(perpendiculaireRight.x - right.x);
    else
        coefDirRight = 9999999.9999999;
    if(coefDirLeft == coefDirRight) //parallèles
        return left;
    b1 = left.y - (coefDirLeft*left.x);
    b2 = right.y - (coefDirRight*right.x);
    xCommun = (b2-b1)/(coefDirLeft-coefDirRight);
    center.x = (LCoord) xCommun;
    center.y = (LCoord) coefDirLeft*xCommun + b1;
*/
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
		if(Cx==Dx) return left;
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

LCell	pCell	=	LCell_GetVisible();
LFile	pFile	=	LCell_GetFile(pCell);
LWireConfig config;
LPoint wire_arr[5];
wire_arr[0] = left;
wire_arr[1] = perpendiculaireLeft;
wire_arr[2] = center;
wire_arr[3] = perpendiculaireRight;
wire_arr[4] = right;
LObject wire;
//wire = LWire_New( pCell, LLayer_Find ( pFile, "CIRCLE" ), &config,0, wire_arr, 5);
//LWire_SetWidth( pCell, wire, 1000 );
//LCircle_New(pCell, LLayer_Find ( pFile, "CIRCLE" ), center, 10000);

    return center;
}


void FindTangentPoints(LPoint* tanLeft, LPoint* tanRight, int firstPointIndex, LPoint* point_arr, int nbPointsArr )
{
    double fillet = FILLET_VALUE;

    int i = (firstPointIndex - 1);
    if(i < 0)
        i = nbPointsArr -1;
    int j = (firstPointIndex + 1);
    if(j >= nbPointsArr)
                j = 0;
    int rightPos;
    LPoint left, right, center;
    center = point_arr[firstPointIndex];
    left = point_arr[i];
    right = point_arr[j];

LCell	pCell	=	LCell_GetVisible();
LFile	pFile	=	LCell_GetFile(pCell);
    while(PointDistance(center, left) < fillet || PointDistance(center, right) < fillet || PointDistance(point_arr[firstPointIndex], left) < fillet || PointDistance(point_arr[firstPointIndex], right) < fillet)
    {
        left = point_arr[i];
        right = point_arr[j];

        //rightPos = FindRightPointWithDistance( &right, PointDistance(point_arr[firstPointIndex], left) , firstPointIndex, point_arr, nbPointsArr, i); //find a point on the right side with a particulare distance form the angle
        center = FindCenter(left, point_arr[ (i+1) % nbPointsArr], point_arr[ (j-1) % nbPointsArr], right);
        if(PointDistance(point_arr[firstPointIndex], left) < PointDistance(point_arr[firstPointIndex], right)) //we increment the closest point from the concave angle
        {
            i = i - 1;
            if(i < 0)
                i = nbPointsArr -1;
        }
        else
        {
            j = j + 1;
            if(j >= nbPointsArr)
                j = 0;
        }

LWireConfig config;
LPoint wire_arr[2];
wire_arr[0] = left;
//wire_arr[1] = center;
wire_arr[1] = right;
LObject wire;
wire = LWire_New( pCell, LLayer_Find ( pFile, "WGUIDE" ), &config,0, wire_arr, 2 );
LWire_SetWidth( pCell, wire, 1000 );
//LCircle_New(pCell, LLayer_Find ( pFile, "CIRCLE" ), center, 10000);

    }

//LCircle_New(pCell, LLayer_Find ( pFile, "WGUIDE" ), center, 10000);
    
    tanLeft->x = left.x;
    tanLeft->y = left.y;
    tanRight->x = right.x;
    tanRight->y = right.y;
}


int isPresentInDeleteArray(LPoint* to_delete_point_arr, int nbPointsToDelete, LPoint point)
{
    int i = 0;
    for(i = 0; i<nbPointsToDelete; i++)
    {
        if(to_delete_point_arr[i].x == point.x && to_delete_point_arr[i].y == point.y)
            return 1;
    }
    return 0;
}


void AATorusFillet(void)
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);

    LPoint point_arr[MAX_POLYGON_SIZE];
    LPoint final_point_arr[MAX_POLYGON_SIZE];
    LPoint to_delete_point_arr[MAX_POLYGON_SIZE];
    long numberVertex = 0;
    int nbPointsFinal = 0;
    int nbPointsToDelete = 0;

    LCoord prevX = 0;
    LCoord prevY = 0;
    LCoord x = 0;
    LCoord y = 0;
    LCoord nextX = 0;
    LCoord nextY = 0;

    LPoint tanLeft;
    LPoint tanRight;

    long dxPrev;
    long dyPrev;
    long dxNext;
    long dyNext;

    double angle, angle1, angle2;

    int i = 0;
    int cpt = 0;
    
    LUpi_LogMessage("\n\n\n\n\nSTART MACRO\n");

    if(LSelection_GetList() == NULL) //if no selection made
	{
        LUpi_LogMessage("No selection were made\n");
		return;
	}
    else
    {
        for(LSelection pSelection = LSelection_GetList() ; pSelection != NULL; pSelection = LSelection_GetNext(pSelection) )
        {

            LObject object = LSelection_GetObject(pSelection);
            nbPointsToDelete = 0;
            nbPointsFinal = 0;

            if(LObject_GetGeometry(object) == LAllAngle || LObject_GetGeometry(object) == LOrthogonal || LObject_GetGeometry(object) == LFortyFive)
            {
                numberVertex = LVertex_GetArray( object, point_arr, MAX_POLYGON_SIZE );

                for(i=0; i<numberVertex; i++) //store the current, previous and next point
                {
                    x = point_arr[i].x;
                    y = point_arr[i].y;
                    if(i == 0){
                        prevX = point_arr[numberVertex-1].x;
                        prevY = point_arr[numberVertex-1].y;
                    }
                    else{
                        prevX = point_arr[i-1].x;
                        prevY = point_arr[i-1].y;
                    }
                    if(i == numberVertex-1){
                        nextX = point_arr[0].x;
                        nextY = point_arr[0].y;
                    }
                    else{
                        nextX = point_arr[i+1].x;
                        nextY = point_arr[i+1].y;
                    }

                    dxPrev = x-prevX;
                    dyPrev = y-prevY;
                    dxNext = nextX-x;
                    dyNext = nextY-y;

                    angle1 = atan2(dyPrev,dxPrev) - M_PI;
                    angle2 = atan2(dyNext,dxNext);
                    angle = angle2 - angle1;
                    angle = fmod(angle, 2*M_PI);
                    while(angle < 0)
                        angle = angle + 2*M_PI;

                    if( ! (angle > M_PI - ANGLE_LIMIT && angle < M_PI +ANGLE_LIMIT) ) //if not in the limit range
                    {
                        FindTangentPoints(&tanLeft, &tanRight, i, point_arr, numberVertex);
LCircle_New(pCell, LLayer_Find ( pFile, "TANCIRCLE" ), tanLeft , 1000);
LCircle_New(pCell, LLayer_Find ( pFile, "TANCIRCLE" ), tanRight , 1000);
                        cpt = (i-1)%numberVertex;
                        while(point_arr[cpt].x != tanLeft.x || point_arr[cpt].y != tanLeft.y)
                        {
                            if(isPresentInDeleteArray(to_delete_point_arr, nbPointsToDelete, point_arr[cpt]) == 0)
                            {
                                to_delete_point_arr[nbPointsToDelete] = point_arr[cpt];
                                nbPointsToDelete = nbPointsToDelete + 1;
                            }
                            cpt = cpt - 1;
                            if(cpt<0)
                                cpt = numberVertex-1;
                        }
                        cpt = (i+1)%numberVertex;
                        while(point_arr[cpt].x != tanRight.x || point_arr[cpt].y != tanRight.y)
                        {
                            if(isPresentInDeleteArray(to_delete_point_arr, nbPointsToDelete, point_arr[cpt]) == 0)
                            {
                                to_delete_point_arr[nbPointsToDelete] = point_arr[cpt];
                                nbPointsToDelete = nbPointsToDelete + 1;
                            }
                            cpt = cpt + 1;
                            if(cpt>=numberVertex)
                                cpt = 0;
                        }

LCircle_New(pCell, LLayer_Find ( pFile, "CIRCLELEFT" ), point_arr[i] , 10000);

                        LUpi_LogMessage(LFormat("I: %d, point: %ld %ld, angle: %lf\n", i+1,point_arr[i].x,point_arr[i].y, angle));
                    }
                }

                cpt = 0;
                for( cpt=0; cpt < numberVertex; cpt++ )
                {
                    if(isPresentInDeleteArray(to_delete_point_arr, nbPointsToDelete, point_arr[cpt]) == 1)
                    {
                        LCircle_New(pCell, LLayer_Find ( pFile, "CIRCLE" ), point_arr[cpt] , 1000);
                        //LUpi_LogMessage(LFormat("point %d supprime\n", cpt));
                    }
                    else
                    {
                        final_point_arr[nbPointsFinal].x = point_arr[cpt].x;
                        final_point_arr[nbPointsFinal].y = point_arr[cpt].y;
                        nbPointsFinal = nbPointsFinal + 1; 
                    }
                   
                }
                LPolygon_New(pCell, LLayer_Find ( pFile, "TEST" ), final_point_arr, nbPointsFinal);
            }
        }
    }
    LUpi_LogMessage(LFormat("nbPointsToDelete: %d\n", nbPointsToDelete));
    LUpi_LogMessage(LFormat("nbPointsFinal: %d\n", nbPointsFinal));
    LUpi_LogMessage(LFormat("\nEND MACRO\n"));
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "AA torus to be fillet", "AATorusFillet", NULL /*hotkey category*/);
	return 1;
}