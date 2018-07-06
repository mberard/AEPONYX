#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <math.h>

#define MAX_POLYGON_SIZE 20000
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
    double exactX, exactY;
    double dx = right.x - prevRight.x;
    double dy = right.y - prevRight.y;
    perpendiculaireRight = LPoint_Set(right.x + dy, right.y - dx);

    exactX = perpendiculaireRight.x;
    exactY = perpendiculaireRight.y;
    center.x = (LCoord)exactX;
    center.y = (LCoord)exactY;

    while(PointDistance(right, center) < PointDistance(left, center))
    {
        exactX = exactX + 0.0002*dy;
        exactY = exactY - 0.0002*dx;
        center.x = (LCoord)exactX;
        center.y = (LCoord)exactY;
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

LPoint FindTangentPoints(LPoint* tanLeft, LPoint* tanRight, int angleIndex, LPoint* point_arr, int nbPointsArr, double fillet, int step )
{
LCell	pCell	=	LCell_GetVisible();
LFile	pFile	=	LCell_GetFile(pCell);
    LPoint origin, left, prevLeft, right, nextRight;
    LPoint testPointLeft, testPointRight;
    LPoint lastTestPointLeft, lastTestPointRight;
    LPoint center, betterCenter;
    int currentPrevLeftIndex, currentNextRightIndex;
int cpt = 0;
    double dxLeft, dyLeft, dxRight, dyRight;
    double exactPosLeftX, exactPosLeftY, exactPosRightX, exactPosRightY;
    double angle=0, angle1=0, angle2=0;

    if(angleIndex == 0)
        currentPrevLeftIndex = nbPointsArr-1;
    else
        currentPrevLeftIndex = angleIndex-1;
    
    if(angleIndex == nbPointsArr-1)
        currentNextRightIndex = 0;
    else
        currentNextRightIndex = angleIndex+1;

    origin = point_arr[angleIndex];
    left = point_arr[angleIndex];
    right = point_arr[angleIndex];
    prevLeft = point_arr[currentPrevLeftIndex];
    nextRight = point_arr[currentNextRightIndex];

    lastTestPointLeft = origin;
    lastTestPointRight = origin;
    center = origin;
    betterCenter = origin;

    exactPosLeftX = origin.x;
    exactPosLeftY = origin.y;
    exactPosRightX = origin.x;
    exactPosRightY = origin.y;

    dxLeft = prevLeft.x - left.x;
    dyLeft = prevLeft.y - left.y;
    while(dxLeft*dxLeft+dyLeft*dyLeft > step*step)
    {
        dxLeft = dxLeft/1.5;
        dyLeft = dyLeft/1.5;
    }

    dxRight = nextRight.x - right.x;
    dyRight = nextRight.y - right.y;
    while(dxRight*dxRight+dyRight*dyRight > step*step)
    {
        dxRight = dxRight/1.5;
        dyRight = dyRight/1.5;
    }

    exactPosLeftX = exactPosLeftX + dxLeft;
    exactPosLeftY = exactPosLeftY + dyLeft;
    exactPosRightX = exactPosRightX + dxRight;
    exactPosRightY = exactPosRightY + dyRight;

    testPointLeft.x = (LCoord)exactPosLeftX;
    testPointLeft.y = (LCoord)exactPosLeftY;
    testPointRight.x = (LCoord)exactPosRightX;
    testPointRight.y = (LCoord)exactPosRightY;

    while(PointDistance(testPointLeft, origin) < fillet || PointDistance(testPointRight, origin) < fillet  || PointDistance(testPointLeft, center) < fillet || PointDistance(testPointRight, center) < fillet  || centerIsBetweenPoints(testPointLeft, origin, testPointRight, center) == 0)
    {

        center = FindCenter(testPointLeft, prevLeft, right, testPointRight);
        betterCenter = FindBetterCenter(testPointLeft, prevLeft, right, testPointRight);

        if(PointDistance(origin, testPointLeft) < PointDistance(origin, testPointRight))
        {
            exactPosLeftX = exactPosLeftX + dxLeft;
            exactPosLeftY = exactPosLeftY + dyLeft;

            testPointLeft.x = (LCoord)exactPosLeftX;
            testPointLeft.y = (LCoord)exactPosLeftY;
            
            if(PointDistance(testPointLeft, left) > PointDistance(prevLeft, left))
            {
                testPointLeft = prevLeft;
                left = prevLeft;

                while(PointDistance(left, prevLeft) < step)
                {
                    if(currentPrevLeftIndex == 0)
                        currentPrevLeftIndex = nbPointsArr-1;
                    else
                        currentPrevLeftIndex = currentPrevLeftIndex-1;
                    prevLeft = point_arr[currentPrevLeftIndex];
                }

                angle1 = atan2(dyLeft, dxLeft);

                dxLeft = prevLeft.x - left.x;
                dyLeft = prevLeft.y - left.y;
                while(dxLeft*dxLeft+dyLeft*dyLeft > step*step)
                {
                    dxLeft = dxLeft/1.5;
                    dyLeft = dyLeft/1.5;
                }
                angle2 = atan2(dyLeft, dxLeft);
                
                angle = angle + angle1 - angle2;
                //if(angle >= M_PI-ANGLE_LIMIT || angle <= -M_PI+ANGLE_LIMIT)
                if(angle >= M_PI/2.0 || angle <= -M_PI/2.0)
                {
                    return LPoint_Set(-1,-1);
                }
                exactPosLeftX = exactPosLeftX + dxLeft;
                exactPosLeftY = exactPosLeftY + dyLeft;

                testPointLeft.x = (LCoord)exactPosLeftX;
                testPointLeft.y = (LCoord)exactPosLeftY;
            }
        }
        else
        {
            exactPosRightX = exactPosRightX + dxRight;
            exactPosRightY = exactPosRightY + dyRight;

            testPointRight.x = (LCoord)exactPosRightX;
            testPointRight.y = (LCoord)exactPosRightY;
            
            if(PointDistance(testPointRight, right) > PointDistance(nextRight, right))
            {
                testPointRight = nextRight;
                right = nextRight;

                while(PointDistance(right, nextRight) < step)
                {
                    if(currentNextRightIndex == nbPointsArr-1)
                        currentNextRightIndex = 0;
                    else
                        currentNextRightIndex = currentNextRightIndex+1;
                    nextRight = point_arr[currentNextRightIndex];
                }
                

                angle1 = atan2(dyLeft, dxLeft);

                dxRight = nextRight.x - right.x;
                dyRight = nextRight.y - right.y;
                while(dxRight*dxRight+dyRight*dyRight > step*step)
                {
                    dxRight = dxRight/1.5;
                    dyRight = dyRight/1.5;
                }
                angle2 = atan2(dyLeft, dxLeft);
                
                angle = fabs(angle1 - angle2);
                if(angle >= M_PI)
                {
                    return LPoint_Set(-1,-1);
                }
                exactPosRightX = exactPosRightX + dxRight;
                exactPosRightY = exactPosRightY + dyRight;
                testPointRight.x = (LCoord)exactPosRightX;
                testPointRight.y = (LCoord)exactPosRightY;
            }
        }
//LCircle_New( pCell, LLayer_Find(pFile, "LEFTCIRCLE"), testPointLeft, 1 );
//LCircle_New( pCell, LLayer_Find(pFile, "RIGHTCIRCLE"), testPointRight, 1 );
        lastTestPointLeft = testPointLeft;
        lastTestPointRight = testPointRight;
    }

    tanLeft->x = testPointLeft.x;
    tanLeft->y = testPointLeft.y;
    tanRight->x = testPointRight.x;
    tanRight->y = testPointRight.y;

    if(abs(fillet-PointDistance(center,testPointLeft))+abs(fillet-PointDistance(center,testPointRight)) > abs(fillet-PointDistance(betterCenter,testPointLeft))+abs(fillet-PointDistance(betterCenter,testPointRight)))
    {
        center = betterCenter;
    }    

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
    int newAngleIndex = 0;
    int cpt = 0;


    LUpi_LogMessage("\n\n\n\n\nSTART MACRO\n");

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

        strcpy(strLayer, "OX"); //preloaded text in the dialog box
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

        for(LObject obj = LObject_GetList(pCell, tmpLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            originalNumberVertex = LVertex_GetArray( obj, original_point_arr, MAX_POLYGON_SIZE );
            
            if(originalNumberVertex >= MAX_POLYGON_SIZE)
            {
                LDialog_AlertBox( "Limit number of polygon vertex has been reach, return" );
                return;
            }
            for(i=0; i<originalNumberVertex; i++) //store the current, previous and next point
            {
LUpi_LogMessage(LFormat("test %d sur %d\n", i+1, originalNumberVertex));
                x = original_point_arr[i].x;
                y = original_point_arr[i].y;
                if(i == 0){
                    prevX = original_point_arr[originalNumberVertex-1].x;
                    prevY = original_point_arr[originalNumberVertex-1].y;
                }
                else{
                    prevX = original_point_arr[i-1].x;
                    prevY = original_point_arr[i-1].y;
                }
                if(i == originalNumberVertex-1){
                    nextX = original_point_arr[0].x;
                    nextY = original_point_arr[0].y;
                }
                else{
                    nextX = original_point_arr[i+1].x;
                    nextY = original_point_arr[i+1].y;
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
                //if( ! (angle > M_PI - ANGLE_LIMIT) ) //if not in the limit range and concave
                if( angle < M_PI - ANGLE_LIMIT ) //if not in the limit range and concave
                {
LUpi_LogMessage("Point need to be fillet\n");
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), original_point_arr[i], 1000 );
                    
                    center = FindTangentPoints(&tanLeft, &tanRight, i, original_point_arr, originalNumberVertex, fillet, 5);
                    if( !(center.x == -1 && center.y == -1) )
                    {
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), tanLeft, 10 );
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), tanRight, 10 );
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), center, 100 );

                        tParams.ptCenter = center;
                        tParams.nInnerRadius = max( PointDistance(center, tanLeft), PointDistance(center, tanRight));
                        tParams.nOuterRadius = PointDistance(original_point_arr[i], center)*1.02;
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
                    }
                    else
                    {
                        LDialog_AlertBox(LFormat("An angle could not be fillet automatically"));
                    }
                }
                LUpi_LogMessage("Test the next point\n");
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