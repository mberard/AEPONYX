#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <stdlib.h>

// #include <regex.h> //may be usefull to delete all the cell
// see: https://nicolasj.developpez.com/articles/regex/
// str_regex : "^T_C[[:digit:]]{2,3}T[[:digit:]]{2,3}"

#define MAX_NUMBER_OBJECT 500

void AutomaticNumerotationEmptyNumberMacro()
{
    LCell pCell	= LCell_GetVisible();
	LFile pFile	= LCell_GetFile(pCell);
    LLayer pLayer;
    LLayer tmpLayer;
    LLayer tmpLayerShrink;
  
    char strNameWanted[MAX_TDBFILE_NAME];
    char strName[MAX_TDBFILE_NAME];
    char strText[MAX_TDBFILE_NAME];
    char buffer[10];

    int hasLabel = 0;
    LLayer labelLayer;
    LCoord ref_x, ref_y;

    LDialogItem DialogItems[4] = {{ "Start number","0"}, { "increment","1"}, { "Stop number","0"}, { "Space between two numbers (in microns)","1000"}};

    double tmpDouble;
    LCoord delta;
    int startNumber, increment, stopNumber;
    LCoord textSize = 0;
    int tmp;
    double shrinkValue;
    int diff;
    int value;
    int cpt;

    LObject obj_arr[MAX_NUMBER_OBJECT];
    int numberObject = 0;
    LObject obj_arr_shrink[MAX_NUMBER_OBJECT];
    int numberObjectShrink = 0;

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

    if(hasLabel == 0)
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

        tmp = atoi(DialogItems[3].value);
        delta = LFile_MicronsToIntU(pFile, tmp);
    }
    else
        return;

    diff = abs(stopNumber - startNumber);
    while(diff > 0)
    {
        diff = diff - increment;
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
    LLayer_New( pFile, NULL, "tmpLayerNum");
    tmpLayer = LLayer_Find(pFile, "tmpLayerNum");

    LLayer_New( pFile, NULL, "tmpLayerShrink");
    tmpLayerShrink = LLayer_Find(pFile, "tmpLayerShrink");

    cpt = 0;
    value = startNumber;
    while(value != stopNumber)
    {
        itoa(value, strText, 10);

        LCell_MakeLogo( pCell,
                        strText,
                        textSize,
                        tmpLayer,
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
                        NULL );

        value = value + increment;
        cpt= cpt + 1;
    }
    //generate the last value = stopNumber
    itoa(value, strText, 10);
    LCell_MakeLogo( pCell,
                    strText,
                    textSize,
                    tmpLayer,
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
                    NULL );

    for(LObject obj = LObject_GetList(pCell, tmpLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        obj_arr[numberObject] = obj;
        numberObject = numberObject + 1;
    }

    LCell_BooleanOperation(pCell, LBoolOp_SHRINK, shrinkValue, obj_arr, numberObject, NULL, 0, tmpLayerShrink, LFALSE);

    for(LObject obj = LObject_GetList(pCell, tmpLayerShrink) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        obj_arr_shrink[numberObjectShrink] = obj;
        numberObjectShrink = numberObjectShrink + 1;
    }

    LCell_BooleanOperation(pCell, LBoolOp_XOR, 0, obj_arr, numberObject, obj_arr_shrink, numberObjectShrink, labelLayer, LTRUE);
/*
    for(LObject obj = LObject_GetList(pCell, tmpLayer) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpLayer );

    for(LObject obj = LObject_GetList(pCell, tmpLayerShrink) ; obj != NULL; obj = LObject_GetNext(obj) )
    {
        LObject_Delete( pCell, obj );
    }
    LLayer_Delete( pFile, tmpLayerShrink );
*/

}

int UPI_Entry_Point(void)
{
    LMacro_BindToMenuAndHotKey_v9_30(NULL, "F4" /*hotkey*/, "AEPONYX\\Automatic Numerotation with empty number (E-Beam) Macro\nWindow", "AutomaticNumerotationEmptyNumberMacro", NULL /*hotkey category*/);
	return 1;
}