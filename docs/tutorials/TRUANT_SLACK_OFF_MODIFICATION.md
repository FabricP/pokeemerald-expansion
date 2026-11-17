# Truant Ability + Slack Off Balance Modification

## Descrição da Mudança

Este documento descreve a modificação implementada na habilidade **Truant** para permitir que um Pokémon use o move **Slack Off** durante seus turnos de inatividade.

## Motivação

A habilidade Truant faz um Pokémon atacar um turno sim, um turno não. Essa modificação permite que o Pokémon use **Slack Off** (que recupera 50% do HP máximo) nos turnos em que normalmente estaria "preguiçoso/se loafando" (loafing around), fornecendo um balanceamento estratégico interessante.

## Implementação Técnica

### Arquivo Modificado
- **Arquivo**: `src/battle_util.c`
- **Função**: `CancelerTruant()` (linhas ~2173-2189)

### Mudança Específica

**Antes:**
```c
static enum MoveCanceler CancelerTruant(struct BattleContext *ctx)
{
    if (GetBattlerAbility(ctx->battlerAtk) == ABILITY_TRUANT && gDisableStructs[ctx->battlerAtk].truantCounter)
    {
        CancelMultiTurnMoves(ctx->battlerAtk, SKY_DROP_ATTACKCANCELER_CHECK);
        gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LOAFING;
        gBattlerAbility = ctx->battlerAtk;
        gBattlescriptCurrInstr = BattleScript_TruantLoafingAround;
        gBattleStruct->moveResultFlags[ctx->battlerDef] |= MOVE_RESULT_MISSED;
        return MOVE_STEP_FAILURE;
    }
    return MOVE_STEP_SUCCESS;
}
```

**Depois:**
```c
static enum MoveCanceler CancelerTruant(struct BattleContext *ctx)
{
    if (GetBattlerAbility(ctx->battlerAtk) == ABILITY_TRUANT && gDisableStructs[ctx->battlerAtk].truantCounter)
    {
        // Allow Slack Off to be used even during Truant's loafing turns
        if (ctx->currentMove == MOVE_SLACK_OFF)
            return MOVE_STEP_SUCCESS;
        
        CancelMultiTurnMoves(ctx->battlerAtk, SKY_DROP_ATTACKCANCELER_CHECK);
        gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LOAFING;
        gBattlerAbility = ctx->battlerAtk;
        gBattlescriptCurrInstr = BattleScript_TruantLoafingAround;
        gBattleStruct->moveResultFlags[ctx->battlerDef] |= MOVE_RESULT_MISSED;
        return MOVE_STEP_FAILURE;
    }
    return MOVE_STEP_SUCCESS;
}
```

### Como Funciona

1. **Verificação de Truant**: A função verifica se o Pokémon tem a habilidade Truant e se está em um turno de inatividade (`truantCounter != 0`)

2. **Exceção para Slack Off**: Se o movimento escolhido é `MOVE_SLACK_OFF`, a função retorna `MOVE_STEP_SUCCESS`, permitindo que o movimento seja executado normalmente

3. **Outros Movimentos**: Qualquer outro movimento continua bloqueado como anteriormente, mantendo a mecânica original do Truant

## Comportamento em Batalha

### Cenário de Exemplo:

```
Turno 1: Slaking com Truant pode atacar normalmente
Turno 2: Slaking está "se loafando" (loafing around) - PODE usar Slack Off, mas NÃO pode usar outros moves
Turno 3: Slaking pode atacar normalmente novamente
Turno 4: Slaking está se loafando - PODE usar Slack Off novamente
```

## Testes Incluídos

Três testes foram adicionados ao arquivo `test/battle/ability/truant.c`:

1. **Truant: Prevents attack on loafing turn** - Valida que Truant ainda funciona como esperado, bloqueando ataques em turnos alternados

2. **Truant: Slack Off can be used during loafing turn** - Valida que Slack Off específicamente pode ser usado durante turnos de inatividade

3. **Truant: Slack Off doesn't affect alternating turn pattern** - Valida que o padrão de alternância continua correto mesmo usando Slack Off

## Constantes Necessárias

O arquivo `src/battle_util.c` já inclui todas as constantes necessárias:
- `#include "constants/moves.h"` - Fornece `MOVE_SLACK_OFF`
- `#include "constants/abilities.h"` - Fornece `ABILITY_TRUANT`

## Compilação

Para compilar a ROM modificada:

```bash
make
```

A compilação foi testada e bem-sucedida. A ROM gerada (`pokeemerald.gba`) reflete essas mudanças.

## Balanceamento

Esta mudança fornece:

- **Utilidade**: Pokémon com Truant agora tem uma forma estratégica de se recuperar
- **Compensação**: Ainda está sujeito ao padrão de ataque alternado quando usa outros movimentos
- **Equilíbrio**: Slack Off custa efetivamente 2 turnos para recuperar 50% do HP (1 turno para Slack Off + 1 turno loafando), tornando-o equilibrado

## Modificações Futuras Possíveis

Se desejar estender isso para outros movimentos:

1. Adicionar mais movimentos à exceção (ex: healing moves)
2. Criar uma função auxiliar para verificar se um movimento é "permitido durante Truant"
3. Adicionar uma configuração (B_UPDATED_TRUANT_*) para controlar esse comportamento

Exemplo para múltiplos movimentos:

```c
if (ctx->currentMove == MOVE_SLACK_OFF || 
    ctx->currentMove == MOVE_RECOVER ||
    ctx->currentMove == MOVE_ROOST)
    return MOVE_STEP_SUCCESS;
```

## Autoria

Modificação realizada para balanceamento de ROM hack Pokemon Emerald Expansion.
