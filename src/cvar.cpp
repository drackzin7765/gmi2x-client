#include "functions.hpp"

Cvar_Get_t Cvar_Get = (Cvar_Get_t)0x43D9E0;
Cvar_Set_t Cvar_Set = (Cvar_Set_t)0x43DC50;
Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x43D8F0;

float Cvar_VariableValue(const char *var_name) {
	cvar_t  *var;

	var = Cvar_FindVar(var_name);
	if (!var) {
		return 0;
	}
	return var->value;
}

int Cvar_VariableIntegerValue(const char *var_name) {
	cvar_t  *var;

	var = Cvar_FindVar(var_name);
	if (!var) {
		return 0;
	}
	return var->integer;
}

char *Cvar_VariableString(const char *var_name) {
	cvar_t *var;

	var = Cvar_FindVar(var_name);
	if (!var) {
		return (char*)"";
	}
	return var->string;
}
