#include <unistd.h> //getcwd

#include <ldata.h>
#include <string.h>

void AutomaticNumerotationMacro()
{

}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Automatic Numerotation Macro", "AutomaticNumerotationMacro", NULL /*hotkey category*/);
	return 1;
}