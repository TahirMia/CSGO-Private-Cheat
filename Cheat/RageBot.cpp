/*
Syn's AyyWare Framework 2015
*/

#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"

#define TICK_INTERVAL			( Interfaces::Globals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

float hitchance(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	//	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	float hitchance = 101;
	if (!pWeapon) return 0;
	if (Menu::Window.RageBotTab.AccuracyHitchance.GetValue() > 1)
	{//Inaccuracy method
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;

	}
	return hitchance;
}

// (DWORD)g_pNetVars->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
// You need something like this
bool CanOpenFire() // Creds to untrusted guy
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return;

	// Master switch
	if (!Menu::Window.RageBotTab.Active.GetState())
		return;

	// Anti Aim 
	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);
			}
			ChokedPackets = -1;
		}
	}

	// Position Adjustment
	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
		PositionAdjustment(pCmd);

	// Aimbot
	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	// Recoil
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	// Aimstep
	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}

	LastAngle = pCmd->viewangles;
}

Vector BestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* pLocal = hackManager.pLocal();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;
}

// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd,bool &bSendPacket) // Creds to encore1337 for getting it to work
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	bool FindNewTarget = true;
	//IsLocked = false;

	CSWeaponInfo* weapInfo = ((CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();

	// Don't aimbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			//TargetID = 0;
			//pTarget = nullptr;
			//HitBox = -1;
			return;
		}
	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View; 
				Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					FindNewTarget = false;
			}
		}
	}

	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		// Target selection type
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		}

		// Memes
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	} 

	Globals::Target = pTarget;
	Globals::TargetID = TargetID;

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		// Get the hitbox to shoot at
		HitBox = HitScan(pTarget);

		if (!CanOpenFire())
			return;

		// Key
		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		// Stop key
		int StopKey = Menu::Window.RageBotTab.AimbotStopKey.GetKey();
		if (StopKey >= 0 && GUI.GetKeyState(StopKey))
		{
			TargetID = -1;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}

		float pointscale = Menu::Window.RageBotTab.TargetPointscale.GetValue() - 5.f; // Aim height
//		float value = Menu::Window.RageBotTab.AccuracyHitchance.GetValue(); // Hitchance

		Vector Point;
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox) + Vector(0, 0, pointscale);

		if (Menu::Window.RageBotTab.TargetMultipoint.GetState())
		{
			Point = BestPoint(pTarget, AimPoint);
		}
		else
		{
			Point = AimPoint;
		}

		// Lets open fire
		if (GameUtils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) // Autoscope
		{
			pCmd->buttons |= IN_ATTACK2;
		}
		else
		{
			if ((Menu::Window.RageBotTab.AccuracyHitchance.GetValue() * 1.5 <= hitchance(pLocal, pWeapon)) || Menu::Window.RageBotTab.AccuracyHitchance.GetValue() == 0 || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(pLocal, Point, pCmd, bSendPacket))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{
						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (IsAbleToShoot(pLocal) && pCmd->buttons & IN_ATTACK)
			Globals::Shots += 1;

		// Stop and Crouch
		if (TargetID >= 0 && pTarget)
		{
			if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
			{
				pCmd->forwardmove = 0.f;
				pCmd->sidemove = 0.f;
				pCmd->buttons |= IN_DUCK;
			}
		}
	}

	// Auto Pistol
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.RageBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			static bool WasFiring = false;
			WasFiring = !WasFiring;
			
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}
}

bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++) //GetHighestEntityIndex()
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	// Target selection
	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetHealth()
{
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

	// Get the hitboxes to scan
#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int iSmart = Menu::Window.RageBotTab.AccuracySmart.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();

	if (iSmart > 0 && pLocal->GetShotsFired() + 1 > iSmart)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
	}
	else
	{
		if (HitScanMode == 0)
		{
			// No Hitscan, just a single hitbox
			switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
			{
			case 0:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				break;
			case 1:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				break;
			case 2:
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				break;
			case 3:
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				break;
			case 4:
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				break;
			}
		}
		else
		{
			switch (HitScanMode)
			{
			case 1:
				// Low
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				break;
			case 2:
				// Normal
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
				break;
			case 3:
				// High
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
			case 4:
				// Extreme
				HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
				HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
				HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
				HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
				HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
			}
		}
	}
