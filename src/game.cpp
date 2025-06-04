#include <windows.h>
#include "gmi2x-cl.hpp"
DWORD uo_game_mp;

#include <stdint.h>
#include <math.h>

//typedef void (*Jump_Height_Set_t)(float height);
//#define Jump_Height_Set ((Jump_Height_Set_t)GAME_OFF(0x20008A20))


extern cvar_t* g_bounce;
extern cvar_t* jump_height;

/*by xoxor4d*/
void G_PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out)
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
void G_PM_ProjectVelocity(vec3_t in, vec3_t normal, vec3_t out)
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
extern "C" __stdcall uint32_t G_PM_Bounce(vec3_t in, vec3_t normal, vec3_t out)
{
	if (g_bounce->integer)
	{
		G_PM_ProjectVelocity(in, normal, out);
	}
	else
	{
		G_PM_ClipVelocity(in, normal, out);
	}
	return GAME_OFF(0x2000f39c);
}

__attribute__((naked)) void G_PM_Bounce_Stub()
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
        : "r"( (void*)G_PM_Bounce )
        : "eax", "ecx", "edx", "esi"
    );
}

/**/

/*void G_Jump_Height()
{
	float height = jump_height->value;
	Jump_Height_Set(height);
}*/

/**/

void G_Init(DWORD base)
{
	uo_game_mp = base;

	__jmp(GAME_OFF(0x2000f397), (int)G_PM_Bounce_Stub);
	//__call(GAME_OFF(0x20008B22), (int)G_Jump_Height);
}