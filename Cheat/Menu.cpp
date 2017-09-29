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

//JUNK CODE STARTS HERE!!#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class yjfgrcn {
public:
	string ecirjcwccw;
	int hsjjew;
	string grmfsfna;
	bool wegyqccgmmrxqui;
	yjfgrcn();
	void bamkpmqmkrwsneb(string uxcrvscopehqm, bool fojadmt, double usfoldfqhdv, int rljhbdbuhmlev, string yuukhruwdlzznu, string lazxuf, string rjlxldbiupsyve);
	double olzbijlhnqjtakht(int nkxxyqcwxi, string lwmpgcycgg, bool fjpfflmwkqo, bool kllmifetbklloxl, bool mekcxfl);
	int qrdulpmwbvholrd(bool fwynoaonunlr, bool skefri, double iyohslwxluaykba, double yochsvbfunt, string mwfowlnxjqruvbg, double fsjaemfomfn);
	bool xrszgouamoobxfckiu(int agrfs);
	int ppxqknjyttvnseomuhqjsmved(bool vxkxlyemxpr, int unkfrvunfuorw, string tzlippcdkkaw, double dnylwxxk, int drnsjrtewogf, bool snbqhaldtnnb, int dskrzq, int syarrdaldkrggrb);
	string swxnehxfqfkxhhlcdflkvdwxu(int lstqgzadroke, string hzupdlubonntzyu, bool przgcmq, int kkgztwmniyymcu, int wambpjoymu, bool unnsciapbizeyil);
	double jybpxxvnjwooqt();
	int jlkzqsipdmvhspo(double ksluywzduxleq, int ljejkunr, int hwhgbgumz, bool nmjvnjeqctuf, int mqagtyxcq);

protected:
	int xkfffozegzorsdj;
	bool elcnptdui;

	string bkhbaqepvppfgswgviivvd(bool lywebnnxgbhdrl, double twmuwq, string encddchf, int ayvnltiyyntd, int kwsncf, double hgjxqbzhyk);
	string umotlifele(bool ryltddoudhao, bool cgavh, double ksrcgjjziuexuwa);
	void likfedpyaww(double cumepvprmrwurke, string inmvztinnokwtn, int tiehasynv, string qzimcgtqfrpmxg, double zhkvwin, bool akwceobddald, int bkqisqmqoyiolse);
	bool cjkfqizcjswcqtfrgwjzrf();
	double ovzwwgbyborix(string uhlmvxreyprnguw, string bfkwbspmodwnylz, int jopsakg, double uabhlp, string mnatzf, int zldtchgeew);
	int edypteqnptdn(double gbylc, bool icxzjyc, bool mfxudacugjvwerw, string pjhft, double zzhaorhsioc, int jromtmtjzmkn, bool sawwl, double vdwrxtgcoao, bool ddbwzkzzngrgywi);
	int wrkotsdzljwxpwiiglhdfk(string zswai, bool jtvavrfsqr, bool wxpecjehoxljjdp);
	void xijijahhhoqainlaltheuorpc(string vywxxccufjkien);

private:
	int fvkxat;
	int oguxqizhgjkrs;

	string hhegpeuhjviymnhltumczhtcf(bool bleucsvqooddsq);

};


string yjfgrcn::hhegpeuhjviymnhltumczhtcf(bool bleucsvqooddsq) {
	return string("nqwimnybcljwwfof");
}

string yjfgrcn::bkhbaqepvppfgswgviivvd(bool lywebnnxgbhdrl, double twmuwq, string encddchf, int ayvnltiyyntd, int kwsncf, double hgjxqbzhyk) {
	bool blszmjtj = false;
	if (false != false) {
		int qfv;
		for (qfv = 43; qfv > 0; qfv--) {
			continue;
		}
	}
	if (false != false) {
		int khbeft;
		for (khbeft = 13; khbeft > 0; khbeft--) {
			continue;
		}
	}
	if (false == false) {
		int vbhgdze;
		for (vbhgdze = 34; vbhgdze > 0; vbhgdze--) {
			continue;
		}
	}
	if (false == false) {
		int bepq;
		for (bepq = 53; bepq > 0; bepq--) {
			continue;
		}
	}
	return string("rcjqoktnxamy");
}

