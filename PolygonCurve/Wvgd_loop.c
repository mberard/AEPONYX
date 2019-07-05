/*******************************************************************************
 * Macro Name: Wvgd_loop
 * Creator  : Michael Menard
 *
 * Revision History:
 * 01 Sept 2010
 *******************************************************************************/
module Precise_loop_macro_module
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

#define Pi 3.14159265358979

//*************************************************************************
//* PROTOTYPES
//*************************************************************************
LLayer CheckLayer(LFile pTDBFile, const char* szLayerName, const char* szLayerTitle);
void Draw_length_out(LFile pTDBFile, char STR_out[256], LLen Draw_length[20], int Nb_wvgd, unsigned int Termination);
void Test_length_out(LFile pTDBFile, char STR_out[256], LLen Test_length[4]);
void Coord_out(LFile pTDBFile, LCoord Display_coord[4][4]);

	void Loop(void)
	{
		
		static char szLayer[128] = "Waveguide";
		static double dWidth = 0.435;
		static double dLength = 5480.0;
		static double dXcoord = 0.0;
		static double dYcoord = 0.0;
		static double dDiameter= 490.0;
		static double dNb_wvgd = 1.0;
		static double dSpacing = 20.0;
		static double dBracket_inc = 0.0;
		static double dCopies = 1.0;
		static double dRadius = 70.0;
		static double dTaper_offset = 0.0;
		
		int i;
		LRenderingAttribute	ra;
		LStipple 	fill;
		unsigned int n,m,Br_change, Nb_wvgd,Copies,Nb_loop,Go,Termination_case;
		LLayerParamEx830 LayerParameters;
		char STR_out[256]="Waveguide lengths (um)\n";
		char Test[256];
		char Out1[256];
		char Out2[256];
		
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
				
		LDialogItem Dialog_Items[12];
		strcpy(Dialog_Items[0].prompt, "Layer:");
		strcpy(Dialog_Items[1].prompt, "Width:");
		strcpy(Dialog_Items[2].prompt, "Length:");
		strcpy(Dialog_Items[3].prompt, "X coordinate (left):");
		strcpy(Dialog_Items[4].prompt, "Y coordinate (bottom):");
		strcpy(Dialog_Items[5].prompt, "Spiral width:");
		strcpy(Dialog_Items[6].prompt, "Number of waveguides:");
		strcpy(Dialog_Items[7].prompt, "Spacing:");
		strcpy(Dialog_Items[8].prompt, "Bracketing increments:");
		strcpy(Dialog_Items[9].prompt, "Nb of identical waveguides:");
		strcpy(Dialog_Items[10].prompt, "Radius:");
		strcpy(Dialog_Items[11].prompt, "Taper Offset:");
				
		strcpy(Dialog_Items[0].value, szLayer);
		sprintf(Dialog_Items[1].value, "%lf", dWidth);
		sprintf(Dialog_Items[2].value, "%lf", dLength);
		sprintf(Dialog_Items[3].value, "%lf", dXcoord);
		sprintf(Dialog_Items[4].value, "%lf", dYcoord);
		sprintf(Dialog_Items[5].value, "%lf", dDiameter);
		sprintf(Dialog_Items[6].value, "%lf", dNb_wvgd);
		sprintf(Dialog_Items[7].value, "%lf", dSpacing);
		sprintf(Dialog_Items[8].value, "%lf", dBracket_inc);
		sprintf(Dialog_Items[9].value, "%lf", dCopies);
		sprintf(Dialog_Items[10].value, "%lf", dRadius);
		sprintf(Dialog_Items[11].value, "%lf", dTaper_offset);
		
		//LDialog_AlertBox( szLayer1 );
		//LDialog_AlertBox( szFile );
		
		if(!LDialog_MultiLineInputBox("Loop Data", Dialog_Items, 12))
		{
			return;
		}
		
		// Do some error checking.
		LLayer pLayer = CheckLayer(pTDBFile, Dialog_Items[0].value, "Waveguide");
		if(NotAssigned(pLayer))
		{
			LLayer Start_Layer  = LLayer_GetCurrent ( pTDBFile );
			LLayer_New (pTDBFile, Start_Layer, "Waveguide");
			pLayer = LLayer_Find(pTDBFile,"Waveguide");
			for(n=raiFirstRenderingAttribute; n<=raiLastRenderingAttribute; n++){
			LLayer_GetRenderingAttribute(pLayer, n, &ra);
			ra.mFillColorIndex = 0; // Color
			ra.mOutlineColorIndex = 0;// Outline color
			for (m=0;m<=7;m++) {fill[m]=ra.mFillPattern[m];} // Pattern   
			LLayer_SetRenderingAttribute(pLayer, n, &ra);}
			LLayer_GetParametersEx830(pLayer, &LayerParameters);
			LayerParameters.GDSNumber =0;
			LLayer_SetParametersEx830(pLayer, &LayerParameters);
		}
		strcpy(szLayer, Dialog_Items[0].value);
		
		LLen Width = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[1].value));
		LLen Length = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[2].value));				
		LCoord X = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[3].value));
		LCoord Y = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[4].value));
		LLen Diameter = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[5].value));
		Nb_wvgd = atoi(Dialog_Items[6].value);
		LLen Spacing = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[7].value));
		LLen Bracket_inc = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[8].value));
		Copies = atoi(Dialog_Items[9].value);
		LLen Radius = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[10].value));
		LLen Taper_offset = LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[11].value));
		LLen Tolerance = LFile_DispUtoIntU(pTDBFile, 10.0);
		LLen Draw_length[20];
		LLen Test_length=0;
		LLen Full_loop_length=0;
		LLen Max_connection=0;
		LLen Min_connection=0;
		LLen Min_connection_test=0;
		LLen Closing_length=0;
		/*LLen Connect1=0;
		LLen Connect2=0;
		LLen Connect3=0;
		LLen Connect4=0;*/
		LLen In_length[4], Out_length[4];
		LCoord In_coord[4][4], Out_coord[4][4];
		for (n=0;n<Nb_wvgd;n++) {Draw_length[n]=0;} //Initialise Draw_length variable
		LLen Length_curve=Radius*Pi/2;
		
		LTorusParams Upper_left_curve;
		Upper_left_curve.dStartAngle = 90;
		Upper_left_curve.dStopAngle = 180;
		LTorusParams Upper_right_curve;
		Upper_right_curve.dStartAngle = 0;
		Upper_right_curve.dStopAngle = 90;
		LTorusParams Lower_left_curve;
		Lower_left_curve.dStartAngle = 180;
		Lower_left_curve.dStopAngle = 270;
		LTorusParams Lower_right_curve;
		Lower_right_curve.dStartAngle = 270;
		Lower_right_curve.dStopAngle = 0;
		
		double Loop_spacing=Nb_wvgd*Spacing;
		//Find the number of full turns
		Go=0;
		Termination_case=0;
		while(Go<1)
		{
			i=0;
			n=0;
			Test_length=0;
			while (i<(Diameter-4*Radius)/(4*(Spacing*Nb_wvgd)))
			{
				if (i==0)
				{
					In_length[0]=Diameter-Radius-Width/2-Taper_offset;
					In_coord[0][0]=X+Width/2;
					In_coord[0][1]=Y+Taper_offset;
					In_coord[0][2]=In_coord[0][0];
					In_coord[0][3]=Y+In_length[0];
					Out_length[0]=(Diameter-(Radius+(Spacing*(Nb_wvgd-1))+Width/2))-Taper_offset;
					Out_coord[0][0]=X+Diameter-(Spacing*(Nb_wvgd-1))-Width/2;
					Out_coord[0][1]=(Y+Radius+(Spacing*(Nb_wvgd-1))+Width/2);
					Out_coord[0][2]=Out_coord[0][0];
					Out_coord[0][3]=Y+Diameter-Taper_offset;
				}
				else
				{
					In_length[0]=((Diameter-(Loop_spacing*2*i)-Radius-Width/2)-((Loop_spacing*(2*i-1))+Radius+Width/2));
					In_coord[0][0]=X+Width/2+(Loop_spacing*2*i);
					In_coord[0][1]=(Y+(Loop_spacing*(2*i-1))+Radius+Width/2);
					In_coord[0][2]=In_coord[0][0];
					In_coord[0][3]=(Y+Diameter-(Loop_spacing*2*i)-Radius-Width/2);
					Out_length[0]=((Y+Diameter-(Spacing*(Nb_wvgd-1))-(Loop_spacing*(2*i-1))-Radius-Width/2)-(Y+Radius+(Spacing*(Nb_wvgd-1))+(Loop_spacing*2*i)+Width/2));
					Out_coord[0][0]=X+Diameter-Width/2-(Loop_spacing*2*i)-(Spacing*(Nb_wvgd-1));
					Out_coord[0][1]=(Y+Radius+(Spacing*(Nb_wvgd-1))+(Loop_spacing*2*i)+Width/2);
					Out_coord[0][2]=Out_coord[0][0];
					Out_coord[0][3]=(Y+Diameter-(Spacing*(Nb_wvgd-1))-(Loop_spacing*(2*i-1))-Radius-Width/2);
				}
				Min_connection=(4*Length_curve)+(Out_coord[0][3]-In_coord[0][1])+(Out_coord[0][2]-In_coord[0][0])-4*Radius;
				Max_connection=In_length[0]+Out_length[0]+(4*Length_curve)+(In_coord[0][3]-Out_coord[0][1])+(Out_coord[0][0]-In_coord[0][2])-4*Radius;
				//sprintf(Test, "Connections %lf %lf %lf %d", LFile_IntUtoMicrons(pTDBFile, Test_length),LFile_IntUtoMicrons(pTDBFile, Min_connection),LFile_IntUtoMicrons(pTDBFile, Max_connection),i);
				//LDialog_AlertBox( Test );
				if((Length > (Test_length+Min_connection)) && (Length < (Test_length+Max_connection)))
				{
					Termination_case=1;
					Nb_loop=i;
					break;
				}
				else
				{
					Test_length=Test_length+In_length[0]+Out_length[0]+2*Length_curve;
				}
				In_length[1]=((Diameter-(Loop_spacing*(2*i+1))-Radius-Width/2)-((Loop_spacing*2*i)+Radius+Width/2));
				In_coord[1][0]=(X+(Loop_spacing*2*i)+Radius+Width/2);
				In_coord[1][1]=Y+Diameter-(Loop_spacing*2*i)-Width/2;
				In_coord[1][2]=(X+Diameter-(Loop_spacing*(2*i+1))-Radius-Width/2);
				In_coord[1][3]=In_coord[1][1];
				Out_length[1]=((Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2)-((Spacing*(Nb_wvgd-1))+(Loop_spacing*(2*i+1))+Radius+Width/2));
				Out_coord[1][0]=(X+(Spacing*(Nb_wvgd-1))+(Loop_spacing*(2*i+1))+Radius+Width/2);
				Out_coord[1][1]=Y+(Spacing*(Nb_wvgd-1))+(Loop_spacing*2*i)+Width/2;
				Out_coord[1][2]=(X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2);
				Out_coord[1][3]=Out_coord[1][1];
				Min_connection=(4*Length_curve)+(In_coord[1][1]-Out_coord[1][3])+(Out_coord[1][2]-In_coord[1][0])-4*Radius;
				Max_connection=In_length[1]+Out_length[1]+(4*Length_curve)+(In_coord[1][3]-Out_coord[1][1])+(In_coord[1][2]-Out_coord[1][0])-4*Radius;
				//sprintf(Test, "Connections %lf %lf %lf %d", LFile_IntUtoMicrons(pTDBFile, Test_length),LFile_IntUtoMicrons(pTDBFile, Min_connection),LFile_IntUtoMicrons(pTDBFile, Max_connection),i);
				//LDialog_AlertBox( Test );
				if((Length > (Test_length+Min_connection)) && (Length < (Test_length+Max_connection)))
				{
					Termination_case=2;
					Nb_loop=i;
					break;
				}
				else
				{
					Test_length=Test_length+In_length[1]+Out_length[1]+2*Length_curve;
				}
				In_length[2]=((Diameter-(Loop_spacing*(2*i))-Radius-Width/2 )-((Loop_spacing*(2*i+1))+Radius+Width/2));
				In_coord[2][0]=X+Diameter-(Loop_spacing*(2*i+1))-Width/2;
				In_coord[2][1]=(Y+(Loop_spacing*(2*i+1))+Radius+Width/2);
				In_coord[2][2]=In_coord[2][0];
				In_coord[2][3]=(Y+Diameter-(Loop_spacing*(2*i))-Radius-Width/2 );
				Out_length[2]=((Diameter-(Spacing*(Nb_wvgd-1))-(Loop_spacing*(2*i+1))-Radius-Width/2)-((Spacing*(Nb_wvgd-1))+(Loop_spacing*(2*i))+Radius+Width/2));
				Out_coord[2][0]=X+(Spacing*(Nb_wvgd-1))+(Loop_spacing*(2*i+1))+Width/2;
				Out_coord[2][1]=(Y+(Spacing*(Nb_wvgd-1))+(Loop_spacing*(2*i))+Radius+Width/2);
				Out_coord[2][2]=Out_coord[2][0];
				Out_coord[2][3]=(Y+Diameter-(Spacing*(Nb_wvgd-1))-(Loop_spacing*(2*i+1))-Radius-Width/2);
				Min_connection=(4*Length_curve)+(In_coord[2][3]-Out_coord[2][1])+(In_coord[2][2]-Out_coord[2][0])-4*Radius;
				Max_connection=In_length[2]+Out_length[2]+(4*Length_curve)+(Out_coord[2][3]-In_coord[2][1])+(In_coord[2][0]-Out_coord[2][2])-4*Radius;
				//sprintf(Test, "Connections %lf %lf %lf %d", LFile_IntUtoMicrons(pTDBFile, Test_length),LFile_IntUtoMicrons(pTDBFile, Min_connection),LFile_IntUtoMicrons(pTDBFile, Max_connection),i);
				//LDialog_AlertBox( Test );
				if((Length > (Test_length+Min_connection)) && (Length < (Test_length+Max_connection)))
				{
					Termination_case=3;
					Nb_loop=i;
					break;
				}
				else
				{
					Test_length=Test_length+In_length[2]+Out_length[2]+2*Length_curve;
				}
				In_length[3]=((Diameter-(Loop_spacing*(2*i+1))-Radius-Width/2)-((Loop_spacing*(2*i+2))+Radius+Width/2));
				In_coord[3][0]=(X+(Loop_spacing*(2*i+2))+Radius+Width/2);
				In_coord[3][1]=Y+(Loop_spacing*(2*i+1))+Width/2;
				In_coord[3][2]=(X+Diameter-(Loop_spacing*(2*i+1))-Radius-Width/2);
				In_coord[3][3]=In_coord[3][1];
				Out_length[3]=((X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+2))-Radius-Width/2)-(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2));
				Out_coord[3][0]=(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2);
				Out_coord[3][1]=Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Width/2;
				Out_coord[3][2]=(X+Diameter-(Spacing*(Nb_wvgd-1))-(Loop_spacing*(2*i+2))-Radius-Width/2);
				Out_coord[3][3]=Out_coord[3][1];
				Min_connection=(4*Length_curve)+(Out_coord[3][1]-In_coord[3][3])+(In_coord[3][2]-Out_coord[3][0])-4*Radius;
				Max_connection=In_length[3]+Out_length[3]+(4*Length_curve)+(Out_coord[3][3]-In_coord[3][1])+(Out_coord[3][2]-In_coord[3][0])-4*Radius;
				//sprintf(Test, "Connections %lf %lf %lf %d", LFile_IntUtoMicrons(pTDBFile, Test_length),LFile_IntUtoMicrons(pTDBFile, Min_connection),LFile_IntUtoMicrons(pTDBFile, Max_connection),i);
				//LDialog_AlertBox( Test );
				if((Length > (Test_length+Min_connection)) && (Length < (Test_length+Max_connection)))
				{
					Termination_case=4;
					Nb_loop=i;
					break;
				}
				else
				{
					Test_length=Test_length+In_length[3]+Out_length[3]+2*Length_curve;
				}
				//sprintf(Test, "Connections %lf %lf %lf %d", LFile_IntUtoMicrons(pTDBFile, Test_length),LFile_IntUtoMicrons(pTDBFile, Min_connection),LFile_IntUtoMicrons(pTDBFile, Max_connection),Nb_loop);
				//LDialog_AlertBox( Test );
				
				if(Test_length > Length) 
				{
					Termination_case=5;
					Nb_loop=0;
					break;
				}
				i++;
				
				//Coord_out(pTDBFile, In_coord);
				//Coord_out(pTDBFile, Out_coord);
				//Test_length_out(pTDBFile, STR_out, In_length);
				//Test_length_out(pTDBFile, STR_out, Out_length);
				//Test_length=Test_length+In_length[0]+In_length[1]+In_length[2]+In_length[3]+Out_length[0]+Out_length[1]+Out_length[2]+Out_length[3]+(8*Length_curve);
			}
			
			if ((Termination_case==5)&&(LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[5].value))-Diameter< LFile_DispUtoIntU(pTDBFile,100)))
			{
				Diameter=Diameter-Tolerance;
				i=0;
			}
			else
			{
				Go=1;
			}
			if (i >= (Diameter-4*Radius)/(4*(Spacing*Nb_wvgd)))
			{
				sprintf(Out2, "Waveguide length too long");
				LDialog_AlertBox( Out2 );
				Go=1;
				Nb_loop=0;
			}
		}
		
		if (Diameter < LFile_DispUtoIntU(pTDBFile, atof(Dialog_Items[5].value)))
		{
			sprintf(Out1, "Spiral width reduced to %lf um", LFile_IntUtoMicrons(pTDBFile, Diameter));
			LDialog_AlertBox( Out1 );
		}
		//Draw the full turns
		i=0;		
		while (i<Nb_loop)
		{
			for (n=0;n<Nb_wvgd;n++)
			{	
				Br_change = (int) n/Copies;
				//Input Side
				Upper_left_curve.ptCenter = LPoint_Set(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
				Upper_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
				Upper_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
				Upper_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
				Upper_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
				Upper_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
				Lower_left_curve.ptCenter = LPoint_Set(X+(Spacing*n)+(Loop_spacing*(2*i+2))+Radius+Width/2,Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2);
				Lower_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
				Lower_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
				Lower_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2,Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2);
				Lower_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
				Lower_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
				if (i==0)
				{
					LBox_New(pCell, pLayer, X+(Spacing*n)-(Bracket_inc/2*Br_change),Y+Taper_offset,(X+(Spacing*n))+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Draw_length[n]=Draw_length[n]+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2-Taper_offset;
				}
				else
				{
					LBox_New(pCell, pLayer, X+(Spacing*n)+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2, (X+(Spacing*n))+(Loop_spacing*2*i)+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2)-(Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2));
				}
				LTorus_CreateNew(pCell, pLayer,&Upper_left_curve);
				LBox_New(pCell, pLayer,X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2, Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change),X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2, Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width);
				Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2)-(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2));
				LTorus_CreateNew(pCell, pLayer,&Upper_right_curve);
				LBox_New(pCell, pLayer, X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-(Bracket_inc/2*Br_change)-Width,Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2 , X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))+(Bracket_inc/2*Br_change),Y+Diameter-(Spacing*n)-(Loop_spacing*(2*i))-Radius-Width/2 );
				Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*n)-(Loop_spacing*(2*i))-Radius-Width/2 )-(Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2));
				LTorus_CreateNew(pCell, pLayer,&Lower_left_curve);
				LBox_New(pCell, pLayer, X+(Spacing*n)+(Loop_spacing*(2*i+2))+Radius+Width/2, Y+(Spacing*n)+(Loop_spacing*(2*i+1))-(Bracket_inc/2*Br_change), X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2, Y+(Spacing*n)+(Loop_spacing*(2*i+1))+(Bracket_inc/2*Br_change)+Width);
				Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2)-(X+(Spacing*n)+(Loop_spacing*(2*i+2))+Radius+Width/2));
				LTorus_CreateNew(pCell, pLayer,&Lower_right_curve);
				Draw_length[n]=Draw_length[n]+(4*Length_curve);
				//Output Side
				Upper_left_curve.ptCenter = LPoint_Set(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2,Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2);
				Upper_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+2))-Radius-Width/2,Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2);
				Lower_left_curve.ptCenter = LPoint_Set(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
				Lower_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
				if (i==0)
				{
					LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))+(Bracket_inc/2*Br_change), Y+Diameter-Taper_offset);
					Draw_length[n]=Draw_length[n]+((Y+Diameter)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2))-Taper_offset;
				}
				else
				{
					LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2);
					Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2));
				} 
				LTorus_CreateNew(pCell, pLayer,&Upper_left_curve);
				LBox_New(pCell, pLayer,X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2, Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2, Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+(Bracket_inc/2*Br_change)+Width);
				Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2)-(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2));
				LTorus_CreateNew(pCell, pLayer,&Upper_right_curve);
				LBox_New(pCell, pLayer, X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))-(Bracket_inc/2*Br_change),Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2 , X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Width+(Bracket_inc/2*Br_change),Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2 );
				Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2)-(Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2));
				LTorus_CreateNew(pCell, pLayer,&Lower_left_curve);
				LBox_New(pCell, pLayer, X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2, Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-(Bracket_inc/2*Br_change)-Width, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+2))-Radius-Width/2, Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))+(Bracket_inc/2*Br_change));
				Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+2))-Radius-Width/2)-(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2));
				LTorus_CreateNew(pCell, pLayer,&Lower_right_curve);
				Draw_length[n]=Draw_length[n]+(4*Length_curve);
			}
			i++;	
		}
		
		LTorusParams Curve_1;
		LTorusParams Curve_2;
		LTorusParams Curve_3;
		LTorusParams Curve_4;
		Closing_length=Length-(Test_length+Min_connection);
		//sprintf(Test, "Connections %lf %lf %lf %d", LFile_IntUtoMicrons(pTDBFile, Test_length),LFile_IntUtoMicrons(pTDBFile, Min_connection),LFile_IntUtoMicrons(pTDBFile, Closing_length),i);
		//LDialog_AlertBox( Test );
		//Draw_length_out(pTDBFile, STR_out, Draw_length, Nb_wvgd, Termination_case);
		switch(Termination_case)
		{
			case 1:
				Curve_1.dStartAngle = 90;
				Curve_1.dStopAngle = 180;
				Curve_2.dStartAngle = 0;
				Curve_2.dStopAngle = 90;
				Curve_3.dStartAngle = 180;
				Curve_3.dStopAngle = 270;
				Curve_4.dStartAngle = 270;
				Curve_4.dStopAngle = 0;
				for (n=0;n<Nb_wvgd;n++)
				{
					Br_change = (int) n/Copies;
					Curve_1.ptCenter = LPoint_Set(In_coord[0][0]+(Spacing*n)+Radius,In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)));
					Curve_1.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_1.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_2.ptCenter = LPoint_Set(In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2,In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)));
					Curve_2.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_2.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_3.ptCenter = LPoint_Set(In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+3*Radius+Width/2+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2,In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)));
					Curve_3.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_3.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_4.ptCenter = LPoint_Set(Out_coord[0][0]-Radius+(Spacing*n),In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)));
					Curve_4.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_4.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					LBox_New(pCell, pLayer,In_coord[0][0]+(Spacing*n)-(Bracket_inc/2*Br_change)-Width/2, In_coord[0][1]+(Spacing*n),In_coord[0][0]+(Spacing*n)+(Bracket_inc/2*Br_change)+Width/2, In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)));
					Draw_length[n]=Draw_length[n]+((In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)))-(In_coord[0][1]+(Spacing*n)));
					LTorus_CreateNew(pCell, pLayer,&Curve_1);
					LBox_New(pCell, pLayer,In_coord[0][0]+(Spacing*n)+Radius, In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))-Width/2-(Bracket_inc/2*Br_change)+Radius, In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2, In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))+(Bracket_inc/2*Br_change)+Radius+Width/2);
					Draw_length[n]=Draw_length[n]+((In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2)-(In_coord[0][0]+(Spacing*n)+Radius));
					LTorus_CreateNew(pCell, pLayer,&Curve_2);
					LBox_New(pCell, pLayer,In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+2*Radius+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2-(Bracket_inc/2*Br_change), In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+2*Radius+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2+(Bracket_inc/2*Br_change)+Width, In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)));
					Draw_length[n]=Draw_length[n]+((In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)))-(In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))));
					LTorus_CreateNew(pCell, pLayer,&Curve_3);
					LBox_New(pCell, pLayer, In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+3*Radius+Width/2+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2, In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))+(Bracket_inc/2*Br_change)+Width/2-Radius, Out_coord[0][2]-Radius+(Spacing*n), In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))-(Bracket_inc/2*Br_change)-Width/2-Radius);
					Draw_length[n]=Draw_length[n]+((Out_coord[0][2]-Radius+(Spacing*n))-(In_coord[0][0]+(Spacing*(Nb_wvgd/2-n+1))+3*Radius+Width/2+(Out_coord[0][2]-In_coord[0][0]-4*Radius)/2)) ;
					LTorus_CreateNew(pCell, pLayer,&Curve_4);
					LBox_New(pCell, pLayer, Out_coord[0][0]+(Spacing*n)-Width/2-(Bracket_inc/2*Br_change), In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), Out_coord[0][0]+(Spacing*n)+Width/2+(Bracket_inc/2*Br_change), Out_coord[0][3]+(Spacing*n));
					Draw_length[n]=Draw_length[n]+((Out_coord[0][3]+(Spacing*n))-(In_coord[0][1]+(Out_coord[0][3]-In_coord[0][1])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))))+4*Length_curve;
				}
				break;
			case 2:
				for (n=0;n<Nb_wvgd;n++)
				{	
					Br_change = (int) n/Copies;
					//Input Side
					Upper_left_curve.ptCenter = LPoint_Set(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Upper_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Upper_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Lower_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Lower_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					LBox_New(pCell, pLayer, X+(Spacing*n)+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2, (X+(Spacing*n))+(Loop_spacing*2*i)+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					if (i==0)
					{
						LBox_New(pCell, pLayer, X+(Spacing*n)-(Bracket_inc/2*Br_change),Y,(X+(Spacing*n))+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2+Length_curve;
					}
					else
					{
						LBox_New(pCell, pLayer, X+(Spacing*n)+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2, (X+(Spacing*n))+(Loop_spacing*2*i)+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2)-(Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2))+Length_curve;
					}
					LTorus_CreateNew(pCell, pLayer,&Upper_left_curve);
					//Output Side
					Lower_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
					if (i==0)
					{
						LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))+(Bracket_inc/2*Br_change), Y+Diameter);
						Draw_length[n]=Draw_length[n]+((Y+Diameter)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2))+Length_curve;
					}
					else
					{
						LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2))+Length_curve;
					}
					LTorus_CreateNew(pCell, pLayer,&Lower_right_curve);
				}
				Curve_1.dStartAngle = 0;
				Curve_1.dStopAngle = 90;
				Curve_2.dStartAngle = 270;
				Curve_2.dStopAngle = 0;
				Curve_3.dStartAngle = 90;
				Curve_3.dStopAngle = 180;
				Curve_4.dStartAngle = 180;
				Curve_4.dStopAngle = 270;
				for (n=0;n<Nb_wvgd;n++)
				{
					Br_change = (int) n/Copies;
					Curve_1.ptCenter = LPoint_Set(In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[1][1]-(Spacing*n)-Radius);
					Curve_1.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_1.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_2.ptCenter = LPoint_Set(In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2);
					Curve_2.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_2.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_3.ptCenter = LPoint_Set(In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-3*Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2);
					Curve_3.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_3.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_4.ptCenter = LPoint_Set(In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), Out_coord[1][1]+Radius-(Spacing*n));
					Curve_4.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_4.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					LBox_New(pCell, pLayer,In_coord[1][0]+(Spacing*n), In_coord[1][1]-(Spacing*n)+(Bracket_inc/2*Br_change)+Width/2,In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[1][1]-(Spacing*n)-(Bracket_inc/2*Br_change)-Width/2);
					Draw_length[n]=Draw_length[n]+((In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)))-(In_coord[1][0]+(Spacing*n)));
					LTorus_CreateNew(pCell, pLayer,&Curve_1);
					LBox_New(pCell, pLayer,In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))+Radius-Width/2-(Bracket_inc/2*Br_change),In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2 ,In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Bracket_inc/2*Br_change), In_coord[1][1]-(Spacing*n)-Radius);
					Draw_length[n]=Draw_length[n]+((In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4-(Spacing*n)+Radius+Width/2+(Bracket_inc/2*Br_change), In_coord[1][1]-(Spacing*n)-Radius)-(In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2));
					LTorus_CreateNew(pCell, pLayer,&Curve_2);
					LBox_New(pCell, pLayer,In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-2*Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2-(Bracket_inc/2*Br_change)-Width/2, In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-2*Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2+(Bracket_inc/2*Br_change)+Width/2);
					Draw_length[n]=Draw_length[n]+((In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2+Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)))-(In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))));
					LTorus_CreateNew(pCell, pLayer,&Curve_3);
					LBox_New(pCell, pLayer, In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))-Radius-(Bracket_inc/2*Br_change)-Width/2, Out_coord[1][1]+Radius-(Spacing*n), In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))-Radius+(Bracket_inc/2*Br_change)+Width/2, In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-3*Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2 );
					Draw_length[n]=Draw_length[n]+((In_coord[1][1]-(Spacing*(Nb_wvgd/2-n+1))-3*Radius-Width/2-(In_coord[1][1]-Out_coord[1][3]-4*Radius)/2)-(Out_coord[1][1]+Radius-(Spacing*n))) ;
					LTorus_CreateNew(pCell, pLayer,&Curve_4);
					LBox_New(pCell, pLayer, In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1)), Out_coord[1][1]-(Spacing*n)-Width/2-(Bracket_inc/2*Br_change), Out_coord[1][2]+(Spacing*n), Out_coord[1][1]-(Spacing*n)+Width/2+(Bracket_inc/2*Br_change));
					Draw_length[n]=Draw_length[n]+((Out_coord[1][2]+(Spacing*n))-(In_coord[1][0]+(Out_coord[1][2]-In_coord[1][0])/2-Closing_length/4+(Spacing*(Nb_wvgd/2-n+1))))+4*Length_curve;
				}
				break;
			case 3:
				for (n=0;n<Nb_wvgd;n++)
				{	
					Br_change = (int) n/Copies;
					//Input Side
					Upper_left_curve.ptCenter = LPoint_Set(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Upper_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Upper_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Upper_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Upper_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Upper_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Lower_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Lower_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Lower_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Lower_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					if (i==0)
					{
						LBox_New(pCell, pLayer, X+(Spacing*n)-(Bracket_inc/2*Br_change),Y,(X+(Spacing*n))+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2;
					}
					else
					{
						LBox_New(pCell, pLayer, X+(Spacing*n)+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2, (X+(Spacing*n))+(Loop_spacing*2*i)+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2)-(Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2));
					}
					LTorus_CreateNew(pCell, pLayer,&Upper_left_curve);
					LBox_New(pCell, pLayer,X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2, Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change),X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2, Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width);
					Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2)-(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2))+2*Length_curve;
					LTorus_CreateNew(pCell, pLayer,&Upper_right_curve);
					//Output Side
					Lower_left_curve.ptCenter = LPoint_Set(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
					Lower_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
					if (i==0)
					{
						LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))+(Bracket_inc/2*Br_change), Y+Diameter);
						Draw_length[n]=Draw_length[n]+((Y+Diameter)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2));
					}
					else
					{
						LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2));
					} 
					LTorus_CreateNew(pCell, pLayer,&Lower_left_curve);
					LBox_New(pCell, pLayer,X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2, Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2, Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+(Bracket_inc/2*Br_change)+Width);
					Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2)-(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2))+2*Length_curve;
					LTorus_CreateNew(pCell, pLayer,&Lower_right_curve);
				}
				Curve_1.dStartAngle = 270;
				Curve_1.dStopAngle = 0;
				Curve_2.dStartAngle = 180;
				Curve_2.dStopAngle = 270;
				Curve_3.dStartAngle = 0;
				Curve_3.dStopAngle = 90;
				Curve_4.dStartAngle = 90;
				Curve_4.dStopAngle = 180;
				
				for (n=0;n<Nb_wvgd;n++)
				{
					Br_change = (int) n/Copies;
					Curve_1.ptCenter = LPoint_Set(In_coord[2][2]-(Spacing*n)-Radius,In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)));
					Curve_1.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_1.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_2.ptCenter = LPoint_Set(In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2,In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)));
					Curve_2.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_2.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_3.ptCenter = LPoint_Set(In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-3*Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2,In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)));
					Curve_3.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_3.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_4.ptCenter = LPoint_Set(Out_coord[2][0]+Radius-(Spacing*n),In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)));
					Curve_4.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_4.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					LBox_New(pCell, pLayer, In_coord[2][2]-(Spacing*n)-(Bracket_inc/2*Br_change)-Width/2, In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)),In_coord[2][2]-(Spacing*n)+(Bracket_inc/2*Br_change)+Width/2, In_coord[2][3]-(Spacing*n));
					Draw_length[n]=Draw_length[n]+((In_coord[2][3]-(Spacing*n))-(In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))));
					LTorus_CreateNew(pCell, pLayer,&Curve_1);
					LBox_New(pCell, pLayer, In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2, In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))-Width/2-(Bracket_inc/2*Br_change)-Radius, In_coord[2][2]-(Spacing*n)-Radius, In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))+(Bracket_inc/2*Br_change)-Radius+Width/2);
					Draw_length[n]=Draw_length[n]+((In_coord[2][2]-(Spacing*n)-Radius)-(In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2));
					LTorus_CreateNew(pCell, pLayer,&Curve_2);
					LBox_New(pCell, pLayer,In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-2*Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2-(Bracket_inc/2*Br_change)-Width/2, In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-2*Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2+(Bracket_inc/2*Br_change)+Width/2, In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)));
					Draw_length[n]=Draw_length[n]+((In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)))-(In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))));
					LTorus_CreateNew(pCell, pLayer,&Curve_3);
					LBox_New(pCell, pLayer, Out_coord[2][0]+Radius-(Spacing*n), In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))-(Bracket_inc/2*Br_change)-Width/2+Radius, In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-3*Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2, In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))+(Bracket_inc/2*Br_change)+Width/2+Radius);
					Draw_length[n]=Draw_length[n]+((In_coord[2][2]-(Spacing*(Nb_wvgd/2-n+1))-3*Radius-Width/2-(In_coord[2][2]-Out_coord[2][0]-4*Radius)/2)-(Out_coord[2][0]+Radius-(Spacing*n))) ;
					LTorus_CreateNew(pCell, pLayer,&Curve_4);
					LBox_New(pCell, pLayer, Out_coord[2][0]-(Spacing*n)-Width/2-(Bracket_inc/2*Br_change), Out_coord[2][1]-(Spacing*n), Out_coord[2][0]-(Spacing*n)+Width/2+(Bracket_inc/2*Br_change),In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)) );
					Draw_length[n]=Draw_length[n]+((In_coord[2][3]-(In_coord[2][3]-Out_coord[2][1])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)))-(Out_coord[2][1]-(Spacing*n)))+4*Length_curve;
				}
				break;
			case 4:
				for (n=0;n<Nb_wvgd;n++)
				{	
					Br_change = (int) n/Copies;
					//Input Side
					Upper_left_curve.ptCenter = LPoint_Set(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Upper_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Upper_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Upper_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2,Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
					Upper_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Upper_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Lower_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2,Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2);
					Lower_left_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Lower_left_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Lower_right_curve.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Lower_right_curve.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					if (i==0)
					{
						LBox_New(pCell, pLayer, X+(Spacing*n)-(Bracket_inc/2*Br_change),Y,(X+(Spacing*n))+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2;
					}
					else
					{
						LBox_New(pCell, pLayer, X+(Spacing*n)+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2, (X+(Spacing*n))+(Loop_spacing*2*i)+Width+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-Radius-Width/2)-(Y+(Spacing*n)+(Loop_spacing*(2*i-1))+Radius+Width/2));
					}
					LTorus_CreateNew(pCell, pLayer,&Upper_left_curve);
					LBox_New(pCell, pLayer,X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2, Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change),X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2, Y+Diameter-(Spacing*n)-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width);
					Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-Radius-Width/2)-(X+(Spacing*n)+(Loop_spacing*2*i)+Radius+Width/2));
					LTorus_CreateNew(pCell, pLayer,&Upper_right_curve);
					LBox_New(pCell, pLayer, X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))-(Bracket_inc/2*Br_change)-Width,Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2 , X+Diameter-(Spacing*n)-(Loop_spacing*(2*i+1))+(Bracket_inc/2*Br_change),Y+Diameter-(Spacing*n)-(Loop_spacing*(2*i))-Radius-Width/2 );
					Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*n)-(Loop_spacing*(2*i))-Radius-Width/2 )-(Y+(Spacing*n)+(Loop_spacing*(2*i+1))+Radius+Width/2))+3*Length_curve;
					LTorus_CreateNew(pCell, pLayer,&Lower_right_curve);
					//Output Side
					Upper_left_curve.ptCenter = LPoint_Set(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2,Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2);
					Lower_left_curve.ptCenter = LPoint_Set(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
					Lower_right_curve.ptCenter = LPoint_Set(X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2,Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2);
					if (i==0)
					{
						LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))+(Bracket_inc/2*Br_change), Y+Diameter);
						Draw_length[n]=Draw_length[n]+((Y+Diameter)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+Width/2));
					}
					else
					{
						LBox_New(pCell, pLayer, X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*2*i)-(Bracket_inc/2*Br_change)-Width,Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2,(X+Diameter-(Spacing*(Nb_wvgd-n-1)))-(Loop_spacing*2*i)+(Bracket_inc/2*Br_change), Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2);
						Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i-1))-Radius-Width/2)-(Y+Radius+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+Width/2));
					} 
					LBox_New(pCell, pLayer,X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2, Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)-(Bracket_inc/2*Br_change),X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2, Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*2*i)+(Bracket_inc/2*Br_change)+Width);
					Draw_length[n]=Draw_length[n]+((X+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i))-Radius-Width/2)-(X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Radius+Width/2));
					LTorus_CreateNew(pCell, pLayer,&Upper_left_curve);
					LBox_New(pCell, pLayer, X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))-(Bracket_inc/2*Br_change),Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2 , X+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i+1))+Width+(Bracket_inc/2*Br_change),Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2 );
					Draw_length[n]=Draw_length[n]+((Y+Diameter-(Spacing*(Nb_wvgd-n-1))-(Loop_spacing*(2*i+1))-Radius-Width/2)-(Y+(Spacing*(Nb_wvgd-n-1))+(Loop_spacing*(2*i))+Radius+Width/2))+3*Length_curve;
					LTorus_CreateNew(pCell, pLayer,&Lower_left_curve);
					LTorus_CreateNew(pCell, pLayer,&Lower_right_curve);
				}
				Curve_1.dStartAngle = 180;
				Curve_1.dStopAngle = 270;
				Curve_2.dStartAngle = 90;
				Curve_2.dStopAngle = 180;
				Curve_3.dStartAngle = 270;
				Curve_3.dStopAngle = 0;
				Curve_4.dStartAngle = 0;
				Curve_4.dStopAngle = 90;
				
				for (n=0;n<Nb_wvgd;n++)
				{
					Br_change = (int) n/Copies;
					Curve_1.ptCenter = LPoint_Set(In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[3][3]+(Spacing*n)+Radius);
					Curve_1.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_1.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_2.ptCenter = LPoint_Set(In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2);
					Curve_2.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_2.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_3.ptCenter = LPoint_Set(In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+3*Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2);
					Curve_3.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_3.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					Curve_4.ptCenter = LPoint_Set(In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), Out_coord[3][1]-Radius+(Spacing*n));
					Curve_4.nInnerRadius = Radius-Width/2-(Bracket_inc/2*Br_change);
					Curve_4.nOuterRadius = Radius+Width/2+(Bracket_inc/2*Br_change);
					LBox_New(pCell, pLayer,In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[3][3]+(Spacing*n)-(Bracket_inc/2*Br_change)-Width/2,In_coord[3][2]-(Spacing*n), In_coord[3][3]+(Spacing*n)+(Bracket_inc/2*Br_change)+Width/2);
					Draw_length[n]=Draw_length[n]+((In_coord[3][2]-(Spacing*n))-(In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))));
					LTorus_CreateNew(pCell, pLayer,&Curve_1);
					LBox_New(pCell, pLayer,In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))-Radius-Width/2-(Bracket_inc/2*Br_change), In_coord[3][3]+Radius+(Spacing*n),In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))-Radius+Width/2+(Bracket_inc/2*Br_change), In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2);
					Draw_length[n]=Draw_length[n]+((In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2)-(In_coord[3][3]+Radius+(Spacing*n)));
					LTorus_CreateNew(pCell, pLayer,&Curve_2);
					LBox_New(pCell, pLayer, In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+2*Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2-(Bracket_inc/2*Br_change)-Width/2, In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+2*Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2+(Bracket_inc/2*Br_change)+Width/2);
					Draw_length[n]=Draw_length[n]+((In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)))-(In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2-Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))));
					LTorus_CreateNew(pCell, pLayer,&Curve_3);
					LBox_New(pCell, pLayer, In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))+Radius-(Bracket_inc/2*Br_change)-Width/2, In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+3*Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2, In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1))+Radius+(Bracket_inc/2*Br_change)+Width/2, Out_coord[3][1]-Radius+(Spacing*n));
					Draw_length[n]=Draw_length[n]+((Out_coord[3][1]-Radius+(Spacing*n))-(In_coord[3][3]+(Spacing*(Nb_wvgd/2-n+1))+3*Radius+Width/2+(Out_coord[3][1]-In_coord[3][3]-4*Radius)/2)) ;
					LTorus_CreateNew(pCell, pLayer,&Curve_4);
					LBox_New(pCell, pLayer, Out_coord[3][0]-(Spacing*n), Out_coord[3][1]+(Spacing*n)-Width/2-(Bracket_inc/2*Br_change), In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)), Out_coord[3][1]+(Spacing*n)+Width/2+(Bracket_inc/2*Br_change));
					Draw_length[n]=Draw_length[n]+((In_coord[3][2]-(In_coord[3][2]-Out_coord[3][0])/2+Closing_length/4-(Spacing*(Nb_wvgd/2-n+1)))-(Out_coord[3][0]-(Spacing*n)))+4*Length_curve;
				}
				break;
			case 5:
				sprintf(Out1, "Impossible to get the appropriate length");
				LDialog_AlertBox( Out1 );
				break;
		}
		
		
		Draw_length_out(pTDBFile, STR_out, Draw_length, Nb_wvgd, Termination_case);
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
	
	void Draw_length_out(LFile pTDBFile, char STR_out[256], LLen Draw_length[20], int Nb_wvgd, unsigned int Termination)
	{
		char STR_display[256], STR1_out[256];
		int n;
		sprintf(STR_display, STR_out);
		for (n=0;n<Nb_wvgd;n++)
		{
			sprintf(STR1_out,"Waveguide %d length: %lf \n",n,LFile_IntUtoMicrons(pTDBFile, Draw_length[n])); 
			strcat(STR_display,STR1_out);
		}
		sprintf(STR1_out,"Termination: %d \n", Termination);
		strcat(STR_display,STR1_out);
		LDialog_AlertBox( STR_display );
	}
	
	void Test_length_out(LFile pTDBFile, char STR_out[256], LLen Test_length[4])
	{
		char STR_display[256], STR1_out[256];
		sprintf(STR_display, STR_out);
		sprintf(STR1_out,"Section length: %lf %lf %lf %lf \n",LFile_IntUtoMicrons(pTDBFile, Test_length[0]),LFile_IntUtoMicrons(pTDBFile, Test_length[1]),LFile_IntUtoMicrons(pTDBFile, Test_length[2]),LFile_IntUtoMicrons(pTDBFile, Test_length[3])); 
		strcat(STR_display,STR1_out);
		LDialog_AlertBox( STR_display );
	}
	
	void Coord_out(LFile pTDBFile, LCoord Display_coord[4][4])
	{
		char STR_display[512], STR1_out[256];
		int n;
		for (n=0;n<4;n++)
		{
			sprintf(STR1_out,"Waveguide coordinates: %lf %lf %lf %lf \n",LFile_IntUtoMicrons(pTDBFile, Display_coord[n][0]),LFile_IntUtoMicrons(pTDBFile,Display_coord[n][1]),LFile_IntUtoMicrons(pTDBFile,Display_coord[n][2]),LFile_IntUtoMicrons(pTDBFile,Display_coord[n][3])); 
			strcat(STR_display,STR1_out);
		}
		LDialog_AlertBox( STR_display );
		sprintf(STR_display," ");
	}
	
	void Loop_register(void)
	{
		LMacro_BindToMenuAndHotKey_v9_30("Tools", "F3" /*hotkey*/, " Precise Loop", "Loop", "Macro" /*hotkey category*/);
	}

}
Loop_register();