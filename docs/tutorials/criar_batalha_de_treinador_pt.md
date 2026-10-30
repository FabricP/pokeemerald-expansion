# Como criar uma batalha de treinador iniciada ao conversar com um NPC

Este tutorial explica como configurar um NPC de mapa para mostrar uma fala e
iniciar uma batalha de treinador ao ser abordado pelo jogador. O exemplo usa o
Bug Catcher da loja Goblin Hero em Mama Island.

## Como as peças se conectam

Uma batalha de treinador usa quatro partes:

1. `include/constants/opponents.h` associa um nome a um ID numérico. Esse ID
   identifica a equipe e também a flag que registra se o treinador foi derrotado.
2. `src/data/trainers.party` define o nome, a classe e a equipe do treinador.
   Durante a compilação, o processador gera os dados C do treinador a partir
   deste arquivo.
3. O arquivo `data/maps/.../scripts.inc` do mapa define o diálogo e chama
   `trainerbattle_single`.
4. O `map.json` do mapa associa o script ao objeto do NPC.

O nome usado nos arquivos deve ser consistente: o ID definido em
`opponents.h` deve ser o mesmo usado na equipe e na chamada
`trainerbattle_single`.

## 1. Registrar um ID de treinador

Abra `include/constants/opponents.h` e escolha um ID ainda não usado, próximo
do final da lista:

```c
#define TRAINER_GOBLIN_STORE_BUG_CATCHER 855
```

Atualize também `TRAINERS_COUNT` para ser **um número maior que o maior ID de
treinador**. Por exemplo, se o maior ID usado for `855`:

```c
#define TRAINERS_COUNT 856
```

Esses dois valores têm propósitos diferentes:

- `TRAINERS_COUNT` é a quantidade de posições necessárias para a tabela de
  treinadores; deve ser maior que o maior ID atribuído.
- `MAX_TRAINERS_COUNT` reserva uma flag persistente por treinador para lembrar
  se ele foi derrotado. O intervalo disponível é limitado por
  `TRAINER_FLAGS_START` e `TRAINER_FLAGS_END`, em `include/constants/flags.h`.

**Não aumente `MAX_TRAINERS_COUNT` automaticamente.** Aumentá-lo pode exigir
alterações no espaço de flags e nos dados salvos. Consulte os comentários em
`opponents.h` e `flags.h` antes de usar os últimos IDs disponíveis. Neste
checkout, os IDs dos treinadores normais ocupam o espaço até `863`; o ID `855`
já está reservado para o Bug Catcher da loja.

Nunca reutilize o ID de outro treinador: isso faria os dois compartilharem
equipe e estado de derrota.

## 2. Definir o treinador e sua equipe

Adicione uma seção em `src/data/trainers.party`. O nome entre `===` corresponde
ao nome definido em `opponents.h`:

```text
=== TRAINER_GOBLIN_STORE_BUG_CATCHER ===
Name: ELIAS
Class: Bug Catcher
Pic: Bug Catcher
Gender: Male
Music: Male
Double Battle: No
AI: Check Bad Move

Caterpie
Level: 12
IVs: 0 HP / 0 Atk / 0 Def / 0 SpA / 0 SpD / 0 Spe
- Tackle
- String Shot

Weedle
Level: 12
IVs: 0 HP / 0 Atk / 0 Def / 0 SpA / 0 SpD / 0 Spe
- Poison Sting
- String Shot
```

Cada Pokémon é um bloco separado por uma linha em branco. Os golpes são
opcionais: se não forem informados, o sistema usa os últimos quatro golpes que
a espécie aprende por nível até aquele nível. Para escolher exatamente os
golpes da equipe, adicione de uma a quatro linhas iniciadas por `- `, depois
dos atributos do Pokémon.

`Level` e `IVs` também podem ser omitidos, mas nesse caso são usados os valores
padrão do processador de equipes. Defina-os explicitamente se quiser controlar
o nível e os IVs. Veja o cabeçalho de `src/data/trainers.party` para a lista
completa de campos opcionais e formatos aceitos.

## 3. Criar o diálogo e iniciar a batalha

No `scripts.inc` do mapa, adicione um script como este:

```asm
MamaIsland_GoblinStore_1F_EventScript_BugCatcher::
    goto_if_defeated TRAINER_GOBLIN_STORE_BUG_CATCHER, MamaIsland_GoblinStore_1F_EventScript_BugCatcherPostBattle
    msgbox MamaIsland_GoblinStore_1F_Text_BugCatcher, MSGBOX_DEFAULT
    trainerbattle_single TRAINER_GOBLIN_STORE_BUG_CATCHER, MamaIsland_GoblinStore_1F_Text_BugCatcherIntro, MamaIsland_GoblinStore_1F_Text_BugCatcherDefeat
    end

MamaIsland_GoblinStore_1F_EventScript_BugCatcherPostBattle::
    msgbox MamaIsland_GoblinStore_1F_Text_BugCatcherPostBattle, MSGBOX_DEFAULT
    end
```

