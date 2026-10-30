# Criar habilidades personalizadas

Este guia explica o fluxo geral para adicionar uma habilidade ao projeto e documenta como a habilidade **Fable** foi implementada. O código de Fable serve como exemplo de uma habilidade ativada ao entrar em campo que combina efeitos já existentes: limpar clima e terreno, como Teraform Zero, e remover estágios de atributos, usando a lógica de normalização de Haze e o comportamento de Curious Medicine.

> **Atenção:** no contexto de batalha, “estágios de atributos” são os modificadores de Ataque, Defesa, Velocidade, Ataque Especial, Defesa Especial, Precisão e Evasão. Fable não remove condições de status como queimadura, veneno, paralisia ou sono.

## Arquivos principais

| Arquivo | Responsabilidade |
| --- | --- |
| [`include/constants/abilities.h`](../../include/constants/abilities.h) | IDs das habilidades e contagem por geração. |
| [`src/data/abilities.h`](../../src/data/abilities.h) | Nome, descrição, avaliação da IA e propriedades especiais. |
| [`src/battle_util.c`](../../src/battle_util.c) | Lógica C de habilidades disparadas por eventos de batalha, como entrada em campo. |
| [`data/battle_scripts_1.s`](../../data/battle_scripts_1.s) | Sequência de mensagens, efeitos visuais e comandos de batalha. |
| [`include/battle_scripts.h`](../../include/battle_scripts.h) | Declarações dos scripts chamados pelo código C. |
| [`src/data/pokemon/species_info/`](../../src/data/pokemon/species_info/) | Habilidades disponíveis para cada espécie. |
| [`test/battle/ability/`](../../test/battle/ability/) | Testes automatizados das habilidades. |
| [`src/battle_message.c`](../../src/battle_message.c) e [`include/constants/battle_string_ids.h`](../../include/constants/battle_string_ids.h) | Textos novos de batalha, quando os textos já existentes não forem adequados. |

Uma habilidade simples pode exigir apenas os dados em `src/data/abilities.h` e sua associação a uma espécie. Uma habilidade com efeito próprio geralmente também precisa de lógica em C, um script de batalha, textos e testes.

## Antes de implementar

1. **Descreva o comportamento sem ambiguidade.** Especifique quando ativa, quem afeta, se há condições para ativar, se funciona uma vez ou em todas as entradas em campo e o que acontece quando o efeito não pode ser aplicado.
2. **Procure por efeitos equivalentes.** Pesquise habilidades e movimentos existentes antes de adicionar lógica nova. Reutilizar helpers e comandos de script reduz duplicação e mantém comportamentos consistentes.
3. **Determine o ponto de execução.** Habilidades podem atuar ao entrar em campo, ao usar ou receber um movimento, no fim do turno, ao sair de campo, entre outros momentos. O evento escolhido determina onde a habilidade deve ser ligada.
4. **Decida se precisa de um script.** Efeitos que precisam apresentar uma habilidade, mensagens, animações ou várias ações em sequência normalmente devem usar um BattleScript. Efeitos pequenos podem ser resolvidos diretamente em C, seguindo um padrão já usado pelo projeto.

## 1. Adicionar um ID

Abra `include/constants/abilities.h` e acrescente a nova constante ao final da lista da geração mais recente, antes de `ABILITIES_COUNT_GEN9` (ou do contador da geração atual do projeto). Não reutilize IDs existentes nem altere IDs de habilidades já registradas: eles são referências usadas em dados e lógica por todo o jogo.

Exemplo da entrada atual de Fable:

```c
ABILITY_ENERVATE = 311,
ABILITY_FABLE = 312,
ABILITIES_COUNT_GEN9,
```

O contador acompanha o fim da lista. Ao adicionar outra habilidade, use o próximo ID disponível e preserve a organização e os comentários de geração do arquivo.

## 2. Definir nome, descrição e propriedades

Em `src/data/abilities.h`, adicione uma entrada indexada pelo ID:

```c
[ABILITY_FABLE] =
{
    .name = _("Fable"),
    .description = COMPOUND_STRING("Clears weather, terrain and stat changes."),
    .aiRating = 8,
},
```

Pontos a considerar:

- `.name` é o nome mostrado pelo jogo.
- `.description` é o resumo exibido nos dados da habilidade. Mantenha-o curto e fiel ao comportamento real.
- `.aiRating` informa à IA a utilidade estimada da habilidade. Use valores próximos de habilidades comparáveis e reveja o valor se o efeito ou as condições mudarem.
- Propriedades como `.cantBeCopied`, `.cantBeSwapped`, `.cantBeTraced`, `.cantBeSuppressed` e `.breakable` alteram interações com outras habilidades. Só adicione as que forem exigidas pelo comportamento desejado, usando habilidades semelhantes como referência.

