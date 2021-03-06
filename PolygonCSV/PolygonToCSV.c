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
#define MAX_CELL_NAME_LENGTH 200
#define MAX_GEOMETRY_LENGTH 15
#define MAX_SHAPE_LENGTH 15
#define MAX_JOIN_LENGTH 15
#define MAX_CAP_LENGTH 15
#define MAX_ARCDIR_LENGTH 5

double Round(double d)
{
	if (d >= 0)
		return d+0.5;
	else
		return d-0.5;
}

double Round0or5(double val)
{
	double tmpFloat = val*10000;
	long tmpInt = 0;
	double returnedVal = 0;
	if(val >= 0)
		tmpFloat = (double)(tmpFloat + 2.5)/5.0;
	else
		tmpFloat = (double)(tmpFloat - 2.5)/5.0;
	tmpInt = (long)tmpFloat; //delete the digits after the '.'
	returnedVal = tmpInt*5;
	returnedVal = returnedVal/10000;
	return returnedVal;
}

LCoord nLastx;
LCoord nLasty;

int firstVertex = 1;

bool bLastSegVertical = true;

char* GetGeometry(LObject object)
{
	LGeomType geo;
	char* geoStr = malloc(MAX_GEOMETRY_LENGTH* sizeof(char));
	geo = LObject_GetGeometry( object );
	switch(geo)
	{
		case LOrthogonal:
			strcpy(geoStr,"LOrthogonal");
			break;
		case LFortyFive:
			strcpy(geoStr,"LFortyFive");
			break;
		case LAllAngle:
			strcpy(geoStr,"LAllAngle");
			break;
		case LCurved:
			strcpy(geoStr,"LCurved");
			break;
		case LNonGeometric:
			strcpy(geoStr,"LNonGeometric");
			break;
		default:
			strcpy(geoStr,"");
	}
	return geoStr;
}

char* GetShape(LObject object)
{
	LShapeType shape;
	char* shapeStr = malloc(MAX_SHAPE_LENGTH* sizeof(char));
	shape = LObject_GetShape( object );
	switch(shape)
	{
		case LBox:
			strcpy(shapeStr,"LBox");
			break;
		case LCircle:
			strcpy(shapeStr,"LCircle");
			break;
		case LWire:
			strcpy(shapeStr,"LWire");
			break;
		case LPolygon:
			strcpy(shapeStr,"LPolygon");
			break;
		case LTorus:
			strcpy(shapeStr,"LTorus");
			break;
		case LPie:
			strcpy(shapeStr,"LPie");
			break;
		case LOtherObject:
			strcpy(shapeStr,"LOtherObject");
			break;
		case LObjInstance:
			strcpy(shapeStr,"LObjInstance");
			break;
		case LObjPort:
			strcpy(shapeStr,"LObjPort");
			break;
		case LObjRuler:
			strcpy(shapeStr,"LObjRuler");
			break;
		default:
			strcpy(shapeStr,"");
	}
	return shapeStr;
}
char* getArcDirection(LArcDirection dir)
{
	char* dirStr = malloc(MAX_ARCDIR_LENGTH* sizeof(char));
	switch(dir)
	{
		case CW:
			strcpy(dirStr, "CW");
			break;
		case CCW:
			strcpy(dirStr, "CCW");
			break;
		default:
			strcpy(dirStr,"");
	}
	return dirStr;
}

char* GetJoinStr(LJoinType join)
{
	char* joinStr = malloc(MAX_JOIN_LENGTH* sizeof(char));
	switch(join)
	{
		case LJoinMiter:
			strcpy(joinStr,"LJoinMiter");
			break;
		case LJoinRound:
			strcpy(joinStr,"LJoinRound");
			break;
		case LJoinBevel:
			strcpy(joinStr,"LJoinBevel");
			break;
		case LJoinLayout:
			strcpy(joinStr,"LJoinLayout");
			break;
		default:
			strcpy(joinStr,"");
	}
	return joinStr;
}

