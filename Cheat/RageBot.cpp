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

class eaoewxj {
public:
	double xnhtdu;
	int gbszhslpcivc;
	bool yiberlyrbqfs;
	eaoewxj();
	double dlfpmaukkdemcspavb(int ejkguqckxhowuv, bool muagrr, double bgxqlxwjtdhpf, int bvrfgixnh, string zbzowapmqfxf, string wxkvkwyuxjvd, double daeizzocr, bool vlrhoopjpg);

protected:
	string upyqo;
	double zwwuratrosjtyp;
	double rqqbcryfes;
	string eiwlfai;

	int bqsrtiwjlhshmybhnimlqcvpn(double qubvpafpk, int npaudt, string mlfyiahvkzd, string halkqpbevlykh, string ojtwdrtbibcvnps);
	void ffzkpqamlzwwx();
	void chdtstujqu(int bsysnwvgy, int ovlmjbhskaep, bool axfsljqjd, string jwrwszfhckuwqd, int npxedwubxh, bool lgvfcgbn, string labjurwp, double uobnpykrwx, bool lvmpfplp);
	string aggkaaevpmivlucmifkvyb(double jikwl, int fsgwaerdiq, int upzsm, string mtaoxd, int ynsjgupamr, bool arsjqleroohzi, bool fthrdadxr);
	void hhpvfrbzpkemqvko(bool dfjhzvljcpwks, double ibcmjnkt, double eyyuhsjw, string gwdtciltbe, string fxhpubd, string mglmwex);
	string uuszrhstnryzw(string jjcroydovhd, int miczkzmno, int btrqk, bool bthdtjnwurvkwoz, bool chisfcbwjze);
	int ugzvnclpdhwcwhzkwkkzr(int qdlaaqwwgig, int kfirbjbscmqmqgd, bool gucvvwulhxbvgk, int cdmjyckuyqpum, int pffrbretbnccwn);
	double abbguwzyxtd(double pdhltkmkqunziei, string kujihvwzgudrmk, int kbqcemsg);
	int bmmphfxmbasvx(bool ihogbiz, bool lhtbleoresuggdh, int vfkooywlcim, bool lbrxckqeowlelp, double pieejdrysat, double nybobdzcaemsdct, bool zltucw);

private:
	bool iarddnnistssdgn;
	double raweo;
	int vtnpycmeev;

	bool osugpnvuiri(int sfgbnrdi, int yfikcrr, double fvrkarpcpknvl, double ntoxxe, double qsdtmoclng, string bbbfuf);
	double uammyudsmapyifehpttbyhmf(bool xaymhbjmga, bool zzemificbcbz, double ywafqh, bool wccbaihsxbyi, int inckivd, string puwzdunepst, string wkioxnpju);
	string xowwbgvrgmqvsppwr(int qtyyybbfudvyn, bool bvowetu, string eoylawikjdw, bool mncpabrfbfgdr, double nngzr, bool rorjf);
	int yhdjqxpdzeolevcn();
	void rwjiohonmkpq(string grrtkhcicrancmq, double twlrzqbf, double qmijol, bool ndkqternncjhg, string ueqgphierz, bool pfoklfwopqt, string tscbk);
	void rthpmyljoiunjzpuhilutfou(string eutohja, int eediypj);
	bool tnzgdujfvwhvznc(string bqortwgk, string kidgjwpi, string gfvxcpxposcqfz, int drhciadczndsvaq, double mstbr, double zdgfe);
	void hqgaoptbix(int sdajcjfvgsmh, string ankmhozjwo);
	bool ktmwxpnntrgtyb(bool nksmxqwlipce, bool movgvlxythnfv, bool umlbwkuukivk, bool lktfnzbyb, string vxtrsnxo, string xvaxj);

};


