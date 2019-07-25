#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <string>
#include <time.h>

#define EXCLUDE_LEDIT_LEGACY_UPI
#include <ldata.h>
#include <lcomp.h>

extern "C" {
	void setStartPoint(LPoint point);
	void WaveguideLengthMacro();
	int UPI_Entry_Point(void);
}

void WaveguideLengthMacro(){
	/*LOCAL VARIABLES*/
	LCell pCell = LCell_GetVisible();
	LFile pFile = LCell_GetFile(pCell);
	
	if(!pCell)
		return;
	
	LGrid_v16_30 grid;
	LFile_GetGrid_v16_30(pFile, &grid);
	
	//asked
	double WG_width = 0.450;
	double taper_width = 2.6;
	double MMI_width = 8.65;
	double waist = 0.05;

	/*SELECTION LIST*/
	double WGLength = 0; //we will add to it for every object (at each loop)
	
	//Cette for loop va chercher tous les objets sélectionné
	LSelection pSelNext = NULL;
	for (LSelection pSel = LSelection_GetList(); pSel; pSel = pSelNext ){
		pSelNext = LSelection_GetNext(pSel);
		LObject pObj = LSelection_GetObject(pSel);

		//wire
		if (LObject_GetShape(pObj)==LWire){ 
			LDialog_AlertBox("Didn't think you'd need that");
		}
		//circle
		else if (LObject_GetShape(pObj)==LCircle){ 
			LDialog_AlertBox("Why are you trying to get the \"length\" of a circle?");
		}
		//torus
		else if (LObject_GetShape(pObj)==LTorus){
			LTorusParams torusParams;
			LTorus_GetParams(pObj, &torusParams);
			double theta;
			theta = torusParams.dStopAngle - torusParams.dStartAngle; //angle de l'arc de cercle
			if(theta<0){ //si l'angle est négatif, alors on prend l'angle inverse
				theta = 360 + theta;
			}
			WGLength += (2*M_PI*(torusParams.nInnerRadius+torusParams.nOuterRadius)/20000)*(theta)/(360); //on veut la distance moyenne alors on prend le rayon au centre: (ext+int)/2
		}
		//pie
		else if (LObject_GetShape(pObj)==LPie){
			LPieParams pieParams;
			LPie_GetParams(pObj, &pieParams);
			double theta;
			theta = pieParams.dStopAngle - pieParams.dStartAngle;//angle de l'arc de cercle
			if(theta<0){ //si l'angle est négatif, alors on prend l'angle inverse
				theta = 360 + theta;
			}
			WGLength += (2*M_PI*(pieParams.nRadius/20000))*(theta)/(360); //on veut la distance moyenne alors on prend le rayon au centre: ext/2 puisqu'il n'y a pas de rayon intérieur
		}
		//polygon
	 	else if(LObject_GetShape(pObj)==LPolygon){
			//local variables
			LPoint* arr = (LPoint*) malloc (100000);
			int numberOfPoint = 0;
			double distance;
			LPoint startPoint, endPoint, newStartPoint, newEndPoint;
			int WGSide = 0, taperSide = 0, MMISide = 0;

			//get the points and put them in an array
			LVertex vertexNext = NULL;
			for (LVertex vertex = LObject_GetVertexList(pObj); vertex; vertex = vertexNext){
				arr[numberOfPoint] = LVertex_GetPoint(vertex);
				vertexNext = LVertex_GetNext(vertex);
				numberOfPoint++;
			}

			//get the type of polygon (taper, MMI or other waveguide)
			startPoint = arr[numberOfPoint-1];
 			for(int i=0; i<numberOfPoint; i++){
				endPoint = arr[i];
       		 	distance = (round(sqrt((double)(endPoint.x - startPoint.x)*(endPoint.x - startPoint.x)+(double)(endPoint.y - startPoint.y)*(endPoint.y - startPoint.y))))/10000;
				if(distance==WG_width){
					WGSide+=1;
				}
				else if(distance==taper_width){
					taperSide+=1;
				}
				else if(distance==MMI_width){
					MMISide+=1;
				}
				startPoint = arr[i];
    		}
			
			//Is it a taper? (or butterfly MMI)
			if(taperSide==1 && WGSide==1 || MMISide==1){
				double waist_width = MMI_width-waist;
				//trouver le côté de largeur du waveguide et du taper (ou MMI et MMI - waist)
				for(int j=0; j<numberOfPoint; j++){
					endPoint = arr[j];
					distance = (sqrt((double)(endPoint.x - startPoint.x)*(endPoint.x - startPoint.x)+(double)(endPoint.y - startPoint.y)*(endPoint.y - startPoint.y)))/10000;
					//set the startPoint (taper)
					if(distance == WG_width){
						if((double)endPoint.x == (double)startPoint.x){
							newStartPoint.x = startPoint.x;
							if((double)startPoint.y<(double)endPoint.y){
								newStartPoint.y = (double)startPoint.y + (10000 * WG_width/2);
							}else{
								newStartPoint.y = (double)endPoint.y + (10000 * WG_width/2);
							}
						}
						else if((double)endPoint.y == (double)startPoint.y){
							newStartPoint.y = startPoint.y;
							if((double)startPoint.x<(double)endPoint.x){
								newStartPoint.x = (double)startPoint.x + (10000 * WG_width/2);
							}else{
								newStartPoint.x = (double)endPoint.x + (10000 * WG_width/2);
							}
						}
					}
					//set the endPoint (taper)
					else if(distance == taper_width){
						if((double)endPoint.x == (double)startPoint.x){
							newEndPoint.x = endPoint.x;
							if((double)startPoint.y<(double)endPoint.y){
								newEndPoint.y = (double)startPoint.y + (10000 * taper_width/2);
							}else{
								newEndPoint.y = (double)endPoint.y + (10000 * taper_width/2);
							}
						}
						else if((double)endPoint.y == (double)startPoint.y){
							newEndPoint.y = endPoint.y;
							if((double)startPoint.x<(double)endPoint.x){
								newEndPoint.x = (double)startPoint.x + (10000 * taper_width/2);
							}else{
								newEndPoint.x = (double)endPoint.x + (10000 * taper_width/2);
							}
						}
					}
					//set the startPoint (BMMI)
					else if(distance == MMI_width){
						if((double)endPoint.x == (double)startPoint.x){
							newStartPoint.x = startPoint.x;
							if((double)startPoint.y<(double)endPoint.y){
								newStartPoint.y = (double)startPoint.y + (10000 * MMI_width/2);
							}else{
								newStartPoint.y = (double)endPoint.y + (10000 * MMI_width/2);
							}
						}
						else if((double)endPoint.y == (double)startPoint.y){
							newStartPoint.y = startPoint.y;
							if((double)startPoint.x<(double)endPoint.x){
								newStartPoint.x = (double)startPoint.x + (10000 * MMI_width/2);
							}else{
								newStartPoint.x = (double)endPoint.x + (10000 * MMI_width/2);
							}
						}
					}
					//set the endPoint (BMMI)
					else if(distance == waist_width){
						if((double)endPoint.x == (double)startPoint.x){
							newEndPoint.x = endPoint.x;
							if((double)startPoint.y<(double)endPoint.y){
								newEndPoint.y = (double)startPoint.y + (10000 * waist_width/2);
							}else{
								newEndPoint.y = (double)endPoint.y + (10000 * waist_width/2);
							}
						}
						else if((double)endPoint.y == (double)startPoint.y){
							newEndPoint.y = endPoint.y;
							if((double)startPoint.x<(double)endPoint.x){
								newEndPoint.x = (double)startPoint.x + (10000 * waist_width/2);
							}else{
								newEndPoint.x = (double)endPoint.x + (10000 * waist_width/2);
							}
						}
					}
					startPoint = arr[j];
				}
				WGLength += (sqrt(pow((double)(newEndPoint.x - newStartPoint.x), 2) + pow((double)(newEndPoint.y - newStartPoint.y), 2)))/10000;
			}
			//other polygon
		 	else{
				distance = 0;
				bool flag = false;
				while(flag == false){
					if(WGSide>=2){
						distance -= WG_width*2;
						flag = true;
					}else if (MMISide>=2){
						distance -= MMI_width*2;
						flag = true;
					}else{
						LUpi_LogMessage("This polygon with x verticies does not have a side that corresponds to a waveguide or a MMI\n"); //changing values LDialogBox
						flag = true;
					}
				}
				for(int i=0; i<numberOfPoint; i++){
					
					endPoint = arr[i];
					distance += (round(sqrt((double)(endPoint.x - startPoint.x)*(endPoint.x - startPoint.x)+(double)(endPoint.y - startPoint.y)*(endPoint.y - startPoint.y))))/10000;
					startPoint = arr[i];
    			}
				WGLength += distance/2;
			}
			//free 
			free(arr);
		}
		//box
		else if(LObject_GetShape(pObj)==LBox){
			LRect box = LBox_GetRect(pObj);
			double x = abs(box.x1-box.x0);
			x = x/10000;
			double y = abs(box.y1-box.y0);
			y = y/10000;
			if(x==WG_width || x==MMI_width){
				WGLength += y;
			}
			else if(y==WG_width || y==MMI_width){
				WGLength += x;
			}
		}
		//other objects
		else{
			LDialog_AlertBox("Did not detect what kind of object that was");
		}

	LDialog_AlertBox(LFormat("waveguide length: %f\n\n", WGLength));
	}
}

int UPI_Entry_Point(void){
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F5" /*hotkey*/, 
			"AEPONYX\\Find the waveguide's length\nWindow", "WaveguideLengthMacro", NULL /*hotkey category*/);
	return 1;
}