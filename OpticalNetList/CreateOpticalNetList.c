
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ldata.h>
#include <string.h>

void CreateOpticalNetList(void)
{
	//Variables
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer pLayer;

	char str[MAX_LAYER_NAME];

	char strFileName[MAX_LAYER_NAME];

	char strStartLabelNameWanted[MAX_LAYER_NAME];
	char strEndLabelNameWanted[MAX_LAYER_NAME];
	char strLabelNameFound[MAX_LAYER_NAME];

	char strStartCellName[MAX_LAYER_NAME];
	char strEndCellName[MAX_LAYER_NAME];

	LCell tmpCell;
	LLabel pLab;

	int nbLabelFound;
	int userChoice = LOK;

	FILE * myFile = NULL;
	float radius;
	float width;

	LDialogItem DialogItems[2] = {{"Start Label Name","P1"},{"End Label Name","P2"}};
	strcpy(strFileName, "OpticalNetList.csv");

	//file name
	if ( LDialog_InputBox("Save File Name", "Enter the name of the ", strFileName) == 0)
		return;
	myFile = fopen(strFileName,"w");

	//radius
	strcpy(str, "10");
	if ( LDialog_InputBox("Radius", "Enter the radius of the circles wanted in microns", str) == 0)
		return;
	else
		radius = atof(str);

	//width
	strcpy(str, "1");
	if ( LDialog_InputBox("Width", "Enter the width of the guide wanted in microns", str) == 0)
		return;
	else
		width = atof(str);


	userChoice = LDialog_MultiLineInputBox("Start - End (Cancel to finish)",DialogItems,2);
	while(userChoice != LCANCEL)
	{
		strcpy(strStartLabelNameWanted,DialogItems[0].value);
		strcpy(strEndLabelNameWanted,DialogItems[1].value);

		nbLabelFound = 0;
		tmpCell = LCell_GetList(pFile);
		while( tmpCell != NULL && nbLabelFound != 2)
		{
			for(pLab = LLabel_GetList(tmpCell); pLab != NULL; pLab = LLabel_GetNext(pLab))
			{
				LLabel_GetName( pLab, strLabelNameFound, MAX_LAYER_NAME );
				if(strcmp(strStartLabelNameWanted, strLabelNameFound)==0)
				{
					LCell_GetName( tmpCell, strStartCellName, MAX_LAYER_NAME );
					nbLabelFound++;
				}
				else if(strcmp(strEndLabelNameWanted, strLabelNameFound)==0)
				{
					LCell_GetName( tmpCell, strEndCellName, MAX_LAYER_NAME );
					nbLabelFound++;
				}
			}
			tmpCell = LCell_GetNext(tmpCell);
		}
		if(nbLabelFound != 2)
		{
			LUpi_LogMessage(LFormat("ERROR: %d label found instead of 2\n", nbLabelFound));
		}
		else
		{
			//save in the file
			fprintf(myFile, "guide,%s,%s,%s,%s,%f,%f\n", strStartCellName, strStartLabelNameWanted, strEndCellName, strEndLabelNameWanted, radius, width);

			//end become the start
			strcpy(DialogItems[0].value, strEndLabelNameWanted);
			strcpy(DialogItems[1].value, "");
		}
		userChoice = LDialog_MultiLineInputBox("Start - End (Cancel to finish)",DialogItems,2);
	}
	fclose(myFile);

	LDisplay_Refresh();
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F6" /*hotkey*/, 
			"AEPONYX\\Create Optical Net List\nWindow", "CreateOpticalNetList", NULL /*hotkey category*/);
	return 1;
}