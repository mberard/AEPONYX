#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include <string>

#include <ldata.h>



#define EXCLUDE_LEDIT_LEGACY_UPI //This statement make the C language macros, which are now superseded by C++ functions, unavailable.

extern "C" {
    #include "DubinsPath.hpp"
    #include "DubinsPoint.hpp"

    #include "DubinsPath.cpp"
    #include "DubinsPoint.cpp"

    void DubinsMacro(void);
	int UPI_Entry_Point(void);
}

void DubinsMacro()
{
    LUpi_LogMessage( "Macro DEBUT\n" );

    DubinsPath path;
    DubinsPoint start, end;

    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;
    char sLayerName[MAX_LAYER_NAME];
    char strLayer[MAX_LAYER_NAME];

    LPoint pLabelLocation;
    double xPosLabel, yPosLabel;
    char startLabelName[MAX_CELL_NAME];
    char endLabelName[MAX_CELL_NAME];
    char sLabelName[MAX_CELL_NAME];

    char startCellName[MAX_CELL_NAME];
    char endCellName[MAX_CELL_NAME];

    double dAngle;
    int nmbLabel = 0;

    strcpy(strLayer, "WGUIDE"); //preloaded text in the dialog box
	if ( LDialog_InputBox("Layer", "Enter name of the layer of the active cell in which the guide will be loaded", strLayer) == 0)
		return;
	else
		pLayer = LLayer_Find(pFile, strLayer);

	if(NotAssigned(pLayer)) //the layer is not found
	{
		LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
		return;
    }
    LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
    //LDialog_AlertBox(LFormat("The guide will be added in Layer %s", sLayerName));

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
            pLabelLocation = LLabel_GetPosition( pLabel );
            xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

            start.SetPoint(xPosLabel , yPosLabel, pFile);

            if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
            {
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                {
                    start.SetAngleDegre( dAngle );
                }	
                else
                    LUpi_LogMessage("Angle GetPropertyValue failed\n");
            }		
            else
            {
                LUpi_LogMessage("Angle property not found\n");
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
            pLabelLocation = LLabel_GetPosition( pLabel );
            xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

            end.SetPoint(xPosLabel , yPosLabel, pFile);

            if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
            {
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                {
                    end.SetAngleDegre( dAngle );
                }	
                else
                    LUpi_LogMessage("Angle GetPropertyValue failed\n");
            }		
            else
            {
                LUpi_LogMessage("Angle property not found\n");
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

    path.SetFile(pFile);
    path.SetCell(pCell);
    path.SetLayer(pLayer);

    path.SetStartPoint(start);
    path.SetEndPoint(end);

    strcpy(strLayer, "10");
    if ( LDialog_InputBox("Radius", "Select the radius of the circles in microns", strLayer) == 0)
		return;
	else
    {
        float radius = atof(strLayer);
        path.SetRadius(radius);
    }

    strcpy(strLayer, "1");
    if ( LDialog_InputBox("Guide width", "Select the radius of the circles in microns", strLayer) == 0)
		return;
	else
    {
        float width = atof(strLayer);
        LUpi_LogMessage( LFormat("width %f\n",width) );
        path.SetGuideWidth(width);
    }
    
    if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
        path.DrawLine();
    else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
        if(start.GetAngleRadian() == end.GetAngleRadian())
            LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
        else
            LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
    else
    {
        path.ComputeDubinsPaths();
        path.RasterizePath();
    }

    LUpi_LogMessage( "Macro FIN\n" );

}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Dubins Macro", "DubinsMacro", NULL /*hotkey category*/);
	return 1;
}