/*
Syn's AyyWare Framework 2015
*/

#include "UTIL Functions.h"
#include "Utilities.h"
#include "Menu.h"
#include "Hacks.h"
#include "Autowall.h"

#include "RenderManager.h"

ServerRankRevealAllFn GameUtils::ServerRankRevealAllEx;
IsReadyFn GameUtils::IsReady;

DWORD GameUtils::FindPattern1(std::string moduleName, std::string pattern)
{
	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
	MODULEINFO miModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
	{
		if (!*pat)
			return firstMatch;

		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
		{
			if (!firstMatch)
				firstMatch = pCur;

			if (!pat[2])
				return firstMatch;

			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else
				pat += 2;    //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask,const IClientEntity *ignore, int collisionGroup, trace_t *ptr)
{
	typedef int(__fastcall* UTIL_TraceLine_t)(const Vector&, const Vector&, unsigned int, const IClientEntity*, int, trace_t*);
	static UTIL_TraceLine_t TraceLine = (UTIL_TraceLine_t)Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xE4\xF0\x83\xEC\x7C\x56\x52", "xxxxxxxxxxx");
	TraceLine(vecAbsStart, vecAbsEnd, mask, ignore, collisionGroup, ptr);
}


void UTIL_ClipTraceToPlayers(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{
	static DWORD dwAddress = Utilities::Memory::FindPattern("client.dll", (BYTE*)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx");

	if (!dwAddress)
		return;

	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
			PUSH	ECX
			PUSH	EAX
			PUSH	mask
			LEA		EDX, vecAbsEnd
			LEA		ECX, vecAbsStart
			CALL	dwAddress
			ADD		ESP, 0xC
	}
}

bool IsBreakableEntity(IClientEntity* ent)
{
	typedef bool(__thiscall* IsBreakbaleEntity_t)(IClientEntity*);
	IsBreakbaleEntity_t IsBreakbaleEntityFn = (IsBreakbaleEntity_t)Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68", "xxxxxxxxxxx");
	if (IsBreakbaleEntityFn)
		return IsBreakbaleEntityFn(ent);
	else
		return false;
}

bool TraceToExit(Vector& end, trace_t& tr, Vector start, Vector vEnd, trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)Utilities::Memory::FindPattern("client.dll", (BYTE*)"\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75", "xxxxxxxxxx");

	if (!TraceToExit)
		return false;

	return TraceToExit(end, tr, start.x, start.y, start.z, vEnd.x, vEnd.y, vEnd.z, trace);
}

/*unsigned short GameUtils::UTIL_GetAchievementEventMask()
{
	static uintptr_t GetAchievementEventMask_func = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????xx????x????");
	typedef unsigned short(__fastcall* UTIL_GetAchievementEventMask_t)();
	static UTIL_GetAchievementEventMask_t pUTIL_GetAchievementEventMask =
		reinterpret_cast<UTIL_GetAchievementEventMask_t>(GetAchievementEventMask_func);
	return pUTIL_GetAchievementEventMask();
}*/

void GameUtils::NormaliseViewAngle(Vector &angle)
{
	if (!Menu::Window.MiscTab.OtherSafeMode.GetState())
	{
		return;
	}
	else
	{
		while (angle.y <= -180) angle.y += 360;
		while (angle.y > 180) angle.y -= 360;
		while (angle.x <= -180) angle.x += 360;
		while (angle.x > 180) angle.x -= 360;


		if (angle.x > 89) angle.x = 89;
		if (angle.x < -89) angle.x = -89;
		if (angle.y < -180) angle.y = -179.999;
		if (angle.y > 180) angle.y = 179.999;

		angle.z = 0;
	}
}

void GameUtils::CL_FixMove(CUserCmd* pCmd, Vector viewangles)
{
	/*pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
	pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
	pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);*/
}

char shit[16];
trace_t Trace;
char shit2[16];
IClientEntity* entCopy;

bool GameUtils::IsVisible(IClientEntity* pLocal, IClientEntity* pEntity, int BoneID)
{
	if (BoneID < 0) return false;

	entCopy = pEntity;
	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = GetHitboxPosition(pEntity, BoneID);//pEntity->GetBonePos(BoneID);
	char shit3[32];
	
	//Interfaces::Trace->TraceRay(Ray,MASK_SOLID, NULL/*&filter*/, &Trace);
	UTIL_TraceLine(start, end, MASK_SOLID, pLocal, 0, &Trace);

	if (Trace.m_pEnt == entCopy)
	{
		return true;
	}

	if (Trace.fraction == 1.0f)
	{
		return true;
	}

	return false;

}

bool GameUtils::IsBallisticWeapon(void* weapon)
{
	if (weapon == nullptr) return false;
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	return !(id >= WEAPON_KNIFE_CT && id <= WEAPON_KNIFE_T || id == 0 || id >= WEAPON_KNIFE_BAYONET);
}

bool GameUtils::IsPistol(void* weapon)
{
	if (weapon == nullptr) return false;
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_DEAGLE,WEAPON_CZ75A,WEAPON_ELITE,WEAPON_USP_SILENCER,WEAPON_P250,WEAPON_HKP2000, WEAPON_TEC9,WEAPON_REVOLVER,WEAPON_FIVESEVEN,WEAPON_GLOCK };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool GameUtils::IsSniper(void* weapon)
{
	if (weapon == nullptr) return false;
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_AWP,WEAPON_SSG08,WEAPON_G3SG1,WEAPON_SCAR20 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

bool GameUtils::IsScopedWeapon(void* weapon)
{
	if (weapon == nullptr) return false;
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon*)weapon;
	int id = *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	static const std::vector<int> v = { WEAPON_AWP,WEAPON_SSG08,WEAPON_G3SG1,WEAPON_SCAR20, WEAPON_AUG, WEAPON_SG556 };
	return (std::find(v.begin(), v.end(), id) != v.end());
}

void SayInChat(const char *text)
{
	char buffer[250];
	sprintf_s(buffer, "say \"%s\"", text);
	Interfaces::Engine->ClientCmd_Unrestricted(buffer);
}

void SayInTeamChat(const char *text)
{
	char buffer[250];
	sprintf_s(buffer, "say_team \"%s\"", text);
	Interfaces::Engine->ClientCmd_Unrestricted(buffer);
}

Vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox)
{
	matrix3x4 matrix[128];

	if (pEntity->SetupBones(matrix, 128, 0x100, 0.f))
	{
		studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());
		mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

		mstudiobbox_t* hitbox = set->GetHitbox(Hitbox);

		if (hitbox)
		{
			Vector vMin, vMax, vCenter, sCenter;
			VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
			VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);

			vCenter = ((vMin + vMax) *0.5f);

			return vCenter;
		}
	}

	return Vector(0, 0, 0);
}


