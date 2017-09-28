/*
Syn's AyyWare Framework 2015
*/

#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" // for the unload meme
#include "Interfaces.h"
#include "CRC32.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 630

AyyWareWindow Menu::Window;

void SaveCallbk()
{
	GUI.SaveWindowState(&Menu::Window, "config.xml");
}

void LoadCallbk()
{
	GUI.LoadWindowState(&Menu::Window, "config.xml");
}

void UnLoadCallbk()
{
	DoUnload = true;
}

void KnifeApplyCallbk()
{
	static ConVar* Meme = Interfaces::CVar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate");
}

void AyyWareWindow::Setup()
{
	SetPosition(350, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("Whip");

	RegisterTab(&RageBotTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&MiscTab);
	//RegisterTab(&Playerlist);

	RECT Client = GetClientArea();
	Client.bottom -= 29;

	RageBotTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	MiscTab.Setup();
	//Playerlist.Setup();

#pragma region Bottom Buttons
	SaveButton.SetText("Save Configuration");
	SaveButton.SetCallback(SaveCallbk);
	SaveButton.SetPosition(16, Client.bottom - 42);

	LoadButton.SetText("Load Configuration");
	LoadButton.SetCallback(LoadCallbk);
	LoadButton.SetPosition(203, Client.bottom - 42);
	
	UnloadButton.SetText("Complete Unload");
	UnloadButton.SetCallback(UnLoadCallbk);
	UnloadButton.SetPosition(396, Client.bottom - 42);

	RageBotTab.RegisterControl(&SaveButton);
	LegitBotTab.RegisterControl(&SaveButton);
	VisualsTab.RegisterControl(&SaveButton);
	MiscTab.RegisterControl(&SaveButton);
	//Playerlist.RegisterControl(&SaveButton);

	RageBotTab.RegisterControl(&LoadButton);
	LegitBotTab.RegisterControl(&LoadButton);
	VisualsTab.RegisterControl(&LoadButton);
	MiscTab.RegisterControl(&LoadButton);
	//Playerlist.RegisterControl(&LoadButton);

	RageBotTab.RegisterControl(&UnloadButton);
	LegitBotTab.RegisterControl(&UnloadButton);
	VisualsTab.RegisterControl(&UnloadButton);
	MiscTab.RegisterControl(&UnloadButton);
	//Playerlist.RegisterControl(&UnloadButton);

#pragma endregion Setting up the settings buttons
}

void CRageBotTab::Setup()
{
	SetTitle("Rage");

	ActiveLabel.SetPosition(16, 16);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, 16);
	RegisterControl(&Active);

#pragma region Aimbot

	AimbotGroup.SetPosition(16, 48);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(376, 270);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(39.f);
	AimbotGroup.PlaceLabledControl("FOV Range", this, &AimbotFov);

	AimbotSilentAim.SetFileId("aim_silent");
	AimbotGroup.PlaceLabledControl("Silent Aim", this, &AimbotSilentAim);

	AimbotPerfectSilentAim.SetFileId("aim_psilent");
	AimbotGroup.PlaceLabledControl("Perfect Silent", this, &AimbotPerfectSilentAim);

	AimbotAutoPistol.SetFileId("aim_autopistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceLabledControl("Aim Step", this, &AimbotAimStep);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key", this, &AimbotKeyBind);

	AimbotStopKey.SetFileId("aim_stop");
	AimbotGroup.PlaceLabledControl("Stop Aim", this, &AimbotStopKey);
#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(16, 334);
	TargetGroup.SetText("Target");
	TargetGroup.SetSize(376, 168);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("Closest To Crosshair");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Lowest Health");
	TargetGroup.PlaceLabledControl("Selection", this, &TargetSelection);

	TargetFriendlyFire.SetFileId("tgt_friendlyfire");
	TargetGroup.PlaceLabledControl("Friendly Fire", this, &TargetFriendlyFire);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	TargetHitbox.AddItem("Shins");
	TargetGroup.PlaceLabledControl("Hitbox", this, &TargetHitbox);

	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("Off"); //0
	TargetHitscan.AddItem("Low"); // 1
	TargetHitscan.AddItem("Medium"); // 2
	TargetHitscan.AddItem("High"); // 3
	TargetHitscan.AddItem("Extreme"); // 4
	TargetGroup.PlaceLabledControl("Hitscan", this, &TargetHitscan);

	TargetMultipoint.SetFileId("tgt_multipoint");
	TargetGroup.PlaceLabledControl("Multipoint", this, &TargetMultipoint);

	TargetPointscale.SetFileId("tgt_pointscale");
	TargetPointscale.SetBoundaries(0.f, 10.f);
	TargetPointscale.SetValue(5.f);
	TargetGroup.PlaceLabledControl("Aim Height", this, &TargetPointscale);
#pragma endregion Targetting controls 

#pragma region Accuracy
	AccuracyGroup.SetPosition(408, 48);
	AccuracyGroup.SetText("Accuracy");
	AccuracyGroup.SetSize(360, 240); //280
	RegisterControl(&AccuracyGroup);

	AccuracyRecoil.SetFileId("acc_norecoil");
	AccuracyGroup.PlaceLabledControl("Anti Recoil", this, &AccuracyRecoil);

	AccuracyAutoWall.SetFileId("acc_awall");
	AccuracyGroup.PlaceLabledControl("Auto Wall", this, &AccuracyAutoWall);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(1.f, 99.f);
	AccuracyMinimumDamage.SetValue(1.f);
	AccuracyGroup.PlaceLabledControl("Autowall Damage", this, &AccuracyMinimumDamage);

	AccuracyAutoStop.SetFileId("acc_stop");
	AccuracyGroup.PlaceLabledControl("Auto Stop / Crouch", this, &AccuracyAutoStop);

	AccuracyAutoScope.SetFileId("acc_scope");
	AccuracyGroup.PlaceLabledControl("Auto Scope", this, &AccuracyAutoScope);

	AccuracyHitchance.SetFileId("acc_chance");
	AccuracyHitchance.SetBoundaries(0, 100);
	AccuracyHitchance.SetValue(0);
	AccuracyGroup.PlaceLabledControl("Hit Chance", this, &AccuracyHitchance);
	
	AccuracyResolver.SetFileId("acc_aaa");
	AccuracyResolver.AddItem("Off");
	AccuracyResolver.AddItem("Auto Yaw Resolve");
	AccuracyResolver.AddItem("Auto Yaw + Pitch");
	AccuracyGroup.PlaceLabledControl("Anti Aim Resolver", this, &AccuracyResolver);

	AccuracyPositionAdjustment.SetFileId("acc_posadj");
	AccuracyGroup.PlaceLabledControl("Position Adjustment", this, &AccuracyPositionAdjustment);

	AccuracySmart.SetFileId("acc_smart");
	AccuracySmart.SetBoundaries(0, 20);
	AccuracySmart.SetValue(1);
	AccuracyGroup.PlaceLabledControl("Smart Aim", this, &AccuracySmart);
#pragma endregion  Accuracy controls get Setup in here

#pragma region AntiAim
	AntiAimGroup.SetPosition(408, 307); //344
	AntiAimGroup.SetText("Anti Aim");
	AntiAimGroup.SetSize(360, 195);
	RegisterControl(&AntiAimGroup);

	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl("Enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("None");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("SMAC Safe");
	AntiAimPitch.AddItem("Jitter");
	AntiAimPitch.AddItem("Static");
	AntiAimPitch.AddItem("Fake Down");
	AntiAimPitch.AddItem("Lisp Down");
	AntiAimPitch.AddItem("Lisp Up");
	AntiAimGroup.PlaceLabledControl("Pitch", this, &AntiAimPitch);

	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("None");
	AntiAimYaw.AddItem("Fake Edge");
	AntiAimYaw.AddItem("Fake Sideways");
	AntiAimYaw.AddItem("Fake Static");
	AntiAimYaw.AddItem("T Fake");
	AntiAimYaw.AddItem("Fake Jitter");
	AntiAimYaw.AddItem("Jitter");
	AntiAimYaw.AddItem("T Jitter");
	AntiAimYaw.AddItem("Back Jitter");
	AntiAimYaw.AddItem("Backwards");
	AntiAimYaw.AddItem("Fake Lowerbody");
	AntiAimGroup.PlaceLabledControl("Yaw", this, &AntiAimYaw);

	AntiAimEdge.SetFileId("aa_edge");
	AntiAimEdge.AddItem("Off");
	AntiAimEdge.AddItem("Normal");
	AntiAimGroup.PlaceLabledControl("Wall Detect", this, &AntiAimEdge);

	AntiAimOffset.SetFileId("aa_offset");
	AntiAimOffset.SetBoundaries(0, 360);
	AntiAimOffset.SetValue(0);
	AntiAimGroup.PlaceLabledControl("Yaw Offset", this, &AntiAimOffset);

	AntiAimKnife.SetFileId("aa_knife");
	AntiAimGroup.PlaceLabledControl("Anti Aim on Knife", this, &AntiAimKnife);

	AntiAimTarget.SetFileId("aa_target");
	AntiAimGroup.PlaceLabledControl("Anti Aim At Target", this, &AntiAimTarget);
#pragma endregion  AntiAim controls get setup in here
}

void CLegitBotTab::Setup()
{
	SetTitle("Legit");

	ActiveLabel.SetPosition(16, 16);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, 16);
	RegisterControl(&Active);

#pragma region Aimbot
	AimbotGroup.SetPosition(16, 48);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(240, 210);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFriendlyFire.SetFileId("aim_friendfire");
	AimbotGroup.PlaceLabledControl("Friendly Fire", this, &AimbotFriendlyFire);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key Bind", this, &AimbotKeyBind);
	
	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

#pragma endregion Aimbot shit

#pragma region Triggerbot
	TriggerGroup.SetPosition(272, 48);
	TriggerGroup.SetText("Triggerbot");
	TriggerGroup.SetSize(240, 210);
	RegisterControl(&TriggerGroup);

	TriggerEnable.SetFileId("trig_enable");
	TriggerGroup.PlaceLabledControl("Enable", this, &TriggerEnable);

	TriggerKeyPress.SetFileId("trig_onkey");
	TriggerGroup.PlaceLabledControl("On Key Press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	TriggerGroup.PlaceLabledControl("Key Bind", this, &TriggerKeyBind);

	TriggerDelay.SetFileId("trig_time");
	TriggerDelay.SetBoundaries(0.f, 1000.f);
	TriggerGroup.PlaceLabledControl("Delay (ms)", this, &TriggerDelay);
#pragma endregion Triggerbot stuff

#pragma region Main Weapon
	WeaponMainGroup.SetPosition(16, 274);
	WeaponMainGroup.SetText("Rifles/Other");
	WeaponMainGroup.SetSize(240, 210);
	RegisterControl(&WeaponMainGroup);

	WeaponMainSpeed.SetFileId("main_speed");
	WeaponMainSpeed.SetBoundaries(0.1f, 2.f); 
	WeaponMainSpeed.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl("Max Speed", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fov");
	WeaponMainFoV.SetBoundaries(0.1f, 30.f);
	WeaponMainFoV.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl("FoV", this, &WeaponMainFoV);

	WeaponMainRecoil.SetFileId("main_recoil");
	WeaponMainGroup.PlaceLabledControl("Recoil", this, &WeaponMainRecoil);

	WeaponMainPSilent.SetFileId("main_psilent");
	WeaponMainGroup.PlaceLabledControl("Perfect Silent", this, &WeaponMainPSilent);

	WeaponMainInacc.SetFileId("main_inacc");
	WeaponMainInacc.SetBoundaries(0.f, 15.f);
	WeaponMainGroup.PlaceLabledControl("Inaccuracy", this, &WeaponMainInacc);

	WeaponMainHitbox.SetFileId("main_hitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainGroup.PlaceLabledControl("Hitbox", this, &WeaponMainHitbox);
#pragma endregion

#pragma region Pistols
	WeaponPistGroup.SetPosition(272, 274);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetSize(240, 210);
	RegisterControl(&WeaponPistGroup);

	WeaponPistSpeed.SetFileId("pist_speed");
	WeaponPistSpeed.SetBoundaries(0.1f, 2.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl("Max Speed", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov");
	WeaponPistFoV.SetBoundaries(0.1f, 30.f);
	WeaponPistFoV.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl("FoV", this, &WeaponPistFoV);

	WeaponPistRecoil.SetFileId("pist_recoil");
	WeaponPistGroup.PlaceLabledControl("Recoil", this, &WeaponPistRecoil);

	WeaponPistPSilent.SetFileId("pist_psilent");
	WeaponPistGroup.PlaceLabledControl("Perfect Silent", this, &WeaponPistPSilent);

	WeaponPistInacc.SetFileId("pist_inacc");
	WeaponPistInacc.SetBoundaries(0.f, 15.f);
	WeaponPistGroup.PlaceLabledControl("Inaccuracy", this, &WeaponPistInacc);

	WeaponPistHitbox.SetFileId("pist_hitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistGroup.PlaceLabledControl("Hitbox", this, &WeaponPistHitbox);
#pragma endregion

#pragma region Snipers
	WeaponSnipGroup.SetPosition(528, 274);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetSize(240, 210);
	RegisterControl(&WeaponSnipGroup);

	WeaponSnipSpeed.SetFileId("snip_speed");
	WeaponSnipSpeed.SetBoundaries(0.1f, 2.f);
	WeaponSnipSpeed.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl("Max Speed", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("snip_fov");
	WeaponSnipFoV.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoV.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl("FoV", this, &WeaponSnipFoV);

	WeaponSnipRecoil.SetFileId("snip_recoil");
	WeaponSnipGroup.PlaceLabledControl("Recoil", this, &WeaponSnipRecoil);

	WeaponSnipPSilent.SetFileId("snip_psilent");
	WeaponSnipGroup.PlaceLabledControl("Perfect Silent", this, &WeaponSnipPSilent);

	WeaponSnipInacc.SetFileId("snip_inacc");
	WeaponSnipInacc.SetBoundaries(0.f, 15.f);
	WeaponSnipGroup.PlaceLabledControl("Inaccuracy", this, &WeaponSnipInacc);

	WeaponSnipHitbox.SetFileId("snip_hitbox");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipGroup.PlaceLabledControl("Hitbox", this, &WeaponSnipHitbox);
#pragma endregion
}

void CVisualTab::Setup()
{
	SetTitle("Visuals");

	ActiveLabel.SetPosition(16, 16);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, 16);
	RegisterControl(&Active);

#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.SetPosition(16, 48);
	OptionsGroup.SetSize(193, 430);
	RegisterControl(&OptionsGroup);

	OptionsBox.SetFileId("opt_box");
	OptionsGroup.PlaceLabledControl("Box", this, &OptionsBox);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceLabledControl("Name", this, &OptionsName);

	OptionsHealth.SetFileId("opt_hp");
	OptionsGroup.PlaceLabledControl("Health", this, &OptionsHealth);

	OptionsWeapon.SetFileId("opt_weapon");
	OptionsGroup.PlaceLabledControl("Weapon", this, &OptionsWeapon);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl("Info", this, &OptionsInfo);

	OptionsChams.SetFileId("opt_chams");
	OptionsChams.AddItem("Off");
	OptionsChams.AddItem("Normal");
	OptionsChams.AddItem("Flat");
	OptionsGroup.PlaceLabledControl("Chams", this, &OptionsChams);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl("Skeleton", this, &OptionsSkeleton);

	OptionsAimSpot.SetFileId("opt_aimspot");
	OptionsGroup.PlaceLabledControl("Head Cross", this, &OptionsAimSpot);
	
	OptionsCompRank.SetFileId("opt_comprank");
	OptionsGroup.PlaceLabledControl("Player Ranks", this, &OptionsCompRank);

#pragma endregion Setting up the Options controls

#pragma region Filters
	FiltersGroup.SetText("Filters");
	FiltersGroup.SetPosition(225, 48);
	FiltersGroup.SetSize(193, 430);
	RegisterControl(&FiltersGroup);

	FiltersAll.SetFileId("ftr_all");
	FiltersGroup.PlaceLabledControl("All", this, &FiltersAll);

	FiltersPlayers.SetFileId("ftr_players");
	FiltersGroup.PlaceLabledControl("Players", this, &FiltersPlayers);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	FiltersGroup.PlaceLabledControl("Enemies Only", this, &FiltersEnemiesOnly);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersGroup.PlaceLabledControl("Weapons", this, &FiltersWeapons);

	FiltersChickens.SetFileId("ftr_chickens");
	FiltersGroup.PlaceLabledControl("Chickens", this, &FiltersChickens);

	FiltersC4.SetFileId("ftr_c4");
	FiltersGroup.PlaceLabledControl("C4", this, &FiltersC4);
#pragma endregion Setting up the Filters controls

#pragma region Other
	OtherGroup.SetText("Other");
	OtherGroup.SetPosition(434, 48);
	OtherGroup.SetSize(334, 430);
	RegisterControl(&OtherGroup);

	OtherCrosshair.SetFileId("otr_xhair");
	OtherGroup.PlaceLabledControl("Crosshair", this, &OtherCrosshair);

	OtherRecoilCrosshair.SetFileId("otr_recoilhair");
	OtherRecoilCrosshair.AddItem("Off");
	OtherRecoilCrosshair.AddItem("Recoil Position");
	OtherRecoilCrosshair.AddItem("Radius");
	OtherGroup.PlaceLabledControl("Recoil Crosshair", this, &OtherRecoilCrosshair);

	OtherRadar.SetFileId("otr_radar");
	OtherGroup.PlaceLabledControl("Radar", this, &OtherRadar);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceLabledControl("No Visual Recoil", this, &OtherNoVisualRecoil);

	OtherNoFlash.SetFileId("otr_noflash");
	OtherGroup.PlaceLabledControl("No Flash", this, &OtherNoFlash);

	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("Off");
	OtherNoHands.AddItem("None");
	OtherNoHands.AddItem("Transparent");
	OtherNoHands.AddItem("Chams");
	OtherNoHands.AddItem("Rainbow");
	OtherGroup.PlaceLabledControl("Hands", this, &OtherNoHands);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(0.f, 180.f);
	OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Viewmodel FOV Changer", this, &OtherViewmodelFOV);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 180.f);
	OtherFOV.SetValue(90.f);
	OtherGroup.PlaceLabledControl("Field of View Changer", this, &OtherFOV);
#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	SetTitle("Misc");

#pragma region Knife
	KnifeGroup.SetPosition(16, 16);
	KnifeGroup.SetSize(360, 126);
	KnifeGroup.SetText("Knife Changer");
	RegisterControl(&KnifeGroup);

	KnifeEnable.SetFileId("knife_enable");
	KnifeGroup.PlaceLabledControl("Enable", this, &KnifeEnable);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("Karam");
	KnifeModel.AddItem("Bayo");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("Doppler Sapphire");
	KnifeSkin.AddItem("Doppler Ruby");
	KnifeSkin.AddItem("Tiger");
	KnifeSkin.AddItem("Lore");
	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);

	KnifeApply.SetText("Apply Knife");
	KnifeApply.SetCallback(KnifeApplyCallbk);
	KnifeGroup.PlaceLabledControl("", this, &KnifeApply);

#pragma endregion

#pragma region Other
	OtherGroup.SetPosition(408, 16);
	OtherGroup.SetSize(360, 430);
	OtherGroup.SetText("Other");
	RegisterControl(&OtherGroup);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherAutoJump.AddItem("Off");
	OtherAutoJump.AddItem("Normal");
	OtherGroup.PlaceLabledControl("Auto Jump", this, &OtherAutoJump);

	OtherEdgeJump.SetFileId("otr_edgejump");
	OtherGroup.PlaceLabledControl("Edge Jump", this, &OtherEdgeJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherAutoStrafe.AddItem("Off");
	OtherAutoStrafe.AddItem("Legit");
	OtherAutoStrafe.AddItem("Rage");
	OtherGroup.PlaceLabledControl("Auto Strafer", this, &OtherAutoStrafe);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceLabledControl("Anti Untrust", this, &OtherSafeMode);

	OtherChatSpam.SetFileId("otr_spam");
	OtherChatSpam.AddItem("Off");
	OtherChatSpam.AddItem("Namestealer");
	OtherChatSpam.AddItem("Regular");
	OtherChatSpam.AddItem("Interwebz");
	OtherChatSpam.AddItem("Disperse Name");
	OtherGroup.PlaceLabledControl("Chat Spam", this, &OtherChatSpam);

	OtherClantag.SetFileId("otr_spam");
	OtherClantag.AddItem("Off");
	OtherClantag.AddItem("pasteware");
	OtherClantag.AddItem("skeet.cc");
	OtherClantag.AddItem("Blank");
	OtherClantag.AddItem("Valve");
	OtherGroup.PlaceLabledControl("Custom Clantag", this, &OtherClantag);

	OtherTeamChat.SetFileId("otr_teamchat");
	OtherGroup.PlaceLabledControl("Team Chat Only", this, &OtherTeamChat);

	OtherChatDelay.SetFileId("otr_chatdelay");
	OtherChatDelay.SetBoundaries(0.1, 3.0);
	OtherChatDelay.SetValue(0.5);
	OtherGroup.PlaceLabledControl("Spam Delay", this, &OtherChatDelay);

	OtherAirStuck.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Air Stuck", this, &OtherAirStuck);

	OtherSpectators.SetFileId("otr_speclist");
	OtherGroup.PlaceLabledControl("Spectators List", this, &OtherSpectators);

	OtherThirdperson.SetFileId("aa_thirdpsr");
	OtherGroup.PlaceLabledControl("Thirdperson", this, &OtherThirdperson);

#pragma endregion other random options

#pragma region FakeLag
	FakeLagGroup.SetPosition(16, 160);
	FakeLagGroup.SetSize(360, 141);
	FakeLagGroup.SetText("Fake Lag");
	RegisterControl(&FakeLagGroup);

	FakeLagEnable.SetFileId("fakelag_enable");
	FakeLagGroup.PlaceLabledControl("Fake Lag", this, &FakeLagEnable);

	FakeLagChoke.SetFileId("fakelag_choke");
	FakeLagChoke.SetBoundaries(0, 16);
	FakeLagChoke.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Choke Factor", this, &FakeLagChoke);

	FakeLagSend.SetFileId("fakelag_send");
	FakeLagSend.SetBoundaries(0, 16);
	FakeLagSend.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Send Factor", this, &FakeLagSend);

	ChokeRandomize.SetFileId("choke_random");
	FakeLagGroup.PlaceLabledControl("Randomize Choke", this, &ChokeRandomize);

	SendRandomize.SetFileId("send_random");
	FakeLagGroup.PlaceLabledControl("Randomize Send", this, &SendRandomize);
#pragma endregion fakelag shit

#pragma region Teleport
	TeleportGroup.SetPosition(16, 316);
	TeleportGroup.SetSize(360, 75);
	TeleportGroup.SetText("Teleport");
	RegisterControl(&TeleportGroup);

	TeleportEnable.SetFileId("teleport_enable");
	TeleportGroup.PlaceLabledControl("Enable", this, &TeleportEnable);

	TeleportKey.SetFileId("teleport_key");
	TeleportGroup.PlaceLabledControl("Key", this, &TeleportKey);

#pragma endregion

/*#pragma region OverideFov
	FOVGroup.SetPosition(16, 365);
	FOVGroup.SetSize(360, 75);
	FOVGroup.SetText("FOV Changer");
	RegisterControl(&FOVGroup);

	FOVEnable.SetFileId("fov_enable");
	FOVGroup.PlaceLabledControl("Enable", this, &FOVEnable);

	FOVSlider.SetFileId("fov_slider");
	FOVSlider.SetBoundaries(0, 200);
	FOVSlider.SetValue(0);
	FOVGroup.PlaceLabledControl("FOV Amount", this, &FOVSlider);

#pragma endregion*/
}

/*void CPlayersTab::Setup()
{
	SetTitle("PlayerList");

#pragma region PList

	pListGroup.SetPosition(16, 16);
	pListGroup.SetSize(680, 200);
	pListGroup.SetText("Player List");
	pListGroup.SetColumns(2);
	RegisterControl(&pListGroup);

	pListPlayers.SetPosition(26, 46);
	pListPlayers.SetSize(640, 50);
	pListPlayers.SetHeightInItems(20);
	RegisterControl(&pListPlayers);

#pragma endregion

#pragma region Options
	
	OptionsGroup.SetPosition(16, 257);
	OptionsGroup.SetSize(450, 120);
	OptionsGroup.SetText("Player Options");
	RegisterControl(&OptionsGroup);

	OptionsFriendly.SetFileId("pl_friendly");
	OptionsGroup.PlaceLabledControl("Friendly", this, &OptionsFriendly);

	OptionsAimPrio.SetFileId("pl_priority");
	OptionsGroup.PlaceLabledControl("Priority", this, &OptionsAimPrio);

	OptionsCalloutSpam.SetFileId("pl_callout");
	OptionsGroup.PlaceLabledControl("Callout Spam", this, &OptionsCalloutSpam);

#pragma endregion
}

DWORD GetPlayerListIndex(int EntId)
{
	player_info_t pinfo;
	Interfaces::Engine->GetPlayerInfo(EntId, &pinfo);

	// Bot
	if (pinfo.guid[0] == 'B' && pinfo.guid[1] == 'O')
	{
		char buf[64]; sprintf_s(buf, "BOT_420%sAY", pinfo.name);
		return CRC32(buf, 64);
	}
	else
	{
		return CRC32(pinfo.guid, 32);
	}
}

bool IsFriendly(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].Friendly;
	}

	return false;
}

bool IsAimPrio(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].AimPrio;
	}

	return false;
}

bool IsCalloutTarget(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].Callout;
	}

	return false;
}

void UpdatePlayerList()
{
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
	{
		Menu::Window.Playerlist.pListPlayers.ClearItems();

		// Loop through all active entitys
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
		{
			// Get the entity

			player_info_t pinfo;
			if (i != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerInfo(i, &pinfo))
			{
				IClientEntity* pEntity = Interfaces::EntList->GetClientEntity(i);
				int HP = 100; char* Location = "Unknown";
				char *Friendly = " ", *AimPrio = " ";

				DWORD plistId = GetPlayerListIndex(Menu::Window.Playerlist.pListPlayers.GetValue());
				if (PlayerList.find(plistId) != PlayerList.end())
				{
					Friendly = PlayerList[plistId].Friendly ? "Friendly" : "";
					AimPrio = PlayerList[plistId].AimPrio ? "AimPrio" : "";
				}

				if (pEntity && !pEntity->IsDormant())
				{
					HP = pEntity->GetHealth();
					Location = pEntity->GetLastPlaceName();
				}

				char nameBuffer[512];
				sprintf_s(nameBuffer, "%-24s %-10s %-10s [%d HP] [Last Seen At %s]", pinfo.name, IsFriendly(i) ? "Friend" : " ", IsAimPrio(i) ? "AimPrio" : " ", HP, Location);
				Menu::Window.Playerlist.pListPlayers.AddItem(nameBuffer, i);

			}

		}

		DWORD meme = GetPlayerListIndex(Menu::Window.Playerlist.pListPlayers.GetValue());

		// Have we switched to a different player?
		static int PrevSelectedPlayer = 0;
		if (PrevSelectedPlayer != Menu::Window.Playerlist.pListPlayers.GetValue())
		{
			if (PlayerList.find(meme) != PlayerList.end())
			{
				Menu::Window.Playerlist.OptionsFriendly.SetState(PlayerList[meme].Friendly);
				Menu::Window.Playerlist.OptionsAimPrio.SetState(PlayerList[meme].AimPrio);
				Menu::Window.Playerlist.OptionsCalloutSpam.SetState(PlayerList[meme].Callout);

			}
			else
			{
				Menu::Window.Playerlist.OptionsFriendly.SetState(false);
				Menu::Window.Playerlist.OptionsAimPrio.SetState(false);
				Menu::Window.Playerlist.OptionsCalloutSpam.SetState(false);

			}
		}
		PrevSelectedPlayer = Menu::Window.Playerlist.pListPlayers.GetValue();

		PlayerList[meme].Friendly = Menu::Window.Playerlist.OptionsFriendly.GetState();
		PlayerList[meme].AimPrio = Menu::Window.Playerlist.OptionsAimPrio.GetState();
		PlayerList[meme].Callout = Menu::Window.Playerlist.OptionsCalloutSpam.GetState();
	}
}*/

void Menu::SetupMenu()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	// General Processing

	// If the "all filter is selected tick all the others
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
	}

	GUI.Update();
	GUI.Draw();

	
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class yrpktyk {
public:
	int gffulsdnfi;
	int zhgdpijkhdwwor;
	yrpktyk();
	int fqgextszvggcramtuf();
	void kuqaznsrujpszwgwgmvopr(bool xwacsufcn, string soxfstptgqdnbjy, string lbepalprunqnujw, double inrxxflszghtng, int cgdahz, string ctkdtjiby);
	string uzjrippqrmwxklu(bool fnnwgqj, double wlpymeddofqx, double cufqtkqlww, bool ncabfksvvassaz, string jdztfklii);
	string rapkrdbzqmdphrcjvcfw(double kepwnahco, double eqtrdjzah);
	string ypalxtqnlqg(bool neysalbstbeon, double weqotgpqv, bool vfuwqcymlg);
	string etqluhrwniqwd(bool prdnkqimcdht, string rzafusrikptwjrq, int ghwgpajroua);
	string uqonmjrqbbtomdpucwionbpe(double azdwgldt, int ryfawr, string ajucjrq, string wufcnfgwd, int uoagfxpi, double evserswjopugxfc, int hyziqjsledu, int ahitbayoj, string bewoeefat);
	void mcewachhylicdoxsft(bool fawiprycc, double ejyymevis, double qzbjfc, bool hcmihaovj, int eyiduwgp, string gqxuqpjx, string xzdwdkis, string ycvtaaw);
	void lvmradxocsnwrktmowlkraxt(double apnxwafpaibcb, string czrfcfljst);

protected:
	int vcuanxicxqp;
	double mwswuejzkwgnkyv;
	double bazdniyxj;

	bool axkkclivmnsttopgrnpv(string gmhnplcgswzc, double ehqwkleeoi);
	double qnnwqqhoqcjxpmuzq(double hywcjb, int vkvyu);
	void nxpghyqtxtbbjsnsuccfsybzd(double lxbnvzns, bool odprnwqgih, string lfapaxyyz, bool cisuxwkuy, int dkrzryb, string ofithtkbmrlihsh, bool vugfybvkjyk, double udwldhs);
	double ymdeamlpktsgfp(bool dlncydkiihuw, double byiflafmuvvjivq, int fxfuyw, int pfwjx, double ubawlhlz);
	int xliqeyoxgxris(double wyidb, double wtetduq, double ebvearwftly);
	int lvohkwhmekf(double pnnygw, double blwxitucjawgqzx, bool mjbjnv, bool mmbwzjx, bool esvoat, double wgnpy, bool rtiucsjzsgnr);
	bool solyyiwerbexkiki(int ofdmwttrwoaygdj, string sfcvryy, bool jtqdqdlwuifp, double xiwmbqfle, double sgcbnnpehblozl, bool ibmlys, double xclfrngjl, bool hytzzvgqn, double oitemeql, double nebswnqiuwonv);

private:
	bool fyutyox;
	string bazxogsz;
	int xfvlraskpalsje;
	double mvlgayezg;
	double grjansdewsundte;

	void smuqoasexu(string lgwqdgswgttb, int hffcohtcwiuy, int rqxmfmkbtiija, bool rhsrpewhvxq, bool eyselfooy, double gklwjtykjkqfdrv, double ykckmexhdadkjn, int legyywpmawtfskk, string roybkinhon, string cgquglxm);
	void dwdmqybqndtecietocix();
	string hkfouvzrivmkeu(double diuuijhowomqr);
	bool ioisobclcqohiqmgawj(bool ccjfbbotggcoo, bool wdgcntkmiwqodgb);
	bool aybasnzxugovvcmmofpzqt(string xtthbthlzze, double upkuweeihfomv, int qbmwqnhuw, int upphvpam);
	void ygdwwxrjyceoib(double faxnxtxb, bool szsioevsvu);
	double hsfplwekrhwsdg(string hteaqoaqhdr, string attgtpo);
	int rbvebdhkrocxepky(bool szifp, int wzlbdhobnjmuns, double bnwiopmjnxhooz);
	void urcjjdnvyaijgw(string lwovnh, bool rxvwiezlhpn, int hnsvqukslm, int keptzeeccjoyupe, int vlovz, bool czfjailddjzoicg, int becfnwmoilmyyv, string gxexj);

};


void yrpktyk::smuqoasexu(string lgwqdgswgttb, int hffcohtcwiuy, int rqxmfmkbtiija, bool rhsrpewhvxq, bool eyselfooy, double gklwjtykjkqfdrv, double ykckmexhdadkjn, int legyywpmawtfskk, string roybkinhon, string cgquglxm) {
	int gsbxagdydkuktiy = 874;
	string ynyafleyyzmdzz = "ldwiugpefimqqsjvlkvpmjswnlzroa";
	double raekjfp = 25237;
	string mwrjkygm = "cbupplslmcwguvhbwjppqxfjemjczchtnmpkvmlyvvtyzre";
	double tnnwucu = 31054;
	bool ydhhh = false;
	bool meqfvb = false;
	string dkvulzbgyj = "ywbtuwqrlzoeagjajohnwj";

}

void yrpktyk::dwdmqybqndtecietocix() {
	string ucmfa = "osavmnbrpqfizbjmxcpqqesmuxvjjmefgysyborgjejbn";
	string rfqww = "reypyaznsdgfimbvngrlfcwjqhxxulwnybuqbrrzjaujmycyvkdepcj";
	double oblxojrwgdsvu = 39609;
	string autzsulprmho = "ofnpgjhlxdbqtcbbnssrzixpilxwcpsbybapvuvqyduqrwmdg";
	string umqszmmqzun = "xlusxvysfedijgbtcnovqagnxzxffzlpqfefcminkfevefatryjs";
	double shwwvdza = 33572;
	double dgmxhel = 45982;
	double dpeveqf = 50218;
	if (string("osavmnbrpqfizbjmxcpqqesmuxvjjmefgysyborgjejbn") == string("osavmnbrpqfizbjmxcpqqesmuxvjjmefgysyborgjejbn")) {
		int qmk;
		for (qmk = 24; qmk > 0; qmk--) {
			continue;
		}
	}
	if (39609 != 39609) {
		int bollyqboty;
		for (bollyqboty = 54; bollyqboty > 0; bollyqboty--) {
			continue;
		}
	}

}

string yrpktyk::hkfouvzrivmkeu(double diuuijhowomqr) {
	int qdoojpd = 962;
	string vtuflfxq = "tjw";
	double bkosylnpwgsd = 35860;
	if (962 == 962) {
		int fcpbtc;
		for (fcpbtc = 67; fcpbtc > 0; fcpbtc--) {
			continue;
		}
	}
	if (35860 != 35860) {
		int mlrdlil;
		for (mlrdlil = 92; mlrdlil > 0; mlrdlil--) {
			continue;
		}
	}
	if (35860 != 35860) {
		int nkkwa;
		for (nkkwa = 76; nkkwa > 0; nkkwa--) {
			continue;
		}
	}
	if (35860 != 35860) {
		int solcvgj;
		for (solcvgj = 22; solcvgj > 0; solcvgj--) {
			continue;
		}
	}
	return string("lvcxf");
}

bool yrpktyk::ioisobclcqohiqmgawj(bool ccjfbbotggcoo, bool wdgcntkmiwqodgb) {
	string qwyxmeilhnjslmm = "wagrbjbfgzeeznahbbbb";
	double noqkzihzioqy = 1920;
	string tbfwykvpvo = "vunjfsjcbbuiagltbpdagwynyfrkmhxbnrcpaz";
	double ebqllnvbjtpjp = 69546;
	double vqobk = 4466;
	int vtrsqniti = 7381;
	int zhpudpsjd = 1483;
	bool zagyevfais = false;
	if (4466 == 4466) {
		int fzouqhuk;
		for (fzouqhuk = 42; fzouqhuk > 0; fzouqhuk--) {
			continue;
		}
	}
	if (string("wagrbjbfgzeeznahbbbb") == string("wagrbjbfgzeeznahbbbb")) {
		int edhnzpmuwq;
		for (edhnzpmuwq = 59; edhnzpmuwq > 0; edhnzpmuwq--) {
			continue;
		}
	}
	return true;
}

bool yrpktyk::aybasnzxugovvcmmofpzqt(string xtthbthlzze, double upkuweeihfomv, int qbmwqnhuw, int upphvpam) {
	double izlrtwgo = 3701;
	int dkqhzutybuv = 625;
	if (625 == 625) {
		int cbz;
		for (cbz = 29; cbz > 0; cbz--) {
			continue;
		}
	}
	if (3701 == 3701) {
		int ygm;
		for (ygm = 70; ygm > 0; ygm--) {
			continue;
		}
	}
	return false;
}

void yrpktyk::ygdwwxrjyceoib(double faxnxtxb, bool szsioevsvu) {
	double kigbjtofbzkxxn = 56045;
	bool ckbgwtjo = false;
	int tmqkbhxrvlv = 349;
	double xkfrjlado = 41988;
	string rlkkbyv = "jrbllfu";
	int ethqryt = 1119;
	string midjyozyvyakv = "fguplyulmszn";
	string opfgrporaa = "phnyjenjfhpymccrwusjktsqrwysgyqchkjmniioutovziuoicuvlepbhjfaxahqdmbqlctxiincusx";

}

double yrpktyk::hsfplwekrhwsdg(string hteaqoaqhdr, string attgtpo) {
	string gquuufbnd = "dntvfxxduixnmu";
	bool onqtoo = false;
	double osbdvpmxw = 10225;
	string mhcvhhzzuaqw = "uevtistueunnbnseayohhzngzkdpvzwhallgajw";
	double ysqifjqqbefdg = 43953;
	bool uudhokazz = true;
	bool sqguappzty = false;
	bool oxpfvfwys = false;
	int oedmeffwodic = 2088;
	if (10225 != 10225) {
		int aicehle;
		for (aicehle = 52; aicehle > 0; aicehle--) {
			continue;
		}
	}
	return 62400;
}

int yrpktyk::rbvebdhkrocxepky(bool szifp, int wzlbdhobnjmuns, double bnwiopmjnxhooz) {
	double werewbwiar = 7814;
	int dfhenwdy = 8455;
	if (7814 == 7814) {
		int ohj;
		for (ohj = 94; ohj > 0; ohj--) {
			continue;
		}
	}
	if (7814 == 7814) {
		int bovsllooo;
		for (bovsllooo = 9; bovsllooo > 0; bovsllooo--) {
			continue;
		}
	}
	if (7814 != 7814) {
		int fxedk;
		for (fxedk = 5; fxedk > 0; fxedk--) {
			continue;
		}
	}
	return 88429;
}

void yrpktyk::urcjjdnvyaijgw(string lwovnh, bool rxvwiezlhpn, int hnsvqukslm, int keptzeeccjoyupe, int vlovz, bool czfjailddjzoicg, int becfnwmoilmyyv, string gxexj) {
	string oeynuxuqdbsvbqj = "asrrguba";
	bool mjwwsfbtmdkkz = true;
	int bbwcybqvh = 5511;
	double qdrgdh = 61885;
	double zjbckaoxhuroxie = 16769;
	string ullftseudwxjxw = "jmpwuwjbfexotiezeyjxliamqikixmcqvxkbz";
	if (true != true) {
		int hxrynvxn;
		for (hxrynvxn = 45; hxrynvxn > 0; hxrynvxn--) {
			continue;
		}
	}
	if (16769 != 16769) {
		int fztimh;
		for (fztimh = 56; fztimh > 0; fztimh--) {
			continue;
		}
	}

}

bool yrpktyk::axkkclivmnsttopgrnpv(string gmhnplcgswzc, double ehqwkleeoi) {
	bool zykyd = false;
	string fnzghvgok = "itrdziixouwh";
	string mlpmhtw = "gxcigxypkfvsjbgvfoyqyntufoqkmdkcm";
	bool iiyjfafomfx = false;
	int yitpxduvkssklqf = 2827;
	if (string("itrdziixouwh") != string("itrdziixouwh")) {
		int kelscvvhv;
		for (kelscvvhv = 56; kelscvvhv > 0; kelscvvhv--) {
			continue;
		}
	}
	if (string("gxcigxypkfvsjbgvfoyqyntufoqkmdkcm") == string("gxcigxypkfvsjbgvfoyqyntufoqkmdkcm")) {
		int zgcdvfzyh;
		for (zgcdvfzyh = 71; zgcdvfzyh > 0; zgcdvfzyh--) {
			continue;
		}
	}
	if (string("gxcigxypkfvsjbgvfoyqyntufoqkmdkcm") != string("gxcigxypkfvsjbgvfoyqyntufoqkmdkcm")) {
		int dfltbyvx;
		for (dfltbyvx = 8; dfltbyvx > 0; dfltbyvx--) {
			continue;
		}
	}
	return true;
}

double yrpktyk::qnnwqqhoqcjxpmuzq(double hywcjb, int vkvyu) {
	int nxnlhetyse = 9063;
	string lhgqdntklkygmz = "qzdfqyxuoxgmhbczvozbjqsrxmlxhiiofrluxwrxmlelcvpbjqmnie";
	int mpyvwsuhrvjgug = 533;
	return 75001;
}

void yrpktyk::nxpghyqtxtbbjsnsuccfsybzd(double lxbnvzns, bool odprnwqgih, string lfapaxyyz, bool cisuxwkuy, int dkrzryb, string ofithtkbmrlihsh, bool vugfybvkjyk, double udwldhs) {
	double gzrhziuyoaqjm = 64454;
	string xvimvrjwh = "orjvcwsuclviqqlzyonlvmxxpxnjtlaumucgpfzflhmwweaangwtaunadyfkclpcrrntpsbgokpdj";
	int eofvfoa = 3837;
	int wsvxizmdiompbvh = 3620;
	bool brmjjqrprxuxa = false;
	int lylxcllmejt = 3776;

}

double yrpktyk::ymdeamlpktsgfp(bool dlncydkiihuw, double byiflafmuvvjivq, int fxfuyw, int pfwjx, double ubawlhlz) {
	return 12611;
}

int yrpktyk::xliqeyoxgxris(double wyidb, double wtetduq, double ebvearwftly) {
	bool cisofzpskrmg = false;
	bool yuaqs = true;
	bool tslczdymncu = false;
	int zyjru = 4317;
	string bfijr = "lpbkgynkafotidvrmimynmjpzzuxfyssmygfeldexnlvseyntiobteqmdtkpsnhzcqjel";
	string qvhjvfk = "pglubwotxkaromssxmxlgioctqurh";
	double cprojbbuoosxfgb = 23323;
	string oudzhhndcmthulr = "ukxcabvhfzkn";
	int lvbije = 8048;
	return 90728;
}

int yrpktyk::lvohkwhmekf(double pnnygw, double blwxitucjawgqzx, bool mjbjnv, bool mmbwzjx, bool esvoat, double wgnpy, bool rtiucsjzsgnr) {
	bool xlzvwrbnwlryelb = true;
	bool hcqvcbltyj = true;
	int ygyfudlszvkvm = 5113;
	return 31078;
}

bool yrpktyk::solyyiwerbexkiki(int ofdmwttrwoaygdj, string sfcvryy, bool jtqdqdlwuifp, double xiwmbqfle, double sgcbnnpehblozl, bool ibmlys, double xclfrngjl, bool hytzzvgqn, double oitemeql, double nebswnqiuwonv) {
	int ybdkvxenfcktuvz = 621;
	bool qxnfqgpjzdzmrle = false;
	string sjezigcsfkjvqy = "lsjvfyfmplnrtkhlrvyxxzqf";
	bool vxtbjcuf = true;
	double kjonvysjme = 6187;
	bool vavxejvnnty = false;
	string cbciy = "dkygarifwnyrfeytlbueacktwztdueeostvvwtovgkkrujartsuxb";
	int becbdxfdx = 5311;
	double bqihe = 1007;
	if (5311 == 5311) {
		int zcjnloa;
		for (zcjnloa = 67; zcjnloa > 0; zcjnloa--) {
			continue;
		}
	}
	if (string("lsjvfyfmplnrtkhlrvyxxzqf") == string("lsjvfyfmplnrtkhlrvyxxzqf")) {
		int taqt;
		for (taqt = 71; taqt > 0; taqt--) {
			continue;
		}
	}
	if (6187 == 6187) {
		int rqdqwdsh;
		for (rqdqwdsh = 60; rqdqwdsh > 0; rqdqwdsh--) {
			continue;
		}
	}
	return true;
}

int yrpktyk::fqgextszvggcramtuf() {
	bool ilwszskw = true;
	double bgplecu = 24897;
	string fwtmvzhgzmuz = "pqyaujwaachscmmywskhabjnlbnumy";
	string pnvzuxhgomvv = "jxqygmonouuikqmhtnysesjqbnvlebawbmpwymixdplidfizaxpnbwyixhfvbgnvdoprtnidvycvmdarqgovhfybiscaec";
	if (true != true) {
		int qrjd;
		for (qrjd = 80; qrjd > 0; qrjd--) {
			continue;
		}
	}
	if (string("pqyaujwaachscmmywskhabjnlbnumy") != string("pqyaujwaachscmmywskhabjnlbnumy")) {
		int xw;
		for (xw = 41; xw > 0; xw--) {
			continue;
		}
	}
	if (string("pqyaujwaachscmmywskhabjnlbnumy") != string("pqyaujwaachscmmywskhabjnlbnumy")) {
		int oterv;
		for (oterv = 35; oterv > 0; oterv--) {
			continue;
		}
	}
	if (24897 == 24897) {
		int qrbbw;
		for (qrbbw = 17; qrbbw > 0; qrbbw--) {
			continue;
		}
	}
	return 49329;
}

void yrpktyk::kuqaznsrujpszwgwgmvopr(bool xwacsufcn, string soxfstptgqdnbjy, string lbepalprunqnujw, double inrxxflszghtng, int cgdahz, string ctkdtjiby) {
	double tjnxwzddcfw = 5815;
	double cvvanstw = 51374;
	int gmnienpqrfepcj = 7015;
	bool jwrmqwfnbej = false;
	if (false == false) {
		int ymj;
		for (ymj = 96; ymj > 0; ymj--) {
			continue;
		}
	}
	if (51374 != 51374) {
		int pg;
		for (pg = 80; pg > 0; pg--) {
			continue;
		}
	}
	if (false != false) {
		int lxxocgdrqu;
		for (lxxocgdrqu = 55; lxxocgdrqu > 0; lxxocgdrqu--) {
			continue;
		}
	}
	if (5815 != 5815) {
		int dkuax;
		for (dkuax = 9; dkuax > 0; dkuax--) {
			continue;
		}
	}
	if (false == false) {
		int ypjey;
		for (ypjey = 27; ypjey > 0; ypjey--) {
			continue;
		}
	}

}

string yrpktyk::uzjrippqrmwxklu(bool fnnwgqj, double wlpymeddofqx, double cufqtkqlww, bool ncabfksvvassaz, string jdztfklii) {
	double yvylkhbsfwhdo = 17560;
	int vjzetxdwke = 987;
	bool zbapyfhfeknzsxs = true;
	double aujbd = 7531;
	if (true != true) {
		int qac;
		for (qac = 27; qac > 0; qac--) {
			continue;
		}
	}
	if (7531 == 7531) {
		int ff;
		for (ff = 94; ff > 0; ff--) {
			continue;
		}
	}
	if (7531 != 7531) {
		int caazwc;
		for (caazwc = 80; caazwc > 0; caazwc--) {
			continue;
		}
	}
	return string("yytnpbqxorvcti");
}

string yrpktyk::rapkrdbzqmdphrcjvcfw(double kepwnahco, double eqtrdjzah) {
	int cryty = 3823;
	int xwarwtapzihsmhm = 5569;
	string ojtiin = "wrdxptqvbwdsxfborafnpxjtsfeocubqpnppqhustbzgxtqamkboattpos";
	int hvifmuab = 1294;
	string uxavy = "gyapecbvgehqdslqijvlfuitkgegchgeubkozmuxfbjncfrdalijrdfbm";
	double idrigwmiawcszf = 12921;
	if (1294 == 1294) {
		int otj;
		for (otj = 30; otj > 0; otj--) {
			continue;
		}
	}
	if (string("gyapecbvgehqdslqijvlfuitkgegchgeubkozmuxfbjncfrdalijrdfbm") != string("gyapecbvgehqdslqijvlfuitkgegchgeubkozmuxfbjncfrdalijrdfbm")) {
		int ocdh;
		for (ocdh = 84; ocdh > 0; ocdh--) {
			continue;
		}
	}
	if (string("wrdxptqvbwdsxfborafnpxjtsfeocubqpnppqhustbzgxtqamkboattpos") != string("wrdxptqvbwdsxfborafnpxjtsfeocubqpnppqhustbzgxtqamkboattpos")) {
		int mg;
		for (mg = 31; mg > 0; mg--) {
			continue;
		}
	}
	return string("ztostsloyovkqp");
}

string yrpktyk::ypalxtqnlqg(bool neysalbstbeon, double weqotgpqv, bool vfuwqcymlg) {
	string bmqdrmyhji = "iqlarjzkq";
	if (string("iqlarjzkq") == string("iqlarjzkq")) {
		int jeynigwh;
		for (jeynigwh = 88; jeynigwh > 0; jeynigwh--) {
			continue;
		}
	}
	if (string("iqlarjzkq") == string("iqlarjzkq")) {
		int ppbv;
		for (ppbv = 70; ppbv > 0; ppbv--) {
			continue;
		}
	}
	if (string("iqlarjzkq") == string("iqlarjzkq")) {
		int tj;
		for (tj = 72; tj > 0; tj--) {
			continue;
		}
	}
	if (string("iqlarjzkq") == string("iqlarjzkq")) {
		int uuynpwfpjg;
		for (uuynpwfpjg = 58; uuynpwfpjg > 0; uuynpwfpjg--) {
			continue;
		}
	}
	return string("vowaga");
}

string yrpktyk::etqluhrwniqwd(bool prdnkqimcdht, string rzafusrikptwjrq, int ghwgpajroua) {
	string zxvsypntdqbww = "xdpxpemukjodwqritcmeflqpxehtszgdhuddldhtgriqmcisvcb";
	string lexfim = "bdtbrznvfapiggtmnzrcdvkxsuxmhn";
	string zrodf = "rieguzsqvmqrjkereucznvqiscardblqsbmanwymrryjqmhsodsciwlwagnlervntcvznfrbwhpffwhgwleb";
	if (string("rieguzsqvmqrjkereucznvqiscardblqsbmanwymrryjqmhsodsciwlwagnlervntcvznfrbwhpffwhgwleb") != string("rieguzsqvmqrjkereucznvqiscardblqsbmanwymrryjqmhsodsciwlwagnlervntcvznfrbwhpffwhgwleb")) {
		int nhrt;
		for (nhrt = 73; nhrt > 0; nhrt--) {
			continue;
		}
	}
	if (string("xdpxpemukjodwqritcmeflqpxehtszgdhuddldhtgriqmcisvcb") != string("xdpxpemukjodwqritcmeflqpxehtszgdhuddldhtgriqmcisvcb")) {
		int lbn;
		for (lbn = 19; lbn > 0; lbn--) {
			continue;
		}
	}
	if (string("bdtbrznvfapiggtmnzrcdvkxsuxmhn") != string("bdtbrznvfapiggtmnzrcdvkxsuxmhn")) {
		int qahm;
		for (qahm = 65; qahm > 0; qahm--) {
			continue;
		}
	}
	if (string("bdtbrznvfapiggtmnzrcdvkxsuxmhn") != string("bdtbrznvfapiggtmnzrcdvkxsuxmhn")) {
		int qtmsb;
		for (qtmsb = 52; qtmsb > 0; qtmsb--) {
			continue;
		}
	}
	if (string("bdtbrznvfapiggtmnzrcdvkxsuxmhn") != string("bdtbrznvfapiggtmnzrcdvkxsuxmhn")) {
		int rj;
		for (rj = 66; rj > 0; rj--) {
			continue;
		}
	}
	return string("vfbgxbhsvr");
}

string yrpktyk::uqonmjrqbbtomdpucwionbpe(double azdwgldt, int ryfawr, string ajucjrq, string wufcnfgwd, int uoagfxpi, double evserswjopugxfc, int hyziqjsledu, int ahitbayoj, string bewoeefat) {
	bool kmarovsc = false;
	bool foeryqnoiakvra = true;
	double mfyovyakpsptjtv = 9653;
	string grhvfpz = "fasdxqnxefipyxoimvgnvabgqsbksxeebapkggequzckqannihpusfo";
	string syyky = "wvowthodrigndufuezpmmxmcrzksytqiqjtubpcgtcgwxysatiicofczbubybumbjajnmvuxmwbneolivwz";
	if (string("wvowthodrigndufuezpmmxmcrzksytqiqjtubpcgtcgwxysatiicofczbubybumbjajnmvuxmwbneolivwz") == string("wvowthodrigndufuezpmmxmcrzksytqiqjtubpcgtcgwxysatiicofczbubybumbjajnmvuxmwbneolivwz")) {
		int rc;
		for (rc = 42; rc > 0; rc--) {
			continue;
		}
	}
	return string("vizoznjxpwcu");
}

void yrpktyk::mcewachhylicdoxsft(bool fawiprycc, double ejyymevis, double qzbjfc, bool hcmihaovj, int eyiduwgp, string gqxuqpjx, string xzdwdkis, string ycvtaaw) {

}

void yrpktyk::lvmradxocsnwrktmowlkraxt(double apnxwafpaibcb, string czrfcfljst) {
	double urbfrhgvotloghb = 18234;
	int uldjnqhn = 29;
	bool khkrrjnekstlrs = true;
	int ukoalpq = 2048;
	string vajmv = "kfwozjxiqcsplzluhdomwsoucfdkovyndwmcvtrxourxoxosygoydlxoqtnbcesexqpziswwhotvpkwvzjgtcvkdyxo";
	bool tddhglljiaiw = true;
	double izabipapyzjh = 16527;
	int ycelggeorhxktl = 645;
	bool bjxhup = true;
	if (true != true) {
		int rucuiyte;
		for (rucuiyte = 23; rucuiyte > 0; rucuiyte--) {
			continue;
		}
	}
	if (true == true) {
		int rgjkdwoixu;
		for (rgjkdwoixu = 79; rgjkdwoixu > 0; rgjkdwoixu--) {
			continue;
		}
	}
	if (18234 == 18234) {
		int soufdqk;
		for (soufdqk = 24; soufdqk > 0; soufdqk--) {
			continue;
		}
	}
	if (true != true) {
		int rhxcuo;
		for (rhxcuo = 33; rhxcuo > 0; rhxcuo--) {
			continue;
		}
	}
	if (true == true) {
		int ja;
		for (ja = 89; ja > 0; ja--) {
			continue;
		}
	}

}

yrpktyk::yrpktyk() {
	this->fqgextszvggcramtuf();
	this->kuqaznsrujpszwgwgmvopr(true, string("xflxhojvigczzpczrxcufmzxeuovffpbvlzoaiunmfgwsyxihdovvpeobxtzgjwfymlxhnmd"), string("zpwvvlnceopqdihfashwysfueaakgkntumcj"), 23890, 485, string("lxgrgpgoakovtewjerfxalrkhmoblxmlfhfwhwnl"));
	this->uzjrippqrmwxklu(true, 14470, 9701, false, string("polamwbiotlangmizukinjjp"));
	this->rapkrdbzqmdphrcjvcfw(31049, 25653);
	this->ypalxtqnlqg(true, 29744, false);
	this->etqluhrwniqwd(false, string("srzvqqpbqvkwwrrpuyntpfnrwmmdhzmyhyu"), 101);
	this->uqonmjrqbbtomdpucwionbpe(11452, 346, string("huowlnwxwsqogfrqskgdrmqoaloqssqkeazupjzybbovnylmgsreoyhzkpevsfyyaeqcfpjwqkjccnhzelyyftd"), string("enjijaikefkbrvpabmfnilwnrhdzmguplhbqojergkgypup"), 1459, 41156, 6338, 2348, string("dnxevhpixjljvmgvtbifoooasqzmundjjzxffqaiomvptsrlgdihyvcigorsf"));
	this->mcewachhylicdoxsft(true, 16595, 36871, false, 6791, string("rjnheitilhpwisvidszhampltstypcnlrpnqxbdcaohvwlalmwhkvpuulsmasmhka"), string("pkgemtziajqeedjuaivmynhibtyxsagvgwbvzieptdlgcsrbvpxtqiteqk"), string("iirbjpssfuyjzsaralfuzwqflhimonvdfaeqdedlkvocukw"));
	this->lvmradxocsnwrktmowlkraxt(36126, string("jhhlfufwrajoouuzsxbkmjoi"));
	this->axkkclivmnsttopgrnpv(string("gtidihbtnapbzgemztlggxqkfqyoyqzgedzcafbnmqc"), 42381);
	this->qnnwqqhoqcjxpmuzq(2874, 2938);
	this->nxpghyqtxtbbjsnsuccfsybzd(4666, false, string("mkamuvzxnhllyikaqsapkifnmlfgxcycmqodemkqgehzpouaywsiaboffvteddmjbtagbttsdtprobmzbdwzzgxiqnz"), false, 400, string("zeglxhqckbzbpuwzmdydsyqhnxgdsxwbbxhlucerwpnhejgxqbgytura"), false, 80239);
	this->ymdeamlpktsgfp(false, 4359, 812, 1536, 19295);
	this->xliqeyoxgxris(5119, 30519, 8929);
	this->lvohkwhmekf(39573, 7330, false, false, false, 32825, true);
	this->solyyiwerbexkiki(1721, string("ugmutyogkaokczfkblmxvwjoertgmsotabpnphbol"), true, 30863, 14112, true, 33193, true, 4842, 9466);
	this->smuqoasexu(string("wgxrejgiatf"), 74, 1338, true, false, 7293, 92948, 2980, string("brnirvdgnhnsdosnwufcvogtiqqfdjfykupptkbclreawlrcwlbrbhinygawlhqtqcbshytqvbdjfgakkaaelccc"), string("puobzptpqcszdtdjeriomqetiketxdgwnetdpzrceqqrjbv"));
	this->dwdmqybqndtecietocix();
	this->hkfouvzrivmkeu(37603);
	this->ioisobclcqohiqmgawj(false, true);
	this->aybasnzxugovvcmmofpzqt(string("hirnkbmmjdinkgauyidlnxaomtge"), 35712, 3846, 693);
	this->ygdwwxrjyceoib(50226, true);
	this->hsfplwekrhwsdg(string("zbtinmptsojchoemxmxtnmcxvnbqyopsyxirmsbqvzkogtiemsqefqrqkcgnmwyeolb"), string("se"));
	this->rbvebdhkrocxepky(true, 4518, 3534);
	this->urcjjdnvyaijgw(string("cankvbkapfiydksckezjoxompijsbnqnvffzvejkvdlqtrkaenzwezlblosmilabrmxvefbzcbujbed"), true, 4142, 2922, 2866, true, 4562, string("unowcdwadtrbvepuafelvincuztjvjourppycvnxzqmrlseyeusbfyghkaqwjxygcimhnjnvnlxripifzcxbfq"));
}
