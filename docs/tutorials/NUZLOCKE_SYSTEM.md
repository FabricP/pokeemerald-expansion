# Sistema Nuzlocke - Documentação

## Funcionalidades Implementadas

Este sistema implementa as regras clássicas do Nuzlocke Challenge para Pokémon Emerald Expansion:

### 1. Release Automático ao Desmaiar
- Quando um Pokémon do jogador desmaia em batalha, ele é automaticamente liberado (released) após a batalha
- O sistema já estava implementado anteriormente

### 2. Uma Captura Por Área (NOVO)
- **Apenas 1 captura por região**: Cada área do jogo (identificada por `regionMapSectionId`) permite apenas uma tentativa de captura
- **Interiores e andares contam como mesma área**: Locais como diferentes andares de cavernas, ou interiores de edifícios na mesma cidade, compartilham a mesma região no mapa
- **Bloqueio após primeiro encontro**: Após o primeiro encontro selvagem em uma área, as Pokébolas ficam bloqueadas **mesmo que você não tenha capturado o Pokémon**
- **Mensagem de erro**: Uma mensagem clara informa quando você não pode mais capturar naquela área

## Como Funciona

### Sistema de Rastreamento de Áreas

O sistema usa flags (bits) para rastrear quais áreas já tiveram um encontro:
- Cada `regionMapSectionId` (ex: MAPSEC_ROUTE_101, MAPSEC_PETALBURG_CITY) tem uma flag correspondente
- Quando você entra em uma batalha selvagem pela primeira vez em uma área, a flag é ativada
- Uma vez ativada, você não pode mais usar Pokébolas naquela área

### Arquivos Modificados

1. **include/nuzlocke.h**
   - Adicionadas funções: `Nuzlocke_MarkAreaAsEncountered()`, `Nuzlocke_HasEncounteredInArea()`, `Nuzlocke_CanCatchInArea()`

2. **src/nuzlocke.c**
   - Implementação das funções de rastreamento de áreas
   - Usa flags a partir de `0x4000` para armazenar áreas visitadas

3. **src/battle_setup.c**
   - `DoStandardWildBattle()` marca a área atual como "encontrada" ao iniciar batalha selvagem

4. **src/item_use.c**
   - `GetBallThrowableState()` verifica se a captura é permitida na área atual
   - Adicionada mensagem de erro: "You already had your chance in this area!"

5. **include/item_use.h**
   - Novo estado: `BALL_THROW_UNABLE_NUZLOCKE`

## Como Usar

### Em Scripts/Eventos

O sistema é **automático** - não precisa de configuração adicional nos scripts. Ele funciona assim que o jogo é compilado.

Se você quiser **desabilitar temporariamente** o Nuzlocke em áreas específicas (ex: Safari Zone, eventos especiais), você pode:

1. Usar a flag `B_FLAG_NO_CATCHING` para desabilitar capturas completamente
2. Ou adicionar lógica customizada no `GetBallThrowableState()`

### Resetar Áreas (exemplo para New Game+)

Se quiser limpar as áreas visitadas ao iniciar um novo jogo, adicione no script de novo jogo:

```c
// Limpar flags de áreas visitadas do Nuzlocke
for (u16 i = 0; i < MAPSEC_COUNT; i++)
{
    FlagClear(NUZLOCKE_ENCOUNTER_FLAGS_START + i);
}
```

## Regras Aplicadas

### ✅ Implementado
- [x] Pokémon desmaiado = release automático
- [x] Apenas 1 captura por área
- [x] Bloqueio após primeiro encontro (mesmo sem captura)
- [x] Mensagem de erro clara
- [x] Áreas compartilham mesmo ID (interiores/andares)

### Cenários Especiais

#### Safari Zone
O sistema funciona normalmente na Safari Zone - cada setor permite uma tentativa.

#### Batalhas Duplas Selvagens
O sistema marca a área no primeiro encontro, independente de ser batalha simples ou dupla.

#### Pyramid / Battle Frontier
Batalhas do Battle Pyramid/Frontier não acionam o sistema de áreas (são ignoradas).

