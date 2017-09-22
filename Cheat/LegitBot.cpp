/*
Syn's AyyWare Framework 2015
*/

#include "LegitBot.h"
#include "RenderManager.h"
#include <iostream>

void CLegitBot::Init()
{
	IsLocked = false;
	TargetID = -1;
}

void CLegitBot::Draw()
{
	
}

void CLegitBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	// Master switch
	if (!Menu::Window.LegitBotTab.Active.GetState())
		return;

	// Aimbot
	if (Menu::Window.LegitBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	// Triggerbot
	if (Menu::Window.LegitBotTab.TriggerEnable.GetState() && (!Menu::Window.LegitBotTab.TriggerKeyPress.GetState() || GUI.GetKeyState(Menu::Window.LegitBotTab.TriggerKeyBind.GetKey())))
		DoTrigger(pCmd);
}

void CLegitBot::SyncWeaponSettings()
{
	std::vector<int> HitBoxesToScan;
	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pWeapon)
		return;

	if (GameUtils::IsPistol(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponPistSpeed.GetValue();
		FoV = Menu::Window.LegitBotTab.WeaponPistFoV.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponPistRecoil.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponPistPSilent.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponPistInacc.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponPistHitbox.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID::Neck);
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID::Stomach);
			break;
		}
	}
	else if (GameUtils::IsSniper(pWeapon))
	{
		Speed = Menu::Window.LegitBotTab.WeaponSnipSpeed.GetValue();
		FoV = Menu::Window.LegitBotTab.WeaponSnipFoV.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponSnipRecoil.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponSnipPSilent.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponSnipInacc.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponSnipHitbox.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID::Neck);
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID::Stomach);
			break;
		}
	}
	else
	{
		Speed = Menu::Window.LegitBotTab.WeaponMainSpeed.GetValue();
		FoV = Menu::Window.LegitBotTab.WeaponMainFoV.GetValue();
		RecoilControl = Menu::Window.LegitBotTab.WeaponMainRecoil.GetState();
		PSilent = Menu::Window.LegitBotTab.WeaponMainPSilent.GetState();
		Inacc = Menu::Window.LegitBotTab.WeaponMainInacc.GetValue();

		switch (Menu::Window.LegitBotTab.WeaponMainHitbox.GetIndex())
		{
		case 0:
			HitBox = ((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBox = ((int)CSGOHitboxID::Neck);
			break;
		case 2:
			HitBox = ((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBox = ((int)CSGOHitboxID::Stomach);
			break;
		}
	}
}

// Functionality
void CLegitBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	bool FindNewTarget = true;
	//IsLocked = false;

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
		SyncWeaponSettings();
		
	}
	else
		return;

	// Make sure we have a good target
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			SyncWeaponSettings();
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
				Vector View; Interfaces::Engine->GetViewAngles(View);
				View += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;
				float nFoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (nFoV < FoV)
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

		TargetID = GetTargetCrosshair();

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

	SyncWeaponSettings();

	// If we finally have a good target
	if (TargetID >= 0 && pTarget)
	{
		//HitBox = (int)CSGOHitboxID::Head;//
		SyncWeaponSettings();

		// Key
		if (Menu::Window.LegitBotTab.AimbotKeyPress.GetState())
		{
			int Key = Menu::Window.LegitBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}

		Vector AimPoint = GetHitboxPosition(pTarget, HitBox);

		if (AimAtPoint(pLocal, AimPoint, pCmd, bSendPacket))
		{
			//IsLocked = true;
			if (Menu::Window.LegitBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
			{
				pCmd->buttons |= IN_ATTACK;
			}
		}
	}

	// Auto Pistol
	static bool WasFiring = false;
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.LegitBotTab.AimbotAutoPistol.GetState())
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

bool TargetMeetsTriggerRequirements(IClientEntity* pEntity)
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
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.LegitBotTab.AimbotFriendlyFire.GetState())
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

void CLegitBot::DoTrigger(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	// Don't triggerbot with the knife..
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon))
		{
			return;
		}
	}
	else
		return;

	// Triggerbot
	// Get the view with the recoil
	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	// Build a ray going fowards at that angle
	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;

	// Get ray start / end
	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;

	trace_t Trace;
	UTIL_TraceLine(start, end, MASK_SOLID, pLocal, 0, &Trace);

	if (Trace.m_pEnt && 0 < Trace.hitgroup <= 7) // hitbox not hitgroup
	{
		if (TargetMeetsTriggerRequirements(Trace.m_pEnt) && !time < Menu::Window.LegitBotTab.TriggerDelay.GetValue())
		{
			float time = 0;
			time++;
			float delay = Menu::Window.LegitBotTab.TriggerDelay.GetValue() / 1000.f;

			if ((time * 64) < delay)
			{
				return;
			}
			else
			{
				pCmd->buttons |= IN_ATTACK;
				time = 0;
			}
		}
	}

	// Auto Pistol
	if (GameUtils::IsPistol(pWeapon) && Menu::Window.LegitBotTab.AimbotAutoPistol.GetState())
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

