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
    LDialog_AlertBox(LFormat("The guide will be added in Layer %s", sLayerName));

    start.SetPoint(0.0 , 0.0, pFile);
    start.SetAngleDegre(0.0);
    end.SetPoint(20.0 , 10.0, pFile);
    end.SetAngleDegre(0.0);

    path.SetFile(pFile);
    path.SetCell(pCell);
    path.SetLayer(pLayer);

    path.SetStartPoint(start);
    path.SetEndPoint(end);
    path.SetRadius(10);

    path.SetGuideWidth(0.5);
    
    path.ComputeDubinsPaths();

    LUpi_LogMessage( "Macro FIN\n" );

}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Dubins Macro", "DubinsMacro", NULL /*hotkey category*/);
	return 1;
}