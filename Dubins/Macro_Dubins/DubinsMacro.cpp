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

    LLabel startLabel, endLabel;
    LPoint pLabelLocation;
    double xPosLabel, yPosLabel;
    char startLabelName[MAX_CELL_NAME];
    char endLabelName[MAX_CELL_NAME];
    char value_buffer[MAX_CELL_NAME];
    char* token;
    char sLabelName[MAX_CELL_NAME];
    double dAngle;
    int nmbLabel = 0;

    strcpy(strLayer, "WGUIDE"); //preloaded text in the dialog box
	if ( LDialog_InputBox("Layer", "Enter name of the layer in which the guide will be loaded", strLayer) == 0)
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

    strcpy(value_buffer, "start;end");
    if ( LDialog_InputBox("Start/end points", "Enter the name of the start and end label separate by ';'", value_buffer) == 0)
        return;

    token = strtok(value_buffer, ";");
    strcpy(startLabelName, token);
    token = strtok(NULL , ";");
    strcpy(endLabelName, token);

    for(LLabel pLabel = LLabel_GetList(pCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
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
        }
        else if(strcmp(sLabelName, endLabelName) == 0)
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
        }
	}

    if(nmbLabel != 2)
    {
        LUpi_LogMessage( LFormat("ERROR: %d LLabel(s) found instead of 2\n",nmbLabel) );
        return;
    }

    path.SetFile(pFile);
    path.SetCell(pCell);
    path.SetLayer(pLayer);

    path.SetStartPoint(start);
    path.SetEndPoint(end);
    path.SetRadius(10);

    path.SetGuideWidth(1);
    
    path.ComputeDubinsPaths();

    LUpi_LogMessage( "Macro FIN\n" );

}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Dubins Macro", "DubinsMacro", NULL /*hotkey category*/);
	return 1;
}