Não é necessário adicionar a habilidade a uma espécie para que sua definição e código compilem; sem associação, ela simplesmente não será obtida naturalmente por Pokémon.

## 3. Implementar o efeito no evento correto

Para efeitos de batalha, procure o tratamento correspondente no código, em geral dentro de `AbilityBattleEffects` em `src/battle_util.c` ou em outras rotinas de batalha, conforme o evento. Casos de entrada em campo costumam verificar `gSpecialStatuses[battler].switchInAbilityDone` para evitar ativar repetidamente durante o mesmo processamento de entrada.

O padrão de entrada de Fable é:

```c
case ABILITY_FABLE:
    if (!gSpecialStatuses[battler].switchInAbilityDone)
    {
        gSpecialStatuses[battler].switchInAbilityDone = TRUE;
        BattleScriptPushCursorAndCallback(BattleScript_ActivateFable);
        effect++;
    }
    break;
```

Essa condição limita a execução ao processamento daquela entrada em campo. Quando o Pokémon sair e entrar novamente, o sistema de batalha prepara o estado de entrada correspondente e a habilidade pode ativar outra vez. Não use uma flag persistente da espécie ou da batalha inteira se o requisito for ativar a cada entrada.

Antes de criar uma função nova, procure helpers que já cubram a operação. Por exemplo:

- `TryResetBattlerStatChanges(battler)` zera os estágios de atributos e os valores acumulados de Stockpile do battler.
- O comando `normalisebuffs` executa essa normalização para os battlers da batalha.
- `TryChangeBattleTerrain` é usado por habilidades que estabelecem terreno.
- A remoção e as notificações de clima/terreno têm comandos e tabelas de mensagens próprios.

Confira sempre a semântica exata do helper antes de reutilizá-lo. Um nome parecido não garante que afete os mesmos alvos ou estados que a habilidade requer.

## 4. Criar e declarar o BattleScript (quando necessário)

Os scripts de batalha ficam em arquivos `.s`, principalmente `data/battle_scripts_1.s`. Eles descrevem a ordem visível e funcional dos efeitos, por exemplo: mostrar o popup da habilidade, imprimir uma mensagem, tocar uma animação, remover um efeito de campo e atualizar habilidades afetadas.

Para que um script possa ser chamado a partir de C, declare-o em `include/battle_scripts.h`:

```c
extern const u8 BattleScript_ActivateFable[];
```

Em seguida, defina o rótulo no arquivo `.s`. Fable chama o popup, normaliza estágios de atributos, trata clima e terreno e atualiza efeitos dependentes do campo. Sua implementação está em [`data/battle_scripts_1.s`](../../data/battle_scripts_1.s), no rótulo `BattleScript_ActivateFable`.

Ao montar um script:

- Reutilize comandos existentes em vez de alterar a implementação dos comandos para uma única habilidade.
- Preserve e restaure variáveis globais de atacante/alvo quando outros efeitos ou callbacks possam alterá-las.
- Confira como scripts próximos atualizam habilidades que dependem do clima ou do terreno.
- Use as tabelas de mensagens existentes quando elas já representam o evento corretamente.
- Declare no header toda rotina referenciada por código C, com o nome exatamente igual ao rótulo do script.

Se o comportamento exigir um comando de script inexistente, sua implementação pode envolver `src/battle_script_commands.c`, o registro do comando e as declarações correspondentes. Esse é um passo mais amplo: procure primeiro uma combinação dos comandos atuais ou uma função auxiliar já disponível.

## 5. Associar a habilidade às espécies

Para disponibilizar a habilidade a Pokémon, edite os dados da espécie na família/geração correta em `src/data/pokemon/species_info/`. Localize `.abilities` e inclua o ID no slot apropriado, respeitando a ordem dos slots e as regras do projeto:

```c
.abilities = { ABILITY_PRIMEIRA, ABILITY_SEGUNDA, ABILITY_OCULTA },
```

Confira as espécies que já usam a habilidade e os padrões em famílias próximas. Se a habilidade for planejada apenas para uso em batalhas customizadas, testes ou scripts, pode ser apropriado deixá-la sem associação.

**Fable não foi associada a nenhuma espécie** na implementação descrita neste guia. Para distribuí-la, será necessário editar os dados de espécie separadamente.