char* GetCapStr(LCapType cap)
{
	char* capStr = malloc(MAX_CAP_LENGTH* sizeof(char));
	switch(cap)
	{
		case LCapButt:
			strcpy(capStr,"LCapButt");
			break;
		case LCapRound:
			strcpy(capStr,"LCapRound");
			break;
		case LCapExtend:
			strcpy(capStr,"LCapExtend");
			break;
		default:
			strcpy(capStr,"");
	}
	return capStr;
}

void Add( LObject object, double x, double y, FILE * myFile, LFile pFile )
{
	double nx = Round( x );
	double ny = Round( y );

	if(firstVertex != 1 &&  nx == nLastx && ny == nLasty ) //not the first vertex AND the same point as the previous
		return; // do not duplicate vertex
		
	if(firstVertex == 1)
		firstVertex = 0;
	
	x = LFile_IntUtoMicrons(pFile, nx); //store in microns
	y = LFile_IntUtoMicrons(pFile, ny);

	fprintf(myFile, "%s,%s,%f,%f\n", GetShape(object), GetGeometry(object), x, y);

	nLastx = nx;
	nLasty = ny;
}

void PrintInSaveFile( LObject object, FILE * myFile, LFile pFile , double x, double y, DPoint XY0, double r, double r2, double startAngle, double stopAngle, LArcDirection dir, char* str, LWireConfig* wireConfig)
{
	char string1[MAX_PORT_NAME_LENGTH];
	char string2[MAX_JOIN_LENGTH];
	char string3[MAX_CAP_LENGTH];

	if(strcmp(str, "") == 0)
		strcpy(string1,"NA");
	else
		strcpy(string1,str);

	if(strcmp(GetJoinStr(wireConfig->join), "") == 0)
		strcpy(string2,"NA");
	else
		strcpy(string2,GetJoinStr(wireConfig->join));

	if(strcmp(GetCapStr(wireConfig->cap), "") == 0)
		strcpy(string3,"NA");
	else
		strcpy(string3,GetCapStr(wireConfig->cap));

	fprintf(myFile, "%s,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%s,%s,%lf,%s,%s,%lf\n",
								GetShape(object),
								GetGeometry(object),
								LFile_IntUtoMicrons(pFile, x),
								LFile_IntUtoMicrons(pFile, y),
								Round0or5(LFile_IntUtoMicrons(pFile, XY0.x)),
								Round0or5(LFile_IntUtoMicrons(pFile, XY0.y)),
								Round0or5(LFile_IntUtoMicrons(pFile, r)),
								Round0or5(LFile_IntUtoMicrons(pFile, r2)),
								startAngle,
								stopAngle,
								getArcDirection(dir),
								string1,
								Round0or5(LFile_IntUtoMicrons(pFile, wireConfig->width)),
								string2,
								string3,
								wireConfig->miter_angle );
}

void AddWirePoint( double x, double y, LObject object, FILE * myFile, LFile pFile )
{
	LWireConfig conf;
	DPoint XY0;
	XY0.x = 0;
	XY0.y = 0;

	double nx = Round( x );
	double ny = Round( y );

	if(firstVertex != 1 &&  nx == nLastx && ny == nLasty )
		return; // do not duplicate vertex
		
	if(firstVertex == 1)
		firstVertex = 0;

	conf.width = LWire_GetWidth(object); //create the wire config
	conf.join = LWire_GetJoinType(object);
	conf.cap = LWire_GetCapType(object);
	conf.miter_angle = LWire_GetMiterAngle(object);
	
	PrintInSaveFile(object, myFile, pFile , nx, ny, XY0, 0, 0, 0, 0, CCW, "", &conf);

	nLastx = nx;
	nLasty = ny;
}