bool eaoewxj::osugpnvuiri(int sfgbnrdi, int yfikcrr, double fvrkarpcpknvl, double ntoxxe, double qsdtmoclng, string bbbfuf) {
	int yvecektk = 3365;
	int linuwmiusncwz = 9153;
	double ezjahtcoxxg = 17428;
	if (17428 != 17428) {
		int srgwfry;
		for (srgwfry = 28; srgwfry > 0; srgwfry--) {
			continue;
		}
	}
	if (3365 != 3365) {
		int pl;
		for (pl = 71; pl > 0; pl--) {
			continue;
		}
	}
	return true;
}

double eaoewxj::uammyudsmapyifehpttbyhmf(bool xaymhbjmga, bool zzemificbcbz, double ywafqh, bool wccbaihsxbyi, int inckivd, string puwzdunepst, string wkioxnpju) {
	bool cutwdkelbkrq = true;
	if (true != true) {
		int gdshovfgn;
		for (gdshovfgn = 67; gdshovfgn > 0; gdshovfgn--) {
			continue;
		}
	}
	if (true != true) {
		int xg;
		for (xg = 81; xg > 0; xg--) {
			continue;
		}
	}
	if (true == true) {
		int rxjmuxcwyu;
		for (rxjmuxcwyu = 3; rxjmuxcwyu > 0; rxjmuxcwyu--) {
			continue;
		}
	}
	if (true != true) {
		int zu;
		for (zu = 27; zu > 0; zu--) {
			continue;
		}
	}
	if (true == true) {
		int efopya;
		for (efopya = 37; efopya > 0; efopya--) {
			continue;
		}
	}
	return 50811;
}

string eaoewxj::xowwbgvrgmqvsppwr(int qtyyybbfudvyn, bool bvowetu, string eoylawikjdw, bool mncpabrfbfgdr, double nngzr, bool rorjf) {
	int xbyyhzdqkxtbt = 339;
	string bajslpavctjf = "kuqunzwifcyybkg";
	int hxpggargyg = 1829;
	bool xbveyjldnmje = true;
	int xjqwhlcwwz = 177;
	double sxaednmke = 16364;
	string qumiocptxkilhjc = "garsyebjmlqeosjtcawdqzamxjxdyiijahjibozezhidjfdeaperltxhqhrajudxbblzgmtxbhpejeltpyhcfinpslvwbaenz";
	string nmrzkdjmgoiyi = "lnhhihmxekmcxplzestddpdtwjfwijmktazyecrbdnrkrrqnyfmokoxgohrtuzvbf";
	return string("huwfr");
}

int eaoewxj::yhdjqxpdzeolevcn() {
	bool ccyojkw = false;
	bool nfzxwixgizi = false;
	double kyhfmwxadgxnmo = 8137;
	if (false == false) {
		int wrozlwzjr;
		for (wrozlwzjr = 33; wrozlwzjr > 0; wrozlwzjr--) {
			continue;
		}
	}
	if (false != false) {
		int hairsbw;
		for (hairsbw = 83; hairsbw > 0; hairsbw--) {
			continue;
		}
	}
	return 64658;
}

void eaoewxj::rwjiohonmkpq(string grrtkhcicrancmq, double twlrzqbf, double qmijol, bool ndkqternncjhg, string ueqgphierz, bool pfoklfwopqt, string tscbk) {
	int ivpxshcby = 1376;
	bool lvhgwg = false;
	string bubvavmmqjrruc = "bxqxjgsmnzjhutisgcjaikdifsstsfwpepiutqatrjdzokgrrebznhxbyqeb";
	bool dueujjadboraa = false;
	string okzgcyedmjfc = "sv";
	int xwijqdszs = 4390;
	bool trdwdbjs = true;
	string munwh = "uqcehibhgtdcwjdghsjtkhjietjqjmxpvcdbhmubdzovmankrfjkzrheccbrfzozjxsmgbgiymajahqr";
	int gnlyuzqfajtjdk = 51;
	if (4390 != 4390) {
		int usqwvysnbc;
		for (usqwvysnbc = 9; usqwvysnbc > 0; usqwvysnbc--) {
			continue;
		}
	}
	if (false != false) {
		int ekvnxnyudh;
		for (ekvnxnyudh = 43; ekvnxnyudh > 0; ekvnxnyudh--) {
			continue;
		}
	}
	if (string("uqcehibhgtdcwjdghsjtkhjietjqjmxpvcdbhmubdzovmankrfjkzrheccbrfzozjxsmgbgiymajahqr") != string("uqcehibhgtdcwjdghsjtkhjietjqjmxpvcdbhmubdzovmankrfjkzrheccbrfzozjxsmgbgiymajahqr")) {
		int crtuzua;
		for (crtuzua = 31; crtuzua > 0; crtuzua--) {
			continue;
		}
	}
	if (false == false) {
		int eliwkajj;
		for (eliwkajj = 36; eliwkajj > 0; eliwkajj--) {
			continue;
		}
	}
	if (51 != 51) {
		int mcggxi;
		for (mcggxi = 71; mcggxi > 0; mcggxi--) {
			continue;
		}
	}

}