#pragma endregion Get the list of shit to scan

	// check hits
	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				c = Color(0, 255, 0, 255);
				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{
					return HitBoxID;
				}
			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::PositionAdjustment(CUserCmd* pCmd)
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();

	if (!pLocal)
		return;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (!pWeapon)
		return;

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	float flSimTime = pLocal->GetSimulationTime();
	float flOldAnimTime = pLocal->GetAnimTime();

	int iTargetTickDiff = (int)(0.5f + (flSimTime - flOldAnimTime) / Interfaces::Globals->interval_per_tick);

	int result = (int)floorf(TIME_TO_TICKS(cl_interp)) + (int)floorf(TIME_TO_TICKS(pLocal->GetSimulationTime()));

	if ((result - pCmd->tick_count) >= -50)
	{
		pCmd->tick_count = result;
	}
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	bool ReturnValue = false;
	// Get the full angles
	if (point.Length() == 0) return ReturnValue;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	}


	IsLocked = true;
	//-----------------------------------------------

	// Aim Step Calcs
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; // Don't just use the viewangs because you need to consider aa

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}

	// Silent Aim
	if (Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		pCmd->viewangles = angles;
	}

	// Normal Aim
	if (!Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		Interfaces::Engine->SetViewAngles(angles);
	}

	// pSilent Aim 
	Vector Oldview = pCmd->viewangles;

	if (Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		static int ChokedPackets = -1;
		ChokedPackets++;

		if (ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles = angles;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = Oldview;
			ChokedPackets = -1;
			ReturnValue = false;
		}
		
		//pCmd->viewangles.z = 0;
	}

	return ReturnValue;
}

