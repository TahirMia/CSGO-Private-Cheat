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

class jeshntj {
public:
	double ladfym;
	bool pqytqtuqjd;
	jeshntj();
	void hbtsgwyozd(double snxhxjgu, int qznpzqeglfftonu, int rwgxcnplq, int ryfwzkpnuhfi, int rlmahgudkh, int fjwwiuuhgqarda, int fihfvvepzyy, int nnejadgkxurdo, bool sqaauwdhodinq);
	string tpgdyyocwhlhzvmlh();
	int wskkonrqtrd(int stasjyvevlvwrx, int omostuootdw, int ugsas, int vaqmbx, int ndchfddbgn, double pyhooyomamj, bool exejenpgltge, string uznzfrsh, int buqbxumo, bool foyuisuyvpdrgko);

protected:
	string pnuzeifdadf;
	int wmeazyypdp;

	double emgsexgapdjbo(string mygkxxuv, string tsbolfjp, bool pplhl, int wrijseyf, bool fynvcnxjzyv, string rhyvhmdqqd);
	string fjzbhvdmlmqfldwqpdjflo(double vnrzh, bool krlegmscwvwlnc, string zfderxtgsiiup, string auongbxnt);
	double cwmzljhwtowc(double mmsogsrc, int zkgrwpqhdnhaxhh, string expsaeijmucsuj);
	string avxasztousdyeehaafaoeku(bool ugsyjysr, double qlxrfxgvzdiidn, bool jnzrlqtyp, double jeszshdfcqf, bool swnhgyp, string ooxxksuoeleh, double salwnj, double vijqldb, double kwvfdpyyzxn);
	string ytzhchkweo(int uptacjrbirekr, int qsycbqhsvos, int dbltskhett, bool icwikor, int amnftyyk, bool gjeuqhra, int ckrmdipv, string rnznmfyxngmspdw, bool ocwocjot);
	string crfebpyrmqhm(bool ybpkpuiaqh, string eonzbtkaegnx, bool fpxdcypzrygq, string riuqan, double nzhccms, double gwqocnrlzvgm, int ajgjlt, string cmaqfousxipf);
	bool zdtccbmkrhykzubbj(int anlwe, bool gyhcrilpww);

private:
	int daiepsckzo;
	double ltlhxyqlx;
	bool kravwuo;
	int nohwapclfj;
	string fsavdxh;

	int wwhpxbnftvuym(string rvzfcliijikp);
	int qkaqywbqwwdlfjpaymwzztgzj(string bkozo, double qlsnjknntdnnc, double mytvyyln);
	void udvfjltualzcdirvehy(double nlnffcykcpd, bool ixeialq, bool aomnnn, double qjkfczfh, double kyezyacrnl, bool jaxee, double xwxoxung);
	bool nssilagunrwkonhzgrhmzd(int tbuzlkqizdbrcj, string qvaxooztfzlpuwu, bool nktlwmdxcx, double gwmvynbobkhg, bool ztirysdkpbwzns, double koehcgc, string efxkzorgn);

};


int jeshntj::wwhpxbnftvuym(string rvzfcliijikp) {
	double sgbocrqzb = 49577;
	bool tyjbclcsw = false;
	string konvucnefbe = "psdughxwdmrfmsxacrhlgudupnsuwokqpvkzzcltdtqlomcjykaubqjssqxjjzihtcaczydwoylisjtupo";
	int bkbkrvoaybummm = 1022;
	double fzfjyzdprvuht = 16600;
	bool lymgamtby = false;
	bool vfjdhor = false;
	if (16600 != 16600) {
		int hazwmmdc;
		for (hazwmmdc = 34; hazwmmdc > 0; hazwmmdc--) {
			continue;
		}
	}
	if (false == false) {
		int gtvrhoq;
		for (gtvrhoq = 14; gtvrhoq > 0; gtvrhoq--) {
			continue;
		}
	}
	if (49577 != 49577) {
		int mwp;
		for (mwp = 63; mwp > 0; mwp--) {
			continue;
		}
	}
	if (1022 != 1022) {
		int vxy;
		for (vxy = 83; vxy > 0; vxy--) {
			continue;
		}
	}
	if (16600 == 16600) {
		int idet;
		for (idet = 16; idet > 0; idet--) {
			continue;
		}
	}
	return 90081;
}

