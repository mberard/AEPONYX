#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <stdlib.h>

// #include <regex.h> //may be usefull to delete all the cell
// see: https://nicolasj.developpez.com/articles/regex/
// str_regex : "^T_C[[:digit:]]{2,3}T[[:digit:]]{2,3}"

#define MAX_NUMBER_WINDOWS 50

void AutomaticNumerotationMacro()
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;

    LLayer oxideLayer;
    LLayer facetLayer;
  
    char strNameWanted[MAX_TDBFILE_NAME];
    char strName[MAX_TDBFILE_NAME];
    char strText[MAX_TDBFILE_NAME];
    char buffer[10];

    int hasLabel = 0;
    LLayer labelLayer;
    LCoord ref_x, ref_y;

    LDialogItem DialogItems1[2] = {{ "X (in microns)","100"}, { "Y (in microns)","100"}};
    LDialogItem DialogItems2[2] = {{ "Number of column","1"}, { "Number of line","1"}};
    LDialogItem DialogItems3[2] = {{ "Oxide fillet in microns (0 for no oxide)","2.5"}, { "Facet fillet in microns (0 for no facet)","2.5"}};

    double tmp1, tmp2;
    LCoord delta_x, delta_y;
    int nbLine, nbCol;
    LCoord textSize = 0;

    LCoord oxideFilletValue = 0;
    LCoord facetFilletValue = 0;
    LRect rectMBB;
    LPoint point_arr[10];
    int nbPoints = 0;
    LObject oxidePolygon;
    LObject facetPolygon;
    LVertex vertex;

    int cpt;

    LCell bigCell;
    LCell smallCell;
    LInstance instanceCreated;

    LTransform_Ex99 transformation;

    LWindow activeWindows[MAX_NUMBER_WINDOWS];
    int numberWindows = 0;
    int hasBeenFoundInArray = 0;
    LWindow pWindow = NULL;


    char *Pick_List [ ] = {
    "Create an automatic numerotation",
    "Delete an existing automatic numerotation"
    };
    int Pick_Count = 2;
    int Picked;
    
    Picked = LDialog_PickList ("Select Element", Pick_List, Pick_Count, 0);

    if(Picked == -1)
        return;
    else if(Picked == 1)
    {
        LCell cellListItem = LCell_GetList(pFile);
        
        bigCell = LCell_Find(pFile, "AutomaticNumerotation"); //cell with all the other instance of numerotation

        LInstance inst;
        if(bigCell)
            for( inst = LInstance_GetList(bigCell); inst != NULL; inst = LInstance_GetNext(inst)) 
                LInstance_Delete(bigCell, inst); //deleting all the instance in bigCell

        inst = LInstance_Find(pCell, "Die numerotation");
        if(inst)
            LInstance_Delete(pCell, inst); //delete the instance of the big cell in pCell
        LCell_Delete( bigCell ); //delete the cell

        while ( cellListItem != NULL )
        {
            LCell_GetName( cellListItem, strName, MAX_TDBFILE_NAME );
            LUpi_LogMessage(LFormat("try cell %s\n", strName));
            if(strlen(strName)>=8)
            {
                if(strName[0]=='T' && strName[1]=='_' && strName[2]=='C') //delete the cell beginning by T_C; could be done with regex
                {
                    LCell toBeDeleted = cellListItem;
                    cellListItem = LCell_GetNext(cellListItem);
                    LCell_Delete( toBeDeleted );
                    LUpi_LogMessage(LFormat("Deleting cell %s\n", strName));
                }
                else
                    cellListItem = LCell_GetNext(cellListItem);
            }
            else
                cellListItem = LCell_GetNext(cellListItem);
        }

        return; //end of the macro
    }