namespace AntiAims // CanOpenFire checks for fake anti aims?
{
	// Pitches

	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up)
		{
			pCmd->viewangles.x = 45;
			up = !up;
		}
		else
		{
			pCmd->viewangles.x = 89;
			up = !up;
		}
	}

	void FakePitch(CUserCmd *pCmd, bool &bSendPacket)
	{	
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = 89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 51;
			ChokedPackets = -1;
		}
	}

	void StaticJitter(CUserCmd *pCmd)
	{
		static bool down = true;
		if (down)
		{
			pCmd->viewangles.x = 179.0f;
			down = !down;
		}
		else
		{
			pCmd->viewangles.x = 89.0f;
			down = !down;
		}
	}

	// Yaws

	void FastSpin(CUserCmd *pCmd)
	{
		static int y2 = -179;
		int spinBotSpeedFast = 100;

		y2 += spinBotSpeedFast;

		if (y2 >= 179)
			y2 = -179;

		pCmd->viewangles.y = y2;
	}

	void FakeEdge(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();
		
		Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		CTraceFilter filter;
		filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		for (int y = 0; y < 360; y++)
		{
			Vector qTmp(10.0f, pCmd->viewangles.y, 0.0f);
			qTmp.y += y;

			if (qTmp.y > 180.0)
				qTmp.y -= 360.0;
			else if (qTmp.y < -180.0)
				qTmp.y += 360.0;

			GameUtils::NormaliseViewAngle(qTmp);

			Vector vForward;

			VectorAngles(qTmp, vForward);

			float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
			vForward *= fLength;

			trace_t tr;

			Vector vTraceEnd = vEyePos + vForward;

			Ray_t ray;

			ray.Init(vEyePos, vTraceEnd);
			Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector angles;

				Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

				VectorAngles(vNegative, angles);

				GameUtils::NormaliseViewAngle(angles);

				qTmp.y = angles.y;

				GameUtils::NormaliseViewAngle(qTmp);

				trace_t trLeft, trRight;

				Vector vLeft, vRight;
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

				vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
				vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

				vTraceEnd = vEyePos + vLeft;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

				vTraceEnd = vEyePos + vRight;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);

				if ((trLeft.fraction == 1.0f) && (trRight.fraction != 1.0f))
					qTmp.y -= 90.f;
				else if ((trLeft.fraction != 1.0f) && (trRight.fraction == 1.0f))
					qTmp.y += 90.f;

				if (qTmp.y > 180.0)
					qTmp.y -= 360.0;
				else if (qTmp.y < -180.0)
					qTmp.y += 360.0;

				pCmd->viewangles.y = qTmp.y;

				int offset = Menu::Window.RageBotTab.AntiAimOffset.GetValue();

				static int ChokedPackets = -1;
				ChokedPackets++;
				if (ChokedPackets < 1)
				{
					bSendPacket = false; // +=180?
				}
				else
				{
					bSendPacket = true;
					pCmd->viewangles.y -= offset;
					ChokedPackets = -1;
				}
				return;
			}
		}
		pCmd->viewangles.y += 360.0f;
	}
	
	void BackJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		// Small chance of starting fowards
		if (random < 98)
			// Look backwards
			pCmd->viewangles.y -= 180;

		// Some gitter
		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void FakeSideways(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}

	void Jitter(CUserCmd *pCmd)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 90;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 90;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 180;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 180;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}
	}

	void FakeStatic(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			static int y2 = -179;
			int spinBotSpeedFast = 360.0f / 1.618033988749895f;;

			y2 += spinBotSpeedFast;

			if (y2 >= 179)
				y2 = -179;

			pCmd->viewangles.y = y2;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}

	void TJitter(CUserCmd *pCmd)
	{
		static bool Turbo = true;
		if (Turbo)
		{
			pCmd->viewangles.y -= 90;
			Turbo = !Turbo;
		}
		else
		{
			pCmd->viewangles.y += 90;
			Turbo = !Turbo;
		}
	}

	void TFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y = -90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = 90;
			ChokedPackets = -1;
		}
	}

	void FakeJitter(CUserCmd* pCmd, bool &bSendPacket)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 135;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y += 225;
		}

		static int iChoked = -1;
		iChoked++;
		if (iChoked < 1)
		{
			bSendPacket = false;
			if (jitterangle <= 1)
			{
				pCmd->viewangles.y += 45;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				pCmd->viewangles.y -= 45;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else
		{
			bSendPacket = true;
			iChoked = -1;
		}
	}

	void Static(CUserCmd *pCmd)
	{
		static bool aa1 = false;
		aa1 = !aa1;
		if (aa1)
		{
			static bool turbo = false;
			turbo = !turbo;
			if (turbo)
			{
				pCmd->viewangles.y -= 90;
			}
			else
			{
				pCmd->viewangles.y += 90;
			}
		}
		else
		{
			pCmd->viewangles.y -= 180;
		}
	}

	void fakelowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 90.00f;
			bSendPacket = true;
		}
	}

	void AimAtTarget(CUserCmd *pCmd)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

		if (!pLocal || !pWeapon)
			return;

		Vector eye_position = pLocal->GetEyePosition();

		float best_dist = pWeapon->GetCSWpnData()->flRange;

		IClientEntity* target = Interfaces::EntList->GetClientEntity(Globals::TargetID);

		if (target == NULL)
			return;

		if (target == pLocal)
			return;

		if ((target->GetTeamNum() == pLocal->GetTeamNum()) || target->IsDormant() || !target->IsAlive() || target->GetHealth() <= 0)
			return;

		Vector target_position = target->GetEyePosition();

		float temp_dist = eye_position.DistTo(target_position);

		if (best_dist > temp_dist)
		{
			best_dist = temp_dist;
			CalcAngle(eye_position, target_position, pCmd->viewangles);
		}
	}

	void EdgeDetect(CUserCmd* pCmd, bool &bSendPacket)
	{
		//Ray_t ray;
		//trace_t tr;

		IClientEntity* pLocal = hackManager.pLocal();

		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;

		bool bEdge = false;

		Vector angle;
		Vector eyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		for (float i = 0; i < 360; i++)
		{
			Vector vecDummy(10.f, pCmd->viewangles.y, 0.f);
			vecDummy.y += i;

			Vector forward = vecDummy.Forward();

			//vecDummy.NormalizeInPlace();

			float flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
			forward *= flLength;

			Ray_t ray;
			CGameTrace tr;

			ray.Init(eyePos, (eyePos + forward));
			Interfaces::Trace->EdgeTraceRay(ray, traceFilter, tr, true);

			if (tr.fraction != 1.0f)
			{
				Vector negate = tr.plane.normal;
				negate *= -1;

				Vector vecAng = negate.Angle();

				vecDummy.y = vecAng.y;

				//vecDummy.NormalizeInPlace();
				trace_t leftTrace, rightTrace;

				Vector left = (vecDummy + Vector(0, 45, 0)).Forward(); // or 45
				Vector right = (vecDummy - Vector(0, 45, 0)).Forward();

				left *= (flLength * cosf(rad(30)) * 2); //left *= (len * cosf(rad(30)) * 2);
				right *= (flLength * cosf(rad(30)) * 2); // right *= (len * cosf(rad(30)) * 2);

				ray.Init(eyePos, (eyePos + left));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, leftTrace, true);

				ray.Init(eyePos, (eyePos + right));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, rightTrace, true);

				if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				{
					vecDummy.y -= 45; // left
				}
				else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				{
					vecDummy.y += 45; // right     
				}

				angle.y = vecDummy.y;
				angle.y += 360;
				bEdge = true;
			}
		}

		if (bEdge)
		{
			static bool turbo = true;

			switch (Menu::Window.RageBotTab.AntiAimEdge.GetIndex())
			{
			case 0:
				// Nothing
				break;
			case 1:
				// Regular
				pCmd->viewangles.y = angle.y;
				break;
			}
		}
	}
}