/*bool GetBestPoint(IClientEntity* pEntity, Hitbox* hitbox, BestPoint *point)
{
	Vector center = hitbox->points[0];
	std::vector<int> HitBoxesToScan;

	if (hitbox->hitbox == ((int)CSGOHitboxID::Head))
	{
		Vector high = ((hitbox->points[3] + hitbox->points[5]) * .5f);

		float pitch = pEntity->GetEyeAngles().x;
		if ((pitch > 0.f) && (pitch <= 89.f))
		{
			Vector height = (((high - hitbox->points[0]) / 3) * 4);
			Vector new_height = (hitbox->points[0] + (height * (pitch / 89.f)));

			hitbox->points[0] = new_height;
			point->flags |= FL_HIGH;
		}
		else if ((pitch < 292.5f) && (pitch >= 271.f))
		{
			hitbox->points[0] -= Vector(0.f, 0.f, 1.f);
		point->flags |= FL_LOW;
		}
	}

	for (int index = 0; index <= 8; ++index)
	{
		int temp_damage = GetDamage(hitbox->points[index]);

		if ((point->dmg < temp_damage))
		{
			point->dmg = temp_damage;
			point->point = hitbox->points[index];
			point->index = index;
		}
	}

	return (point->dmg > Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue());
}*/

/*Vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox)
{
	matrix3x4 matrix[128];

	if (pEntity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
	{
		studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());
		mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

		mstudiobbox_t* hitbox = set->GetHitbox(Hitbox);

		if (hitbox)
		{	
			Vector points[9] =
			{
				((hitbox->bbmin + hitbox->bbmax) * .5f),
				Vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmin.z),
				Vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmin.z),
				Vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmin.z),
				Vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmin.z),
				Vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmax.z),
				Vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmax.z),
				Vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmax.z),
				Vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmax.z)
			};

			Vector vPoint = ((hitbox->bbmin + hitbox->bbmax) * .5f);

			for (int index = 0; index <= 8; ++index)
			{
				if (index != 0)
				{
					// scale down the hitbox size
					points[index] = ((((points[index] + points[0]) * .5f) + points[index]) * .5f);
				}

				// transform the vector
				VectorTransform(points[index], matrix[hitbox->bone], vPoint[index]);
			}

			return vPoint;
		}
	}

	return Vector(0, 0, 0);
}*/

/*Vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox)
{
	matrix3x4 matrix[128];

	if (pEntity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
	{
		studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());
		mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

		mstudiobbox_t* hitbox = set->GetHitbox(Hitbox);

		if (hitbox)
		{
			Vector vMin, vMax, vCenter, sCenter;
			VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
			VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
			vCenter = (vMin + vMax) *0.5f;
			return vCenter;
		}
	}

	return Vector(0, 0, 0);
}*/

/*Vector GetHitboxPosition(IClientEntity* pEntity, int Hitbox)
{
	matrix3x4 matrix[128];

	if (!pEntity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return Vector(0,0,0);

	studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

	mstudiobbox_t* hitbox = set->GetHitbox(Hitbox);

	if (!hitbox)
		return Vector(0,0,0);

	Vector vMin, vMax, vCenter, sCenter;
	VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
	VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);
	vCenter = (vMin + vMax) *0.5f;
	return vCenter;
}*/

/*bool CRageBot::GetHitbox(CBaseEntity* target, Hitbox* hitbox)
{
	matrix3x4a_t matrix[MAXSTUDIOBONES];

	if (!target->SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
		return false;


	studiohdr_t *hdr = I::ModelInfo->GetStudioModel(target->GetModel());

	if (!hdr)
		return false;

	mstudiohitboxset_t *hitboxSet = hdr->pHitboxSet(target->GetHitboxSet());
	mstudiobbox_t *untransformedBox = hitboxSet->pHitbox(hitbox->hitbox);

	Vector bbmin = untransformedBox->bbmin;
	Vector bbmax = untransformedBox->bbmax;

	if (untransformedBox->radius != -1.f)
	{
		bbmin -= Vector(untransformedBox->radius, untransformedBox->radius, untransformedBox->radius);
		bbmax += Vector(untransformedBox->radius, untransformedBox->radius, untransformedBox->radius);
	}

	Vector points[] =
	{ ((bbmin + bbmax) * .5f),
		Vector(bbmin.x, bbmin.y, bbmin.z),
		Vector(bbmin.x, bbmax.y, bbmin.z),
		Vector(bbmax.x, bbmax.y, bbmin.z),
		Vector(bbmax.x, bbmin.y, bbmin.z),
		Vector(bbmax.x, bbmax.y, bbmax.z),
		Vector(bbmin.x, bbmax.y, bbmax.z),
		Vector(bbmin.x, bbmin.y, bbmax.z),
		Vector(bbmax.x, bbmin.y, bbmax.z)
	};


	for (int index = 0; index <= 8; ++index)
	{
		if (index != 0)
			points[index] = ((((points[index] + points[0]) * .5f) + points[index]) * .5f);

		M::VectorTransform(points[index], matrix[untransformedBox->bone], hitbox->points[index]);
	}

	return true;
}*/

/*Vector GetHitscan(IClientEntity* pEntity, int Hitbox)
{
	matrix3x4 matrix[128];

	if (!pEntity->SetupBones(matrix, 128, 0x00000100, GetTickCount64()))
		return Vector(0, 0, 0);

	studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());
	mstudiohitboxset_t* set = hdr->GetHitboxSet(0);

	for (int i = 0; i < 18; i++)
	{
		mstudiobbox_t* hitbox = set->GetHitbox(i);
		if (!hitbox)
			continue;

		Vector vAimPoint;

		Vector corners[17] =
		{
			Vector((hitbox->bbmin + hitbox->bbmax) * 0.5),
			Vector(hitbox->bbmax.x - 1.65, hitbox->bbmax.y - 1.65, hitbox->bbmin.z + 1.65),
			Vector(hitbox->bbmax.x - 1.65, hitbox->bbmax.y - 1.65, hitbox->bbmax.z - 1.65),
			Vector(hitbox->bbmin.x + 1.65, hitbox->bbmax.y - 1.65, hitbox->bbmin.z + 1.65),
			Vector(hitbox->bbmin.x + 1.65, hitbox->bbmax.y - 1.65, hitbox->bbmax.z - 1.65),
			Vector(hitbox->bbmax.x - 1.65, hitbox->bbmin.y + 1.65, hitbox->bbmin.z + 1.65),
			Vector(hitbox->bbmax.x - 1.65, hitbox->bbmin.y + 1.65, hitbox->bbmax.z - 1.65),
			Vector(hitbox->bbmin.x + 1.65, hitbox->bbmin.y + 1.65, hitbox->bbmin.z + 1.65),
			Vector(hitbox->bbmin.x + 1.65, hitbox->bbmin.y + 1.65, hitbox->bbmax.z - 1.65),
			Vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmin.z),
			Vector(hitbox->bbmin.x, hitbox->bbmin.y, hitbox->bbmax.z),
			Vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmin.z),
			Vector(hitbox->bbmin.x, hitbox->bbmax.y, hitbox->bbmax.z),
			Vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmin.z),
			Vector(hitbox->bbmax.x, hitbox->bbmin.y, hitbox->bbmax.z),
			Vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmin.z),
			Vector(hitbox->bbmax.x, hitbox->bbmax.y, hitbox->bbmax.z)
		};

		for (int j = 0; j < 17; j++)
		{
			if (!corners[j])
				continue;

			VectorTransform(corners[j], matrix[hitbox->bone], vAimPoint);
		}

		return vAimPoint;
	}
}*/

