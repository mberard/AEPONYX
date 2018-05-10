/*******************************************************************************
 * Macro Name: PolygonCSV
 * Creator  : Martin Berard AEPONYX
 *
 * Revision History:
 * 15 Dec 2017
 *******************************************************************************/
#include <unistd.h> //getcwd

//#define EXCLUDE_LEDIT_LEGACY_UPI

#include <ldata.h>
/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */

#define MAX_POLYGON_SIZE 5000

void PolygonToCSV(void)
{
    //get the correct layer´/ cell
	//Variables
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer pLayer;
	char cwd[MAX_TDBFILE_NAME];
	char sLayerName[MAX_LAYER_NAME];

	LCurve CurveSetup;

	LObject object;
	LVertex vertex;
	
	FILE * myFile;

	LCoord xc, yc;
	float x,y;

	LFile_GetCurveSetup(pFile, &CurveSetup);
	LFile_SetCurveSetup(pFile, &CurveSetup);

	LDialogItem DialogItems[2] = {{ "Name of the CSV file","polygon.csv"}, { "Which layer to save","WGUIDE"}};
	
	if (LDialog_MultiLineInputBox("Polygon File",DialogItems,2))
   	{
		pLayer = LLayer_Find(pFile, DialogItems[1].value);
		if(NotAssigned(pLayer)) 
		{
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", DialogItems[1].value));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
   		LDialog_AlertBox(LFormat("The Polygon will be saved from Layer %s", sLayerName));

		if (getcwd(cwd, sizeof(cwd)) == NULL)
   			LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
		else
   			LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));

	   	strcat(cwd,DialogItems[0].value);
  		LUpi_LogMessage(LFormat("current csv file is: %s\n", cwd));
  		myFile = fopen(cwd,"w+");
		
		//test si le fichier existe dans le dossier
		//if (fopen(chemin,"r")!=NULL)
		
		if (myFile == NULL)
   			LUpi_LogMessage(LFormat("fopen() error: %s\n",strerror(errno)));
		else 
		{
			//LUpi_LogMessage(LFormat("opened polygon file is: %s\n", cwd));

			//for each object
			for(object = LObject_GetList(pCell, pLayer); object != NULL; object = LObject_GetNext(object))
			{
				/*
				if(LPolygon_HasCurve(object) == 1)
				{
					LUpi_LogMessage(LFormat("This polygon contains curves\n"));
					if(LPolygon_StraightenAllCurves(pCell, object) == LBadParameters)
					{
						LUpi_LogMessage(LFormat("LPolygon_StraightenAllCurves error\n"));
					}
				}
				*/

				if(LPolygon_StraightenAllCurves(pCell, object) == LBadParameters)
				{
					LUpi_LogMessage(LFormat("LPolygon_StraightenAllCurves error\n"));
				}
				

				for(vertex = LObject_GetVertexList(object); vertex != NULL; vertex = LVertex_GetNext(vertex))
				{
					xc = LVertex_GetPoint(vertex).x;
					yc = LVertex_GetPoint(vertex).y;

					x = (float)LFile_IntUtoMicrons(pFile, xc);
					y = (float)LFile_IntUtoMicrons(pFile, yc);

					fprintf(myFile, "%f,%f\n", (float)x, (float)y);
				}
				
			}
			fclose(myFile);
		}
	}
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F3" /*hotkey*/, 
			"AEPONYX\\PolygonToCSV\nWindow", "PolygonToCSV", NULL /*hotkey category*/);
	return 1;
}
