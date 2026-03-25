#include "global.h"
#include "test/battle.h"

// Test that Truant ability prevents attacking on alternating turns
SINGLE_BATTLE_TEST("Truant: Prevents attack on loafing turn")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    }
    WHEN {
        TURN { MOVE(PLAYER, MOVE_TACKLE); MOVE(OPPONENT, MOVE_TELEPORT); }
        TURN { MOVE(PLAYER, MOVE_TACKLE); MOVE(OPPONENT, MOVE_TELEPORT); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, PLAYER); // First turn - attack succeeds
        MESSAGE("Slaking is loafing around!"); // Second turn - loafing
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, PLAYER); // No attack animation
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, PLAYER); // Third turn - attack succeeds again
    }
}

// Test that Slack Off can be used during Truant's loafing turns
SINGLE_BATTLE_TEST("Truant: Slack Off can be used during loafing turn")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); HP(50); MaxHP(100); Moves(MOVE_SLACK_OFF, MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    }
    WHEN {
        TURN { MOVE(PLAYER, MOVE_TACKLE); MOVE(OPPONENT, MOVE_TELEPORT); }
        TURN { MOVE(PLAYER, MOVE_SLACK_OFF); MOVE(OPPONENT, MOVE_TELEPORT); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, PLAYER); // First turn - attack succeeds
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLACK_OFF, PLAYER); // Second turn - Slack Off succeeds despite loafing
        MESSAGE("Slaking used Slack Off!"); // Confirmation message
    }
}

// Test alternating turns work correctly with Slack Off usage
SINGLE_BATTLE_TEST("Truant: Slack Off doesn't affect alternating turn pattern")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); HP(50); MaxHP(100); Moves(MOVE_SLACK_OFF, MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    }
    WHEN {
        TURN { MOVE(PLAYER, MOVE_TACKLE); MOVE(OPPONENT, MOVE_TELEPORT); } // Turn 1: attack
        TURN { MOVE(PLAYER, MOVE_SLACK_OFF); MOVE(OPPONENT, MOVE_TELEPORT); } // Turn 2: loaf (but use Slack Off)
        TURN { MOVE(PLAYER, MOVE_TACKLE); MOVE(OPPONENT, MOVE_TELEPORT); } // Turn 3: attack
        TURN { MOVE(PLAYER, MOVE_SLACK_OFF); MOVE(OPPONENT, MOVE_TELEPORT); } // Turn 4: loaf
    }
    SCENE {
        // Turn 1: Normal attack
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, PLAYER);
        // Turn 2: Slack Off instead of loafing completely
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLACK_OFF, PLAYER);
        // Turn 3: Normal attack
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, PLAYER);
        // Turn 4: Loafing, but can use Slack Off again
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLACK_OFF, PLAYER);
    }
}
