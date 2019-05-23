/*******************************************************************************
 * Cell Name: cell6
 * Creator  : Tanner EDA Library Development Team
 *
 * Revision History:
 * 22 May 2019	Generated by L-Edit
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
void cell6_main(void)
{
	int iTmpUpiReturnCode = LUpi_GetReturnCode();
	/* Begin DO NOT EDIT SECTION generated by L-Edit */
	LCell          cellCurrent    = (LCell)LMacro_GetNewTCell();
	double       radius       = LCell_GetParameterAsDouble(cellCurrent, "radius");
	double       width        = LCell_GetParameterAsDouble(cellCurrent, "width");
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
	
	radius += 10.000;
	double theta;
	double x;
	double y;
	double h;
	/* TODO: Begin custom generator code.*/
	
	/*
	* First curve
	*/
	nInnerRadius = LC_Microns(radius);
	nOuterRadius = LC_Microns(radius + WG_width);
	
	LTorusParams * pParams = (LTorusParams*) malloc(sizeof(LTorusParams));
	pParams -> ptCenter = ptCenter;
	pParams -> nInnerRadius = nInnerRadius;
	pParams -> nOuterRadius = nOuterRadius;
	pParams -> dStartAngle = 180.0000;
	pParams -> dStopAngle = 180.0000 + angle;
	
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
	
	/*
	* Second curve
	*/
	//Trouver le point central de la deuxième courbe
	if(angle < 90.0000){
		theta = 90-angle;
		h = 2*radius + WG_width;
		x = -1*(sin(theta*M_PI/180) * (h));
		y = -1*(sqrt(pow(h,2) - pow(x,2)));
	}else if(angle > 90.0000){
		theta = 180-angle;
		h = 2*radius + WG_width;
		y = -1*(sin(theta*M_PI/180) * (h));
		x = sqrt(pow(h,2) - pow(y,2));
	}else{
		x = 0;
		y = radius * (-2) - WG_width;
	}
	
	//redefine the variables 
	nInnerRadius = LC_Microns(radius);
	nOuterRadius = LC_Microns(radius + WG_width);
	ptCenter.x = LC_Microns(x);
	ptCenter.y = LC_Microns(y);
	
	pParams -> nInnerRadius = nInnerRadius;
	pParams -> nOuterRadius = nOuterRadius;
	pParams -> ptCenter = ptCenter;
	pParams -> dStartAngle = 0;
	pParams -> dStopAngle = angle;
	
	//Créer le deuxième waveguide en suivant les dimentions données
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
	cell6_main();
	return 1;
}