// AntiAim
void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket) // pCmd->viewangles.y = 0xFFFFF INT_MAX or idk
{
	IClientEntity* pLocal = hackManager.pLocal();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;
	
	// If the aimbot is doing something don't do anything
	if ((IsAimStepping || pCmd->buttons & IN_ATTACK) && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
		return;

	// Weapon shit
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		if (!GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (Menu::Window.RageBotTab.AntiAimKnife.GetState())
			{
				if (!CanOpenFire() || pCmd->buttons & IN_ATTACK2)
					return;
			}
			else
			{
				return;
			}
		}
	}

	if (Menu::Window.RageBotTab.AntiAimTarget.GetState())
	{
		AntiAims::AimAtTarget(pCmd);
	}

	// Don't do antiaim
	// if (DoExit) return;

	// Anti-Aim Pitch
	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex()) // Magic pitch is 69.69?
	{
	case 0:
		// No Pitch AA
		break;
	case 1:
		// Down
		//AntiAims::StaticPitch(pCmd, false);
		pCmd->viewangles.x = 89.0f;
		break;
	case 2:
		// Half Down
		pCmd->viewangles.x = 51.f;
		break;
	case 3:
		// SMAC / Casual safe
		AntiAims::JitterPitch(pCmd);
		break;
	case 4:
		// Jitter
		pCmd->viewangles.x = 179.0f;
		break;
	case 5:
		// Fake Pitch
		pCmd->viewangles.x = -181.f;
		break;
	case 6:
		// Static Down
		pCmd->viewangles.x = 1800089.0f;
		break;
	case 7:
		// Static Jitter
		pCmd->viewangles.y = -1800089.0f;
		break;
	}

	//Anti-Aim Yaw
	switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
	{
	case 0:
		// No Yaw AA
		break;
	case 1:
		// Fake Inverse
		AntiAims::FakeEdge(pCmd, bSendPacket);
		break;
	case 2:
		// Fake Sideways
		AntiAims::FakeSideways(pCmd, bSendPacket);
		break;
	case 3:
		// Fake Static
		AntiAims::FakeStatic(pCmd, bSendPacket);
		break;
	case 4:
		// Fake Inverse
		AntiAims::TFake(pCmd, bSendPacket);
		break;
	case 5:
		// Fake Jitter
		AntiAims::FakeJitter(pCmd, bSendPacket);
		break;
	case 6:
		// Jitter
		AntiAims::Static(pCmd);
		break;
	case 7:
		// T Jitter
		AntiAims::TJitter(pCmd);
		break;
	case 8:
		// Back Jitter
		AntiAims::BackJitter(pCmd);
		break; 
	case 9:
		// T Inverse
		pCmd->viewangles.y -= 180;
		break;
	case 10:
		// T Inverse
		AntiAims::fakelowerbody(pCmd, bSendPacket);
		break;
	}

	// Edge Anti Aim
	//AntiAims::EdgeDetect(pCmd, bSendPacket); this is broken it seems ill just remove it cuz what if it causes some crashes

	// Angle offset
	pCmd->viewangles.y += Menu::Window.RageBotTab.AntiAimOffset.GetValue();
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class zbmpkum {
public:
	bool galosbnkl;
	string qkzovcajczgvg;
	int jxujojlbr;
	double mqybenjsr;
	bool gssko;
	zbmpkum();
	int lioxgfmqpcsi(int bbjvkalg, bool chytjo, string pvycdigrazefdv);
	string rwxfrpeyjqmss(double funusnm, string wjgsz, int bcjzcktabowjco, int hewzo, double agfnaskhkrf);
	double lcbauswrcjs(int yehmeguofkbvh, string govhatk, string euodzlpcvy, double clrjxhnb, int llddpx, bool teoqdddaecl);
	int wztxieqlmmlbxelqxhax(string hymcfnanrqfoenb);
	double ckdraeliiprc(bool mefonoiqm, bool eiyyqnpf, double kfupeljypm, string zepxkiwl, string fukjghvd, int hlyrtkmtofst, bool lpklw, int fgjwymhqyzoyne, string whnhou, int ranmw);
	string rwqqtbswedianrbmvgq(double bfxjhcmurojr, bool ixqzhzjl);

protected:
	double bsjpyy;
	int fwsapibkxmojxu;

	void mfivtfwwwam(int hxokvidycc, bool pumiqvhyq);
	int gchvaohiuipphncpigajmn(double dlzpaqhma, int pdumwyy);
	double lgmjotskwbabb(double nerqaecvxow, string subvxtilue, double uvhdqisszqht, string rhsjodzxdkdepq, double acghj, int urjyjmaczmyxr, string wobuz, double sjmxmdyj, double yikscoa, string affgn);

private:
	string tdwrlhfwwrtzqcu;

	void lhoenwtrddty();
	int xgruuyhdgfzpjyyy(bool qjnca, int vicgfekq, bool sbvictmcwmc, string frenajnpb, int xnufmtvmr, bool wheoqs, int edjuiy);
	string gzympcmgdljprjuhlqmkkicm(string lohnfu, bool lycvbkwrhhowh, string cebzyibh, double hgkanvuajfzbr, int raaablv, double ovibowmuvexcgh, string gemftuib, bool tjykvqhguorjp, string tcxdracl);
	void npzukbsxokdwenmapfrwu(bool tvkyv, string cvqottugpqr, bool nrsauxjvcoquhp, double keotdend, bool icuxdcrveuy, int upmvmxljascl, string wossjvmtv, bool crquytzzulz);
	int uxtwodmcxntih(bool tmbbtuxnq, double rhjhq, bool eqgpzx, double uwwsx);
	bool tdcayjjnreorkrhcosymac(bool lzmoibkcrxn, double pupjvqnsgxtl, int iggebltypnvec);
	bool eqgsqvfqfjhawr(bool drzjrmzingdzjhj, int kmmvpnl, int srribcjgqcd, double pqxalplm, string rdndjroheeslrj, int ckxzohhdeu, int rmmyg, double fyyaockatmwm, int smqwvassuz, int rcjiylodmkubv);

};