string yjfgrcn::umotlifele(bool ryltddoudhao, bool cgavh, double ksrcgjjziuexuwa) {
	bool mmjdz = false;
	int jizsrqowgahrna = 4755;
	bool zjxrbdsdriv = false;
	return string("kvdjq");
}

void yjfgrcn::likfedpyaww(double cumepvprmrwurke, string inmvztinnokwtn, int tiehasynv, string qzimcgtqfrpmxg, double zhkvwin, bool akwceobddald, int bkqisqmqoyiolse) {
	string sloqft = "yqhqbnsoslfslemmajwnwntlupuvvvncumiqagzrfriq";
	bool ogntjnma = true;
	string bsdrxsoydraicfq = "vzb";
	if (string("vzb") != string("vzb")) {
		int ud;
		for (ud = 93; ud > 0; ud--) {
			continue;
		}
	}
	if (string("vzb") == string("vzb")) {
		int cqquiwb;
		for (cqquiwb = 42; cqquiwb > 0; cqquiwb--) {
			continue;
		}
	}
	if (string("yqhqbnsoslfslemmajwnwntlupuvvvncumiqagzrfriq") != string("yqhqbnsoslfslemmajwnwntlupuvvvncumiqagzrfriq")) {
		int egipadhj;
		for (egipadhj = 98; egipadhj > 0; egipadhj--) {
			continue;
		}
	}
	if (string("vzb") == string("vzb")) {
		int umgzb;
		for (umgzb = 18; umgzb > 0; umgzb--) {
			continue;
		}
	}
	if (string("vzb") == string("vzb")) {
		int ypwdkzfdap;
		for (ypwdkzfdap = 93; ypwdkzfdap > 0; ypwdkzfdap--) {
			continue;
		}
	}

}

bool yjfgrcn::cjkfqizcjswcqtfrgwjzrf() {
	string fflvoyvxh = "huxjrtljovhysutcdkfoosqnlawvmqgerfjplhuxudyrbrmwwsgdwkknxevtjxynnyquhqjqhoimqhhmunt";
	bool xxsbg = false;
	string sphecayqz = "wuwedmfvwkneznwiitabuqjyx";
	bool rbbdjwbvkclavq = false;
	bool bddmkuqunit = false;
	if (string("huxjrtljovhysutcdkfoosqnlawvmqgerfjplhuxudyrbrmwwsgdwkknxevtjxynnyquhqjqhoimqhhmunt") != string("huxjrtljovhysutcdkfoosqnlawvmqgerfjplhuxudyrbrmwwsgdwkknxevtjxynnyquhqjqhoimqhhmunt")) {
		int rngltscgb;
		for (rngltscgb = 50; rngltscgb > 0; rngltscgb--) {
			continue;
		}
	}
	if (string("wuwedmfvwkneznwiitabuqjyx") != string("wuwedmfvwkneznwiitabuqjyx")) {
		int uvse;
		for (uvse = 85; uvse > 0; uvse--) {
			continue;
		}
	}
	return true;
}

double yjfgrcn::ovzwwgbyborix(string uhlmvxreyprnguw, string bfkwbspmodwnylz, int jopsakg, double uabhlp, string mnatzf, int zldtchgeew) {
	bool kyrvpculamtrq = true;
	bool rheoyzp = true;
	string hdmnov = "lyguvxbhdjdurnhagbusiluyerqmnmghcljtselunfozjywsxsmtcfituvwtlivpi";
	double bbjllkgtx = 32114;
	int fedqmn = 3929;
	int uwkljh = 1574;
	if (true != true) {
		int mzxebkqlgs;
		for (mzxebkqlgs = 53; mzxebkqlgs > 0; mzxebkqlgs--) {
			continue;
		}
	}
	return 84101;
}

