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

void PolygonCSV(void)
{
	//Variables
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer pLayer;
	LCoord XC = 0;
	LCoord YC = 0;
	LPoint point_arr[MAX_POLYGON_SIZE]; // could use malloc of LPoint (2 x LCoord or 2x Long or 2x 32bits) for undefined array size or base buffer on linecount
	float curveHeight_arr[MAX_POLYGON_SIZE];
	float X,Y,CH;
	char cwd[MAX_TDBFILE_NAME];
	char sLayerName[MAX_LAYER_NAME];
   	FILE * myFile;
   	int nPoints = 0;

	LDialogItem DialogItems[2] = {{ "CSV file","polygon_fillet.csv"}, { "Layer","WGUIDE"}};
	
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
   			while (!feof(myFile))
   			{
    			fscanf(myFile, "%f,%f,%f", &X, &Y, &CH);
   				XC = LC_Microns(X);
   				YC = LC_Microns(Y);
				point_arr[nPoints] = LPoint_Set(XC, YC);
				curveHeight_arr[nPoints] = CH;
				nPoints = nPoints+1;	
			}
			fclose(myFile);
		}
	}

	//LDialog_MsgBox(LFormat("nPoints = %d." , nPoints ));
	LUpi_LogMessage(LFormat("nPoints = %d.\n" , nPoints ));
	LPolygon_New(pCell, pLayer, point_arr, nPoints);
	//LCell_MakeVisible ( pCell );
	LDisplay_Refresh();
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F2" /*hotkey*/, 
			"AEPONYX\\PolygonToCSV\nWindow", "PolygonCSV", NULL /*hotkey category*/);
	return 1;
}
