/*******************************************************************************
 * Macro Name: Polygon Curve
 * Creator  : Michael Menard
 *
 * Revision History:
 * 19 Jun 2017 version 1.0	
 *******************************************************************************/
module Polygon_curve_waveguide_module
{
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include "ldata.h"
/* Begin -- Remove this block if you are not using L-Comp. */
//#include "lcomp.h"
/* End */

#define Pi 3.141592654
 
//*************************************************************************
//* PROTOTYPES
//*************************************************************************
LLayer CheckLayer(LFile pTDBFile, const char* szLayerName, const char* szLayerTitle);
//void Debug_out(LFile pTDBFile, char STR_out[256], LLen Input, LLen Center, LLen Output, LLen Curve, LLen Bg, LLen Off);
LLen Round(LFile pTDBFile, LLen r_length);
	void Polygon_curve(void)
	{
		static char szLayer[256] = "Waveguide_critical";
		static double dXcoord = -0.2175;
		static double dYcoord = 0.7825;
		static double dR1 = 100.2175;
		static double dR2 = 99.7825;
		static double dStart = 90.0;
		static double dEnd = 180.0;
		static double dDelta = 0.1;
												
		int i;
		double Nb_points;
		char Out1[256];
		
		LCell pCell = LCell_GetVisible(); 		// The current cell.
		if(NotAssigned(pCell))
		{
			LDialog_AlertBox("ERROR:  Could not find a Visible Cell.");
			return;
		}
		
		LFile pTDBFile = LCell_GetFile(pCell);		// The TDB current file.
		if(NotAssigned(pTDBFile))
		{
			LDialog_AlertBox("ERROR:  Could not get the TDB file from the Visible Cell.");
			return;
		}

		LDialogItem Dialog_Items[8];
		strcpy(Dialog_Items[0].prompt, "Layer:");
		strcpy(Dialog_Items[1].prompt, "X coordinate:");
		strcpy(Dialog_Items[2].prompt, "Y coordinate:");
		strcpy(Dialog_Items[3].prompt, "Outer radius (um):");
		strcpy(Dialog_Items[4].prompt, "Inner radius (um):");
		strcpy(Dialog_Items[5].prompt, "Start angle (degree):");
		strcpy(Dialog_Items[6].prompt, "End angle (degree):");
		strcpy(Dialog_Items[7].prompt, "Angle delta (degree):");
						
		strcpy(Dialog_Items[0].value, szLayer);
		sprintf(Dialog_Items[1].value, "%lf", dXcoord);
		sprintf(Dialog_Items[2].value, "%lf", dYcoord);
		sprintf(Dialog_Items[3].value, "%lf", dR1);
		sprintf(Dialog_Items[4].value, "%lf", dR2);
		sprintf(Dialog_Items[5].value, "%lf", dStart);
		sprintf(Dialog_Items[6].value, "%lf", dEnd);
		sprintf(Dialog_Items[7].value, "%lf", dDelta);
				
		if(!LDialog_MultiLineInputBox("Access Wvgds Parameters", Dialog_Items, 8))
		{
			return;
		}
		// Do some error checking.
		LLayer pLayer = CheckLayer(pTDBFile, Dialog_Items[0].value, "Waveguide");
		if(NotAssigned(pLayer)) return;
		
		//Declare variables & assign values from dialogue box
		strcpy(szLayer, Dialog_Items[0].value);
		LCoord X = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[1].value));
		LCoord Y = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[2].value));
		LLen R1 = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[3].value));
		LLen R2 = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[4].value));
		double Start = atof(Dialog_Items[5].value);
		double End = atof(Dialog_Items[6].value);
		double Delta = atof(Dialog_Items[7].value);
		
		Nb_points=2*(((End-Start)/Delta)+1);
		
		//LDialog_AlertBox(LFormat("Nb_points = %lf",Nb_points));
		
		int Array = Nb_points;
			
		LPoint Point_arr[Array];
					
		for(i=0; i < Nb_points; ++i)
		{
			if(i < Nb_points/2)
			{
				Point_arr[i]=LPoint_Set(X+R1*cos((Start+Delta*i)*Pi/180),Y+R1*sin((Start+Delta*i)*Pi/180));
				//if(i==0 || i==(Nb_points/2-1))
				//{
					//Point_arr[i]=LPoint_Set(Round(pTDBFile, X+R1*cos((Start+Delta*i)*Pi/180)),Round(pTDBFile, Y+R1*sin((Start+Delta*i)*Pi/180)));
				//}
			}
			if(i >= Nb_points/2)
			{
				Point_arr[i]=LPoint_Set(X+R2*cos((Start+Delta*(Nb_points-i-1))*Pi/180),Y+R2*sin((Start+Delta*(Nb_points-i-1))*Pi/180));
				//if(i==(Nb_points/2) || i==(Nb_points))
				//{
						//Point_arr[i]=LPoint_Set(Round(pTDBFile, X+R2*cos((Start+Delta*(Nb_points-i-1))*Pi/180)),Round(pTDBFile,Y+R2*sin((Start+Delta*(Nb_points-i-1))*Pi/180) ));
				//}*/
			}
			
		}
				
		LPolygon_New(pCell, pLayer, Point_arr, Nb_points);					
						
		LDisplay_Refresh();
		
	}
//*************************************************************************
// Function Name    : CheckLayer
// Parameters       : pTDBFile
//                    szLayerName
//                    szLayerTitle
// Description      : 
// Returns          : 
//*************************************************************************
//  Revision History
//   03/11/2003 - NJW - Created function.
//*************************************************************************
	LLayer CheckLayer(LFile pTDBFile, const char* szLayerName, const char* szLayerTitle)
	{
		LLayer pLayer = LLayer_Find(pTDBFile, szLayerName);
		if(NotAssigned(pLayer))
		{
			LDialog_AlertBox(LFormat("Error:  Couldn't find the %s Layer in the TDB File.\nLayer:  \"%s\"\nPlease check the spelling and make sure the layer exists.",  szLayerTitle, szLayerName));
		}
		return(pLayer);
	}

	LLen Round(LFile pTDBFile, LLen r_length)
	{
		LLen R_out;
		long R_long;
		float value;
		value = (LFile_IntUtoMicrons(pTDBFile, r_length)+0.0005)*1000;  //floor +0. round +0.0005
		R_long =(long) value;
		R_out = LFile_DispUtoIntU(pTDBFile, R_long/1000.0);
		//LDialog_AlertBox(LFormat("R_out = %lf",LFile_IntUtoMicrons(pTDBFile, R_out  )));
		return(R_out);
	}
	
	void Polygon_curve_register(void)
	{
		LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Polygon curve", "Polygon_curve", "Macro" /*hotkey category*/);
	}

}
Polygon_curve_register();
