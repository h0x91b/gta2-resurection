#pragma once

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
#include "gta2.h"
#pragma pack(pop)   /* restore original alignment from stack */

#pragma region asserts
static_assert(sizeof(Ped) == 0x290, "Wrong size of Car struct");
static_assert(offsetof(struct Ped, health) == 0x216, "Wrong offset of health in Ped struct");
#pragma endregion

#pragma region gta
Game* game = cast(Game, 0x005eb4fc);

typedef Ped* (__stdcall GetPedById)(int);
static GetPedById* fnGetPedByID = (GetPedById*)0x0043ae10;

// 0045c1f0 - void __fastcall GameTick(Game *game)
#pragma endregion
