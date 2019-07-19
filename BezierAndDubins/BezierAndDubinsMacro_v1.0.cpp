#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include <string>

#include <ldata.h>
#include <unistd.h> //getcwd

#define MAX_LENGTH_PATH 256

#define EXCLUDE_LEDIT_LEGACY_UPI //This statement make the C language macros, which are now superseded by C++ functions, unavailable.

extern "C" {
    #include "DubinsPath.hpp"
    #include "DubinsPoint.hpp"
    #include "BezierCurve.hpp"

    #include "DubinsPath.cpp"
    #include "DubinsPoint.cpp"
    #include "BezierCurve.cpp"

    bool twoLabelsHasBeenSelected(void);
    void BezierAndDubinsMacro(void);
	int UPI_Entry_Point(void);
}

bool twoLabelsHasBeenSelected()
{
    char strName[MAX_CELL_NAME];
    LSelection pSelection = LSelection_GetList();
    if(pSelection == NULL)
        return false;
    else
    {
        int cpt = 0;
        for(pSelection = LSelection_GetList() ; pSelection != NULL; pSelection = LSelection_GetNext(pSelection) )
        {
            LObject object = LSelection_GetObject(pSelection);
            if(LObject_GetShape(object) == LObjLabel)
            {
                cpt = cpt +1;
                LUpi_LogMessage( LFormat("%s\n",LLabel_GetName( (LLabel)object, strName, MAX_CELL_NAME ) ) );
            }
            else
            {
                LUpi_LogMessage( "A non label object has been selected ==> manual selection\n" );
                return false;
            }
        }
        if(cpt == 2)
            return true;
        else
        {
            LUpi_LogMessage( LFormat("Need to select exactly 2 labels: %d selected ==> manual selection\n",cpt) );
            return false;
        }
    }
}

