#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

double PointDistance(LPoint start, LPoint end)
{
    double dist=0.0;
    dist = (double)(end.x - start.x)*(end.x - start.x);
    dist += (double)(end.y - start.y)*(end.y - start.y);
    dist = sqrt(dist);
    return dist;
}

int twoLabelsHasBeenSelected()
{
    char strName[MAX_CELL_NAME];
    LSelection pSelection = LSelection_GetList();
    if(pSelection == NULL)
        return 0;
    else
    {
        int cpt = 0;
        for(pSelection = LSelection_GetList() ; pSelection != NULL; pSelection = LSelection_GetNext(pSelection) )
        {
            LObject object = LSelection_GetObject(pSelection);
            if(LObject_GetShape(object) == LObjLabel)
            {
                cpt = cpt +1;
                LUpi_LogMessage( LFormat("%s\n",LLabel_GetName( (LLabel)object, strName, MAX_CELL_NAME ) ) );
            }
            else
            {
                LUpi_LogMessage( "A non label object has been selected ==> manual selection\n" );
                return 0;
            }
        }
        if(cpt == 2)
            return 1;
        else
        {
            LUpi_LogMessage( LFormat("Need to select exactly 2 labels: %d selected ==> manual selection\n",cpt) );
            return 0;
        }
    }
}

