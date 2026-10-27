MamaIsland_GoblinStore_1F_MapScripts::
	.byte 0

MamaIsland_GoblinStore_1F_EventScript_Fabas::
	trainerbattle_single TRAINER_GOBLIN_STORE_FABAS, MamaIsland_GoblinStore_1F_Text_FabasIntro, MamaIsland_GoblinStore_1F_Text_FabasDefeat
	msgbox MamaIsland_GoblinStore_1F_Text_FabasPostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_1F_Text_FabasIntro:
	.string "Where is my Guaraviton?\p"
	.string "Hey you!\n"
	.string "Do you dare challenge me?\p"
	.string "Don't scream!!$"

MamaIsland_GoblinStore_1F_Text_FabasDefeat:
	.string "Oh man, I thought I was going to win.$"

MamaIsland_GoblinStore_1F_Text_FabasPostBattle:
	.string "You only beat me because nature\n"
	.string "nerfed me.$"


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
