MamaIsland_GoblinStore_1F_MapScripts::
	.byte 0

@ ---------- EVENTOS DA LOJA ----------

MamaIsland_GoblinStore_1F_EventScript_Pedrinho::
	lock
	faceplayer
	message MamaIsland_GoblinStore_1F_Text_PedrinhoOpening
	waitmessage
	pokemart MamaIsland_GoblinStore_1F_Pokemart
	msgbox MamaIsland_GoblinStore_1F_Text_PedrinhoClosing, MSGBOX_DEFAULT
	release
	end

MamaIsland_GoblinStore_1F_Text_PedrinhoOpening:
	.string "Welcome to GOBLIN HERO STORE!\n"
	.string "My name is PEDRINHO, how can i help?$"

MamaIsland_GoblinStore_1F_Text_PedrinhoClosing:
	.string "Pedrinho fim$"

	.align 2

MamaIsland_GoblinStore_1F_Pokemart:
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_ABILITY_PATCH
	pokemartlistend


@ ---------- OUTROS EVENTOS ----------

MamaIsland_GoblinStore_1F_EventScript_Poster1::
	msgbox MamaIsland_GoblinStore_1F_Text_Poster1, MSGBOX_SIGN
	end

MamaIsland_GoblinStore_1F_Text_Poster1:
	.string "A poster about a magical game!\n"
	.string "It seems complex.$"

MamaIsland_GoblinStore_1F_EventScript_Poster2::
	msgbox MamaIsland_GoblinStore_1F_Text_Poster2, MSGBOX_SIGN
	end

MamaIsland_GoblinStore_1F_Text_Poster2:
	.string "A poster about a linked-fissure!\n"
	.string "This game looks fun!$"

MamaIsland_GoblinStore_1F_EventScript_Display::
	msgbox MamaIsland_GoblinStore_1F_Text_Display, MSGBOX_SIGN
	end

MamaIsland_GoblinStore_1F_Text_Display:
	.string "A display showing the results of the\n"
	.string "matches.$"

MamaIsland_GoblinStore_1F_EventScript_Clock::
	msgbox MamaIsland_GoblinStore_1F_Text_Clock, MSGBOX_SIGN
	end

MamaIsland_GoblinStore_1F_Text_Clock:
	.string "Time flies when we’re having fun!$"

MamaIsland_GoblinStore_1F_EventScript_Treats::
	msgbox MamaIsland_GoblinStore_1F_Text_Treats, MSGBOX_SIGN
	end

MamaIsland_GoblinStore_1F_Text_Treats:
	.string "A shelf full of delicious treats!$"

MamaIsland_GoblinStore_1F_EventScript_Fridge::
	msgbox MamaIsland_GoblinStore_1F_Text_Fridge, MSGBOX_SIGN
	end

MamaIsland_GoblinStore_1F_Text_Fridge:
	.string "Do they have my favorite drink?$"


@ ---------- BATALHA CONTRA FABAS ----------

MamaIsland_GoblinStore_1F_EventScript_Fabas::
	trainerbattle_single TRAINER_GOBLIN_STORE_FABAS, MamaIsland_GoblinStore_1F_Text_FabasIntro, MamaIsland_GoblinStore_1F_Text_FabasDefeat, MamaIsland_GoblinStore_1F_EventScript_FabasPostBattle
	goto_if_set FLAG_RECEIVED_ITEM_FROM_FABAS, MamaIsland_GoblinStore_1F_EventScript_FabasAlreadyRewarded
	end

MamaIsland_GoblinStore_1F_Text_FabasIntro:
	.string "Where is my natural guarana?\p"
	.string "Hey you!\n"
	.string "Do you dare challenge me?\p"
	.string "Don't scream!!$"

MamaIsland_GoblinStore_1F_Text_FabasDefeat:
	.string "You only beat me because I was nerfed\n"
	.string "by the nature.$"

MamaIsland_GoblinStore_1F_EventScript_FabasPostBattle::
	msgbox MamaIsland_GoblinStore_1F_Text_FabasDefeated, MSGBOX_DEFAULT
	giveitem ITEM_HEART_SCALE, 12
	goto_if_eq VAR_RESULT, FALSE, Common_EventScript_ShowBagIsFull
	setflag FLAG_RECEIVED_ITEM_FROM_FABAS
	release
	end