## 6. Mensagens

Para mensagens já existentes, use seus IDs e tabelas (por exemplo, as mensagens de encerramento de clima e terreno). Para uma fala nova:

1. Adicione um ID em `include/constants/battle_string_ids.h`, na seção correta.
2. Defina o texto correspondente em `src/battle_message.c`.
3. Emita a mensagem no script ou código no momento apropriado.
4. Verifique os marcadores de nomes e variáveis, como `{B_ATK_NAME_WITH_PREFIX}`, para que a mensagem use o battler correto.

Evite criar IDs de mensagem duplicados ou reutilizar uma mensagem cujo sujeito, sentido ou gramática não corresponda ao evento.

## Implementação de referência: Fable

Fable foi especificada como uma habilidade que, ao entrar em campo, limpa o clima e o terreno e remove os estágios de atributos de todos os Pokémon ativos. O efeito ocorre novamente quando o usuário entra em campo outra vez.

### O que foi reutilizado

- **Teraform Zero:** referência para limpar clima e terreno e para atualizar as habilidades que dependem dessas condições. O script correspondente fica em `BattleScript_ActivateTeraformZero` em `data/battle_scripts_1.s`.
- **Curious Medicine:** referência para zerar estágios de atributos, especialmente `TryResetBattlerStatChanges` e a mensagem existente sobre alterações de atributos removidas.
- **Haze:** fornece o comando `normalisebuffs`, que reseta os estágios de atributos dos battlers sem implementar um loop separado para cada Pokémon.

### Arquivos específicos

- `include/constants/abilities.h`: registra `ABILITY_FABLE = 312`.
- `src/data/abilities.h`: nome, descrição e avaliação da IA.
- `src/battle_util.c`: agenda a habilidade no processamento de entrada em campo, usando `switchInAbilityDone`.
- `include/battle_scripts.h`: declara `BattleScript_ActivateFable`.
- `data/battle_scripts_1.s`: executa o popup, a normalização dos atributos e a remoção de clima e terreno.
- `src/data/pokemon/species_info/`: não foi alterado; nenhuma espécie recebeu Fable.

Fable remove **estágios de atributos**, não condições de status primárias. O comando de normalização também limpa os acumuladores de Defesa e Defesa Especial do Stockpile, de acordo com o helper reutilizado. A remoção de clima/terreno usa os textos existentes do jogo e chama o processamento relacionado às habilidades de campo.

Não há atualmente um teste automatizado de Fable no repositório: o arquivo de teste criado durante a implementação foi removido a pedido do usuário. Para manter cobertura futura, recomenda-se criar um teste em `test/battle/ability/fable.c` que verifique clima, terreno, estágios de atributos dos lados aliados e adversários, ausência de alteração de condições de status primárias e uma segunda entrada em campo.

## Testes e validação

Compile a ROM depois de alterar código ou dados:

```sh
make -j2
```

Se houver testes para a habilidade, filtre pelo nome do teste:

```sh
make check TESTS="Fable"
```

O filtro seleciona testes por nome, mas o sistema pode ainda compilar a suíte de testes necessária. Portanto, erros em outros arquivos de teste podem impedir a execução do caso específico. Distinga falhas de compilação, falhas do runner e falhas do teste em si ao analisar a saída.

Para uma habilidade nova, procure cobrir:

- ativação no evento esperado;
- todos os alvos corretos e nenhum alvo indevido;
- condições em que o efeito não deve acontecer;
- interação com Neutralizing Gas, supressão, troca/cópia de habilidade e habilidades de campo, se aplicável;
- ativação em uma nova entrada em campo;
- mensagens e animações observadas na ordem certa.

## Checklist para uma habilidade nova

- [ ] Especificar gatilho, alvos, condições e frequência de ativação.
- [ ] Pesquisar habilidades, scripts e helpers existentes que possam ser reutilizados.
- [ ] Adicionar um ID novo ao enum sem renumerar IDs existentes.
- [ ] Definir nome, descrição e avaliação da IA.
- [ ] Implementar a lógica no evento correto e impedir ativações duplicadas no mesmo evento.
- [ ] Criar e declarar um BattleScript se houver sequência de mensagens/efeitos.
- [ ] Adicionar mensagens novas somente se as existentes não forem adequadas.
- [ ] Associar a habilidade às espécies desejadas, ou documentar que ainda não foi distribuída.
- [ ] Criar testes automatizados para efeitos e casos-limite.
- [ ] Executar o build e os testes relevantes e revisar `git diff --check`.