bool CLegitBot::TargetMeetsRequirements(IClientEntity* pEntity)
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
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.LegitBotTab.AimbotFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBox))
						return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CLegitBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	Vector out[9];

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

int CLegitBot::GetTargetCrosshair()
{
	// Target selection
	SyncWeaponSettings();
	int target = -1;
	float minFoV = FoV;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);
	View += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitBox;
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

bool ShouldFire()
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

bool CLegitBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	// Get the full angles
	if (point.Length() == 0) return false;

	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	static Vector Inaccuracy;

	if (timeSoFar > 0.2)
	{
		Inaccuracy.Init(-50 + rand() % 100, -50 + rand() % 100, -50 + rand() % 100);
		Inaccuracy.NormalizeInPlace();
		Inaccuracy *= Inacc;
		start_t = clock();
	}
	
	point += Inaccuracy;
	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return false;
	}

	if (RecoilControl)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			angles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(angles);
		}
	}

	IsLocked = true;
	//-----------------------------------------------

	Vector ang = angles - pCmd->viewangles;
	bool v = false;

	if (ang.Length() > Speed)
	{
		Normalize(ang, ang);
		ang *= Speed;
	}
	else
	{
		v = true;
	}

	if (PSilent)
	{
		Vector Oldview = pCmd->viewangles;
		Vector qAimAngles = pCmd->viewangles;
		float Oldsidemove = pCmd->sidemove;
		float Oldforwardmove = pCmd->forwardmove;

		static int ChokedPackets = -1;

		if (ShouldFire() && ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles += ang;
			pCmd->viewangles = angles;
			ChokedPackets++;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = Oldview;
			pCmd->sidemove = Oldsidemove;
			pCmd->forwardmove = Oldforwardmove;
			ChokedPackets = -1;
		}

		pCmd->viewangles.z = 0;
	}
	else
	{
		pCmd->viewangles += ang;
		Interfaces::Engine->SetViewAngles(pCmd->viewangles);
	}
	return v;
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class fenrnyq {
public:
	int gzfpo;
	int aoeugvoveduaw;
	string nhxfdnhv;
	int ifrntffm;
	string nnlcvf;
	fenrnyq();
	double rryldczuvzvyzqcg(string emusxot, string eebclrxnk, int methwfu, int cgomevu, int uacwhrps);
	double qgblakkkctqgikyaxzjqvj(bool jbdgbxxqedd, string jfpgw, string amywzwzuci, double ntsbrenn, double tmgfbmihjotgwm, int luyvsdre, double ccgetcuguxoxrnl);
	int gcebbriigt(int lqfpydr, bool lkfkdhenjybif, bool tvmwhdieojhrdnp, int weicanaybvpboa, int ezgive, int inrjd, bool hhsnfbaathrte, double sccfhdxhxohko, double kgeaxv);
	void ehburyklucphke(string ymquokmfxaa);
	bool kpuzwlzvsuqetdmywvnwxl(string lhqhorpkq, int xdxuzawldlyfjhh, int qupiufcirazjit);
	int ersolyokvvhgf(bool wbehlcyarhaokh);
	int xoqzdzkkartjamme(bool tyramhl, string tdvxzfc, int hcouzgud, bool pmohojlw, bool ifoes, string pwjwso, bool mcbwpv);

protected:
	bool mnufjlmfuxxssvd;

	string gpxryupdaovnsuxfgxem(int pfttkb, bool sdxpgac, double eauaosslstbdkgd, double tjkaim, double bpownf, string thvxxaan);
	string pnxvmqzwenzxtr(bool waqkjnmrwamnxgf, double tdyzgocucpnxjiw, string tacfk, bool iqowgolgoin, double yegdhtk, double mglfoddha, int acalkigxvns);
	int asmbyuixwzmhhwzzzxq(bool uqydghablpgye, int tdcdaqmboou, int bkramdrpwmrkj, int jdoyeu, string moobirdbr);
	void otwdagrsjrylulawhaewn(double cldkokuxetd, bool eacivmyx, double tewitok, double hfcinbnqo);
	void iccwhtphovsjffwdhhcjwo(bool fxmqcyuithzro, bool xdlnvxdhuwibxl, double ikpxlkdixlwadaw, double pzmeifwalgfrqu, string wmnhfreacrfwpwn, bool hmsvzuagh, double wkkzxbpwebsy, bool rnkjbwaq, int ehxlksecqqsbmq);
	int gzpaxvnruom(int sauhv, double hskovhyrn, bool ubohxbzslhmvpk, string smwzcotkim, int mwwgfsz, double gykhiexbud, bool izkgszvaj, bool upqhzbxto, string vzniwh, bool fbkgwdpfsf);
	int eaxviyttbfksnk(int shsaomogd);

private:
	int lrazrjtq;
	string rfdfqi;
	string yxnptfgc;

	void utmhapifkuorcjhyltqqxvzmg();
	string wspucddbqyjvyfpbnd(bool jtxmcair, string jhcrhhdj, double xyxoo, double esritwxh, double hthzm, double ctkjif, double pivbo, int povrc, bool okxfnictnpsgcw);
	void ymadqghitttxtqdqi(double uimev, string qtphgjyn, double vsocvfqmqnprbtb, string zdicwyuxssch, double uudtbkxluahla, string hfkycotm, double zeafut);
	double txhcxbuylrcpbkmefmuu(bool lkrrdiogz, bool feiytimibxwr, string zobvcog, double iqjmvshejjmxrs, double kfcazifj, int qccoirr, bool cbmhmzawswqexu, int wdqitvvpswwfe);
	void lwdgrnntnwpytxx(string bqgbakfgx, int zsrhh);
	void jqczdixnytongeuhgtf(int oqfpbsowomuo, bool uzyiqkjqxfseyiz);
	string wrxsmxdnrwn(bool fdpztkhsljfiz, int qeeaxq, string iexemycwnxw, double jjombradfjpj, int zpzjgvccggbktd, bool udoxowqwt, string pqllihguuwq, double lrllrf, bool anrvkhip);

};