void BezierAndDubinsMacro()
{
    LUpi_LogMessage( "Macro DEBUT\n" );

    DubinsPath path;
    BezierCurve bezierCurve;
    DubinsPoint start, end;

    LCell	pCell	=	LCell_GetVisible();
	 LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;
    char sLayerName[MAX_LAYER_NAME];
    char strLayer[MAX_LAYER_NAME];

    char cwd[MAX_TDBFILE_NAME];
	char strPath[MAX_LENGTH_PATH];
    char* token; //to read the file
   	FILE * myFile = NULL;
    char line[256];

    int cpt = 0;
    int i = 0;

    char value_offset[50];

    //to find the points locations
    LPoint pLabelLocation;
    double xPosLabel, yPosLabel;
    char startLabelName[MAX_CELL_NAME];
    char endLabelName[MAX_CELL_NAME];
    char sLabelName[MAX_CELL_NAME];

    char startCellName[MAX_CELL_NAME];
    char endCellName[MAX_CELL_NAME];

    double dAngle;
    int nmbLabel = 0;
    float radius;
    float width;
    double paramBezier;

    int rasterizeWaveguide = 1;

/*<<<<<<< HEAD
    LDialogItem DialogItems[2] = {{ "Oxide size on each size (in microns)","5"}, { "Oxide layer","OX"}};/*
=======
    LDialogItem DialogItems[2] = {{ "Oxide size on each side (in microns)","10"}, { "Oxide layer","OX"}};
>>>>>>> refs/remotes/origin/master*/

    const char *Pick_List [ ] = {
    "Dubins curves with circles",
    "Dubins curves with Bezier curves",
    "Bezier curves"
    };
    int nbChoice = 3;
    int choice = -1;
    choice = LDialog_PickList ("Which type of curve?", Pick_List, nbChoice, 0);

    if(choice == -1)
        return;
	
	
	strcpy(strLayer, "WG"); //preloaded text for the dialog box
	
	//Make the list of the items for the MultiLineDialogBox
	LDialogItem dialog_items[10];
	//The prompts
	strcpy(dialog_items[0].prompt, "Layer name of the active cell:");
	//The values entered in parameters
	strcpy(dialog_items[0].value, strLayer); 
	/* To be continued lower according to the selection made in the first dialogBox */
	
	//set the overlay layers for the DubinsPath object
	path.SetWGGROW003Layer(LLayer_Find(pFile, "WGGROW003"));
	path.SetWGOVL010Layer(LLayer_Find(pFile, "WGOVL010"));
	path.SetWGOVLHOLELayer(LLayer_Find(pFile, "WGOVLHOLE"));
	//set the overlay layers for the BezierCurve object
	bezierCurve.SetWGGROW003LayerBezier(LLayer_Find(pFile, "WGGROW003"));
	bezierCurve.SetWGOVL010LayerBezier(LLayer_Find(pFile, "WGOVL010"));
	bezierCurve.SetWGOVLHOLELayerBezier(LLayer_Find(pFile, "WGOVLHOLE"));
	
    //get the path
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
	else
        LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));

    path.SetFile(pFile);
    path.SetCell(pCell);


    if(choice == 0){ //Dubins curves with circles
        LUpi_LogMessage( "\n\nDubins curve with circle\n\n" );

        path.SetOffsetCurveIsSelected(true); //offset?
        strcpy(value_offset, "0.0");
		
		//The prompts
		strcpy(dialog_items[1].prompt, "Offset value (in microns):");
		strcpy(dialog_items[2].prompt, "Oxide size on each size (in microns):");
		strcpy(dialog_items[3].prompt, "Oxide layer:");
		strcpy(dialog_items[4].prompt, "WGGROW003 width:");
		strcpy(dialog_items[5].prompt, "WGOVL010  width:");
		strcpy(dialog_items[6].prompt, "WGOVLHOLE width:");
		strcpy(dialog_items[7].prompt, "Radius of the circles (in microns):");
		strcpy(dialog_items[8].prompt, "Width of the guide (in microns):");
		strcpy(dialog_items[9].prompt, "Do you want to rasterize the waveguide?");
		//The values entered in parameters
		strcpy(dialog_items[1].value, value_offset);
		strcpy(dialog_items[2].value, "5");
		strcpy(dialog_items[3].value, "OX");
		strcpy(dialog_items[4].value, "3");
		strcpy(dialog_items[5].value, "10");
		strcpy(dialog_items[6].value, "1.5");
		strcpy(dialog_items[7].value, "100");
		strcpy(dialog_items[8].value, "0.45");
		strcpy(dialog_items[9].value, "1");

		//Calls the the dialog box
		if(!LDialog_MultiLineInputBox("Dubins curves with circles", dialog_items, 10)){
			return;
		}
		
		//Declare variables & assign values from the dialogue box
		pLayer = LLayer_Find(pFile, dialog_items[0].value); //the layer value
		strcpy(value_offset, dialog_items[1].value);
		radius = atof(dialog_items[7].value);
		width = atof(dialog_items[8].value);
		rasterizeWaveguide = LFile_MicronsToIntU(pFile, atoi(dialog_items[9].value)); 
		
		//set the layer for the DubinsPath object
		path.SetLayer(pLayer);
		
		//the offset value
        path.SetOffsetValue( atof(value_offset) ); 
        if(atof(value_offset) == 0.0) 
            path.SetOffsetCurveIsSelected(false); //store in the dubinsPath object
		else
            path.SetOffsetCurveIsSelected(true); //store in the dubinsPath object

		//the oxide layer and size
		path.SetOxideSizeValue(2*atof(dialog_items[2].value)); //store in the dubinsPath object
		if(LLayer_Find(pFile, dialog_items[3].value)){
			path.SetOxideLayer(LLayer_Find(pFile, dialog_items[3].value));
		}else{
			LDialog_AlertBox("Oxide layer could not be found, oxide will not be generated");
		}
        
		//WGGROW003 width
		path.SetWGGROW003SizeValue(2*atof(dialog_items[4].value)); //store in the dubinsPath object
		//WGGROW010 width
		path.SetWGOVL010SizeValue(2*atof(dialog_items[5].value)); //store in the dubinsPath object
		//WGOVLHOLE width
		path.SetWGOVLHOLESizeValue(2*atof(dialog_items[6].value)); //store in the dubinsPath object
		
		//catch some errors
		if(NotAssigned(pLayer)){ //the layer is not found
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);


        if(twoLabelsHasBeenSelected()){ //2 labels selected : we will store the positions and angles
            LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
            LSelection pSelection = LSelection_GetList() ;

            LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
            LLabel pLabel = (LLabel)object;

            //endLabel
            pLabelLocation = LLabel_GetPosition(pLabel);
            xPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.y);
            end.SetPoint(xPosLabel, yPosLabel, pFile);
            if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK){
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK){
                    end.SetAngleDegre(dAngle);
                }else{
                    end.SetAngleDegre(0);
                    LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                }
            }else{
                end.SetAngleDegre(0);
                LUpi_LogMessage("Angle property not found, 0 by default\n");
            }
			
			//get the second label
            pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
            object = LSelection_GetObject(pSelection);
            pLabel = (LLabel)object;

            //startLabel
            pLabelLocation = LLabel_GetPosition(pLabel);
            xPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.y);
            start.SetPoint(xPosLabel, yPosLabel, pFile);
            if(LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK){
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK){
                    start.SetAngleDegre(dAngle);
                }else{
                    start.SetAngleDegre(0);
                    LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                }
            }else{
                start.SetAngleDegre(0);
                LUpi_LogMessage("Angle property not found, 0 by default\n");
            }

            //store in the dubinsPath object
            path.SetStartPoint(start);
            path.SetEndPoint(end);
			path.SetRadius(radius);
			path.SetGuideWidth(width);
            
            if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
                path.DrawLine();
            else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
                if(start.GetAngleRadian() == end.GetAngleRadian())
                    LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
                else
                    LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
            else{
                path.ComputeDubinsPaths();
                path.RasterizePath(rasterizeWaveguide);
            }
			
			//longueur du waveguide
			float WGLength = path.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
            return; //fin de programme
        }


        //if not 2 label selected, try with the file
        strcpy(strPath,"guideFile.csv");
        LDialog_File( strPath, "CSV file", strPath, "CSV Files (*.csv)|*.csv|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", 1, "Enter path of the CSV file containing the guides between labels", "OK", "csv", "*.csv|*.txt||", pFile );
        strPath[strlen(strPath)-2]='\0'; //delete the last 2 char of the string ("|0")
        
        if (strPath != NULL)
        {
            LFile_GetResolvedFileName( pFile, strPath, cwd, MAX_TDBFILE_NAME ); //get the absolute path of strPath in cwd
            myFile = fopen(cwd,"r");
        }
        
        if(myFile != NULL)
        {
            while(!feof(myFile))
            {
                nmbLabel = 0;
                // reads text until new line 
                fscanf(myFile,"%[^\n]", line);
                LUpi_LogMessage(LFormat("Read line: %s\n", line));
                cpt=0;
                for(i=0; i<strlen(line);i++)
                {
                    if(line[i]==',') //count the number of ',' to know what kind of shape it is
                        cpt++;
                }

                token = strtok(line, ","); //first string before the first ',' of line

                if(cpt == 6) //guide,cell1,P1,cell2,P2,radius,width
                {
                    token = strtok(NULL, ","); //next token
                    strcpy(startCellName,token);
                    token = strtok(NULL, ",");
                    strcpy(startLabelName,token);
                    token = strtok(NULL, ",");
                    strcpy(endCellName,token);
                    token = strtok(NULL, ",");
                    strcpy(endLabelName,token);
                    token = strtok(NULL, ",");
                    radius = atof(token);
                    token = strtok(NULL, ",");
                    width = atof(token);
                }
                LCell startCell;
                if(strcmp(startCellName," ")==0)
                    startCell = pCell;
                else
                    startCell = LCell_Find( pFile, startCellName );
                if(startCell == NULL)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\nInstruction will not be executed \n", startCellName) );
                    continue;
                }

                for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
                {
                    LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                    
                    if(strcmp(sLabelName, startLabelName) == 0)
                    {
                        pLabelLocation = LLabel_GetPosition( pLabel );
                        xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                        yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                        start.SetPoint(xPosLabel , yPosLabel, pFile);

                        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                        {
                            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                            {
                                start.SetAngleDegre( dAngle );
                            }	
                            else
                            {
                                start.SetAngleDegre( 0 );
                                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                            }
                                
                        }		
                        else
                        {
                            start.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle property not found, 0 by default\n");
                        }
                        nmbLabel++;
                        break;
                    }
                    
                }
                if(nmbLabel != 1)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\nInstruction will not be executed \n",startLabelName, startCellName) );
                    continue;
                }
                
                LCell endCell;
                if(strcmp(endCellName," ")==0)
                    endCell = pCell;
                else
                    endCell = LCell_Find( pFile, endCellName );

                if(endCell == NULL)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\nInstruction will not be executed \n",endCellName) );
                    continue;
                }

                for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
                {
                    LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                    
                    if(strcmp(sLabelName, endLabelName) == 0)
                    {
                        pLabelLocation = LLabel_GetPosition( pLabel );
                        xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                        yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                        end.SetPoint(xPosLabel , yPosLabel, pFile);

                        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                        {
                            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                            {
                                end.SetAngleDegre( dAngle );
                            }	
                            else
                            {
                                end.SetAngleDegre( 0 );
                                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                            }
                        }		
                        else
                        {
                            end.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle property not found, 0 by default\n");
                        }
                        nmbLabel++;
                        break;
                    }
                }

                if(nmbLabel != 2)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\nInstruction will not be executed \n",endLabelName, endCellName) );
                    continue;
                }

                path.SetStartPoint(start);
                path.SetEndPoint(end);
                path.SetRadius(radius);
                path.SetGuideWidth(width);
                
                if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
                    path.DrawLine();
                else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
                    if(start.GetAngleRadian() == end.GetAngleRadian())
                        LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
                    else
                        LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
                else
                {
                    path.ComputeDubinsPaths();
                    path.RasterizePath(rasterizeWaveguide);
                }

                fscanf(myFile,"\n"); //got to the next line
            }
            fclose(myFile);
			
			//longueur du waveguide
			float WGLength = path.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
		}



        else //manual selection (= no label selected and no file found/selected)
        {
            LDialog_AlertBox(LFormat("No file found: manual selection"));
            LDialogItem DialogItems[2] = {{ "Cell","cell1"}, { "Name","P1"}};
            if (LDialog_MultiLineInputBox("Start point",DialogItems,2) == 0)
                return;
            strcpy(startCellName,DialogItems[0].value);
            strcpy(startLabelName,DialogItems[1].value);

            LCell startCell = LCell_Find( pFile, startCellName );
            if(startCell == NULL)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n", startCellName) );
                return;
            }

            for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
            {
                LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                
                if(strcmp(sLabelName, startLabelName) == 0)
                {
                    pLabelLocation = LLabel_GetPosition( pLabel );
                    xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                    yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                    start.SetPoint(xPosLabel , yPosLabel, pFile);

                    if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                    {
                        if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                        {
                            start.SetAngleDegre( dAngle );
                        }	
                        else
                        {
                            start.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                        }
                            
                    }		
                    else
                    {
                        start.SetAngleDegre( 0 );
                        LUpi_LogMessage("Angle property not found, 0 by default\n");
                    }
                    nmbLabel++;
                    break;
                }
            }
            if(nmbLabel != 1)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",startLabelName, startCellName) );
                return;
            }



            LDialogItem DialogItemsEnd[2] = {{ "Cell","cell2"}, { "Name","P2"}};
            if (LDialog_MultiLineInputBox("End point",DialogItemsEnd,2) == 0)
                return;
            strcpy(endCellName,DialogItemsEnd[0].value);
            strcpy(endLabelName,DialogItemsEnd[1].value);

            LCell endCell = LCell_Find( pFile, endCellName );
            if(endCell == NULL)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n",endCellName) );
                return;
            }

            for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
            {
                LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                
                if(strcmp(sLabelName, endLabelName) == 0)
                {
                    pLabelLocation = LLabel_GetPosition( pLabel );
                    xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                    yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                    end.SetPoint(xPosLabel , yPosLabel, pFile);

                    if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                    {
                        if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                        {
                            end.SetAngleDegre( dAngle );
                        }	
                        else
                        {
                            end.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                        }
                            
                    }		
                    else
                    {
                        end.SetAngleDegre( 0 );
                        LUpi_LogMessage("Angle property not found, 0 by default\n");
                    }
                    nmbLabel++;
                    break;
                }
            }

            if(nmbLabel != 2)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",endLabelName, endCellName) );
                return;
            }

            path.SetStartPoint(start);
            path.SetEndPoint(end);
            path.SetRadius(radius);
            path.SetGuideWidth(width);
            
            if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
                path.DrawLine();
            else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
                if(start.GetAngleRadian() == end.GetAngleRadian())
                    LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
                else
                    LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
            else
            {
                path.ComputeDubinsPaths();
                path.RasterizePath(rasterizeWaveguide);
            }
			
			//longueur du waveguide
			float WGLength = path.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
        }
    }


    else if(choice == 1){ //Dubins curve with Bézier
        LUpi_LogMessage( "\n\nDubins curve with Bézier\n\n" );

        strcpy(strLayer, "0.3");
		
		//The prompts
		strcpy(dialog_items[1].prompt, "Bezier parameter (between 0 and 1):");
		strcpy(dialog_items[2].prompt, "Oxide size on each size (in microns):");
		strcpy(dialog_items[3].prompt, "Oxide layer:");
		strcpy(dialog_items[4].prompt, "WGGROW003 width:");
		strcpy(dialog_items[5].prompt, "WGOVL010  width:");
		strcpy(dialog_items[6].prompt, "WGOVLHOLE width:");
		strcpy(dialog_items[7].prompt, "Radius of the circles (in microns):");
		strcpy(dialog_items[8].prompt, "Width of the guide (in microns):");
		//The values entered in parameters
		strcpy(dialog_items[1].value, strLayer);
		strcpy(dialog_items[2].value, "5");
		strcpy(dialog_items[3].value, "OX");
		strcpy(dialog_items[4].value, "3");
		strcpy(dialog_items[5].value, "10");
		strcpy(dialog_items[6].value, "1.5");
		strcpy(dialog_items[7].value, "100");
		strcpy(dialog_items[8].value, "0.435");
		
		//Calls the the dialog box
		if(!LDialog_MultiLineInputBox("Dubins curves with Bezier", dialog_items, 9)){
			return;
		}
		
		//Declare variables & assign values from the dialogue box
		pLayer = LLayer_Find(pFile, dialog_items[0].value); //the layer value
		strcpy(strLayer, dialog_items[1].value);
		radius = atof(dialog_items[7].value);
		width = atof(dialog_items[8].value);
		
		//set the layer for DubinsPath
		path.SetLayer(pLayer);
		
		//Bezier parameter
		paramBezier = atof(strLayer);
		path.SetParamBezier(paramBezier);    

		//the oxide layer and size
		path.SetOxideSizeValue(2*atof(dialog_items[2].value));
		if(LLayer_Find(pFile, dialog_items[3].value)){
			path.SetOxideLayer(LLayer_Find(pFile, dialog_items[3].value));
		}else{
			LDialog_AlertBox("Oxide layer could not be found, oxide will not be generated");
		}
		
		//WGGROW003 width
		path.SetWGGROW003SizeValue(2*atof(dialog_items[4].value));
		//WGGROW010 width
		path.SetWGOVL010SizeValue(2*atof(dialog_items[5].value));
		//WGOVLHOLE width
		path.SetWGOVLHOLESizeValue(2*atof(dialog_items[6].value));
		
		//catch some errors
		if(NotAssigned(pLayer)){ //the layer is not found
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);


        if(twoLabelsHasBeenSelected()){
            LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
            LSelection pSelection = LSelection_GetList() ;

            LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
            LLabel pLabel = (LLabel)object;
			
			//endLabel
            pLabelLocation = LLabel_GetPosition(pLabel);
            xPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.y);
            end.SetPoint(xPosLabel, yPosLabel, pFile);
            if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK){
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK){
                    end.SetAngleDegre(dAngle);
                }else{
                    end.SetAngleDegre(0);
                    LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                }
            }else{
                end.SetAngleDegre(0);
                LUpi_LogMessage("Angle property not found, 0 by default\n");
            }
			
			//get the second label
            pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
            object = LSelection_GetObject(pSelection);
            pLabel = (LLabel)object;
			
			//startLabel
            pLabelLocation = LLabel_GetPosition(pLabel);
            xPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.y);
            start.SetPoint(xPosLabel, yPosLabel, pFile);
            if(LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK){
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK){
                    start.SetAngleDegre(dAngle);
                }else{
                    start.SetAngleDegre(0);
                    LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                }
            }else{
                start.SetAngleDegre(0);
                LUpi_LogMessage("Angle property not found, 0 by default\n");
            }

            //store in the dubinsPath object
            path.SetStartPoint(start);
            path.SetEndPoint(end);
            path.SetRadius(radius);
            path.SetGuideWidth(width);
            
            if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
                path.DrawLine();
            else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
                if(start.GetAngleRadian() == end.GetAngleRadian())
                    LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
                else
                    LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
            else{
                path.ComputeDubinsPaths();
                path.DubinsPathWithBezierCurvesCall();
            }
			
			//longueur du waveguide
			float WGLength = path.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
            return; //fin de programme
        }


        strcpy(strPath,"guideFile.csv");
        LDialog_File( strPath, "CSV file", strPath, "CSV Files (*.csv)|*.csv|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", 1, "Enter path of the CSV file containing the guides between labels", "OK", "csv", "*.csv|*.txt||", pFile );
        strPath[strlen(strPath)-2]='\0'; //delete the last 2 char of the string ("|0")
        
        if (strPath != NULL)
        {
            LFile_GetResolvedFileName( pFile, strPath, cwd, MAX_TDBFILE_NAME ); //get the absolute path of strPath in cwd
            myFile = fopen(cwd,"r");
        }
        
        if(myFile != NULL)
        {
            while(!feof(myFile))
            {
                nmbLabel = 0;
                // reads text until new line 
                fscanf(myFile,"%[^\n]", line);
                LUpi_LogMessage(LFormat("Read line: %s\n", line));
                cpt=0;
                for(i=0; i<strlen(line);i++)
                {
                    if(line[i]==',') //count the number of ',' to know what kind of shape it is
                        cpt++;
                }

                token = strtok(line, ","); //first string before the first ',' of line

                if(cpt == 6) //guide,cell1,P1,cell2,P2,radius,width
                {
                    token = strtok(NULL, ","); //next token
                    strcpy(startCellName,token);
                    token = strtok(NULL, ",");
                    strcpy(startLabelName,token);
                    token = strtok(NULL, ",");
                    strcpy(endCellName,token);
                    token = strtok(NULL, ",");
                    strcpy(endLabelName,token);
                    token = strtok(NULL, ",");
                    radius = atof(token);
                    token = strtok(NULL, ",");
                    width = atof(token);
                }
LUpi_LogMessage(LFormat("\nstartCellName %s\n", startCellName));
LUpi_LogMessage(LFormat("startLabelName %s\n", startLabelName));
LUpi_LogMessage(LFormat("endCellName %s\n", endCellName));
LUpi_LogMessage(LFormat("endLabelName %s\n\n", endLabelName));
                LCell startCell;
                if(strcmp(startCellName," ")==0)
                    startCell = pCell;
                else
                    startCell = LCell_Find( pFile, startCellName );
                if(startCell == NULL)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\nInstruction will not be executed \n", startCellName) );
                    continue;
                }

                for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
                {
                    LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                    
                    if(strcmp(sLabelName, startLabelName) == 0)
                    {
                        pLabelLocation = LLabel_GetPosition( pLabel );
                        xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                        yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                        start.SetPoint(xPosLabel , yPosLabel, pFile);

                        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                        {
                            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                            {
                                start.SetAngleDegre( dAngle );
                            }	
                            else
                            {
                                start.SetAngleDegre( 0 );
                                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                            }
                                
                        }		
                        else
                        {
                            start.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle property not found, 0 by default\n");
                        }
                        nmbLabel++;
                        break;
                    }
                    
                }
                if(nmbLabel != 1)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\nInstruction will not be executed \n",startLabelName, startCellName) );
                    continue;
                }

                LCell endCell;
                if(strcmp(endCellName," ")==0)
                    endCell = pCell;
                else
                    endCell = LCell_Find( pFile, endCellName );

                if(endCell == NULL)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\nInstruction will not be executed \n",endCellName) );
                    continue;
                }

                for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
                {
                    LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                    
                    if(strcmp(sLabelName, endLabelName) == 0)
                    {
                        pLabelLocation = LLabel_GetPosition( pLabel );
                        xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                        yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                        end.SetPoint(xPosLabel , yPosLabel, pFile);

                        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                        {
                            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                            {
                                end.SetAngleDegre( dAngle );
                            }	
                            else
                            {
                                end.SetAngleDegre( 0 );
                                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                            }
                        }		
                        else
                        {
                            end.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle property not found, 0 by default\n");
                        }
                        nmbLabel++;
                        break;
                    }
                }

                if(nmbLabel != 2)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\nInstruction will not be executed \n",endLabelName, endCellName) );
                    continue;
                }

                path.SetStartPoint(start);
                path.SetEndPoint(end);
                path.SetRadius(radius);
                path.SetGuideWidth(width);
                
                if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
                    path.DrawLine();
                else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
                    if(start.GetAngleRadian() == end.GetAngleRadian())
                        LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
                    else
                        LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
                else
                {
                    path.ComputeDubinsPaths();
                    path.DubinsPathWithBezierCurvesCall();
                }

                fscanf(myFile,"\n"); //got to the next line
            }
			
			//longueur du waveguide
			float WGLength = path.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
            fclose(myFile);
        }



        else //manual selection
        {
	
            LDialog_AlertBox(LFormat("No file found: manual selection"));
            LDialogItem DialogItems[2] = {{ "Cell","cell1"}, { "Name","P1"}};
            if (LDialog_MultiLineInputBox("Start point",DialogItems,2) == 0)
                return;
            strcpy(startCellName,DialogItems[0].value);
            strcpy(startLabelName,DialogItems[1].value);

            LCell startCell = LCell_Find( pFile, startCellName );
            if(startCell == NULL)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n", startCellName) );
                return;
            }

            for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
            {
                LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                
                if(strcmp(sLabelName, startLabelName) == 0)
                {
                    pLabelLocation = LLabel_GetPosition( pLabel );
                    xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                    yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                    start.SetPoint(xPosLabel , yPosLabel, pFile);

                    if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                    {
                        if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                        {
                            start.SetAngleDegre( dAngle );
                        }	
                        else
                        {
                            start.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                        }
                            
                    }		
                    else
                    {
                        start.SetAngleDegre( 0 );
                        LUpi_LogMessage("Angle property not found, 0 by default\n");
                    }
                    nmbLabel++;
                    break;
                }
            }
            if(nmbLabel != 1)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",startLabelName, startCellName) );
                return;
            }



            LDialogItem DialogItemsEnd[2] = {{ "Cell","cell2"}, { "Name","P2"}};
            if (LDialog_MultiLineInputBox("End point",DialogItemsEnd,2) == 0)
                return;
            strcpy(endCellName,DialogItemsEnd[0].value);
            strcpy(endLabelName,DialogItemsEnd[1].value);

            LCell endCell = LCell_Find( pFile, endCellName );
            if(endCell == NULL)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n",endCellName) );
                return;
            }

            for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
            {
                LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                
                if(strcmp(sLabelName, endLabelName) == 0)
                {
                    pLabelLocation = LLabel_GetPosition( pLabel );
                    xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                    yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                    end.SetPoint(xPosLabel , yPosLabel, pFile);

                    if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                    {
                        if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                        {
                            end.SetAngleDegre( dAngle );
                        }	
                        else
                        {
                            end.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                        }
                            
                    }		
                    else
                    {
                        end.SetAngleDegre( 0 );
                        LUpi_LogMessage("Angle property not found, 0 by default\n");
                    }
                    nmbLabel++;
                    break;
                }
            }

            if(nmbLabel != 2)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",endLabelName, endCellName) );
                return;
            }

            path.SetStartPoint(start);
            path.SetEndPoint(end);
            path.SetRadius(radius);
            path.SetGuideWidth(width);
            
            if(start.GetAngleRadian() == end.GetAngleRadian() && atan2(end.GetPoint().y-start.GetPoint().y , end.GetPoint().x-start.GetPoint().x) == start.GetAngleRadian()) //draw only a ligne
                path.DrawLine();
            else if(start.GetPoint().x == end.GetPoint().x && start.GetPoint().y == end.GetPoint().y)
                if(start.GetAngleRadian() == end.GetAngleRadian())
                    LUpi_LogMessage( LFormat("Warning: start and end points are identical\n") );
                else
                    LDialog_AlertBox(LFormat("ERROR:  start and end points are identical but with a different angle."));
            else
            {
                path.ComputeDubinsPaths();
                path.DubinsPathWithBezierCurvesCall();
            }
			
			//longueur du waveguide
			float WGLength = path.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
        }
    }


    else if(choice == 2){ //Bezier curves
        LUpi_LogMessage( "\n\nBezier curve\n\n" );

		//The prompts
		strcpy(dialog_items[1].prompt, "Oxide size on each size (in microns):");
		strcpy(dialog_items[2].prompt, "Oxide layer:");
		strcpy(dialog_items[3].prompt, "WGGROW003 width:");
		strcpy(dialog_items[4].prompt, "WGOVL010  width:");
		strcpy(dialog_items[5].prompt, "WGOVLHOLE width:");
		strcpy(dialog_items[6].prompt, "Width of the guide (in microns):");
		strcpy(dialog_items[7].prompt, "Bezier parameter (between 0 and 1):");
		//The values entered in parameters
		strcpy(dialog_items[1].value, "5");
		strcpy(dialog_items[2].value, "OX");
		strcpy(dialog_items[3].value, "3");
		strcpy(dialog_items[4].value, "10");
		strcpy(dialog_items[5].value, "1.5");
		strcpy(dialog_items[6].value, "0.435");
		strcpy(dialog_items[7].value, "0.3");
		
		//Calls the the dialog box
		if(!LDialog_MultiLineInputBox("Bezier curves", dialog_items, 8)){
			return;
		}
		
		//Declare variables & assign values from the dialogue box
		pLayer = LLayer_Find(pFile, dialog_items[0].value); //the layer value
		width = atof(dialog_items[7].value);
		paramBezier = atof(dialog_items[8].value);
	
		//WGGROW003 width, WGGROW010 width, WGOVLHOLE width
		bezierCurve.SetWGGROW003SizeValueBezier(2*atof(dialog_items[4].value));
		bezierCurve.SetWGOVL010SizeValueBezier(2*atof(dialog_items[5].value));
		bezierCurve.SetWGOVLHOLESizeValueBezier(2*atof(dialog_items[6].value));
		
		//the oxide layer and size
		bezierCurve.SetOxideSizeValueBezier(2*atof(dialog_items[1].value));
		if(LLayer_Find(pFile, dialog_items[2].value)){
			bezierCurve.SetOxideLayerBezier(LLayer_Find(pFile, dialog_items[2].value));
		}else{
			LDialog_AlertBox("Oxide layer could not be found, oxide will not be generated");
		}
        
		//catch some errors
		if(NotAssigned(pLayer)){ //the layer is not found
			LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
			return;
		}
		LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);
		
		
        if(twoLabelsHasBeenSelected()){
            LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
            LSelection pSelection = LSelection_GetList() ;

            LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
            LLabel pLabel = (LLabel)object;
			
			//endLabel
            pLabelLocation = LLabel_GetPosition( pLabel );
            xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);
            end.SetPoint(xPosLabel , yPosLabel, pFile);
            if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK){
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK){
                    end.SetAngleDegre( dAngle );
                }else{
                    end.SetAngleDegre( 0 );
                    LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                }
            }else{
                end.SetAngleDegre( 0 );
                LUpi_LogMessage("Angle property not found, 0 by default\n");
            }
			
			//get the second label
            pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
            object = LSelection_GetObject(pSelection);
            pLabel = (LLabel)object;
			
			//startLabel
            pLabelLocation = LLabel_GetPosition(pLabel);
            xPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.x);
            yPosLabel = LFile_IntUtoMicrons(pFile, pLabelLocation.y);
            start.SetPoint(xPosLabel, yPosLabel, pFile);
            if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK){
                if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK){
                    start.SetAngleDegre( dAngle );
                }else{
                    start.SetAngleDegre( 0 );
                    LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                }
            }else{
                start.SetAngleDegre( 0 );
                LUpi_LogMessage("Angle property not found, 0 by default\n");
            }


            bezierCurve.SetFile(pFile);
            bezierCurve.SetCell(pCell);
            bezierCurve.SetLayer(pLayer);
            bezierCurve.SetStartPoint(start);
            bezierCurve.SetEndPoint(end);
            bezierCurve.SetGuideWidth(width);
            bezierCurve.SetParamBezier(paramBezier);

            bezierCurve.ComputeBezierCurveCall();
			
			//longueur du waveguide
			float WGLength = bezierCurve.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			LDisplay_Refresh();
            return; //fin de programme
        }


        strcpy(strPath,"guideFile.csv");
        LDialog_File( strPath, "CSV file", strPath, "CSV Files (*.csv)|*.csv|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", 1, "Enter path of the CSV file containing the guides between labels", "OK", "csv", "*.csv|*.txt||", pFile );
        strPath[strlen(strPath)-2]='\0'; //delete the last 2 char of the string ("|0")
        
        if (strPath != NULL)
        {
            LFile_GetResolvedFileName( pFile, strPath, cwd, MAX_TDBFILE_NAME ); //get the absolute path of strPath in cwd
            myFile = fopen(cwd,"r");
        }
        
        if(myFile != NULL)
        {
            while(!feof(myFile))
            {
                nmbLabel = 0;
                // reads text until new line 
                fscanf(myFile,"%[^\n]", line);
                LUpi_LogMessage(LFormat("Read line: %s\n", line));
                cpt=0;
                for(i=0; i<strlen(line);i++)
                {
                    if(line[i]==',') //count the number of ',' to know what kind of shape it is
                        cpt++;
                }

                token = strtok(line, ","); //first string before the first ',' of line

                if(cpt == 6) //guide,cell1,P1,cell2,P2,radius,width
                {
                    token = strtok(NULL, ","); //next token
                    strcpy(startCellName,token);
                    token = strtok(NULL, ",");
                    strcpy(startLabelName,token);
                    token = strtok(NULL, ",");
                    strcpy(endCellName,token);
                    token = strtok(NULL, ",");
                    strcpy(endLabelName,token);
                    token = strtok(NULL, ",");
                    radius = atof(token);
                    token = strtok(NULL, ",");
                    width = atof(token);
                }

                LCell startCell;
                if(strcmp(startCellName," ")==0)
                    startCell = pCell;
                else
                    startCell = LCell_Find( pFile, startCellName );
                if(startCell == NULL)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\nInstruction will not be executed \n", startCellName) );
                    continue;
                }

                for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
                {
                    LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                    
                    if(strcmp(sLabelName, startLabelName) == 0)
                    {
                        pLabelLocation = LLabel_GetPosition( pLabel );
                        xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                        yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                        start.SetPoint(xPosLabel , yPosLabel, pFile);

                        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                        {
                            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                            {
                                start.SetAngleDegre( dAngle );
                            }	
                            else
                            {
                                start.SetAngleDegre( 0 );
                                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                            }
                                
                        }		
                        else
                        {
                            start.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle property not found, 0 by default\n");
                        }
                        nmbLabel++;
                        break;
                    }
                    
                }
                if(nmbLabel != 1)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\nInstruction will not be executed \n",startLabelName, startCellName) );
                    continue;
                }

                LCell endCell;
                if(strcmp(endCellName," ")==0)
                    endCell = pCell;
                else
                    endCell = LCell_Find( pFile, endCellName );

                if(endCell == NULL)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\nInstruction will not be executed \n",endCellName) );
                    continue;
                }

                for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
                {
                    LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                    
                    if(strcmp(sLabelName, endLabelName) == 0)
                    {
                        pLabelLocation = LLabel_GetPosition( pLabel );
                        xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                        yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                        end.SetPoint(xPosLabel , yPosLabel, pFile);

                        if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                        {
                            if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                            {
                                end.SetAngleDegre( dAngle );
                            }	
                            else
                            {
                                end.SetAngleDegre( 0 );
                                LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                            }
                        }		
                        else
                        {
                            end.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle property not found, 0 by default\n");
                        }
                        nmbLabel++;
                        break;
                    }
                }

                if(nmbLabel != 2)
                {
                    fscanf(myFile,"\n"); //got to the next line
                    LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\nInstruction will not be executed \n",endLabelName, endCellName) );
                    continue;
                }

                
                bezierCurve.SetFile(pFile);
                bezierCurve.SetCell(pCell);
                bezierCurve.SetLayer(pLayer);
                bezierCurve.SetStartPoint(start);
                bezierCurve.SetEndPoint(end);
                bezierCurve.SetGuideWidth(width);
                bezierCurve.SetParamBezier(paramBezier);
                
                bezierCurve.ComputeBezierCurveCall();
            

                fscanf(myFile,"\n"); //got to the next line
            }
            
			//longueur du waveguide
			float WGLength = bezierCurve.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
			fclose(myFile);
        }



        else //manual selection
        {
            LDialog_AlertBox(LFormat("No file found: manual selection"));
            LDialogItem DialogItems[2] = {{ "Cell","cell1"}, { "Name","P1"}};
            if (LDialog_MultiLineInputBox("Start point",DialogItems,2) == 0)
                return;
            strcpy(startCellName,DialogItems[0].value);
            strcpy(startLabelName,DialogItems[1].value);

            LCell startCell = LCell_Find( pFile, startCellName );
            if(startCell == NULL)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n", startCellName) );
                return;
            }

            for(LLabel pLabel = LLabel_GetList(startCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
            {
                LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );
                
                if(strcmp(sLabelName, startLabelName) == 0)
                {
                    pLabelLocation = LLabel_GetPosition( pLabel );
                    xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                    yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                    start.SetPoint(xPosLabel , yPosLabel, pFile);

                    if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                    {
                        if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                        {
                            start.SetAngleDegre( dAngle );
                        }	
                        else
                        {
                            start.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                        }
                            
                    }		
                    else
                    {
                        start.SetAngleDegre( 0 );
                        LUpi_LogMessage("Angle property not found, 0 by default\n");
                    }
                    nmbLabel++;
                    break;
                }
            }
            if(nmbLabel != 1)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",startLabelName, startCellName) );
                return;
            }



            LDialogItem DialogItemsEnd[2] = {{ "Cell","cell2"}, { "Name","P2"}};
            if (LDialog_MultiLineInputBox("End point",DialogItemsEnd,2) == 0)
                return;
            strcpy(endCellName,DialogItemsEnd[0].value);
            strcpy(endLabelName,DialogItemsEnd[1].value);

            LCell endCell = LCell_Find( pFile, endCellName );
            if(endCell == NULL)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" cell\n",endCellName) );
                return;
            }

            for(LLabel pLabel = LLabel_GetList(endCell); pLabel != NULL ; pLabel =  LLabel_GetNext(pLabel))
            {
                LLabel_GetName( pLabel, sLabelName, MAX_CELL_NAME );

                if(strcmp(sLabelName, endLabelName) == 0)
                {
                    pLabelLocation = LLabel_GetPosition( pLabel );
                    xPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.x);
                    yPosLabel = LFile_IntUtoMicrons( pFile, pLabelLocation.y);

                    end.SetPoint(xPosLabel , yPosLabel, pFile);

                    if (LEntity_PropertyExists((LEntity)pLabel, "Angle") == LStatusOK)
                    {
                        if(LEntity_GetPropertyValue((LEntity)pLabel, "Angle", &dAngle, sizeof(double)) == LStatusOK)
                        {
                            end.SetAngleDegre( dAngle );
                        }	
                        else
                        {
                            end.SetAngleDegre( 0 );
                            LUpi_LogMessage("Angle GetPropertyValue failed, 0 by default\n");
                        }
                            
                    }		
                    else
                    {
                        end.SetAngleDegre( 0 );
                        LUpi_LogMessage("Angle property not found, 0 by default\n");
                    }
                    nmbLabel++;
                    break;
                }
            }

            if(nmbLabel != 2)
            {
                LUpi_LogMessage( LFormat("ERROR: Unable to find \"%s\" label in \"%s\" cell\n",endLabelName, endCellName) );
                return;
            }


            bezierCurve.SetFile(pFile);
            bezierCurve.SetCell(pCell);
            bezierCurve.SetLayer(pLayer);
            bezierCurve.SetStartPoint(start);
            bezierCurve.SetEndPoint(end);
            bezierCurve.SetGuideWidth(width);
            bezierCurve.SetParamBezier(paramBezier);
            
            bezierCurve.ComputeBezierCurveCall();
            
			//longueur du waveguide
			float WGLength = bezierCurve.GetWGLength();
			LUpi_LogMessage(LFormat("\nLength du waveguide: %.4f microns\n\n", WGLength));
			
			LDisplay_Refresh();
        }
    }
    
	//other choice = nothing
	
	
    LUpi_LogMessage( "Macro FIN\n" );
}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F5" /*hotkey*/, 
			"AEPONYX\\Dubins and Bezier Macro\nWindow", "BezierAndDubinsMacro", NULL /*hotkey category*/);
	return 1;
}