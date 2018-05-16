/*******************************************************************************
 * Macro Name: PolygonCSV
 * Creator  : Martin Berard AEPONYX
 *
 * Revision History:
 * 15 Dec 2017
 *******************************************************************************/
#include <unistd.h> //getcwd
#include <math.h>

#include <stdio.h>
#include <io.h> //modify this on linux

//#define EXCLUDE_LEDIT_LEGACY_UPI

#include <ldata.h>
/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */

#define MAX_POLYGON_SIZE 5000
#define MAX_PORT_NAME_LENGTH 200

LCoord Round(double d)
{
	if (d >= 0)
		return d+0.5;
	else
		return d-0.5;
} 

LCoord nLastx;
LCoord nLasty;

int firstVertex = 1;

bool bLastSegVertical = true;

void Add( double x, double y, FILE * myFile, LFile pFile )
{
	LCoord nx = Round( x );
	LCoord ny = Round( y );

	if(firstVertex != 1 &&  nx == nLastx && ny == nLasty )
		return; // do not duplicate vertex
		
	if(firstVertex == 1)
		firstVertex = 0;
	
	x = (float)LFile_IntUtoMicrons(pFile, nx);
	y = (float)LFile_IntUtoMicrons(pFile, ny);

	fprintf(myFile, "%f,%f\n", (float)x, (float)y);

	nLastx = nx;
	nLasty = ny;
}

void AddWirePoint( double x, double y, LObject object, FILE * myFile, LFile pFile )
{
	int type = 6;
	char strJoin[15];
	char strCap[15];

	LJoinType jt;
	LCapType ct;

	LCoord nx = Round( x );
	LCoord ny = Round( y );

	if(firstVertex != 1 &&  nx == nLastx && ny == nLasty )
		return; // do not duplicate vertex
		
	if(firstVertex == 1)
		firstVertex = 0;
	
	x = (float)LFile_IntUtoMicrons(pFile, nx);
	y = (float)LFile_IntUtoMicrons(pFile, ny);

	jt = LWire_GetJoinType(object);
	switch(jt)
	{
		case LJoinMiter:
			strcpy(strJoin,"LJoinMiter");
			break;
		case LJoinRound:
			strcpy(strJoin,"LJoinRound");
			break;
		case LJoinBevel:
			strcpy(strJoin,"LJoinBevel");
			break;
		case LJoinLayout:
			strcpy(strJoin,"LJoinLayout");
			break;
	}

	ct = LWire_GetCapType(object);
	switch(ct)
	{
		case LCapButt:
			strcpy(strCap,"LCapButt");
			break;
		case LCapRound:
			strcpy(strCap,"LCapRound");
			break;
		case LCapExtend:
			strcpy(strCap,"LCapExtend");
			break;
	}
	
	fprintf(myFile, "%f,%f,%d,%f,%s,%s,%f\n",(float)x, (float)y, type, (float)LFile_IntUtoMicrons(pFile, LWire_GetWidth(object)), strJoin, strCap, LWire_GetMiterAngle(object) );

	nLastx = nx;
	nLasty = ny;
}

void AddBox(LObject object, FILE * myFile, LFile pFile  )
{
	LRect rect = LBox_GetRect( object );
	Add( rect.x0, rect.y0, myFile, pFile );
	Add( rect.x0, rect.y1, myFile, pFile );
	Add( rect.x1, rect.y1, myFile, pFile );
	Add( rect.x1, rect.y0, myFile, pFile );
}

void AddPolygon(LObject object, FILE * myFile, LFile pFile  )
{
	LVertex currentVertex;
	for (currentVertex = LObject_GetVertexList(object); currentVertex != NULL; currentVertex = LVertex_GetNext(currentVertex))
	{
		Add( LVertex_GetPoint(currentVertex).x, LVertex_GetPoint(currentVertex).y, myFile, pFile );
	}
}

void AddWire(LObject object, FILE * myFile, LFile pFile  )
{
	LVertex currentVertex;
	for (currentVertex = LObject_GetVertexList(object); currentVertex != NULL; currentVertex = LVertex_GetNext(currentVertex))
	{
		AddWirePoint( LVertex_GetPoint(currentVertex).x, LVertex_GetPoint(currentVertex).y, object, myFile, pFile );
	}
}

void AddPort(LObject object, FILE * myFile, LFile pFile  )
{
	int type = 5;
	LRect rect;
	rect = LPort_GetRect( object );
	char str[MAX_PORT_NAME_LENGTH];
	LPort_GetText( object, str, MAX_PORT_NAME_LENGTH );
	fprintf(myFile, "%f,%f,%d,%f,%f,%s\n", (float)LFile_IntUtoMicrons(pFile, rect.x0), (float)LFile_IntUtoMicrons(pFile, rect.y0),type,(float)LFile_IntUtoMicrons(pFile, rect.x1),(float)LFile_IntUtoMicrons(pFile, rect.y1),str);
}

