#pragma once

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#include "gta2.h"
#pragma pack(pop)   /* restore original alignment from stack */

#pragma region asserts
static_assert(sizeof(Ped) == 0x294, "Wrong size of Ped struct");
static_assert(offsetof(struct Ped, health) == 0x216, "Wrong offset of health in Ped struct");
#pragma endregion

#pragma region gta
#define pGame 0x005eb4fc
#define pFrontend 0x005eb160

typedef Ped* (__stdcall GetPedById)(int);
static GetPedById* fnGetPedByID = (GetPedById*)0x0043ae10;

// 0045c1f0 - void __fastcall GameTick(Game *game)
#pragma endregion
