#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <math.h>

#define MAX_POLYGON_SIZE 30000
#define MAX_NUMBER_POLYGON 1000
#define ANGLE_LIMIT 0.25 //in radian, 0.523599 rad == 30 degrés, 0.785398 rad == 45 degrés, 1.5708 rad == 90 degrés
#define LIMIT_DISTANCE_POINT_LABEL 150 //in internal units
#define LIMIT_FAST_APPROACH_1 1.30
#define LIMIT_FAST_APPROACH_2 1.47
#define LIMIT_FAST_APPROACH_3 1.55


void RecursiveInstanceDetection(LInstance instance, LCell cell, LLayer originLayer, LLayer destinationLayer, LTransform_Ex99 prevTransform)
{

    LTransform_Ex99 transform;
    transform = LInstance_GetTransform_Ex99( instance ); //get the transform form this to the previous one
    //add it to the previous one (add translation and orientation, multiply magnification)
    transform.translation.x = transform.translation.x + prevTransform.translation.x;
    transform.translation.y = transform.translation.y + prevTransform.translation.y;
    transform.orientation = transform.orientation + prevTransform.orientation;
    transform.magnification.num = transform.magnification.num * prevTransform.magnification.num;
    transform.magnification.denom = transform.magnification.denom * prevTransform.magnification.denom;
    
    LObject obj;
    //find the right cell
    LCell instancedCell = LInstance_GetCell(instance);

    //if there is an instance in this instance
    for(LInstance newInstance = LInstance_GetList( instancedCell ) ; newInstance != NULL ; newInstance = LInstance_GetNext( newInstance ) )
    {
        //recall the same function
        RecursiveInstanceDetection((LInstance)newInstance, cell, originLayer, destinationLayer, transform);
    }

    //copy polygons from an instance to the tmp layer
    for(LObject instancedObject = LObject_GetList(instancedCell, originLayer) ; instancedObject != NULL ; instancedObject = LObject_GetNext(instancedObject) )
    {
        obj = LObject_Copy( cell, destinationLayer, instancedObject );
        LObject tmp_obj_arr[1];
        tmp_obj_arr[0] = obj;
        LObject_ConvertToPolygon( cell, tmp_obj_arr, 1 ); //necessary to ensure the tranform will be good (especially for box)
        LObject_Transform_Ex99( tmp_obj_arr[0], transform );
    }
}

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

