// src/nuzlocke.c
#include "global.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/region_map_sections.h"
#include "overworld.h"
#include "event_data.h"

EWRAM_DATA u8 gNuzlockeReleaseSlots[PARTY_SIZE];

// Nuzlocke capture flags - Using a large enough range for all map sections
// MAPSEC_COUNT is around 220+, so we need at least 220 flags
// Using range starting at 0x1000 (4096) which is safely beyond normal game flags
#define NUZLOCKE_ENCOUNTER_FLAGS_START 0x1000

void Nuzlocke_ClearReleaseQueue(void)
{
    memset(gNuzlockeReleaseSlots, 0, sizeof(gNuzlockeReleaseSlots));
}

void Nuzlocke_QueueReleaseSlot(u8 slot)
{
    if (slot < PARTY_SIZE)
        gNuzlockeReleaseSlots[slot] = 1;
}

static bool32 PartyWouldBecomeEmptyAfterReleases(void)
{
    s32 i;
    s32 survivors = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG);
        if (species != SPECIES_NONE && species != SPECIES_EGG)
        {
            if (!gNuzlockeReleaseSlots[i])
                survivors++;
        }
    }
    return (survivors == 0);
}

void Nuzlocke_ProcessReleases_AfterBattle(u32 battleTypeFlags)
{
    s32 i;

    // Não mexer em Link battle (desync)
    if (battleTypeFlags & BATTLE_TYPE_LINK)
    {
        Nuzlocke_ClearReleaseQueue();
        return;
    }

    // Segurança: não deixar a party virar vazia (por enquanto)
    if (PartyWouldBecomeEmptyAfterReleases())
    {
        Nuzlocke_ClearReleaseQueue();
        return;
    }

    for (i = PARTY_SIZE - 1; i >= 0; i--)
    {
        if (gNuzlockeReleaseSlots[i])
            ZeroMonData(&gPlayerParty[i]);
    }

    CompactPartySlots();
    Nuzlocke_ClearReleaseQueue();
}

// =============================================================================
// NUZLOCKE CAPTURE SYSTEM - One catch per area
// =============================================================================

// Mark that the current area has had a wild encounter
// This should be called when a wild battle ends
void Nuzlocke_MarkAreaAsEncountered(void)
{
    u16 mapsec = GetCurrentRegionMapSectionId();
    
    // Only mark if Nuzlocke mode is active
    if (!FlagGet(FLAG_SYS_NUZLOCKE_MODE))
        return;
    
    // Validate map section ID
    if (mapsec >= MAPSEC_COUNT)
        return;
    
    // Set the flag for this area
    FlagSet(NUZLOCKE_ENCOUNTER_FLAGS_START + mapsec);
}

// Check if the current area has already had a wild encounter
bool32 Nuzlocke_HasEncounteredInArea(void)
{
    u16 mapsec = GetCurrentRegionMapSectionId();
    
    // Validate map section ID
    if (mapsec >= MAPSEC_COUNT)
        return FALSE;
    
    // Check the flag for this area
    return FlagGet(NUZLOCKE_ENCOUNTER_FLAGS_START + mapsec);
}

// Check if catching is allowed in the current area
// Returns TRUE if catching is allowed (no encounter yet OR nuzlocke mode is OFF)
// Returns FALSE if an encounter has already happened AND nuzlocke mode is ON
bool32 Nuzlocke_CanCatchInArea(void)
{
    // If Nuzlocke mode is not active, always allow catching
    if (!FlagGet(FLAG_SYS_NUZLOCKE_MODE))
        return TRUE;
    
    // Nuzlocke mode is active: check if already had encounter in this area
    return !Nuzlocke_HasEncounteredInArea();
}