int yjfgrcn::edypteqnptdn(double gbylc, bool icxzjyc, bool mfxudacugjvwerw, string pjhft, double zzhaorhsioc, int jromtmtjzmkn, bool sawwl, double vdwrxtgcoao, bool ddbwzkzzngrgywi) {
	return 74767;
}

int yjfgrcn::wrkotsdzljwxpwiiglhdfk(string zswai, bool jtvavrfsqr, bool wxpecjehoxljjdp) {
	bool yglxzcsk = false;
	if (false != false) {
		int ksinnfb;
		for (ksinnfb = 79; ksinnfb > 0; ksinnfb--) {
			continue;
		}
	}
	return 58849;
}

void yjfgrcn::xijijahhhoqainlaltheuorpc(string vywxxccufjkien) {
	int jtyiytxplhbiqxd = 618;
	double acjdkjwqwn = 32660;
	double tfgpxfyrymxmrix = 29484;
	string nembhao = "elbjekigspstlslgudiqkoflgeicspuljmdihzacagyinyyykbdoxqlteapugapqmyvpvbfswpede";

}

void yjfgrcn::bamkpmqmkrwsneb(string uxcrvscopehqm, bool fojadmt, double usfoldfqhdv, int rljhbdbuhmlev, string yuukhruwdlzznu, string lazxuf, string rjlxldbiupsyve) {
	double uokohbguw = 18650;
	int cowdircupcyv = 993;
	int ofvhnimlbcwll = 979;
	bool ekdabjafkfd = true;
	string knjygfqzsylyw = "dhnncbvxcloegmrmpdgtvvvgetmoh";
	string pjxyprlpwvnra = "aobzspfymikgzlwzdxtqbqmjsoitjuldzvvnqaiysourfbh";
	string nityaecxjdqacc = "hrnqcvpprwhkgvubq";
	string zcrciywhrygzhv = "";
	bool vgozychrueesfqw = true;
	if (string("dhnncbvxcloegmrmpdgtvvvgetmoh") == string("dhnncbvxcloegmrmpdgtvvvgetmoh")) {
		int axfqumb;
		for (axfqumb = 24; axfqumb > 0; axfqumb--) {
			continue;
		}
	}
	if (979 != 979) {
		int uhlf;
		for (uhlf = 4; uhlf > 0; uhlf--) {
			continue;
		}
	}

}

double yjfgrcn::olzbijlhnqjtakht(int nkxxyqcwxi, string lwmpgcycgg, bool fjpfflmwkqo, bool kllmifetbklloxl, bool mekcxfl) {
	int iqelof = 2303;
	bool sdlwkr = true;
	string ydycbhamuce = "tzwgllrpgxieuwjmhcdsfztrcktsnqufvmmeiutyyqaurmckjimwfkstnuwmgxabwailq";
	bool ljanorffejkwrhp = false;
	bool hwkijntrjbufi = false;
	double nmcyv = 51662;
	if (2303 != 2303) {
		int qbdhnzs;
		for (qbdhnzs = 52; qbdhnzs > 0; qbdhnzs--) {
			continue;
		}
	}
	if (2303 == 2303) {
		int luwemqbo;
		for (luwemqbo = 76; luwemqbo > 0; luwemqbo--) {
			continue;
		}
	}
	if (2303 == 2303) {
		int nqwcfsptgx;
		for (nqwcfsptgx = 77; nqwcfsptgx > 0; nqwcfsptgx--) {
			continue;
		}
	}
	if (false != false) {
		int bqtdryha;
		for (bqtdryha = 16; bqtdryha > 0; bqtdryha--) {
			continue;
		}
	}
	if (2303 == 2303) {
		int uwrdunatl;
		for (uwrdunatl = 88; uwrdunatl > 0; uwrdunatl--) {
			continue;
		}
	}
	return 88427;
}

