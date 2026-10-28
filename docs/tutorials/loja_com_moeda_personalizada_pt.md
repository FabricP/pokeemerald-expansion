# Loja com moeda personalizada (Heart Scales)

Este tutorial documenta a loja do Pedrinho no primeiro andar do Goblin Store da Mama Island. Ela usa a interface padrão do Poké Mart, mas cobra **Heart Scales** em vez de Poké Dólares. No estado atual, cada item custa **2 Heart Scales**.

A cobrança é específica para esse mapa. Os outros Poké Marts continuam usando os preços e o dinheiro normais do jogo.

## Arquivos envolvidos

- `data/maps/MamaIsland_GoblinStore_1F/map.json`: define o personagem, o sprite, a posição e o script executado ao conversar com ele.
- `data/maps/MamaIsland_GoblinStore_1F/scripts.inc`: contém o diálogo do Pedrinho e a lista de itens da loja.
- `src/shop.c`: adapta o sistema padrão de loja para identificar o mapa especial, consultar o saldo, exibir a moeda, verificar o pagamento e retirar as Heart Scales.

O mapa chama o comando `pokemart`, portanto a loja continua usando a tela normal de compra. A diferença de moeda é aplicada internamente em `src/shop.c` quando o mapa atual é `MAP_MAMA_ISLAND_GOBLIN_STORE_1F`.

## Como o jogo identifica esta loja

A loja não recebe um identificador de moeda no comando `pokemart`. Esse comando abre a interface padrão; quem escolhe entre a moeda personalizada e Poké Dólares é o código em `src/shop.c`.

A função `IsHeartScaleMart()` faz essa identificação comparando o mapa atual com o mapa do Goblin Store:

```c
static bool8 IsHeartScaleMart(void)
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_MAMA_ISLAND_GOBLIN_STORE_1F)
        && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_MAMA_ISLAND_GOBLIN_STORE_1F);
}
```

O mapa atual é guardado em `gSaveBlock1Ptr->location.mapGroup` e `mapNum`. A constante `MAP_MAMA_ISLAND_GOBLIN_STORE_1F`, gerada a partir dos dados de mapas, identifica o grupo e o número correspondentes. A comparação exige que os dois valores coincidam; assim, lojas abertas em outros mapas não satisfazem a condição.

As funções de preço, consulta de saldo, validação do pagamento, exibição da moeda e dedução chamam `IsHeartScaleMart()`. Quando ela retorna verdadeiro, a loja usa Heart Scales; quando retorna falso, segue o fluxo normal do sistema, que usa Poké Dólares. A condição também protege as outras lojas contra alterações acidentais ao comportamento padrão.

**Importante:** a verificação é pelo mapa, não pelo personagem, pelo nome do script ou pela lista de estoque. Portanto, qualquer comando `pokemart` executado enquanto o jogador estiver nesse mapa usará a moeda personalizada. No mapa atual, o Pedrinho é o vendedor que abre o Poké Mart. Se houver outro Poké Mart nesse mesmo mapa, ele também usará Heart Scales.

Para mover essa loja para outro mapa, atualize as duas ocorrências de `MAP_MAMA_ISLAND_GOBLIN_STORE_1F` em `IsHeartScaleMart()` para a constante do novo mapa. Não basta trocar o script do vendedor: a função decide a moeda usando a localização do jogador no momento em que a interface da loja está ativa.

## Como a loja é associada ao personagem

Em `map.json`, o objeto do vendedor precisa apontar para o script correspondente. Por exemplo:

```json
{
  "graphics_id": "OBJ_EVENT_GFX_MAN_4",
  "script": "MamaIsland_GoblinStore_1F_EventScript_Pedrinho"
}
```

O sprite pode ser trocado por outro `OBJ_EVENT_GFX_*` disponível no projeto sem alterar a lógica da moeda. Preserve as demais propriedades do objeto, como coordenadas, movimento e flag.

O script em `scripts.inc` segue o formato normal:

```asm
MamaIsland_GoblinStore_1F_EventScript_Pedrinho::
    lock
    faceplayer
    message MamaIsland_GoblinStore_1F_Text_PedrinhoOpening
    waitmessage
    pokemart MamaIsland_GoblinStore_1F_Pokemart
    msgbox MamaIsland_GoblinStore_1F_Text_PedrinhoClosing, MSGBOX_DEFAULT
    release
    end
```

As falas são os textos `MamaIsland_GoblinStore_1F_Text_PedrinhoOpening` e `MamaIsland_GoblinStore_1F_Text_PedrinhoClosing`. A lista passada ao comando `pokemart` é a lista de estoque descrita na próxima seção.

## Como adicionar ou remover itens do estoque

Edite a lista `MamaIsland_GoblinStore_1F_Pokemart` em `data/maps/MamaIsland_GoblinStore_1F/scripts.inc`. Cada item é informado com uma diretiva `.2byte`, e `pokemartlistend` encerra a lista:

```asm
MamaIsland_GoblinStore_1F_Pokemart:
    .2byte ITEM_MASTER_BALL
    .2byte ITEM_RARE_CANDY
    pokemartlistend
```

Use constantes `ITEM_*` declaradas pelo projeto. A lista atual contém apenas `ITEM_MASTER_BALL`; para vender mais itens, acrescente uma linha `.2byte` por item. Para retirar um item, remova sua linha. Não remova `pokemartlistend`.

Todos os itens dessa loja custam o mesmo valor, atualmente 2 Heart Scales. Isso inclui qualquer item que você adicionar à lista.

Alguns itens importantes, como a Master Ball, são tratados pelo Poké Mart como itens de estoque limitado e a interface pode impedir que se compre outro quando o jogador já possui um. Essa regra é independente do preço em Heart Scales.

## Como alterar o preço comum a todos os itens

Em `src/shop.c`, procure a definição:

```c
#define HEART_SCALE_SHOP_PRICE 2
```

Altere `2` para o custo desejado. Por exemplo, para cobrar 5 Heart Scales por item:

```c
#define HEART_SCALE_SHOP_PRICE 5
```

O mesmo valor é usado para:

- preço mostrado na lista;
- custo mostrado na etapa de quantidade e confirmação;
- verificação do saldo necessário;
- cálculo da quantidade máxima comprável;
- total de Heart Scales retirado da mochila após a compra.

Por isso, altere apenas essa constante para mudar o custo geral. Não altere separadamente as contas de interface ou cobrança, senão os valores mostrados e debitados podem divergir.

## Como trocar Heart Scales por outra moeda que seja um item

O código atual considera a moeda um item da mochila. Para mudar, por exemplo, para Shoal Shells, faça a troca de `ITEM_HEART_SCALE` por `ITEM_SHOAL_SHELL` em **todos** os pontos específicos da moeda em `src/shop.c`:

1. **Saldo disponível:** em `GetShopCurrency()`, troque o item passado a `CountTotalItemQuantityInBag`.
2. **Validação do pagamento:** `HasEnoughShopCurrency()` usa o saldo retornado por `GetShopCurrency()`. Em geral, não precisa de alteração adicional.
3. **Dedução:** em `BuyMenuSubtractCurrency()`, troque o item passado a `RemoveBagItem`.
4. **Mensagens exibidas:** os textos específicos da moeda ficam no início de `src/shop.c`, nas constantes `sText_*` que contêm `HeartScale`. Edite o texto entre `_(` e `)`, mantendo as aspas e os marcadores `{STR_VAR_1}`, `{STR_VAR_2}` e `{STR_VAR_3}` nos textos que os usam:

   | Constante | Onde aparece | Texto atual |
   | --- | --- | --- |
   | `sText_YouDontHaveEnoughHeartScales` | Quando o jogador não tem escalas suficientes | `You don't have enough\nHeart Scales.` |
   | `sText_YouWantedVar1ThatllBeVar2HeartScales` | Confirmação de item de compra única, como a Master Ball | `You wanted {STR_VAR_1}?\nThat'll be {STR_VAR_2} Heart Scales. Will that be okay?` |
   | `sText_Var1AndYouWantedVar2HeartScales` | Confirmação após escolher quantidade | `{STR_VAR_1}? And you wanted {STR_VAR_2}?\nThat'll be {STR_VAR_3} Heart Scales.` |
   | `sText_HeartScalePrice` | Preço de cada item na lista de estoque | `{STR_VAR_1} SCALES` |
   | `sText_HeartScaleBalance` | Saldo atual no topo da tela de compra | `SCALES:{STR_VAR_1}` |
   | `sText_HeartScaleQuantityPrice` | Custo total na tela de escolha da quantidade | `{STR_VAR_1} SCALES` |

   Os marcadores são substituídos pelo jogo: `STR_VAR_1` é o nome do item nos diálogos de confirmação e o número do preço ou saldo nos rótulos; `STR_VAR_2` e `STR_VAR_3` recebem quantidade e custo, conforme a mensagem. Mantenha-os onde quer que esses valores apareçam. `\n` inicia uma nova linha.

   Por exemplo, para trocar a moeda para Shoal Shells, além de trocar o item cobrado nas funções descritas acima, altere esses textos para mencionar “Shoal Shells” ou “SHELLS”, conforme o espaço disponível na interface. O preço na lista e o saldo têm largura limitada, então prefira rótulos curtos, como `"{STR_VAR_1} SHELLS"` e `"SHELLS:{STR_VAR_1}"`.

   Altere apenas essas constantes locais da loja. Não mude textos genéricos como `gText_YouDontHaveMoney` ou `gText_PokedollarVar1`: eles são compartilhados por outros mercados, e editá-los alteraria a interface global. Ao substituir o conteúdo, mantenha os nomes das constantes e os marcadores intactos para que os pontos que selecionam cada mensagem continuem funcionando.

Exemplo das duas operações centrais, usando Shoal Shells:

```c
// Saldo disponível
return CountTotalItemQuantityInBag(ITEM_SHOAL_SHELL);

// Dedução depois de confirmar e receber o item
RemoveBagItem(ITEM_SHOAL_SHELL, sShopData->totalCost);
```

O nome da constante depende do item pretendido; confira-o em `include/constants/items.h`. Se o item estiver numa bolsa diferente ou tiver limites de quantidade próprios, teste especialmente o saldo e o limite máximo de compra.

### Moeda em item versus moeda armazenada em variável

Heart Scales são contadas na mochila e removidas de lá. Para outra moeda que também seja um item, o padrão acima costuma ser suficiente.

Se quiser cobrar uma moeda que não seja item — por exemplo, uma variável de pontos, Battle Points ou outro contador próprio — a alteração é maior: o saldo deve ser lido do armazenamento correto, a confirmação precisa de uma verificação correspondente e a dedução deve atualizar esse contador. Não use `CountTotalItemQuantityInBag` ou `RemoveBagItem` para esse tipo de moeda.

Em ambos os casos, deixe a condição `IsHeartScaleMart()` (ou renomeie-a para refletir a moeda personalizada) restrita ao mapa correto. Essa condição é o que evita alterar os outros Poké Marts.

## Testes recomendados após alterar a loja

1. Compile o jogo com `make -j2` e confirme que não há erros.
2. Entre no Goblin Store da Mama Island e confirme que os outros markets do jogo continuam mostrando Poké Dólares.
3. Confira se a lista contém exatamente os itens definidos em `MamaIsland_GoblinStore_1F_Pokemart`.
4. Com menos moedas do que o preço, tente selecionar o item e confirme que a compra é recusada sem retirar moeda nem conceder o item.
5. Com o saldo exato, compre uma unidade e confirme que o item é recebido e o custo é retirado.
6. Confira se o saldo visível atualiza depois da compra.
7. Se houver quantidade selecionável, teste o limite máximo e uma compra com várias unidades.
8. Verifique a regra de itens importantes, incluindo a Master Ball, e confirme que o diálogo de confirmação mostra a moeda correta.

## Referências no código

- Lista de estoque e script do vendedor: `data/maps/MamaIsland_GoblinStore_1F/scripts.inc`
- Personagem e mapa: `data/maps/MamaIsland_GoblinStore_1F/map.json`
- Preço, identificação do mapa, saldo, mensagens e cobrança: `src/shop.c`
- Identificadores de itens: `include/constants/items.h`