LPoint FindTangentFromCenter(int angleIndex, LPoint* point_arr, int nbPointsArr, double fillet, LPoint center, double* returnedRightAngle, double* returnedLeftAngle)
{
LCell	pCell	=	LCell_GetVisible();
LFile	pFile	=	LCell_GetFile(pCell);
    LPoint origin, left, prevLeft, right, nextRight;
    LPoint testPointLeft, testPointRight;
    LPoint lastTestPointLeft, lastTestPointRight;
    
    int currentPrevLeftIndex, currentLeftIndex, currentRightIndex, currentNextRightIndex;
    double dxLeft, dyLeft, dxRight, dyRight;
    double exactPosLeftX, exactPosLeftY, exactPosRightX, exactPosRightY;
    double distStart;
    double rightAngle, leftAngle;
    double prevRightAngle, prevLeftAngle, cumulAngle;
    
    if(angleIndex == 0)
        currentPrevLeftIndex = nbPointsArr-1;
    else
        currentPrevLeftIndex = angleIndex-1;
    
    if(angleIndex == nbPointsArr-1)
        currentNextRightIndex = 0;
    else
        currentNextRightIndex = angleIndex+1;

    currentLeftIndex = angleIndex;
    currentRightIndex = angleIndex;

    origin = point_arr[angleIndex];
    left = point_arr[currentLeftIndex];
    right = point_arr[currentRightIndex];
    prevLeft = point_arr[currentPrevLeftIndex];
    nextRight = point_arr[currentNextRightIndex];

    rightAngle = atan2(nextRight.y-right.y,nextRight.x-right.x)-atan2(center.y-nextRight.y, center.x-nextRight.x);
    while(rightAngle > M_PI)
        rightAngle = rightAngle - 2*M_PI;
    while(rightAngle < -M_PI)
        rightAngle = rightAngle + 2*M_PI;
    prevRightAngle = rightAngle;
    rightAngle = fabs(rightAngle);

    leftAngle = atan2(prevLeft.y-left.y,prevLeft.x-left.x)-atan2(center.y-prevLeft.y, center.x-prevLeft.x);
    while(leftAngle > M_PI)
        leftAngle = leftAngle - 2*M_PI;
    while(leftAngle < -M_PI)
        leftAngle = leftAngle + 2*M_PI;
    prevLeftAngle = leftAngle;
    leftAngle = fabs(leftAngle);

    cumulAngle = 0;
    while( rightAngle < M_PI/2.0)
    {
        currentRightIndex = currentNextRightIndex;
        if(currentNextRightIndex == nbPointsArr-1)
            currentNextRightIndex = 0;
        else
            currentNextRightIndex = currentNextRightIndex+1;
        right = point_arr[currentRightIndex];
        nextRight = point_arr[currentNextRightIndex];

        rightAngle = atan2(nextRight.y-right.y,nextRight.x-right.x)-atan2(center.y-nextRight.y, center.x-nextRight.x);
        while(rightAngle > M_PI)
            rightAngle = rightAngle - 2*M_PI;
        while(rightAngle < -M_PI)
            rightAngle = rightAngle + 2*M_PI;
        cumulAngle = cumulAngle + rightAngle - prevRightAngle;
        prevRightAngle = rightAngle;

        if(cumulAngle > M_PI/2.0)
            return LPoint_Set(-1,-1);

        rightAngle = fabs(rightAngle);
    }

    cumulAngle = 0;
    while( leftAngle < M_PI/2.0)
    {
        currentLeftIndex = currentPrevLeftIndex;
        if(currentPrevLeftIndex == 0)
            currentPrevLeftIndex = nbPointsArr-1;
        else
            currentPrevLeftIndex = currentPrevLeftIndex-1;
        left = point_arr[currentLeftIndex];
        prevLeft = point_arr[currentPrevLeftIndex];

        leftAngle = atan2(prevLeft.y-left.y,prevLeft.x-left.x)-atan2(center.y-prevLeft.y, center.x-prevLeft.x);
        while(leftAngle > M_PI)
            leftAngle = leftAngle - 2*M_PI;
        while(leftAngle < -M_PI)
            leftAngle = leftAngle + 2*M_PI;
        cumulAngle = cumulAngle + leftAngle - prevLeftAngle;
        prevLeftAngle = leftAngle;

        if(cumulAngle > M_PI/2.0)
            return LPoint_Set(-1,-1);

        leftAngle = fabs(leftAngle);
    }

    dxLeft = prevLeft.x-left.x;
    dyLeft = prevLeft.y-left.y;
    dxRight = nextRight.x-right.x;
    dyRight = nextRight.y-right.y;

    // return angles
    *returnedLeftAngle = (atan2(dyLeft,dxLeft) - M_PI/2.0)*180/M_PI;
    while(*returnedLeftAngle < 0)
        *returnedLeftAngle = *returnedLeftAngle + 360;
    while(*returnedLeftAngle > 360)
        *returnedLeftAngle = *returnedLeftAngle - 360;

    *returnedRightAngle = (atan2(dyRight,dxRight) + M_PI/2.0)*180/M_PI;
    while(*returnedRightAngle < 0)
        *returnedRightAngle = *returnedRightAngle + 360;
    while(*returnedRightAngle > 360)
        *returnedRightAngle = *returnedRightAngle - 360;

    //return center; (-1,-1) if not able to find correct tangent points
    return center;
}