void eaoewxj::rthpmyljoiunjzpuhilutfou(string eutohja, int eediypj) {
	int vyryx = 1157;
	double czzmsb = 25354;
	string yjrslwiy = "uymexzjgzycvssvkgduukdyylflntx";
	bool zmiioghvqueuaz = true;
	double pzyuhylpofnrxk = 751;
	int lgqxbv = 580;
	double pvsgabtliojjmb = 8414;
	bool ibnnfhhjjwyhrg = true;
	string iozeximwlalph = "bhozboktlobrtwugtvlqqpccinwq";
	bool qzuvpwpefxvns = false;
	if (string("bhozboktlobrtwugtvlqqpccinwq") == string("bhozboktlobrtwugtvlqqpccinwq")) {
		int kay;
		for (kay = 96; kay > 0; kay--) {
			continue;
		}
	}
	if (25354 != 25354) {
		int acwrqiloqy;
		for (acwrqiloqy = 87; acwrqiloqy > 0; acwrqiloqy--) {
			continue;
		}
	}
	if (false != false) {
		int mjfakvzt;
		for (mjfakvzt = 69; mjfakvzt > 0; mjfakvzt--) {
			continue;
		}
	}
	if (string("bhozboktlobrtwugtvlqqpccinwq") != string("bhozboktlobrtwugtvlqqpccinwq")) {
		int dnxxxjno;
		for (dnxxxjno = 72; dnxxxjno > 0; dnxxxjno--) {
			continue;
		}
	}

}

bool eaoewxj::tnzgdujfvwhvznc(string bqortwgk, string kidgjwpi, string gfvxcpxposcqfz, int drhciadczndsvaq, double mstbr, double zdgfe) {
	return true;
}

void eaoewxj::hqgaoptbix(int sdajcjfvgsmh, string ankmhozjwo) {

}

bool eaoewxj::ktmwxpnntrgtyb(bool nksmxqwlipce, bool movgvlxythnfv, bool umlbwkuukivk, bool lktfnzbyb, string vxtrsnxo, string xvaxj) {
	string gybsefgiogr = "smtz";
	bool upppijukornefq = true;
	int nhupgcwlsrf = 341;
	string tjvmfjwmzvwj = "evltkwkfrqughmauldttahnwwbztuiptpctzvsfrcxboxirllpyvzmbhfbryoljmsvonqkbhqrdorfxns";
	return true;
}

int eaoewxj::bqsrtiwjlhshmybhnimlqcvpn(double qubvpafpk, int npaudt, string mlfyiahvkzd, string halkqpbevlykh, string ojtwdrtbibcvnps) {
	bool yijny = true;
	string zhzholwtdbjdxcb = "gotbdfpwomberbxwnzgacisbtdwaxbhfivnwirygfdlholtpfqfnirnmywhfvpjfmgniaufvc";
	bool tfwwwxgtqkz = true;
	int fhxyzerrur = 3206;
	if (true != true) {
		int pdlzkfrv;
		for (pdlzkfrv = 29; pdlzkfrv > 0; pdlzkfrv--) {
			continue;
		}
	}
	return 88239;
}

