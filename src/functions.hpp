#pragma once

#include <windows.h>

#define MAX_MSGLEN 16384


#define CVAR_ARCHIVE        1   // set to cause it to be saved to vars.rc
#define CVAR_USERINFO       2   // sent to server on connect or change
#define CVAR_SERVERINFO     4   // sent in response to front end requests
#define CVAR_SYSTEMINFO     8   // these cvars will be duplicated on all clients
#define CVAR_INIT           16  // don't allow change from console at all,
#define CVAR_LATCH          32  // will only change when C code next does

#define CVAR_ROM            64  // display only, cannot be set by user at all
#define CVAR_USER_CREATED   128 // created by a set command
#define CVAR_TEMP           256 // can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT          512 // can not be changed if cheats are disabled
#define CVAR_NORESTART      1024    // do not clear when a cvar_restart is issued
#define CVAR_WOLFINFO       2048    // DHM - NERVE :: Like userinfo, but for wolf multiplayer info

#define CVAR_UNSAFE         4096    // ydnar: unsafe system cvars (renderer, sound settings, anything that might cause a crash)
#define CVAR_SERVERINFO_NOUPDATE        8192    // gordon: WONT automatically send this to clients, but server browsers will see it



typedef enum
{
	qfalse,
	qtrue
} qboolean;

typedef enum
{
	NA_BOT,
	NA_BAD, // an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
} netadrtype_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	netadrtype_t type;
	union
	{
		int _ip;
		unsigned char ip[4];
	};
	unsigned char ipx[10];
	unsigned short port;
} netadr_t;

typedef enum
{
	ERR_FATAL,				// exit the entire game with a popup window
	//ERR_VID_FATAL,		// exit the entire game with a popup window and doesn't delete profile.pid
	ERR_DROP,				// print to console and disconnect from game
	ERR_SERVERDISCONNECT,	// don't kill server
	ERR_DISCONNECT,			// client disconnected from the server
	ERR_NEED_CD,			// pop up the need-cd dialog
	ERR_AUTOUPDATE
} errorParm_t;


typedef struct cvar_s
{
	char* name;
	char* string;
	char* resetString; // cvar_restart will reset to this value
	char* latchedString; // for CVAR_LATCH vars
	int flags;
	qboolean modified; // set each time the cvar is changed
	int modificationCount; // incremented each time the cvar is changed
	float value; // atof( string )
	int integer; // atoi( string )
	struct cvar_s* next;
	struct cvar_s* hashNext;
} cvar_t;


typedef void(*Cvar_Set_t)(const char*, const char*);
typedef cvar_t* (*Cvar_Get_t)(const char*, const char*, int);

typedef cvar_t* (*Cvar_FindVar_t)(const char*);

extern Cvar_Set_t Cvar_Set;
extern Cvar_Get_t Cvar_Get;

extern Cvar_FindVar_t Cvar_FindVar;

char* Cvar_VariableString(const char*);
int Cvar_VariableIntegerValue(const char* var_name);
float Cvar_VariableValue(const char *var_name);

//extern DWORD uo_game_mp;
extern DWORD uo_cgame_mp;

#define GAME_OFF(x) (uo_game_mp + (x - 0x20000000))
#define CGAME_OFF(x) (uo_cgame_mp + (x - 0x30000000))


typedef void(*Com_Printf_t)(const char*, ...);
extern Com_Printf_t Com_Printf;
typedef void(*Com_DPrintf_t)(const char*, ...);
extern Com_DPrintf_t Com_DPrintf;


typedef void(*Com_Error_t)(int, const char*, ...);
extern Com_Error_t Com_Error;

typedef void(*xfunc)(void);

typedef void(*Cmd_AddCommand_t)(const char*, xfunc);
extern Cmd_AddCommand_t Cmd_AddCommand;

char* va(const char* format, ...);