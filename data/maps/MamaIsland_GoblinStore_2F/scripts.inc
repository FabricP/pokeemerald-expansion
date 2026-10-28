MamaIsland_GoblinStore_2F_MapScripts::
	.byte 0


MamaIsland_GoblinStore_2F_EventScript_Alexandre::
	trainerbattle_single TRAINER_GOBLIN_STORE_ALEXANDRE, MamaIsland_GoblinStore_2F_Text_AlexandreIntro, MamaIsland_GoblinStore_2F_Text_AlexandreDefeat
	msgbox MamaIsland_GoblinStore_2F_Text_AlexandrePostBattle, MSGBOX_AUTOCLOSE
	end

MamaIsland_GoblinStore_2F_Text_AlexandreIntro:
	.string "Alexandre intro$"

MamaIsland_GoblinStore_2F_Text_AlexandreDefeat:
	.string "Alexandre lost$"

MamaIsland_GoblinStore_2F_Text_AlexandrePostBattle:
	.string "Alexandre post battle$"