void eaoewxj::ffzkpqamlzwwx() {
	int pucirtsdup = 3152;
	int iccova = 731;
	double ujwgfkhgwo = 70;
	double lfsgkju = 7767;
	string nskqwkcfjbn = "ztdskukhtanxstafewhlrxbojgvvlcwdvbg";
	double kulmtcnvo = 26974;
	bool mjexxqiddez = false;
	double bfxecxqqlmuqgcz = 21130;
	if (3152 == 3152) {
		int pvng;
		for (pvng = 47; pvng > 0; pvng--) {
			continue;
		}
	}
	if (731 == 731) {
		int budnba;
		for (budnba = 41; budnba > 0; budnba--) {
			continue;
		}
	}
	if (string("ztdskukhtanxstafewhlrxbojgvvlcwdvbg") == string("ztdskukhtanxstafewhlrxbojgvvlcwdvbg")) {
		int ughjaz;
		for (ughjaz = 0; ughjaz > 0; ughjaz--) {
			continue;
		}
	}
	if (7767 != 7767) {
		int ixu;
		for (ixu = 58; ixu > 0; ixu--) {
			continue;
		}
	}
	if (26974 != 26974) {
		int qllam;
		for (qllam = 34; qllam > 0; qllam--) {
			continue;
		}
	}

}

void eaoewxj::chdtstujqu(int bsysnwvgy, int ovlmjbhskaep, bool axfsljqjd, string jwrwszfhckuwqd, int npxedwubxh, bool lgvfcgbn, string labjurwp, double uobnpykrwx, bool lvmpfplp) {
	int dnitmsrgwyozuu = 97;
	bool rwmakhrqqd = false;
	double afjarbhrwd = 50612;
	bool ronfigivxg = true;
	bool yanbpxrqlmpxa = true;
	int ksrnze = 3076;
	string obpxweqwuaueh = "agyvircvkyj";
	bool qckcozbo = true;
	string yfqcojqzjrtj = "orlznpy";
	if (97 == 97) {
		int ax;
		for (ax = 47; ax > 0; ax--) {
			continue;
		}
	}
	if (false == false) {
		int js;
		for (js = 82; js > 0; js--) {
			continue;
		}
	}

}

string eaoewxj::aggkaaevpmivlucmifkvyb(double jikwl, int fsgwaerdiq, int upzsm, string mtaoxd, int ynsjgupamr, bool arsjqleroohzi, bool fthrdadxr) {
	bool zvuatbd = true;
	double nnytslbpshs = 22972;
	int hmvlj = 3463;
	double wfyxoulaldsfgt = 132;
	int gzogbgciswcky = 6924;
	int siwqyxsrfut = 4195;
	int awxgyjst = 1948;
	double jzheg = 18495;
	double ggmhkew = 3340;
	if (4195 != 4195) {
		int pzvp;
		for (pzvp = 78; pzvp > 0; pzvp--) {
			continue;
		}
	}
	if (132 != 132) {
		int hbi;
		for (hbi = 28; hbi > 0; hbi--) {
			continue;
		}
	}
	if (18495 == 18495) {
		int tbugpucv;
		for (tbugpucv = 81; tbugpucv > 0; tbugpucv--) {
			continue;
		}
	}
	if (3340 == 3340) {
		int sfvpjdiura;
		for (sfvpjdiura = 61; sfvpjdiura > 0; sfvpjdiura--) {
			continue;
		}
	}
	if (132 != 132) {
		int wtaoysv;
		for (wtaoysv = 65; wtaoysv > 0; wtaoysv--) {
			continue;
		}
	}
	return string("bnyfxaoiql");
}

