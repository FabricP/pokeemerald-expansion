// include/nuzlocke.h
#pragma once
#include "global.h"

void Nuzlocke_ClearReleaseQueue(void);
void Nuzlocke_QueueReleaseSlot(u8 slot);
void Nuzlocke_ProcessReleases_AfterBattle(u32 battleTypeFlags);

// Nuzlocke capture system - one catch per area
void Nuzlocke_MarkAreaAsEncountered(void);
bool32 Nuzlocke_HasEncounteredInArea(void);
bool32 Nuzlocke_CanCatchInArea(void);
