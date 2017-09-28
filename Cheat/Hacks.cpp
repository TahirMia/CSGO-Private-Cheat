/*
Syn's Payhake Framework for Insanity & Razor
*/

#define _CRT_SECURE_NO_WARNINGS

#include "Hacks.h"
#include "Interfaces.h"
#include "RenderManager.h"

#include "ESP.h"
#include "Visuals.h"
#include "RageBot.h"
#include "MiscHacks.h"
#include "LegitBot.h"

CEsp Esp;
CVisuals Visuals;
CMiscHacks MiscHacks;
CRageBot RageBot;
CLegitBot LegitBot;

// Initialise and register ALL hackmanager hacks in here nigga
void Hacks::SetupHacks()
{
	Esp.Init();
	Visuals.Init();
	MiscHacks.Init();
	RageBot.Init();
	LegitBot.Init();

	hackManager.RegisterHack(&Esp);
	hackManager.RegisterHack(&Visuals);
	hackManager.RegisterHack(&MiscHacks);
	hackManager.RegisterHack(&RageBot);
	hackManager.RegisterHack(&LegitBot);

	//--------------------------------
	hackManager.Ready();
}

/*void SpecList()
{
	IClientEntity *pLocal = hackManager.pLocal();

	RECT scrn = Render::GetViewport();
	int ayy = 0;

	// Loop through all active entitys
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something
		if (pEntity &&  pEntity != pLocal)
		{
			if (Interfaces::Engine->GetPlayerInfo(i, &pinfo) && !pEntity->IsAlive() && !pEntity->IsDormant())
			{
				HANDLE obs = pEntity->GetObserverTargetHandle();

				if (obs)
				{
					IClientEntity *pTarget = Interfaces::EntList->GetClientEntityFromHandle(obs);
					player_info_t pinfo2;
					if (pTarget)
					{
						if (Interfaces::Engine->GetPlayerInfo(pTarget->GetIndex(), &pinfo2))
						{
							char buf[255]; sprintf_s(buf, "%s => %s", pinfo.name, pinfo2.name);
							RECT TextSize = Render::GetTextSize(Render::Fonts::ESP, buf);
							Render::Clear(scrn.right - 260, (scrn.bottom / 2) + (16 * ayy), 260, 16, Color(0, 0, 0, 140));
							Render::Text(scrn.right - TextSize.right - 4, (scrn.bottom / 2) + (16 * ayy), pTarget->GetIndex() == pLocal->GetIndex() ? Color(240, 70, 80, 255) : Color(255, 255, 255, 255), Render::Fonts::ESP, buf);
							ayy++;
						}
					}
				}
			}
		}
	}

	Render::Outline(scrn.right - 261, (scrn.bottom / 2) - 1, 262, (16 * ayy) + 2, Color(23, 23, 23, 255));
	Render::Outline(scrn.right - 260, (scrn.bottom / 2), 260, (16 * ayy), Color(90, 90, 90, 255));
}*/

/*using PlaySoundFn = void(__stdcall*)(const char*);
extern PlaySoundFn oPlaySound;

namespace G // Global Stuff
{
	//extern bool			Aimbotting;
	//extern bool			InAntiAim;
	//extern bool			Return;
	//extern CUserCmd*	UserCmd;
	//extern HMODULE		Dll;
	extern HWND			Window;
	//extern bool			PressedKeys[256];
	//extern bool			d3dinit;
	//extern float		FOV;
	//extern int			ChamMode;
	//extern bool			SendPacket;
	//extern int			BestTarget;
}

HWND				G::Window;

PlaySoundFn oPlaySound;
void __stdcall Hooked__PlaySoundCSGO(const char* fileName)
{
	IClientEntity* pLocal = hackManager.pLocal();

	oPlaySound(fileName);

	if (Interfaces::Engine->IsInGame() || !Menu::Window.MiscTab.OtherAutoAccept.GetState())
		return;

	if (!strcmp(fileName, "weapons/hegrenade/beep.wav"))
	{
		//Accept the game
		GameUtils::IsReady = (IsReadyFn)(Offsets::Functions::dwIsReady);
		GameUtils::IsReady();

		//This will flash the CSGO window on the taskbar
		//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = G::Window;
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
		FlashWindowEx(&fi);
	}
}*/

// Only gets called in game, use a seperate draw UI call for menus in the hook
void Hacks::DrawHacks()
{
	IClientEntity *pLocal = hackManager.pLocal();

	void Hooked__PlaySoundCSGO(const char* fileName);

	// Spectator List
	/*if (Menu::Window.MiscTab.OtherSpectators.GetState())
		SpecList();*/

	// Check the master visuals switch, just to be sure
	if (!Menu::Window.VisualsTab.Active.GetState())
		return;

	if (Menu::Window.VisualsTab.OptionsCompRank.GetState() && GUI.GetKeyState(VK_TAB))
	{
		GameUtils::ServerRankRevealAll();
	}

	hackManager.Draw();
	//--------------------------------
		
}

// Game Cmd Changes
void Hacks::MoveHacks(CUserCmd *pCmd,bool &bSendPacket)
{
	Vector origView = pCmd->viewangles;
	IClientEntity *pLocal = hackManager.pLocal();
	hackManager.Move(pCmd, bSendPacket);
	// ------------------------------

	// Put it in here so it's applied AFTER the aimbot
	int AirStuckKey = Menu::Window.MiscTab.OtherAirStuck.GetKey();
	if (AirStuckKey > 0 && GUI.GetKeyState(AirStuckKey))
	{
		if (!(pCmd->buttons & IN_ATTACK))
		{
			pCmd->tick_count = INT_MAX;//0xFFFFF or 16777216
		}
	}

	int TeleportKey = Menu::Window.MiscTab.TeleportKey.GetKey();
	if (Menu::Window.MiscTab.TeleportEnable.GetState() && !Menu::Window.MiscTab.OtherSafeMode.GetState())
	{
		if (TeleportKey > 0 && GUI.GetKeyState(TeleportKey))
		{
			pCmd->viewangles.z = 9e+37;
		}
	}

}

//---------------------------------------------------------------------//
HackManager hackManager;

// Register a new hack
void HackManager::RegisterHack(CHack* hake)
{
	Hacks.push_back(hake);
	hake->Init();
}

// Draw all the hakes
void HackManager::Draw()
{
	if (!IsReady)
		return;

	// Grab the local player for drawing related hacks
	pLocalInstance = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalInstance) return;

	for (auto &hack : Hacks)
	{
		hack->Draw();
	}
}

// Handle all the move hakes
void HackManager::Move(CUserCmd *pCmd,bool &bSendPacket)
{
	if (!IsReady)
		return;

	// Grab the local player for move related hacks
	pLocalInstance = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalInstance) return;

	for (auto &hack : Hacks)
	{
		hack->Move(pCmd,bSendPacket); // 
	}
}

//---------------------------------------------------------------------//
// Other Utils and shit

// Saves hacks needing to call a bunch of virtuals to get the instance
// Saves on cycles and file size. Recycle your plastic kids
IClientEntity* HackManager::pLocal()
{
	return pLocalInstance;
}

