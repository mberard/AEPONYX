#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>
#include <stdlib.h>

#include <regex.h> //may be usefull to delete all the cell
// see: https://nicolasj.developpez.com/articles/regex/
// str_regex : "^T_C[[:digit:]]{2,3}T[[:digit:]]{2,3}"

// global variables
char strName[MAX_TDBFILE_NAME];
LCell	pCell;
LFile	pFile;
double dbuPerMicrons;
LLayer labelLayer;   
   
   
void LabelMacro()
{
	pCell = LCell_GetVisible();
	pFile = LCell_GetFile(pCell);
	dbuPerMicrons = LFile_MicronsToIntU(pFile, 1);
	
	char *Pick_List [ ] = {
		"Add labels",
   	"Delete existing labels",
   	"Modify existing labels"
   };
   int Pick_Count = 3;
   int Picked;
   
   Picked = LDialog_PickList ("Select Element", Pick_List, Pick_Count, 0);
   switch (Picked)
   { 
   	case -1:
   		LUpi_LogMessage(LFormat("Pick -1\n"));
			return;
      case 0:
      	LUpi_LogMessage(LFormat("Pick 0\n"));
      	AddLabels();
      	break;
    	case 1:
    		LUpi_LogMessage(LFormat("Pick 1\n"));
    		RemoveLabels();
      	break;
     	case 2:
    		LUpi_LogMessage(LFormat("Pick 2\n"));
    		ModifyLabels();
      default:
      	LUpi_LogMessage(LFormat("Invalid selection\n"));
    }   	
}    

void AddLabels ()
{
	LUpi_LogMessage(LFormat("Adding Labels"));
	return;
}

void RemoveLabels ()
{
	LUpi_LogMessage(LFormat("Deleting Labels"));
	return;
}

void ModifyLabels()
{
	LCoord ref_x, ref_y;

	
	if(LLabel_GetList(pCell)) //if there are some labels
	{
   	for(LLabel pLab = LLabel_GetList(pCell); pLab != NULL; pLab = LLabel_GetNext(pLab) ) //for each label
      {
			LLabel_GetName(pLab, strName, MAX_TDBFILE_NAME);
			LUpi_LogMessage(LFormat("Label found %s, modifying third character %c\n to 3",strName, strName[2]));
			LPoint point = LLabel_GetPosition(pLab);
			labelLayer = LLabel_GetLayer(pLab);
			ref_x = point.x;
			ref_y = point.y;
			
			// modification
         //strName[2] =  '2';
			//LLabel_SetName(pCell, pLab, strName, dbuPerMicrons*25);
		}
	}	
	else
   {
		LUpi_LogMessage(LFormat("No Label found %s\n",strName));
	}
	return;
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30(NULL, "F2" /*hotkey*/, "AEPONYX\\Labels\nWindow", "LabelMacro", NULL /*hotkey category*/);
	return 1;
}

/*
int RegexFind (void) {
	int err;
   regex_t preg;
	const char *str_request = "http://www.developpez.net/forums/index.php";
	const char *str_regex = "(www\\.[-_[:alnum:]]+\\.[[:lower:]]{2,4})";
	
   err = regcomp (&preg, str_regex, REG_EXTENDED);
   if (err == 0)
   {
		int match;
		size_t nmatch = 0;
		regmatch_t *pmatch = NULL;
		nmatch = preg.re_nsub;
		pmatch = malloc (sizeof (*pmatch) * nmatch);
		if (pmatch)
		{
			
			match = regexec (&preg, str_request, nmatch, pmatch, 0);
			
			regfree (&preg);
			
			if (match == 0)
			{
				char *site = NULL;
				int start = pmatch[0].rm_so;
				int end = pmatch[0].rm_eo;
				size_t size = end - start;
				site = malloc (sizeof (*site) * (size + 1));
				if (site)
				{
					strncpy (site, &str_request[start], size);
					site[size] = '\0';
					LUpi_LogMessage(LFormat ("%s\n", site));
					free (site);
				}
			}
			
			else if (match == REG_NOMATCH)
			{
				LUpi_LogMessage(LFormat("%s n\'est pas une adresse internet valide\n", str_request));
			}
			
			else
			{
				char *text;
				size_t size;
				
				size = regerror (err, &preg, NULL, 0);
				text = malloc (sizeof (*text) * size);
				if (text)
				{
					
					regerror (err, &preg, text, size);
					LUpi_LogMessage(LFormat("%s\n", text));
					free (text);
				}
				else
				{
					LUpi_LogMessage(LFormat("Memoire insuffisante\n"));
					exit (EXIT_FAILURE);
				}
			}
		}
      else
      {
			LUpi_LogMessage(LFormat("Memoire insuffisante\n"));
			exit (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}
*/