#include <unistd.h> //getcwd

//#define EXCLUDE_LEDIT_LEGACY_UPI

#include <ldata.h>
/* Begin -- Uncomment this block if you are using L-Comp. */
#include <lcomp.h>
/* End */
#include <string.h>

void TorusFillet(void)
{
    
}

int UPI_Entry_Point(void)
{
	LMacro_BindToMenuAndHotKey_v9_30("Tools", "F2" /*hotkey*/, "Torus to be fillet", "TorusFillet", NULL /*hotkey category*/);
	return 1;
}