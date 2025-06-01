#include "cgame.hpp"
#include "client.hpp"
#include "cuocx.hpp"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

DWORD uo_cgame_mp;

#define	FPS_FRAMES 4
extern cvar_t* cg_drawFPS;
extern cvar_t* cg_drawFPS_x;
extern cvar_t* cg_drawFPS_y;

void _CG_DrawFPS(float y)
{
	if (cg_drawFPS->integer && cg_drawFPS->integer == 3)
	{
		//TODO: do like original code instead

		static int previousTimes[FPS_FRAMES];
		static int index;
		int	i, total;
		int	fps;
		static int previous;
		int	t, frameTime;

		t = timeGetTime();
		frameTime = t - previous;
		previous = t;
		previousTimes[index % FPS_FRAMES] = frameTime;
		index++;

		if (index > FPS_FRAMES)
		{
			total = 0;
			for (i = 0; i < FPS_FRAMES; i++)
			{
				total += previousTimes[i];
			}
			if (!total)
			{
				total = 1;
			}
			fps = 1000 * FPS_FRAMES / total;

			const char* s = va("%ifps", fps);

			void(*CG_DrawBigString)(float x, float y, const char* s, float alpha);
			*(int*)&CG_DrawBigString = CGAME_OFF(0x3001cf10);
			CG_DrawBigString(cg_drawFPS_x->integer, cg_drawFPS_y->integer, s, 1.0F);
		}
	}
	else
	{
		void(*CG_DrawFPS)(float);
		*(int*)&CG_DrawFPS = CGAME_OFF(0x30018090);
		CG_DrawFPS(y);
	}
}

void CG_Init(DWORD base) {
	uo_cgame_mp = base;
	
	
	__call(CGAME_OFF(0x3001875c), (int)_CG_DrawFPS);

	//__call(CGAME_OFF(0x30008D72), (int)JumpHeightCrap);
	*(UINT32*)CGAME_OFF(0x3008523C) = CVAR_ARCHIVE; // Enable cg_fov
	*(UINT32*)CGAME_OFF(0x3008570C) = CVAR_ARCHIVE; // Unlock cg_thirdpersonangle
	*(UINT32*)CGAME_OFF(0x3008571C) = CVAR_ARCHIVE; // Unlock cg_thirdperson

    Com_Printf("Hooked into CG_INIT \n");
}