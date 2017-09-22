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