## Flags Usadas

O sistema usa flags no range `0x4000` a `0x4000 + MAPSEC_COUNT`:
- `0x4000` = MAPSEC_LITTLEROOT_TOWN
- `0x4001` = MAPSEC_OLDALE_TOWN
- `0x4002` = MAPSEC_DEWFORD_TOWN
- ... e assim por diante

**Nota**: Certifique-se de que este range de flags não conflita com outras flags do seu projeto!

## Mensagens

### Português (para modificar)
Se quiser traduzir a mensagem para português, edite em `src/item_use.c`:

```c
static const u8 sText_CantThrowPokeBall_Nuzlocke[] = _("NUZLOCKE: Você já teve sua\nchance nesta área!\p");
```

## Exemplos de Uso

### Exemplo 1: Route 101
1. Jogador entra na Route 101
2. Encontra um Zigzagoon selvagem → área marcada como "encontrada"
3. Jogador usa Pokébola → permitido (primeiro encontro)
4. Captura falha, Zigzagoon foge
5. Próximo encontro na Route 101 → Pokébolas bloqueadas (mensagem aparece)

### Exemplo 2: Granite Cave
1. Jogador entra no Granite Cave 1F
2. Encontra um Zubat → área marcada
3. Jogador sobe para Granite Cave B1F
4. Encontra outro Pokémon → **mesma área** (mesmo MAPSEC)
5. Pokébolas bloqueadas

## Customizações Possíveis

### Alterar Mensagem
Edite `sText_CantThrowPokeBall_Nuzlocke` em `src/item_use.c`

### Adicionar Exceções
Modifique `Nuzlocke_CanCatchInArea()` em `src/nuzlocke.c` para permitir capturas em áreas específicas:

```c
bool32 Nuzlocke_CanCatchInArea(void)
{
    u16 mapsec = GetCurrentRegionMapSectionId();
    
    // Exceção: sempre permitir captura no Safari Zone
    if (mapsec == MAPSEC_SAFARI_ZONE_NORTH || mapsec == MAPSEC_SAFARI_ZONE_SOUTH)
        return TRUE;
    
    return !Nuzlocke_HasEncounteredInArea();
}
```

### Limpar Flags de Teste
Para testar durante desenvolvimento, você pode adicionar um script de debug:

```
script ClearNuzlockeFlags
    call ClearNuzlockeFlags_C
    msgbox gText_NuzlockeFlagsCleared, MSGBOX_DEFAULT
    release
    end
```

E no código C:
```c
void ClearNuzlockeFlags_C(void)
{
    for (u16 i = 0; i < MAPSEC_COUNT; i++)
        FlagClear(NUZLOCKE_ENCOUNTER_FLAGS_START + i);
}
```

## Compatibilidade

Este sistema é compatível com:
- ✅ pokeemerald-expansion
- ✅ Batalhas normais selvagens
- ✅ Batalhas duplas selvagens
- ✅ Surf encounters
- ✅ Fishing encounters
- ✅ Rock Smash encounters

Não afeta:
- ❌ Batalhas contra treinadores
- ❌ Encontros estáticos/legendários (a menos que use BattleSetup_StartWildBattle)
- ❌ Link battles
- ❌ Battle Frontier

## Solução de Problemas

### "Pokébolas nunca bloqueiam"
- Verifique se `Nuzlocke_MarkAreaAsEncountered()` está sendo chamada em `DoStandardWildBattle()`
- Confirme que não há conflito de flags

### "Todas as áreas compartilham a mesma flag"
- Verifique se `GetCurrentRegionMapSectionId()` retorna valores diferentes para áreas diferentes
- Confirme que o `gMapHeader.regionMapSectionId` está correto nos arquivos de mapa

### "Sistema não funciona em área específica"
- Verifique o `regionMapSectionId` desse mapa
- Confirme que não é uma batalha especial (Pyramid, etc.)

## Créditos

Sistema implementado para pokeemerald-expansion.
Baseado nas regras oficiais do Nuzlocke Challenge.