void AATorusFilletWithoutDeformation(void)
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer  pLayer;

    LObject obj_arr[MAX_NUMBER_POLYGON];
    int nbPolygonSelected = 0;

    LLayer tmpLayer;
    LLayer tmpLayerGrow;
    LLayer tmpLayerWithAllPolygons;

    char strLayer[MAX_LAYER_NAME];

    LPoint original_point_arr[MAX_POLYGON_SIZE];
    long originalNumberVertex = 0;
    LPoint point_arr[MAX_POLYGON_SIZE];
    long numberVertex = 0;

    LPoint points_from_grow[MAX_POLYGON_SIZE];
    int numberPointsFromGrow = 0;
    LPoint points_already_used[MAX_POLYGON_SIZE];
    int numberPointsAlreadyUsed = 0;
    int isAlreadyUsed = 0;

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
    double leftAngle, rightAngle;

    int onlyWithLabel = 0;
    LCoord xLabel, yLabel;

    double fillet;
    char strFillet[20];

    LTorusParams tParams;

    int i = 0;
    int j = 0;
    int newAngleIndex = 0;
    int cpt = 0;

    long minDist;
    LPoint savedPoint;


    LUpi_LogMessage("\n\n\n\n\nSTART MACRO\n");

    if(LSelection_GetList() == NULL) //if no selection made
	{
        LUpi_LogMessage("No selection were made\n");
		return;
	}
    else
    {
        strcpy(strFillet, "2.5"); //preloaded text in the dialog box
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
        
        if( LDialog_YesNoBox("Fillet all the angles? (No = only angles near a label)") )
            onlyWithLabel = 0;
        else
            onlyWithLabel = 1;

        //create the tmp layers
        LLayer_New( pFile, NULL, "tmp");
        tmpLayer = LLayer_Find(pFile, "tmp");

        LLayer_New( pFile, NULL, "tmpGrow");
        tmpLayerGrow = LLayer_Find(pFile, "tmpGrow");

        LLayer_New( pFile, NULL, "tmpLayerWithAllPolygons");
        tmpLayerWithAllPolygons = LLayer_Find(pFile, "tmpLayerWithAllPolygons");
        
        //for each selected element
        for(LSelection pSelection = LSelection_GetList() ; pSelection != NULL; pSelection = LSelection_GetNext(pSelection) )
        {
            LObject object = LSelection_GetObject(pSelection);
            //detect if it is an instance
            if( LObject_GetShape(object) == LObjInstance)
            {
                
                //set the tranform to null operand (0 translation and orientation, 1 magnification) for the first iteration of recursive function
                LTransform_Ex99 transformOrigin;
                transformOrigin.translation.x = 0;
                transformOrigin.translation.y = 0;
                transformOrigin.orientation = 0;
                transformOrigin.magnification.num = 1;
                transformOrigin.magnification.denom = 1;
                RecursiveInstanceDetection((LInstance)object, pCell, pLayer, tmpLayerWithAllPolygons, transformOrigin);
            }
            else
            {
                //copy in the right tmp layer
                LObject_Copy( pCell, tmpLayerWithAllPolygons, object );
            }
        }
        //all polygons from the tmp layer in an object array for bollean aperation
        for(LObject object = LObject_GetList(pCell, tmpLayerWithAllPolygons) ; object != NULL ; object = LObject_GetNext(object) )
        {
            obj_arr[nbPolygonSelected] = object;
            nbPolygonSelected++;
        }
        LUpi_LogMessage(LFormat("nbPolygonSelected %d\n", nbPolygonSelected));

        //OR with all polygons
        LCell_BooleanOperation(pCell,
                               LBoolOp_OR, 
                               NULL, 
                               obj_arr, 
                               nbPolygonSelected, 
                               NULL, 
                               0, 
                               tmpLayer, 
                               LFALSE );

        //OR and GROW for all polygons
        LCell_BooleanOperation(pCell,
                               LBoolOp_OR, 
                               NULL, 
                               obj_arr, 
                               nbPolygonSelected, 
                               NULL, 
                               0, 
                               tmpLayerGrow, 
                               LFALSE );
        nbPolygonSelected = 0;
        for(LObject obj = LObject_GetList(pCell, tmpLayerGrow) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            obj_arr[nbPolygonSelected] = obj;
            nbPolygonSelected++;
        }
        LCell_BooleanOperation(pCell,
                               LBoolOp_GROW, 
                               fillet, 
                               obj_arr, 
                               nbPolygonSelected, 
                               NULL, 
                               0, 
                               tmpLayerGrow, 
                               LTRUE );

        //for each polygons after the OR/OR-GROW
        //detect and store all points that need to be fillet
        for(LObject obj = LObject_GetList(pCell, tmpLayerGrow) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            originalNumberVertex = LVertex_GetArray( obj, original_point_arr, MAX_POLYGON_SIZE );

            for(i=0; i<originalNumberVertex; i++) //store the current, previous and next point
            {
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
                
                if( angle < M_PI - ANGLE_LIMIT ) //if not in the limit range and concave
                {
                    points_from_grow[numberPointsFromGrow] = original_point_arr[i];
                    numberPointsFromGrow++;
                }
            }
        }

        //for each polygons after the OR/OR-GROW
        //create the torus on the right place
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
                
                if( angle < M_PI - ANGLE_LIMIT ) //if not in the limit range and concave
                {
                    if(onlyWithLabel == 1) //if only angle near label
                    {
                        int hasLabelNear = 0;
                        for(LSelection pSelection = LSelection_GetList() ; pSelection != NULL; pSelection = LSelection_GetNext(pSelection) )
                        {
                            LObject object = LSelection_GetObject(pSelection);
                            if(LObject_GetShape(object) == LObjLabel)
                            {
                                LPoint labelLocation = LLabel_GetPosition((LLabel)object);
                                if(PointDistance(original_point_arr[i], labelLocation) < LIMIT_DISTANCE_POINT_LABEL)
                                    hasLabelNear = 1;
                            }
                        }
                        if(hasLabelNear == 0) //if no label detected
                            continue;
                    }

                    LUpi_LogMessage("Point need to be fillet\n");
                    
                    savedPoint = LPoint_Set(-1,-1);
                    minDist = 9999999999.999999999;
                    for(j = 0; j < numberPointsFromGrow; j++) //find the closest point saved from the grow
                    {
                        if(centerIsBetweenPoints(LPoint_Set(prevX,prevY), original_point_arr[i], LPoint_Set(nextX,nextY), points_from_grow[j]) == 1)
                        {
                            if(PointDistance(original_point_arr[i], points_from_grow[j]) < minDist)
                            {
                                savedPoint = points_from_grow[j];
                                minDist = PointDistance(original_point_arr[i],points_from_grow[j]);
                            }
                        }
                    }
                    points_already_used[numberPointsAlreadyUsed] = savedPoint;
                    numberPointsAlreadyUsed = numberPointsAlreadyUsed + 1;
                    center = savedPoint; //center is one of the point saved from the grow
                    
                    center = FindTangentFromCenter(i, original_point_arr, originalNumberVertex, fillet, center, &rightAngle, &leftAngle); //find the tangent angles

                    if( !(center.x == -1 && center.y == -1) )
                    {
                        minDist = 9999999999.999999999;
                        for(j = 0; j < numberPointsFromGrow; j++)
                        {
                            if(PointDistance(center,points_from_grow[j])<minDist)
                            {
                                savedPoint = points_from_grow[j];
                                minDist = PointDistance(center,points_from_grow[j]);
                            }
                        }
                        center = savedPoint;

                        LUpi_LogMessage(LFormat("\n\ncenter %ld %ld\n",center.x,center.y));
                        LUpi_LogMessage(LFormat("fillet %lf\n",fillet));
                        LUpi_LogMessage(LFormat("nOuterRadius %lf\n",PointDistance(original_point_arr[i], center)));
                        LUpi_LogMessage(LFormat("rightAngle %lf\n",rightAngle));
                        LUpi_LogMessage(LFormat("leftAngle %lf\n\n\n",leftAngle));

                        if(PointDistance(original_point_arr[i], center) > 100*fillet) //if too big
                        {
                            tParams.ptCenter = original_point_arr[i];
                            tParams.nInnerRadius = fillet;
                            tParams.nOuterRadius = fillet+1000; //only a 1 micron torus
                        }
                        else
                        {
                            tParams.ptCenter = center;
                            tParams.nInnerRadius = fillet;
                            tParams.nOuterRadius = max(PointDistance(original_point_arr[i], center)*1.05, fillet*1.5); //a little bit more than the distance center-angle
                        }
                        tParams.dStartAngle = rightAngle;
                        tParams.dStopAngle = leftAngle;

                        LTorus_CreateNew( pCell, pLayer, &tParams ); //create the torus
                    }
                    else
                    {
                        LUpi_LogMessage(LFormat("\n\nERROR: An angle could not be fillet automatically\n\n"));
                    }

                    LDisplay_Refresh();
                }
                if(i!=originalNumberVertex-1)
                    LUpi_LogMessage("Test the next point\n");
            }
        }
    }

    if(onlyWithLabel != 1)
    {
        LUpi_LogMessage("\n\nTest the last points\n");
        for(i=0; i<numberPointsFromGrow; i++) //add torus if a point from the grow has not been processed
        {
            isAlreadyUsed = 0;
            for(j=0; j<numberPointsAlreadyUsed; j++)
            {
                if(points_from_grow[i].x == points_already_used[j].x && points_from_grow[i].y == points_already_used[j].y)
                    isAlreadyUsed = 1;
            }
            if(isAlreadyUsed == 0)
            {
                LUpi_LogMessage("traitement \n");
                LPoint prev, next, current;
                for(LObject obj = LObject_GetList(pCell, tmpLayerGrow) ; obj != NULL; obj = LObject_GetNext(obj) )
                {
                    originalNumberVertex = LVertex_GetArray( obj, original_point_arr, MAX_POLYGON_SIZE );

                    for(j=0; j<originalNumberVertex; j++) //store the current, previous and next point
                    {
                        current = original_point_arr[j];
                        if(current.x == points_from_grow[i].x && current.y == points_from_grow[i].y)
                        {
                            if(j == 0)
                                prev = original_point_arr[originalNumberVertex-1];
                            else
                                prev = original_point_arr[j-1];
                            if(j == originalNumberVertex-1)
                                next = original_point_arr[0];
                            else
                                next = original_point_arr[j+1];

                            leftAngle = (atan2(prev.y-current.y,prev.x-current.x) - M_PI/2.0)*180/M_PI;
                            while(leftAngle < 0)
                                leftAngle = leftAngle + 360;
                            while(leftAngle > 360)
                                leftAngle = leftAngle - 360;

                            rightAngle = (atan2(next.y-current.y,next.x-current.x) + M_PI/2.0)*180/M_PI;
                            while(rightAngle < 0)
                                rightAngle = rightAngle + 360;
                            while(rightAngle > 360)
                                rightAngle = rightAngle - 360;

                            tParams.ptCenter = points_from_grow[i];
                            tParams.nInnerRadius = fillet;
                            tParams.nOuterRadius = fillet*2;
                            tParams.dStartAngle = rightAngle;
                            tParams.dStopAngle = leftAngle;

                            LTorus_CreateNew( pCell, pLayer, &tParams );
                        }
                    }
                }
            }
        }
    }

    //delete the objects and tmp layers
    for(LObject obj = LObject_GetList(pCell, tmpLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpLayer );
    for(LObject obj = LObject_GetList(pCell, tmpLayerGrow) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpLayerGrow );
    for(LObject obj = LObject_GetList(pCell, tmpLayerWithAllPolygons) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpLayerWithAllPolygons );

    LUpi_LogMessage(LFormat("\nEND MACRO\n"));
}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F3" /*hotkey*/, "AEPONYX\\Fillet AA polygons without deformation\nWindow", "AATorusFilletWithoutDeformation", NULL /*hotkey category*/);
	return 1;
}