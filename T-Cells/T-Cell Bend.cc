/*******************************************************************************
 * Cell Name: Bend
 * Creator  : Tanner EDA Library Development Team
 *
 * Revision History:
 * 17 May 2019	Generated by L-Edit
 *******************************************************************************/
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <string>
#include <time.h>

#define EXCLUDE_LEDIT_LEGACY_UPI
#include <ldata.h>

/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */

/* TODO: Put local functions here. */
void Bend_main(void)
{
	int iTmpUpiReturnCode = LUpi_GetReturnCode();
	/* Begin DO NOT EDIT SECTION generated by L-Edit */
	LCell          cellCurrent    = (LCell)LMacro_GetNewTCell();
	double       radius       = LCell_GetParameterAsDouble(cellCurrent, "radius");
	double       angle        = LCell_GetParameterAsDouble(cellCurrent, "angle");
	double       WG_width     = LCell_GetParameterAsDouble(cellCurrent, "WG width");
	/* End DO NOT EDIT SECTION generated by L-Edit */

	if(LUpi_GetReturnCode())
	{
		LDialog_MsgBox("Error: Tcell failed to read parameters.");
		return;
	}
	LUpi_SetReturnCode(iTmpUpiReturnCode);


	/* Begin -- Uncomment this block if you are using L-Comp. */
		LC_InitializeStateFromCell(cellCurrent);
	/* End */

	/* TODO: Put local variables here. */
	//Get the necessary layers as a LLayer object
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer WGLayer = LLayer_FindByNames(pFile, "WG", "");
	LLayer WGOVLHOLELayer = LLayer_FindByNames(pFile, "WGOVLHOLE", "");
	LLayer WGGROW003Layer = LLayer_FindByNames(pFile, "WGGROW003", "");
	LLayer WGOVL010Layer = LLayer_FindByNames(pFile, "WGOVL010", "");

	LCoord nInnerRadius = 0;
	LCoord nOuterRadius = 0;
	LPoint ptCenter;
	ptCenter.x = LC_Microns(0);
	ptCenter.y = LC_Microns(0);
	
	/* TODO: Begin custom generator code.*/
	nInnerRadius = LC_Microns(radius - WG_width/2);
	nOuterRadius = LC_Microns(radius + WG_width/2);
	
	LTorusParams * pParams = (LTorusParams*) malloc(sizeof(LTorusParams));
	pParams -> ptCenter = ptCenter;
	pParams -> nInnerRadius = nInnerRadius;
	pParams -> nOuterRadius = nOuterRadius;
	pParams -> dStartAngle = 0.0000;
	pParams -> dStopAngle = angle;
	
	//Créer le premier waveguide en suivant les dimentions données
	LTorus_CreateNew(cellCurrent, WGLayer, pParams);
	
	//Créer le WGOVLHOLE à 1.5 microns des waveguides
	pParams -> nInnerRadius = nInnerRadius - LC_Microns(1.5);
	pParams -> nOuterRadius = nOuterRadius + LC_Microns(1.5);
	LTorus_CreateNew(cellCurrent, WGOVLHOLELayer, pParams);
	
	//Créer le WGGROW003 à 3 microns des waveguides
	pParams -> nInnerRadius = nInnerRadius - LC_Microns(3);
	pParams -> nOuterRadius = nOuterRadius + LC_Microns(3);
	LTorus_CreateNew(cellCurrent, WGGROW003Layer, pParams);
	
	//Créer le WGOVL010 à 10 microns des waveguides
	pParams -> nInnerRadius = nInnerRadius - LC_Microns(10);
	pParams -> nOuterRadius = nOuterRadius + LC_Microns(10);
	LTorus_CreateNew(cellCurrent, WGOVL010Layer, pParams);
	
	free(pParams);
	/* End custom generator code.*/
}
extern "C" int UPI_Entry_Point(void)
{
	Bend_main();
	return 1;
}
