#include "global.h"
#include "test/battle.h"

// Test that Truant ability prevents attacking on alternating turns
SINGLE_BATTLE_TEST("Truant: Prevents attack on loafing turn")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Speed(2); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { HP(1000); MaxHP(1000); Speed(1); }
    }
    WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player); // First turn - attack succeeds
        MESSAGE("Slaking is loafing around!"); // Second turn - loafing
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player); // No attack animation
        MESSAGE("Slaking used Tackle!"); // Third turn starts after the loafing turn
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player); // Third turn - attack succeeds again
    }
}

// Test that Slack Off can be used during Truant's loafing turns
SINGLE_BATTLE_TEST("Truant: Slack Off can be used during loafing turn")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Speed(2); HP(50); MaxHP(100); Moves(MOVE_SLACK_OFF, MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { HP(1000); MaxHP(1000); Speed(1); }
    }
    WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SLACK_OFF); MOVE(opponent, MOVE_TACKLE); }
    }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player); // First turn - attack succeeds
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLACK_OFF, player); // Second turn - Slack Off succeeds despite loafing
        HP_BAR(player, hp: 100);
    }
}

// Test alternating turns work correctly with Slack Off usage
SINGLE_BATTLE_TEST("Truant: Slack Off doesn't affect alternating turn pattern")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Speed(2); HP(50); MaxHP(100); Moves(MOVE_SLACK_OFF, MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { HP(1000); MaxHP(1000); Speed(1); }
    }
    WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); } // Turn 1: attack
        TURN { MOVE(player, MOVE_SLACK_OFF); MOVE(opponent, MOVE_TACKLE); } // Turn 2: loaf (but use Slack Off)
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); } // Turn 3: attack
        TURN { MOVE(player, MOVE_SLACK_OFF); MOVE(opponent, MOVE_CELEBRATE); } // Turn 4: loaf
    }
    SCENE {
        // Turn 1: Normal attack
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        // Turn 2: Slack Off instead of loafing completely
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLACK_OFF, player);
        // Turn 3: Normal attack
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        // Turn 4: Loafing, but can use Slack Off again
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLACK_OFF, player);
    }
}