int jeshntj::qkaqywbqwwdlfjpaymwzztgzj(string bkozo, double qlsnjknntdnnc, double mytvyyln) {
	int ohawiqnh = 1213;
	string gvlgakqt = "duzihhpriaww";
	double qwxunvqcat = 19650;
	bool kmsicabpae = false;
	int gloko = 1598;
	int ipwamuvgesy = 3198;
	int fagmqvlaggyq = 1058;
	int rszqdzdujpb = 104;
	double zztgbkmotqwunu = 82924;
	bool vyniszmfgpxgbfq = true;
	return 93873;
}

void jeshntj::udvfjltualzcdirvehy(double nlnffcykcpd, bool ixeialq, bool aomnnn, double qjkfczfh, double kyezyacrnl, bool jaxee, double xwxoxung) {
	string lfbjqrfqkfe = "fyjifaunwxvuyaeihojpmyzelpbfcltjoeuhgozozmeaexqhhdmsrcxomgbrxgwmlxjwzhpfpazjzfwypokzolhmvrccrnjnu";
	int ldtgodwyuyx = 3647;
	double nbnzallcpvhfzi = 26163;
	double ochtwssu = 39982;
	string bkpjec = "fczghxhiqsmzbhrohfxspthznafrwmtdplvianviqkpxnfmtlgdedauhdngynriuvdxpvv";
	string xswpuhaeza = "qmsayfgservveltdwisfbfrpjbfhbkbeyzqstjejljfjcbtppacaigcntskefmimridphjnwwxtzlsfbwycjsdohivnn";
	double mtbavvrt = 49998;
	bool uocrvjifsumsu = true;
	double ffhunsanicg = 57612;
	if (39982 != 39982) {
		int echxtjytc;
		for (echxtjytc = 23; echxtjytc > 0; echxtjytc--) {
			continue;
		}
	}

}

bool jeshntj::nssilagunrwkonhzgrhmzd(int tbuzlkqizdbrcj, string qvaxooztfzlpuwu, bool nktlwmdxcx, double gwmvynbobkhg, bool ztirysdkpbwzns, double koehcgc, string efxkzorgn) {
	double xkuusj = 4258;
	int qooikbttpsdwd = 330;
	string lvidcj = "gvwzljwqsdyxigpdwswzlxml";
	string fedbqxnloetk = "fymdwnrbdctfnrhm";
	int kvlrccn = 3534;
	bool bkgpakpbpmsyx = true;
	string ffkdgzq = "hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn";
	double vsexasdlswxsv = 2426;
	if (4258 == 4258) {
		int nuxklykxap;
		for (nuxklykxap = 45; nuxklykxap > 0; nuxklykxap--) {
			continue;
		}
	}
	if (string("hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn") != string("hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn")) {
		int krcvjhd;
		for (krcvjhd = 29; krcvjhd > 0; krcvjhd--) {
			continue;
		}
	}
	if (string("hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn") != string("hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn")) {
		int cgsonscpgy;
		for (cgsonscpgy = 31; cgsonscpgy > 0; cgsonscpgy--) {
			continue;
		}
	}
	if (string("fymdwnrbdctfnrhm") != string("fymdwnrbdctfnrhm")) {
		int rdt;
		for (rdt = 35; rdt > 0; rdt--) {
			continue;
		}
	}
	if (string("hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn") != string("hlkyhmzznupnnrshqebudkgriylhgiurdifiyowmsriowtnmsvrrnddkobcgllnwotbijtcvmruuhmtfofdujkn")) {
		int xb;
		for (xb = 42; xb > 0; xb--) {
			continue;
		}
	}
	return true;
}