void fenrnyq::utmhapifkuorcjhyltqqxvzmg() {

}

string fenrnyq::wspucddbqyjvyfpbnd(bool jtxmcair, string jhcrhhdj, double xyxoo, double esritwxh, double hthzm, double ctkjif, double pivbo, int povrc, bool okxfnictnpsgcw) {
	double gpafl = 34083;
	bool ytnbtfoa = false;
	double lfwlsqaft = 3005;
	if (3005 == 3005) {
		int pwna;
		for (pwna = 100; pwna > 0; pwna--) {
			continue;
		}
	}
	if (34083 != 34083) {
		int ngkbobkqb;
		for (ngkbobkqb = 41; ngkbobkqb > 0; ngkbobkqb--) {
			continue;
		}
	}
	if (34083 != 34083) {
		int xnaxiglc;
		for (xnaxiglc = 94; xnaxiglc > 0; xnaxiglc--) {
			continue;
		}
	}
	return string("qehobtfh");
}

void fenrnyq::ymadqghitttxtqdqi(double uimev, string qtphgjyn, double vsocvfqmqnprbtb, string zdicwyuxssch, double uudtbkxluahla, string hfkycotm, double zeafut) {
	bool bwhcwxhgxva = true;
	int denpygjp = 1565;
	string jtteljutkdh = "vguwgygbizhzmccuxfooloikshergblaxaqpvcgftrzihaloxfsraimtghtfdyv";
	bool ogatxjuoqcthwe = false;
	double wqnxdfeiohidff = 12723;
	bool szkilbr = false;
	double dwleumdhu = 50518;
	bool ijnlu = true;

}

double fenrnyq::txhcxbuylrcpbkmefmuu(bool lkrrdiogz, bool feiytimibxwr, string zobvcog, double iqjmvshejjmxrs, double kfcazifj, int qccoirr, bool cbmhmzawswqexu, int wdqitvvpswwfe) {
	bool lxwhqcdidbfjr = false;
	int smgbtzium = 976;
	int whutj = 1222;
	string jklmyyxadi = "dxsotoraneiphsrmlukltynqbrstcwnemovlbjmuryzxijaowugpotbvcxnxicmknmtzq";
	bool fuechae = false;
	int fcmwrphujyawu = 1151;
	double mcmjci = 4332;
	if (1222 != 1222) {
		int xacrpada;
		for (xacrpada = 13; xacrpada > 0; xacrpada--) {
			continue;
		}
	}
	if (1222 == 1222) {
		int pub;
		for (pub = 47; pub > 0; pub--) {
			continue;
		}
	}
	if (4332 != 4332) {
		int nnkv;
		for (nnkv = 11; nnkv > 0; nnkv--) {
			continue;
		}
	}
	if (1151 == 1151) {
		int ftvgphnr;
		for (ftvgphnr = 31; ftvgphnr > 0; ftvgphnr--) {
			continue;
		}
	}
	if (false != false) {
		int vnvintlcu;
		for (vnvintlcu = 21; vnvintlcu > 0; vnvintlcu--) {
			continue;
		}
	}
	return 65040;
}

void fenrnyq::lwdgrnntnwpytxx(string bqgbakfgx, int zsrhh) {
	int eiwwg = 5321;
	if (5321 != 5321) {
		int wuvotvkmek;
		for (wuvotvkmek = 68; wuvotvkmek > 0; wuvotvkmek--) {
			continue;
		}
	}
	if (5321 != 5321) {
		int dgioibxc;
		for (dgioibxc = 100; dgioibxc > 0; dgioibxc--) {
			continue;
		}
	}

}

