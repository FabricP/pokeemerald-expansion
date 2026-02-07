// src/nuzlocke.c
#include "global.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "battle.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/region_map_sections.h"
#include "overworld.h"
#include "event_data.h"
#include "pokedex.h"

EWRAM_DATA u8 gNuzlockeReleaseSlots[PARTY_SIZE];
EWRAM_DATA bool32 gNuzlockeDupeEncountered;  // Tracks if current encounter is a dupe

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
// NUZLOCKE CAPTURE SYSTEM - One catch per area + Clauses
// =============================================================================

// Mark that the current area has had a wild encounter
// This should be called when a wild battle ends
// EXCEPTION: Won't mark if it was a Dupe and player fled (Dupe Clause)
//            Will mark if it was a Dupe but player captured (encounter consumed)
void Nuzlocke_MarkAreaAsEncountered(void)
{
    u16 mapsec = GetCurrentRegionMapSectionId();

    // Only mark if Nuzlocke mode is active
    if (!FlagGet(FLAG_SYS_NUZLOCKE_MODE))
        return;

    // Validate map section ID
    if (mapsec >= MAPSEC_COUNT)
        return;

    // If this was a dupe encounter that was fled, don't mark the area
    // This allows the player to attempt another catch in this area
    if (gNuzlockeDupeEncountered)
    {
        // But if the pokemon was actually captured (even if dupe), mark the area
        // because the encounter was used up by the capture
        if (gBattleResults.caughtMonSpecies == SPECIES_NONE)
        {
            gNuzlockeDupeEncountered = FALSE;  // Reset for next encounter
            return;
        }
        // If pokemon was caught, fall through to mark the area
    }

    // Set the flag for this area (encountered)
    FlagSet(NUZLOCKE_ENCOUNTER_FLAGS_START + mapsec);
    gNuzlockeDupeEncountered = FALSE;  // Reset dupe flag after marking
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

// Check if pokemon is shiny
// A pokemon is shiny if the xor of personality and OT ID produces specific bits
bool32 Nuzlocke_IsShinyPokemon(struct Pokemon *pokemon)
{
    if (pokemon == NULL)
        return FALSE;
    
    u32 personality = GetMonData(pokemon, MON_DATA_PERSONALITY);
    u32 otId = GetMonData(pokemon, MON_DATA_OT_ID);
    
    // Shiny checking algorithm (xor of personality and OT ID)
    u16 otIdHigh = otId >> 16;
    u16 otIdLow = otId & 0xFFFF;
    u16 xorValue = (personality >> 16) ^ (personality & 0xFFFF) ^ otIdHigh ^ otIdLow;
    
    // If xor < 8, it's shiny
    return (xorValue < 8);
}

// Check if a species is already in the Pokedex (caught before)
// Used for Dupe Clause - returns TRUE if species was already caught
bool32 Nuzlocke_IsSpeciesDuplicate(u16 species)
{
    // Check the Pokedex to see if this species was caught before
    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;
    
    // Get the National Pokedex number and check if it was caught
    u16 natDexNo = SpeciesToNationalPokedexNum(species);
    return GetSetPokedexFlag(natDexNo, FLAG_GET_CAUGHT) != 0;
}

// Check and register if current encounter is a dupe
// Should be called at the start of a wild battle
void Nuzlocke_CheckDupeEncounter(void)
{
    u16 enemySpecies;
    
    gNuzlockeDupeEncountered = FALSE;  // Reset
    
    // If Nuzlocke mode is not active, ignore
    if (!FlagGet(FLAG_SYS_NUZLOCKE_MODE))
        return;
    
    // Get the species of the wild pokemon
    enemySpecies = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES_OR_EGG);
    
    if (enemySpecies != SPECIES_NONE && enemySpecies != SPECIES_EGG)
    {
        // Check if this species was already caught (Dupe Clause via Pokedex)
        if (Nuzlocke_IsSpeciesDuplicate(enemySpecies))
        {
            gNuzlockeDupeEncountered = TRUE;
        }
    }
}

// Check if catching is allowed in the current area
// Returns TRUE if catching is allowed (no encounter yet OR nuzlocke mode is OFF)
// Returns FALSE if an encounter has already happened AND nuzlocke mode is ON
// EXCEPTION: Always allows if pokemon is shiny (Shiny Clause)
bool32 Nuzlocke_CanCatchInArea(void)
{
    // If Nuzlocke mode is not active, always allow catching
    if (!FlagGet(FLAG_SYS_NUZLOCKE_MODE))
        return TRUE;
    
    // Check if the enemy pokemon is shiny - if yes, ALWAYS allow catching (Shiny Clause)
    if (Nuzlocke_IsShinyPokemon(&gEnemyParty[0]))
        return TRUE;
    
    // Nuzlocke mode is active: check if already had encounter in this area
    return !Nuzlocke_HasEncounteredInArea();
}