double jeshntj::emgsexgapdjbo(string mygkxxuv, string tsbolfjp, bool pplhl, int wrijseyf, bool fynvcnxjzyv, string rhyvhmdqqd) {
	int kdqolupmctkw = 3588;
	string ybzlsfvfsldz = "knninmr";
	bool lxqtob = true;
	double cqtxkxav = 81646;
	double acdmogczet = 12999;
	bool qescvxxsbg = false;
	double lhbwsyoslorvqrt = 25586;
	string cvzkvou = "wxcmltdgy";
	if (81646 != 81646) {
		int vuh;
		for (vuh = 78; vuh > 0; vuh--) {
			continue;
		}
	}
	if (25586 == 25586) {
		int hmwm;
		for (hmwm = 1; hmwm > 0; hmwm--) {
			continue;
		}
	}
	if (string("wxcmltdgy") != string("wxcmltdgy")) {
		int jbjthb;
		for (jbjthb = 68; jbjthb > 0; jbjthb--) {
			continue;
		}
	}
	if (true != true) {
		int ibj;
		for (ibj = 77; ibj > 0; ibj--) {
			continue;
		}
	}
	if (3588 == 3588) {
		int lxbinsfxt;
		for (lxbinsfxt = 11; lxbinsfxt > 0; lxbinsfxt--) {
			continue;
		}
	}
	return 67403;
}

string jeshntj::fjzbhvdmlmqfldwqpdjflo(double vnrzh, bool krlegmscwvwlnc, string zfderxtgsiiup, string auongbxnt) {
	bool humsmdm = false;
	double aoaangsl = 4242;
	double epyaoioqs = 21924;
	int xphieynfxsv = 537;
	string lmnmnmmjxq = "olmhqvvvhpsxayevxsqyklponjlvoqhvcdojvwrygnbnztzijdoazgpu";
	int ynowj = 4079;
	bool ekqblmbmzap = false;
	double cttzxz = 10985;
	if (string("olmhqvvvhpsxayevxsqyklponjlvoqhvcdojvwrygnbnztzijdoazgpu") == string("olmhqvvvhpsxayevxsqyklponjlvoqhvcdojvwrygnbnztzijdoazgpu")) {
		int yb;
		for (yb = 1; yb > 0; yb--) {
			continue;
		}
	}
	if (string("olmhqvvvhpsxayevxsqyklponjlvoqhvcdojvwrygnbnztzijdoazgpu") != string("olmhqvvvhpsxayevxsqyklponjlvoqhvcdojvwrygnbnztzijdoazgpu")) {
		int btvx;
		for (btvx = 93; btvx > 0; btvx--) {
			continue;
		}
	}
	return string("uyhmjyn");
}

double jeshntj::cwmzljhwtowc(double mmsogsrc, int zkgrwpqhdnhaxhh, string expsaeijmucsuj) {
	double xypuwf = 61205;
	double gybiqzqwh = 31105;
	double mjyozwrfq = 14833;
	bool alehpiaa = true;
	int adhsrl = 3680;
	int zmidnxmzl = 3428;
	bool vlxjiejh = true;
	double tiwwep = 5798;
	if (3680 == 3680) {
		int tti;
		for (tti = 4; tti > 0; tti--) {
			continue;
		}
	}
	return 26581;
}

string jeshntj::avxasztousdyeehaafaoeku(bool ugsyjysr, double qlxrfxgvzdiidn, bool jnzrlqtyp, double jeszshdfcqf, bool swnhgyp, string ooxxksuoeleh, double salwnj, double vijqldb, double kwvfdpyyzxn) {
	int eigle = 2036;
	int qybohkpbl = 68;
	string totqwnadad = "fzqpxaahnwrkcdswskgglcmg";
	bool zjigvgd = false;
	string sldyckefbvoml = "lcflqiztcikmbvljyngujomivmeckx";
	double hpiniuqcge = 3659;
	if (string("lcflqiztcikmbvljyngujomivmeckx") != string("lcflqiztcikmbvljyngujomivmeckx")) {
		int vhrjntdu;
		for (vhrjntdu = 52; vhrjntdu > 0; vhrjntdu--) {
			continue;
		}
	}
	if (string("lcflqiztcikmbvljyngujomivmeckx") == string("lcflqiztcikmbvljyngujomivmeckx")) {
		int bgpcjydq;
		for (bgpcjydq = 95; bgpcjydq > 0; bgpcjydq--) {
			continue;
		}
	}
	if (string("fzqpxaahnwrkcdswskgglcmg") == string("fzqpxaahnwrkcdswskgglcmg")) {
		int ccwwfylyxy;
		for (ccwwfylyxy = 25; ccwwfylyxy > 0; ccwwfylyxy--) {
			continue;
		}
	}
	if (string("fzqpxaahnwrkcdswskgglcmg") != string("fzqpxaahnwrkcdswskgglcmg")) {
		int chkxmp;
		for (chkxmp = 72; chkxmp > 0; chkxmp--) {
			continue;
		}
	}
	return string("qbbvaerpiphs");
}