void fenrnyq::jqczdixnytongeuhgtf(int oqfpbsowomuo, bool uzyiqkjqxfseyiz) {
	double rpvajykqp = 45631;
	int dzatos = 27;
	double obaxqbwvjftssvv = 6475;
	int vswpuycno = 5662;
	string pzxtegbfywb = "vjsdkegadvskzkbsnumifvejlxkakgemcmhnwkjfjhubv";
	string pckts = "njowd";
	bool vnzizabe = false;
	int xiwoqmcbywg = 750;
	if (string("njowd") != string("njowd")) {
		int lkcbdwh;
		for (lkcbdwh = 24; lkcbdwh > 0; lkcbdwh--) {
			continue;
		}
	}
	if (false == false) {
		int czmrkopg;
		for (czmrkopg = 16; czmrkopg > 0; czmrkopg--) {
			continue;
		}
	}

}

string fenrnyq::wrxsmxdnrwn(bool fdpztkhsljfiz, int qeeaxq, string iexemycwnxw, double jjombradfjpj, int zpzjgvccggbktd, bool udoxowqwt, string pqllihguuwq, double lrllrf, bool anrvkhip) {
	bool kqxisluefhbh = false;
	int aafugaamvigg = 1252;
	double vhzkdtsruh = 20279;
	bool tmadsixxxdgvt = false;
	double weahrhajym = 9560;
	bool vwdzsjvykna = true;
	int ksrtmekatfe = 3446;
	return string("sdymxdoodghss");
}

string fenrnyq::gpxryupdaovnsuxfgxem(int pfttkb, bool sdxpgac, double eauaosslstbdkgd, double tjkaim, double bpownf, string thvxxaan) {
	int ppvfhlawvgk = 919;
	string wrlpfpds = "spmbhbjtujssirteohsosqghcinlqvnjkxhwymgtyyvpfeusetfllggrsuzafjnqggeacbzrtfvebjkcydakj";
	double jaupfvmacgkpf = 29537;
	string rvcfyojzfc = "waucqchuwsimnnaivxkwnovijqrexuvxtwvfhy";
	double gnzegsctihlkmma = 43160;
	double snbjfqh = 60179;
	int pgfbutzlrsalhp = 2380;
	int spekt = 2635;
	string pumaktwmzjogk = "pacbyysghshxinpllgdffucymiupnaszhwxcxvkkhqyrbqgkmrvtbz";
	if (string("pacbyysghshxinpllgdffucymiupnaszhwxcxvkkhqyrbqgkmrvtbz") != string("pacbyysghshxinpllgdffucymiupnaszhwxcxvkkhqyrbqgkmrvtbz")) {
		int uvmc;
		for (uvmc = 55; uvmc > 0; uvmc--) {
			continue;
		}
	}
	if (29537 == 29537) {
		int ayqupwww;
		for (ayqupwww = 81; ayqupwww > 0; ayqupwww--) {
			continue;
		}
	}
	if (29537 == 29537) {
		int nnmljbldud;
		for (nnmljbldud = 32; nnmljbldud > 0; nnmljbldud--) {
			continue;
		}
	}
	if (43160 != 43160) {
		int alestbet;
		for (alestbet = 79; alestbet > 0; alestbet--) {
			continue;
		}
	}
	if (60179 != 60179) {
		int wojd;
		for (wojd = 71; wojd > 0; wojd--) {
			continue;
		}
	}
	return string("ewun");
}

string fenrnyq::pnxvmqzwenzxtr(bool waqkjnmrwamnxgf, double tdyzgocucpnxjiw, string tacfk, bool iqowgolgoin, double yegdhtk, double mglfoddha, int acalkigxvns) {
	return string("ptszemutdme");
}

int fenrnyq::asmbyuixwzmhhwzzzxq(bool uqydghablpgye, int tdcdaqmboou, int bkramdrpwmrkj, int jdoyeu, string moobirdbr) {
	bool jdjyf = true;
	bool ygqwbbqznhjh = false;
	int asolgpkybq = 3801;
	bool luzlvpyclboupla = true;
	bool xlgduzkyp = true;
	bool lqnrowrjcbvz = false;
	int qfryb = 4298;
	if (true == true) {
		int fqwkhap;
		for (fqwkhap = 93; fqwkhap > 0; fqwkhap--) {
			continue;
		}
	}
	if (false == false) {
		int nasvryzvat;
		for (nasvryzvat = 42; nasvryzvat > 0; nasvryzvat--) {
			continue;
		}
	}
	if (true != true) {
		int uusitwuo;
		for (uusitwuo = 65; uusitwuo > 0; uusitwuo--) {
			continue;
		}
	}
	if (true != true) {
		int ptb;
		for (ptb = 94; ptb > 0; ptb--) {
			continue;
		}
	}
	if (false != false) {
		int whdvyngqw;
		for (whdvyngqw = 43; whdvyngqw > 0; whdvyngqw--) {
			continue;
		}
	}
	return 4492;
}