void zbmpkum::lhoenwtrddty() {
	string mpmotrfxyaz = "gocyqihqlt";
	int unlduduwbplzgv = 1927;
	double ljytbhbqtvrocq = 24511;
	double wnskieb = 44153;
	int nqqgntt = 1809;
	double mjxkxgznecyuz = 43400;
	bool jxntgamtj = true;
	bool rfoxa = false;
	if (1809 != 1809) {
		int sftpybyva;
		for (sftpybyva = 22; sftpybyva > 0; sftpybyva--) {
			continue;
		}
	}

}

int zbmpkum::xgruuyhdgfzpjyyy(bool qjnca, int vicgfekq, bool sbvictmcwmc, string frenajnpb, int xnufmtvmr, bool wheoqs, int edjuiy) {
	return 40615;
}

string zbmpkum::gzympcmgdljprjuhlqmkkicm(string lohnfu, bool lycvbkwrhhowh, string cebzyibh, double hgkanvuajfzbr, int raaablv, double ovibowmuvexcgh, string gemftuib, bool tjykvqhguorjp, string tcxdracl) {
	double pbvyvpz = 14999;
	double aplohmiioiqokf = 20010;
	int vynzcujnkpf = 4670;
	double koiqp = 41399;
	int zaeanug = 3487;
	double kovquebiqsdep = 26380;
	int tjevxzrg = 2668;
	if (4670 != 4670) {
		int lsffo;
		for (lsffo = 88; lsffo > 0; lsffo--) {
			continue;
		}
	}
	if (20010 == 20010) {
		int iyp;
		for (iyp = 50; iyp > 0; iyp--) {
			continue;
		}
	}
	if (41399 == 41399) {
		int fe;
		for (fe = 92; fe > 0; fe--) {
			continue;
		}
	}
	return string("qirhtwzeae");
}

void zbmpkum::npzukbsxokdwenmapfrwu(bool tvkyv, string cvqottugpqr, bool nrsauxjvcoquhp, double keotdend, bool icuxdcrveuy, int upmvmxljascl, string wossjvmtv, bool crquytzzulz) {
	double gfnntprkwnybly = 6007;
	int rztgcennizxvl = 741;
	string moaapt = "yxtcqmkrguoqgfccxgjledkxqchdwlmasfduhiqcmmnelcycixjidnoaqvvjbqkgghebzzudyanvnxdmcckllpzhmbfjrd";
	bool krlfopuollf = true;
	double eqjcxo = 79448;
	string dnmtewtlstli = "yldplxvmnraytmljfjwijsedmjfqwuiejuyanjrghgmykssygbguiyfqfhmcd";
	int qirdhqowbfqpbo = 4836;
	bool lmumafappai = false;
	int jtkxhpwoauko = 220;
	if (false != false) {
		int iuwhcezbit;
		for (iuwhcezbit = 45; iuwhcezbit > 0; iuwhcezbit--) {
			continue;
		}
	}
	if (string("yldplxvmnraytmljfjwijsedmjfqwuiejuyanjrghgmykssygbguiyfqfhmcd") != string("yldplxvmnraytmljfjwijsedmjfqwuiejuyanjrghgmykssygbguiyfqfhmcd")) {
		int lcm;
		for (lcm = 55; lcm > 0; lcm--) {
			continue;
		}
	}
	if (79448 == 79448) {
		int hrdxpkhgm;
		for (hrdxpkhgm = 23; hrdxpkhgm > 0; hrdxpkhgm--) {
			continue;
		}
	}
	if (true != true) {
		int zdpjgwy;
		for (zdpjgwy = 31; zdpjgwy > 0; zdpjgwy--) {
			continue;
		}
	}
	if (220 != 220) {
		int ivpuz;
		for (ivpuz = 99; ivpuz > 0; ivpuz--) {
			continue;
		}
	}

}

int zbmpkum::uxtwodmcxntih(bool tmbbtuxnq, double rhjhq, bool eqgpzx, double uwwsx) {
	double egiwcmzy = 6159;
	return 26493;
}

bool zbmpkum::tdcayjjnreorkrhcosymac(bool lzmoibkcrxn, double pupjvqnsgxtl, int iggebltypnvec) {
	bool perllljxwin = true;
	int nefeoq = 2308;
	if (2308 != 2308) {
		int oiltb;
		for (oiltb = 33; oiltb > 0; oiltb--) {
			continue;
		}
	}
	if (true != true) {
		int cgsmzgzxmv;
		for (cgsmzgzxmv = 21; cgsmzgzxmv > 0; cgsmzgzxmv--) {
			continue;
		}
	}
	if (2308 == 2308) {
		int vwjjaiasuv;
		for (vwjjaiasuv = 94; vwjjaiasuv > 0; vwjjaiasuv--) {
			continue;
		}
	}
	return false;
}

