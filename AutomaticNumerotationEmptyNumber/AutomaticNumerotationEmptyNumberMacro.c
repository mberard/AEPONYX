#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <stdlib.h>

// #include <regex.h> //may be usefull to delete all the cell
// see: https://nicolasj.developpez.com/articles/regex/
// str_regex : "^T_C[[:digit:]]{2,3}T[[:digit:]]{2,3}"

#define MAX_NUMBER_OBJECT 500

int ObjectIsInArray(LObject obj, LObject* array, int number)
{
    int i;
    for(i=0; i<number; i++)
    {
        if(array[i] == obj)
            return 1;
    }
    return 0;
}

void AutomaticNumerotationEmptyNumberMacro()
{
    LCell pCell	= LCell_GetVisible();
	LFile pFile	= LCell_GetFile(pCell);
    LLayer pLayer;
    LLayer tmp;
    LLayer tmpShrink;
    LLayer tmpPreviousPolygons;
  
    char strNameWanted[MAX_TDBFILE_NAME];
    char strName[MAX_TDBFILE_NAME];
    char strText[MAX_TDBFILE_NAME];
    char str[MAX_TDBFILE_NAME];
    char buffer[10];

    int hasLabel = 0;
    LLayer labelLayer;
    LCoord ref_x, ref_y;

    LDialogItem DialogItems[4] = {{ "Start number","0"}, { "increment","1"}, { "Stop number","0"}, { "Space between two numbers (in microns)","1000"}};

    double tmpDouble;
    LCoord delta;
    int startNumber, increment, stopNumber;
    LCoord textSize = 0;
    int tmpInt;
    double shrinkValue;
    int diff;
    int value;
    int cpt;
    int result;
    int numberInSelection;

    LObject obj_arr[MAX_NUMBER_OBJECT];
    int numberObject = 0;
    LObject obj_arr_shrink[MAX_NUMBER_OBJECT];
    int numberObjectShrink = 0;

    LCell newCell;
    LTransform_Ex99 transformation;
    LInstance instanceCreated;

    char *Pick_List [ ] = {
        "Create an automatic numerotation with empty digits",
        "Delete an existing automatic numerotation with empty digits"
    };
    int Pick_Count = 2;
    int Picked;
    
    Picked = LDialog_PickList ("Select Element", Pick_List, Pick_Count, 0);

    if(Picked == -1)
        return;
    else if(Picked == 1) //delete existing numerotation
    {
        LCell cellListItem = LCell_GetList(pFile);

        LInstance inst;
        for( inst = LInstance_GetList(pCell); inst != NULL; inst = LInstance_GetNext(inst)) 
        {
            LInstance_GetName(inst, strName, MAX_TDBFILE_NAME );
            if(strName[0]=='A' && strName[1]=='U' && strName[2]=='T' && strName[3]=='O' && strName[4]=='_')
            {
                LInstance_Delete( pCell, inst); //delete instance beginning by "AUTO_"
            }
        }

        while ( cellListItem != NULL )
        {
            LCell_GetName( cellListItem, strName, MAX_TDBFILE_NAME );
            if(strlen(strName)>=8)
            {
                if(strName[0]=='A' && strName[1]=='U' && strName[2]=='T' && strName[3]=='O' && strName[4]=='_')
                {
                    LCell toBeDeleted = cellListItem;
                    cellListItem = LCell_GetNext(cellListItem);
                    LCell_Delete( toBeDeleted );
                    LUpi_LogMessage(LFormat("Deleting cell %s\n", strName)); //delete instance beginning by "AUTO_"
                }
                else
                    cellListItem = LCell_GetNext(cellListItem);
            }
            else
                cellListItem = LCell_GetNext(cellListItem);
        }

        return; //end of the macro
    }

//choice to create a numerotation (choice == 0)
    LSelection pSelection = LSelection_GetList();
    if(pSelection)
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
        }
        if(cpt == 1) //if a label is selected
            strcpy(strNameWanted, strName); //preloaded text in the dialog box with the name of the label selected
        else
            strcpy(strNameWanted, "labelName"); //preloaded text in the dialog box
    }
    else
        strcpy(strNameWanted, "labelName"); //preloaded text in the dialog box
	if ( LDialog_InputBox("Layer", "Enter the name of the origin label", strNameWanted) == 0)
		return;

    if(LLabel_GetList(pCell)) //if there are some labels
    {
        for(LLabel pLab = LLabel_GetList(pCell); pLab != NULL; pLab = LLabel_GetNext(pLab) ) //for each label
        {
            LLabel_GetName( pLab, strName, MAX_TDBFILE_NAME );
            if( strcmp(strName, strNameWanted) == 0 ) //try to find the label
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

    if(hasLabel == 0) //if no label found, return
        return;

    strcpy(buffer, "100.0");
    if ( LDialog_InputBox("Size of the texte", "Enter the text size (in microns)", buffer) == 0)
			return;
	else
    {
        tmpDouble = atof(buffer);
        textSize = LFile_MicronsToIntU(pFile, tmpDouble);
    }
        

	if(LDialog_MultiLineInputBox("User input",DialogItems,4))
    {
        startNumber = atoi(DialogItems[0].value);

        increment = atoi(DialogItems[1].value);
    
        stopNumber = atoi(DialogItems[2].value);

        tmpInt = atoi(DialogItems[3].value);
        delta = LFile_MicronsToIntU(pFile, tmpInt);
    }
    else
        return;

    //is the increment in the good direction
    if(stopNumber < startNumber && increment > 0 )
    {
        LUpi_LogMessage("Impossible to reach the stop number from the start number with this increment");
        return;
    }
    if(stopNumber > startNumber && increment < 0 )
    {
        LUpi_LogMessage("Impossible to reach the stop number from the start number with this increment");
        return;
    }

    //is it possible to reach the destination ?
    diff = abs(stopNumber - startNumber);
    while(diff > 0)
    {
        diff = diff - abs(increment);
    }
    if(diff != 0 || (stopNumber == startNumber && increment != 0 ) )
    {
        LUpi_LogMessage("Impossible to reach the stop number from the start number with this increment");
        return;
    }

    strcpy(buffer, "0.5");
    if ( LDialog_InputBox("Shrink value", "Enter the shrink value (in microns)", buffer) == 0)
			return;
	else
    {
        tmpDouble = atof(buffer);
        shrinkValue = LFile_MicronsToIntU(pFile, tmpDouble);
    }

    //create tmp layers
    LLayer_New( pFile, NULL, "tmpNum");
    tmp = LLayer_Find(pFile, "tmpNum");

    LLayer_New( pFile, NULL, "tmpShrink");
    tmpShrink = LLayer_Find(pFile, "tmpShrink");

    //before computing, change the layer of the existing polygons to not group them with digits
    LLayer_New( pFile, NULL, "tmpPreviousPolygons");
    tmpPreviousPolygons = LLayer_Find(pFile, "tmpPreviousPolygons");
    for(LObject obj = LObject_GetList(pCell, labelLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_ChangeLayer(pCell, obj, tmpPreviousPolygons );
    }

    cpt = 0;
    value = startNumber;
    while(value != stopNumber)
    {
        itoa(value, strText, 10);

        //create a new cell
        strcpy(strName, "AUTO_");
        strcat(strName, strText);
        strcat(strName, "_");
        strcat(strName, LLayer_GetName( labelLayer, str, MAX_TDBFILE_NAME ));
        newCell = LCell_Find(pFile, strName);
        if(newCell == NULL)
            newCell = LCell_New(pFile, strName);
        else
        {
            LCell_Delete( newCell );
            newCell = LCell_New(pFile, strName);
        }

        //create the text
        LCell_MakeLogo( newCell,
                        strText,
                        textSize,
                        tmp,
                        LFALSE,
                        LFALSE,
                        LFALSE,
                        ref_x,
                        (LCoord)(textSize/2 - textSize*0.1538 + ref_y + cpt * delta),
                        LFALSE,
                        LTRUE,
                        LFALSE,
                        "",
                        "",
                        "",
                        NULL);

        numberObject = 0;
        for(LObject obj = LObject_GetList(newCell, tmp) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            obj_arr[numberObject] = obj;
            numberObject = numberObject + 1;
        }
        //shrink to a new layer
        LCell_BooleanOperation(newCell, LBoolOp_SHRINK, shrinkValue, obj_arr, numberObject, NULL, 0, tmpShrink, LFALSE);
        //find all the shrink object
        numberObjectShrink = 0;
        for(LObject obj = LObject_GetList(newCell, tmpShrink) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            obj_arr_shrink[numberObjectShrink] = obj;
            numberObjectShrink = numberObjectShrink + 1;
        }
        //made the XOR to the good layer in the new cell
        LCell_BooleanOperation(newCell, LBoolOp_XOR, 0, obj_arr, numberObject, obj_arr_shrink, numberObjectShrink, labelLayer, LTRUE);
        //create an instance in the previous cell
        transformation.translation.x = 0; //already on the good place in the newCell
        transformation.translation.y = 0;
        transformation.orientation = 0.0;
        transformation.magnification.num = 1;
        transformation.magnification.denom = 1;
        instanceCreated = LInstance_GenerateV(pCell, newCell, NULL);
        if(LInstance_Set_Ex99(newCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(1, 1)) == LStatusOK)
        {
            LInstance_SetName( newCell, instanceCreated, strName );
        }
        value = value + increment;
        cpt= cpt + 1;
    }

//generate the last value = stopNumber

    itoa(value, strText, 10);
    //create a new cell
    strcpy(strName, "AUTO_");
    strcat(strName, strText);
    strcat(strName, "_");
    strcat(strName, LLayer_GetName( labelLayer, str, MAX_TDBFILE_NAME ));
    newCell = LCell_Find(pFile, strName);
    if(newCell == NULL)
        newCell = LCell_New(pFile, strName);
    else
    {
        LCell_Delete( newCell );
        newCell = LCell_New(pFile, strName);
    }

    //creating the text
    LCell_MakeLogo( newCell,
                        strText,
                        textSize,
                        tmp,
                        LFALSE,
                        LFALSE,
                        LFALSE,
                        ref_x,
                        (LCoord)(textSize/2 - textSize*0.1538 + ref_y + cpt * delta),
                        LFALSE,
                        LTRUE,
                        LFALSE,
                        "",
                        "",
                        "",
                        NULL);

        numberObject = 0;
        for(LObject obj = LObject_GetList(newCell, tmp) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            obj_arr[numberObject] = obj;
            numberObject = numberObject + 1;
        }
        //shrink to a new layer
        LCell_BooleanOperation(newCell, LBoolOp_SHRINK, shrinkValue, obj_arr, numberObject, NULL, 0, tmpShrink, LFALSE);
        //find all the shrink object
        numberObjectShrink = 0;
        for(LObject obj = LObject_GetList(newCell, tmpShrink) ; obj != NULL; obj = LObject_GetNext(obj) )
        {
            obj_arr_shrink[numberObjectShrink] = obj;
            numberObjectShrink = numberObjectShrink + 1;
        }
        //made the XOR to the good layer in the new cell
        LCell_BooleanOperation(newCell, LBoolOp_XOR, 0, obj_arr, numberObject, obj_arr_shrink, numberObjectShrink, labelLayer, LTRUE);
    //create an instance in the previous cell
    transformation.translation.x = 0;
    transformation.translation.y = 0;
    transformation.orientation = 0.0;
    transformation.magnification.num = 1;
    transformation.magnification.denom = 1;
    instanceCreated = LInstance_GenerateV(pCell, newCell, NULL);
    if(LInstance_Set_Ex99(newCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(1, 1)) == LStatusOK)
    {
        LInstance_SetName( newCell, instanceCreated, strName );
    }


    LInstance inst;
    for( inst = LInstance_GetList(pCell); inst != NULL; inst = LInstance_GetNext(inst))
    {
        LCell tmpCell = LInstance_GetCell( inst );
        LCell_GetName( tmpCell, strName, MAX_TDBFILE_NAME );
        LInstance_SetName( pCell, inst, strName );
    }

    for(LObject obj = LObject_GetList(pCell, tmpPreviousPolygons) ; obj != NULL; obj = LObject_GetNext(obj) )
        LObject_ChangeLayer(pCell, obj, labelLayer );

    //delete the tmp layer
    for(LObject obj = LObject_GetList(pCell, tmp) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmp );

    for(LObject obj = LObject_GetList(pCell, tmpShrink) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpShrink );

    for(LObject obj = LObject_GetList(pCell, tmpPreviousPolygons) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpPreviousPolygons );

}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F4" /*hotkey*/, "AEPONYX\\Automatic Numerotation with empty number (E-Beam) Macro\nWindow", "AutomaticNumerotationEmptyNumberMacro", NULL /*hotkey category*/);
	return 1;
}