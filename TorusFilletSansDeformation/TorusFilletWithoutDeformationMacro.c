#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <math.h>

#define MAX_POLYGON_SIZE 30000
#define MAX_NUMBER_POLYGON 1000
#define ANGLE_LIMIT 0.25 //in radian, 0.523599 rad == 30 degrés, 0.785398 rad == 45 degrés, 1.5708 rad == 90 degrés

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}

int centerIsBetweenPoints(LPoint left, LPoint middle, LPoint right, LPoint center)
{
    double dx1, dx2, dx3, dy1, dy2, dy3;
    dx1 = left.x - middle.x;
    dx2 = right.x - middle.x;
    dx3 = center.x- middle.x;
    dy1 = left.y - middle.y;
    dy2 = right.y - middle.y;
    dy3 = center.y - middle.y;

    double angleLeft = atan2( dy1 , dx1 );
    double angleRight = atan2( dy2 , dx2 );
    double angleCenter = atan2( dy3 , dx3 );

    while(angleLeft<0)
        angleLeft = angleLeft + 2*M_PI;
    while(angleRight<angleLeft)
        angleRight = angleRight + 2*M_PI;
    while(angleCenter<angleLeft)
        angleCenter = angleCenter + 2*M_PI;

    if(abs(angleLeft - angleRight) > M_PI)
        return 1;

    if(angleCenter > angleLeft && angleCenter < angleRight)
        return 1;
    else
        return 0;

    return 0; //not between points
}

LPoint FindBetterCenter(LPoint left, LPoint nextLeft , LPoint prevRight, LPoint right  )
{
    LPoint perpendiculaireRight, center;

    double dx = right.x - prevRight.x;
    double dy = right.y - prevRight.y;
    perpendiculaireRight = LPoint_Set(right.x + dy, right.y - dx);

    center = perpendiculaireRight;

    while(PointDistance(right, center) < PointDistance(left, center))
    {
        center.x = center.x + 0.1*dy;
        center.y = center.y - 0.1*dx;
    }

    return center;
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

    return center;
}


LPoint FindTangentPoints(LPoint* tanLeft, LPoint* tanRight, int firstPointIndex, LPoint* point_arr, int nbPointsArr, double fillet, LPoint* original_point_arr, int originalNumberVertex )
{
    int result1, result2;
    LPoint betterCenter;
    int i = (firstPointIndex - 1);
    if(i < 0)
        i = nbPointsArr -1;
    int j = (firstPointIndex + 1);
    if(j >= nbPointsArr)
                j = 0;

    LPoint left, right, center;
    LPoint precLeft, precRight;
    
    center = point_arr[firstPointIndex];
    precLeft = point_arr[firstPointIndex];
    precRight = point_arr[firstPointIndex];
    left = point_arr[i];
    right = point_arr[j];

    while(PointDistance(center, left) < fillet || PointDistance(center, right) < fillet || PointDistance(point_arr[firstPointIndex], left) < fillet || PointDistance(point_arr[firstPointIndex], right) < fillet || centerIsBetweenPoints(left, point_arr[firstPointIndex], right, center) == 0)
    {

        left = point_arr[i];
        right = point_arr[j];

        center = FindCenter(left, point_arr[ (i+1) % nbPointsArr], point_arr[ (j-1) % nbPointsArr], right);
        betterCenter = FindBetterCenter(left, point_arr[ (i+1) % nbPointsArr], point_arr[ (j-1) % nbPointsArr], right);
        
        if(PointDistance(point_arr[firstPointIndex], left) < PointDistance(point_arr[firstPointIndex], right)) //we increment the closest point from the concave angle
        {
            i = i - 1;
            if(i < 0)
                i = nbPointsArr - 1;
        }
        else
        {
            j = j + 1;
            if(j >= nbPointsArr)
                j = 0;
        }

        result1 = IsInArray(original_point_arr, originalNumberVertex, left);
        result2 = IsInArray(original_point_arr, originalNumberVertex, right);

        if( result2 != -1 )
        {
            if(PointDistance(original_point_arr[result2], original_point_arr[(result2+1)%originalNumberVertex]) > fillet*10)
            {
                LDialog_AlertBox(LFormat("An angle could not be fillet automatically"));
                return LPoint_Set(-1,-1);
            }
        }
        else if( result1 != -1 )
        {
            if(result1 == 0)
                result1 = originalNumberVertex;
            if(PointDistance(original_point_arr[result1], original_point_arr[(result1-1)%originalNumberVertex]) > fillet*10)
            {
                LDialog_AlertBox(LFormat("An angle could not be fillet automatically"));
                return LPoint_Set(-1,-1);
            }
        }

        precLeft = left;
        precRight = right;

    }

    tanLeft->x = left.x;
    tanLeft->y = left.y;
    tanRight->x = right.x;
    tanRight->y = right.y;

    center = betterCenter;

    return center;
}