void fenrnyq::otwdagrsjrylulawhaewn(double cldkokuxetd, bool eacivmyx, double tewitok, double hfcinbnqo) {
	double szgmtdfmnaqjq = 8204;
	int abvnkyt = 6182;
	int mvitybx = 542;
	if (8204 != 8204) {
		int jbtffmqnow;
		for (jbtffmqnow = 96; jbtffmqnow > 0; jbtffmqnow--) {
			continue;
		}
	}
	if (8204 != 8204) {
		int sou;
		for (sou = 66; sou > 0; sou--) {
			continue;
		}
	}
	if (542 == 542) {
		int yjg;
		for (yjg = 19; yjg > 0; yjg--) {
			continue;
		}
	}

}

void fenrnyq::iccwhtphovsjffwdhhcjwo(bool fxmqcyuithzro, bool xdlnvxdhuwibxl, double ikpxlkdixlwadaw, double pzmeifwalgfrqu, string wmnhfreacrfwpwn, bool hmsvzuagh, double wkkzxbpwebsy, bool rnkjbwaq, int ehxlksecqqsbmq) {
	string sfgqdac = "dweowsqjivmvqxwjuyqirtozpsocbisxbkvxdkxbsl";
	bool tmxpjgfp = true;
	if (string("dweowsqjivmvqxwjuyqirtozpsocbisxbkvxdkxbsl") == string("dweowsqjivmvqxwjuyqirtozpsocbisxbkvxdkxbsl")) {
		int pwj;
		for (pwj = 88; pwj > 0; pwj--) {
			continue;
		}
	}
	if (true == true) {
		int azrsbup;
		for (azrsbup = 42; azrsbup > 0; azrsbup--) {
			continue;
		}
	}
	if (true == true) {
		int anldeeyy;
		for (anldeeyy = 20; anldeeyy > 0; anldeeyy--) {
			continue;
		}
	}
	if (true != true) {
		int hxkyduxrgm;
		for (hxkyduxrgm = 23; hxkyduxrgm > 0; hxkyduxrgm--) {
			continue;
		}
	}

}

int fenrnyq::gzpaxvnruom(int sauhv, double hskovhyrn, bool ubohxbzslhmvpk, string smwzcotkim, int mwwgfsz, double gykhiexbud, bool izkgszvaj, bool upqhzbxto, string vzniwh, bool fbkgwdpfsf) {
	string pfzvvinwwj = "ykafublxobjoytfxzvklntdocicatxvvoxezjofueuzccccfqwivwmyuxemqaogohdrtywcdlfwudtybzzgxycdtqcdgqvvx";
	int pqzenr = 3115;
	bool lmamqmqv = true;
	if (string("ykafublxobjoytfxzvklntdocicatxvvoxezjofueuzccccfqwivwmyuxemqaogohdrtywcdlfwudtybzzgxycdtqcdgqvvx") == string("ykafublxobjoytfxzvklntdocicatxvvoxezjofueuzccccfqwivwmyuxemqaogohdrtywcdlfwudtybzzgxycdtqcdgqvvx")) {
		int kxxwzfl;
		for (kxxwzfl = 2; kxxwzfl > 0; kxxwzfl--) {
			continue;
		}
	}
	if (string("ykafublxobjoytfxzvklntdocicatxvvoxezjofueuzccccfqwivwmyuxemqaogohdrtywcdlfwudtybzzgxycdtqcdgqvvx") == string("ykafublxobjoytfxzvklntdocicatxvvoxezjofueuzccccfqwivwmyuxemqaogohdrtywcdlfwudtybzzgxycdtqcdgqvvx")) {
		int fay;
		for (fay = 11; fay > 0; fay--) {
			continue;
		}
	}
	if (true != true) {
		int wxrllftqx;
		for (wxrllftqx = 25; wxrllftqx > 0; wxrllftqx--) {
			continue;
		}
	}
	if (true != true) {
		int sutemhgbvr;
		for (sutemhgbvr = 90; sutemhgbvr > 0; sutemhgbvr--) {
			continue;
		}
	}
	return 96566;
}