void AddCurve(FILE* myFile, LFile pFile, double x, double y, int type, LCoord cx, LCoord cy, double r, double r2, double start, double stop, LArcDirection dir)
{
	char str[4];
	if(dir == CW)
		strcpy(str, "CW");
	else
		strcpy(str, "CCW");
	fprintf(myFile, "%f,%f,%d,%f,%f,%f,%f,%f,%f,%s\n", (float)LFile_IntUtoMicrons(pFile, x), (float)LFile_IntUtoMicrons(pFile, y),type,(float)LFile_IntUtoMicrons(pFile, cx),(float)LFile_IntUtoMicrons(pFile, cy),(float)LFile_IntUtoMicrons(pFile, r),(float)LFile_IntUtoMicrons(pFile, r2),(float)start,(float)stop,str);
}

void PolygonToCSV(void)
{
    //get the correct layer´/ cell
	//Variables
	LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
	LLayer pLayer;
	char cwd[MAX_TDBFILE_NAME];
	char sLayerName[MAX_LAYER_NAME];

	LObject pObj;
	LVertex vertex;
	
	FILE * myFile;
	char filesRoot[MAX_TDBFILE_NAME*3];
	char fileName[MAX_TDBFILE_NAME*4];
	char name[MAX_TDBFILE_NAME];

	LGrid_v16_30 grid;
	LFile_GetGrid_v16_30( pFile, &grid );

	#ifndef ALGORITHM_A
		double dArcSegLen = grid.manufacturing_grid_size * 10.0;
	#endif	

	LStatus status;

	LCoord xc, yc;
	float x,y;
	int cpt=0;

	LDialogItem DialogItems[1] = { "Which layer to save if no selection made","WGUIDE"};
	
	if (LDialog_MultiLineInputBox("Polygon File",DialogItems,1))
   	{
		pLayer = LLayer_Find(pFile, DialogItems[0].value);
		if(NotAssigned(pLayer)) 
		{
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", DialogItems[0].value));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
		if(LSelection_GetList() == NULL)
		{
			LSelection_AddAllObjectsOnLayer( pLayer );
			LDialog_AlertBox(LFormat("Layer %s saved", sLayerName));
		}
		else
			LDialog_AlertBox(LFormat("Selection saved"));

		if (getcwd(cwd, sizeof(cwd)) == NULL)
   			LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
		else
   			LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));
		
		//test si le fichier existe dans le dossier
		//if (fopen(chemin,"r")!=NULL)
		
		//LUpi_LogMessage(LFormat("opened polygon file is: %s\n", cwd));

		filesRoot[0] = '\0';
		strcat (filesRoot,cwd);
		strcat(filesRoot, LCell_GetName(pCell, name, MAX_CELL_NAME) );
		strcat(filesRoot,"\\");
		if(access(filesRoot,0) != 0) //directory do not exist
		{
			#if defined(_WIN32)
				_mkdir(filesRoot);
			#else 
				mkdir(filesRoot, 0755); 
			#endif
			LUpi_LogMessage(LFormat("NEED TO CREATE FOLDER\n",filesRoot));
		}
		else
			LUpi_LogMessage(LFormat("FOLDER OK\n",filesRoot));
		strcat(filesRoot, LLayer_GetName(pLayer, name, MAX_LAYER_NAME) );
		strcat(filesRoot,"_");
		LUpi_LogMessage(LFormat("filesRoot: %s\n",filesRoot));

		for(LSelection pSel = LSelection_GetList(); pSel != NULL; pSel = LSelection_GetNext(pSel) )
		{
			pObj = LSelection_GetObject(pSel);
			//prend un compteur, chercher a mettre le nom stocké dans propriete du polygone
			fileName[0] = '\0';
			name[0] = '\0';
			strcat(fileName,filesRoot);
			LEntity_GetPropertyValue ((LEntity)pObj, "name", name, MAX_TDBFILE_NAME);
			if(name[0] != '\0')
				strcat(fileName, name);
			else
				strcat(fileName, itoa(cpt+1, name, 10));
			strcat(fileName, ".csv");
			
  			LUpi_LogMessage(LFormat("current csv file is: %s\n", fileName));
  			myFile = fopen(fileName,"w");
			

			if (LObject_GetShape(pObj) == LCircle)
			{
				//LDialog_MsgBox("Found a circle");
				LPoint ptCenter = LCircle_GetCenter(pObj);
				LCoord nRadius = LCircle_GetRadius(pObj);

				AddCurve(myFile, pFile, 0, 0, 2, ptCenter.x, ptCenter.y, nRadius, 0, 0, 0, CCW);
			}
			//LUpi_LogMessage(LFormat("shape is: %s\n", LObject_GetShape(pObj)));

			else if (LObject_GetGeometry(pObj) != LCurved)
			{
				//Polygon without curves
				if(LObject_GetShape(pObj) == LBox)
					AddBox(pObj, myFile, pFile);
				else if(LObject_GetShape(pObj) == LPolygon)
					AddPolygon(pObj, myFile, pFile);
				else if(LObject_GetShape(pObj) == LWire)
					AddWire(pObj, myFile, pFile);
				else if(LObject_GetShape(pObj) == LObjPort)
					AddPort(pObj, myFile, pFile);
				cpt++;
				fclose(myFile);
				continue;
			}

			switch( LObject_GetShape(pObj) )
			{
				case LPolygon:
				{
	//				LDialog_MsgBox("Found a curved polygon");
					for(LVertex pVert = LObject_GetVertexList(pObj); pVert != NULL; pVert = LVertex_GetNext(pVert))
					{
	//					char msg[200];
	//					sprintf(msg, "Found a vertex (%ld, %ld)", LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y);
	//					LDialog_MsgBox(msg);
						if ( ! LVertex_HasCurve(pVert))
						{
							Add( LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, myFile, pFile );
							continue;
						}
						
						LPoint ptStart;
						LPoint ptEnd;
						LPoint ptCenter;
						LArcDirection Dir;
						LCoord nRadius;
						LVertex_GetCurveEX(pObj, pVert, &ptCenter, &nRadius, &ptStart, &ptEnd, &Dir);

						DPoint ExactCenter = LVertex_GetCurveExactCenter( pObj, pVert, nRadius, &Dir );

						if(ExactCenter.x == ptCenter.x && ExactCenter.y == ptCenter.y)
							LUpi_LogMessage(LFormat("CENTRE IDENTIQUE\n"));
						else
							LUpi_LogMessage(LFormat("int x: %d float x: %f\nint x: %d float x: %f\n",ptCenter.x,ExactCenter.x,ptCenter.y,ExactCenter.y));

						//AddCurve(myFile, pFile, LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, 1, ptCenter.x, ptCenter.y, nRadius, 0, 0, 0, Dir);
						AddCurve(myFile, pFile, LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, 1, ExactCenter.x, ExactCenter.y, nRadius, 0, 0, 0, Dir);
					}
					cpt++;
					fclose(myFile);
					break;
				}
				case LTorus:
				{
					LTorusParams TorusParams;
					LTorus_GetParams(pObj, &TorusParams);

					AddCurve(myFile, pFile, 0, 0, 3, TorusParams.ptCenter.x, TorusParams.ptCenter.y, TorusParams.nInnerRadius, TorusParams.nOuterRadius, TorusParams.dStartAngle, TorusParams.dStopAngle, CCW);
					break;
				}

				case LPie:
				{
					LPieParams PieParams;
					LPie_GetParams(pObj, &PieParams);
					
					AddCurve(myFile, pFile, 0, 0, 4, PieParams.ptCenter.x, PieParams.ptCenter.y, PieParams.nRadius, 0, PieParams.dStartAngle, PieParams.dStopAngle, CCW);

					break;
				}

				default:
					LUpi_LogMessage(LFormat("DEFAULT\n"));
					break;
			}
			cpt++;
			fclose(myFile);
		}
		if(LDialog_YesNoBox("Do you want to export all the labels of the current layer ?"))
		{
			LPoint point;
			cpt=0;
			for(LLabel pLab = LLabel_GetList(pCell); pLab != NULL; pLab = LLabel_GetNext(pLab) )
			{
				int type = 7;
				fileName[0] = '\0';
				name[0] = '\0';
				strcat(fileName,filesRoot);
				strcat(fileName,"Label_");
				LLabel_GetName( pLab, name, MAX_TDBFILE_NAME );
				strcat(fileName,name);
				strcat(fileName,".csv");

				LUpi_LogMessage(LFormat("current csv file is: %s\n", fileName));
  				myFile = fopen(fileName,"w");
				
				point = LLabel_GetPosition(pLab);
				
				fprintf(myFile, "%f,%f,%d,%f,%f,%s\n", (float)LFile_IntUtoMicrons(pFile, point.x), (float)LFile_IntUtoMicrons(pFile, point.y), type, (float)LFile_IntUtoMicrons(pFile, LLabel_GetTextSize(pLab)), (float)LLabel_GetTextAlignment(pLab), name);

				fclose(myFile);

				cpt++;
			}
		}
	}
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F3" /*hotkey*/, 
			"AEPONYX\\PolygonToCSV\nWindow", "PolygonToCSV", NULL /*hotkey category*/);
	return 1;
}