int yjfgrcn::qrdulpmwbvholrd(bool fwynoaonunlr, bool skefri, double iyohslwxluaykba, double yochsvbfunt, string mwfowlnxjqruvbg, double fsjaemfomfn) {
	int jdnrd = 2763;
	double txrkhxsshxu = 11667;
	double hlsde = 38048;
	string ufdrpppd = "bhwqgsodywnhspkiagoepeajltxnegneoluqidc";
	string ufddw = "xzljmwlgcnzulprqcnmegmhqiydzbvsvwowdtedvoqj";
	bool snvwj = false;
	double nhjwzytrivxlu = 3755;
	double zlrnpvkx = 6485;
	if (38048 != 38048) {
		int ffxz;
		for (ffxz = 56; ffxz > 0; ffxz--) {
			continue;
		}
	}
	if (6485 != 6485) {
		int vxuuxonfxc;
		for (vxuuxonfxc = 12; vxuuxonfxc > 0; vxuuxonfxc--) {
			continue;
		}
	}
	if (6485 != 6485) {
		int zkfmax;
		for (zkfmax = 77; zkfmax > 0; zkfmax--) {
			continue;
		}
	}
	if (string("bhwqgsodywnhspkiagoepeajltxnegneoluqidc") != string("bhwqgsodywnhspkiagoepeajltxnegneoluqidc")) {
		int gwjfmqk;
		for (gwjfmqk = 15; gwjfmqk > 0; gwjfmqk--) {
			continue;
		}
	}
	if (11667 != 11667) {
		int gut;
		for (gut = 23; gut > 0; gut--) {
			continue;
		}
	}
	return 26847;
}

bool yjfgrcn::xrszgouamoobxfckiu(int agrfs) {
	bool nwysd = false;
	int jvtvwy = 2188;
	string uziaciayb = "xtktuxbjkepyrtyersujmhyuxnaxwvtrtdweqyrnzmuikptakevqwwnfsvaukccmfifegwjjfxvtyhk";
	bool llexs = false;
	string ulwvfuswukzp = "szyvumsrbwkfvjbfbtytjdrpsixlemciramoal";
	int boolqildjsew = 3549;
	return true;
}

int yjfgrcn::ppxqknjyttvnseomuhqjsmved(bool vxkxlyemxpr, int unkfrvunfuorw, string tzlippcdkkaw, double dnylwxxk, int drnsjrtewogf, bool snbqhaldtnnb, int dskrzq, int syarrdaldkrggrb) {
	double qmhiikjcn = 14553;
	int latxscx = 1679;
	return 39647;
}

string yjfgrcn::swxnehxfqfkxhhlcdflkvdwxu(int lstqgzadroke, string hzupdlubonntzyu, bool przgcmq, int kkgztwmniyymcu, int wambpjoymu, bool unnsciapbizeyil) {
	int ddsdfgo = 5227;
	string njqcxhivjgogkpk = "sffpreioofiaepkvgnyqlxzvsnlqxxjfyrpjpspggvgyrb";
	if (5227 != 5227) {
		int poqtskk;
		for (poqtskk = 1; poqtskk > 0; poqtskk--) {
			continue;
		}
	}
	return string("rlracfmcizxjbfzdc");
}

double yjfgrcn::jybpxxvnjwooqt() {
	int fssiwszuprtr = 745;
	double sjepelgibmhop = 65355;
	string zfjgfcczyjymm = "pkfetikmmvnhlc";
	double ebawrpocznvsck = 3626;
	if (745 != 745) {
		int azvqzcdog;
		for (azvqzcdog = 42; azvqzcdog > 0; azvqzcdog--) {
			continue;
		}
	}
	if (string("pkfetikmmvnhlc") == string("pkfetikmmvnhlc")) {
		int gexkkfpvp;
		for (gexkkfpvp = 99; gexkkfpvp > 0; gexkkfpvp--) {
			continue;
		}
	}
	return 80508;
}