O que cada parte faz:

- `goto_if_defeated` consulta a flag ligada ao ID do treinador. Se o jogador já
  o derrotou, o script pula a apresentação inicial e mostra a fala pós-batalha.
- `msgbox ...Text_BugCatcher` é a fala de interação antes da batalha.
- `trainerbattle_single` inicia uma batalha Single com o treinador indicado.
- O segundo argumento de `trainerbattle_single` é a fala de desafio usada pelo
  treinador antes da luta.
- O terceiro argumento é a fala do treinador **quando o jogador vence**. Não é
  a fala do jogador nem a fala após o treinador derrotar o jogador.
- A macro registra o resultado da batalha usando o ID do treinador. Não é
  necessário criar uma flag separada para registrar que ele foi derrotado.

Declare os textos no mesmo `scripts.inc`:

```asm
MamaIsland_GoblinStore_1F_Text_BugCatcher:
    .string "Welcome to the GOBLIN HERO STORE!\n"
    .string "I collect rare bugs and cards.$"

MamaIsland_GoblinStore_1F_Text_BugCatcherIntro:
    .string "Let's battle! My Bug-type team is\n"
    .string "ready to show off!$"

MamaIsland_GoblinStore_1F_Text_BugCatcherDefeat:
    .string "You beat me! I'll keep training.$"

MamaIsland_GoblinStore_1F_Text_BugCatcherPostBattle:
    .string "Thanks for the battle!$"
```

`$` encerra o texto; `\n` inicia uma nova linha na mesma caixa. O uso de `goto_if_defeated` faz com que a fala de boas-vindas não se repita depois que o treinador foi derrotado. Se preferir uma fala diferente após a batalha, use o destino `...PostBattle` como no exemplo.

Para fazer uma revanche, não use essa verificação para impedir a batalha. Nesse
caso, consulte os padrões de revanche e as macros `trainerbattle_rematch` no
projeto; uma revanche envolve configurações adicionais além de apenas chamar
`trainerbattle_single` novamente.

## 3.1. Entregar um item depois de vencer a batalha

Para executar ações imediatamente após a vitória, use o quarto argumento de
`trainerbattle_single`: o script de callback da vitória. Nele, mostre a fala
pós-batalha, entregue o item com `giveitem` e marque uma flag persistente para
que o item não seja entregue novamente.

Exemplo de Potion entregue uma única vez:

```asm
MamaIsland_GoblinStore_1F_EventScript_BugCatcher::
    goto_if_set FLAG_RECEIVED_POTION_FROM_BUG_CATCHER, MamaIsland_GoblinStore_1F_EventScript_BugCatcherAlreadyRewarded
    checktrainerflag TRAINER_GOBLIN_STORE_BUG_CATCHER
    goto_if TRUE, MamaIsland_GoblinStore_1F_EventScript_BugCatcherGivePotion
    trainerbattle_single TRAINER_GOBLIN_STORE_BUG_CATCHER, MamaIsland_GoblinStore_1F_Text_BugCatcherIntro, MamaIsland_GoblinStore_1F_Text_BugCatcherDefeat, MamaIsland_GoblinStore_1F_EventScript_BugCatcherPostBattle
    end

MamaIsland_GoblinStore_1F_EventScript_BugCatcherPostBattle::
    msgbox MamaIsland_GoblinStore_1F_Text_BugCatcherPostBattle, MSGBOX_DEFAULT
    goto MamaIsland_GoblinStore_1F_EventScript_BugCatcherGivePotion

MamaIsland_GoblinStore_1F_EventScript_BugCatcherGivePotion::
    giveitem ITEM_POTION
    goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
    setflag FLAG_RECEIVED_POTION_FROM_BUG_CATCHER
    release
    end

MamaIsland_GoblinStore_1F_EventScript_BugCatcherAlreadyRewarded::
    msgbox MamaIsland_GoblinStore_1F_Text_BugCatcherAlreadyRewarded, MSGBOX_AUTOCLOSE
    end
```

O fluxo é:

1. Se a flag de recompensa já estiver ativa, mostrar apenas a fala de item já
   recebido.