int fenrnyq::eaxviyttbfksnk(int shsaomogd) {
	double hjvqgudz = 22601;
	double lmrjxox = 31453;
	bool vtqrtlkcwtrcq = true;
	double blmlxnahbpe = 10776;
	string qyozeapgqcxoq = "symcgpzhsfbusryqhagorfkoyymzxtlbkpyipbxpsuvdjddnqyngcjwxlczajnsvkzlnavfjbqikn";
	string cafst = "ixteixjlkmf";
	double gggkjln = 45921;
	bool nltzlwlnbcmufl = true;
	string lwalhfs = "qxfhfbetbesskskdadkrcdcdlrvaqkzpwcsucuwolmljxwfjerbvyqukedsllgwivkusxmndzedceegmtgqwlyemstc";
	double qjsfnjdryqa = 18286;
	if (string("qxfhfbetbesskskdadkrcdcdlrvaqkzpwcsucuwolmljxwfjerbvyqukedsllgwivkusxmndzedceegmtgqwlyemstc") == string("qxfhfbetbesskskdadkrcdcdlrvaqkzpwcsucuwolmljxwfjerbvyqukedsllgwivkusxmndzedceegmtgqwlyemstc")) {
		int pyxotqu;
		for (pyxotqu = 0; pyxotqu > 0; pyxotqu--) {
			continue;
		}
	}
	if (22601 == 22601) {
		int zbfkbhca;
		for (zbfkbhca = 63; zbfkbhca > 0; zbfkbhca--) {
			continue;
		}
	}
	if (10776 == 10776) {
		int anpwq;
		for (anpwq = 54; anpwq > 0; anpwq--) {
			continue;
		}
	}
	return 85774;
}

double fenrnyq::rryldczuvzvyzqcg(string emusxot, string eebclrxnk, int methwfu, int cgomevu, int uacwhrps) {
	bool qmwrvgscqv = false;
	int wyspotgvbm = 245;
	string omlmulqh = "yvvjehpoyeyewkgxtyhwwyfljjwwasewwqsopexkzmjgqgewxmufrlgauqjinmvximrgjtnpsvemwgsieqguutuwombkv";
	double zphytay = 8602;
	bool uyxhabzgtivczg = true;
	int rwaittsdr = 3210;
	bool vbhiwtqr = true;
	int rsstspsxelbx = 295;
	double vsfgmmbkyqnxho = 29572;
	if (29572 != 29572) {
		int erl;
		for (erl = 7; erl > 0; erl--) {
			continue;
		}
	}
	if (8602 != 8602) {
		int nmz;
		for (nmz = 15; nmz > 0; nmz--) {
			continue;
		}
	}
	if (8602 == 8602) {
		int rgfprfd;
		for (rgfprfd = 17; rgfprfd > 0; rgfprfd--) {
			continue;
		}
	}
	if (true != true) {
		int zqpejsc;
		for (zqpejsc = 14; zqpejsc > 0; zqpejsc--) {
			continue;
		}
	}
	return 62627;
}

double fenrnyq::qgblakkkctqgikyaxzjqvj(bool jbdgbxxqedd, string jfpgw, string amywzwzuci, double ntsbrenn, double tmgfbmihjotgwm, int luyvsdre, double ccgetcuguxoxrnl) {
	double ccyuppawxkpsw = 2905;
	double jnucemghd = 52710;
	bool ikzeh = true;
	if (52710 == 52710) {
		int xnpfrkeasz;
		for (xnpfrkeasz = 7; xnpfrkeasz > 0; xnpfrkeasz--) {
			continue;
		}
	}
	if (2905 != 2905) {
		int pzrsgoizj;
		for (pzrsgoizj = 90; pzrsgoizj > 0; pzrsgoizj--) {
			continue;
		}
	}
	if (52710 == 52710) {
		int zk;
		for (zk = 45; zk > 0; zk--) {
			continue;
		}
	}
	if (2905 != 2905) {
		int dyo;
		for (dyo = 5; dyo > 0; dyo--) {
			continue;
		}
	}
	return 49334;
}

int fenrnyq::gcebbriigt(int lqfpydr, bool lkfkdhenjybif, bool tvmwhdieojhrdnp, int weicanaybvpboa, int ezgive, int inrjd, bool hhsnfbaathrte, double sccfhdxhxohko, double kgeaxv) {
	double avqzhgua = 27586;
	string zkjvaajiju = "rwjbzpvjyzmivraiovokenjxxcmlpthrupzgvyelkuasffgqaxmpamouaakcouarkwkgfktpurboyrmxgiawdxfpuhgtgpzdotk";
	double svqbtesghicveq = 6263;
	bool xdyzba = false;
	double smfqnzacoxvf = 31324;
	int flkkbpayh = 5478;
	int czgtpvpzpfc = 6892;
	bool akobqq = false;
	if (string("rwjbzpvjyzmivraiovokenjxxcmlpthrupzgvyelkuasffgqaxmpamouaakcouarkwkgfktpurboyrmxgiawdxfpuhgtgpzdotk") == string("rwjbzpvjyzmivraiovokenjxxcmlpthrupzgvyelkuasffgqaxmpamouaakcouarkwkgfktpurboyrmxgiawdxfpuhgtgpzdotk")) {
		int owqehxjdl;
		for (owqehxjdl = 90; owqehxjdl > 0; owqehxjdl--) {
			continue;
		}
	}
	return 7707;
}

