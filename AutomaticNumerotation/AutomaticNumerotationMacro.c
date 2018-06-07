#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>


void AutomaticNumerotationMacro()
{
    LCell	pCell	=	LCell_GetVisible();
	LFile	pFile	=	LCell_GetFile(pCell);
    LLayer pLayer;
  
    char strNameWanted[MAX_TDBFILE_NAME];
    char strName[MAX_TDBFILE_NAME];
    char buffer[10];

    int hasLabel = 0;
    LLayer labelLayer;
    LCoord ref_x, ref_y;
    float delta_x, delta_y;
    int nbLine, nbCol;

    LCell bigCell;
    LCell smallCell;
    LInstance instanceCreated;

    LTransform_Ex99 transformation;

    strcpy(strNameWanted, "die_id"); //preloaded text in the dialog box
	if ( LDialog_InputBox("Layer", "What is the name of the origin label", strNameWanted) == 0)
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

    delta_x = 1000000;
    delta_y = 1000000;
    nbLine = 3;
    nbCol = 3;

    bigCell = LCell_New(pFile, "AutomaticNumerotation");

    for(int i = 0; i<nbLine; i++)
    {
        for(int j = 0; j<nbLine; j++)
        {
            strcpy(strName, "");
            strcat(strName, "C");
            if(j+1 < 10)
                strcat(strName, "0");
            itoa(j+1, buffer, 10);
            strcat(strName, buffer);
            strcat(strName, "L");
            if(i+1 < 10)
                strcat(strName, "0");
            itoa(i+1, buffer, 10);
            strcat(strName, buffer);
            smallCell = LCell_New(pFile, strName);

            LCell_MakeLogo( smallCell, 
                            strName, 
                            LFile_MicronsToIntU(pFile, 5), 
                            labelLayer, 
                            LFALSE, 
                            LFALSE, 
                            LFALSE, 
                            0, 
                            0, 
                            LFALSE, 
                            LTRUE, 
                            LFALSE, 
                            "", 
                            "", 
                            "", 
                            NULL );

            transformation.translation.x = j*delta_x;
            transformation.translation.y = i*delta_y;
            transformation.orientation = 0.0;
            transformation.magnification.num = 1;
            transformation.magnification.denom = 1;
            instanceCreated = LInstance_GenerateV(bigCell, smallCell, NULL);
			if(LInstance_Set_Ex99(smallCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(delta_x, delta_y)) == LStatusOK)
			{
				LInstance_SetName( smallCell, instanceCreated, strName );
            }
            //LInstance_New_Ex99(smallCell, bigCell, transformation, LPoint_Set(1,1), LPoint_Set(delta_x,delta_y));
        }
    }

    transformation.translation.x = ref_x;
    transformation.translation.y = ref_y;
    transformation.orientation = 0;
    transformation.magnification.num = 1;
    transformation.magnification.denom = 1;
    instanceCreated = LInstance_GenerateV(pCell, bigCell, NULL);
	if(LInstance_Set_Ex99(smallCell, instanceCreated, transformation, LPoint_Set(1, 1), LPoint_Set(delta_x, delta_y)) == LStatusOK)
	{
		LInstance_SetName( pCell, instanceCreated, strName );
    }
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Automatic Numerotation Macro", "AutomaticNumerotationMacro", NULL /*hotkey category*/);
	return 1;
}