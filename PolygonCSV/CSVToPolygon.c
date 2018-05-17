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
#define MAX_PORT_NAME_LENGTH 200
#define MAX_LABEL_TEXT_LENGTH 200
#define MAX_LENGTH_PATH 256
const char saveFile[] = "importPath.txt";

typedef struct shape{
	LShapeType shapeType;
	LGeomType geomType;
	LPoint point_arr[MAX_POLYGON_SIZE]; // could use malloc of LPoint (2 x LCoord or 2x Long or 2x 32bits) for undefined array size or base buffer on linecount
	int nPoints;
	LPoint XY[MAX_POLYGON_SIZE];
	double r;
	double r2;
	double startAngle; 
	double stopAngle;
	LArcDirection dir[MAX_POLYGON_SIZE];
	char text[MAX_PORT_NAME_LENGTH];
	LWireConfig wireConfig;
}t_shape;

typedef struct label{
	char text[MAX_LABEL_TEXT_LENGTH];
	double x, y;
	double textSize;
	int textAlignment;
}t_label;

LShapeType GetShapeType(char* str)
{
	LShapeType shape;
	if(strcmp(str,"LBox")==0)
		shape = LBox;
	else if(strcmp(str,"LCircle")==0)
		shape = LCircle;
	else if(strcmp(str,"LWire")==0)
		shape = LWire;
	else if(strcmp(str,"LPolygon")==0)
		shape = LPolygon;
	else if(strcmp(str,"LTorus")==0)
		shape = LTorus;
	else if(strcmp(str,"LPie")==0)
		shape = LPie;
	else if(strcmp(str,"LOtherObject")==0)
		shape = LOtherObject;
	else if(strcmp(str,"LObjInstance")==0)
		shape = LObjInstance;
	else if(strcmp(str,"LObjPort")==0)
		shape = LObjPort;
	else if(strcmp(str,"LObjRuler")==0)
		shape = LObjRuler;
	else
		shape = LOtherObject;
	
	return shape;
}

LGeomType GetGeometryType(char* str)
{
	LGeomType geom;
	if(strcmp(str,"LOrthogonal")==0)
		geom = LOrthogonal;
	else if(strcmp(str,"LFortyFive")==0)
		geom = LFortyFive;
	else if(strcmp(str,"LAllAngle")==0)
		geom = LAllAngle;
	else if(strcmp(str,"LCurved")==0)
		geom = LCurved;
	else if(strcmp(str,"LNonGeometric")==0)
		geom = LNonGeometric;
	else
		geom = LNonGeometric;

	return geom;
}

LArcDirection GetArcDirection(char* str)
{
	LArcDirection dir;
	if(strcmp(str,"CW")==0)
		dir = CW;
	else if(strcmp(str,"CCW")==0)
		dir = CCW;
	else
		dir = CW;

	return dir;
}

LJoinType GetJoinType(char* str)
{
	LJoinType join;
	if(strcmp(str,"LJoinMiter")==0)
		join = LJoinMiter;
	else if(strcmp(str,"LJoinRound")==0)
		join = LJoinRound;
	else if(strcmp(str,"LJoinBevel")==0)
		join = LJoinBevel;
	else if(strcmp(str,"LJoinLayout")==0)
		join = LJoinLayout;
	else
		join = LJoinLayout;

	return join;
}

LCapType GetCapType(char* str)
{
	LCapType cap;
	if(strcmp(str,"LCapButt")==0)
		cap = LCapButt;
	else if(strcmp(str,"LCapRound")==0)
		cap = LCapRound;
	else if(strcmp(str,"LCapExtend")==0)
		cap = LCapExtend;
	else
		cap = LCapExtend;
	
	return cap;
}