// Makes sure none of the hacks are called in their 
// hooks until they are completely ready for use
void HackManager::Ready()
{
	IsReady = true;
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class gwmkqps {
public:
	bool hcefqbtca;
	bool cbugdwrmp;
	double khhigfmnwidlwf;
	string jeoektzwl;
	gwmkqps();
	string idgjfogbutfueclltemrzoul(int vvdplilhfdjgwbk, bool vopilt, bool gpmlcvp);
	int ocernigsjyivk(double ddufbw, int togjzitflmijam, double lbwgwnuztaoec, string ltfisbgkfrwhezl, bool huqffdbcxei, bool eezeuoltxlf, string khslvvxl);
	bool ogbmnurllbmfkposgxfokqn(double apnbzkocq, double mbscwzwnwzn, double qbcdbvfsvvccyvc);

protected:
	double stjswaa;
	double uztxdqtzrcqbcuh;
	bool pvsqmfvugmd;
	double nvpztlvi;
	string wtgiayngvchm;

	int qomjvtmkxpmwszwohzlv(int babgnwjbawrf, string fkgamicz, string kdideohbpn, bool xooiy, double exjucvsd, int ldsxow, int qaqnwnhmxfxz, bool tnhhwui, int bmyowloq);
	bool lsdaeigjfvssmykwsj(int iamabxfdgcqqcx, int wpqxpcquwfvr, int sxuelygt, bool xxqiqjbxskp, string mrixrx, int babfmgsa);
	int aqkjbkpptooealouop();
	int lwxscecdudkvohs(bool glnogmusogxwoxl, string igpgsrnpbc, string npqsidxmil, int qfvywsjq, double nqxklhalhmclvys, double pxjvxqfbthke, double qfxinsgmfextrhy, int ersspd);
	string rkxzzpioyihtoghwhdsl(string gygmukdrrqztwhy, bool ciazjimwmixjf, string kuhxpivrledauq, double kshpnevhbrqp, bool vmrtrja, bool omatunqpxddlwt, int sucsrjotmessx, bool cmvoelvyhzgeo, int webicekmrexrse);
	string spmnktctgoejymsmbaes(string ofsmbh, bool rpvsfkngxcb, int xdmzqtiid, string whpcrmqwcvp, int tvijkmb, int amputmc, int fopeubyepst);
	bool qfpibrzpoakstpknqvwtkvut(int hnfwcmonojpkfn, int mudkginhvl, bool cxntdw, int abginqsdsoj);
	int utmvvrkuhdltcrazz(string sxhfhjpqlrfz, bool poviud, string uydcgdredr);
	int oxvhszjxsgdohkrdjifjbiaji(string ayujt, bool qrhhcgcjbwdrec, double wzhspdf);
	string ddkxwyxdywmegvjyetncdab(string szblwmdhitpuo, bool mtajdzubm, string dtghuigoo, double xsrupj);

private:
	bool beppopimgvpl;
	bool iebiikbonr;
	int ogpkzevwcxwdmbn;

	string jegrteamkpcasdtik(string ffqdurjsfawhg, bool axiglphacgruxhf);
	string zeurjflwwq(int ukjcampkeuvpawl, bool wuvmutmijdois, bool bouzjtog, string kseyerbrvkqu, bool wlxwci);
	int oadhjtbzzisnqzdxlsff(string tjdoatpt, bool liwwquf, double mfcnhcokmfjdu);
	int jgznhqypzaoowgxkddf(bool vxgxrgmwfillt, bool aaysvb);

};


string gwmkqps::jegrteamkpcasdtik(string ffqdurjsfawhg, bool axiglphacgruxhf) {
	string dmjkjcwpxipbfa = "itmayzahpldwkzmdybitczycdjacpubsyhehtgoyievieiqwrkaetvx";
	double iyrbhv = 34288;
	double osujiwhgezlwac = 24549;
	if (24549 != 24549) {
		int syc;
		for (syc = 97; syc > 0; syc--) {
			continue;
		}
	}
	if (34288 == 34288) {
		int uftlx;
		for (uftlx = 100; uftlx > 0; uftlx--) {
			continue;
		}
	}
	if (string("itmayzahpldwkzmdybitczycdjacpubsyhehtgoyievieiqwrkaetvx") == string("itmayzahpldwkzmdybitczycdjacpubsyhehtgoyievieiqwrkaetvx")) {
		int drab;
		for (drab = 84; drab > 0; drab--) {
			continue;
		}
	}
	if (string("itmayzahpldwkzmdybitczycdjacpubsyhehtgoyievieiqwrkaetvx") != string("itmayzahpldwkzmdybitczycdjacpubsyhehtgoyievieiqwrkaetvx")) {
		int tabnuoncr;
		for (tabnuoncr = 53; tabnuoncr > 0; tabnuoncr--) {
			continue;
		}
	}
	return string("xjmsstoms");
}

string gwmkqps::zeurjflwwq(int ukjcampkeuvpawl, bool wuvmutmijdois, bool bouzjtog, string kseyerbrvkqu, bool wlxwci) {
	return string("vxplgxdwedbnpobmxd");
}

int gwmkqps::oadhjtbzzisnqzdxlsff(string tjdoatpt, bool liwwquf, double mfcnhcokmfjdu) {
	string wdyrwmmbwrkr = "hjecrczydzspcwoijobnbu";
	bool hlklvf = false;
	string vkkth = "gaxiujkokizgjxqemzpkviazjzxutby";
	int jnzzfwagyaohwe = 730;
	string itqezavewz = "vfzjcvbmfutrcsxzekrdninpwfprobggcrjmumgjadj";
	bool gqkllybys = true;
	double scrbqscwy = 36672;
	string ulikmtwxt = "tzcntgogbnijipfajhdffotdbtpekwafksgurpodkjmvccmqywtjzhnsh";
	bool jddopzqqjpgu = false;
	if (36672 != 36672) {
		int gd;
		for (gd = 13; gd > 0; gd--) {
			continue;
		}
	}
	if (string("tzcntgogbnijipfajhdffotdbtpekwafksgurpodkjmvccmqywtjzhnsh") != string("tzcntgogbnijipfajhdffotdbtpekwafksgurpodkjmvccmqywtjzhnsh")) {
		int oj;
		for (oj = 2; oj > 0; oj--) {
			continue;
		}
	}
	if (true != true) {
		int eaahi;
		for (eaahi = 50; eaahi > 0; eaahi--) {
			continue;
		}
	}
	if (true == true) {
		int hqnnfguuev;
		for (hqnnfguuev = 46; hqnnfguuev > 0; hqnnfguuev--) {
			continue;
		}
	}
	if (730 == 730) {
		int mhgjlombpa;
		for (mhgjlombpa = 85; mhgjlombpa > 0; mhgjlombpa--) {
			continue;
		}
	}
	return 93672;
}

int gwmkqps::jgznhqypzaoowgxkddf(bool vxgxrgmwfillt, bool aaysvb) {
	int pzvgl = 1174;
	double wiskanedc = 14360;
	string mrhjt = "qkgc";
	int gljwypd = 1810;
	bool fquqo = false;
	string rdwosimpvjnu = "tjzccikslfd";
	int pkjzxgkp = 4069;
	string qbnevcncisareru = "pyixsbksotmxzhlvwlpxjabpshyixsztvoahooqyhfhvhsamctoejdamccjbutaiaisrsawlpehzpxw";
	if (1174 != 1174) {
		int cdt;
		for (cdt = 67; cdt > 0; cdt--) {
			continue;
		}
	}
	if (1810 != 1810) {
		int nekqvyfp;
		for (nekqvyfp = 15; nekqvyfp > 0; nekqvyfp--) {
			continue;
		}
	}
	if (string("pyixsbksotmxzhlvwlpxjabpshyixsztvoahooqyhfhvhsamctoejdamccjbutaiaisrsawlpehzpxw") != string("pyixsbksotmxzhlvwlpxjabpshyixsztvoahooqyhfhvhsamctoejdamccjbutaiaisrsawlpehzpxw")) {
		int lg;
		for (lg = 6; lg > 0; lg--) {
			continue;
		}
	}
	return 32140;
}

int gwmkqps::qomjvtmkxpmwszwohzlv(int babgnwjbawrf, string fkgamicz, string kdideohbpn, bool xooiy, double exjucvsd, int ldsxow, int qaqnwnhmxfxz, bool tnhhwui, int bmyowloq) {
	double qeyonmsleabkp = 19856;
	double vjaja = 45206;
	int uinvenlzp = 3089;
	double qypnukeyx = 69993;
	string mcgfjfsrbtaotk = "rzbzufcwcbhicioqushpdnbxn";
	bool lzgxp = true;
	return 79746;
}

bool gwmkqps::lsdaeigjfvssmykwsj(int iamabxfdgcqqcx, int wpqxpcquwfvr, int sxuelygt, bool xxqiqjbxskp, string mrixrx, int babfmgsa) {
	double ygfwqijbaxwuruy = 27944;
	int escrexshvbenow = 647;
	bool cimhzwlzmpmcnjq = true;
	string yobrrmfpiybmny = "rewhodmcilyrrvypznlgkzcyyjjvqtcbzgrptatnttiqwbzofqjdcdlozou";
	bool ikvdibzrmouwjlm = true;
	bool mwmajestmy = false;
	double ypxdjh = 38068;
	double absemx = 2548;
	int dftkmp = 4525;
	int cqqezbrcaukz = 2170;
	if (2170 != 2170) {
		int llzdjvdh;
		for (llzdjvdh = 96; llzdjvdh > 0; llzdjvdh--) {
			continue;
		}
	}
	if (38068 != 38068) {
		int glg;
		for (glg = 40; glg > 0; glg--) {
			continue;
		}
	}
	if (647 != 647) {
		int kbi;
		for (kbi = 83; kbi > 0; kbi--) {
			continue;
		}
	}
	return false;
}

int gwmkqps::aqkjbkpptooealouop() {
	bool fdpzsndrxdphhml = false;
	string ojwkhbpirkpq = "uutuvofwodewfwmafoehyqskoxzcoxmprkoovvwbvkasfgkwyqxzujymeyrosvggnzkxqcthazpxzafefzhuj";
	double bpjbha = 7634;
	int cbkzvhfnre = 3927;
	return 10850;
}

int gwmkqps::lwxscecdudkvohs(bool glnogmusogxwoxl, string igpgsrnpbc, string npqsidxmil, int qfvywsjq, double nqxklhalhmclvys, double pxjvxqfbthke, double qfxinsgmfextrhy, int ersspd) {
	bool bpuwtzdhwrm = true;
	string eazczsztzwhw = "hljvetsjrwypdelgnjbmuunppogypixwknkspoqnsvtcychnmhzthhxvfeffwvepeahdzyvxtxjxzhuahuiae";
	bool kaqpnc = false;
	double ootahhcybxmp = 49566;
	bool uucpmvnqikhwov = false;
	double kepeidrkxfqjwcj = 59507;
	if (false != false) {
		int znsdzudufd;
		for (znsdzudufd = 65; znsdzudufd > 0; znsdzudufd--) {
			continue;
		}
	}
	if (59507 != 59507) {
		int zsmc;
		for (zsmc = 82; zsmc > 0; zsmc--) {
			continue;
		}
	}
	if (49566 != 49566) {
		int prba;
		for (prba = 84; prba > 0; prba--) {
			continue;
		}
	}
	return 20684;
}

string gwmkqps::rkxzzpioyihtoghwhdsl(string gygmukdrrqztwhy, bool ciazjimwmixjf, string kuhxpivrledauq, double kshpnevhbrqp, bool vmrtrja, bool omatunqpxddlwt, int sucsrjotmessx, bool cmvoelvyhzgeo, int webicekmrexrse) {
	int mjaibbdrgtw = 833;
	string cozzugsm = "duqrkxmxdwylpgvtatffznkwemdgzibdorwygqexaqjhglclagukyqgfppvqyptnng";
	int jvddmk = 1670;
	int oevenvvmd = 5117;
	double pxcagtkfdojqxu = 2520;
	bool kajsmdlp = true;
	string zjlbehyluqu = "xumumgbn";
	bool qiuxjuhptp = false;
	double saluzewarkwcak = 64955;
	if (5117 == 5117) {
		int ll;
		for (ll = 90; ll > 0; ll--) {
			continue;
		}
	}
	return string("n");
}

string gwmkqps::spmnktctgoejymsmbaes(string ofsmbh, bool rpvsfkngxcb, int xdmzqtiid, string whpcrmqwcvp, int tvijkmb, int amputmc, int fopeubyepst) {
	return string("bxp");
}

bool gwmkqps::qfpibrzpoakstpknqvwtkvut(int hnfwcmonojpkfn, int mudkginhvl, bool cxntdw, int abginqsdsoj) {
	int yjlgobhesjf = 926;
	string vkcbhi = "ivbbyieksdtgdnatuzcsbcmjtodrsynhpnxuyjhlvdjdvwwxicsvoeir";
	bool ofjpuzjcsddofjl = false;
	string czvyig = "ujalxhxeuoygc";
	string owemmnebh = "hlquobyidhjarepyozlkuikucqiz";
	bool exgmakjihzj = true;
	int airrtm = 6347;
	if (string("hlquobyidhjarepyozlkuikucqiz") != string("hlquobyidhjarepyozlkuikucqiz")) {
		int leu;
		for (leu = 49; leu > 0; leu--) {
			continue;
		}
	}
	if (string("ujalxhxeuoygc") != string("ujalxhxeuoygc")) {
		int csppbupmz;
		for (csppbupmz = 97; csppbupmz > 0; csppbupmz--) {
			continue;
		}
	}
	if (true == true) {
		int fooffodl;
		for (fooffodl = 53; fooffodl > 0; fooffodl--) {
			continue;
		}
	}
	if (6347 != 6347) {
		int sxbvqofdo;
		for (sxbvqofdo = 46; sxbvqofdo > 0; sxbvqofdo--) {
			continue;
		}
	}
	return true;
}

int gwmkqps::utmvvrkuhdltcrazz(string sxhfhjpqlrfz, bool poviud, string uydcgdredr) {
	bool liyjsg = true;
	string upsokkxojrd = "rpwqdlbodkrkwjrhrhuvcjhmypgxulgurjeeirl";
	int zxqswrrmajyxrxd = 3110;
	double yteghscwub = 45152;
	if (45152 != 45152) {
		int ew;
		for (ew = 86; ew > 0; ew--) {
			continue;
		}
	}
	if (string("rpwqdlbodkrkwjrhrhuvcjhmypgxulgurjeeirl") != string("rpwqdlbodkrkwjrhrhuvcjhmypgxulgurjeeirl")) {
		int copec;
		for (copec = 42; copec > 0; copec--) {
			continue;
		}
	}
	if (true == true) {
		int cukvz;
		for (cukvz = 78; cukvz > 0; cukvz--) {
			continue;
		}
	}
	if (string("rpwqdlbodkrkwjrhrhuvcjhmypgxulgurjeeirl") == string("rpwqdlbodkrkwjrhrhuvcjhmypgxulgurjeeirl")) {
		int mhhtx;
		for (mhhtx = 8; mhhtx > 0; mhhtx--) {
			continue;
		}
	}
	if (45152 != 45152) {
		int ypsnimp;
		for (ypsnimp = 72; ypsnimp > 0; ypsnimp--) {
			continue;
		}
	}
	return 728;
}

int gwmkqps::oxvhszjxsgdohkrdjifjbiaji(string ayujt, bool qrhhcgcjbwdrec, double wzhspdf) {
	int yplmpeuh = 3835;
	bool ukmwxusyiueu = true;
	bool mqpqzpwvj = false;
	string voanmuwmbjhhy = "xvpvstothyyzhzcfgbakcovcpzbespygvwbovmwletixqhghsfyobdutrhkyudqojvwduibfwgeoqsmnqppjthmftbvwnlgmcdeg";
	bool dxtbsa = false;
	string rpnlbqsdhuh = "xqeiylnmmuhylsrknpwvoadsodoozpgeagnxphnafzihgtpiavbzasdccsjdpdrrbfomhqzocxqnfzkmsicnixmdm";
	double norrjkpkdgnfjiz = 51126;
	int njgrvgqm = 2547;
	string durnsexuydug = "isfwavnewldtpypjkwhxhgxgymaoiglnsxbrmobpfvnldbzopojygqjstdiapyerwz";
	string hneeugjwzlg = "xymcwqfnrgrqsqkugmudocjbybuoounqfdqyiycrooj";
	if (2547 == 2547) {
		int fjyjnzful;
		for (fjyjnzful = 42; fjyjnzful > 0; fjyjnzful--) {
			continue;
		}
	}
	if (string("isfwavnewldtpypjkwhxhgxgymaoiglnsxbrmobpfvnldbzopojygqjstdiapyerwz") != string("isfwavnewldtpypjkwhxhgxgymaoiglnsxbrmobpfvnldbzopojygqjstdiapyerwz")) {
		int sycmh;
		for (sycmh = 70; sycmh > 0; sycmh--) {
			continue;
		}
	}
	if (2547 == 2547) {
		int ese;
		for (ese = 37; ese > 0; ese--) {
			continue;
		}
	}
	if (true == true) {
		int pcn;
		for (pcn = 26; pcn > 0; pcn--) {
			continue;
		}
	}
	if (false != false) {
		int xvijsjgf;
		for (xvijsjgf = 14; xvijsjgf > 0; xvijsjgf--) {
			continue;
		}
	}
	return 72459;
}

string gwmkqps::ddkxwyxdywmegvjyetncdab(string szblwmdhitpuo, bool mtajdzubm, string dtghuigoo, double xsrupj) {
	int dpcdruvyzvl = 24;
	double urcofgzkoakuc = 13813;
	bool gtujlvtvnzo = false;
	string bjvircseyisad = "ghztgojjxtnhubrmzkqhst";
	bool safen = false;
	int nqzolrwnycptfnu = 2498;
	string ojcjzu = "qleieniwidlniwdofakepolhnajujokuzwsyesoxaklzhywknzwxgctortmrzrfxxxdgubluttkvqqeywcnpamjkeffdeoas";
	bool rcksczeu = false;
	int lpgylhmo = 108;
	if (13813 != 13813) {
		int nhhfc;
		for (nhhfc = 68; nhhfc > 0; nhhfc--) {
			continue;
		}
	}
	if (false != false) {
		int lj;
		for (lj = 39; lj > 0; lj--) {
			continue;
		}
	}
	if (2498 == 2498) {
		int zkixwin;
		for (zkixwin = 84; zkixwin > 0; zkixwin--) {
			continue;
		}
	}
	if (false != false) {
		int scmu;
		for (scmu = 33; scmu > 0; scmu--) {
			continue;
		}
	}
	if (false != false) {
		int tcbothr;
		for (tcbothr = 33; tcbothr > 0; tcbothr--) {
			continue;
		}
	}
	return string("aycwvkrqujj");
}

string gwmkqps::idgjfogbutfueclltemrzoul(int vvdplilhfdjgwbk, bool vopilt, bool gpmlcvp) {
	int sndif = 1377;
	if (1377 != 1377) {
		int gdjhukmu;
		for (gdjhukmu = 76; gdjhukmu > 0; gdjhukmu--) {
			continue;
		}
	}
	if (1377 == 1377) {
		int hzwrrv;
		for (hzwrrv = 49; hzwrrv > 0; hzwrrv--) {
			continue;
		}
	}
	if (1377 != 1377) {
		int xjj;
		for (xjj = 71; xjj > 0; xjj--) {
			continue;
		}
	}
	if (1377 == 1377) {
		int treakruw;
		for (treakruw = 87; treakruw > 0; treakruw--) {
			continue;
		}
	}
	if (1377 == 1377) {
		int imqncv;
		for (imqncv = 8; imqncv > 0; imqncv--) {
			continue;
		}
	}
	return string("ovnyqfnlaqeyqjwkp");
}

int gwmkqps::ocernigsjyivk(double ddufbw, int togjzitflmijam, double lbwgwnuztaoec, string ltfisbgkfrwhezl, bool huqffdbcxei, bool eezeuoltxlf, string khslvvxl) {
	string chvpkb = "nlvgpcmeyyzmcoezlbqskaelhadju";
	int ueuzdqfddp = 1246;
	string jrmxrymzidk = "pmqbcuwmjzwdhrcfxulrufkqwhnqfegvtpgfhrbmjeqxarznjsnfdjkzehhtcvwxk";
	string tgjlcdzkakz = "wxuagzwjtzbugnpnzqu";
	bool ivdotoeayidmzn = false;
	int ahpvvkg = 8145;
	string mohyjmewgqscbs = "ryubligmyozbezojqabvrxqvnplglkuoaga";
	double jwxhcanjnveph = 22989;
	int eguxzxw = 1542;
	string orhcqmrihhcchf = "ucdbwckvrbgxgccndgcywmbnckhckcmxvvqjlbsetvd";
	if (string("pmqbcuwmjzwdhrcfxulrufkqwhnqfegvtpgfhrbmjeqxarznjsnfdjkzehhtcvwxk") == string("pmqbcuwmjzwdhrcfxulrufkqwhnqfegvtpgfhrbmjeqxarznjsnfdjkzehhtcvwxk")) {
		int ghbvzl;
		for (ghbvzl = 50; ghbvzl > 0; ghbvzl--) {
			continue;
		}
	}
	return 30389;
}

bool gwmkqps::ogbmnurllbmfkposgxfokqn(double apnbzkocq, double mbscwzwnwzn, double qbcdbvfsvvccyvc) {
	string zelyu = "urxmcywnupidtgtfwssmjiveifhldojpnoxiljowsqhttpuap";
	int exghbpiubnsqwg = 2248;
	bool mfvxh = true;
	int zsvgjitvygwep = 766;
	string qlekjxy = "sldrksssguclkoqzwmvneflccvmvxdvuskydc";
	double gkonmdagmsuslue = 3583;
	if (766 == 766) {
		int ixw;
		for (ixw = 37; ixw > 0; ixw--) {
			continue;
		}
	}
	if (3583 == 3583) {
		int ttewykihrz;
		for (ttewykihrz = 28; ttewykihrz > 0; ttewykihrz--) {
			continue;
		}
	}
	if (true == true) {
		int zx;
		for (zx = 94; zx > 0; zx--) {
			continue;
		}
	}
	if (true == true) {
		int quozayx;
		for (quozayx = 22; quozayx > 0; quozayx--) {
			continue;
		}
	}
	return true;
}

gwmkqps::gwmkqps() {
	this->idgjfogbutfueclltemrzoul(3877, false, false);
	this->ocernigsjyivk(52456, 791, 13543, string("qroygojvwckhfpacbmcxxahbesyhsuufwadikwontaaaatqmibuvklniggirsbgs"), false, true, string("nggchuyocskokgoomroybcynzgjoptfavtqsnoedhazobsdkoqbdymqywykhsewhnqssahtsadosnonzjsdvutxayhmmhad"));
	this->ogbmnurllbmfkposgxfokqn(1761, 8721, 27858);
	this->qomjvtmkxpmwszwohzlv(826, string("mtpokncnqmahkivspknecsemyknv"), string("hrcjdlvieplpiwigs"), false, 11976, 313, 1902, true, 2495);
	this->lsdaeigjfvssmykwsj(3821, 1859, 619, true, string("posvplbuvujoskxqiigejzsmdvwwczgddhauipiovpltdgyhccgyatyfk"), 789);
	this->aqkjbkpptooealouop();
	this->lwxscecdudkvohs(true, string("zblzbtri"), string("rkdgkefdakicpojiotvdasmzzjyiskszorhtjlywtndngdadpsxujboafzqprxa"), 391, 13267, 19637, 13241, 570);
	this->rkxzzpioyihtoghwhdsl(string("aqnvnsceifcjvdyvpcanfzkzbhvkohbnxlhebvrcgykyzhriykhxezdvrcvtapmuvgezerdnzmkljgzvyjbclk"), true, string("cwklutrfrdfkghoafajamdodavnehoawiq"), 8630, true, true, 390, false, 8754);
	this->spmnktctgoejymsmbaes(string("cerhdtwycujckgmzpkpcxvoikfrfklwgzoorvohifjodveaxpbqfxjkiwwmjmfryprtiwlpzodkcbugegdositouzac"), false, 5916, string("vhaly"), 5421, 2627, 2806);
	this->qfpibrzpoakstpknqvwtkvut(1641, 1525, false, 4269);
	this->utmvvrkuhdltcrazz(string("parmkpu"), true, string("pqdloezlxqffvqxlcbcquczrnynueusfwnjqrj"));
	this->oxvhszjxsgdohkrdjifjbiaji(string("mogavgwciqtryxkejypgipzmbtbprebtxrfthcxtnix"), false, 50858);
	this->ddkxwyxdywmegvjyetncdab(string("umnohjxewjgonxeipcvvyteialcxbsrfhkcoxhbyezowqgduvdrtszqigqmroszhcej"), true, string("xhwtpojxcdiuqrynkcnqpyoyasozsnkmfnfznsizslcxdrimkrxglvsgpkrawcigsdreqpvfqsqbkjyvrf"), 20062);
	this->jegrteamkpcasdtik(string("igerxavdinzpkqsibmzjbgknqhkbpcfgpgktnagavefnrkkpcqkjkimxnruigcrpjqdqpwjoitpfxmcuohp"), true);
	this->zeurjflwwq(909, false, true, string("ozrdstovsworwkziy"), false);
	this->oadhjtbzzisnqzdxlsff(string("xnxdpktgmeqzrdwvzkroo"), true, 24042);
	this->jgznhqypzaoowgxkddf(true, true);
}
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class rsiimlq {
public:
	int ohwpcmrpmfssm;
	double vahsftjkhagstk;
	double gpgjlkvpinmvq;
	rsiimlq();
	void llrlzeixuhy(double ylgxnlw, bool ukxlx, int cdfrvdmddxfbua, bool hrwmll, string xexuhnhyqsbu, string tipbraywbu, string fyhwky, int abpypwpaqw, bool lihtplrdznybyuu, int cwekwzgg);
	string stcuegqilurjkga(bool gfbqynyvbmqr, bool oszllhnowotd, int vwgtp, bool nzdhf);
	double ihqcvqnxruryndprbswucjp(double uxipu, double rcekx, int iacucim, int ssahvzrxqdgj, double etsoywtvwdynhn, bool sfnbjb, string ubzotvxohreuu, bool ockgkkqzs, string mdkcbojkrf, int hsysswlpekgbw);
	string lelaaxyljzylyqt(double dkobta, bool bbuimibqpivoez, double lgpqqnkwbw, bool cpbvubvsk, bool jvsmqhwwprvpidm, bool wmcfspteuk);
	string tnjhwphhhm(int olkszfka, string tmiiebyob, double hdtpnn, string amrgk, int nehddrwlefhiovd, int njurf, double toxxatdylyst);
	void qpdcekrlwahztylszisfnczb(string jgdejirbyujnwoi, int occfmpsxikgd, int cyfcndegxctzeps);
	bool fmfykgffzareftkjhud(double ruubukaigwdl, int ejonq, int bsisdrape, double ljgrlmzgxpmlcyj, string tzxoqnp, bool calalhldbpirm, int puqhlyba, bool arizjn, int sgbnyjm, bool kfhcsrpwqsuzl);
	void unzyqeepouwdowaaevau(int elrnvlibhvl, string cofkyyqk, double ysyrxkjrxdkd, double gzwtyedwyzqw, double pvyiua, double aignypnuemdknl, string haoignh, int zbupgnmmhoh);
	int rfzufnpolpkwadalcep(string qmkbjseiz, double rtcgdrhqn, int cvuklmsqxxjyu, double hsiunrg, double yhgpglwpukdk, double bttgshoflekleu, string piupsodooflrf, string emolltgwf, int dntgakrb);
	int ffcgufzxldmnab(bool wmkwgefxpcujsmh, string dmeinrbexvppba, string fhalvth, string aabpehahwhxvkb, double jaiegaxotyvaw, bool kpotdea);

protected:
	bool dtnrwhc;
	double mmtszohihber;

	double womnycgclshpmxatbsjdf(bool pqytaebtfsddesh, int qqhjvzvzpdjqvbz, string yjaxgmjcgoxffd, string bfzny, int vagzigdumwwkl, string kagyhcvdnjyhhty, int bgusqndevezszb, bool gckjg, double tpdklkesmhzt);
	double fhhijdyrufrpnmlxmsecyndw(int oqszs, int dkdxswswezog);
	double avndtqdjwlbbapmzcjl(double fwwsqtggowuvoex, int mvaiojpx, int hcrmryvlktfp, string cedadsusmdxgwx, string osfuulrvtvpn);
	string zmdquqsxrlufeukvmtfi(int mkvrvqdqnpoazda, bool xubjubsdekqljj);
	void uqgisiltnxonaszskbz(bool vqtcrleticvan, bool kgkgemexumpeqnx, bool lzpqfjhrb, int bhhjzri);
	void pjcewogjcvbalforgyowxglbx(int uxxwisndzla, string zvvvpauytte, string petgxgxrwurm, string uxtplicc, string pwoohoimgeyaiul, bool lhrhxmejyrl, int ifdlfcqesoid, string pcndeufsxx, int vhdwjx);
	double zivvseoeyaterfsxzhegotv(string rknxmvv, bool mbalpuwoznc, bool pikqjzlh, double qiqebdvibhdfng, string jdazr, int xqgmnvrzqmvp, bool tkxhvaa, bool pbwyucgbcxdk, double giecnxmjpgw, double uvktjknou);
	double bxpdwxznraovlyp(int axccmegt, double cfyvamyyceg, int jvmurbnogntrld, int tuirao, double rzltaxifn, double crvzmywvm, double xwchmm, string vzfrppmgdhiz, int vvdbnswtzpgrcry);

private:
	double yfoufjku;
	bool prhjxcajgo;

	void hpishujzhreii(double jxgxtzkkwbysqm, int exyfjjb, int ytfkp, double siusxlablhk, bool wknokmmk);
	int kjgdznmxch(double egivakddufa, bool xnwlhkyskvrb, double bopinjvftcph, double feeidgjj, double ymofjw, int whirvpnd, int zuhtysfhsd, double zelomf, bool vmwicmdvy, string dhregsfmfcgej);
	void ghwfiitmfzkoeflkbyd(int kkgqjbjwgtdrw, int czmioycgw, double ltwxcq, bool yljluwedv, string dskqyxzyc, bool utjbodtxhzzikm, double uqhtkeip);
	bool fswagwhpglxuwm(bool syroti, double zrmwmbxxnngi, bool pibkumna, string dcrozcax, bool areruezgtcr, double kkucmlcqbcieiy, double zacoltellsqjn, double pxgrfdb, int uyrev);
	string gsjafblpzxqfut(double wkyguazrjzl, double tmrlv, int sxzbuweaodv, bool gfthu, string lbueggp, bool quuunqu);

};


void rsiimlq::hpishujzhreii(double jxgxtzkkwbysqm, int exyfjjb, int ytfkp, double siusxlablhk, bool wknokmmk) {
	double nxbipoqvxtaki = 14685;
	bool bxdhah = false;
	bool dqtraa = false;
	string ymfrpino = "utaxkrdrwhybdjcxvzoekbvkduwxfakothujvbmgouyoqkdmldjmfb";
	bool efrizwydljk = false;
	double rlgxgp = 3438;
	bool txaeeu = false;
	string lmgrnb = "yohzsepszhdoxubtvjzosvzockqegsemgioqlneocqjmffbeajunnmgxhclvcchkxpgrfirxlgfthhxeefslnttxkewxrszaei";
	int nkxplb = 1207;
	if (false != false) {
		int sc;
		for (sc = 43; sc > 0; sc--) {
			continue;
		}
	}
	if (1207 != 1207) {
		int rv;
		for (rv = 21; rv > 0; rv--) {
			continue;
		}
	}
	if (string("yohzsepszhdoxubtvjzosvzockqegsemgioqlneocqjmffbeajunnmgxhclvcchkxpgrfirxlgfthhxeefslnttxkewxrszaei") == string("yohzsepszhdoxubtvjzosvzockqegsemgioqlneocqjmffbeajunnmgxhclvcchkxpgrfirxlgfthhxeefslnttxkewxrszaei")) {
		int biln;
		for (biln = 49; biln > 0; biln--) {
			continue;
		}
	}
	if (14685 != 14685) {
		int fodmpasel;
		for (fodmpasel = 30; fodmpasel > 0; fodmpasel--) {
			continue;
		}
	}
	if (string("utaxkrdrwhybdjcxvzoekbvkduwxfakothujvbmgouyoqkdmldjmfb") == string("utaxkrdrwhybdjcxvzoekbvkduwxfakothujvbmgouyoqkdmldjmfb")) {
		int blvhrmmqxs;
		for (blvhrmmqxs = 87; blvhrmmqxs > 0; blvhrmmqxs--) {
			continue;
		}
	}

}

int rsiimlq::kjgdznmxch(double egivakddufa, bool xnwlhkyskvrb, double bopinjvftcph, double feeidgjj, double ymofjw, int whirvpnd, int zuhtysfhsd, double zelomf, bool vmwicmdvy, string dhregsfmfcgej) {
	string qtogkawegrtgr = "qkvsvndhljebjhbtiwgjaiopgkqwtnltkstiflrcbyxiczgzqonebpxpqbwfpgfpkkwxqvz";
	string pdwpo = "iibktxtwogohqfiypsbvzdkeobonskklchuxksgbnbufiliyrlmhdswwo";
	int rqsxmxclvmbxoa = 191;
	double alizlkj = 15907;
	string iubeccdiglmv = "odurlwrssmikxrznqaqaxvsykmakidzkphkjgfznxpitqdiftjecmejifzxbiqycshtxjcwmedx";
	bool oevvjobjq = false;
	bool alotvhcyjslvztg = true;
	return 74871;
}

void rsiimlq::ghwfiitmfzkoeflkbyd(int kkgqjbjwgtdrw, int czmioycgw, double ltwxcq, bool yljluwedv, string dskqyxzyc, bool utjbodtxhzzikm, double uqhtkeip) {

}

bool rsiimlq::fswagwhpglxuwm(bool syroti, double zrmwmbxxnngi, bool pibkumna, string dcrozcax, bool areruezgtcr, double kkucmlcqbcieiy, double zacoltellsqjn, double pxgrfdb, int uyrev) {
	int bijlsmo = 6626;
	string zikftn = "cagzbzcxddvmimvozsieipfeoyoebsszomfszidzuyhuuxnir";
	double kyzurq = 47522;
	int wacavufobtrygb = 580;
	double zjmrweunwioy = 1103;
	int ilorau = 2102;
	int bubjocgousq = 858;
	string obhnnwo = "lezkavzk";
	if (580 != 580) {
		int nnqmvog;
		for (nnqmvog = 98; nnqmvog > 0; nnqmvog--) {
			continue;
		}
	}
	if (6626 == 6626) {
		int olb;
		for (olb = 16; olb > 0; olb--) {
			continue;
		}
	}
	if (580 != 580) {
		int hcjtmpx;
		for (hcjtmpx = 10; hcjtmpx > 0; hcjtmpx--) {
			continue;
		}
	}
	return false;
}

string rsiimlq::gsjafblpzxqfut(double wkyguazrjzl, double tmrlv, int sxzbuweaodv, bool gfthu, string lbueggp, bool quuunqu) {
	int lukvwzzqbexl = 1070;
	if (1070 == 1070) {
		int mdca;
		for (mdca = 56; mdca > 0; mdca--) {
			continue;
		}
	}
	return string("qtqmiduyeiy");
}

double rsiimlq::womnycgclshpmxatbsjdf(bool pqytaebtfsddesh, int qqhjvzvzpdjqvbz, string yjaxgmjcgoxffd, string bfzny, int vagzigdumwwkl, string kagyhcvdnjyhhty, int bgusqndevezszb, bool gckjg, double tpdklkesmhzt) {
	return 38706;
}

double rsiimlq::fhhijdyrufrpnmlxmsecyndw(int oqszs, int dkdxswswezog) {
	string qdkunqkqk = "dkahgfhszhlvdkbix";
	if (string("dkahgfhszhlvdkbix") == string("dkahgfhszhlvdkbix")) {
		int zkwjjql;
		for (zkwjjql = 18; zkwjjql > 0; zkwjjql--) {
			continue;
		}
	}
	return 22949;
}

double rsiimlq::avndtqdjwlbbapmzcjl(double fwwsqtggowuvoex, int mvaiojpx, int hcrmryvlktfp, string cedadsusmdxgwx, string osfuulrvtvpn) {
	return 51292;
}

string rsiimlq::zmdquqsxrlufeukvmtfi(int mkvrvqdqnpoazda, bool xubjubsdekqljj) {
	int irdmyow = 4318;
	string earasy = "yqgqlivmboplovqybajuuetuetaqvlznpwgrlgupxbavpanqrjszjnxvlqwv";
	string rwrsufsinronloi = "maeginwwdlyoke";
	bool njardps = true;
	int uvhsmeyujw = 647;
	double drbduab = 25798;
	int uhrxnc = 407;
	bool pgzohr = false;
	double zhoedoxoekver = 30613;
	if (647 != 647) {
		int lf;
		for (lf = 4; lf > 0; lf--) {
			continue;
		}
	}
	return string("d");
}

void rsiimlq::uqgisiltnxonaszskbz(bool vqtcrleticvan, bool kgkgemexumpeqnx, bool lzpqfjhrb, int bhhjzri) {
	int kklprqhk = 3084;
	bool daqgicpn = true;
	string xrjkxzo = "mbszhcyfsh";
	int jleyleg = 914;
	if (3084 == 3084) {
		int nbohdiezpq;
		for (nbohdiezpq = 6; nbohdiezpq > 0; nbohdiezpq--) {
			continue;
		}
	}
	if (string("mbszhcyfsh") == string("mbszhcyfsh")) {
		int oj;
		for (oj = 9; oj > 0; oj--) {
			continue;
		}
	}
	if (string("mbszhcyfsh") == string("mbszhcyfsh")) {
		int nbrejpp;
		for (nbrejpp = 19; nbrejpp > 0; nbrejpp--) {
			continue;
		}
	}

}

void rsiimlq::pjcewogjcvbalforgyowxglbx(int uxxwisndzla, string zvvvpauytte, string petgxgxrwurm, string uxtplicc, string pwoohoimgeyaiul, bool lhrhxmejyrl, int ifdlfcqesoid, string pcndeufsxx, int vhdwjx) {
	double tisjpl = 2056;
	bool bwmtr = true;
	string dztnxtrig = "wqwaxsyygbqvojxczpszhrptolqmwxtsxyiebokifalupapghzyapd";
	if (true == true) {
		int xwdwa;
		for (xwdwa = 25; xwdwa > 0; xwdwa--) {
			continue;
		}
	}
	if (true == true) {
		int lcbde;
		for (lcbde = 12; lcbde > 0; lcbde--) {
			continue;
		}
	}
	if (2056 != 2056) {
		int bdqpkgjxa;
		for (bdqpkgjxa = 57; bdqpkgjxa > 0; bdqpkgjxa--) {
			continue;
		}
	}
	if (string("wqwaxsyygbqvojxczpszhrptolqmwxtsxyiebokifalupapghzyapd") != string("wqwaxsyygbqvojxczpszhrptolqmwxtsxyiebokifalupapghzyapd")) {
		int jyppr;
		for (jyppr = 15; jyppr > 0; jyppr--) {
			continue;
		}
	}

}

double rsiimlq::zivvseoeyaterfsxzhegotv(string rknxmvv, bool mbalpuwoznc, bool pikqjzlh, double qiqebdvibhdfng, string jdazr, int xqgmnvrzqmvp, bool tkxhvaa, bool pbwyucgbcxdk, double giecnxmjpgw, double uvktjknou) {
	double ynkylgbjns = 71243;
	bool furuyhr = false;
	int evveducdo = 326;
	bool dlzvlbloxkrv = false;
	bool kvzuptsrbzbl = false;
	int tkorfuas = 1041;
	if (false != false) {
		int lnxifswvo;
		for (lnxifswvo = 18; lnxifswvo > 0; lnxifswvo--) {
			continue;
		}
	}
	return 71592;
}

double rsiimlq::bxpdwxznraovlyp(int axccmegt, double cfyvamyyceg, int jvmurbnogntrld, int tuirao, double rzltaxifn, double crvzmywvm, double xwchmm, string vzfrppmgdhiz, int vvdbnswtzpgrcry) {
	return 84573;
}

void rsiimlq::llrlzeixuhy(double ylgxnlw, bool ukxlx, int cdfrvdmddxfbua, bool hrwmll, string xexuhnhyqsbu, string tipbraywbu, string fyhwky, int abpypwpaqw, bool lihtplrdznybyuu, int cwekwzgg) {
	double ewmlvrbdjssn = 14963;
	bool fpjtalvuxxzd = true;
	string bqkwxi = "dqxknzpofoumdnurezxqzzjquxnpbxnwthxiijmskyrpyeblyowozznvhwzlvexwgzududluiggjblazrmdcvizsp";
	string kvohcd = "udgmghvtrbb";
	bool xkxnjbapnsgelw = false;

}

string rsiimlq::stcuegqilurjkga(bool gfbqynyvbmqr, bool oszllhnowotd, int vwgtp, bool nzdhf) {
	bool sccdkj = false;
	bool dxxxjiamjxofohr = true;
	string wyputvlpegtjudp = "xjeahacijutevoeovlwbkjquwhfr";
	double nlhlshcgarpnwch = 21372;
	double dvrkwi = 13195;
	string glcshutwgf = "yaecahiyrgjghejiclvpqfdnyz";
	double kpkoucztjburfr = 45279;
	double juubv = 4147;
	bool gakgetmuxng = false;
	if (false != false) {
		int sfvmxnw;
		for (sfvmxnw = 6; sfvmxnw > 0; sfvmxnw--) {
			continue;
		}
	}
	return string("gbgyhxkpfths");
}

double rsiimlq::ihqcvqnxruryndprbswucjp(double uxipu, double rcekx, int iacucim, int ssahvzrxqdgj, double etsoywtvwdynhn, bool sfnbjb, string ubzotvxohreuu, bool ockgkkqzs, string mdkcbojkrf, int hsysswlpekgbw) {
	double jhhkdvlwkjr = 25855;
	string avlhsduaffrp = "cyvfy";
	bool odbnnlxptvtvu = true;
	double oqpxmdmbei = 42648;
	int avpdhrmcaepgmc = 726;
	double yrdqqd = 30792;
	string zrugrug = "eqamzxsilnytucgt";
	double heymqkxie = 88929;
	double cibfdkl = 81002;
	int srwuprmhjjnve = 8563;
	if (25855 == 25855) {
		int jusyp;
		for (jusyp = 10; jusyp > 0; jusyp--) {
			continue;
		}
	}
	if (25855 != 25855) {
		int rzapoem;
		for (rzapoem = 91; rzapoem > 0; rzapoem--) {
			continue;
		}
	}
	return 39691;
}

string rsiimlq::lelaaxyljzylyqt(double dkobta, bool bbuimibqpivoez, double lgpqqnkwbw, bool cpbvubvsk, bool jvsmqhwwprvpidm, bool wmcfspteuk) {
	string gxkdbijuynkhdsv = "";
	double hjrggbnnpycja = 32815;
	string rxsypwfh = "fqnujzjgdgotbbjqijftctrkeqzuhlflskslixtfuswwdnbqkzvtxgynwe";
	int fnwyhnb = 1359;
	bool jwtnzmdfbnqp = false;
	bool ububzztuokotvq = true;
	string ozucswlvni = "xdejhycinzzuzedieafjpxgygljtlddyymyljywkmuzvionzvplfprvtxkedsjgloxaqmyzwielrdumderldmtcvcdlno";
	if (string("fqnujzjgdgotbbjqijftctrkeqzuhlflskslixtfuswwdnbqkzvtxgynwe") != string("fqnujzjgdgotbbjqijftctrkeqzuhlflskslixtfuswwdnbqkzvtxgynwe")) {
		int lq;
		for (lq = 12; lq > 0; lq--) {
			continue;
		}
	}
	if (1359 == 1359) {
		int xruxyohlrl;
		for (xruxyohlrl = 16; xruxyohlrl > 0; xruxyohlrl--) {
			continue;
		}
	}
	return string("cxflcvoovxfiu");
}

string rsiimlq::tnjhwphhhm(int olkszfka, string tmiiebyob, double hdtpnn, string amrgk, int nehddrwlefhiovd, int njurf, double toxxatdylyst) {
	bool sbgbkozmzhs = false;
	return string("ucdsyzpxvmtwbtpn");
}

void rsiimlq::qpdcekrlwahztylszisfnczb(string jgdejirbyujnwoi, int occfmpsxikgd, int cyfcndegxctzeps) {
	int xoaiswxvughx = 4337;
	if (4337 != 4337) {
		int pinmt;
		for (pinmt = 15; pinmt > 0; pinmt--) {
			continue;
		}
	}
	if (4337 != 4337) {
		int pccpezxp;
		for (pccpezxp = 91; pccpezxp > 0; pccpezxp--) {
			continue;
		}
	}
	if (4337 != 4337) {
		int oqrekzax;
		for (oqrekzax = 55; oqrekzax > 0; oqrekzax--) {
			continue;
		}
	}

}

bool rsiimlq::fmfykgffzareftkjhud(double ruubukaigwdl, int ejonq, int bsisdrape, double ljgrlmzgxpmlcyj, string tzxoqnp, bool calalhldbpirm, int puqhlyba, bool arizjn, int sgbnyjm, bool kfhcsrpwqsuzl) {
	double jqokpf = 5488;
	string ydcvrhamn = "toblrioipgysetzioclnijdneughfapnxczhckkzgoxapvdnqtqpybsidgppojxbtjxwpqiguoq";
	string lqvsdtits = "ylzgozrzlhrabccrnbmjeuajvnndc";
	if (string("toblrioipgysetzioclnijdneughfapnxczhckkzgoxapvdnqtqpybsidgppojxbtjxwpqiguoq") != string("toblrioipgysetzioclnijdneughfapnxczhckkzgoxapvdnqtqpybsidgppojxbtjxwpqiguoq")) {
		int xx;
		for (xx = 57; xx > 0; xx--) {
			continue;
		}
	}
	if (string("ylzgozrzlhrabccrnbmjeuajvnndc") == string("ylzgozrzlhrabccrnbmjeuajvnndc")) {
		int akqinrzfhc;
		for (akqinrzfhc = 61; akqinrzfhc > 0; akqinrzfhc--) {
			continue;
		}
	}
	return true;
}

void rsiimlq::unzyqeepouwdowaaevau(int elrnvlibhvl, string cofkyyqk, double ysyrxkjrxdkd, double gzwtyedwyzqw, double pvyiua, double aignypnuemdknl, string haoignh, int zbupgnmmhoh) {
	double nllbyyyit = 19064;
	double wyujrtqxzkly = 53922;

}

int rsiimlq::rfzufnpolpkwadalcep(string qmkbjseiz, double rtcgdrhqn, int cvuklmsqxxjyu, double hsiunrg, double yhgpglwpukdk, double bttgshoflekleu, string piupsodooflrf, string emolltgwf, int dntgakrb) {
	bool ctooienakd = true;
	if (true != true) {
		int kw;
		for (kw = 64; kw > 0; kw--) {
			continue;
		}
	}
	return 75815;
}

int rsiimlq::ffcgufzxldmnab(bool wmkwgefxpcujsmh, string dmeinrbexvppba, string fhalvth, string aabpehahwhxvkb, double jaiegaxotyvaw, bool kpotdea) {
	int biydardahccl = 254;
	bool guiiosetj = true;
	string dgzwsam = "fqerucducnarxbpqdyyxqipmscitej";
	if (string("fqerucducnarxbpqdyyxqipmscitej") != string("fqerucducnarxbpqdyyxqipmscitej")) {
		int awiutt;
		for (awiutt = 80; awiutt > 0; awiutt--) {
			continue;
		}
	}
	if (254 != 254) {
		int navstxfdrb;
		for (navstxfdrb = 87; navstxfdrb > 0; navstxfdrb--) {
			continue;
		}
	}
	if (254 != 254) {
		int jodsl;
		for (jodsl = 52; jodsl > 0; jodsl--) {
			continue;
		}
	}
	if (true == true) {
		int idembizs;
		for (idembizs = 86; idembizs > 0; idembizs--) {
			continue;
		}
	}
	if (true != true) {
		int ntjwzdojvd;
		for (ntjwzdojvd = 14; ntjwzdojvd > 0; ntjwzdojvd--) {
			continue;
		}
	}
	return 29891;
}

rsiimlq::rsiimlq() {
	this->llrlzeixuhy(6670, true, 3985, false, string("luxhmakibqnmvgqrxhvzrwnmfbftlgqylszjzqpy"), string("ozbjihjqforeriyqrvqjsfhanctqgvurixulytrbsilbdrhurtsvwdcjrwykqqzudcewqn"), string("sgerupwcfszjhxodqiijkvpanfipyrwytywuiwslduedicuaegutptawwyihehiumdnc"), 4384, true, 610);
	this->stcuegqilurjkga(false, true, 1066, false);
	this->ihqcvqnxruryndprbswucjp(20029, 4433, 662, 1356, 5704, false, string("otzcnynjhbcrmgybbgpmftxiqddprnrsughgzcsncgojqgjdrmmyvirbeoihmvrqzgnewbcyaqczndpuunweunsb"), true, string("bhvwvdmplluonxcwricheofpsvzvqeajpeogljdyouumkhqhivnls"), 830);
	this->lelaaxyljzylyqt(9759, false, 11375, false, false, false);
	this->tnjhwphhhm(3243, string("inidpavqkqkqyenmjnqhbxwelcjayyjcitsxbzbuiezi"), 6259, string("vwjdou"), 6825, 3468, 85562);
	this->qpdcekrlwahztylszisfnczb(string("vflacwbymuyzftmgigiucczjeqvojjndphdiihxrrerulyubrajefdcjjtjerfsrondgwhwbwgwcqpviikjwvuzpftulmo"), 1324, 6021);
	this->fmfykgffzareftkjhud(37765, 8118, 6940, 32141, string("vpjtsjyfwqylbpbxrpsqfxzumkvaofwcalfbhydmwjgvvtmnbovezqeqjyhshsdjvdcgcnqahzlvakczegdxycics"), false, 979, false, 5523, true);
	this->unzyqeepouwdowaaevau(1031, string("vhcbcmtmiyjllpkjgwzvbfpgpksjcpdp"), 16530, 22828, 34292, 10918, string("jobjwduficiwmcgfqvemylmnpglqxhdkcglfbrooxuczodctsqobncif"), 1066);
	this->rfzufnpolpkwadalcep(string("gbjgiuthmfuszskswpohctxaiyflquskqcdrr"), 31734, 1009, 70116, 56709, 1139, string("pixsolzevnqtvjduiqvvrpsprabmzdgxhndhc"), string("qtjqnrhcuniiqtteinwgplhxohurzjczsywierarfizkwabnktalhxzmxktgsvfwkhbrvkovasbjbsnzqjt"), 546);
	this->ffcgufzxldmnab(true, string("jwqbeetjfxkfefcu"), string("hlfqxjruxbaotvmyqdwmkqmcedoj"), string("icgluvlqnnvjprilgnqwetwhjnqsxcrxlglxdjkmuvxtncvpzsheoxxehrujtylq"), 6183, false);
	this->womnycgclshpmxatbsjdf(true, 2131, string("sfxuqiausyjdyiuawcmobfqxyukohjnievtfmd"), string("ykcdmhggmtdziqjratmykqwyoxjvpxcixjffdippyrdbtdspfzbzsnwvisimmnedamtimumofjrviesfksjbqnycltydrxvr"), 1557, string("wetoptbalrcyeakhhfphtyzjuwhuhndombndtthwurklvizbanwhcpbkvnghxwulmeyzcvfmxaiatphlrrfckczezkhqvlmw"), 658, true, 42107);
	this->fhhijdyrufrpnmlxmsecyndw(1109, 5171);
	this->avndtqdjwlbbapmzcjl(23490, 1550, 3377, string("oectvfahermwgihqghqztgtqjukltypwhptihqxicwykrjtfawshknhudpcgrgmornpcgtni"), string("slwlmbbdlu"));
	this->zmdquqsxrlufeukvmtfi(3727, false);
	this->uqgisiltnxonaszskbz(true, true, false, 4848);
	this->pjcewogjcvbalforgyowxglbx(3397, string("uawpnoxdozckkkuxtprlfjwztqgoxaotsjqpozkgizenybyfsnqjabxodeuxywcqxmhejcmlctetatuigoscuqcbxxpwjddik"), string("wnkacnmbdyjlqwskpyhpszdmxaduqsynknlmedasqbdijeqyapxdhtitikjoclkkiwrxnxnykdiylhfziowuncvtzvamqpkpjg"), string("znbbguzezckcatf"), string("cluicykmxilitqissihpcxkqqvjdzvziypcpriumedenmqiuytxhwzsa"), false, 1403, string("qajkzbhhpdllgvrzmympcyyyxjkpbbgbruxtldyqnkyyjdvtrdwcy"), 5033);
	this->zivvseoeyaterfsxzhegotv(string("iqvxtotllyyptutjicqctknlwsedlxirdiqlye"), true, false, 39555, string("fsvcfaxvoaf"), 2809, true, false, 36675, 2661);
	this->bxpdwxznraovlyp(25, 16794, 463, 368, 51, 71189, 4585, string("wlajlssllykphovtayy"), 5461);
	this->hpishujzhreii(72302, 1210, 3746, 27289, true);
	this->kjgdznmxch(16083, true, 72151, 34751, 79987, 1581, 2721, 23144, false, string("xsfaynebdqkukkjj"));
	this->ghwfiitmfzkoeflkbyd(680, 1987, 10684, true, string("trnhrwljmszbqqbikkbdxrgekatwlocywkupycifqdklhsscfwdlhxxbuoooifukltkwxidvbvruusmvqmcnuekvgrnrufsjlyd"), false, 15969);
	this->fswagwhpglxuwm(true, 22924, true, string("rrawsssgtvqhvnxaxzaciepdaqzmcicjaavqpkgoaewiiixlhnqqlogbdkcqbyllugqqebk"), true, 63401, 18722, 68463, 1256);
	this->gsjafblpzxqfut(1977, 24943, 4619, true, string("lncrdfkjjvgvprobtbikvulyckepekusmexavxq"), true);
}
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class vgboyjf {
public:
	int artfrr;
	bool xpkmjke;
	vgboyjf();
	void xiuhzlqsdqevmqibupped(double imxlnamueyzrj, bool bawixpznisfajmm, bool rubetckwrroz, bool ywzyrglxsgs, bool qkekwcuwe);
	bool rnuymlsaairkrjsxgz();
	int yifeixutjqtisdgoottbxrr();
	int flssqzxaexjmlqmtqadtmzsmr(string gsvjhvpmctlrm, int vctpivfbwflwx, double pspzu, int dnyvvfbewukmpge, int maohqcpet, bool zmmxulgk, int oifaawmb);
	bool ddihxdscscsmp(string zjexwskps, int nigdam, double ehnxengrrsfhr);
	int jelejcseiifqsiztrsbnqaovd(string tfrwanf, string fkknryh, bool zbqaheegrr, double oovxtzg);
	bool ucslirjwie(bool ysrrcclq, int xsrmdtqur, int ecpatskohfnjvoa, int huigvpb, int mzjhclzfgkm, bool ifbjfkqvwebqi);

protected:
	bool xflcmmz;
	string onagccemoqjgio;

	void weatkqlaacdzkofehphxrlo(string nmyzmzzw, int mswxolohqlnyu, double rjwmbchowv, double xngdhmsysfv, double vvwqcqapkcquvok, int annkggcy, string buxzyojgpuo);
	int bylydfyigrcfkavhvbfafaj(string wpagnwfuia, bool ehtvg, int bawhhxwcmznja, string ladddubzelge, double yjligpewsfsejqf, string ngrla, int rmqprgdvjns, int wtxefoxovck, int hxpmfjmuxcvcbxw);
	bool frbnvzfqxryrvmkwetgprp();
	string tluzdnlrtwycvgiflieu(bool zecgmgjd, string jtfayfncszrj, double cygnvarbaaoky, double qveiknydxrnwmlt, int hvlpwphcwxwklhf, string znecsaxh, string gzdobnep, bool kymigunglzx);
	int yajsyalmntqafeuvh(bool coohkdvmshg, string orxrqetumgbsyj, double hguibilelspkrk, int myszsefwiasef, string gzgdxldrg, int pvmvjona, string mpelnl, int arbmg);

private:
	int xstxijkkitt;
	double tdasshr;

	string ghogjaqzzgbtgnyxqkxbcnc(int ngkamm, double zfhiefif, bool isiktzh, string obmzvjsbebdwiv, bool tzspmpktydzi, int zerwtewcwiaqjq, double azwth, double lllzjid, int ollrpf);
	double cgnkybvxarnjndrlyk(bool vyjgzrlrsljzmfz, bool atzrdcbjj, bool lcrkwvdhj, double emztrrbqcaayc, int hqgbxfbjk, double uhxko, int qzfxnkdcpnr, string mmyzwhdxdjhaiz);
	string wuzzahnqmvavg(int piurgnkfa, bool aumkabczymzur, string qnhdmxnlf, int ljoulqkp, int wryqpqsgqp, int ehhcdctiic, double absauthybjr, int itakdhezbtbv, bool tyrdlbkgpjyy);
	int aprfvnonyhjwes(bool mnshowrjrk, int zkugj, bool kcavqwtokhastuc, string xotcgfwqyjjknae);
	int vfolydnhfwldpzb(string bcnjgtdrd);
	bool vijxrlerhsmuzzhfi(bool fhujax, double zfxzz, bool evngvdpstnjkibh, double quyleoxvfg, int lgegurwee, string jmqkihtg, double ubqbynk, double cnigxiwjanfk);
	int ufcuiuvusgubudefyziuzdeks(double msdfwmalvkjbh, double ibhtekxoqujs, string cgdyjuhnpvbmff, bool wyeofub, int slrykqvhchucpi);

};


string vgboyjf::ghogjaqzzgbtgnyxqkxbcnc(int ngkamm, double zfhiefif, bool isiktzh, string obmzvjsbebdwiv, bool tzspmpktydzi, int zerwtewcwiaqjq, double azwth, double lllzjid, int ollrpf) {
	double fbkhyp = 46395;
	string eirgvt = "upfztsibsheoxwyagczrfhijkedyaykekcxsioxezblq";
	bool wccacj = false;
	bool mnlzxeb = true;
	int iiqjjv = 860;
	int uuqhwsuyjp = 5056;
	double mkyhht = 49507;
	bool cjojp = true;
	int ajnkriofbjgr = 4856;
	if (49507 != 49507) {
		int worbcxgrpr;
		for (worbcxgrpr = 80; worbcxgrpr > 0; worbcxgrpr--) {
			continue;
		}
	}
	return string("byyok");
}

double vgboyjf::cgnkybvxarnjndrlyk(bool vyjgzrlrsljzmfz, bool atzrdcbjj, bool lcrkwvdhj, double emztrrbqcaayc, int hqgbxfbjk, double uhxko, int qzfxnkdcpnr, string mmyzwhdxdjhaiz) {
	int nubmig = 2859;
	double lenepmbtinhnhc = 24342;
	string lfunzbvyfrseyy = "ascvrsjiisxggbubbsfznsufnkcfpnkfqfkvvhymmaczbhosv";
	string yvgebmb = "gytmjpsodrueqacdyedgvwmxiuxkccndgfqkozitsskoeewmtddbbrbzofcc";
	string kwqvjah = "hzsmhhqzrmcnzbgzndyrvzxsqikecketlafasjjjoadtnolductiuqnahpyvtkiarpkdzmtlymcfugxhji";
	bool isvfajmf = true;
	return 89614;
}

string vgboyjf::wuzzahnqmvavg(int piurgnkfa, bool aumkabczymzur, string qnhdmxnlf, int ljoulqkp, int wryqpqsgqp, int ehhcdctiic, double absauthybjr, int itakdhezbtbv, bool tyrdlbkgpjyy) {
	double hslxsqmeqilmd = 10662;
	bool eohznlb = false;
	bool hjrurj = true;
	string tjdur = "opopxtpjrlvdvabdyjfsuafzpggxvvuhssspoxoympaoqskvagjvihssjyhszmtwjzoaohtzbbgpimmixqmnuvbx";
	string bxbcowicj = "dbnngiti";
	double bwrvvqqicg = 33996;
	int blakjugyhin = 2602;
	string tqagvnfheynyngo = "brclrgmqtuldenwchazcjqmpuejbcarifojhedtppwawzmaesfnmxnhdohpklmyurwtgjulaccnqfztwfittyf";
	string qotnyjx = "tizjqxfhnmtkoriphrbajzlgqeucowxemqitt";
	bool tciamb = true;
	if (10662 == 10662) {
		int quyc;
		for (quyc = 84; quyc > 0; quyc--) {
			continue;
		}
	}
	if (string("dbnngiti") != string("dbnngiti")) {
		int unie;
		for (unie = 96; unie > 0; unie--) {
			continue;
		}
	}
	if (true == true) {
		int tilz;
		for (tilz = 59; tilz > 0; tilz--) {
			continue;
		}
	}
	return string("bbgglkpgxahu");
}

int vgboyjf::aprfvnonyhjwes(bool mnshowrjrk, int zkugj, bool kcavqwtokhastuc, string xotcgfwqyjjknae) {
	int kszww = 5646;
	if (5646 != 5646) {
		int qucufkjuqx;
		for (qucufkjuqx = 15; qucufkjuqx > 0; qucufkjuqx--) {
			continue;
		}
	}
	if (5646 == 5646) {
		int dezgbuw;
		for (dezgbuw = 75; dezgbuw > 0; dezgbuw--) {
			continue;
		}
	}
	if (5646 == 5646) {
		int cfwzmchpkz;
		for (cfwzmchpkz = 34; cfwzmchpkz > 0; cfwzmchpkz--) {
			continue;
		}
	}
	if (5646 != 5646) {
		int dbtwbmx;
		for (dbtwbmx = 17; dbtwbmx > 0; dbtwbmx--) {
			continue;
		}
	}
	if (5646 == 5646) {
		int sm;
		for (sm = 17; sm > 0; sm--) {
			continue;
		}
	}
	return 68697;
}

int vgboyjf::vfolydnhfwldpzb(string bcnjgtdrd) {
	bool whdtbswhwziiy = false;
	bool xrhdvwcffsl = true;
	int homvkzrcv = 2500;
	double tazthdjmvoee = 7636;
	bool uvfnbxihgj = false;
	int ikvououdyj = 110;
	string hxszum = "vgohjuq";
	string aewwvodnli = "bewu";
	string uekjfbndjrq = "mxezjwydvs";
	string caajpiehoqza = "bplaefzkiairlimqffpmkadcvrkgutvblwmnjcqqahiknvxkzuipgrnobnsgrzhcujvkclndyyya";
	return 62384;
}

bool vgboyjf::vijxrlerhsmuzzhfi(bool fhujax, double zfxzz, bool evngvdpstnjkibh, double quyleoxvfg, int lgegurwee, string jmqkihtg, double ubqbynk, double cnigxiwjanfk) {
	bool pubugkmjmrvmixa = false;
	int kwbpfq = 1380;
	int wtumhhhzsrjobvh = 349;
	double hipkkumxvf = 9937;
	bool zltrlan = false;
	int txudc = 3402;
	if (349 == 349) {
		int imlyjssuuf;
		for (imlyjssuuf = 47; imlyjssuuf > 0; imlyjssuuf--) {
			continue;
		}
	}
	return true;
}

int vgboyjf::ufcuiuvusgubudefyziuzdeks(double msdfwmalvkjbh, double ibhtekxoqujs, string cgdyjuhnpvbmff, bool wyeofub, int slrykqvhchucpi) {
	string sgekekoj = "kvjlnrvxwsaljlmmvaozzjpsdsaouoiskbwhmhmgobxiwpuzlyrynppbgpdkbtucyscnvggsotdoqoh";
	int hniteppy = 4504;
	if (4504 == 4504) {
		int hqcrptwnkd;
		for (hqcrptwnkd = 7; hqcrptwnkd > 0; hqcrptwnkd--) {
			continue;
		}
	}
	return 16737;
}

void vgboyjf::weatkqlaacdzkofehphxrlo(string nmyzmzzw, int mswxolohqlnyu, double rjwmbchowv, double xngdhmsysfv, double vvwqcqapkcquvok, int annkggcy, string buxzyojgpuo) {
	int yyjdnxexks = 983;
	int rzvziqxuhgmsc = 3655;
	string smaeqhkcqtn = "mqldgjiqbj";
	bool fyythmm = true;
	int xhcfti = 6613;
	bool osoicmoewst = false;
	if (false == false) {
		int rmtyqxb;
		for (rmtyqxb = 60; rmtyqxb > 0; rmtyqxb--) {
			continue;
		}
	}
	if (6613 != 6613) {
		int xnvdliux;
		for (xnvdliux = 51; xnvdliux > 0; xnvdliux--) {
			continue;
		}
	}

}

int vgboyjf::bylydfyigrcfkavhvbfafaj(string wpagnwfuia, bool ehtvg, int bawhhxwcmznja, string ladddubzelge, double yjligpewsfsejqf, string ngrla, int rmqprgdvjns, int wtxefoxovck, int hxpmfjmuxcvcbxw) {
	return 3478;
}

bool vgboyjf::frbnvzfqxryrvmkwetgprp() {
	string ywkokyvdkoqc = "vmcqeilqsbkcxbepubxt";
	bool tqomqaxr = true;
	bool htlguepweounbh = false;
	if (string("vmcqeilqsbkcxbepubxt") == string("vmcqeilqsbkcxbepubxt")) {
		int pptipgvu;
		for (pptipgvu = 37; pptipgvu > 0; pptipgvu--) {
			continue;
		}
	}
	if (false == false) {
		int emxjpzcp;
		for (emxjpzcp = 22; emxjpzcp > 0; emxjpzcp--) {
			continue;
		}
	}
	return true;
}

string vgboyjf::tluzdnlrtwycvgiflieu(bool zecgmgjd, string jtfayfncszrj, double cygnvarbaaoky, double qveiknydxrnwmlt, int hvlpwphcwxwklhf, string znecsaxh, string gzdobnep, bool kymigunglzx) {
	double labkvzf = 18523;
	bool ftnmrriyfc = true;
	bool fvwvcwzzwbgfpx = true;
	int gdrajwl = 3729;
	int ftsmulkhc = 2340;
	bool qtsxbxbiccrxl = false;
	string tmupldgqbh = "goftzosaapjzjkx";
	string eqesbzjuybik = "dlrnxtubacxwjrdtptazsshpjwarqevemumyvnqbecbmfxhspwvmwpbgwcxjamypzmc";
	if (18523 == 18523) {
		int sdhuqy;
		for (sdhuqy = 5; sdhuqy > 0; sdhuqy--) {
			continue;
		}
	}
	if (string("dlrnxtubacxwjrdtptazsshpjwarqevemumyvnqbecbmfxhspwvmwpbgwcxjamypzmc") != string("dlrnxtubacxwjrdtptazsshpjwarqevemumyvnqbecbmfxhspwvmwpbgwcxjamypzmc")) {
		int mej;
		for (mej = 91; mej > 0; mej--) {
			continue;
		}
	}
	if (2340 != 2340) {
		int jmnwnmkc;
		for (jmnwnmkc = 48; jmnwnmkc > 0; jmnwnmkc--) {
			continue;
		}
	}
	if (3729 != 3729) {
		int rnppod;
		for (rnppod = 26; rnppod > 0; rnppod--) {
			continue;
		}
	}
	if (true == true) {
		int hlb;
		for (hlb = 96; hlb > 0; hlb--) {
			continue;
		}
	}
	return string("yzzg");
}

int vgboyjf::yajsyalmntqafeuvh(bool coohkdvmshg, string orxrqetumgbsyj, double hguibilelspkrk, int myszsefwiasef, string gzgdxldrg, int pvmvjona, string mpelnl, int arbmg) {
	bool vzhxvnh = false;
	int tqoowyu = 8033;
	string isdtqwyr = "ljsnyavceidyklshykiaqagssuhnsbwbzlatnqjker";
	bool frudompu = false;
	if (string("ljsnyavceidyklshykiaqagssuhnsbwbzlatnqjker") != string("ljsnyavceidyklshykiaqagssuhnsbwbzlatnqjker")) {
		int alrjiltnf;
		for (alrjiltnf = 67; alrjiltnf > 0; alrjiltnf--) {
			continue;
		}
	}
	if (8033 == 8033) {
		int ifeoydrbqi;
		for (ifeoydrbqi = 41; ifeoydrbqi > 0; ifeoydrbqi--) {
			continue;
		}
	}
	if (false == false) {
		int ttclhj;
		for (ttclhj = 20; ttclhj > 0; ttclhj--) {
			continue;
		}
	}
	if (string("ljsnyavceidyklshykiaqagssuhnsbwbzlatnqjker") != string("ljsnyavceidyklshykiaqagssuhnsbwbzlatnqjker")) {
		int qpkhy;
		for (qpkhy = 26; qpkhy > 0; qpkhy--) {
			continue;
		}
	}
	return 58112;
}

void vgboyjf::xiuhzlqsdqevmqibupped(double imxlnamueyzrj, bool bawixpznisfajmm, bool rubetckwrroz, bool ywzyrglxsgs, bool qkekwcuwe) {
	double kzsaf = 911;
	int dyojgjjwkvxqm = 3101;
	int hifleubkiki = 1839;
	string bfnsxdjhynl = "afqokynrgghsozhltmrtffczwsckdgwldviroqrfnemwjcfrdufufjmfzjlpk";
	double htvby = 25336;
	string fvpdnxzptdwnnqg = "jwysyxpvhuyxbocevtyqvtdpwzbnbvgwppqpbikxnijqbwyqzbidtkjrpdopudjzcubhfzdqjfasfirrxckvuwsqywatg";
	string bcoysnfp = "jbuthwtcqtdhqyqvwcithxqjhsvlqbdxpeqbtcwsxadmftvxxwzexlnvrpdpimpokpwfturfoae";
	if (string("jwysyxpvhuyxbocevtyqvtdpwzbnbvgwppqpbikxnijqbwyqzbidtkjrpdopudjzcubhfzdqjfasfirrxckvuwsqywatg") != string("jwysyxpvhuyxbocevtyqvtdpwzbnbvgwppqpbikxnijqbwyqzbidtkjrpdopudjzcubhfzdqjfasfirrxckvuwsqywatg")) {
		int qmmuvp;
		for (qmmuvp = 61; qmmuvp > 0; qmmuvp--) {
			continue;
		}
	}
	if (string("jwysyxpvhuyxbocevtyqvtdpwzbnbvgwppqpbikxnijqbwyqzbidtkjrpdopudjzcubhfzdqjfasfirrxckvuwsqywatg") != string("jwysyxpvhuyxbocevtyqvtdpwzbnbvgwppqpbikxnijqbwyqzbidtkjrpdopudjzcubhfzdqjfasfirrxckvuwsqywatg")) {
		int iyiiadlwep;
		for (iyiiadlwep = 71; iyiiadlwep > 0; iyiiadlwep--) {
			continue;
		}
	}

}

bool vgboyjf::rnuymlsaairkrjsxgz() {
	return false;
}

int vgboyjf::yifeixutjqtisdgoottbxrr() {
	string exxgfhuwopmxor = "tbycqzcjangliqzoaiqwhcqesoroqagxihcfoczhxjydkgbootnpdjhgeloapggsevlczxgomshdwhpdrzfgfxf";
	string owldqwgbkc = "wppsdqrzemqsrudnxyhzbizrvkjtbuvbldjaiallpphmt";
	int irosdckvbqfgsa = 1128;
	if (string("wppsdqrzemqsrudnxyhzbizrvkjtbuvbldjaiallpphmt") != string("wppsdqrzemqsrudnxyhzbizrvkjtbuvbldjaiallpphmt")) {
		int xqkzlzlgw;
		for (xqkzlzlgw = 76; xqkzlzlgw > 0; xqkzlzlgw--) {
			continue;
		}
	}
	if (string("wppsdqrzemqsrudnxyhzbizrvkjtbuvbldjaiallpphmt") != string("wppsdqrzemqsrudnxyhzbizrvkjtbuvbldjaiallpphmt")) {
		int vny;
		for (vny = 31; vny > 0; vny--) {
			continue;
		}
	}
	return 78236;
}

int vgboyjf::flssqzxaexjmlqmtqadtmzsmr(string gsvjhvpmctlrm, int vctpivfbwflwx, double pspzu, int dnyvvfbewukmpge, int maohqcpet, bool zmmxulgk, int oifaawmb) {
	string elrhy = "kkczdqwabookgnokhtxkzlmxsxjgdtyxkbdhvmujzhhywdtgptcfmqyjehziuqitctsjxwmqgbhqjaqzuqljzbmfxtxz";
	bool vhmubkde = true;
	bool wtuda = true;
	double azubsmciq = 28302;
	int ifkjwlgqccjkel = 1006;
	double flufso = 37737;
	string jnrqetqtpkcgnql = "wejtaljvchwialvp";
	return 83750;
}

bool vgboyjf::ddihxdscscsmp(string zjexwskps, int nigdam, double ehnxengrrsfhr) {
	string mmbic = "kcycxbprrkkxevjtsvahotrtgbgndfrpdfqn";
	int cealugpcz = 2019;
	double onyrop = 9125;
	if (2019 == 2019) {
		int br;
		for (br = 48; br > 0; br--) {
			continue;
		}
	}
	if (string("kcycxbprrkkxevjtsvahotrtgbgndfrpdfqn") == string("kcycxbprrkkxevjtsvahotrtgbgndfrpdfqn")) {
		int pw;
		for (pw = 94; pw > 0; pw--) {
			continue;
		}
	}
	return false;
}

int vgboyjf::jelejcseiifqsiztrsbnqaovd(string tfrwanf, string fkknryh, bool zbqaheegrr, double oovxtzg) {
	int mdexbkdpzhwspxv = 2043;
	string kvwllrbz = "jagwpjwecbzlartnbraweqfmjylodbxwzzagjixiajokufseafbfrbdfmsmkhkzckdwmtkyykfyjqqt";
	int qwnqwjdjdbgpj = 1064;
	string yqolpq = "qnlpgllvuowhxp";
	double yvceobc = 669;
	string drtwjr = "ibtqowawgmddrakgxddscruovvvgy";
	string cczvjs = "";
	return 9485;
}

bool vgboyjf::ucslirjwie(bool ysrrcclq, int xsrmdtqur, int ecpatskohfnjvoa, int huigvpb, int mzjhclzfgkm, bool ifbjfkqvwebqi) {
	bool ohgjhmtgzdyyce = true;
	double llsgogqluknf = 564;
	string smcpuflxatsjkt = "tfbsmmrkyaejsyccckoiumgtodbeabvyyjpehqvpzbhjhunknrxikoimxvremewdxhznnxxrgovsfhxyxpiuszhzpqnf";
	double nmmxatuphxdilq = 15843;
	int zyhlgpalco = 4781;
	double bjasqjtnm = 43738;
	double umxaevdkgjss = 5889;
	int sxosircuzdesxji = 5746;
	bool nlnpzrwyyymxdg = false;
	int dtqkathjurcjpt = 1638;
	return true;
}

vgboyjf::vgboyjf() {
	this->xiuhzlqsdqevmqibupped(9709, true, true, true, true);
	this->rnuymlsaairkrjsxgz();
	this->yifeixutjqtisdgoottbxrr();
	this->flssqzxaexjmlqmtqadtmzsmr(string("ztckwzthmsnlzssmykolooqsmcjtcatcngfhadlymskpmppbexooiagqknvmdbllrelhzwkxfajfbiopr"), 5403, 33529, 3375, 5825, true, 4451);
	this->ddihxdscscsmp(string("dywlbtlflprxburwvsjpoqvmxqtgmlvtttmxfojohuxzagbgpwbhddxwzozsznheazogtauuycimocbaycomaaxwvcls"), 1865, 16718);
	this->jelejcseiifqsiztrsbnqaovd(string("okljngvoavbflldjvkquwmgvxonewsjcilwhftiel"), string("rhhxlakfjftmwuhiltvqyvpguydweclhusqppzmhbcaqydqtpdgzrcjwwolwnittcqaemcikdvuxpolvesjefd"), true, 23498);
	this->ucslirjwie(true, 1132, 3962, 3677, 1042, true);
	this->weatkqlaacdzkofehphxrlo(string("xleucwudyeyjimsmqhpkyeeiogjixhpspegbzoaklmkkhpbyfngtdigtglwez"), 1212, 42931, 20181, 26678, 3278, string("nfwlcmqcmeibrnmkdbmbuvovbiqecntqfwzsovsulofypsnnjvvlibhaokdukgsmz"));
	this->bylydfyigrcfkavhvbfafaj(string("uuiovpabifttalclrthwujzkgkhgipeaqwnimpyjvvtadmjowoblmpcwnjanoekpdxlwyktkggpsjgbbs"), true, 144, string("ndhgqdfiotv"), 7831, string("kvimqfrgtcaynugjtovvbkefyrz"), 6990, 619, 767);
	this->frbnvzfqxryrvmkwetgprp();
	this->tluzdnlrtwycvgiflieu(true, string("gplmyresshhogagckujntddwotyhrwkdahjzknzufiwmhfeqcqrvo"), 28549, 15689, 4292, string("xbprzivsjnhihizvxpmtyeq"), string("mulzahagfwiutsxmdulgrdrdxixgskwkjezyvntkzamfqrvyqyqoonjenmhjxrdmbfnxxbkwmcgbrnfoqxexdeg"), false);
	this->yajsyalmntqafeuvh(false, string("izzhnvkcxzcdapglqouqmmkmjptqebttafbvbrkcfvrswuslnwtfiwlqxhmurjnehn"), 38074, 1458, string("gxmrekhkhshegxivihfeblnuvmfaul"), 742, string("ppuob"), 2024);
	this->ghogjaqzzgbtgnyxqkxbcnc(299, 60747, false, string("acydaygktgfxngfiusomiejawiywizqduzyvknpfpignrlqhnudapdp"), false, 4536, 24850, 86787, 8230);
	this->cgnkybvxarnjndrlyk(true, false, true, 36406, 3962, 88663, 598, string("rnmbugfsjmabahdbsvudtwcsmriepykbm"));
	this->wuzzahnqmvavg(1702, true, string("wkwviqvnoewustyvanluizczcbcilelkdxoryykznorkibcprwvfwxvgkxjfxjbvayazgy"), 1309, 286, 1068, 11756, 987, false);
	this->aprfvnonyhjwes(true, 3869, true, string("rlzrvjlbkzahddbfaafltpcjkzprzgvrpkbyuaaruhrvtgktfukaalggxexvdjqnszahtj"));
	this->vfolydnhfwldpzb(string("oqmjgdifhcyripqyddjn"));
	this->vijxrlerhsmuzzhfi(true, 11373, false, 42851, 2114, string("syzpjfxnxjvjgzrvzbjebf"), 71313, 11373);
	this->ufcuiuvusgubudefyziuzdeks(18353, 9367, string("gxoixwtqhpftswvfaaokvifjoasoblzjoyzmokgncggmeiovhkgelmyrmajdqsjaqrwquhdurt"), true, 4509);
}
