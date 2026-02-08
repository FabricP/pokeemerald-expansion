# Sistema de Drop de Itens em Batalhas Selvagens

## Descrição

Esta funcionalidade permite que Pokémon selvagens **soltem (dropar)** os itens que estão segurando quando são derrotados em batalha. O item é automaticamente adicionado à bolsa do jogador, com uma **mensagem informativa** e um **efeito sonoro**.

## Como Funciona

- Quando você derrota um Pokémon selvagem em batalha
- Se o Pokémon estava segurando um item (held item)
- E há espaço na sua bolsa para o item
- O item é automaticamente adicionado à sua bolsa
- Uma **mensagem aparece**: "*[Nome do Pokémon]* dropped one *[Nome do Item]*!"
- Um **som de item obtido** é reproduzido (SE_RG_BALL_CLICK)

## Configuração

A funcionalidade pode ser ativada ou desativada no arquivo de configuração:

**Arquivo:** `include/config/battle.h`

```c
#define B_WILD_ITEM_DROP    TRUE   // Se TRUE, Pokémon selvagens dropam itens ao serem derrotados
```

### Opções:
- `TRUE` - Pokémon selvagens dropam seus itens quando derrotados
- `FALSE` - Desabilita o sistema de drop (comportamento padrão do jogo)

## Limitações

- A funcionalidade **não** funciona em:
  - Batalhas contra treinadores
  - Batalhas de link (multiplayer)
  - Batalhas na Battle Frontier

- Se a bolsa estiver cheia e não houver espaço para o item, ele **não** será adicionado (será perdido)

## Exemplos

### Exemplo 1: Pikachu com Light Ball
Se você derrotar um Pikachu selvagem que está segurando uma Light Ball, a Light Ball será automaticamente adicionada à sua bolsa.

### Exemplo 2: Chansey com Lucky Egg
Se você derrotar uma Chansey selvagem segurando Lucky Egg, o Lucky Egg será adicionado à sua bolsa.

## Notas para Desenvolvedores

### Arquivos Modificados

1. **include/config/battle.h** - Adicionada a config `B_WILD_ITEM_DROP`
2. **include/constants/battle_string_ids.h** - Adicionada `STRINGID_WILDPKMNDROPITEM`
3. **include/battle_scripts.h** - Declaração do `BattleScript_WildMonDroppedItem`
4. **src/battle_message.c** - Mensagem de texto para o item dropado
5. **src/battle_script_commands.c** - Modificada a função `Cmd_tryfaintmon` para adicionar a lógica de drop
6. **data/battle_scripts_1.s** - Criado `BattleScript_WildMonDroppedItem` para exibir mensagem e tocar som

### Implementação Técnica

O sistema verifica:
1. Se é uma batalha selvagem (não treinador, não link, não frontier)
2. Se o Pokémon derrotado tem um item segurado
3. Se há espaço na bolsa para o item

Quando um item é dropado:
1. O item é adicionado à bolsa via `AddBagItem()`
2. O item é armazenado em `gLastUsedItem` para exibição na mensagem
3. O battle script `BattleScript_WildMonDroppedItem` é executado
4. Uma pequena pausa é aplicada
5. O som `SE_RG_BALL_CLICK` é reproduzido
6. A mensagem "*[Pokémon]* dropped one *[Item]*!" é exibida
7. O script aguarda antes de continuar

```c
#if B_WILD_ITEM_DROP == TRUE
// Se é uma batalha selvagem e o Pokémon tem um item segurado, dropa na bag
if (!(gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER)))
{
    u16 heldItem = GetMonData(GetBattlerMon(battler), MON_DATA_HELD_ITEM, NULL);
    if (heldItem != ITEM_NONE && CheckBagHasSpace(heldItem, 1))
    {
        AddBagItem(heldItem, 1);
        gLastUsedItem = heldItem;
        BattleScriptPushCursor();
        gBattlescriptCurrInstr = BattleScript_WildMonDroppedItem;
        return;
    }
}
#endif
```

**Battle Script:**
```asm
BattleScript_WildMonDroppedItem::
	pause B_WAIT_TIME_SHORT
	playse SE_RG_BALL_CLICK
	copybyte sBATTLER, gBattlerFainted
	printstring STRINGID_WILDPKMNDROPITEM
	waitmessage B_WAIT_TIME_LONG
	return
```

## Possíveis Melhorias Futuras

- ~~Adicionar mensagem de texto quando um item é dropado~~ ✅ **Implementado!**
- ~~Adicionar efeito sonoro ao dropar item~~ ✅ **Implementado!**
- Adicionar chance percentual de drop (nem sempre 100%)
- Permitir configurar quais tipos de batalha podem dropar itens
- Adicionar opção de enviar para PC se a bolsa estiver cheia
- Adicionar animação de item caindo/brilhando
