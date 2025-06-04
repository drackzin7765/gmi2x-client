#include "client.hpp"
#include "gmi2x-cl.hpp"
#include "cl_command.hpp"

cvar_t* cg_drawFPS;
cvar_t* cg_drawFPS_x;
cvar_t* cg_drawFPS_y;
cvar_t* g_bounce;
cvar_t* cg_drawWeaponSelection;

void _CL_Init(void)
{
	void(*CL_Init)();
	* (int*)(&CL_Init) = 0x00414500;
	CL_Init();

    Cvar_Set("version", va("[GMI2x-CL] CODUO MP 1.51 build %d %s %s win-x86", 10000, __DATE__, __TIME__));

	Cvar_Get("gmi2x-cl", "1", CVAR_USERINFO | CVAR_ROM);

	cg_drawFPS = Cvar_Get("cg_drawFPS", "0", CVAR_ARCHIVE);
	cg_drawFPS_x = Cvar_Get("cg_drawFPS_x", "523", CVAR_ARCHIVE);
	cg_drawFPS_y = Cvar_Get("cg_drawFPS_y", "2", CVAR_ARCHIVE);
	g_bounce = Cvar_Get("g_bounce", "0", CVAR_ARCHIVE);
	cg_drawWeaponSelection = Cvar_Get("cg_drawWeaponSelection", "1", CVAR_ARCHIVE);

	Cvar_Set("com_hunkmegs", "512");
	
	//Cmd_AddCommand("minimize", Cmd_Minimize);
	Cmd_AddCommand("lookback", Cmd_LookBack);
	Cmd_AddCommand("minimize", Cmd_Minimize);
}