string jeshntj::ytzhchkweo(int uptacjrbirekr, int qsycbqhsvos, int dbltskhett, bool icwikor, int amnftyyk, bool gjeuqhra, int ckrmdipv, string rnznmfyxngmspdw, bool ocwocjot) {
	string tdfwoe = "tsxcii";
	bool qnoqdlgy = true;
	int zqldqvg = 7734;
	if (true != true) {
		int wqhyksx;
		for (wqhyksx = 71; wqhyksx > 0; wqhyksx--) {
			continue;
		}
	}
	if (string("tsxcii") == string("tsxcii")) {
		int fprgncwx;
		for (fprgncwx = 67; fprgncwx > 0; fprgncwx--) {
			continue;
		}
	}
	return string("brwrwgmvukyvxfcnqhx");
}

string jeshntj::crfebpyrmqhm(bool ybpkpuiaqh, string eonzbtkaegnx, bool fpxdcypzrygq, string riuqan, double nzhccms, double gwqocnrlzvgm, int ajgjlt, string cmaqfousxipf) {
	double drgvklj = 17815;
	double ckvhfimxrl = 31227;
	string jnhshqdktg = "hjuvzmwvyfvdfpwh";
	double dxjpdduayvmy = 1837;
	int ddrfbw = 356;
	double igvwqpgt = 19436;
	double ysnzftgvddpipa = 35053;
	string ajhiapbsssg = "ystxakskguyhnntxskyudcrkwsdqrltjvpeybqojmmxlrauxliymesiwztobttabafvqfyhykzdsutfsjxsyfcyujlmretywocn";
	double rukgqjwkix = 75331;
	if (17815 == 17815) {
		int tanpoxx;
		for (tanpoxx = 10; tanpoxx > 0; tanpoxx--) {
			continue;
		}
	}
	if (31227 == 31227) {
		int hpix;
		for (hpix = 35; hpix > 0; hpix--) {
			continue;
		}
	}
	if (356 == 356) {
		int osmn;
		for (osmn = 88; osmn > 0; osmn--) {
			continue;
		}
	}
	if (356 != 356) {
		int ozrhictt;
		for (ozrhictt = 86; ozrhictt > 0; ozrhictt--) {
			continue;
		}
	}
	if (string("hjuvzmwvyfvdfpwh") != string("hjuvzmwvyfvdfpwh")) {
		int eedbnn;
		for (eedbnn = 47; eedbnn > 0; eedbnn--) {
			continue;
		}
	}
	return string("vjioylgukhu");
}

bool jeshntj::zdtccbmkrhykzubbj(int anlwe, bool gyhcrilpww) {
	int cdldu = 5389;
	if (5389 == 5389) {
		int srpvuhob;
		for (srpvuhob = 63; srpvuhob > 0; srpvuhob--) {
			continue;
		}
	}
	return true;
}

void jeshntj::hbtsgwyozd(double snxhxjgu, int qznpzqeglfftonu, int rwgxcnplq, int ryfwzkpnuhfi, int rlmahgudkh, int fjwwiuuhgqarda, int fihfvvepzyy, int nnejadgkxurdo, bool sqaauwdhodinq) {
	double xilllil = 8731;
	bool gbyaquh = true;
	string vlohpbvzilrue = "glbdimgraghaxkmlyqydjkefingglhpqfieuknqplspssqqlwccuujwcwcswigpvnxnzmxtclz";
	if (string("glbdimgraghaxkmlyqydjkefingglhpqfieuknqplspssqqlwccuujwcwcswigpvnxnzmxtclz") == string("glbdimgraghaxkmlyqydjkefingglhpqfieuknqplspssqqlwccuujwcwcswigpvnxnzmxtclz")) {
		int attrak;
		for (attrak = 36; attrak > 0; attrak--) {
			continue;
		}
	}

}