void AddBox(LObject object, FILE * myFile, LFile pFile  )
{
	LRect rect = LBox_GetRect( object );
	DPoint XY0;
	LWireConfig conf;

	XY0.x = rect.x1; //second point of the box (top-right) stored in the DPoint XY0
	XY0.y = rect.y1;
	conf.width = 0;
	conf.miter_angle = 0;

	PrintInSaveFile(object, myFile, pFile , rect.x0, rect.y0, XY0, 0, 0, 0, 0, CCW, "", &conf);
}

void AddPolygon(LObject object, FILE * myFile, LFile pFile  )
{
	LVertex currentVertex;
	for (currentVertex = LObject_GetVertexList(object); currentVertex != NULL; currentVertex = LVertex_GetNext(currentVertex)) //each vertex of the polygon
	{
		Add( object, LVertex_GetPoint(currentVertex).x, LVertex_GetPoint(currentVertex).y, myFile, pFile );
	}
}

void AddWire(LObject object, FILE * myFile, LFile pFile  )
{
	LVertex currentVertex;
	for (currentVertex = LObject_GetVertexList(object); currentVertex != NULL; currentVertex = LVertex_GetNext(currentVertex)) //each vertex of the wire
	{
		AddWirePoint( LVertex_GetPoint(currentVertex).x, LVertex_GetPoint(currentVertex).y, object, myFile, pFile );
	}
}

void AddPort(LObject object, FILE * myFile, LFile pFile  )
{
	LRect rect;
	char str[MAX_PORT_NAME_LENGTH];
	DPoint XY0;
	LWireConfig wireConfig;

	rect = LPort_GetRect( object );
	XY0.x = rect.x1; //second point of the box (top-right) stored in the DPoint XY0
	XY0.y = rect.y1;
	LPort_GetText( object, str, MAX_PORT_NAME_LENGTH ); //text of the port
	wireConfig.width = 0;
	wireConfig.miter_angle = 0;

	PrintInSaveFile( object, myFile, pFile , rect.x0, rect.y0, XY0, 0, 0, 0, 0, CCW, str, &wireConfig);

}

