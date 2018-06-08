#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NUMBER_WINDOWS 50

void AutomaticNumerotationMacro()
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;
  
    char strNameWanted[MAX_TDBFILE_NAME];
    char strName[MAX_TDBFILE_NAME];
    char strText[MAX_TDBFILE_NAME];
    char buffer[10];

    int hasLabel = 0;
    LLayer labelLayer;
    LCoord ref_x, ref_y;

    LDialogItem DialogItems1[2] = {{ "X (in microns)","100"}, { "Y (in microns)","100"}};
    LDialogItem DialogItems2[2] = {{ "Number of column","1"}, { "Number of line","1"}};

    double tmp1, tmp2;
    LCoord delta_x, delta_y;
    int nbLine, nbCol;
    LCoord textSize = 0;
    int cpt;

    LCell bigCell;
    LCell smallCell;
    LInstance instanceCreated;

    LTransform_Ex99 transformation;

    LWindow activeWindows[MAX_NUMBER_WINDOWS];
    int numberWindows = 0;
    int hasBeenFoundInArray = 0;

    LWindow pWindow = NULL;
    for( pWindow = LWindow_GetList(); Assigned(pWindow); pWindow = LWindow_GetNext(pWindow) )
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

    if(hasLabel == 0)
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

    if( ! (LCell_Find(pFile, "AutomaticNumerotation"))) //if not exist
        bigCell = LCell_New(pFile, "AutomaticNumerotation"); //create
    else
        bigCell = LCell_Find(pFile, "AutomaticNumerotation"); //point on the existing one

    for(LInstance inst = LInstance_GetList(bigCell); inst != NULL; inst = LInstance_GetNext(inst)) 
                LInstance_Delete(bigCell, inst); //deleting all the instance in bigCell
    
    for(int i = 0; i<nbLine; i++)
    {
        for(int j = 0; j<nbCol; j++)
        {
            strcpy(strText, "");
            strcat(strText, "C");
            if(j+1 < 10)
                strcat(strText, "0");
            itoa(j+1, buffer, 10);
            strcat(strText, buffer);
            strcat(strText, "L");
            if(i+1 < 10)
                strcat(strText, "0");
            itoa(i+1, buffer, 10);
            strcat(strText, buffer);

            strcpy(strName, "");
            strcat(strName, "T_");
            strcat(strName, strText);

            smallCell = LCell_Find(pFile, strName);
            if(smallCell == NULL)
                smallCell = LCell_New(pFile, strName);
            else
            {
                LCell_Delete( smallCell );
                smallCell = LCell_New(pFile, strName);
            }

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

            if(!(LInstance_Find(bigCell, strText))) //add the instance that not already exist
            {
                transformation.translation.x = j*delta_x;
                transformation.translation.y = i*delta_y;
                transformation.orientation = 0.0;
                transformation.magnification.num = 1;
                transformation.magnification.denom = 1;
                instanceCreated = LInstance_GenerateV(bigCell, smallCell, NULL);
                if(LInstance_Set_Ex99(smallCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(delta_x, delta_y)) == LStatusOK)
                {
                    LInstance_SetName( smallCell, instanceCreated, strText );
                }
            }

            for( pWindow = LWindow_GetList(); Assigned(pWindow); pWindow = LWindow_GetNext(pWindow) )
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
        }
    }

    if(!(LInstance_Find(pCell, "Die numerotation")))
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
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Automatic Numerotation Macro", "AutomaticNumerotationMacro", NULL /*hotkey category*/);
	return 1;
}