void eaoewxj::hhpvfrbzpkemqvko(bool dfjhzvljcpwks, double ibcmjnkt, double eyyuhsjw, string gwdtciltbe, string fxhpubd, string mglmwex) {
	bool ecehlkmzql = true;
	int dlatvltkcvpubjg = 921;
	bool fpqboxmonyxyses = true;
	bool bjklcclmxsvibp = false;
	int cwnayzkuoi = 181;
	int frbvnqrgoueded = 4775;
	int ywyllmpdnspj = 4372;
	if (921 != 921) {
		int udbknps;
		for (udbknps = 88; udbknps > 0; udbknps--) {
			continue;
		}
	}
	if (921 == 921) {
		int jefdplr;
		for (jefdplr = 16; jefdplr > 0; jefdplr--) {
			continue;
		}
	}
	if (false == false) {
		int tubkx;
		for (tubkx = 45; tubkx > 0; tubkx--) {
			continue;
		}
	}
	if (921 != 921) {
		int psylawql;
		for (psylawql = 2; psylawql > 0; psylawql--) {
			continue;
		}
	}
	if (4372 == 4372) {
		int pzwleey;
		for (pzwleey = 12; pzwleey > 0; pzwleey--) {
			continue;
		}
	}

}

string eaoewxj::uuszrhstnryzw(string jjcroydovhd, int miczkzmno, int btrqk, bool bthdtjnwurvkwoz, bool chisfcbwjze) {
	bool dosxfbseoisul = true;
	bool qohbqtkexm = true;
	double xruknzkbc = 1453;
	double npsrki = 43783;
	double ozxramgw = 14625;
	bool bikelxaxyhmxjo = true;
	string loscchuevsdpgri = "v";
	string zxrfydleunjxlwq = "catcyfscwzqjkhobnpypwchrhodrapregevnpeldointddqntznzhxypmefys";
	if (14625 != 14625) {
		int xmcvfgvqcm;
		for (xmcvfgvqcm = 56; xmcvfgvqcm > 0; xmcvfgvqcm--) {
			continue;
		}
	}
	if (true == true) {
		int oqjm;
		for (oqjm = 56; oqjm > 0; oqjm--) {
			continue;
		}
	}
	return string("avxkieizekjrcwkqptkq");
}

int eaoewxj::ugzvnclpdhwcwhzkwkkzr(int qdlaaqwwgig, int kfirbjbscmqmqgd, bool gucvvwulhxbvgk, int cdmjyckuyqpum, int pffrbretbnccwn) {
	int cfgcgrbt = 798;
	string jygrglhtixdz = "rambmtzkyiupsbcqokebgvejjpultfrffgxpqkpqyjmenoeeerulljkpqfdmhhsjxkrxdtclherndezraivuw";
	double hnvafbni = 6719;
	if (string("rambmtzkyiupsbcqokebgvejjpultfrffgxpqkpqyjmenoeeerulljkpqfdmhhsjxkrxdtclherndezraivuw") == string("rambmtzkyiupsbcqokebgvejjpultfrffgxpqkpqyjmenoeeerulljkpqfdmhhsjxkrxdtclherndezraivuw")) {
		int unwoxhoe;
		for (unwoxhoe = 73; unwoxhoe > 0; unwoxhoe--) {
			continue;
		}
	}
	if (798 != 798) {
		int obd;
		for (obd = 79; obd > 0; obd--) {
			continue;
		}
	}
	if (798 == 798) {
		int pyaw;
		for (pyaw = 53; pyaw > 0; pyaw--) {
			continue;
		}
	}
	if (6719 != 6719) {
		int dqm;
		for (dqm = 48; dqm > 0; dqm--) {
			continue;
		}
	}
	return 20989;
}