int IsInArray(LPoint* point_arr_to_check, int nbPointsInArr, LPoint point)
{
    int i = 0;
    for(i = 0; i<nbPointsInArr; i++)
    {
        if(point_arr_to_check[i].x == point.x && point_arr_to_check[i].y == point.y)
            return i;
    }
    return -1;
}

LPoint FindClosestPoint(LPoint point, LPoint* point_arr, int numberVertex)
{
    int i = 0;
    LPoint closestPoint;
    double smallestDistance = 999999999;

    for(i=0; i<numberVertex; i++)
    {
        if(PointDistance(point_arr[i], point) < smallestDistance)
        {
            closestPoint = point_arr[i];
            smallestDistance = PointDistance(point_arr[i], point);
        }
    }
    return closestPoint;
}


int AddPointsToArray(LPoint* point_arr, int numberVertex, int step, double fillet, int max_size)
{
    int i = 0;
    int j = 0;
    int k = 0;
    double x, y;
    LPoint point;
    int numberVertexSaved = numberVertex;

    LPoint saved_point_arr[numberVertex];

    for(i=0; i<numberVertex; i++)
        saved_point_arr[i] = point_arr[i];

    for(i=0; i<numberVertex; i++)
    {
        if(PointDistance(point_arr[i], point_arr[(i+1)%numberVertex]) > step)
        {
            if(numberVertex >= max_size-1)
            {
                for(j=0; j<numberVertex; j++)
                {
                    if(IsInArray(saved_point_arr, numberVertexSaved, point_arr[j]) == -1 && !(point_arr[j].x==point_arr[i].x && point_arr[j].y==point_arr[i].y) )
                    {
                        point = FindClosestPoint(point_arr[j], saved_point_arr, numberVertexSaved);
                        if(PointDistance(point, point_arr[j]) > fillet*8 || PointDistance(point, point_arr[j]) < fillet*0.6)
                        {
LUpi_LogMessage("Intermediate clean up\n");
                            //delete the point
                            for(k=j; k<numberVertex-1; k++)
                            {
                                point_arr[k]=point_arr[k+1];
                            }
                            numberVertex=numberVertex-1;
                            j=j-1;
                        }
                    }
                }

                if(numberVertex >= max_size-1)
                    return max_size;
            }
            
            x = (double)(point_arr[(i+1)%numberVertex].x - point_arr[i].x) / 2.0;
            x = point_arr[i].x + x;
            y = (double)(point_arr[(i+1)%numberVertex].y - point_arr[i].y) / 2.0;
            y = point_arr[i].y + y;

            //add a point to the array and shift the other value
            for(j=numberVertex-1; j>i; j--)
            {
                point_arr[j+1] = point_arr[j];
            }
            point_arr[i+1] = LPoint_Set(x, y);
            numberVertex = numberVertex + 1;
            i = i - 1; //test the same point with the new target point 
        }
    }

LUpi_LogMessage("Final clean up\n");
    for(j=0; j<numberVertex; j++)
    {
        if(IsInArray(saved_point_arr, numberVertexSaved, point_arr[j]) == -1)
        {
            point = FindClosestPoint(point_arr[j], saved_point_arr, numberVertexSaved);
            if(PointDistance(point, point_arr[j]) > fillet*8 || PointDistance(point, point_arr[j]) < fillet*0.6)
            {
                //delete the point
                for(k=j; k<numberVertex; k++)
                {
                    point_arr[k]=point_arr[k+1];   
                }
                numberVertex=numberVertex-1;
                j=j-1;
            }
        }
    }
    
    return numberVertex;
}


void AATorusFilletWithoutDeformation(void)
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer  pLayer;

    LObject obj_arr[MAX_NUMBER_POLYGON];
    int nbPolygonSelected = 0;

    LLayer tmpLayer;

    char strLayer[MAX_LAYER_NAME];

    LPoint original_point_arr[MAX_POLYGON_SIZE];
    long originalNumberVertex = 0;
    LPoint point_arr[MAX_POLYGON_SIZE];
    long numberVertex = 0;

    LCoord prevX = 0;
    LCoord prevY = 0;
    LCoord x = 0;
    LCoord y = 0;
    LCoord nextX = 0;
    LCoord nextY = 0;

    LPoint tanLeft;
    LPoint tanRight;
    LPoint center;

    long dxPrev;
    long dyPrev;
    long dxNext;
    long dyNext;

    double angle, angle1, angle2;

    double fillet;
    char strFillet[20];

    LTorusParams tParams;

    int i = 0;