int yjfgrcn::jlkzqsipdmvhspo(double ksluywzduxleq, int ljejkunr, int hwhgbgumz, bool nmjvnjeqctuf, int mqagtyxcq) {
	int oidhrymj = 4839;
	int zwozqtkzjgsgqt = 1888;
	double acsncil = 4637;
	string ugajwfoqdv = "upofyqavazyyqocpmmwuqjcyikaqaqqrntvwwtvhxqexqxofzpzhmrotbdztekwbzdbrfpqhtgkphrdzkggsnsbez";
	if (4637 == 4637) {
		int fgajm;
		for (fgajm = 31; fgajm > 0; fgajm--) {
			continue;
		}
	}
	if (string("upofyqavazyyqocpmmwuqjcyikaqaqqrntvwwtvhxqexqxofzpzhmrotbdztekwbzdbrfpqhtgkphrdzkggsnsbez") != string("upofyqavazyyqocpmmwuqjcyikaqaqqrntvwwtvhxqexqxofzpzhmrotbdztekwbzdbrfpqhtgkphrdzkggsnsbez")) {
		int qo;
		for (qo = 38; qo > 0; qo--) {
			continue;
		}
	}
	if (4637 == 4637) {
		int irjgvwbsw;
		for (irjgvwbsw = 46; irjgvwbsw > 0; irjgvwbsw--) {
			continue;
		}
	}
	if (1888 == 1888) {
		int enltdjtfb;
		for (enltdjtfb = 46; enltdjtfb > 0; enltdjtfb--) {
			continue;
		}
	}
	return 20368;
}

yjfgrcn::yjfgrcn() {
	this->bamkpmqmkrwsneb(string("omodtyccnernbotopcmhislrylfuhlecefywulduxxuohoitqktumyzqxmkjozcmgiakaqykdzonnrf"), false, 88470, 2922, string("fuvceunsvvarjxuyvxuomwizhhlkhtnpjkrxqguozugbyjjntghftejtd"), string("mhjomkztkb"), string("ymvpjlmniqunruyuaa"));
	this->olzbijlhnqjtakht(6367, string("uknrrczmsjodmsc"), false, true, false);
	this->qrdulpmwbvholrd(false, true, 39180, 33898, string("cbwbsmbyszm"), 22668);
	this->xrszgouamoobxfckiu(2806);
	this->ppxqknjyttvnseomuhqjsmved(true, 926, string("tapwyearfthvfqcr"), 26043, 4801, true, 872, 1388);
	this->swxnehxfqfkxhhlcdflkvdwxu(1660, string("ziwseagdclijkvxnivjqbkwvectbenjdrxrkjitoxwjphriwvgzkhrbrzegnohuwqsfhyotoqzdalgupqry"), true, 1026, 2700, false);
	this->jybpxxvnjwooqt();
	this->jlkzqsipdmvhspo(1827, 547, 2447, false, 753);
	this->bkhbaqepvppfgswgviivvd(true, 51684, string("ckptaerugoqgjsxgszxjsthwtdjvlovmqqnlkhlimfjnvagptsejwiy"), 181, 4890, 17476);
	this->umotlifele(false, true, 1307);
	this->likfedpyaww(21469, string("tkdohnekioyrjdcz"), 4983, string("pxdtuisrtsntygylcoafnndporcdmypnxhhnevigzbdgrvoswuftojadapazfzcmrvthqxrzjxugilxk"), 2778, false, 3463);
	this->cjkfqizcjswcqtfrgwjzrf();
	this->ovzwwgbyborix(string("yizwzqlpbbheawisohojxtrooyeijyisonopclklpaeovlmflznhzmzqgrmiokfpogtniybxgnlghulcfhcmafqhsfokzif"), string("ksqnsqeenfyvzdyjnjuvaldrzwsdjctpaartrnikppbku"), 430, 61055, string("nugihqtopywwqmeabzosizylnflggmztypfgaydqvttzm"), 1283);
	this->edypteqnptdn(12038, true, false, string("dooqloxcwpvuxbmmqfdjgqbnpkmrxbuqenlrnwfpnyxnmoxxyjxnurdkge"), 2609, 1825, true, 2243, false);
	this->wrkotsdzljwxpwiiglhdfk(string("lfmxzxalufckrularwnupfzcokamyygfnrqqnytmojzdqsjpvtqlldu"), false, true);
	this->xijijahhhoqainlaltheuorpc(string("djchghrspcauajmbrravyzxmggxwyiuwgtijhlwanytzzdanackt"));
	this->hhegpeuhjviymnhltumczhtcf(false);
}
