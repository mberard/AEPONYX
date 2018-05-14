/*******************************************************************************
 * Macro Name: CSVToPolygon
 * Creator  : Martin Berard AEPONYX
 *
 * Revision History:
 * 15 Dec 2017
 * 07 May 2018 - Quentin Gaillot
 *******************************************************************************/
#include <unistd.h> //getcwd

//#define EXCLUDE_LEDIT_LEGACY_UPI

#include <ldata.h>
/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */
#include <string.h>

#define MAX_POLYGON_SIZE 5000

typedef struct curve{
	int typeCurve; //0:no curve, 1:curved polygone, 2:circle, 3:torus, 4:pie
	LCoord cx,cy;
	double r,r2;
	double startAngle, stopAngle;
	LArcDirection dir;
}t_curve;

void CSVToPolygon(void)
{
	//Variables
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer pLayer;
	LPoint point_arr[MAX_POLYGON_SIZE]; // could use malloc of LPoint (2 x LCoord or 2x Long or 2x 32bits) for undefined array size or base buffer on linecount
	LCoord X,Y,CX,CY;
	double R,R2,START,STOP;
	int TYPE;
	LArcDirection DIR;

	char cwd[MAX_TDBFILE_NAME];
	char sLayerName[MAX_LAYER_NAME];

	char line[256];

	t_curve tcurve_arr[MAX_POLYGON_SIZE];

	LVertex vertex;

	char* token;

   	FILE * myFile;
   	int nPoints = 0;
	int i = 0;
	int cpt = 0;

	LObject polygon = NULL;

	LDialogItem DialogItems[2] = {{ "CSV file","polygon.csv"}, { "Layer","WGUIDE"}};
	
	if (LDialog_MultiLineInputBox("Polygon File",DialogItems,2))
   	{
		pLayer = LLayer_Find(pFile, DialogItems[1].value);
		if(NotAssigned(pLayer)) 
		{
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", DialogItems[1].value));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
   		LDialog_AlertBox(LFormat("The Polygon will be added in Layer %s", sLayerName));

		if (getcwd(cwd, sizeof(cwd)) == NULL)
   		LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
		else
   		LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));

	   	strcat(cwd,DialogItems[0].value);
  		LUpi_LogMessage(LFormat("current csv file is: %s\n", cwd));
  		myFile = fopen(cwd,"r");
   		if (myFile == NULL)
   			LUpi_LogMessage(LFormat("fopen() error: %s\n",strerror(errno)));
		else 
		{
			LUpi_LogMessage(LFormat("opened polygon file is: %s\n", cwd));
			while(!feof(myFile))
			{
				// reads text until newline 
				fscanf(myFile,"%[^\n]", line);
				LUpi_LogMessage(LFormat("READ LINE: %s\n", line));
				cpt=0;
				for(i=0; i<strlen(line);i++)
				{
					if(line[i]==',')
						cpt++;
				}

				token = strtok(line, ",");

				LUpi_LogMessage(LFormat("i: %d\n", cpt));
				if(cpt == 1)
				{
					X = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					Y = LC_Microns(atoi(token));

					point_arr[nPoints] = LPoint_Set(X, Y);
					tcurve_arr[nPoints].typeCurve = 0;
					nPoints = nPoints+1;
				}
				else if(cpt == 9)
				{
					X = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					Y = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					TYPE = atoi(token);
					token = strtok(NULL, ",");
					CX = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					CY = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					R = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					R2 = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					START = atoi(token);
					token = strtok(NULL, ",");
					STOP = atoi(token);
					token = strtok(NULL, ",");
					if(strcmp(token,"CW") == 0)
						DIR = CW;
					else
						DIR = CCW;

					point_arr[nPoints] = LPoint_Set(X, Y);
					tcurve_arr[nPoints].typeCurve = TYPE;
					tcurve_arr[nPoints].cx = CX;
					tcurve_arr[nPoints].cy = CY;
					tcurve_arr[nPoints].r = R;
					tcurve_arr[nPoints].r2 = R2;
					tcurve_arr[nPoints].startAngle = START;
					tcurve_arr[nPoints].stopAngle = STOP;
					tcurve_arr[nPoints].dir = DIR;
					nPoints = nPoints+1;
				}
				fscanf(myFile,"\n");
			}	
			fclose(myFile);
		}
	}

	//LDialog_MsgBox(LFormat("nPoints = %d." , nPoints ));
	LUpi_LogMessage(LFormat("nPoints = %d.\n" , nPoints ));
	polygon=LPolygon_New(pCell, pLayer, point_arr, nPoints);
	
	vertex = LObject_GetVertexList(polygon);
	for(cpt=0; cpt<nPoints; cpt++)
	{
		if(tcurve_arr[cpt].typeCurve == 1)
		{
			LUpi_LogMessage(LFormat("TRAITEMENT\n" ));
			LVertex_AddCurve(polygon, vertex, LPoint_Set(tcurve_arr[cpt].cx, tcurve_arr[cpt].cy), tcurve_arr[cpt].dir);
		}
		else if(tcurve_arr[cpt].typeCurve == 2)
		{
			LUpi_LogMessage(LFormat("CIRCLE\n" ));
			polygon = LCircle_New( pCell, pLayer, LPoint_Set(tcurve_arr[cpt].cx, tcurve_arr[cpt].cy), tcurve_arr[cpt].r );
		}
		else if(tcurve_arr[cpt].typeCurve == 3)
		{
			LUpi_LogMessage(LFormat("TORUS\n" ));
			LTorusParams p;
			p.ptCenter = LPoint_Set(tcurve_arr[cpt].cx, tcurve_arr[cpt].cy);
			p.nInnerRadius = tcurve_arr[cpt].r;
			p.nOuterRadius = tcurve_arr[cpt].r2;
			p.dStartAngle = tcurve_arr[cpt].startAngle;
			p.dStopAngle = tcurve_arr[cpt].stopAngle;

			polygon = LTorus_CreateNew( pCell, pLayer, &p );
		}
		else if(tcurve_arr[cpt].typeCurve == 4)
		{
			LUpi_LogMessage(LFormat("PIE\n" ));
			LPieParams p;
			p.ptCenter = LPoint_Set(tcurve_arr[cpt].cx, tcurve_arr[cpt].cy);
			p.nRadius = tcurve_arr[cpt].r;
			p.dStartAngle = tcurve_arr[cpt].startAngle;
			p.dStopAngle = tcurve_arr[cpt].stopAngle;

			polygon = LPie_CreateNew( pCell, pLayer, &p );
		}
		vertex = LVertex_GetNext(vertex);
	}

	//LCell_MakeVisible ( pCell );
	LDisplay_Refresh();
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F2" /*hotkey*/, 
			"AEPONYX\\CSVToPolygon\nWindow", "CSVToPolygon", NULL /*hotkey category*/);
	return 1;
}