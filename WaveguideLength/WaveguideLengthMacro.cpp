#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <string>
#include <time.h>
#include <sstream>

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
	
	char print[50];

	/*ASKING FOR THE VARIABLES NEEDED*/
	LDialogItem dialog_items[4];
	//The prompts
	strcpy(dialog_items[0].prompt, "Waveguide width:");
	strcpy(dialog_items[1].prompt, "Taper width:");
	strcpy(dialog_items[2].prompt, "MMI width:");
	strcpy(dialog_items[3].prompt, "Waist:");
	//The values entered in parameters
	strcpy(dialog_items[0].value, "0.450");
	strcpy(dialog_items[1].value, "2.6");
	strcpy(dialog_items[2].value, "8.65");
	strcpy(dialog_items[3].value, "0.05");
	
	//Calls the the dialog box
	if(!LDialog_MultiLineInputBox("Waveguide length: input the variables you need", dialog_items, 4)){
		return;
	}
		
	//Declare variables & assign values from the dialogue box
	double WG_width = atof(dialog_items[0].value);
	double taper_width = atof(dialog_items[1].value);
	double MMI_width = atof(dialog_items[2].value);
	double waist = atof(dialog_items[3].value);
	

	/*SELECTION LIST*/
	double WGLength = 0; //we will add to it for every object (at each loop)
	LUpi_LogMessage("\n\n\n");
	//Cette for loop va chercher tous les objets sélectionné
	LSelection pSelNext = NULL;
	for (LSelection pSel = LSelection_GetList(); pSel; pSel = pSelNext ){
		double width = 0;
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
			
			//Add a log of the type, length and location
			LUpi_LogMessage(LFormat("Torus -> length: %f, center of the Torus: (%f, %f)\n", (2*M_PI*(torusParams.nInnerRadius+torusParams.nOuterRadius)/20000)*(theta)/(360), (double)torusParams.ptCenter.x/10000, (double)torusParams.ptCenter.y/10000));
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

			//Add a log of the type, length and location
			LUpi_LogMessage(LFormat("Pie -> length: %f, center of the Pie: (%f, %f)\n", (2*M_PI*(pieParams.nRadius/20000))*(theta)/(360), (double)pieParams.ptCenter.x/10000, (double)pieParams.ptCenter.y/10000));
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
					LUpi_LogMessage("Taper_");
					taperSide+=1;
				}
				else if(distance==MMI_width){
					LUpi_LogMessage("MMI_");
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
				//Add a log of the type, length and location
				LUpi_LogMessage(LFormat("Polygon -> length: %f, (0,0) point of the object: (%f, %f)\n", (sqrt(pow((double)(newEndPoint.x - newStartPoint.x), 2) + pow((double)(newEndPoint.y - newStartPoint.y), 2)))/10000, (double)newStartPoint.x/10000, (double)newStartPoint.y/10000));
			}
			//other polygon
		 	else{
				distance = 0;
				bool flag = false;
				//Take of the width from the length
				if(WGSide>=2){
					LUpi_LogMessage("WG_");
					distance -= WG_width*2;
				}else if (MMISide>=2){
					distance -= MMI_width*2;
				}else{
					while(flag == false){
						if(width>0){
							distance -= width*2;
							flag = true;
						}else{
							const char* title = LFormat( "No width found for polygon with %i verticies", numberOfPoint);
							//présentera une picklist de tous les côtés s'il y a moins de 7 côtés
							if(numberOfPoint<=6){
								const char* picklist[numberOfPoint];
								if(numberOfPoint>0){
									char msg1[20], msg2[20], msg3[20];
									sprintf (msg1, "%.4lf", (round(sqrt((double)(arr[0].x - arr[numberOfPoint-1].x)*(arr[0].x - arr[numberOfPoint-1].x)+(double)(arr[0].y - arr[numberOfPoint-1].y)*(arr[0].y - arr[numberOfPoint-1].y))))/10000);
									picklist[0] = (char*) msg1;
									sprintf (msg2, "%.4lf", (round(sqrt((double)(arr[1].x - arr[0].x)*(arr[1].x - arr[0].x)+(double)(arr[1].y - arr[0].y)*(arr[1].y - arr[0].y))))/10000);
									picklist[1] = (char*) msg2;
									sprintf (msg3, "%.4lf", (round(sqrt((double)(arr[2].x - arr[1].x)*(arr[2].x - arr[1].x)+(double)(arr[2].y - arr[1].y)*(arr[2].y - arr[1].y))))/10000);
									picklist[2] = (char*) msg3;
								}
								if(numberOfPoint>3){
									char msg4[20];
									sprintf (msg4, "%.4lf", (round(sqrt((double)(arr[3].x - arr[2].x)*(arr[3].x - arr[2].x)+(double)(arr[3].y - arr[2].y)*(arr[3].y - arr[2].y))))/10000);
									picklist[3] = (char*) msg4;
								}
								if(numberOfPoint>4){
									char msg5[20];
									sprintf (msg5, "%.4lf", (round(sqrt((double)(arr[4].x - arr[3].x)*(arr[4].x - arr[3].x)+(double)(arr[4].y - arr[3].y)*(arr[4].y - arr[3].y))))/10000);
									picklist[4] = (char*) msg5;
								}
								if(numberOfPoint==6){
									char msg6[20];
									sprintf (msg6, "%.4lf", (round(sqrt((double)(arr[5].x - arr[4].x)*(arr[5].x - arr[4].x)+(double)(arr[5].y - arr[4].y)*(arr[5].y - arr[4].y))))/10000);
									picklist[5] = (char*) msg6;
								}
								int choice = -1;
								choice = LDialog_PickList (title, picklist, numberOfPoint, 0);
								if(choice == -1)
									return;
								
								width = atof(picklist[choice]);
							//s'il y a plus de 6 côtés on laisse l'utilisateur entrer la grandeur désirée (il s'agit probablement d'un waveguide rasterizer)
							}else{
								char default_value[20];
								strcpy(default_value, "0.450");
								if(!LDialog_InputBox(title, "Value of the width of this object:", default_value)){
									return;
								}
								width = atof(default_value); 
							}
						}
					}
				}

				//calculate the length
				for(int i=0; i<numberOfPoint; i++){
					endPoint = arr[i];
					distance += (round(sqrt((double)(endPoint.x - startPoint.x)*(endPoint.x - startPoint.x)+(double)(endPoint.y - startPoint.y)*(endPoint.y - startPoint.y))))/10000;
					startPoint = arr[i];
    			}

				WGLength += distance/2;
				//Add a log of the type, length and location 
				double cx = 0, cy = 0;
				if(arr[numberOfPoint/4].x < arr[numberOfPoint/4*3].x){
					cx = (double)(arr[numberOfPoint/4].x+abs(arr[numberOfPoint/4].x-arr[numberOfPoint/4*3].x)/2)/10000;
				}else{
					cx = (double)(arr[numberOfPoint/4*3].x+abs(arr[numberOfPoint/4].x-arr[numberOfPoint/4*3].x)/2)/10000;
				}
				if(arr[numberOfPoint/4].y < arr[numberOfPoint/4*3].y){
					cy = (double)(arr[numberOfPoint/4].y+abs(arr[numberOfPoint/4].y-arr[numberOfPoint/4*3].y)/2)/10000;
				}else{
					cy = (double)(arr[numberOfPoint/4*3].y+abs(arr[numberOfPoint/4].y-arr[numberOfPoint/4*3].y)/2)/10000;
				}
				LUpi_LogMessage(LFormat("Polygon -> length: %f, approximate center of the waveguide: (%f, %f)\n", distance/2, cx, cy));
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
				//Add a log of the type, length and location
				LUpi_LogMessage(LFormat("Box -> length: %f, center of the box: (%f, %f)\n", y, (double)box.x0/10000+x/2, (double)box.y0/10000+y/2));
			}
			else if(y==WG_width || y==MMI_width){
				WGLength += x;
				//Add a log of the type, length and location
				LUpi_LogMessage(LFormat("Box -> length: %f, center of the box: (%f, %f)\n", x, (double)box.x0/10000+x/2, (double)box.y0/10000+y/2));
			}
			//if there is no side that correspond to a waveguide or a MMI
			else{
				const char* picklist[2];
				const char* title = LFormat( "Choose which side of the box is the length", 2);
				char msg1[20], msg2[20];

				//list of the two sides of the box
				sprintf (msg1, "%.4lf", x);
				picklist[0] = (char*) msg1;
				sprintf (msg2, "%.4lf", y);
				picklist[1] = (char*) msg2;
					
				//fait choisir l'utilisateur entre l'un des deux côtés
				int choice = -1;
				choice = LDialog_PickList (title, picklist, 2, 0);
				if(choice == -1)
					return;
				
				WGLength += atof(picklist[choice]);
				//Add a log of the type, length and location
				LUpi_LogMessage(LFormat("Box -> length: %f, center of the box: (%f, %f)\n", atof(picklist[choice]), (double)box.x0/10000+x/2, (double)box.y0/10000+y/2));
			}
		}
		//other objects
		else{
			LDialog_AlertBox("Did not detect what kind of object that was");
		}

	
	}
	LDialog_AlertBox(LFormat("Waveguide length: %f", WGLength));
}

int UPI_Entry_Point(void){
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F5" /*hotkey*/, 
			"AEPONYX\\Find the waveguide's length\nWindow", "WaveguideLengthMacro", NULL /*hotkey category*/);
	return 1;
}
