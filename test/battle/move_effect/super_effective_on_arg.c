#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_FREEZE_DRY) == EFFECT_SUPER_EFFECTIVE_ON_ARG);
    ASSUME(GetMoveEffect(MOVE_ABRASIVE_ROCK) == EFFECT_SUPER_EFFECTIVE_ON_ARG);
    ASSUME(GetMoveType(MOVE_ABRASIVE_ROCK) == TYPE_ROCK);
    ASSUME(GetMoveArgType(MOVE_ABRASIVE_ROCK) == TYPE_STEEL);
}

SINGLE_BATTLE_TEST("Freeze Dry is super effective on water types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SHELLDER);
    } WHEN {
        TURN { MOVE(player, MOVE_FREEZE_DRY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FREEZE_DRY, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Abrasive Rock is super effective on Steel types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KLINK);
    } WHEN {
        TURN { MOVE(player, MOVE_ABRASIVE_ROCK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABRASIVE_ROCK, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}