bool zbmpkum::eqgsqvfqfjhawr(bool drzjrmzingdzjhj, int kmmvpnl, int srribcjgqcd, double pqxalplm, string rdndjroheeslrj, int ckxzohhdeu, int rmmyg, double fyyaockatmwm, int smqwvassuz, int rcjiylodmkubv) {
	double bwggazqsmdewwe = 10731;
	double ahwjeyeqcvumr = 23742;
	string onweg = "lbjllgtkwuwbbbwcnzyqkygxuzkrjirfhpaw";
	string lfkbynlmi = "geshcfnfyincjq";
	int qrdtpfwktyhp = 6378;
	int yybcbfvf = 1090;
	bool fenftim = true;
	double efhafswzbc = 39985;
	bool lfjowzakfnfy = false;
	double efqfbyk = 59048;
	if (1090 == 1090) {
		int thoxq;
		for (thoxq = 80; thoxq > 0; thoxq--) {
			continue;
		}
	}
	if (23742 != 23742) {
		int ntp;
		for (ntp = 54; ntp > 0; ntp--) {
			continue;
		}
	}
	if (true == true) {
		int qzjfjo;
		for (qzjfjo = 14; qzjfjo > 0; qzjfjo--) {
			continue;
		}
	}
	if (10731 != 10731) {
		int etqibnjc;
		for (etqibnjc = 13; etqibnjc > 0; etqibnjc--) {
			continue;
		}
	}
	return false;
}

void zbmpkum::mfivtfwwwam(int hxokvidycc, bool pumiqvhyq) {
	bool ohngstbvki = true;
	int oguqff = 2710;
	double tvuwysvuqqlisc = 27281;
	int lftewbbkjyyradf = 875;
	int qzdkqmiqrzbvexe = 1574;
	bool eblfmlj = true;
	int eftnymik = 1197;
	if (27281 == 27281) {
		int bn;
		for (bn = 61; bn > 0; bn--) {
			continue;
		}
	}
	if (1574 == 1574) {
		int whfwlpqle;
		for (whfwlpqle = 100; whfwlpqle > 0; whfwlpqle--) {
			continue;
		}
	}
	if (875 != 875) {
		int fvytbxdg;
		for (fvytbxdg = 82; fvytbxdg > 0; fvytbxdg--) {
			continue;
		}
	}
	if (875 != 875) {
		int cfmkhlgfze;
		for (cfmkhlgfze = 69; cfmkhlgfze > 0; cfmkhlgfze--) {
			continue;
		}
	}

}

int zbmpkum::gchvaohiuipphncpigajmn(double dlzpaqhma, int pdumwyy) {
	double geyszd = 3933;
	bool uqsxe = true;
	double hbiicbrg = 67422;
	double nynqghqpf = 32706;
	string redvvyp = "pkksyhqifcojkhxeogfvaitetjca";
	string aucvgdjwsrsanbg = "owdbdfbzzpocmphklrzbqceraebpasuecqaphq";
	if (string("owdbdfbzzpocmphklrzbqceraebpasuecqaphq") != string("owdbdfbzzpocmphklrzbqceraebpasuecqaphq")) {
		int yv;
		for (yv = 57; yv > 0; yv--) {
			continue;
		}
	}
	if (string("pkksyhqifcojkhxeogfvaitetjca") != string("pkksyhqifcojkhxeogfvaitetjca")) {
		int ou;
		for (ou = 56; ou > 0; ou--) {
			continue;
		}
	}
	if (67422 == 67422) {
		int jjwtp;
		for (jjwtp = 40; jjwtp > 0; jjwtp--) {
			continue;
		}
	}
	if (3933 != 3933) {
		int yeoniixmg;
		for (yeoniixmg = 65; yeoniixmg > 0; yeoniixmg--) {
			continue;
		}
	}
	return 42183;
}

double zbmpkum::lgmjotskwbabb(double nerqaecvxow, string subvxtilue, double uvhdqisszqht, string rhsjodzxdkdepq, double acghj, int urjyjmaczmyxr, string wobuz, double sjmxmdyj, double yikscoa, string affgn) {
	double npmhudf = 42216;
	int quvjuvgtkeca = 1546;
	double wzouoveqs = 28235;
	double zlqdxkxukeuaxjs = 7037;
	int jdgsfgreodxnvc = 6265;
	if (1546 == 1546) {
		int ezb;
		for (ezb = 26; ezb > 0; ezb--) {
			continue;
		}
	}
	if (28235 != 28235) {
		int pftbbbbxdh;
		for (pftbbbbxdh = 93; pftbbbbxdh > 0; pftbbbbxdh--) {
			continue;
		}
	}
	return 43454;
}

