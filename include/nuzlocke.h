// include/nuzlocke.h
#pragma once
#include "global.h"
// Global variables
extern bool32 gNuzlockeDupeEncountered;  // Tracks if current encounter is a dupe
void Nuzlocke_ClearReleaseQueue(void);
void Nuzlocke_QueueReleaseSlot(u8 slot);
void Nuzlocke_ProcessReleases_AfterBattle(u32 battleTypeFlags);

// Nuzlocke capture system - one catch per area
void Nuzlocke_MarkAreaAsEncountered(void);
bool32 Nuzlocke_HasEncounteredInArea(void);
bool32 Nuzlocke_CanCatchInArea(void);
void Nuzlocke_CheckDupeEncounter(void);  // Check if encounter is dupe at battle start

// Dupe Clause - check if species was already caught (via Pokedex)
bool32 Nuzlocke_IsSpeciesDuplicate(u16 species);

// Shiny Clause - check if pokemon is shiny
bool32 Nuzlocke_IsShinyPokemon(struct Pokemon *pokemon);
