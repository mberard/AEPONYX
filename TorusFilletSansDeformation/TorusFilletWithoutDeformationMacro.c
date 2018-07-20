#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <math.h>

#define MAX_POLYGON_SIZE 15000
#define MAX_NUMBER_POLYGON 1000
#define ANGLE_LIMIT 0.25 //in radian, 0.523599 rad == 30 degrés, 0.785398 rad == 45 degrés, 1.5708 rad == 90 degrés
#define LIMIT_DISTANCE_POINT_LABEL 150 //in internal units
#define LIMIT_FAST_APPROACH_1 1.45
#define LIMIT_FAST_APPROACH_2 1.52
#define LIMIT_FAST_APPROACH_3 1.56

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}

long RoundToLong(double value)
{
    if(value > 0)
        return (long)(value+0.5);
    else
        return (long)(value-0.5);
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

LPoint FindTanAndCenterWithCircleMethod(LPoint* tanLeft, LPoint* tanRight, int angleIndex, LPoint* point_arr, int nbPointsArr, double fillet, float step)
{
    LCell prevCell = LCell_GetVisible();
    LFile pFile = LCell_GetFile(prevCell);
    if(LCell_Find(pFile, "AUTO_CELL_TO_COMPUTE_TORUS_CENTER"))
        LCell_Delete("AUTO_CELL_TO_COMPUTE_TORUS_CENTER");
    LCell pCell = LCell_New( pFile, "AUTO_CELL_TO_COMPUTE_TORUS_CENTER" );
    LLayer pLayer;
    if(LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ))
        LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );
    LLayer_New( pFile, NULL, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" );
    pLayer = LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" );

    LObject circleLeft, circleRight;

    LPoint origin, left, prevLeft, right, nextRight;
    LPoint testPointLeft, testPointRight;
    LPoint lastTestPointLeft, lastTestPointRight;
    LPoint center, betterCenter;
    int currentPrevLeftIndex, currentLeftIndex, currentRightIndex, currentNextRightIndex;
    double dxLeft, dyLeft, dxRight, dyRight;
    double exactPosLeftX, exactPosLeftY, exactPosRightX, exactPosRightY;
    double angle=0, angle1=0, angle2=0;
    double rightAngle, leftAngle;
    int keepCompute = 1;

    double distStart = 0;

    double threshold = 0.001;
    double threshold_max = 0.2;
    double dist, maxDist;

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

    lastTestPointLeft = left;
    lastTestPointRight = right;
    center = origin;
    betterCenter = origin;

    exactPosRightX = right.x;
    exactPosRightY = right.y;
    exactPosLeftX = left.x;
    exactPosLeftY = left.y;

    dxLeft = prevLeft.x - left.x;
    dyLeft = prevLeft.y - left.y;
    while(dxLeft*dxLeft+dyLeft*dyLeft > step*step)
    {
        dxLeft = dxLeft/1.05;
        dyLeft = dyLeft/1.05;
    }

    dxRight = nextRight.x - right.x;
    dyRight = nextRight.y - right.y;
    while(dxRight*dxRight+dyRight*dyRight > step*step)
    {
        dxRight = dxRight/1.05;
        dyRight = dyRight/1.05;
    }

    distStart = (1/tan( (atan2(dyRight,dxRight)-atan2(dyLeft,dxLeft)) /2.0 ))*fillet * 0.95;
    //if(point_arr[currentRightIndex].x == origin.x && point_arr[currentRightIndex].y == origin.y && PointDistance(point_arr[currentRightIndex], origin) > 1/tan( (atan2(dyRight,dxRight)-atan2(dyLeft,dxLeft)) /2.0 )*fillet)
    if(point_arr[currentRightIndex].x == origin.x && point_arr[currentRightIndex].y == origin.y && PointDistance(point_arr[currentNextRightIndex], origin) > 1/tan( (atan2(dyRight,dxRight)-atan2(dyLeft,dxLeft)) /2.0 )*fillet*1.5 && point_arr[currentLeftIndex].x == origin.x && point_arr[currentLeftIndex].y == origin.y && PointDistance(point_arr[currentPrevLeftIndex], origin) > 1/tan( (atan2(dyRight,dxRight)-atan2(dyLeft,dxLeft)) /2.0 )*fillet*1.5)
    {
        exactPosRightX = exactPosRightX + distStart * cos(atan2(dyRight,dxRight));
        exactPosRightY = exactPosRightY + distStart * sin(atan2(dyRight,dxRight));
        exactPosLeftX = exactPosLeftX + distStart * cos(atan2(dyLeft,dxLeft));
        exactPosLeftY = exactPosLeftY + distStart * sin(atan2(dyLeft,dxLeft));
    }
    else
    {
        exactPosRightX = exactPosRightX + dxRight;
        exactPosRightY = exactPosRightY + dyRight;
        exactPosLeftX = exactPosLeftX + dxLeft;
        exactPosLeftY = exactPosLeftY + dyLeft;
    }

    testPointLeft.x = (LCoord)exactPosLeftX;
    testPointLeft.y = (LCoord)exactPosLeftY;
    testPointRight.x = (LCoord)exactPosRightX;
    testPointRight.y = (LCoord)exactPosRightY;

    keepCompute = 1;
    rightAngle = M_PI/2.0;
    leftAngle = M_PI/2.0;

    //LCircle_New( prevCell, LLayer_Find(pFile, "LEFTCIRCLE"), testPointLeft, 10 );
    //LCircle_New( prevCell, LLayer_Find(pFile, "RIGHTCIRCLE"), testPointRight, 10 );

    while( rightAngle < M_PI/2.0 //- threshold 
            //|| rightAngle > M_PI/2.0 + threshold_max
            || leftAngle < M_PI/2.0 //- threshold
            //|| leftAngle > M_PI/2.0 + threshold_max 
            || keepCompute == 1 
            || centerIsBetweenPoints(testPointLeft, origin, testPointRight, center) == 0
         )
    {

        if(keepCompute == 1)
            keepCompute = 0;

        circleRight = LCircle_New(pCell, pLayer, testPointRight, fillet);
        circleLeft = LCircle_New(pCell, pLayer, testPointLeft, fillet);
        
        LCell_BooleanOperation( pCell,
                                LBoolOp_AND,
                                NULL,
                                &circleRight,
                                1,
                                &circleLeft,
                                1,
                                pLayer,
                                LTRUE );

        if(LObject_GetList(pCell, pLayer) == NULL)
        {
            for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
            {
                LObject_Delete( pCell, obj );
            }
            LCell_Delete( pCell );
            LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );
            LUpi_LogMessage(LFormat("Pas de polygone apres le AND\n\tcercle right %ld %ld\n\tcercle left %ld %ld\n", testPointRight.x, testPointRight.y, testPointLeft.x, testPointLeft.y));
            return LPoint_Set(-1,-1);
        }
        
        for(LObject obj = LObject_GetList(pCell, pLayer); obj != NULL; obj = LObject_GetNext(obj))
        {
            maxDist = 0;
            for (LVertex vertex = LObject_GetVertexList(obj); vertex !=NULL; vertex = LVertex_GetNext(vertex))
            {
                if(PointDistance(LVertex_GetPoint(vertex), origin) > maxDist)
                {
                    maxDist = PointDistance(LVertex_GetPoint(vertex), origin);
                    center = LVertex_GetPoint(vertex);
                }
            }
        }

        for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
        {
                LObject_Delete( pCell, obj );
        }

        if(rightAngle < LIMIT_FAST_APPROACH_3 && leftAngle < LIMIT_FAST_APPROACH_3)
        {
            if(leftAngle < LIMIT_FAST_APPROACH_1)
            {
                //exactPosLeftX = exactPosLeftX + dxLeft*100;
                //exactPosLeftY = exactPosLeftY + dyLeft*100;
                exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/5.0, pow(exp(M_PI/2.0-leftAngle), 50));
                exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/5.0, pow(exp(M_PI/2.0-leftAngle), 50));
            }
            else if(leftAngle < LIMIT_FAST_APPROACH_2)
            {
                //exactPosLeftX = exactPosLeftX + dxLeft*40;
                //exactPosLeftY = exactPosLeftY + dyLeft*40;
                exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/5.0, pow(exp(M_PI/2.0-leftAngle), 20));
                exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/5.0, pow(exp(M_PI/2.0-leftAngle), 20));
            }
            else if(leftAngle < LIMIT_FAST_APPROACH_3)
            {
                //exactPosLeftX = exactPosLeftX + dxLeft*10;
                //exactPosLeftY = exactPosLeftY + dyLeft*10;
                exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/5.0, pow(exp(M_PI/2.0-leftAngle), 10));
                exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/5.0, pow(exp(M_PI/2.0-leftAngle), 10));
            }
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
                    dxLeft = dxLeft/1.05;
                    dyLeft = dyLeft/1.05;
                }
                angle2 = atan2(dyLeft, dxLeft);
                
                angle = angle + angle1 - angle2;
                
                if(angle >= M_PI/2.0 || angle <= -M_PI/2.0)
                {
                    for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
                    {
                        LObject_Delete( pCell, obj );
                    }
                    LCell_Delete( pCell );
                    LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );
                    LUpi_LogMessage("Angle cumul trop grand (plus de 90 degres)\n");
                    return LPoint_Set(-1,-1);
                }
            
                exactPosLeftX = exactPosLeftX + dxLeft;
                exactPosLeftY = exactPosLeftY + dyLeft;

                testPointLeft.x = (LCoord)exactPosLeftX;
                testPointLeft.y = (LCoord)exactPosLeftY;
            }


            if(rightAngle < LIMIT_FAST_APPROACH_1)
            {
                //exactPosRightX = exactPosRightX + dxRight*100;
                //exactPosRightY = exactPosRightY + dyRight*100;
                exactPosRightX = exactPosRightX + dxRight*min(fillet/5.0, pow(exp(M_PI/2.0-rightAngle), 50));
                exactPosRightY = exactPosRightY + dyRight*min(fillet/5.0, pow(exp(M_PI/2.0-rightAngle), 50));
            }
            else if(rightAngle < LIMIT_FAST_APPROACH_2)
            {
                //exactPosRightX = exactPosRightX + dxRight*40;
                //exactPosRightY = exactPosRightY + dyRight*40;
                exactPosRightX = exactPosRightX + dxRight*min(fillet/5.0, pow(exp(M_PI/2.0-rightAngle), 20));
                exactPosRightY = exactPosRightY + dyRight*min(fillet/5.0, pow(exp(M_PI/2.0-rightAngle), 20));
            }
            else if(rightAngle < LIMIT_FAST_APPROACH_3)
            {
                //exactPosRightX = exactPosRightX + dxRight*10;
                //exactPosRightY = exactPosRightY + dyRight*10;
                exactPosRightX = exactPosRightX + dxRight*min(fillet/5.0, pow(exp(M_PI/2.0-rightAngle), 10));
                exactPosRightY = exactPosRightY + dyRight*min(fillet/5.0, pow(exp(M_PI/2.0-rightAngle), 10));
            }
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
                    dxRight = dxRight/1.05;
                    dyRight = dyRight/1.05;
                }
                angle2 = atan2(dyLeft, dxLeft);
                
                angle = fabs(angle1 - angle2);
                if(angle >= M_PI)
                {
                    for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
                    {
                        LObject_Delete( pCell, obj );
                    }
                    LCell_Delete( pCell );
                    LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );
                    LUpi_LogMessage("Pas de polygone après le AND");
                    return LPoint_Set(-1,-1);
                }
                exactPosRightX = exactPosRightX + dxRight;
                exactPosRightY = exactPosRightY + dyRight;
                testPointRight.x = (LCoord)exactPosRightX;
                testPointRight.y = (LCoord)exactPosRightY;
            }
        }
        else
        {
            if( rightAngle > leftAngle )
            {
                if(leftAngle < LIMIT_FAST_APPROACH_1)
                {
                    //exactPosLeftX = exactPosLeftX + dxLeft*75;
                    //exactPosLeftY = exactPosLeftY + dyLeft*75;
                    exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 50));
                    exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 50));
                }
                else if(leftAngle < LIMIT_FAST_APPROACH_2)
                {
                    //exactPosLeftX = exactPosLeftX + dxLeft*25;
                    //exactPosLeftY = exactPosLeftY + dyLeft*25;
                    exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 25));
                    exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 25));
                }
                else if(leftAngle < LIMIT_FAST_APPROACH_3)
                {
                    //exactPosLeftX = exactPosLeftX + dxLeft*10;
                    //exactPosLeftY = exactPosLeftY + dyLeft*10;
                    exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 20));
                    exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 20));
                }
                else
                {
                    //exactPosLeftX = exactPosLeftX + dxLeft;
                    //exactPosLeftY = exactPosLeftY + dyLeft;
                    exactPosLeftX = exactPosLeftX + dxLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 15));
                    exactPosLeftY = exactPosLeftY + dyLeft*min(fillet/15.0, pow(exp(M_PI/2.0-leftAngle), 15));
                }

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
                        dxLeft = dxLeft/1.05;
                        dyLeft = dyLeft/1.05;
                    }
                    angle2 = atan2(dyLeft, dxLeft);
                    
                    angle = angle + angle1 - angle2;
                    
                    if(angle >= M_PI/2.0 || angle <= -M_PI/2.0)
                    {
                        for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
                        {
                            LObject_Delete( pCell, obj );
                        }
                        LCell_Delete( pCell );
                        LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );
                        LUpi_LogMessage("Angle cumul trop grand (plus de 90 degres)\n");
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
                if(rightAngle < LIMIT_FAST_APPROACH_1)
                {
                    //exactPosRightX = exactPosRightX + dxRight*75;
                    //exactPosRightY = exactPosRightY + dyRight*75;
                    exactPosRightX = exactPosRightX + dxRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 50));
                    exactPosRightY = exactPosRightY + dyRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 50));
                }
                else if(rightAngle < LIMIT_FAST_APPROACH_2)
                {
                    //exactPosRightX = exactPosRightX + dxRight*25;
                    //exactPosRightY = exactPosRightY + dyRight*25;
                    exactPosRightX = exactPosRightX + dxRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 25));
                    exactPosRightY = exactPosRightY + dyRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 25));
                }
                else if(rightAngle < LIMIT_FAST_APPROACH_3)
                {
                    //exactPosRightX = exactPosRightX + dxRight*10;
                    //exactPosRightY = exactPosRightY + dyRight*10;
                    exactPosRightX = exactPosRightX + dxRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 20));
                    exactPosRightY = exactPosRightY + dyRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 20));
                }
                else
                {
                    //exactPosRightX = exactPosRightX + dxRight;
                    //exactPosRightY = exactPosRightY + dyRight;
                    exactPosRightX = exactPosRightX + dxRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 15));
                    exactPosRightY = exactPosRightY + dyRight*min(fillet/15.0, pow(exp(M_PI/2.0-rightAngle), 15));
                }

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
                        dxRight = dxRight/1.05;
                        dyRight = dyRight/1.05;
                    }
                    angle2 = atan2(dyLeft, dxLeft);
                    
                    angle = fabs(angle1 - angle2);
                    if(angle >= M_PI)
                    {
                        for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
                        {
                            LObject_Delete( pCell, obj );
                        }
                        LCell_Delete( pCell );
                        LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );
                        LUpi_LogMessage("Pas de polygone après le AND");
                        return LPoint_Set(-1,-1);
                    }
                    exactPosRightX = exactPosRightX + dxRight;
                    exactPosRightY = exactPosRightY + dyRight;
                    testPointRight.x = (LCoord)exactPosRightX;
                    testPointRight.y = (LCoord)exactPosRightY;
                }
            }
        }
        
        //LCircle_New( prevCell, LLayer_Find(pFile, "LEFTCIRCLE"), testPointLeft, 10 );
        //LCircle_New( prevCell, LLayer_Find(pFile, "RIGHTCIRCLE"), testPointRight, 10 );
        //LCircle_New( prevCell, LLayer_Find(pFile, "TEST"), center, 10 );
        //LCircle_New( prevCell, LLayer_Find(pFile, "CIRCLE"), origin, 10 );
        lastTestPointLeft = testPointLeft;
        lastTestPointRight = testPointRight;

        rightAngle = atan2(dyRight,dxRight)-atan2(center.y-testPointRight.y, center.x-testPointRight.x);
        while(rightAngle > M_PI)
            rightAngle = rightAngle - 2*M_PI;
        while(rightAngle < -M_PI)
            rightAngle = rightAngle + 2*M_PI;
        rightAngle = fabs(rightAngle);
        
        leftAngle = atan2(dyLeft,dxLeft)-atan2(center.y-testPointLeft.y, center.x-testPointLeft.x);
        while(leftAngle > M_PI)
            leftAngle = leftAngle - 2*M_PI;
        while(leftAngle < -M_PI)
            leftAngle = leftAngle + 2*M_PI;
        leftAngle = fabs(leftAngle);

        LUpi_LogMessage(LFormat("\n\n"));
        LUpi_LogMessage(LFormat(" centre x: %ld y: %ld\n", center.x, center.y));
        LUpi_LogMessage(LFormat("   right angle %lf out of %lf\n", rightAngle, M_PI/2.0 ));
        LUpi_LogMessage(LFormat("   left angle %lf out of %lf\n", leftAngle, M_PI/2.0 ));
    }
    
    for(LObject obj = LObject_GetList(pCell, pLayer) ; obj != NULL; obj = LObject_GetNext(obj))
    {
        LObject_Delete( pCell, obj );
    }
    LCell_Delete( pCell );
    LLayer_Delete( pFile, LLayer_Find( pFile, "AUTO_LAYER_TO_COMPUTE_TORUS_CENTER" ) );

    tanLeft->x = testPointLeft.x;
    tanLeft->y = testPointLeft.y;
    tanRight->x = testPointRight.x;
    tanRight->y = testPointRight.y;

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

    int onlyWithLabel = 0;
    LCoord xLabel, yLabel;

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
        
        if( LDialog_YesNoBox("Fillet only the angle near a selected label? (No for all angle)") )
            onlyWithLabel = 1;
        else
            onlyWithLabel = 0;

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
                
                if( angle < M_PI - ANGLE_LIMIT ) //if not in the limit range and concave
                {
                    if(onlyWithLabel == 1)
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
                        if(hasLabelNear == 0)
                            continue;
                    }

LUpi_LogMessage("Point need to be fillet\n");
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), original_point_arr[i], 100 );
                    
                    center = FindTanAndCenterWithCircleMethod(&tanLeft, &tanRight, i, original_point_arr, originalNumberVertex, fillet, 1.5);
                    if( !(center.x == -1 && center.y == -1) )
                    {
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), tanLeft, 1 );
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), tanRight, 1 );
//LCircle_New( pCell, LLayer_Find(pFile, "TEST"), center, 10 );


                        tParams.ptCenter = center;
                        //tParams.nInnerRadius = max( PointDistance(center, tanLeft), PointDistance(center, tanRight));
                        tParams.nInnerRadius = fillet;
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
                        LUpi_LogMessage(LFormat("\n\nERROR: An angle could not be fillet automatically\n"));
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