void AddCurve(LObject object, FILE* myFile, LFile pFile, double x, double y, double cx, double cy, double r, double r2, double start, double stop, LArcDirection dir)
{
	DPoint XY0;
	LWireConfig conf;
	XY0.x = cx; //center of the curve stored in the DPoint XY0
	XY0.y = cy;
	conf.width = 0;
	conf.miter_angle = 0;
	PrintInSaveFile( object, myFile, pFile , x, y, XY0, r, r2, start, stop, dir, "", &conf);
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
	
	if (LDialog_MultiLineInputBox("Polygon File",DialogItems,1)) //seletion of the layer
   	{
		pLayer = LLayer_Find(pFile, DialogItems[0].value); //find the layer
		if(NotAssigned(pLayer)) //layer not found
		{
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", DialogItems[0].value));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
		if(LSelection_GetList() == NULL) //if no selection made
		{
			LSelection_AddAllObjectsOnLayer( pLayer ); //add the entire layer to the selection
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
		strcat (filesRoot,cwd);											//cwd
		strcat(filesRoot, LCell_GetName(pCell, name, MAX_CELL_NAME) );	// + name of the cell
		strcat(filesRoot,"\\");											// + '\'
		if(access(filesRoot,0) != 0) //directory do not exist ==> create the diredctory
		{
			#if defined(_WIN32)
				_mkdir(filesRoot);
			#else 
				mkdir(filesRoot, 0755); 
			#endif
			LUpi_LogMessage(LFormat("New folder created\n",filesRoot));
		}
		strcat(filesRoot, LLayer_GetName(pLayer, name, MAX_LAYER_NAME) );// + name of the layer
		strcat(filesRoot,"_");											 // + '_'
		LUpi_LogMessage(LFormat("filesRoot: %s\n",filesRoot));

		for(LSelection pSel = LSelection_GetList(); pSel != NULL; pSel = LSelection_GetNext(pSel) )
		{
			pObj = LSelection_GetObject(pSel);
			//prend un compteur, chercher a mettre le nom stocké dans propriete du polygone
			fileName[0] = '\0';
			name[0] = '\0';
			strcat(fileName,filesRoot);
			LEntity_GetPropertyValue ((LEntity)pObj, "name", name, MAX_TDBFILE_NAME);
			if(name[0] != '\0')											// + name of the shape OR a number
				strcat(fileName, name);
			else
				strcat(fileName, itoa(cpt+1, name, 10)); //int to string; itoa(int, string, numeric base)
			strcat(fileName, ".csv");									// + ".csv"
			
  			LUpi_LogMessage(LFormat("current csv file is: %s\n", fileName));
  			myFile = fopen(fileName,"w");
			

			if (LObject_GetShape(pObj) == LCircle) //export circle
			{
				//LDialog_MsgBox("Found a circle");
				LPoint ptCenter = LCircle_GetCenter(pObj);
				LCoord nRadius = LCircle_GetRadius(pObj);

				AddCurve(pObj, myFile, pFile, 0, 0, ptCenter.x, ptCenter.y, nRadius, 0, 0, 0, CCW);
			}
			//LUpi_LogMessage(LFormat("shape is: %s\n", LObject_GetShape(pObj)));

			else if (LObject_GetGeometry(pObj) != LCurved) //shape witout curves
			{
				//Polygon without curves
				if(LObject_GetShape(pObj) == LBox) //export box
					AddBox(pObj, myFile, pFile);
				else if(LObject_GetShape(pObj) == LPolygon) //export polygon
					AddPolygon(pObj, myFile, pFile);
				else if(LObject_GetShape(pObj) == LWire) //export wire
					AddWire(pObj, myFile, pFile);
				else if(LObject_GetShape(pObj) == LObjPort) //export port
					AddPort(pObj, myFile, pFile);
				cpt++;
				fclose(myFile);
				continue;
			}

			switch( LObject_GetShape(pObj) ) //any shape not already export
			{
				case LPolygon: //export polygon with shape
				{
					for(LVertex pVert = LObject_GetVertexList(pObj); pVert != NULL; pVert = LVertex_GetNext(pVert)) //for each vertex of the polygon
					{
						if ( ! LVertex_HasCurve(pVert)) //if the vertex is NOT a curve
						{
							Add( pObj, LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, myFile, pFile );
							continue;
						}
						//vertex with curve
						LPoint ptStart;
						LPoint ptEnd;
						LPoint ptCenter;
						LArcDirection Dir;
						LCoord nRadius;
						LVertex_GetCurveEX(pObj, pVert, &ptCenter, &nRadius, &ptStart, &ptEnd, &Dir);

						DPoint ExactCenter = LVertex_GetCurveExactCenter( pObj, pVert, nRadius, &Dir );

						AddCurve(pObj, myFile, pFile, LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, ptCenter.x, ptCenter.y, nRadius, 0, 0, 0, Dir);
						//AddCurve(pObj, myFile, pFile, LVertex_GetPoint(pVert).x, LVertex_GetPoint(pVert).y, ExactCenter.x, ExactCenter.y, nRadius, 0, 0, 0, Dir);
					}
					break;
				}
				case LTorus: //export torus
				{
					LTorusParams TorusParams;
					LTorus_GetParams(pObj, &TorusParams);

					AddCurve(pObj, myFile, pFile, 0, 0, TorusParams.ptCenter.x, TorusParams.ptCenter.y, TorusParams.nInnerRadius, TorusParams.nOuterRadius, TorusParams.dStartAngle, TorusParams.dStopAngle, CCW);
					break;
				}

				case LPie: //export pie
				{
					LPieParams PieParams;
					LPie_GetParams(pObj, &PieParams);
					
					AddCurve(pObj, myFile, pFile, 0, 0, PieParams.ptCenter.x, PieParams.ptCenter.y, PieParams.nRadius, 0, PieParams.dStartAngle, PieParams.dStopAngle, CCW);

					break;
				}

				default: //other shape
					if(LObject_GetShape(pObj) != LCircle)
					{
						LUpi_LogMessage(LFormat("Shape not found: %s\n", LObject_GetShape(pObj)));
					}
					break;
			}
			cpt++;
			fclose(myFile);
		}
		if(LLabel_GetList(pCell)) //if there are some labels
		{
			if(LDialog_YesNoBox("Do you want to export all the labels of the current layer ?")) //ask if needed to export labels
			{
				LPoint point;
				for(LLabel pLab = LLabel_GetList(pCell); pLab != NULL; pLab = LLabel_GetNext(pLab) ) //for each label
				{
					fileName[0] = '\0';
					name[0] = '\0';
					strcat(fileName,filesRoot);						//complete the file path with "Label_" and the name of the label
					strcat(fileName,"Label_");
					LLabel_GetName( pLab, name, MAX_TDBFILE_NAME );
					strcat(fileName,name);
					strcat(fileName,".csv");

					LUpi_LogMessage(LFormat("current csv file is: %s\n", fileName));
					myFile = fopen(fileName,"w");
					
					point = LLabel_GetPosition(pLab);
					
					fprintf(myFile, "%s,%lf,%lf,%lf,%d,%s\n", 
											"LLabel", 
											LFile_IntUtoMicrons(pFile, point.x), 
											LFile_IntUtoMicrons(pFile, point.y), 
											LFile_IntUtoMicrons(pFile, LLabel_GetTextSize(pLab)), 
											LLabel_GetTextAlignment(pLab), 
											name);

					fclose(myFile);
				}
			}
		}
		
		if(LInstance_GetList(pCell)) //if there are some labels
		{
			if(LDialog_YesNoBox("Do you want to export all the instance of the current layer ?")) //ask if needed to export instance
			{
				LTransform_Ex99 tranformation;
				LPoint repeat_cnt;
				LPoint delta;
				char instanceName[MAX_CELL_NAME_LENGTH];
				char cellName[MAX_CELL_NAME_LENGTH];
				for(LInstance instance = LInstance_GetList(pCell); instance != NULL; instance = LInstance_GetNext(instance) ) //for each instance
				{
					tranformation = LInstance_GetTransform_Ex99( instance );
					repeat_cnt = LInstance_GetRepeatCount( instance );
					delta = LInstance_GetDelta( instance );
					LInstance_GetName( instance, instanceName, MAX_CELL_NAME_LENGTH );
					LCell_GetName( LInstance_GetCell( instance ) , cellName, MAX_CELL_NAME_LENGTH );

					fileName[0] = '\0';
					name[0] = '\0';
					strcat(fileName,filesRoot);		
					strcat(fileName,"Instance_");		//complete the file path with "Instance_", the instanced cell and the name of the instance
					strcat(fileName,cellName);
					strcat(fileName,"_");
					strcat(fileName,instanceName);
					strcat(fileName,".csv");
					LUpi_LogMessage(LFormat("current csv file is: %s\n", fileName));
					myFile = fopen(fileName,"w");

					fprintf(myFile, "%s,%lf,%lf,%lf,%ld,%ld,%d,%d,%lf,%lf,%s,%s\n",
											"LInstance",
											LFile_IntUtoMicrons(pFile, tranformation.translation.x),
											LFile_IntUtoMicrons(pFile, tranformation.translation.y),
											tranformation.orientation,
											tranformation.magnification.num,
											tranformation.magnification.denom,
											repeat_cnt.x,
											repeat_cnt.y,
											LFile_IntUtoMicrons(pFile, delta.x),
											LFile_IntUtoMicrons(pFile, delta.y),
											cellName,
											instanceName);
					fclose(myFile);
				}
			}
		}
		
	}
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F2" /*hotkey*/, 
			"AEPONYX\\PolygonToCSV\nWindow", "PolygonToCSV", NULL /*hotkey category*/);
	return 1;
}