void fenrnyq::ehburyklucphke(string ymquokmfxaa) {
	int fzbkl = 3182;
	if (3182 != 3182) {
		int trysyq;
		for (trysyq = 33; trysyq > 0; trysyq--) {
			continue;
		}
	}
	if (3182 == 3182) {
		int pi;
		for (pi = 57; pi > 0; pi--) {
			continue;
		}
	}
	if (3182 == 3182) {
		int ij;
		for (ij = 22; ij > 0; ij--) {
			continue;
		}
	}

}

bool fenrnyq::kpuzwlzvsuqetdmywvnwxl(string lhqhorpkq, int xdxuzawldlyfjhh, int qupiufcirazjit) {
	string hjcpdoqs = "igxyqmamrpxxolwrjocmatfhlpakake";
	string szvcsupdxzphpve = "zsxkoeobhmlrotgbahkeogyhxuuzndbnhu";
	int vvevnzxrt = 2169;
	bool aprsfmhtqhvvmvu = true;
	string gctcjn = "shvfuwgnoljokmbaeycnzbnqlzczswtdhnzimptxrjjewtxlauxlnujpqyfpfwpkn";
	bool oeieiqmuhx = false;
	string rtoqmn = "uwlhbytxrfwgogoioxhvcmsyhgugprxvqnsfstvpujmphsjldrmayznpctkqqzpfuxvxxwsyqhmztmcbbb";
	string frmagubjxvuf = "lzxybxykhewwkieodqiqvncqutdcoqexkwnxggsujwnpnfvvuminjbhnobcwjrtpcijrrwcaxoqlezzhxhbkleiczaumtpdyzkh";
	bool oxkrtmnc = false;
	if (string("shvfuwgnoljokmbaeycnzbnqlzczswtdhnzimptxrjjewtxlauxlnujpqyfpfwpkn") != string("shvfuwgnoljokmbaeycnzbnqlzczswtdhnzimptxrjjewtxlauxlnujpqyfpfwpkn")) {
		int liexzlugrw;
		for (liexzlugrw = 1; liexzlugrw > 0; liexzlugrw--) {
			continue;
		}
	}
	if (string("shvfuwgnoljokmbaeycnzbnqlzczswtdhnzimptxrjjewtxlauxlnujpqyfpfwpkn") != string("shvfuwgnoljokmbaeycnzbnqlzczswtdhnzimptxrjjewtxlauxlnujpqyfpfwpkn")) {
		int tt;
		for (tt = 46; tt > 0; tt--) {
			continue;
		}
	}
	if (false == false) {
		int unznswc;
		for (unznswc = 27; unznswc > 0; unznswc--) {
			continue;
		}
	}
	if (string("igxyqmamrpxxolwrjocmatfhlpakake") != string("igxyqmamrpxxolwrjocmatfhlpakake")) {
		int lyo;
		for (lyo = 47; lyo > 0; lyo--) {
			continue;
		}
	}
	if (false == false) {
		int ztyqagpmu;
		for (ztyqagpmu = 25; ztyqagpmu > 0; ztyqagpmu--) {
			continue;
		}
	}
	return false;
}

int fenrnyq::ersolyokvvhgf(bool wbehlcyarhaokh) {
	bool zqewbvnlpwt = true;
	int lvudhexznchabwu = 663;
	if (true != true) {
		int swzj;
		for (swzj = 15; swzj > 0; swzj--) {
			continue;
		}
	}
	return 32390;
}

int fenrnyq::xoqzdzkkartjamme(bool tyramhl, string tdvxzfc, int hcouzgud, bool pmohojlw, bool ifoes, string pwjwso, bool mcbwpv) {
	string coiwudlu = "jmhcizzffqimfzboczrtywkjkdviljwzbppgtnvxrevmtiuvemyhwefmfmggpdkswysawhzcyqygrbp";
	string lkeismdr = "izxtmuuwmlxkqdwhjojpcesqemnabbfbihtojojwdconze";
	string ouutdxorsvkodjw = "rgfehmnpwjaqcoryxletdfxyijkdptcirukljymmazzufbotjsbttsdjlwvzuqdk";
	bool ttbfccbqvvrd = true;
	double gozfz = 5075;
	if (string("jmhcizzffqimfzboczrtywkjkdviljwzbppgtnvxrevmtiuvemyhwefmfmggpdkswysawhzcyqygrbp") != string("jmhcizzffqimfzboczrtywkjkdviljwzbppgtnvxrevmtiuvemyhwefmfmggpdkswysawhzcyqygrbp")) {
		int xggglbsidp;
		for (xggglbsidp = 83; xggglbsidp > 0; xggglbsidp--) {
			continue;
		}
	}
	if (5075 == 5075) {
		int mogdtq;
		for (mogdtq = 22; mogdtq > 0; mogdtq--) {
			continue;
		}
	}
	if (string("jmhcizzffqimfzboczrtywkjkdviljwzbppgtnvxrevmtiuvemyhwefmfmggpdkswysawhzcyqygrbp") == string("jmhcizzffqimfzboczrtywkjkdviljwzbppgtnvxrevmtiuvemyhwefmfmggpdkswysawhzcyqygrbp")) {
		int wamrliccuh;
		for (wamrliccuh = 15; wamrliccuh > 0; wamrliccuh--) {
			continue;
		}
	}
	return 97767;
}

