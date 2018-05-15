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
#define MAX_LENGTH_PATH 256
const char saveFile[] = "importPath.txt";

typedef struct curve{
	int typeCurve; //0:no curve, 1:curved polygone, 2:circle, 3:torus, 4:pie
	LCoord cx,cy;
	double r,r2;
	double startAngle, stopAngle;
	LArcDirection dir;
}t_curve;

typedef struct port{
	LCoord x0,y0,x1,y1;
	char text[MAX_PORT_NAME_LENGTH];
}t_port;

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
	char cwdFile[MAX_TDBFILE_NAME];
	char sLayerName[MAX_LAYER_NAME];
	char strLayer[MAX_LAYER_NAME];
	char strPath[MAX_LENGTH_PATH];

	char line[256];

	t_curve tcurve_arr[MAX_POLYGON_SIZE];
	t_port port;
	LWireConfig wire_config;

	LVertex vertex;

	char* token;

   	FILE * myFile;
   	int nPoints = 0;
	int i = 0;
	int cpt = 0;

	LObject polygon = NULL;


	if (getcwd(cwd, sizeof(cwd)) == NULL)
		LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
	else
		LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));
	
	strcpy(cwdFile, cwd);
	strcat(cwdFile, saveFile);
	myFile = fopen(cwdFile, "r");
	if(myFile == NULL)
	{
		LUpi_LogMessage(LFormat("Could not find: %s\n", cwdFile));
		strcpy(strPath, "polygon.csv");
	}
	else
	{
		fscanf(myFile,"%[^\n]", line);
		strcpy(strPath,line);
		fclose(myFile);
	}

	LDialog_File( strPath, "CSV file", strPath, "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||", 1, "Enter path of the CSV file containing the polygon", "OK", "csv", "*.csv||", pFile );
	strPath[strlen(strPath)-2]='\0';

	if (strPath != NULL)
   	{
		strcpy(strLayer, "WGUIDE");
		if ( LDialog_InputBox("Layer", "Enter name of the layer in which the polygon will be loaded", strLayer) == 0)
			return;
		else
			pLayer = LLayer_Find(pFile, strLayer);

		if(NotAssigned(pLayer)) 
		{
			if(strlen(strPath) >= MAX_LENGTH_PATH-1)
				LDialog_AlertBox(LFormat("ERROR:  CSV file path is too long."));
			else
				LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
   		LDialog_AlertBox(LFormat("The Polygon will be added in Layer %s", sLayerName));

		if(strPath[1] == ':') //chemin absolu
	   		strcpy(cwd,strPath);
		else //chemin relatif
			strcat(cwd,strPath);
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
				else if(cpt == 5)
				{
					port.x0 = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					port.y0 = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					TYPE = atoi(token);
					token = strtok(NULL, ",");
					port.x1 = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					port.y1 = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					strcpy(port.text, token);
				}
				else if(cpt == 6)
				{
					X = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					Y = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					TYPE = atoi(token);
					token = strtok(NULL, ",");
					wire_config.width = LC_Microns(atoi(token));
					token = strtok(NULL, ",");
					if(strcmp(token,"LJoinMiter")==0)
						wire_config.join = LJoinMiter;
					else if(strcmp(token,"LJoinRound")==0)
						wire_config.join = LJoinRound;
					else if(strcmp(token,"LJoinBevel")==0)
						wire_config.join = LJoinBevel;
					else if(strcmp(token,"LJoinLayout")==0)
						wire_config.join = LJoinLayout;

					token = strtok(NULL, ",");
					if(strcmp(token,"LCapButt")==0)
						wire_config.cap = LCapButt;
					else if(strcmp(token,"LCapRound")==0)
						wire_config.cap = LCapRound;
					else if(strcmp(token,"LCapExtend")==0)
						wire_config.cap = LCapExtend;

					token = strtok(NULL, ",");
					wire_config.miter_angle = LC_Microns(atoi(token));

					point_arr[nPoints] = LPoint_Set(X, Y);
					nPoints++;
				}
				fscanf(myFile,"\n");
			}	
			fclose(myFile);
		}
		if(cpt == 1 || cpt == 9)
		{
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
		}
		else if(cpt == 5)
		{
			LUpi_LogMessage(LFormat("PORT\n" ));
			polygon = LPort_New( pCell, pLayer, port.text, port.x0, port.y0, port.x1, port.y1 );
		}

		else if(cpt == 6)
		{
			LUpi_LogMessage(LFormat("WIRE\n"));
			polygon = LWire_New( pCell, pLayer, &wire_config, LSetWireAll, point_arr, nPoints );
		}
	}

	myFile = fopen(saveFile, "w");
	if(myFile == NULL)
		LUpi_LogMessage(LFormat("Could not save the file path\n"));
	else
	{
		fprintf(myFile, "%s", strPath);
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