//if we choose 0 ("Create an automatic numerotation")
    for( pWindow = LWindow_GetList(); Assigned(pWindow); pWindow = LWindow_GetNext(pWindow) ) //saved the windows opened before the macro
    {
        activeWindows[numberWindows] = pWindow;
        numberWindows++;
    }

    strcpy(strNameWanted, "die_id"); //preloaded text in the dialog box
	if ( LDialog_InputBox("Layer", "Enter the name of the origin label", strNameWanted) == 0)
		return;

    if(LLabel_GetList(pCell)) //if there are some labels
    {
        for(LLabel pLab = LLabel_GetList(pCell); pLab != NULL; pLab = LLabel_GetNext(pLab) ) //for each label
        {
            LLabel_GetName( pLab, strName, MAX_TDBFILE_NAME );
            if( strcmp(strName, strNameWanted) == 0 )
            {
                LUpi_LogMessage(LFormat("Label found %s\n",strName));
                LPoint point = LLabel_GetPosition(pLab);
                labelLayer = LLabel_GetLayer(pLab);
                ref_x = point.x;
                ref_y = point.y;
                hasLabel = 1;
            }
        }
    }

    if(hasLabel == 0) //label not found
        return;

    strcpy(buffer, "100.0");
    if ( LDialog_InputBox("Size of the texte", "Enter the text size (in microns)", buffer) == 0)
			return;
	else
    {
        tmp1 = atof(buffer);
        textSize = LFile_MicronsToIntU(pFile, tmp1);
    }
        

	if(LDialog_MultiLineInputBox("Deltas between 2 dies",DialogItems1,2))
    {

        tmp1 = atoi(DialogItems1[0].value);
        tmp2 = atoi(DialogItems1[1].value);
        delta_x = LFile_MicronsToIntU(pFile, tmp1);
        delta_y = LFile_MicronsToIntU(pFile, tmp2);
    }
    else
        return;

	if(LDialog_MultiLineInputBox("Number of column/line",DialogItems2,2))
    {
        nbCol = atoi(DialogItems2[0].value);
        nbLine = atoi(DialogItems2[1].value);
    }
    else
        return;

    if(LDialog_MultiLineInputBox("Oxide and facet",DialogItems3,2))
    {

        tmp1 = atoi(DialogItems3[0].value);
        tmp2 = atoi(DialogItems3[1].value);
        oxideFilletValue = LFile_MicronsToIntU(pFile, tmp1);
        facetFilletValue = LFile_MicronsToIntU(pFile, tmp2);
    }
    else
        return;

    if(oxideFilletValue != 0)
    {
        strcpy(strName, "OX"); //preloaded text in the dialog box
        if ( LDialog_InputBox("Oxide layer", "Enter the name of the oxide layer", strName) == 0)
            return;
        else
            oxideLayer = LLayer_Find(pFile, strName);
    }
    if(facetFilletValue != 0)
    {
        strcpy(strName, "FACET"); //preloaded text in the dialog box
        if ( LDialog_InputBox("Oxide layer", "Enter the name of the oxide layer", strName) == 0)
            return;
        else
            facetLayer = LLayer_Find(pFile, strName);
    }

    if( ! (LCell_Find(pFile, "AutomaticNumerotation"))) //if not exist
        bigCell = LCell_New(pFile, "AutomaticNumerotation"); //create
    else
        bigCell = LCell_Find(pFile, "AutomaticNumerotation"); //point on the existing one

    for(LInstance inst = LInstance_GetList(bigCell); inst != NULL; inst = LInstance_GetNext(inst)) 
                LInstance_Delete(bigCell, inst); //deleting all the instance in bigCell
    
    for(int i = nbLine; i>0; i--)
    {
        for(int j = 0; j<nbCol; j++)
        {
            strcpy(strText, ""); //concatenate the name CxxLxx
            strcat(strText, "C");
            if(j+1 < 10)
                strcat(strText, "0");
            itoa(j+1, buffer, 10);
            strcat(strText, buffer);
            strcat(strText, "L");
            if(i < 10)
                strcat(strText, "0");
            itoa(i, buffer, 10);
            strcat(strText, buffer);

            strcpy(strName, "");
            strcat(strName, "T_"); //adding T_CxxLxx
            strcat(strName, strText);

            smallCell = LCell_Find(pFile, strName);
            if(smallCell == NULL)
                smallCell = LCell_New(pFile, strName); //create cell if not exist
            else
            {
                LCell_Delete( smallCell ); 
                smallCell = LCell_New(pFile, strName); //if already exist: delete and redo
            }

            //create text CxxLxx
            LCell_MakeLogo( smallCell,
                                strText,
                                textSize,
                                labelLayer,
                                LFALSE,
                                LFALSE,
                                LFALSE,
                                0,
                                (LCoord)(textSize/2 - textSize*0.1538),
                                LFALSE,
                                LTRUE,
                                LFALSE,
                                "",
                                "",
                                "",
                                NULL );

            // create oxide fillet rectangle
            if(oxideFilletValue != 0)
            {
                long rectShift = LFile_MicronsToIntU(pFile, 5);
                rectMBB = LCell_GetMbb( smallCell );
                point_arr[0] = LPoint_Set( rectMBB.x0-rectShift+oxideFilletValue , rectMBB.y0-rectShift );
                point_arr[1] = LPoint_Set( rectMBB.x1+rectShift-oxideFilletValue , rectMBB.y0-rectShift );
                point_arr[2] = LPoint_Set( rectMBB.x1+rectShift , rectMBB.y0-rectShift+oxideFilletValue );
                point_arr[3] = LPoint_Set( rectMBB.x1+rectShift , rectMBB.y1+rectShift-oxideFilletValue );
                point_arr[4] = LPoint_Set( rectMBB.x1+rectShift-oxideFilletValue , rectMBB.y1+rectShift );
                point_arr[5] = LPoint_Set( rectMBB.x0-rectShift+oxideFilletValue , rectMBB.y1+rectShift );
                point_arr[6] = LPoint_Set( rectMBB.x0-rectShift , rectMBB.y1+rectShift-oxideFilletValue );
                point_arr[7] = LPoint_Set( rectMBB.x0-rectShift , rectMBB.y0-rectShift+oxideFilletValue );
                nbPoints = 8;
                oxidePolygon = LPolygon_New(smallCell, oxideLayer, point_arr, nbPoints);
                vertex = LObject_GetVertexList(oxidePolygon);
				for(cpt=0; cpt<nbPoints; cpt++) //for each vertex
				{
					if(cpt == 1)
                        LVertex_AddCurve( oxidePolygon, vertex, LPoint_Set( rectMBB.x1+rectShift-oxideFilletValue , rectMBB.y0-rectShift+oxideFilletValue ), CCW );
                    else if(cpt == 3)
                        LVertex_AddCurve( oxidePolygon, vertex, LPoint_Set( rectMBB.x1+rectShift-oxideFilletValue , rectMBB.y1+rectShift-oxideFilletValue ), CCW );
                    else if(cpt == 5)
                        LVertex_AddCurve( oxidePolygon, vertex, LPoint_Set( rectMBB.x0-rectShift+oxideFilletValue , rectMBB.y1+rectShift-oxideFilletValue ), CCW );
                    else if(cpt == 7)
                        LVertex_AddCurve( oxidePolygon, vertex, LPoint_Set( rectMBB.x0-rectShift+oxideFilletValue , rectMBB.y0-rectShift+oxideFilletValue ), CCW );
					vertex = LVertex_GetNext(vertex);
				}
            }

            // create facet fillet rectangle
            if(facetFilletValue != 0)
            {
                long rectShift = LFile_MicronsToIntU(pFile, 15);
                rectMBB = LCell_GetMbb( smallCell );
                point_arr[0] = LPoint_Set( rectMBB.x0-rectShift+facetFilletValue , rectMBB.y0-rectShift );
                point_arr[1] = LPoint_Set( rectMBB.x1+rectShift-facetFilletValue , rectMBB.y0-rectShift );
                point_arr[2] = LPoint_Set( rectMBB.x1+rectShift , rectMBB.y0-rectShift+facetFilletValue );
                point_arr[3] = LPoint_Set( rectMBB.x1+rectShift , rectMBB.y1+rectShift-facetFilletValue );
                point_arr[4] = LPoint_Set( rectMBB.x1+rectShift-facetFilletValue , rectMBB.y1+rectShift );
                point_arr[5] = LPoint_Set( rectMBB.x0-rectShift+facetFilletValue , rectMBB.y1+rectShift );
                point_arr[6] = LPoint_Set( rectMBB.x0-rectShift , rectMBB.y1+rectShift-facetFilletValue );
                point_arr[7] = LPoint_Set( rectMBB.x0-rectShift , rectMBB.y0-rectShift+facetFilletValue );
                nbPoints = 8;
                facetPolygon = LPolygon_New(smallCell, facetLayer, point_arr, nbPoints);
                vertex = LObject_GetVertexList(facetPolygon);
				for(cpt=0; cpt<nbPoints; cpt++) //for each vertex
				{
					if(cpt == 1)
                        LVertex_AddCurve( facetPolygon, vertex, LPoint_Set( rectMBB.x1+rectShift-facetFilletValue , rectMBB.y0-rectShift+facetFilletValue ), CCW );
                    else if(cpt == 3)
                        LVertex_AddCurve( facetPolygon, vertex, LPoint_Set( rectMBB.x1+rectShift-facetFilletValue , rectMBB.y1+rectShift-facetFilletValue ), CCW );
                    else if(cpt == 5)
                        LVertex_AddCurve( facetPolygon, vertex, LPoint_Set( rectMBB.x0-rectShift+facetFilletValue , rectMBB.y1+rectShift-facetFilletValue ), CCW );
                    else if(cpt == 7)
                        LVertex_AddCurve( facetPolygon, vertex, LPoint_Set( rectMBB.x0-rectShift+facetFilletValue , rectMBB.y0-rectShift+facetFilletValue ), CCW );
					vertex = LVertex_GetNext(vertex);
				}
            }

            if(!(LInstance_Find(bigCell, strText))) //add the instance that not already exist
            {
                transformation.translation.x = j*delta_x;
                transformation.translation.y = (nbLine-i)*delta_y;
                transformation.orientation = 0.0;
                transformation.magnification.num = 1;
                transformation.magnification.denom = 1;
                instanceCreated = LInstance_GenerateV(bigCell, smallCell, NULL);
                if(LInstance_Set_Ex99(smallCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(delta_x, delta_y)) == LStatusOK)
                {
                    LInstance_SetName( smallCell, instanceCreated, strText );
                }
            }
        }
    }

    
    for( pWindow = LWindow_GetList(); Assigned(pWindow); pWindow = LWindow_GetNext(pWindow) ) //keep only the windows from the beginning openned
    {
        hasBeenFoundInArray = 0;
        for(int i = 0; i<numberWindows; i++)
        {
            if(activeWindows[i] == pWindow)
            {
                hasBeenFoundInArray = 1;
                break;
            }
        }
        if(hasBeenFoundInArray == 0)
            LWindow_Close(pWindow);
    }

    if(!(LInstance_Find(pCell, "Die numerotation"))) //add an instance with all the numerotation in the right cell and layer
    {
        transformation.translation.x = ref_x;
        transformation.translation.y = ref_y;
        transformation.orientation = 0;
        transformation.magnification.num = 1;
        transformation.magnification.denom = 1;
        instanceCreated = LInstance_GenerateV(pCell, bigCell, NULL);
        if(LInstance_Set_Ex99(bigCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(delta_x, delta_y)) == LStatusOK)
        {
            LInstance_SetName( bigCell, instanceCreated, "Die numerotation" );
        }
    }
}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F4" /*hotkey*/, "AEPONYX\\Automatic Numerotation Macro\nWindow", "AutomaticNumerotationMacro", NULL /*hotkey category*/);
	return 1;
}