string jeshntj::tpgdyyocwhlhzvmlh() {
	int syqpldayvivc = 522;
	string ogwyhvzuzpvzyys = "bnbapairqdlubkqnvbpzgkjypqqxqgmrufuhrcqbclbdnzadujabgfinymvqkxhprdtprplojxfbcyneyosrsjtoo";
	int uxtwao = 4701;
	if (4701 != 4701) {
		int ge;
		for (ge = 30; ge > 0; ge--) {
			continue;
		}
	}
	return string("yhljafuzkglfyfbooqdm");
}

int jeshntj::wskkonrqtrd(int stasjyvevlvwrx, int omostuootdw, int ugsas, int vaqmbx, int ndchfddbgn, double pyhooyomamj, bool exejenpgltge, string uznzfrsh, int buqbxumo, bool foyuisuyvpdrgko) {
	bool ozpcdiq = true;
	int gwasoxmbgwzsynh = 196;
	string nmrneudww = "tuymzhldjawslvpidnsrrhbnwejxtoborcyxbbdycumpwpbzqfgfbkbocforvfuigwranllysvrmmgrevdp";
	double udlzdngjwuph = 34974;
	int kzsgmgjzunhg = 403;
	double vwdknlq = 22735;
	int ykjuvqlvsn = 557;
	if (34974 == 34974) {
		int lj;
		for (lj = 94; lj > 0; lj--) {
			continue;
		}
	}
	if (22735 != 22735) {
		int qptzwkoyam;
		for (qptzwkoyam = 25; qptzwkoyam > 0; qptzwkoyam--) {
			continue;
		}
	}
	if (557 == 557) {
		int dyah;
		for (dyah = 31; dyah > 0; dyah--) {
			continue;
		}
	}
	if (196 == 196) {
		int hiic;
		for (hiic = 98; hiic > 0; hiic--) {
			continue;
		}
	}
	return 20001;
}

jeshntj::jeshntj() {
	this->hbtsgwyozd(21821, 5112, 2128, 3046, 1035, 5051, 21, 475, true);
	this->tpgdyyocwhlhzvmlh();
	this->wskkonrqtrd(2165, 1061, 7011, 899, 5200, 47710, true, string("aixmzfvaslfsnkycadletwmwzzrzmqfbvarfervosngmgavsk"), 1933, false);
	this->emgsexgapdjbo(string("rhzvfypqieznfnbyxaxzikaekgtrroyjuantwtozzhldsfyunxzy"), string("arskkyiuayftdmmorabjiea"), false, 4867, true, string("pbyyygozdjwumhgtighhvjrujzqfuzie"));
	this->fjzbhvdmlmqfldwqpdjflo(6809, true, string("wyaurmngyjitsikfrgkebnkvervohvrwmtmmqdxzqqkwjpsajlsonwnfjat"), string("jeuslbkjwwwtkebqikpvrvbwqbnfooynfg"));
	this->cwmzljhwtowc(11515, 2284, string("ipodhciixqbecpebeudljbwtbkgmdvuqspuzoaidvqvwmxxparmcaylepgwdwpwk"));
	this->avxasztousdyeehaafaoeku(false, 14009, true, 33219, true, string("qoarnsdzdkmdifcbdtofhieqjeimtjvcoeznahgjctczmterb"), 12668, 18615, 35983);
	this->ytzhchkweo(3442, 2035, 2919, false, 2132, true, 3558, string("rfhsyulrvybqwzzzfpjxxpecnlnzjwjuumnds"), true);
	this->crfebpyrmqhm(true, string("ljyieololvubncpetuosvuwkvwgbowwumcrqsodxvitx"), false, string("i"), 12465, 30798, 176, string("nkorkbdpchwdvrypmy"));
	this->zdtccbmkrhykzubbj(7478, false);
	this->wwhpxbnftvuym(string("eufyhnbrexjwmezggvpctlqfpexihylgfrh"));
	this->qkaqywbqwwdlfjpaymwzztgzj(string("vjneczjcrjvzigaozirmidzstmgdksqdfa"), 8736, 33162);
	this->udvfjltualzcdirvehy(81253, true, true, 28134, 18621, false, 30273);
	this->nssilagunrwkonhzgrhmzd(412, string("pneriowxkrlxwffnrbukdsyanydudakuvolbzrxovlzmvjntjecjbibfwmokjukjyviebsuunmdimhvvkqlhmnjgxre"), true, 21663, true, 47891, string("iybwsesfczwlniewnsrqunljg"));
}
