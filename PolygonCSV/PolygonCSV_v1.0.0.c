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

LPoint getArcCenter(LVertex current, LVertex next, float curveHeight)
{
	LPoint center = LPoint_Set(0,0);
	if(LVertex_GetPoint(current).x < LVertex_GetPoint(next).x)
		if(LVertex_GetPoint(current).y < LVertex_GetPoint(next).y)
			if(curveHeight < 0)
				center = LPoint_Set(LVertex_GetPoint(next).x, LVertex_GetPoint(current).y);
			else
				center = LPoint_Set(LVertex_GetPoint(current).x, LVertex_GetPoint(next).y);
		else			
			if(curveHeight < 0)
				center = LPoint_Set(LVertex_GetPoint(current).x, LVertex_GetPoint(next).y);
			else
				center = LPoint_Set(LVertex_GetPoint(next).x, LVertex_GetPoint(current).y);
	else
		if(LVertex_GetPoint(current).y < LVertex_GetPoint(next).y)
			if(curveHeight < 0)
				center = LPoint_Set(LVertex_GetPoint(current).x, LVertex_GetPoint(next).y);
			else
				center = LPoint_Set(LVertex_GetPoint(next).x, LVertex_GetPoint(current).y);
		else
			if(curveHeight < 0)
				center = LPoint_Set(LVertex_GetPoint(next).x, LVertex_GetPoint(current).y);
			else
				center = LPoint_Set(LVertex_GetPoint(current).x, LVertex_GetPoint(next).y);

	return center;
}

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
	int cpt = 0;

	LObject polygon = NULL;
	LPoint center = LPoint_Set(0,0);
	LVertex firstVertex = NULL;

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
	polygon=LPolygon_New(pCell, pLayer, point_arr, nPoints);

	firstVertex = LObject_GetVertexList(polygon);

	cpt = 0;
	// for each vertex of the polygon
	for(LVertex currentVertex = LObject_GetVertexList(polygon); currentVertex != NULL; currentVertex = LVertex_GetNext(currentVertex))
	{
		//if there is a curve height for that vertex
		if(curveHeight_arr[cpt] != 0)
		{
			// ALWAYS USE CLOCKWISE VERTEX/POLYGON

			//last vertex link to the first one
			if(LVertex_GetNext(currentVertex) == NULL)
				center = getArcCenter(currentVertex, firstVertex, curveHeight_arr[cpt]);
			else
				center = getArcCenter(currentVertex, LVertex_GetNext(currentVertex), curveHeight_arr[cpt]);
			//LPoint center = LPoint_Set((LVertex_GetPoint(currentVertex).x+LVertex_GetPoint(LVertex_GetNext(currentVertex)).x)/2, (LVertex_GetPoint(currentVertex).y+LVertex_GetPoint(LVertex_GetNext(currentVertex)).y)/2);
			//LUpi_LogMessage(LFormat("Centre X: %s\n", center.x));
			LVertex_AddCurve(polygon, currentVertex, center, CW);
		}
		cpt++;
	}

	//LCell_MakeVisible ( pCell );
	LDisplay_Refresh();
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F2" /*hotkey*/, 
			"AEPONYX\\PolygonToCSV\nWindow", "PolygonCSV", NULL /*hotkey category*/);
	return 1;
}