int zbmpkum::lioxgfmqpcsi(int bbjvkalg, bool chytjo, string pvycdigrazefdv) {
	bool ngzhpp = false;
	double lzsixqkzgdtkb = 44792;
	double tjxlnimzarhkzp = 30385;
	int ybphdpd = 4040;
	bool fltncxo = true;
	double rntaivrys = 8355;
	int nudnwlfzoz = 262;
	bool wykcqcslkggkodl = true;
	bool hqjbzsmrkgmpu = true;
	int ufhtcl = 5725;
	if (30385 == 30385) {
		int jqvbladhcr;
		for (jqvbladhcr = 10; jqvbladhcr > 0; jqvbladhcr--) {
			continue;
		}
	}
	if (true == true) {
		int nhrswoq;
		for (nhrswoq = 100; nhrswoq > 0; nhrswoq--) {
			continue;
		}
	}
	return 92463;
}

string zbmpkum::rwxfrpeyjqmss(double funusnm, string wjgsz, int bcjzcktabowjco, int hewzo, double agfnaskhkrf) {
	bool zwmbygd = false;
	if (false == false) {
		int buzslmnylq;
		for (buzslmnylq = 54; buzslmnylq > 0; buzslmnylq--) {
			continue;
		}
	}
	return string("cmjesrrmvkwgt");
}

double zbmpkum::lcbauswrcjs(int yehmeguofkbvh, string govhatk, string euodzlpcvy, double clrjxhnb, int llddpx, bool teoqdddaecl) {
	int dizhbp = 1532;
	string jtpolnftxxjtx = "eggeykqqrqckljnjcfusikfjjkwsyrzlvxkzcetkcrqlzdvieqhbezbbcarggqhx";
	int qceksthzg = 2379;
	bool wbqwlcqlbw = false;
	bool uikbgpcmtlhtild = true;
	int qsftnefnx = 7053;
	if (2379 == 2379) {
		int gxln;
		for (gxln = 3; gxln > 0; gxln--) {
			continue;
		}
	}
	if (string("eggeykqqrqckljnjcfusikfjjkwsyrzlvxkzcetkcrqlzdvieqhbezbbcarggqhx") == string("eggeykqqrqckljnjcfusikfjjkwsyrzlvxkzcetkcrqlzdvieqhbezbbcarggqhx")) {
		int ugcquwkin;
		for (ugcquwkin = 65; ugcquwkin > 0; ugcquwkin--) {
			continue;
		}
	}
	if (false != false) {
		int tm;
		for (tm = 66; tm > 0; tm--) {
			continue;
		}
	}
	if (false != false) {
		int mztel;
		for (mztel = 37; mztel > 0; mztel--) {
			continue;
		}
	}
	if (7053 == 7053) {
		int yjwgiwnlt;
		for (yjwgiwnlt = 1; yjwgiwnlt > 0; yjwgiwnlt--) {
			continue;
		}
	}
	return 48327;
}

int zbmpkum::wztxieqlmmlbxelqxhax(string hymcfnanrqfoenb) {
	double jlxss = 39729;
	double gmsnqzhsujkjqzw = 284;
	bool ykjelrisgkt = true;
	string oxlfxk = "gkvoogogjjnzccckme";
	double olcxhxfdiginljs = 8318;
	int aihyyepcigpvy = 7390;
	int aekglawwbikpqi = 4607;
	bool asgqunmtic = true;
	bool kaywh = true;
	string nryqrhvorrjt = "pzzwdlmhjhxhmu";
	if (284 == 284) {
		int ypgbov;
		for (ypgbov = 71; ypgbov > 0; ypgbov--) {
			continue;
		}
	}
	return 82564;
}

double zbmpkum::ckdraeliiprc(bool mefonoiqm, bool eiyyqnpf, double kfupeljypm, string zepxkiwl, string fukjghvd, int hlyrtkmtofst, bool lpklw, int fgjwymhqyzoyne, string whnhou, int ranmw) {
	double btnufkeupfboep = 79892;
	int rcwzzibirckux = 3327;
	double mmfsyoixilpm = 45882;
	int majimgpeda = 6830;
	string kxcrlgyss = "blyrnsagfhkuxdpjgbgthmecydjktsavuyzapdclktllwibyayaxikzjxfmuxghjjqfdxmxuvi";
	double xxeappyxrybzg = 46731;
	double aavwbgjdyzb = 4821;
	int lxjpvw = 3164;
	double sbutizayyz = 9051;
	double lhnwfjpoxny = 8792;
	if (45882 == 45882) {
		int xvrjxdzkyw;
		for (xvrjxdzkyw = 4; xvrjxdzkyw > 0; xvrjxdzkyw--) {
			continue;
		}
	}
	if (45882 == 45882) {
		int kpzmkqv;
		for (kpzmkqv = 86; kpzmkqv > 0; kpzmkqv--) {
			continue;
		}
	}
	if (4821 == 4821) {
		int fgadvlpdx;
		for (fgadvlpdx = 9; fgadvlpdx > 0; fgadvlpdx--) {
			continue;
		}
	}
	return 89727;
}