//    int cpt = 0;


    LUpi_LogMessage("\n\n\n\n\nSTART MACRO\n");

    if ( LStatusOK != LFile_SaveAs( LFile_GetVisible(), "LastBackupBeforeTorrusFilletMacro", LTdbFile))
        LDialog_AlertBox( "Failed to save new copy of current file" );

    if(LSelection_GetList() == NULL) //if no selection made
	{
        LUpi_LogMessage("No selection were made\n");
		return;
	}
    else
    {
        strcpy(strFillet, "0.5"); //preloaded text in the dialog box
		if ( LDialog_InputBox("Fillet", "Enter the fillet value (in microns)", strFillet) == 0)
			return;
		else
            fillet = LFile_MicronsToIntU(pFile,atof(strFillet));
        LUpi_LogMessage(LFormat("fillet: %lf\n", fillet));

        strcpy(strLayer, "WGUIDE"); //preloaded text in the dialog box
		if ( LDialog_InputBox("Layer", "Enter name of the layer in which the polygon will be loaded", strLayer) == 0)
			return;
		else
            pLayer = LLayer_Find(pFile, strLayer);
        
        LLayer_New( pFile, NULL, "tmp");
        tmpLayer = LLayer_Find(pFile, "tmp");
        
        for(LSelection pSelection = LSelection_GetList() ; pSelection != NULL; pSelection = LSelection_GetNext(pSelection) )
        {
            LObject object = LSelection_GetObject(pSelection);
            obj_arr[nbPolygonSelected] = object;
            nbPolygonSelected++;
        }
        LUpi_LogMessage(LFormat("nbPolygonSelected %d\n", nbPolygonSelected));    

        LCell_BooleanOperation(pCell, LBoolOp_OR , NULL, obj_arr, nbPolygonSelected, NULL, 0, tmpLayer, LFALSE );
LUpi_LogMessage("Bool operation has been made\n");

        for(LObject obj = LObject_GetList(pCell, tmpLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
LUpi_LogMessage("New object\n");
            originalNumberVertex = LVertex_GetArray( obj, original_point_arr, MAX_POLYGON_SIZE );
            
            numberVertex = LVertex_GetArray( obj, point_arr, MAX_POLYGON_SIZE );
LUpi_LogMessage("Begin adding points\n");
            numberVertex = AddPointsToArray(point_arr, numberVertex, 200, fillet, MAX_POLYGON_SIZE);
LUpi_LogMessage("Points has been add\n");

            if(numberVertex >= MAX_POLYGON_SIZE)
            {
                LDialog_AlertBox( "Limit number of polygon vertex has been reach, return" );
                return;
            }
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
                //if( ! (angle > M_PI - ANGLE_LIMIT && angle < M_PI +ANGLE_LIMIT) ) //if not in the limit range
                if( ! (angle > M_PI - ANGLE_LIMIT) ) //if not in the limit range and concave
                {
LUpi_LogMessage("Need to be fillet\n");
                    center = FindTangentPoints(&tanLeft, &tanRight, i, point_arr, numberVertex, fillet, original_point_arr, originalNumberVertex);
LUpi_LogMessage("Tan and center has been found\n");

                    if( !(center.x == -1 && center.y == -1) )
                    {
//LCircle_New( pCell, LLayer_Find(pFile, "CIRCLE"), tanLeft, 100 );
//LCircle_New( pCell, LLayer_Find(pFile, "CIRCLE"), tanRight, 100 );
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), center, 100 );

                        tParams.ptCenter = center;
                        tParams.nInnerRadius = max( PointDistance(center, tanLeft), PointDistance(center, tanRight));
                        tParams.nOuterRadius = PointDistance(point_arr[i], center)*1.05;
                        angle1 = atan2(tanLeft.y - center.y, tanLeft.x - center.x )*180/M_PI;
                        angle2 = atan2(tanRight.y - center.y, tanRight.x - center.x )*180/M_PI;
                        while(angle1<0)
                            angle1 = angle1 + 360;
                        while(angle1>360)
                            angle1 = angle1 - 360;
                        while(angle2<0)
                            angle2 = angle2 + 360;
                        while(angle2>360)
                            angle2 = angle2 - 360;
                        tParams.dStartAngle = angle2;
                        tParams.dStopAngle = angle1;

                        LTorus_CreateNew( pCell, pLayer, &tParams );
LUpi_LogMessage("Torus has been creating\n");
                    }
//                    else
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), point_arr[i], 200 );

                }
            }
        }
    }

    for(LObject obj = LObject_GetList(pCell, tmpLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpLayer );

    LUpi_LogMessage(LFormat("\nEND MACRO\n"));
}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F3" /*hotkey*/, "AEPONYX\\Fillet AA polygons without deformation\nWindow", "AATorusFilletWithoutDeformation", NULL /*hotkey category*/);
	return 1;
}