2. Se o treinador já tiver sido derrotado, mas a recompensa ainda não tiver
   sido entregue, ir diretamente para o script que tenta entregar o item. Isso
   permite tentar novamente caso a mochila estivesse cheia.
3. Caso contrário, iniciar a batalha e passar o callback como quarto argumento.
   O callback é executado após a vitória, mostra a fala e segue para a entrega.
4. `giveitem` define `VAR_RESULT` como `FALSE` se não puder adicionar o item.
   Nesse caso, `Common_EventScript_ShowBagIsFull` avisa o jogador. A flag fica
   desativada para que a entrega possa ser tentada depois.
5. Só depois de `giveitem` ter sucesso, `setflag` registra que a recompensa foi
   recebida.

Declare a flag persistente em `include/constants/flags.h`, usando uma posição
disponível e única. Não use uma flag temporária (`FLAG_TEMP_*`), pois ela é
limpa quando o mapa é carregado. A flag do exemplo é ilustrativa: confirme que
o nome e o valor escolhidos não estão sendo usados por outro evento.

### Termine corretamente cada texto

O marcador `$` encerra uma string de diálogo. Use-o no final do texto de
vitória, assim como nos outros textos:

```asm
MamaIsland_GoblinStore_1F_Text_BugCatcherPostBattle:
    .string "You won! Take this Potion.$"
```

Sem o `$`, o sistema pode continuar lendo os dados seguintes como parte do
texto atual. Isso pode fazer a próxima fala aparecer automaticamente ou
sobrescrever a fala pós-batalha. `\p` inicia uma nova página na mesma conversa;
`\n` quebra a linha, mas nenhum dos dois substitui o `$` final.

## 4. Associar o script ao objeto no mapa

Abra o `map.json` correspondente. Em `object_events`, cada objeto é identificado
por sua posição na lista: o primeiro objeto é o ID local 1, o segundo é o ID
local 2, e assim por diante.

No evento do Bug Catcher, defina o campo `script` para o nome do script:

```json
{
  "graphics_id": "OBJ_EVENT_GFX_BUG_CATCHER",
  "x": 13,
  "y": 3,
  "elevation": 0,
  "movement_type": "MOVEMENT_TYPE_FACE_UP",
  "movement_range_x": 0,
  "movement_range_y": 0,
  "trainer_type": "TRAINER_TYPE_NONE",
  "trainer_sight_or_berry_tree_id": "0",
  "script": "MamaIsland_GoblinStore_1F_EventScript_BugCatcher",
  "flag": "0"
}
```

`TRAINER_TYPE_NONE` deixa o NPC como um personagem de conversa normal: o jogador
precisa falar com ele para executar o script. Isso é diferente de um treinador
que avista e desafia automaticamente o jogador; esse comportamento usa um tipo
de treinador e parâmetros de visão próprios.

No mapa da loja, o Bug Catcher é o segundo item de `object_events`, portanto tem
ID local 2. Ao editar a lista no Porymap ou no JSON, confira que o script foi
associado ao objeto certo.

## 5. Garantir que o script seja incluído

O arquivo `scripts.inc` do mapa também precisa ser incluído em
`data/event_scripts.s`. Os mapas da Goblin Hero Store já têm uma inclusão para
cada andar. Se estiver adicionando um mapa novo e seu script não estiver
incluído, o montador não conseguirá resolver os nomes dos scripts e textos.

## 6. Compilar e verificar no jogo

No WSL, na pasta raiz do projeto, rode:

```bash
make
```

Confirme que a compilação termina sem erros. Depois teste no jogo:

1. Interaja com o objeto correto e confira a fala inicial.
2. Verifique se a batalha usa a equipe, os níveis e os golpes configurados.
3. Vença a batalha e confira o texto de vitória do treinador.
4. Se houver recompensa, confira que o item é entregue uma vez e que a fala
   pós-batalha termina corretamente, sem iniciar o diálogo seguinte por cima.
5. Se possível, repita com a mochila cheia: confira o aviso e tente receber o
   item após liberar espaço.
6. Fale com o treinador novamente e confirme que a batalha não se repete e que
   a recompensa não é entregue outra vez.

## Exemplo existente

O Bug Catcher Elias da Goblin Hero Store demonstra este padrão:

- `include/constants/opponents.h`: ID `TRAINER_GOBLIN_STORE_BUG_CATCHER`.
- `src/data/trainers.party`: equipe de Elias.
- `data/maps/MamaIsland_GoblinStore_1F/scripts.inc`: textos e script da batalha.
- `data/maps/MamaIsland_GoblinStore_1F/map.json`: associação do script ao
  segundo objeto, o Bug Catcher.