double eaoewxj::abbguwzyxtd(double pdhltkmkqunziei, string kujihvwzgudrmk, int kbqcemsg) {
	string cijgvsl = "ztiftxcqnfiggsqdbtcovohbwubxcczqxgxibfchghtkrmsuwoabnndwdpkswbnfzktrzbxluykimffudufsastonhywwdm";
	int adhydlo = 4628;
	int fvoverxafjag = 550;
	bool krxohe = true;
	int zyuciijgxzto = 3293;
	if (3293 == 3293) {
		int onxqd;
		for (onxqd = 14; onxqd > 0; onxqd--) {
			continue;
		}
	}
	if (4628 == 4628) {
		int bckrtslny;
		for (bckrtslny = 49; bckrtslny > 0; bckrtslny--) {
			continue;
		}
	}
	if (true != true) {
		int km;
		for (km = 80; km > 0; km--) {
			continue;
		}
	}
	if (string("ztiftxcqnfiggsqdbtcovohbwubxcczqxgxibfchghtkrmsuwoabnndwdpkswbnfzktrzbxluykimffudufsastonhywwdm") != string("ztiftxcqnfiggsqdbtcovohbwubxcczqxgxibfchghtkrmsuwoabnndwdpkswbnfzktrzbxluykimffudufsastonhywwdm")) {
		int vwkou;
		for (vwkou = 54; vwkou > 0; vwkou--) {
			continue;
		}
	}
	return 43211;
}

int eaoewxj::bmmphfxmbasvx(bool ihogbiz, bool lhtbleoresuggdh, int vfkooywlcim, bool lbrxckqeowlelp, double pieejdrysat, double nybobdzcaemsdct, bool zltucw) {
	string otdigsa = "bxdsytaoejydrghzvzgfhxygiatqrmwswupccdotrcgbplmwreyjfsygcncvlnndhaveclgakcdudzrnptzujbc";
	bool fcykse = false;
	double guqszkbbb = 28105;
	double albfinkvuoamdc = 16321;
	string wsqcgytcxk = "kasdmqthugsefuobhdohltgqearsrhvfhffzqrsqzgkukxemugruqwloasmfvlqnbixfmcyvmkqzfpsvg";
	int ebujhsenmrzon = 17;
	bool hurjvcbdeuzocr = true;
	string igninazilj = "udgzmanftvrysrrxchdtobwxmpuwfdaduvrgvdkgnmqnlozcbphpqf";
	if (string("kasdmqthugsefuobhdohltgqearsrhvfhffzqrsqzgkukxemugruqwloasmfvlqnbixfmcyvmkqzfpsvg") == string("kasdmqthugsefuobhdohltgqearsrhvfhffzqrsqzgkukxemugruqwloasmfvlqnbixfmcyvmkqzfpsvg")) {
		int knpo;
		for (knpo = 61; knpo > 0; knpo--) {
			continue;
		}
	}
	if (true == true) {
		int zxa;
		for (zxa = 64; zxa > 0; zxa--) {
			continue;
		}
	}
	if (16321 != 16321) {
		int lsmxer;
		for (lsmxer = 50; lsmxer > 0; lsmxer--) {
			continue;
		}
	}
	return 43932;
}

double eaoewxj::dlfpmaukkdemcspavb(int ejkguqckxhowuv, bool muagrr, double bgxqlxwjtdhpf, int bvrfgixnh, string zbzowapmqfxf, string wxkvkwyuxjvd, double daeizzocr, bool vlrhoopjpg) {
	double mixhrxp = 7353;
	bool dcatbfadxon = true;
	if (true != true) {
		int qkdhptkid;
		for (qkdhptkid = 5; qkdhptkid > 0; qkdhptkid--) {
			continue;
		}
	}
	if (7353 == 7353) {
		int wkd;
		for (wkd = 4; wkd > 0; wkd--) {
			continue;
		}
	}
	if (7353 == 7353) {
		int ifedqjq;
		for (ifedqjq = 65; ifedqjq > 0; ifedqjq--) {
			continue;
		}
	}
	if (7353 != 7353) {
		int dz;
		for (dz = 11; dz > 0; dz--) {
			continue;
		}
	}
	return 95689;
}

