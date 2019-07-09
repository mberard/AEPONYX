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

    bool rasterizeWaveguide = true;

    LDialogItem DialogItems[2] = {{ "Oxide size on each size (in microns)","5"}, { "Oxide layer","OX"}};


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

    strcpy(strLayer, "WG"); //preloaded text in the dialog box
	if ( LDialog_InputBox("Layer", "Enter name of the layer of the active cell in which the guide will be loaded", strLayer) == 0)
		return;
	else
		pLayer = LLayer_Find(pFile, strLayer);

	if(NotAssigned(pLayer)) //the layer is not found
	{
		LDialog_AlertBox(LFormat("ERROR:  Could not get the Layer %s in visible cell.", strLayer));
		return;
    }
    LLayer_GetName(pLayer, sLayerName, MAX_LAYER_NAME);

    //get the path
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		LUpi_LogMessage(LFormat("getcwd() error: %s\n",strerror(errno)));
	else
        LUpi_LogMessage(LFormat("current working directory is: %s\n", strcat (cwd,"\\")));

    path.SetFile(pFile);
    path.SetCell(pCell);
    path.SetLayer(pLayer);

    if(choice == 0) //Dubins curve with circles
    {
        LUpi_LogMessage( "\n\nDubins curve with circle\n\n" );

        path.SetOffsetCurveIsSelected(true); //offset?
        strcpy(value_offset, "0.00");
        if ( LDialog_InputBox("Offset", "Enter the value of the offset (in microns)", value_offset) == LCANCEL)
            path.SetOffsetValue(0);
        else
        {
            path.SetOffsetValue( atof(value_offset) );
        }
        if(atof(value_offset) == 0.0)
            path.SetOffsetCurveIsSelected(false);
        else
            path.SetOffsetCurveIsSelected(true);


        if(LDialog_MultiLineInputBox("Oxide",DialogItems,2)) //oxide?
        {
            path.SetOxideSizeValue( 2*atof(DialogItems[0].value) );
            if(LLayer_Find(pFile, DialogItems[1].value))
            {
                path.SetOxideLayer( LLayer_Find(pFile, DialogItems[1].value) );
            }
            else
            {
                LDialog_AlertBox("Oxide layer could not be found, oxide will not be generated");
            }
        }
        else
        {
            path.SetOxideSizeValue(0);
        }
        

        if ( LDialog_YesNoBox("Do you want to rasterize the waveguide?") )
            rasterizeWaveguide = true; /*Yes is clicked*/
        else
            rasterizeWaveguide = false; /*No is clicked*/

        if( twoLabelsHasBeenSelected() ) //2 labels selected : we will store the positions and angles
        {
            LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
            LSelection pSelection = LSelection_GetList() ;

            LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
            LLabel pLabel = (LLabel)object;

            //endLabel
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

            pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
            object = LSelection_GetObject(pSelection);
            pLabel = (LLabel)object;

            //startLabel
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
            strcpy(strLayer, "100");
            if ( LDialog_InputBox("Radius", "Select the radius of the circles in microns", strLayer) == 0)
                return;
            else
                radius = atof(strLayer);

            strcpy(strLayer, "0.45");
            if ( LDialog_InputBox("Guide width", "Select the width of the guide in microns", strLayer) == 0)
                return;
            else
                width = atof(strLayer);

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
            else
            {
                path.ComputeDubinsPaths();
                path.RasterizePath(rasterizeWaveguide);
            }
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

            strcpy(strLayer, "100");
            if ( LDialog_InputBox("Radius", "Select the radius of the circles in microns", strLayer) == 0)
                return;
            else
                radius = atof(strLayer);

            strcpy(strLayer, "0.435");
            if ( LDialog_InputBox("Guide width", "Select the width of the guide in microns", strLayer) == 0)
                return;
            else
                width = atof(strLayer);

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
        }
    }


    if(choice == 1) //Dubins curve with Bézier
    {
        LUpi_LogMessage( "\n\nDubins curve with Bézier\n\n" );

        strcpy(strLayer, "0.3");
        if ( LDialog_InputBox("Bezier parameter", "Select the Bezier parameter (between 0 and 1)", strLayer) == 0)
            return;
        else
        {
            paramBezier = atof(strLayer);
            path.SetParamBezier(paramBezier);
        }

        
        if(LDialog_MultiLineInputBox("Oxide",DialogItems,2))
        {

            path.SetOxideSizeValue( 2*atof(DialogItems[0].value) );
            if(LLayer_Find(pFile, DialogItems[1].value))
            {
                path.SetOxideLayer( LLayer_Find(pFile, DialogItems[1].value) );
            }
            else
            {
                LDialog_AlertBox("Oxide layer could not be found, oxide will not be generated");
            }
        }
        else
        {
            path.SetOxideSizeValue(0);
        }
        

        if( twoLabelsHasBeenSelected() )
        {
            LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
            LSelection pSelection = LSelection_GetList() ;

            LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
            LLabel pLabel = (LLabel)object;

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

            pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
            object = LSelection_GetObject(pSelection);
            pLabel = (LLabel)object;

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
            strcpy(strLayer, "100");
            if ( LDialog_InputBox("Radius", "Select the radius of the circles in microns", strLayer) == 0)
                return;
            else
                radius = atof(strLayer);

            strcpy(strLayer, "0.435");
            if ( LDialog_InputBox("Guide width", "Select the width of the guide in microns", strLayer) == 0)
                return;
            else
                width = atof(strLayer);

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
                path.DubinsPathWithBezierCurves();
            }
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
                    path.DubinsPathWithBezierCurves();
                }

                fscanf(myFile,"\n"); //got to the next line
            }
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

            strcpy(strLayer, "100");
            if ( LDialog_InputBox("Radius", "Select the radius of the circles in microns", strLayer) == 0)
                return;
            else
                radius = atof(strLayer);

            strcpy(strLayer, "0.435");
            if ( LDialog_InputBox("Guide width", "Select the width of the guide in microns", strLayer) == 0)
                return;
            else
                width = atof(strLayer);

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
                path.DubinsPathWithBezierCurves();
            }
        }
    }



    else if(choice == 2) //Bezier curves
    {
        LUpi_LogMessage( "\n\nBezier curve\n\n" );

        if(LDialog_MultiLineInputBox("Oxide",DialogItems,2))
        {

            bezierCurve.SetOxideSizeValueBezier( 2*atof(DialogItems[0].value) );
            if(LLayer_Find(pFile, DialogItems[1].value))
            {
                bezierCurve.SetOxideLayerBezier( LLayer_Find(pFile, DialogItems[1].value) );
            }
            else
            {
                LDialog_AlertBox("Oxide layer could not be found, oxide will not be generated");
            }
        }
        else
        {
            bezierCurve.SetOxideSizeValueBezier(0);
        }
        

        if( twoLabelsHasBeenSelected() )
        {
            LUpi_LogMessage(LFormat("2 LLabels has been selected\n"));
            LSelection pSelection = LSelection_GetList() ;

            LObject object = LSelection_GetObject(pSelection); //first label is the second one selected
            LLabel pLabel = (LLabel)object;

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

            pSelection = LSelection_GetNext(pSelection); //second label is the first one selected
            object = LSelection_GetObject(pSelection);
            pLabel = (LLabel)object;

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

            strcpy(strLayer, "0.435");
            if ( LDialog_InputBox("Guide width", "Select the width of the guide in microns", strLayer) == 0)
                return;
            else
                width = atof(strLayer);
            
            strcpy(strLayer, "0.3");
            if ( LDialog_InputBox("Bezier parameter", "Select the Bezier parameter (between 0 and 1)", strLayer) == 0)
                return;
            else
                paramBezier = atof(strLayer);

            bezierCurve.SetFile(pFile);
            bezierCurve.SetCell(pCell);
            bezierCurve.SetLayer(pLayer);
            bezierCurve.SetStartPoint(start);
            bezierCurve.SetEndPoint(end);
            bezierCurve.SetGuideWidth(width);
            bezierCurve.SetParamBezier(paramBezier);

            bezierCurve.ComputeBezierCurve();

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
            strcpy(strLayer, "0.3");
            if ( LDialog_InputBox("Bezier parameter", "Select the Bezier parameter (between 0 and 1)", strLayer) == 0)
                return;
            else
                paramBezier = atof(strLayer);

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
                
                bezierCurve.ComputeBezierCurve();
            

                fscanf(myFile,"\n"); //got to the next line
            }
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

            strcpy(strLayer, "0.435");
            if ( LDialog_InputBox("Guide width", "Select the width of the guide in microns", strLayer) == 0)
                return;
            else
                width = atof(strLayer);

            strcpy(strLayer, "0.3");
            if ( LDialog_InputBox("Bezier parameter", "Select the Bezier parameter (between 0 and 1)", strLayer) == 0)
                return;
            else
                paramBezier = atof(strLayer);

            bezierCurve.SetFile(pFile);
            bezierCurve.SetCell(pCell);
            bezierCurve.SetLayer(pLayer);
            bezierCurve.SetStartPoint(start);
            bezierCurve.SetEndPoint(end);
            bezierCurve.SetGuideWidth(width);
            bezierCurve.SetParamBezier(paramBezier);
            
            bezierCurve.ComputeBezierCurve();
            
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