# Move relearner nos Centros Pokémon

Este guia documenta como o move relearner foi disponibilizado nos Centros
Pokémon e como adaptar esse padrão para outros mapas ou critérios de desbloqueio.

## Como funciona

O NPC reutiliza o script geral
[`Common_EventScript_MoveRelearner`](../../data/scripts/move_relearner.inc),
que oferece as opções de relembre habilitadas na configuração do projeto. Esse
script não cobra Heart Scales.

O NPC foi adicionado aos 16 mapas de primeiro andar dos Centros Pokémon,
incluindo o Centro da Battle Frontier. Em cada `map.json`, o objeto aponta para
o script geral e usa uma flag de ocultação:

```json
{
  "graphics_id": "OBJ_EVENT_GFX_OLD_MAN",
  "x": 4,
  "y": 6,
  "elevation": 3,
  "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
  "movement_range_x": 0,
  "movement_range_y": 0,
  "trainer_type": "TRAINER_TYPE_NONE",
  "trainer_sight_or_berry_tree_id": "0",
  "script": "Common_EventScript_MoveRelearner",
  "flag": "FLAG_SHOW_MOVE_RELEARNER"
}
```

## Desbloqueio após quatro insígnias

O campo `flag` de um objeto de mapa funciona como uma flag de ocultação:
quando está ligada, o objeto não aparece. Por isso, não se deve colocar
diretamente `FLAG_BADGE04_GET` nesse campo; essa flag começa desligada e é
ligada ao obter a insígnia, o que inverteria o comportamento desejado.

O fluxo usa a flag própria `FLAG_SHOW_MOVE_RELEARNER`. Apesar do nome, ela é
referenciada pelo campo de ocultação dos objetos:

1. A flag é definida (ligada) no início de um novo jogo em
   [`new_game.inc`](../../data/scripts/new_game.inc).
2. O evento compartilhado
   [`Common_EventScript_ReadyPetalburgGymForBattle`](../../data/event_scripts.s)
   a limpa quando `VAR_PETALBURG_GYM_STATE` chega a `6`, que representa quatro
   insígnias. O mesmo evento já expande o estoque do Poké Mart de Petalburg.
3. Como o campo `flag` oculta o objeto quando está ligado, o tutor fica
   escondido enquanto `FLAG_SHOW_MOVE_RELEARNER` está ligada. Ao limpar a flag
   após quatro insígnias, o NPC aparece ao entrar nos Centros Pokémon.

A flag é declarada em [`flags.h`](../../include/constants/flags.h) usando uma
posição anteriormente não utilizada. Não reutilize
`FLAG_HIDE_PETALBURG_GYM_GREETER`: ela é ligada novamente depois da Liga e
acabaria escondendo o move relearner no pós-jogo.

## Adicionar o NPC a outro mapa

1. Abra o `map.json` do mapa e acrescente um objeto em `object_events`.
2. Escolha uma posição livre e defina `script` como
   `Common_EventScript_MoveRelearner`.
3. Use `FLAG_SHOW_MOVE_RELEARNER` no campo `flag` para manter o
   NPC sincronizado com o desbloqueio existente.
4. Recompile o projeto para regenerar os dados de eventos do mapa.

Para mudar o requisito de desbloqueio, atualize o ponto que limpa a flag no
evento compartilhado. Não limpe a flag em apenas um ginásio se o requisito for
uma quantidade total de insígnias; use o estado/contador comum que represente
essa quantidade.
