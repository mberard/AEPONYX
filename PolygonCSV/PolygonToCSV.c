/*******************************************************************************
 * Macro Name: PolygonCSV
 * Creator  : Martin Berard AEPONYX
 *
 * Revision History:
 * 15 Dec 2017
 *******************************************************************************/
#include <unistd.h> //getcwd
#include <math.h>

//#define EXCLUDE_LEDIT_LEGACY_UPI

#include <ldata.h>
/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */

#define MAX_POLYGON_SIZE 5000

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
		
		//test si le fichier existe dans le dossier
		//if (fopen(chemin,"r")!=NULL)
		
		//LUpi_LogMessage(LFormat("opened polygon file is: %s\n", cwd));

		filesRoot[0] = '\0';
		strcat (filesRoot,cwd);
		strcat(filesRoot, LCell_GetName(pCell, name, MAX_CELL_NAME) );
		strcat(filesRoot,"\\");
		//strcat(filesRoot, LFile_GetName(pFile, name, MAX_TDBFILE_NAME) );
		//strcat(filesRoot,"_");
		strcat(filesRoot, LLayer_GetName(pLayer, name, MAX_LAYER_NAME) );
		strcat(filesRoot,"_");
		LUpi_LogMessage(LFormat("filesRoot: %s\n",filesRoot));

		if(LSelection_GetList() == NULL)
			LSelection_AddAllObjectsOnLayer( pLayer );

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
			

			// CODE POUR PASSER DES COURBES EN SERIE DE VERTEX
			if (LObject_GetShape(pObj) == LCircle)
			{
				//LDialog_MsgBox("Found a circle");
				LPoint ptCenter = LCircle_GetCenter(pObj);
				LCoord nRadius = LCircle_GetRadius(pObj);

				AddCurve(myFile, pFile, 0, 0, 2, ptCenter.x, ptCenter.y, nRadius, 0, 0, 0, CCW);
			}

			else if (LObject_GetGeometry(pObj) != LCurved)
			{
				//Polygon without curves
				if(LObject_GetShape(pObj) == LBox)
					AddBox(pObj, myFile, pFile);
				else
					AddPolygon(pObj, myFile, pFile);
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

						AddCurve(myFile, pFile, LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, 1, ptCenter.x, ptCenter.y, nRadius, 0, 0, 0, Dir);
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
					break;
			}
			cpt++;
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
