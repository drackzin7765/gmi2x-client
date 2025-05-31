#include "client.hpp"
#include "cuocx.hpp"
#include "cl_command.hpp"

void _CL_Init(void)
{
	void(*CL_Init)();
	* (int*)(&CL_Init) = 0x00414500;
	CL_Init();

    Cvar_Set("version", va("[CUOCX] CODUO MP 1.51 build %d %s %s win-x86", 10000, __DATE__, __TIME__));

	Cvar_Get("coduoxt", "1", CVAR_USERINFO | CVAR_ROM);

	//Cmd_AddCommand("minimize", Cmd_Minimize);
	Cmd_AddCommand("lookback", Cmd_LookBack);
}