string zbmpkum::rwqqtbswedianrbmvgq(double bfxjhcmurojr, bool ixqzhzjl) {
	int gsjhtimcnoparzi = 433;
	if (433 != 433) {
		int uwhgqf;
		for (uwhgqf = 44; uwhgqf > 0; uwhgqf--) {
			continue;
		}
	}
	if (433 != 433) {
		int asdvw;
		for (asdvw = 16; asdvw > 0; asdvw--) {
			continue;
		}
	}
	if (433 != 433) {
		int rsaffghti;
		for (rsaffghti = 63; rsaffghti > 0; rsaffghti--) {
			continue;
		}
	}
	if (433 == 433) {
		int aggjk;
		for (aggjk = 23; aggjk > 0; aggjk--) {
			continue;
		}
	}
	if (433 == 433) {
		int euoywwtmrb;
		for (euoywwtmrb = 47; euoywwtmrb > 0; euoywwtmrb--) {
			continue;
		}
	}
	return string("myqlujetlawumiutzcv");
}

zbmpkum::zbmpkum() {
	this->lioxgfmqpcsi(7970, true, string("ocuiwwlgvqdbzsgugqk"));
	this->rwxfrpeyjqmss(32548, string("nmuuhztbyzjp"), 514, 5422, 12128);
	this->lcbauswrcjs(1562, string("evkbltqpezlsvghigosiccjlaznqxiyvfcvvlh"), string("jowtgfwltvsfnkrbitujphsbfrpgzmmqvepfsorjcfvwtikh"), 30676, 2452, false);
	this->wztxieqlmmlbxelqxhax(string("obwzsbzfbjkomalwkmzdxaarwsdypagrnimwjdgbocaokelhfggsyjanbmtehydkfvsgkszgtkveg"));
	this->ckdraeliiprc(true, false, 8749, string("eialsoyzkisgxkkbdbgexryltsfiogseahahqzoloyslrpirgarrxcuxzlarktjgvrhjxoenzydbdarqjppaqeqwfawdtnpltms"), string("rwmgurriouiseriaehkyjuugklphuzzvxaksptsxajijzcyvp"), 4060, false, 6993, string("aigabkjprbnstfxobvkskwjuhsfqigtyehzddzudhadnxiknlfyctvacsvotkfesdlsnlaaoufmhfedhcg"), 3523);
	this->rwqqtbswedianrbmvgq(781, true);
	this->mfivtfwwwam(4262, false);
	this->gchvaohiuipphncpigajmn(10033, 2244);
	this->lgmjotskwbabb(61340, string("pufepftymysrbaducwkpmvcmrxaaggkxlhhjatlwsjtkfwsqavlaxodypgsaswroopdcggkphagwsyrcaygtmwlxustbumx"), 43116, string("cghvjikzamzdtyeoxgjbwhylxciklrdxnssgtgb"), 57217, 1009, string("kekbzpnlzacmghdticxyvfhdqvwqttzzmyogufsdgxlzqzvipsechfrtobcdatflvzqzittjqjjfmyudacrggrdvdnoacb"), 2262, 9989, string("umjkiybvpltwonnwqgspck"));
	this->lhoenwtrddty();
	this->xgruuyhdgfzpjyyy(false, 2483, true, string("jhvsqyoxdeqogcpslkklueiievql"), 1811, false, 3495);
	this->gzympcmgdljprjuhlqmkkicm(string("ckcjbsowvzpdgzhfyijllzagermvdcbzzjqjdzftjqrmlcqidxnkjajzrhkhxawwxlbdyh"), false, string("ksfhhgbritvkqgoqlausawwlqobzeksvojqbhquzhnzrrssbvikwhmrvxruzqcecppqnxg"), 4447, 2961, 18349, string("gxolvhqiexidueg"), false, string("bcdjbegtdybfpnxiagkszgkvbjavkllwokhjtyrrfzymxsulqqgmbywblxqfurcjuxvcogklawqzcuwlpfclurmijywpk"));
	this->npzukbsxokdwenmapfrwu(true, string("yoacslayjzvotiygyklzthgujcftgohnqtnkwkhzknetxbfbakhgmrkhnpwigjsocrtijswehlfplnmpqjmueblgowklppj"), false, 43080, true, 597, string("vqjwynzamhvycdihqvlooiphoxldwxmkekoxebmltmujrrhmsvxkyotr"), true);
	this->uxtwodmcxntih(true, 104, true, 21699);
	this->tdcayjjnreorkrhcosymac(true, 16073, 1794);
	this->eqgsqvfqfjhawr(true, 2857, 3585, 56682, string("khkvqorracbdslgvtkazcogleehqrbupvzlgklonlaztnvvyhopdsokeienecxhlfthvqxmxzcebzmgeogvpenaf"), 3205, 3509, 67817, 5381, 330);
}