void ComputeOptimalPlaceMacro()
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;

    char strLayer[MAX_LAYER_NAME];

    char startLabelName[MAX_CELL_NAME];
    char endLabelName[MAX_CELL_NAME];
    char sLabelName[MAX_CELL_NAME];
    char startCellName[MAX_CELL_NAME];
    char endCellName[MAX_CELL_NAME];

    LPoint startPoint, endPoint;
    double startAngle, endAngle;
    double dAngle;
    int nmbLabel;
    double radius;
    double angle;

    long difference;

    long xMaxStart, xMinStart, yMaxStart, yMinStart;
    long xMaxEnd, xMinEnd, yMaxEnd, yMinEnd;

    LPoint startCenter, endCenter;

    if(twoLabelsHasBeenSelected() == 1)
    {
        LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
        LSelection pSelection = LSelection_GetList() ;

        LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
        LLabel pLabel = (LLabel)object;

        endPoint = LLabel_GetPosition( pLabel );
        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
        {
            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
            {
                endAngle = dAngle;
            }	
            else
            {
                endAngle = 0;
                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
            }
        }		
        else
        {
            endAngle = 0;
            LUpi_LogMessage("Angle property not found, 0 by default\n");
        }

        pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
        object = LSelection_GetObject(pSelection);
        pLabel = (LLabel)object;

        startPoint = LLabel_GetPosition( pLabel );
        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
        {
            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
            {
                startAngle = dAngle;
            }	
            else
            {
                startAngle = 0;
                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
            }
        }
        else
        {
            startAngle = 0;
            LUpi_LogMessage("Angle property not found, 0 by default\n");
        }
    }
    else
    {
        LDialogItem DialogItems[2] = {{ "Cell","cell1"}, { "Name","P1"}};
        if (LDialog_MultiLineInputBox("Start point",DialogItems,2) == 0)
            return;
        strcpy(startCellName,DialogItems[0].value);
        strcpy(startLabelName,DialogItems[1].value);

        LCell startCell = LCell_Find( pFile, startCellName );
        if(startCell == NULL)
        {
            LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n", startCellName) );
            return;
        }

        for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
        {
            LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
            
            if(strcmp(sLabelName, startLabelName) == 0)
            {
                startPoint = LLabel_GetPosition( pLabel );

                if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                {
                    if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                    {
                        startAngle = dAngle;
                    }	
                    else
                    {
                        startAngle = 0;
                        LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                    }
                        
                }		
                else
                {
                    startAngle = 0;
                    LUpi_LogMessage("Angle property not found, 0 by default\n");
                }
                nmbLabel++;
                break;
            }
        }
        if(nmbLabel != 1)
        {
            LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",startLabelName, startCellName) );
            return;
        }



        LDialogItem DialogItemsEnd[2] = {{ "Cell","cell2"}, { "Name","P2"}};
        if (LDialog_MultiLineInputBox("End point",DialogItemsEnd,2) == 0)
            return;
        strcpy(endCellName,DialogItemsEnd[0].value);
        strcpy(endLabelName,DialogItemsEnd[1].value);

        LCell endCell = LCell_Find( pFile, endCellName );
        if(endCell == NULL)
        {
            LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n",endCellName) );
            return;
        }

        for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
        {
            LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
            
            if(strcmp(sLabelName, endLabelName) == 0)
            {
                endPoint = LLabel_GetPosition( pLabel );

                if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                {
                    if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                    {
                        endAngle = dAngle;
                    }	
                    else
                    {
                        endAngle = 0;
                        LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                    }
                        
                }		
                else
                {
                    endAngle = 0;
                    LUpi_LogMessage("Angle property not found, 0 by default\n");
                }
                nmbLabel++;
                break;
            }
        }

        if(nmbLabel != 2)
        {
            LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",endLabelName, endCellName) );
            return;
        }
    }

    strcpy(strLayer, "10");
    if ( LDialog_InputBox("Radius", "Select the radius of the circles in microns", strLayer) == 0)
        return;
    else
        radius = LFile_MicronsToIntU( pFile, atof(strLayer) );

    //computing
    angle = fmod(endAngle - startAngle, 360);
    while(angle < 0)
        angle = angle + 360;

    if(angle > 270 || angle < 90) //less than 90 degres difference; target: RSL or LSR
    {
        if(startPoint.y < endPoint.y) //target: LSR
        {
            startCenter = LPoint_Set(startPoint.x+radius*cos((startAngle+90)*M_PI/180.0), startPoint.y+radius*sin((startAngle+90)*M_PI/180.0));
            endCenter = LPoint_Set(endPoint.x+radius*cos((endAngle-90)*M_PI/180.0), endPoint.y+radius*sin((endAngle-90)*M_PI/180.0));
        }
        else //target: RSL
        {
            startCenter = LPoint_Set(startPoint.x+radius*cos((startAngle-90)*M_PI/180.0), startPoint.y+radius*sin((startAngle-90)*M_PI/180.0));
            endCenter = LPoint_Set(endPoint.x+radius*cos((endAngle+90)*M_PI/180.0), endPoint.y+radius*sin((endAngle+90)*M_PI/180.0));
        }

        xMinStart = startCenter.x - radius ;
        xMaxStart = startCenter.x + radius ;
        yMinStart = startCenter.y - radius ;
        yMaxStart = startCenter.y + radius ;
        xMinEnd = endCenter.x - radius ;
        xMaxEnd = endCenter.x + radius ;
        yMinEnd = endCenter.y - radius ;
        yMaxEnd = endCenter.y + radius ;
//LUpi_LogMessage(LFormat("angles %lf %lf %lf\n\n",angle-90,angle,angle+90 ));
//LUpi_LogMessage(LFormat("endCenter %ld %ld\n\n",endCenter.x, endCenter.y ));
//LUpi_LogMessage(LFormat("xMinStart %ld\nxMaxStart %ld\nyMinStart %ld\nyMaxStart %ld\nxMinEnd %ld\nxMaxEnd %ld\nyMinEnd %ld\nyMaxEnd %ld\n\n", xMinStart,xMaxStart,yMinStart,yMaxStart,xMinEnd,xMaxEnd,yMinEnd,yMaxEnd));

        //for fixed Y
        if( (yMinStart > yMinEnd && yMinStart < yMaxEnd) || (yMaxStart > yMinEnd && yMaxStart < yMaxEnd) )
        {
            difference = 0;
            while(PointDistance(startCenter, endCenter) > 2*radius)
            {
                if(startCenter.x < endCenter.x)
                {
                    endCenter.x = endCenter.x-1;
                    difference = difference-1;
                }
                else
                {
                    endCenter.x = endCenter.x+1;
                    difference = difference+1;
                }
            }
            while(PointDistance(startCenter, endCenter) < 2*radius)
            {
                if(startCenter.x < endCenter.x)
                {
                    endCenter.x = endCenter.x+1;
                    difference = difference+1;
                }
                else
                {
                    endCenter.x = endCenter.x-1;
                    difference = difference-1;
                }
            }
            LUpi_LogMessage(LFormat("\nWITH FIXED Y:\n\tOptimal X distance (from start to end): %lf\n\tOptimal X start point (with fixed end): %lf\n\tOptimal X end point (with fixed start): %lf\n", LFile_IntUtoMicrons(pFile, endPoint.x-startPoint.x+difference), LFile_IntUtoMicrons(pFile, startPoint.x-difference), LFile_IntUtoMicrons(pFile, endPoint.x+difference) ));
        }
        else
            LUpi_LogMessage("\nERROR: Impossible to find an optimal position with Y fixed\n");
    
        //for fixed x
        if( (xMinStart > xMinEnd && xMinStart < xMaxEnd) || (xMaxStart > xMinEnd && xMaxStart < xMaxEnd) )
        {
            difference = 0;
            while(PointDistance(startCenter, endCenter) > 2*radius)
            {
                if(startCenter.y < endCenter.y)
                {
                    endCenter.y = endCenter.y-1;
                    difference = difference-1;
                }
                else
                {
                    endCenter.y = endCenter.y+1;
                    difference = difference+1;
                }
            }
            while(PointDistance(startCenter, endCenter) < 2*radius)
            {
                if(startCenter.y < endCenter.y)
                {
                    endCenter.y = endCenter.y+1;
                    difference = difference+1;
                }
                else
                {
                    endCenter.y = endCenter.y-1;
                    difference = difference-1;
                }
            }
            LUpi_LogMessage(LFormat("WITH FIXED X:\n\tOptimal Y distance (from start to end): %lf\n\tOptimal Y start point (with fixed end): %lf\n\tOptimal Y end point (with fixed start): %lf\n", LFile_IntUtoMicrons(pFile, endPoint.y-startPoint.y+difference), LFile_IntUtoMicrons(pFile, startPoint.y-difference), LFile_IntUtoMicrons(pFile, endPoint.y+difference) ));
        }
        else
            LUpi_LogMessage("\nERROR: Impossible to find an optimal position with X fixed\n");
    }
/*
    else if(endAngle == startAngle-90 || endAngle == startAngle+90) //90 degres angle; target: only a quarter of circle
    {

    }
    else //more than 90 degres difference; target: RSR or LSL
    {
        
    }
*/

}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F9" /*hotkey*/, "AEPONYX\\Compute Optimal Place Macro\nWindow", "ComputeOptimalPlaceMacro", NULL /*hotkey category*/);
	return 1;
}