MamaIsland_GoblinStore_1F_Text_FabasDefeated:
	.string "Oh man, I thought I was going to win.\p"
	.string "Well, take this, you deserverd!$"

MamaIsland_GoblinStore_1F_EventScript_FabasAlreadyRewarded::
	msgbox MamaIsland_GoblinStore_1F_Text_FabasAlreadyRewarded, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_FabasAlreadyRewarded:
	.string "I already gave you your Potion.\n"
	.string "Take good care of it!$"


@ ---------- BATALHA CONTRA MAMA ----------

MamaIsland_GoblinStore_1F_EventScript_Mama::
	trainerbattle_single TRAINER_GOBLIN_STORE_MAMA, MamaIsland_GoblinStore_1F_Text_MamaIntro, MamaIsland_GoblinStore_1F_Text_MamaDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_MamaPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_MamaIntro:
	.string "Mama intro$"

MamaIsland_GoblinStore_1F_Text_MamaDefeat:
	.string "Mama lost$"

MamaIsland_GoblinStore_1F_Text_MamaPostBattle:
	.string "Mama post battle$"


@ ---------- BATALHA CONTRA GUSTAVS ----------

MamaIsland_GoblinStore_1F_EventScript_Gustavs::
	trainerbattle_single TRAINER_GOBLIN_STORE_GUSTAVS, MamaIsland_GoblinStore_1F_Text_GustavsIntro, MamaIsland_GoblinStore_1F_Text_GustavsDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_GustavsPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_GustavsIntro:
	.string "Gustavs intro$"

MamaIsland_GoblinStore_1F_Text_GustavsDefeat:
	.string "Gustavs lost$"

MamaIsland_GoblinStore_1F_Text_GustavsPostBattle:
	.string "Gustavs post battle$"


@ ---------- BATALHA CONTRA MOTT ----------

MamaIsland_GoblinStore_1F_EventScript_Mott::
	trainerbattle_single TRAINER_GOBLIN_STORE_MOTT, MamaIsland_GoblinStore_1F_Text_MottIntro, MamaIsland_GoblinStore_1F_Text_MottDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_MottPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_MottIntro:
	.string "Mott intro$"

MamaIsland_GoblinStore_1F_Text_MottDefeat:
	.string "Mott lost$"

MamaIsland_GoblinStore_1F_Text_MottPostBattle:
	.string "Mott post battle$"


@ ---------- BATALHA CONTRA DUDU ----------

MamaIsland_GoblinStore_1F_EventScript_Dudu::
	trainerbattle_single TRAINER_GOBLIN_STORE_DUDU, MamaIsland_GoblinStore_1F_Text_DuduIntro, MamaIsland_GoblinStore_1F_Text_DuduDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_DuduPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_DuduIntro:
	.string "Dudu intro$"

MamaIsland_GoblinStore_1F_Text_DuduDefeat:
	.string "Dudu lost$"

MamaIsland_GoblinStore_1F_Text_DuduPostBattle:
	.string "Dudu post battle$"


@ ---------- BATALHA CONTRA BIZZU ----------

MamaIsland_GoblinStore_1F_EventScript_Bizzu::
	trainerbattle_single TRAINER_GOBLIN_STORE_BIZZU, MamaIsland_GoblinStore_1F_Text_BizzuIntro, MamaIsland_GoblinStore_1F_Text_BizzuDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_BizzuPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_BizzuIntro:
	.string "Bizzu intro$"

MamaIsland_GoblinStore_1F_Text_BizzuDefeat:
	.string "Bizzu lost$"

MamaIsland_GoblinStore_1F_Text_BizzuPostBattle:
	.string "Bizzu post battle$"


@ ---------- BATALHA CONTRA RUBIN ----------

MamaIsland_GoblinStore_1F_EventScript_Rubin::
	trainerbattle_single TRAINER_GOBLIN_STORE_RUBIN, MamaIsland_GoblinStore_1F_Text_RubinIntro, MamaIsland_GoblinStore_1F_Text_RubinDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_RubinPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_RubinIntro:
	.string "Rubin intro$"

MamaIsland_GoblinStore_1F_Text_RubinDefeat:
	.string "Rubin lost$"
MamaIsland_GoblinStore_1F_Text_RubinPostBattle:
	.string "Rubin post battle$"
