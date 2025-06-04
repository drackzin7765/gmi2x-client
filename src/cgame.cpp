#include "client.hpp"
#include "gmi2x-cl.hpp"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

DWORD uo_cgame_mp;

extern cvar_t* cg_drawWeaponSelection;
void _CG_DrawWeaponSelect()
{
	if (cg_drawWeaponSelection->integer)
	{
		void(*CG_DrawWeaponSelect)(void);
		*(int*)&CG_DrawWeaponSelect = CGAME_OFF(0x30046bb0);
		CG_DrawWeaponSelect();
	}
}

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

			const char *s = NULL;

			if (fps <= 59) {
    			s = va("^1FPS: %i", fps);
			} else {
    			s = va("FPS: %i", fps);
			}


			//const char* s = va("%ifps", fps);

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



#include <stdint.h>
#include <math.h>
/*by xoxor4d*/
void PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out)
{
	float backoff;
	float change;
	int i;
	float overbounce = 1.001f;

	backoff = DotProduct(in, normal);
	if (backoff < 0)
	{
		backoff *= overbounce;
	}
	else
	{
		backoff /= overbounce;
	}

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
	}
}
void PM_ProjectVelocity(vec3_t in, vec3_t normal, vec3_t out)
{
	float speedXY, DotNormalXY, normalisedNormalXY, projectionZ, projectionXYZ;
	speedXY = in[1] * in[1] + in[0] * in[0];
	if ((normal[2]) < 0.001f || (speedXY == 0.0f))
	{
		VectorCopy(in, out);
	}
	else
	{
		DotNormalXY = normal[1] * in[1] + in[0] * normal[0];
		normalisedNormalXY = -DotNormalXY / normal[2];
		projectionZ = in[2] * in[2] + speedXY;
		projectionXYZ = sqrtf((projectionZ / (speedXY + normalisedNormalXY * normalisedNormalXY)));

		if (projectionXYZ < 1.0f || normalisedNormalXY < 0.0f || in[2] > 0.0f)
		{
			out[0] = projectionXYZ * in[0];
			out[1] = projectionXYZ * in[1];
			out[2] = projectionXYZ * normalisedNormalXY;
		}
	}
}
extern "C" __stdcall uint32_t PM_Bounce(vec3_t in, vec3_t normal, vec3_t out)
{
	int x_cl_bounce = atoi(Info_ValueForKey(cs1, "x_cl_bounce"));
	if (x_cl_bounce)
	{
		PM_ProjectVelocity(in, normal, out);
	}
	else
	{
		PM_ClipVelocity(in, normal, out);
	}
	return CGAME_OFF(0x3000F5EC);
	return 0;
}


__attribute__((naked)) void PM_Bounce_Stub()
{
	__asm__ __volatile__ (
        "push %%esi\n\t"
        "push %%ecx\n\t"
        "push %%edx\n\t"
        "call *%0\n\t"
        "add $12, %%esp\n\t"
        "push %%eax\n\t"
        "ret\n\t"
        :
        : "r"( (void*)PM_Bounce )
        : "eax", "ecx", "edx", "esi"
    );
}



/*

	__asm
	{
		push esi; // out
		push ecx; // normal
		push edx; // in
		call PM_Bounce;
		add esp, 12;
		push eax
			retn;
	}

*/


/*int Jump_Height_bug(float height) {
	const char* jump_height = Info_ValueForKey(cs1, "jump_height");
	if (strlen(jump_height)) {
		height = atof(jump_height);
	}


	int(*call)(float);
	*(int*)(&call) = CGAME_OFF(0x30008C70);
	int result = call(height);
	return result;
}*/

void CG_Init(DWORD base) {
	uo_cgame_mp = base;
	
	__jmp(CGAME_OFF(0x3000F5E7), (int)PM_Bounce_Stub); // bloody hell pls work

	__call(CGAME_OFF(0x3001875c), (int)_CG_DrawFPS);

	__call(CGAME_OFF(0x3001c06f), (int)_CG_DrawWeaponSelect);

	//__call(CGAME_OFF(0x30008D72), (int)Jump_Height_bug);
	*(UINT32*)CGAME_OFF(0x3008523C) = CVAR_ARCHIVE; // Enable cg_fov
	*(UINT32*)CGAME_OFF(0x3008570C) = CVAR_ARCHIVE; // Unlock cg_thirdpersonangle
	*(UINT32*)CGAME_OFF(0x3008571C) = CVAR_ARCHIVE; // Unlock cg_thirdperson

}