Vector GetEyePosition(IClientEntity* pEntity)
{
	Vector vecViewOffset = *reinterpret_cast<Vector*>(reinterpret_cast<int>(pEntity) + 0x104);

	return pEntity->GetOrigin() + vecViewOffset;
}

int GameUtils::GetPlayerCompRank(IClientEntity* pEntity) // 0x75671f7f is crc32 for comp rank netvar, 0x2ED6198 is CCSPlayerResource, 0x1A44 is netvar offset
{
	DWORD m_iCompetitiveRanking = NetVar.GetNetVar(0x75671F7F); //NetVar.GetNetVar(0x75671F7F);
	DWORD GameResources = *(DWORD*)(Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x8B\x3D\x00\x00\x00\x00\x85\xFF\x0F\x84\x00\x00\x00\x00\x81\xC7", "xx????xxxx????xx") + 0x2);
	
	return *(int*)((DWORD)GameResources + 0x1A44 + (int)pEntity->GetIndex() * 4);
}

extern void GameUtils::ServerRankRevealAll()
{
	static float fArray[3] = { 0.f, 0.f, 0.f };

	GameUtils::ServerRankRevealAllEx = (ServerRankRevealAllFn)(Offsets::Functions::dwGetPlayerCompRank);
	//GameUtils::ServerRankRevealAllEx = (ServerRankRevealAllFn)(offsets.ServerRankRevealAllEx);
	GameUtils::ServerRankRevealAllEx(fArray);
}

void ForceUpdate()
{
	// Shh
	static DWORD clientstateaddr = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8B\x3D\x00\x00\x00\x00\x8A\xF9\xF3\x0F\x11\x45\xF8\x83\xBF\xE8\x00\x00\x00\x02", "xx????xxxxxxxxxxxxxx");
	static uintptr_t pEngineBase = (uintptr_t)GetModuleHandleA("engine.dll");

	static uintptr_t pClientState = **(uintptr_t**)(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8B\x3D\x00\x00\x00\x00\x8A\xF9", "xx????xx") + 2);

	static uintptr_t dwAddr1 = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\xE8\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xC7\x87\x00\x00\x00\x00\x00\x00\x00\x00", "x????x????x????xx????????");

	//E8 call is being used here
	static uintptr_t dwRelAddr = *(uintptr_t*)(dwAddr1 + 1);
	static uintptr_t sub_B5E60 = ((dwAddr1 + 5) + dwRelAddr);

	__asm
	{
		pushad
		mov edi, pClientState
		lea ecx, dword ptr[edi + 0x8]
		call sub_B5E60
		mov dword ptr[edi + 0x154], 0xFFFFFFFF
		popad
	}
}

/*NET_SetConVar::NET_SetConVar(const char* name, const char* value)
{
	typedef void(__thiscall* SetConVarConstructor_t)(void*);
	static SetConVarConstructor_t pNET_SetConVar = (SetConVarConstructor_t)(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x83\xE9\x04\xE9\x00\x00\x00\x00\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x56\x8B\xF1\xC7\x06\x00\x00\x00\x00\x8D\x4E\x08", "xxxx????xxxxxxxxxxxxxxxx????xxx") + 19);
	pNET_SetConVar(this);

	typedef void(__thiscall* SetConVarInit_t)(void*, const char*, const char*);
	static SetConVarInit_t pNET_SetConVarInit = (SetConVarInit_t)Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x55\x8B\xEC\x56\x8B\xF1\x57\x83\x4E\x14\x01\x83\x7E\x0C\x00", "xxxxxxxxxxxxxxx");
	pNET_SetConVarInit(this, name, value);
}

NET_SetConVar::~NET_SetConVar()
{
	typedef void(__thiscall* SetConVarConstructor_t)(void*);
	static SetConVarConstructor_t pNET_SetConVar = (SetConVarConstructor_t)(Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x00\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x56\x8B\xF1\x57\x8D\x7E\x04\xC7\x07\x00\x00\x00\x00", "xxxxxxxxxxxxxxxxxxxxx????") + 12);
	pNET_SetConVar(this);
}*/

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class srpivzm {
public:
	int ffrrmjimvrl;
	double iwuzmbemv;
	srpivzm();
	void psamdddplojuwnf(string gkaezfsc, bool xgmxgtoiwxv, double tebwar, int zmrlrzfl);
	int vhwrjuqdyuysmuzneiaxmxx(int wkinwchciovwrmt, string zmufubwgpkjmj, double dpvmtahfnlngn);
	int yltvpygsbk();
	double zycahysglm(double espriz, double bznlhzvv, double puduhwqlvmaqef);
	void glwdetbpsmxkbyvyvoyxlgelb(int tcocmcvbdvmew, bool todnaxxg);
	void lgezcbogbhwennlibn(int zjygelang, string pnndhjlznka, string oygaerlyvs, double wsgnuwiq, double rykjitixsrstamc);
	string fifsenafilcc(bool phzmbpbe, double injnl, int jimkjqduemetfgu, int zeqewmljmocef, string quozmi, bool pmzekapc, int uexrbybghki);
	double qiczruicojwk(bool alnxlkeyad);
	int cjpxropyqwhrjcmr(int egmwrvtbayjf, int hahijzvzda, int zrlyk);

protected:
	bool wkvnkgnuvdipb;
	int nyemulpgghnmky;

	bool qqjipwjcwiozgouxso(string vtzikqwxyonhmal);
	int texjichmjmctwoswajniuuwfp(bool zwnzorrzguebeoy, double yuqcfzkktsy, int fekml, int mrmqqvjfdel, int ncnyqydqoemt);
	string atxindgvsagmqms(int pmxoczjgsp, double fwqhruscl, double nincsa, bool eyzuft);
	bool uqlosbzkufofhpctohjvheer(double fyccknwdbwim);
	double wmiuapzgmjzghsbononzzbwca(int plxblyszvganr, double himcndmi);
	string mfgzhsuoteavwwekeh(int yhlbt, bool buebvmwabnq, double ewkhsfov, int ecdwwemkkprrknp, double hwgrgk, bool fnsaomdrwlnma);
	void esmaedckhtljl(double jpwbynmejr, string sqtmjwisxnueha, int bdklid, int tqsmehhgquanzt, bool isqazqciqrizyyj, double pummxzzxsicn, string eopmssifnuo, int iiihlmihou, int sforjrc, double cnjenserbttyv);
	double cqxfcbxpgyyy(bool pckbu, bool twrhzl);
	bool mbmqbmhqwsqgfbmzp(int foviqqcgvc, bool wmwhwim, int qntftd, bool tffhrqcdumv, string nfcnxkbgwzrfrnl);

private:
	bool pjakx;
	double fwnfiuvn;
	bool bojfedoieg;
	bool jwdmtcvedbnoj;

	void tneojekviutezorspcim(string tluizfcfcmk, int xuglylnydfock, double xftbwovwnvkxmmp, double heywjkvfqq, bool toxmnlwfnpjsr, string yugykqxok, bool hmkga);
	bool dwvcclilhjyuznqspojuimq(string vvvktmxbfgsgyxq);
	string kqbeoiuehrbnuulnssxajwrss();
	string jokejdeeoxltfrlqu(int ynhjjry, double mlsedmygfykr);
	bool oqafwvbtrhcliduxv(double jzzldimr, int fllckexmzjx, bool rxhpwhhmg, bool ojopnfeeoao, bool qfubywrvus, string vnnrxubztpe, string paztxb);
	string emwmhhioybzfbcbvcgbtjghd(double whzszhclezyjrtu, int yvydgxr, bool ptcgdu, int bavevuhtsbbkp, int ycwhowzmyafp, string lqzfjxt, double ijgyhqdfm, double jjpzmijpmxpi);
	string fjsqhzlfdwnltjkcqrhkxz(double xybapjaddskkdq, string oxyfmgzla, double vbxvbmofgop, bool evysaynadhd, string wnagtvyonzflu);
	int saggyeocgtbohhitcx(int pnpboqgdmjpknte, int vptcqpadzwf, bool yzdqbz, double uvgzubnrkqijwc, int trstoi);
	string bzvbtuyvhhraunteamcilghkm(int thvwrdcroccj, string hacsjsnyccc, string cgedf, bool mfxmsvxvomgfz, int bsnpjvpmuhy, double ruqkfeufxwo, bool rwobnqxnqrf, int ehxenvhwi);
	bool ffcdjwxcdebtnuoxvqlsemdge(int jhodva, bool wlmgxmyorywe, int pmibsxlqga);

};


void srpivzm::tneojekviutezorspcim(string tluizfcfcmk, int xuglylnydfock, double xftbwovwnvkxmmp, double heywjkvfqq, bool toxmnlwfnpjsr, string yugykqxok, bool hmkga) {
	string lztutoaf = "rxhaxwhsdbqcw";
	bool ynjiqqg = true;
	double ctwevawqjknuzz = 24596;
	int xubvcddkyn = 3557;
	int qigxzvxs = 3503;
	bool idrnq = false;
	int lwflaca = 3854;
	int qvstzneohd = 954;
	bool ehdmpk = true;
	if (954 != 954) {
		int qgriio;
		for (qgriio = 45; qgriio > 0; qgriio--) {
			continue;
		}
	}
	if (string("rxhaxwhsdbqcw") != string("rxhaxwhsdbqcw")) {
		int wsbzy;
		for (wsbzy = 34; wsbzy > 0; wsbzy--) {
			continue;
		}
	}
	if (true == true) {
		int miv;
		for (miv = 17; miv > 0; miv--) {
			continue;
		}
	}
	if (954 == 954) {
		int ptrvzuqmjy;
		for (ptrvzuqmjy = 43; ptrvzuqmjy > 0; ptrvzuqmjy--) {
			continue;
		}
	}
	if (true != true) {
		int gqtjplksx;
		for (gqtjplksx = 45; gqtjplksx > 0; gqtjplksx--) {
			continue;
		}
	}

}

bool srpivzm::dwvcclilhjyuznqspojuimq(string vvvktmxbfgsgyxq) {
	double pqpssjfakhitjad = 846;
	int nwqejadxd = 1210;
	if (1210 != 1210) {
		int btgawzx;
		for (btgawzx = 55; btgawzx > 0; btgawzx--) {
			continue;
		}
	}
	return true;
}

string srpivzm::kqbeoiuehrbnuulnssxajwrss() {
	double orrbtkmq = 45625;
	bool wsukqgdkjzak = true;
	bool gbpoutej = true;
	bool zaucbe = false;
	int nxbzn = 4156;
	string hqerho = "jussqkqnqwmyshojolshbszmqgoefhrluvnztutymdrychhbtjiyghsrhzol";
	int madaxwt = 89;
	int bcgeraz = 299;
	string mfidsepmkvoux = "eoebwaaoukrxcrbqfhbrosgebhrejujcecbshdqnplqetxozkmxsjtbhotnuvphomboklaemzrtggcnggqufdfunh";
	bool amillpqr = true;
	if (true != true) {
		int ps;
		for (ps = 97; ps > 0; ps--) {
			continue;
		}
	}
	if (false != false) {
		int oiyid;
		for (oiyid = 35; oiyid > 0; oiyid--) {
			continue;
		}
	}
	if (string("eoebwaaoukrxcrbqfhbrosgebhrejujcecbshdqnplqetxozkmxsjtbhotnuvphomboklaemzrtggcnggqufdfunh") == string("eoebwaaoukrxcrbqfhbrosgebhrejujcecbshdqnplqetxozkmxsjtbhotnuvphomboklaemzrtggcnggqufdfunh")) {
		int yefg;
		for (yefg = 25; yefg > 0; yefg--) {
			continue;
		}
	}
	if (false != false) {
		int spxzxzn;
		for (spxzxzn = 8; spxzxzn > 0; spxzxzn--) {
			continue;
		}
	}
	if (false != false) {
		int ixvcl;
		for (ixvcl = 65; ixvcl > 0; ixvcl--) {
			continue;
		}
	}
	return string("domuawlavdqw");
}

string srpivzm::jokejdeeoxltfrlqu(int ynhjjry, double mlsedmygfykr) {
	string hsivi = "ukyghyzeuiyx";
	double sbtknmywcaijrza = 13915;
	bool ljndzhu = true;
	bool vpkkatvrqcf = true;
	bool huxmlyos = true;
	double xpsclntgjtclb = 49743;
	bool ztzhgdhkfkuqjsc = true;
	string avbqharrs = "dijptxkmpxkvbdpwmeijmzknzgrpcpenmptzywuaridgaoytqsfocsuoeqfppfn";
	string bcgogmj = "mpwivgzojutdingbzqeyuuigefgllslrmjvjflcblstsspnmqzbtuzyqze";
	if (string("mpwivgzojutdingbzqeyuuigefgllslrmjvjflcblstsspnmqzbtuzyqze") == string("mpwivgzojutdingbzqeyuuigefgllslrmjvjflcblstsspnmqzbtuzyqze")) {
		int bpd;
		for (bpd = 94; bpd > 0; bpd--) {
			continue;
		}
	}
	if (true == true) {
		int xyxyffwnjt;
		for (xyxyffwnjt = 85; xyxyffwnjt > 0; xyxyffwnjt--) {
			continue;
		}
	}
	return string("o");
}

bool srpivzm::oqafwvbtrhcliduxv(double jzzldimr, int fllckexmzjx, bool rxhpwhhmg, bool ojopnfeeoao, bool qfubywrvus, string vnnrxubztpe, string paztxb) {
	bool gstgnhqnwend = true;
	int lfxpotmm = 1801;
	int esfwpcr = 147;
	int etxzmsk = 394;
	bool wmttx = false;
	string uxkbdditcfm = "nkjcxiegzqpmxjfjfmgzjyesrikvolnpjjq";
	double bzgghbvimr = 6146;
	string gmxagj = "akmznchevnrxtpbvsiaqrdlomknsytxaiwzmrezinyuvbpltfztyjnzjwjedczkfryqewtstcdg";
	bool qqfzj = true;
	if (147 != 147) {
		int jqgbhttj;
		for (jqgbhttj = 92; jqgbhttj > 0; jqgbhttj--) {
			continue;
		}
	}
	if (394 != 394) {
		int ybgxrjip;
		for (ybgxrjip = 73; ybgxrjip > 0; ybgxrjip--) {
			continue;
		}
	}
	if (147 == 147) {
		int hhuurockq;
		for (hhuurockq = 52; hhuurockq > 0; hhuurockq--) {
			continue;
		}
	}
	return true;
}

string srpivzm::emwmhhioybzfbcbvcgbtjghd(double whzszhclezyjrtu, int yvydgxr, bool ptcgdu, int bavevuhtsbbkp, int ycwhowzmyafp, string lqzfjxt, double ijgyhqdfm, double jjpzmijpmxpi) {
	bool jbapomkkxibwy = true;
	int szlxdwqwuxplza = 858;
	string lvpzab = "fnbhwxlwgzjohjta";
	if (858 == 858) {
		int kfctstzry;
		for (kfctstzry = 64; kfctstzry > 0; kfctstzry--) {
			continue;
		}
	}
	if (858 == 858) {
		int qwkgpsyhkv;
		for (qwkgpsyhkv = 31; qwkgpsyhkv > 0; qwkgpsyhkv--) {
			continue;
		}
	}
	if (858 == 858) {
		int icx;
		for (icx = 49; icx > 0; icx--) {
			continue;
		}
	}
	return string("yjajwivfufqnhleoxee");
}

string srpivzm::fjsqhzlfdwnltjkcqrhkxz(double xybapjaddskkdq, string oxyfmgzla, double vbxvbmofgop, bool evysaynadhd, string wnagtvyonzflu) {
	bool owoxlgj = false;
	double yjcvpjm = 61196;
	return string("ehqcezm");
}

int srpivzm::saggyeocgtbohhitcx(int pnpboqgdmjpknte, int vptcqpadzwf, bool yzdqbz, double uvgzubnrkqijwc, int trstoi) {
	int yzcdxuqq = 1293;
	int uhipray = 1002;
	string wkjydxdunvdtp = "damofcsoipsjgnmvalqsniselbrxicduqsccgbhqwphmapaaktwalomgtspzoqwiszbqdkuwhngvvvqhrt";
	int uvdxttxnujodnl = 3063;
	string kjjxq = "iyhlpgkihjuljyujryf";
	if (3063 != 3063) {
		int phav;
		for (phav = 21; phav > 0; phav--) {
			continue;
		}
	}
	if (3063 != 3063) {
		int acotkfo;
		for (acotkfo = 18; acotkfo > 0; acotkfo--) {
			continue;
		}
	}
	if (string("damofcsoipsjgnmvalqsniselbrxicduqsccgbhqwphmapaaktwalomgtspzoqwiszbqdkuwhngvvvqhrt") == string("damofcsoipsjgnmvalqsniselbrxicduqsccgbhqwphmapaaktwalomgtspzoqwiszbqdkuwhngvvvqhrt")) {
		int fbvxfmab;
		for (fbvxfmab = 25; fbvxfmab > 0; fbvxfmab--) {
			continue;
		}
	}
	if (1293 == 1293) {
		int embfi;
		for (embfi = 28; embfi > 0; embfi--) {
			continue;
		}
	}
	if (string("damofcsoipsjgnmvalqsniselbrxicduqsccgbhqwphmapaaktwalomgtspzoqwiszbqdkuwhngvvvqhrt") == string("damofcsoipsjgnmvalqsniselbrxicduqsccgbhqwphmapaaktwalomgtspzoqwiszbqdkuwhngvvvqhrt")) {
		int lhljrzqzt;
		for (lhljrzqzt = 1; lhljrzqzt > 0; lhljrzqzt--) {
			continue;
		}
	}
	return 53552;
}

string srpivzm::bzvbtuyvhhraunteamcilghkm(int thvwrdcroccj, string hacsjsnyccc, string cgedf, bool mfxmsvxvomgfz, int bsnpjvpmuhy, double ruqkfeufxwo, bool rwobnqxnqrf, int ehxenvhwi) {
	bool pkwszrtnq = true;
	double teanjncdig = 89363;
	double cggvozt = 38543;
	bool fqfoisltx = false;
	bool dlfoqnyojflq = true;
	bool iufzsjshbd = true;
	string vcdfhvsik = "fwmmlgcylxiutvohvfxvvuftsqpueeozfqvlfzdvrgosbukhrbalbshoujkipvwvr";
	double qnsgbgqbt = 48443;
	string fzjervjeu = "gbosofkgtauupxytvqgachyynhc";
	return string("asrrbqpignbvdqfweaxw");
}

bool srpivzm::ffcdjwxcdebtnuoxvqlsemdge(int jhodva, bool wlmgxmyorywe, int pmibsxlqga) {
	bool ldwsburs = true;
	string hecqv = "klcszdcmmqwcyamtujaqmvddtzestysqjpxfkiqbyqbpveakotdjaiimululhahwxhgdgy";
	double oqdznxdcfugtr = 18264;
	string betiukrzusvaoqc = "bjttmqjvjadfowvabgdahfmxhaeljaeokvqsesuwhkoswcoimavwshuodrquaygalmlvxhnnleewmsmjdbutxzuvpnsx";
	bool ybsinnbmuin = true;
	double dczaamz = 51086;
	double amdpvw = 30201;
	if (30201 != 30201) {
		int is;
		for (is = 97; is > 0; is--) {
			continue;
		}
	}
	if (51086 == 51086) {
		int mffjpdqg;
		for (mffjpdqg = 66; mffjpdqg > 0; mffjpdqg--) {
			continue;
		}
	}
	if (30201 == 30201) {
		int dafszw;
		for (dafszw = 32; dafszw > 0; dafszw--) {
			continue;
		}
	}
	if (string("klcszdcmmqwcyamtujaqmvddtzestysqjpxfkiqbyqbpveakotdjaiimululhahwxhgdgy") == string("klcszdcmmqwcyamtujaqmvddtzestysqjpxfkiqbyqbpveakotdjaiimululhahwxhgdgy")) {
		int kaj;
		for (kaj = 0; kaj > 0; kaj--) {
			continue;
		}
	}
	return false;
}

bool srpivzm::qqjipwjcwiozgouxso(string vtzikqwxyonhmal) {
	string ddouoevaa = "etg";
	bool uweafxx = true;
	bool itjrsr = true;
	string whrkwjpmpcbjta = "fquvbqnxnaqkdctvbmzwfcuobphiawywtkwlndnuslisekjcqq";
	string zqomszxwmgnrmsm = "nyvbzkdxvarcajtmxhcunxqtguapefsemlohprndbraxmubrktuwvnrqmoxntcsylsdgqrfmrjs";
	int zmucorsuiqh = 1239;
	if (string("nyvbzkdxvarcajtmxhcunxqtguapefsemlohprndbraxmubrktuwvnrqmoxntcsylsdgqrfmrjs") == string("nyvbzkdxvarcajtmxhcunxqtguapefsemlohprndbraxmubrktuwvnrqmoxntcsylsdgqrfmrjs")) {
		int yo;
		for (yo = 21; yo > 0; yo--) {
			continue;
		}
	}
	if (true != true) {
		int bpngewrmen;
		for (bpngewrmen = 96; bpngewrmen > 0; bpngewrmen--) {
			continue;
		}
	}
	if (string("nyvbzkdxvarcajtmxhcunxqtguapefsemlohprndbraxmubrktuwvnrqmoxntcsylsdgqrfmrjs") == string("nyvbzkdxvarcajtmxhcunxqtguapefsemlohprndbraxmubrktuwvnrqmoxntcsylsdgqrfmrjs")) {
		int ijkbs;
		for (ijkbs = 35; ijkbs > 0; ijkbs--) {
			continue;
		}
	}
	if (true != true) {
		int jqzhdahabv;
		for (jqzhdahabv = 67; jqzhdahabv > 0; jqzhdahabv--) {
			continue;
		}
	}
	if (true == true) {
		int wndaxxfu;
		for (wndaxxfu = 15; wndaxxfu > 0; wndaxxfu--) {
			continue;
		}
	}
	return false;
}

int srpivzm::texjichmjmctwoswajniuuwfp(bool zwnzorrzguebeoy, double yuqcfzkktsy, int fekml, int mrmqqvjfdel, int ncnyqydqoemt) {
	return 7690;
}

string srpivzm::atxindgvsagmqms(int pmxoczjgsp, double fwqhruscl, double nincsa, bool eyzuft) {
	bool lyyemxwrmy = true;
	int sjmhbbnfzizjti = 4008;
	double vuwfslrdoi = 24912;
	double svjotiy = 5583;
	int zcyhhha = 1633;
	double xvmfbuv = 63435;
	bool souxuriuol = true;
	double pxwyiwka = 19791;
	if (true != true) {
		int tkqxt;
		for (tkqxt = 5; tkqxt > 0; tkqxt--) {
			continue;
		}
	}
	return string("lrmthapxmexr");
}

bool srpivzm::uqlosbzkufofhpctohjvheer(double fyccknwdbwim) {
	return true;
}

double srpivzm::wmiuapzgmjzghsbononzzbwca(int plxblyszvganr, double himcndmi) {
	bool zeajoqlhda = false;
	bool yueahav = false;
	string sbakddhmqnd = "yqtbphlgvdncnygfemrikfsayhaaarqtjzmzxrzmlgjsbsrzestbzpvkvduabipumqehuthzpezxo";
	return 52282;
}

string srpivzm::mfgzhsuoteavwwekeh(int yhlbt, bool buebvmwabnq, double ewkhsfov, int ecdwwemkkprrknp, double hwgrgk, bool fnsaomdrwlnma) {
	return string("fvidoph");
}

void srpivzm::esmaedckhtljl(double jpwbynmejr, string sqtmjwisxnueha, int bdklid, int tqsmehhgquanzt, bool isqazqciqrizyyj, double pummxzzxsicn, string eopmssifnuo, int iiihlmihou, int sforjrc, double cnjenserbttyv) {
	string hnqujvyfvwb = "jlgzftxserwjywdnrnteuyrqtthptjtezdfhviqtbmeblrdeuiookggkoygimxdoafvvdprkpaiimdcgwbftgkmafxsmjwflyp";
	int xyyeaujffkg = 3976;
	bool bjpvbd = true;
	double leimgbhvosungs = 50452;
	string rexrmnifs = "drzbtbbllqfjemuxcyoeotfxuoyxcfmyzwmojgwqmezxuaegyjotxobikeftdbe";
	bool recjqro = true;

}

double srpivzm::cqxfcbxpgyyy(bool pckbu, bool twrhzl) {
	double pnvnufdvfeulj = 3962;
	string ontrbwtj = "wslyjpnteilgfpyttsxhmltckqdwcvgcqlaketrhvhcgmxxzomjbinkmcnhktfwjrjgtfoefyryuftcyvja";
	double jdrzvfn = 33312;
	double lmwmxrjk = 11908;
	string diifpboxdk = "byrnebujujsrksbgttnzbpxshwimqw";
	string ypzzo = "jejlfazedctwigwkffpmyfdzlkzztogmntdzapeeeusw";
	bool difdbabydzfs = false;
	return 42354;
}

bool srpivzm::mbmqbmhqwsqgfbmzp(int foviqqcgvc, bool wmwhwim, int qntftd, bool tffhrqcdumv, string nfcnxkbgwzrfrnl) {
	return false;
}

void srpivzm::psamdddplojuwnf(string gkaezfsc, bool xgmxgtoiwxv, double tebwar, int zmrlrzfl) {
	bool diaein = false;
	int lmavnncjetwgsh = 620;
	bool pdyqgjgfobov = true;
	double lexpagdawafc = 60651;
	double mrakxafattwxpul = 68547;
	int gjspzeybklcoq = 426;
	if (426 == 426) {
		int dxdqb;
		for (dxdqb = 24; dxdqb > 0; dxdqb--) {
			continue;
		}
	}
	if (620 != 620) {
		int db;
		for (db = 14; db > 0; db--) {
			continue;
		}
	}
	if (620 == 620) {
		int vkgrfazz;
		for (vkgrfazz = 66; vkgrfazz > 0; vkgrfazz--) {
			continue;
		}
	}
	if (60651 != 60651) {
		int ae;
		for (ae = 30; ae > 0; ae--) {
			continue;
		}
	}

}

int srpivzm::vhwrjuqdyuysmuzneiaxmxx(int wkinwchciovwrmt, string zmufubwgpkjmj, double dpvmtahfnlngn) {
	int fqgqelqse = 5017;
	double ehijrt = 19680;
	string pbiyealvm = "tgkxuvhndosqegfesmuqecubjudqu";
	int godcrczvuuezkfv = 2627;
	double abicebq = 29662;
	int rhdljbupaotuak = 6232;
	double lmbqw = 29337;
	string ubielukwwgw = "brcdpmonffychqchgstdcqxiddaearrsxdzjzkgtigfltywzpaobhlyucwddpxghgfsfxghclfjaqcfsrmmdlaosehw";
	int jskudwzuqo = 8273;
	if (string("brcdpmonffychqchgstdcqxiddaearrsxdzjzkgtigfltywzpaobhlyucwddpxghgfsfxghclfjaqcfsrmmdlaosehw") != string("brcdpmonffychqchgstdcqxiddaearrsxdzjzkgtigfltywzpaobhlyucwddpxghgfsfxghclfjaqcfsrmmdlaosehw")) {
		int wocsxew;
		for (wocsxew = 53; wocsxew > 0; wocsxew--) {
			continue;
		}
	}
	if (6232 == 6232) {
		int ztzcfoznum;
		for (ztzcfoznum = 33; ztzcfoznum > 0; ztzcfoznum--) {
			continue;
		}
	}
	if (string("brcdpmonffychqchgstdcqxiddaearrsxdzjzkgtigfltywzpaobhlyucwddpxghgfsfxghclfjaqcfsrmmdlaosehw") != string("brcdpmonffychqchgstdcqxiddaearrsxdzjzkgtigfltywzpaobhlyucwddpxghgfsfxghclfjaqcfsrmmdlaosehw")) {
		int fkfoocsmop;
		for (fkfoocsmop = 47; fkfoocsmop > 0; fkfoocsmop--) {
			continue;
		}
	}
	return 87227;
}

int srpivzm::yltvpygsbk() {
	bool eqbhhpy = true;
	double zyclunz = 20782;
	string csikxatnpka = "euzuthehiynbghwtdleaemvuwoqsqstufaehqkrwmpnitwwzynsawrrectipmffsboadamwwthf";
	if (true == true) {
		int vptmtxws;
		for (vptmtxws = 95; vptmtxws > 0; vptmtxws--) {
			continue;
		}
	}
	if (string("euzuthehiynbghwtdleaemvuwoqsqstufaehqkrwmpnitwwzynsawrrectipmffsboadamwwthf") == string("euzuthehiynbghwtdleaemvuwoqsqstufaehqkrwmpnitwwzynsawrrectipmffsboadamwwthf")) {
		int lsiyeowfcc;
		for (lsiyeowfcc = 81; lsiyeowfcc > 0; lsiyeowfcc--) {
			continue;
		}
	}
	if (true != true) {
		int ec;
		for (ec = 15; ec > 0; ec--) {
			continue;
		}
	}
	return 3519;
}

double srpivzm::zycahysglm(double espriz, double bznlhzvv, double puduhwqlvmaqef) {
	return 8058;
}

void srpivzm::glwdetbpsmxkbyvyvoyxlgelb(int tcocmcvbdvmew, bool todnaxxg) {
	bool dffscuosz = true;
	int mggjuq = 239;
	int hrcifthmsbhuyui = 3074;
	double cxzfbzfghpldfyk = 10152;
	bool fzbtbdmmv = true;
	if (10152 != 10152) {
		int virgt;
		for (virgt = 58; virgt > 0; virgt--) {
			continue;
		}
	}
	if (true != true) {
		int miuqug;
		for (miuqug = 28; miuqug > 0; miuqug--) {
			continue;
		}
	}
	if (3074 != 3074) {
		int pdvy;
		for (pdvy = 32; pdvy > 0; pdvy--) {
			continue;
		}
	}
	if (true != true) {
		int cavm;
		for (cavm = 82; cavm > 0; cavm--) {
			continue;
		}
	}
	if (true != true) {
		int eaydhrb;
		for (eaydhrb = 3; eaydhrb > 0; eaydhrb--) {
			continue;
		}
	}

}

void srpivzm::lgezcbogbhwennlibn(int zjygelang, string pnndhjlznka, string oygaerlyvs, double wsgnuwiq, double rykjitixsrstamc) {
	bool kgmyba = true;
	bool yfzzvonww = false;
	string wjxkqluzzi = "rkhwmiargtacdsmrhqgkmkioadv";
	bool pjjijnzkel = false;
	string jmurkk = "edcmanpezjafptirebqllpaqtvgjsmmsbyvqhtnoiiebcxcbcricxtivmzicesizboaysovnomlhpmabppvwrnzoznluizo";
	bool kttvkt = false;
	int htelxsypfepcoq = 2020;
	bool jfjjxojbwimx = true;
	int fayhighuh = 2862;
	if (2862 != 2862) {
		int dlspumkxe;
		for (dlspumkxe = 99; dlspumkxe > 0; dlspumkxe--) {
			continue;
		}
	}

}

string srpivzm::fifsenafilcc(bool phzmbpbe, double injnl, int jimkjqduemetfgu, int zeqewmljmocef, string quozmi, bool pmzekapc, int uexrbybghki) {
	double rbayskjgcwxi = 17760;
	return string("");
}

double srpivzm::qiczruicojwk(bool alnxlkeyad) {
	double wfhwdkeachcjsb = 86309;
	bool zljkdjzgczhh = false;
	string rdfyyo = "dkftm";
	if (string("dkftm") != string("dkftm")) {
		int mneldfcez;
		for (mneldfcez = 83; mneldfcez > 0; mneldfcez--) {
			continue;
		}
	}
	if (86309 != 86309) {
		int gfgixbxl;
		for (gfgixbxl = 13; gfgixbxl > 0; gfgixbxl--) {
			continue;
		}
	}
	if (false == false) {
		int fad;
		for (fad = 1; fad > 0; fad--) {
			continue;
		}
	}
	if (string("dkftm") != string("dkftm")) {
		int mkjgkwip;
		for (mkjgkwip = 7; mkjgkwip > 0; mkjgkwip--) {
			continue;
		}
	}
	return 36393;
}

int srpivzm::cjpxropyqwhrjcmr(int egmwrvtbayjf, int hahijzvzda, int zrlyk) {
	string ctutawezsocx = "aaxvcdnilgtzagfxnzqaryhshvzdbjdjgvnyasoewgtrxmicctgltoywdrqfpsfrystfvzlcwjq";
	int ypdgf = 7227;
	double euwrgypf = 21447;
	if (21447 == 21447) {
		int drzmnsip;
		for (drzmnsip = 75; drzmnsip > 0; drzmnsip--) {
			continue;
		}
	}
	if (21447 == 21447) {
		int duvyau;
		for (duvyau = 99; duvyau > 0; duvyau--) {
			continue;
		}
	}
	return 11657;
}

srpivzm::srpivzm() {
	this->psamdddplojuwnf(string("iamwneqzcpmwgtjhzmholtgoxapyzpnwuxrhavrmrmw"), false, 17149, 3772);
	this->vhwrjuqdyuysmuzneiaxmxx(148, string("vwfizfsdxgusjkdxxdfstwpolcgtathsaeh"), 19399);
	this->yltvpygsbk();
	this->zycahysglm(24297, 15766, 23203);
	this->glwdetbpsmxkbyvyvoyxlgelb(307, true);
	this->lgezcbogbhwennlibn(1040, string("dmbcpmmjjpciwltfjkcnsjbbkcoqtebemgqzdpsqvsucy"), string("enhoqgvnpfckjdktgunndtnpfglikeonihuymdqoxnjvmgpfp"), 17083, 12548);
	this->fifsenafilcc(false, 3871, 2525, 853, string("mulqawsfrfsmyrxullypnbuldbmobijbmtijfhtvpcjtdllfjqc"), true, 2126);
	this->qiczruicojwk(false);
	this->cjpxropyqwhrjcmr(5421, 615, 2922);
	this->qqjipwjcwiozgouxso(string("nwquctlsmzguvndtmlgewxqexoundqpamevqocwctcvszfkxh"));
	this->texjichmjmctwoswajniuuwfp(true, 51934, 1935, 4950, 583);
	this->atxindgvsagmqms(4378, 51386, 18005, true);
	this->uqlosbzkufofhpctohjvheer(46502);
	this->wmiuapzgmjzghsbononzzbwca(4627, 16523);
	this->mfgzhsuoteavwwekeh(787, false, 71234, 2828, 75861, false);
	this->esmaedckhtljl(45733, string("utjectytyqygowtoxjidfhewkbv"), 260, 5757, true, 29095, string("ahvrmdcgaqfkcibzducnqzlbubzivypgmxdgxdfujxhrmqobelzipxcvpcihzmozxfrhfgstbhr"), 7369, 2933, 2803);
	this->cqxfcbxpgyyy(true, false);
	this->mbmqbmhqwsqgfbmzp(1578, false, 5620, false, string("paihlonx"));
	this->tneojekviutezorspcim(string("ornzbjvxsgykuqkotsbscrvhygupnll"), 4905, 6422, 70478, false, string("pbujibaywtdittvjuehbfoiiwzihkthftixvymsqjpxexjqw"), false);
	this->dwvcclilhjyuznqspojuimq(string("xkdmwufwucpwwcezffymihgwvpoowbbkwmidbvtfapdxbpakllueolwnoykebivsrprrrnehmvuviloadiogwvtawowbglkxwmu"));
	this->kqbeoiuehrbnuulnssxajwrss();
	this->jokejdeeoxltfrlqu(571, 31869);
	this->oqafwvbtrhcliduxv(3263, 9606, true, true, true, string("izrxhlijdjwvlczzwmpftpjwimtcnqpcjsvxgkfcfmserzddiubkliyxcrgxjomxzhysgluoucqgqjmwxdby"), string("zpntmhwexhnpsonbhvhzhrotqvpbgpumoooihbvcmchhjuumxpficbxxqckajqxxnvcd"));
	this->emwmhhioybzfbcbvcgbtjghd(66324, 1526, true, 2078, 898, string("tpzuwrasoxvjmchuscfxoqkalaxcgnomfmxrepaisbbxanlmoimetyukjqjjjztamrwlzlzmdodxuiwecxgqtdsjoodwqbaa"), 24797, 16597);
	this->fjsqhzlfdwnltjkcqrhkxz(1075, string("ucxdulmxmydmlmuszqrmmlcgalwbcqaerjiaftnxiorsumn"), 3082, false, string("ytiylnjhxyjvrhenywxybbjotmchfvfimuecjefyskhlviqyjyqhizczbaidvet"));
	this->saggyeocgtbohhitcx(773, 2133, true, 49165, 8077);
	this->bzvbtuyvhhraunteamcilghkm(5814, string("bhymeaezjgeksqaafhrjjfsyzbnpkpztquvmmrosqjtjluoraovanldpiigprvlkkj"), string("oxocwyveffbfhdgegzdbtipzhftqegacnskwlnrlqgzuixjraorrepfmibspsmwu"), true, 1462, 6629, true, 2128);
	this->ffcdjwxcdebtnuoxvqlsemdge(500, false, 6720);
}