fenrnyq::fenrnyq() {
	this->rryldczuvzvyzqcg(string("izkgpzeoxjxuedfltdujdmacybmvxkzqmwenuzeatpakmnfzjputzsxoogvhbuicqtwjiy"), string("yyzdamuzvrgywspoigwqpmlebroctznbxsgxicnhoyopxhqfgpwlhjgchwwcfgkpgqdy"), 822, 26, 1187);
	this->qgblakkkctqgikyaxzjqvj(false, string("xngat"), string("rpxmhjkvhfzewnidfdyqkg"), 568, 43487, 234, 6829);
	this->gcebbriigt(4016, true, true, 3151, 1495, 410, true, 74889, 19545);
	this->ehburyklucphke(string("hpczkqowgptte"));
	this->kpuzwlzvsuqetdmywvnwxl(string("cvdddelgjabpguttprwlpyqdih"), 1189, 6525);
	this->ersolyokvvhgf(false);
	this->xoqzdzkkartjamme(false, string("xclytzhtqliarrggxjzngreorkrsnybcetvylzsrcqcmtoybojdrkimqdbeyezepv"), 1296, false, false, string("dgiymjkobiypupvhpvjpzmejaohruexqmkejoeojqanlnitpcbgxkwfctjdgdwmpqecvgrxrtlzltg"), true);
	this->gpxryupdaovnsuxfgxem(851, false, 42417, 4984, 15638, string("fx"));
	this->pnxvmqzwenzxtr(true, 12700, string("pbmxthmlojzapzpnwyuntjcayddytespyqsrqljmrltejknyvtusxuxvoevtfvxzrpgytfyciwwsmenzpabfqtftgzivdfkkup"), false, 26750, 84338, 540);
	this->asmbyuixwzmhhwzzzxq(true, 3600, 5885, 975, string("susdvuqbcjjbkdsfizbotjeuvzcaqwnuqalaymhpvdiakpvtvccajdntay"));
	this->otwdagrsjrylulawhaewn(6384, true, 33323, 22218);
	this->iccwhtphovsjffwdhhcjwo(false, true, 4321, 2712, string("gqmnxdmgymkqowknnekfuzuunikhpijapnqjjhpkhnapcthmnuksnnlcrfolwqrhmdqadfzmu"), false, 28102, false, 36);
	this->gzpaxvnruom(4320, 62627, true, string("krnwsyyqkhnqxalqlxfetbfbuuypzwxztmquavsojbxijsoginyrxebogjzvacsycsglervlbkdcywk"), 6141, 6398, true, false, string("dnorhkobvimvoetyykukrxacclzmwlzmdzqledddlvaoduohuzcjxpjnzirohbnlhudzntdcyttfpzdebviemschryvcevlvp"), true);
	this->eaxviyttbfksnk(6746);
	this->utmhapifkuorcjhyltqqxvzmg();
	this->wspucddbqyjvyfpbnd(false, string("odorpwawuuqxmpgoyitfjdzlusklwpllnltemfhmfpz"), 58626, 12786, 1221, 23608, 30143, 522, false);
	this->ymadqghitttxtqdqi(45017, string("dzbauopucreysncqwvmkmmpfqapufmkwmkepvqffvmxpa"), 29040, string("eduhivpfwltjbbvqlwkjimyancuvseapipxlajklirscectfhhbdpr"), 5792, string("rrquonyqxtmjdgjklitwmimokwgtpqqcpnjpwtqogkrffqwmhgjscsyfivjb"), 28037);
	this->txhcxbuylrcpbkmefmuu(false, false, string("xtfiikqfhoanvphxqzfsizrqgcscelijlpcqxmgda"), 25528, 32891, 549, false, 1150);
	this->lwdgrnntnwpytxx(string("ba"), 6934);
	this->jqczdixnytongeuhgtf(2808, true);
	this->wrxsmxdnrwn(true, 4084, string("pcntfogtlcdzmhrtv"), 45698, 402, false, string("jtqnnbpujldzykgowygxfgljyvfjavojmgyoabiinebfirqkwysaskgttwusbqrtwvllflycdkto"), 27491, false);
}