void CSVToPolygon(void)
{
	//Variables
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer pLayer;

	char cwd[MAX_TDBFILE_NAME];
	char cwdFile[MAX_TDBFILE_NAME];
	char sLayerName[MAX_LAYER_NAME];
	char strLayer[MAX_LAYER_NAME];
	char strPath[MAX_LENGTH_PATH];

	char line[256];

	t_shape shape;
	shape.nPoints = 0;
	shape.r = 0;
	shape.r2 = 0;
	shape.startAngle = 0; 
	shape.stopAngle = 0;

	t_label labelStruct;

	LVertex vertex;

	char* token;

   	FILE * myFile;
	int i = 0;
	int cpt = 0;

	LObject polygon = NULL;

	//get the path
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
	else
		LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));
	
	strcpy(cwdFile, cwd);
	strcat(cwdFile, saveFile); //path to the file that save the last csv file path used
	myFile = fopen(cwdFile, "r");
	if(myFile == NULL)
	{
		LUpi_LogMessage(LFormat("Could not find: %s\n", cwdFile));
		strcpy(strPath, "polygon.csv");
	}
	else
	{
		fscanf(myFile,"%[^\n]", line);
		strcpy(strPath,line); //store the last csv file path used
		fclose(myFile);
	}

	//Dialog box to browse the correct file
	LDialog_File( strPath, "CSV file", strPath, "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||", 1, "Enter path of the CSV file containing the polygon", "OK", "csv", "*.csv||", pFile );
	strPath[strlen(strPath)-2]='\0'; //delete the last 2 char of the string ("|0")

	if (strPath != NULL)
   	{
		strcpy(strLayer, "WGUIDE"); //preloaded text in the dialog box
		if ( LDialog_InputBox("Layer", "Enter name of the layer in which the polygon will be loaded", strLayer) == 0)
			return;
		else
			pLayer = LLayer_Find(pFile, strLayer);

		if(NotAssigned(pLayer)) //the layer is not found
		{
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
   		LDialog_AlertBox(LFormat("The Polygon will be added in Layer %s", sLayerName));
			
		LFile_GetResolvedFileName( pFile, strPath, cwd, MAX_TDBFILE_NAME ); //get the absolute path of strPath in cwd

		LUpi_LogMessage(LFormat("current csv file is: %s\n", cwd));
  		myFile = fopen(cwd,"r");
   		if (myFile == NULL)
   			LUpi_LogMessage(LFormat("fopen() error: %s\n",strerror(errno)));
		else 
		{
			LUpi_LogMessage(LFormat("opened polygon file is: %s\n", cwd));
			while(!feof(myFile))
			{
				// reads text until new line 
				fscanf(myFile,"%[^\n]", line);
				LUpi_LogMessage(LFormat("READ LINE: %s\n", line));
				cpt=0;
				for(i=0; i<strlen(line);i++)
				{
					if(line[i]==',') //count the number of ',' to know what kind of shape it is
						cpt++;
				}

				token = strtok(line, ","); //first string before the first ',' of line

				if(cpt == 3) //point
				{
					shape.shapeType = GetShapeType(token);
					token = strtok(NULL, ","); //next part
					shape.geomType = GetGeometryType(token);
					token = strtok(NULL, ",");
					shape.point_arr[shape.nPoints].x = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.point_arr[shape.nPoints].y = LFile_MicronsToIntU(pFile,atof(token));
					shape.nPoints = shape.nPoints+1;
				}
				else if(cpt == 15)
				{
					shape.shapeType = GetShapeType(token);
					token = strtok(NULL, ","); //next part
					shape.geomType = GetGeometryType(token);
					token = strtok(NULL, ",");
					shape.point_arr[shape.nPoints].x = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.point_arr[shape.nPoints].y = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.XY[shape.nPoints].x = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.XY[shape.nPoints].y = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.r = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.r2 = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.startAngle = atof(token);
					token = strtok(NULL, ",");
					shape.stopAngle = atof(token);
					token = strtok(NULL, ",");
					shape.dir[shape.nPoints] = GetArcDirection(token);
					token = strtok(NULL, ",");
					strcpy(shape.text, token);
					token = strtok(NULL, ",");
					shape.wireConfig.width = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					shape.wireConfig.join = GetJoinType(token);
					token = strtok(NULL, ",");
					shape.wireConfig.cap = GetCapType(token);
					token = strtok(NULL, ",");
					shape.wireConfig.miter_angle = atof(token);

					shape.nPoints = shape.nPoints+1;
				}
				else if(cpt == 5) //port or label
				{
					labelStruct.x = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					labelStruct.y = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					labelStruct.textSize = LFile_MicronsToIntU(pFile,atof(token));
					token = strtok(NULL, ",");
					labelStruct.textAlignment = atoi(token);
					token = strtok(NULL, ",");
					strcpy(labelStruct.text, token);
				}
				fscanf(myFile,"\n"); //got o the next line
			}	
			fclose(myFile);
		}



		if(cpt == 3 || cpt == 15) //poygon with or without curve, circle, pie, torus
		{
			polygon=LPolygon_New(pCell, pLayer, shape.point_arr, shape.nPoints); //create the polygon that will be modified
			LUpi_LogMessage(LFormat("POLYGON CREATED\n" ));
			
			vertex = LObject_GetVertexList(polygon);
			for(cpt=0; cpt<shape.nPoints; cpt++) //for each vertex
			{
				switch(shape.shapeType)
				{
					case LBox:
						LUpi_LogMessage(LFormat("BOX\n" ));
						polygon = LBox_New( pCell, pLayer, shape.point_arr[cpt].x, shape.point_arr[cpt].y, shape.XY[cpt].x, shape.XY[cpt].y );
						break;
					case LCircle:
						LUpi_LogMessage(LFormat("CIRCLE\n" ));
						polygon = LCircle_New( pCell, pLayer, shape.XY[cpt], shape.r );
						break;
					case LWire:
						LUpi_LogMessage(LFormat("WIRE CREATED\n"));
						polygon = LWire_New( pCell, pLayer, &shape.wireConfig, LSetWireAll, shape.point_arr, shape.nPoints );
						break;
					case LPolygon:
						if(shape.geomType == LCurved)
						{
							LUpi_LogMessage(LFormat("ADDING A CURVE TO THE POLYGON\n" ));
							LVertex_AddCurve(polygon, vertex, shape.XY[cpt], shape.dir[cpt]);
						}
						break;
					case LTorus:
						LUpi_LogMessage(LFormat("TORUS\n" ));
						LTorusParams tp;
						tp.ptCenter = shape.XY[cpt];
						tp.nInnerRadius = shape.r;
						tp.nOuterRadius = shape.r2;
						tp.dStartAngle = shape.startAngle;
						tp.dStopAngle = shape.stopAngle;

						polygon = LTorus_CreateNew( pCell, pLayer, &tp );
						break;
					case LPie:
						LUpi_LogMessage(LFormat("PIE\n" ));
						LPieParams pp;
						pp.ptCenter = shape.XY[cpt];
						pp.nRadius = shape.r;
						pp.dStartAngle = shape.startAngle;
						pp.dStopAngle = shape.stopAngle;

						polygon = LPie_CreateNew( pCell, pLayer, &pp );
						break;
					case LObjPort:
						LUpi_LogMessage(LFormat("PORT\n"));
						polygon = LPort_New( pCell, pLayer, shape.text, shape.point_arr[cpt].x, shape.point_arr[cpt].y, shape.XY[cpt].x, shape.XY[cpt].y );
					default:
						LUpi_LogMessage(LFormat("Object shape not found\n" ));
				}
				vertex = LVertex_GetNext(vertex);
			}			
		}
		else if(cpt == 5) //for label
		{
			LUpi_LogMessage(LFormat("LABEL CREATED\n"));
			LLabel label;
			label = LLabel_New( pCell, pLayer, labelStruct.text, labelStruct.x, labelStruct.y );
			LLabel_SetTextSize( pCell, label, labelStruct.textSize );
			LLabel_SetTextAlignment( pCell, label, labelStruct.textAlignment );
		}
	}

	//save the csv file path in importPath.txt
	myFile = fopen(saveFile, "w");
	if(myFile == NULL)
		LUpi_LogMessage(LFormat("Could not save the file path\n"));
	else
	{
		fprintf(myFile, "%s\n", cwd);
		fclose(myFile);
		LUpi_LogMessage(LFormat("File path saved\n"));
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