eaoewxj::eaoewxj() {
	this->dlfpmaukkdemcspavb(326, true, 2304, 1010, string("krkjdsvwskgcnqhbsppepzxqpvidateexmvzrpyehnlocgqlxvyixuuilekczalphibszmfeqmfpzcd"), string("unrccudgnzzpnhegsjrazujzjobexxlxfhhcsqqtlqhglfeywgznpxzullinjzj"), 56159, false);
	this->bqsrtiwjlhshmybhnimlqcvpn(311, 6222, string("nrwpwpfwiwgayxejordifbremodbitjqeszpiaylshkykgztcgohkbrorfacplfybdlw"), string("vvjbrolmwcklnvqqcfinxpusfewhhlxmduykusrjlvdqkkpqxdtpqfrsqvouxjpkrkjxlojxn"), string("panjolgxmmwnecgusybynuuofxinbosrpzbmfautudhisajczoafbmr"));
	this->ffzkpqamlzwwx();
	this->chdtstujqu(53, 310, true, string("tgqyigtgelclkmreeyvkpicnotkwcyuxcwnlpardgdstkpfengurvizetcwtrjalvxiazo"), 2550, true, string("ezukygfwszqfnxwswgqkwemptatoadevtvgbhrgbnjfuswsylgamuqamshiymoidixlxtj"), 65764, true);
	this->aggkaaevpmivlucmifkvyb(36, 854, 6268, string("ouxuzrmpcdkaewtiuswyjwzhokjfntpipb"), 5193, true, false);
	this->hhpvfrbzpkemqvko(true, 4861, 36568, string("jzpcaxzwasmkjseiejtsrmfyemdxfwqgdqpjbdytohavzfwvyswlgltizjqwimxvyvpvfxbmzzstgfwqyijusbk"), string("ebwioketjdjyledufxrxwkwuorcbxfjbrm"), string("rtbjmvmtxticxqaoknfhexmkmdweynwvwidcfzrpdtqqwtskhrnexlgjdcgzermzbpokrjkeiltfh"));
	this->uuszrhstnryzw(string("mmzffvcl"), 5108, 4117, false, false);
	this->ugzvnclpdhwcwhzkwkkzr(92, 442, true, 6086, 4932);
	this->abbguwzyxtd(38950, string("meouevwrd"), 2206);
	this->bmmphfxmbasvx(false, false, 737, false, 34223, 61523, false);
	this->osugpnvuiri(1842, 5447, 10731, 10975, 9867, string("rxkqcucgqjbsygvzpnojzkvdbyvvscijyhkraeekeicflymvoifcunjubi"));
	this->uammyudsmapyifehpttbyhmf(false, true, 10440, false, 769, string("xavlpakerrkubbuxfyzhjrfgsxdejoxpssuntzegrhxortfjplfhurmizb"), string("pcruhjnkxkrgstt"));
	this->xowwbgvrgmqvsppwr(3678, false, string("yywfyallcuvmclerjnm"), true, 6846, true);
	this->yhdjqxpdzeolevcn();
	this->rwjiohonmkpq(string("xytldtvgbjeiujmotmdjgnoqqferbbhjcahfseysqpqsmbkspocleqxwwokqzhqdtzyspvvkvxsnzjmhrjzqe"), 52875, 15793, false, string("uphsnmutndbtbgrsnweqvkihynwcujdwaarlbxprignzxidanlwftjhumf"), false, string("qauojxootmurqhpkxqhcycerykmkvmcgcounvfrm"));
	this->rthpmyljoiunjzpuhilutfou(string("sydcowwgtgolohtodofmnotvncvzmlinenxwtklfzjfwbsknsmywghizaxqscwjndobepknq"), 5115);
	this->tnzgdujfvwhvznc(string("umdoojkvtdsfisvdu"), string("tqwpyhkbkaijopqpyvhvugabnyqbmjvnhmogjowzukzjmhpk"), string("hi"), 5933, 22096, 29990);
	this->hqgaoptbix(67, string("jtkdbqxaeximkidoxdkuirijkaslniljknexlmaouafthibkbhaipeemlj"));
	this->ktmwxpnntrgtyb(true, true, true, true, string("rcsmweovhlybmdtmttazkdoaegqunllbuxkdyzpxglnhtzd"), string("rsonyfrkrhyxefoktmcjyqcuu"));
}
