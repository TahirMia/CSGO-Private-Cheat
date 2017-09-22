/*
Syn's AyyWare Framework 2015
*/

#define _CRT_SECURE_NO_WARNINGS

#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"

#include <time.h>

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}

inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

Vector AutoStrafeView;

void CMiscHacks::Init()
{
	// Any init
}

void CMiscHacks::Draw()
{
	// Any drawing	
	// Spams
	switch (Menu::Window.MiscTab.OtherChatSpam.GetIndex())
	{
	case 0:
		// No Chat Spam
		break;
	case 1:
		// Namestealer
		ChatSpamName();
		break;
	case 2:
		// Regular
		ChatSpamRegular();
		break;
	case 3:
		// Interwebz
		ChatSpamInterwebz();
		break;
	case 4:
		// Report Spam
		ChatSpamDisperseName();
		break;
	}
}

void CMiscHacks::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	// Any Move Stuff
	
	// Bhop
	switch (Menu::Window.MiscTab.OtherAutoJump.GetIndex())
	{
	case 0:
		break;
	case 1:
		AutoJump(pCmd);
		break;
	}

	// AutoStrafe
	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	switch (Menu::Window.MiscTab.OtherAutoStrafe.GetIndex())
	{
	case 0:
		// Off
		break;
	case 1:
		LegitStrafe(pCmd);
		break;
	case 2:
		RageStrafe(pCmd);
		break;
	}

	//Fake Lag
	if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		Fakelag(pCmd, bSendPacket);
}

static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm 
	{
		push    ebp
			mov     ebp, esp
			and     esp, 0FFFFFFF8h
			sub     esp, 44h
			push    ebx
			push    esi
			push    edi
			mov     edi, cvar
			mov     esi, value
			jmp     pfn
	}
}
void DECLSPEC_NOINLINE NET_SetConVar(const char* value, const char* cvar)
{
	static DWORD setaddr = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8D\x4C\x24\x1C\xE8\x00\x00\x00\x00\x56", "xxxxx????x");
	if (setaddr != 0) 
	{
		void* pvSetConVar = (char*)setaddr;
		Invoke_NET_SetConVar(pvSetConVar, cvar, value);
	}
}

void change_name(const char* name)
{
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
		NET_SetConVar(name, "name");
}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 50)
		{
			pCmd->forwardmove = 450.f;
		}
	}
}

void CMiscHacks::LegitStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = 0.0f;

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -450.0f;
		}
		else if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 450.0f;
		}
	}
}

void CMiscHacks::RageStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	bool bKeysPressed = true;
	if (GUI.GetKeyState(0x41) || GUI.GetKeyState(0x57) || GUI.GetKeyState(0x53) || GUI.GetKeyState(0x44)) bKeysPressed = false;

	if ((GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND)) && bKeysPressed)
	{
		if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
			pCmd->sidemove = pCmd->mousedx < 0.f ? -450.f : 450.f;
		}
		else {
			pCmd->forwardmove = (1800.f * 4.f) / pLocal->GetVelocity().Length2D();
			pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
			if (pCmd->forwardmove > 450.f)
				pCmd->forwardmove = 450.f;
		}
	}

/*	IClientEntity* pLocal = hackManager.pLocal();
	static bool bDirection = true;


	static float move = 450; //400.f; // move = max(move, (abs(cmd->move.x) + abs(cmd->move.y)) * 0.5f);
	float s_move = move * 0.5065f;


	if ((pCmd->buttons & IN_JUMP) || !(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = move * 0.015f;
		pCmd->sidemove += (float)(((pCmd->tick_count % 2) * 2) - 1) * s_move;

		if (pCmd->mousedx)
			pCmd->sidemove = (float)clamp(pCmd->mousedx, -1, 1) * s_move;

		static float strafe = pCmd->viewangles.y;

		float rt = pCmd->viewangles.y, rotation;
		rotation = strafe - rt;

		if (rotation < FloatNegate(Interfaces::Globals->interval_per_tick))
			pCmd->sidemove = -s_move;

		if (rotation > Interfaces::Globals->interval_per_tick)
			pCmd->sidemove = s_move;

		strafe = rt;
	} */
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}

// …e Õ½Ê¿
void CMiscHacks::ChatSpamInterwebz()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ";

	if (wasSpamming)
	{
		static bool useSpace = true;
		if (useSpace)
		{
			change_name ("INTERWEBZ-");
			useSpace = !useSpace;
		}
		else
		{
			change_name("-INTERWEBZ");
			useSpace = !useSpace;
		}
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamDisperseName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ";

	if (wasSpamming)
	{
		change_name("\n…e…e…e\n");
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != hackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ.CC";

	int randomIndex = rand() % Names.size();
	char buffer[128];
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str());

	if (wasSpamming)
	{
		change_name(buffer);
	}
	else
	{
		change_name ("p$i 1337");
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	int spamtime = Menu::Window.MiscTab.OtherChatDelay.GetValue();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < spamtime)
		return;

	static bool holzed = true;

	if (Menu::Window.MiscTab.OtherTeamChat.GetState())
	{
		SayInTeamChat("INTERWEBZ.CC OWNS ME AND ALL");
	}
	else
	{
		SayInChat("INTERWEBZ.CC OWNS ME AND ALL");
	}

	start_t = clock();
}

void CMiscHacks::Fakelag(CUserCmd *pCmd, bool &bSendPacket)
{
	int iChoke = Menu::Window.MiscTab.FakeLagChoke.GetValue();

	static int iFakeLag = -1;
	iFakeLag++;

	if (iFakeLag <= iChoke && iFakeLag > -1)
	{
		bSendPacket = false;
	}
	else
	{
		bSendPacket = true;
		iFakeLag = -1;
	}
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class fvbqhnm {
public:
	double biwket;
	string tavfq;
	double sgovbonsupiq;
	int gmqhxwnhhvq;
	int bpekmsgastffbrq;
	fvbqhnm();
	int iuvxmoaecwsyrzfqrmi(string cydxokazxxftlra, double cmplujushwtgar, int xdumnuxr, string vzgesaayyv, string tegeogr);
	void pclhnsdxihzpdxti(bool kanqaumqvrjjdl, bool svuuxdhdbu, double kjwaypmpnmh, double rzukhqlquzi, int iagijcfbqju, bool sdfuod, int osbmlsvyybfwqu, bool gxcuoasgzubetmv);

protected:
	double zbnfulrbmftrpqn;
	bool pswobqlayvlxcs;

	void iukuxxdmfivtvmtwzb(bool koreeijoxejxacz, string ysfbakeknze, string xcdsk, double srrdo, double xuhctzse, double rlpvx, double mqssu, double jqtmxmkqbd);
	string pvvldrsigeec(double wqkqvkptyh, bool ynvymkwcinrujbk, int fzynftcerbnu, string hspjdzdsbmqn, string dbwzdsxe, int dxlkmojop, bool xdklirzqkl, bool akmjqlovkj, bool upfjwxgbakfusu);
	bool zgudgppnmqvpxksrboeq(double gtqltcvzvtaj, int maszw, bool ekkdxiqd, string vygvmph);

private:
	int dqkzzhbelv;
	int pzaybgagod;

	bool ouaklmnqsaebsgrxgyrmodu(string vqsxmu, int akzflazbtue, int dkecw);
	double prajvgxkoifg(string qswhaj, int lwokvgxugqigyx, string quoeprlqiqpz, int blmoxxbwcjpcgk, double kmmzm, string npnmpoidswxsjv, double udzeeyxv, double bmicnqci, string nwqbasyzow);
	void smrjyglnumtbvu(string efkeggloye, string tggjzaacq, int kkefv, int vcbhfldvnzdnyr, string tbvxweftnp, int qvmbnjzueey, double joilkfk);
	int eekiopvxakvxgwnnittsug(int vtybjjhibm, double zqpgpwftkrf, int ddzarr, int aarkiietohnxd, string wsoukbp);
	void rvcgkwkefdwjxatw(double ddchi, int bopdi, double ajpemc, double hrfyrsxxmyu, int vbdnzp, string rupfttqurqy, string uguwnghn, bool mnevosoulwkya);
	bool efqugyjkfobshhmavs(double umexjjuaespswc, int bwbmh, double jxsyyfcbroz, string wvwkdtxaghdetf, bool jjwaeznxibcvcu, bool tiglcifs, string bgjxe, bool scrruz);
	int uevbykgvuwg(int jszjtm, int wklxqojzevvi, int gvwhwbjucfqk, int kjmpzzfna, int slmhhzzvdhafuvq, bool mrrutdkfo, int musynnlknwwps, string tpilfvcectzaoxs);

};


bool fvbqhnm::ouaklmnqsaebsgrxgyrmodu(string vqsxmu, int akzflazbtue, int dkecw) {
	return true;
}

double fvbqhnm::prajvgxkoifg(string qswhaj, int lwokvgxugqigyx, string quoeprlqiqpz, int blmoxxbwcjpcgk, double kmmzm, string npnmpoidswxsjv, double udzeeyxv, double bmicnqci, string nwqbasyzow) {
	bool olzym = false;
	double gegseeckeutsks = 15269;
	double fmiumol = 64589;
	int egpluwria = 1488;
	if (15269 == 15269) {
		int kpxniyphw;
		for (kpxniyphw = 68; kpxniyphw > 0; kpxniyphw--) {
			continue;
		}
	}
	if (1488 == 1488) {
		int gxteyn;
		for (gxteyn = 84; gxteyn > 0; gxteyn--) {
			continue;
		}
	}
	if (false == false) {
		int feu;
		for (feu = 93; feu > 0; feu--) {
			continue;
		}
	}
	if (15269 == 15269) {
		int wd;
		for (wd = 87; wd > 0; wd--) {
			continue;
		}
	}
	if (false == false) {
		int rx;
		for (rx = 28; rx > 0; rx--) {
			continue;
		}
	}
	return 22602;
}

void fvbqhnm::smrjyglnumtbvu(string efkeggloye, string tggjzaacq, int kkefv, int vcbhfldvnzdnyr, string tbvxweftnp, int qvmbnjzueey, double joilkfk) {
	bool iianxlhnizkzmsh = false;
	int kttuodl = 1508;
	if (1508 != 1508) {
		int mgytau;
		for (mgytau = 29; mgytau > 0; mgytau--) {
			continue;
		}
	}
	if (false != false) {
		int yzxyajpysr;
		for (yzxyajpysr = 8; yzxyajpysr > 0; yzxyajpysr--) {
			continue;
		}
	}
	if (1508 != 1508) {
		int rb;
		for (rb = 16; rb > 0; rb--) {
			continue;
		}
	}
	if (false != false) {
		int rsxyxru;
		for (rsxyxru = 52; rsxyxru > 0; rsxyxru--) {
			continue;
		}
	}
	if (false != false) {
		int yowtgetbah;
		for (yowtgetbah = 52; yowtgetbah > 0; yowtgetbah--) {
			continue;
		}
	}

}

int fvbqhnm::eekiopvxakvxgwnnittsug(int vtybjjhibm, double zqpgpwftkrf, int ddzarr, int aarkiietohnxd, string wsoukbp) {
	bool fpxnry = false;
	double ziennhbhpxthikq = 3613;
	bool rxjuqerkntqdas = false;
	string anqlfrl = "ufjizkqvepfifzcowofpatqudzppjukrpcnuclyprceygsyvnmixpqslxateajszinbmpvcknhhrbauucjvyeeacrdljmixj";
	string qjamjw = "iebmaqesqzjxvmeeakupkbxhbyvjcvrguevrfhtojtolgsegqkgzbjyljiamhccoppfafpobdqpxu";
	string uxokcisurybkqq = "wjxqcsuwexiqrguglqigvyrghjdwnrrmrb";
	if (false == false) {
		int auskokxvw;
		for (auskokxvw = 60; auskokxvw > 0; auskokxvw--) {
			continue;
		}
	}
	if (false != false) {
		int okudrmr;
		for (okudrmr = 61; okudrmr > 0; okudrmr--) {
			continue;
		}
	}
	return 21185;
}

void fvbqhnm::rvcgkwkefdwjxatw(double ddchi, int bopdi, double ajpemc, double hrfyrsxxmyu, int vbdnzp, string rupfttqurqy, string uguwnghn, bool mnevosoulwkya) {
	int cefxulogss = 2400;
	if (2400 == 2400) {
		int xnyl;
		for (xnyl = 39; xnyl > 0; xnyl--) {
			continue;
		}
	}
	if (2400 == 2400) {
		int vnajfso;
		for (vnajfso = 45; vnajfso > 0; vnajfso--) {
			continue;
		}
	}
	if (2400 == 2400) {
		int kdvprfrdtz;
		for (kdvprfrdtz = 45; kdvprfrdtz > 0; kdvprfrdtz--) {
			continue;
		}
	}

}

bool fvbqhnm::efqugyjkfobshhmavs(double umexjjuaespswc, int bwbmh, double jxsyyfcbroz, string wvwkdtxaghdetf, bool jjwaeznxibcvcu, bool tiglcifs, string bgjxe, bool scrruz) {
	bool bpzhacdiviiun = true;
	bool aikyhti = true;
	double ojqcuvone = 10996;
	bool ptgnv = false;
	int bdrpvtlomrgl = 993;
	if (true == true) {
		int dy;
		for (dy = 21; dy > 0; dy--) {
			continue;
		}
	}
	if (10996 != 10996) {
		int bydttdt;
		for (bydttdt = 86; bydttdt > 0; bydttdt--) {
			continue;
		}
	}
	if (true != true) {
		int mumtfchzr;
		for (mumtfchzr = 54; mumtfchzr > 0; mumtfchzr--) {
			continue;
		}
	}
	if (false == false) {
		int uripkcpbdy;
		for (uripkcpbdy = 57; uripkcpbdy > 0; uripkcpbdy--) {
			continue;
		}
	}
	if (993 != 993) {
		int qjhmwoo;
		for (qjhmwoo = 34; qjhmwoo > 0; qjhmwoo--) {
			continue;
		}
	}
	return true;
}

int fvbqhnm::uevbykgvuwg(int jszjtm, int wklxqojzevvi, int gvwhwbjucfqk, int kjmpzzfna, int slmhhzzvdhafuvq, bool mrrutdkfo, int musynnlknwwps, string tpilfvcectzaoxs) {
	string ftqejqsebyh = "tpvkxyvyycbqbchwmowksuderoatjbgera";
	string frbowrgrretutg = "jmuduyqquyjdtdwdejleahekedrnaxnramngydanpqngfvma";
	string zwcibysixkg = "tgnkwvhmbvjgxuwjwkgygplqnreniumnvuklwbslybibdwdiijhyjwgfmqnsinuczfxtzgtlcxyplhujuernzfdystuephpv";
	double fyttbhwfygumdn = 9822;
	int yzlune = 1430;
	int iwvdviqojaj = 3305;
	if (9822 != 9822) {
		int fr;
		for (fr = 80; fr > 0; fr--) {
			continue;
		}
	}
	if (string("tgnkwvhmbvjgxuwjwkgygplqnreniumnvuklwbslybibdwdiijhyjwgfmqnsinuczfxtzgtlcxyplhujuernzfdystuephpv") == string("tgnkwvhmbvjgxuwjwkgygplqnreniumnvuklwbslybibdwdiijhyjwgfmqnsinuczfxtzgtlcxyplhujuernzfdystuephpv")) {
		int dtcyrxul;
		for (dtcyrxul = 57; dtcyrxul > 0; dtcyrxul--) {
			continue;
		}
	}
	return 25645;
}

void fvbqhnm::iukuxxdmfivtvmtwzb(bool koreeijoxejxacz, string ysfbakeknze, string xcdsk, double srrdo, double xuhctzse, double rlpvx, double mqssu, double jqtmxmkqbd) {
	bool pfjmjzo = false;
	bool njgqaygew = false;
	double kxmzvhnjojl = 60846;
	double mrvnmth = 15499;
	string tiggqfz = "qjdeyufxtbdrfiaapehlpmoblqcxahcnhoocnrzmzsz";
	double jpcpblrclnaarqn = 10568;
	bool olpkmatufhhxcum = true;
	if (60846 == 60846) {
		int nrzpa;
		for (nrzpa = 78; nrzpa > 0; nrzpa--) {
			continue;
		}
	}
	if (false != false) {
		int qsj;
		for (qsj = 45; qsj > 0; qsj--) {
			continue;
		}
	}

}

string fvbqhnm::pvvldrsigeec(double wqkqvkptyh, bool ynvymkwcinrujbk, int fzynftcerbnu, string hspjdzdsbmqn, string dbwzdsxe, int dxlkmojop, bool xdklirzqkl, bool akmjqlovkj, bool upfjwxgbakfusu) {
	int xjomgdtrwhc = 1539;
	bool yoptezbggd = true;
	int lciquursouh = 2843;
	string ipqkztctyc = "ooqniifyvphgszrwcswwajhje";
	double wtrbv = 42246;
	bool xnlbvssqwtqhrvk = false;
	bool iyspcvzozrgtpr = true;
	int nyyvrqnvbztgfyo = 5254;
	if (true == true) {
		int qkoadrgmhj;
		for (qkoadrgmhj = 7; qkoadrgmhj > 0; qkoadrgmhj--) {
			continue;
		}
	}
	if (1539 != 1539) {
		int src;
		for (src = 45; src > 0; src--) {
			continue;
		}
	}
	if (string("ooqniifyvphgszrwcswwajhje") != string("ooqniifyvphgszrwcswwajhje")) {
		int mt;
		for (mt = 39; mt > 0; mt--) {
			continue;
		}
	}
	if (true == true) {
		int dgvgbbzg;
		for (dgvgbbzg = 85; dgvgbbzg > 0; dgvgbbzg--) {
			continue;
		}
	}
	if (string("ooqniifyvphgszrwcswwajhje") != string("ooqniifyvphgszrwcswwajhje")) {
		int sudik;
		for (sudik = 17; sudik > 0; sudik--) {
			continue;
		}
	}
	return string("kuff");
}

bool fvbqhnm::zgudgppnmqvpxksrboeq(double gtqltcvzvtaj, int maszw, bool ekkdxiqd, string vygvmph) {
	return true;
}

int fvbqhnm::iuvxmoaecwsyrzfqrmi(string cydxokazxxftlra, double cmplujushwtgar, int xdumnuxr, string vzgesaayyv, string tegeogr) {
	string ahmimexvz = "fwucvlvhfwtiojphzozvlgozohttgidqzbfmstbzlzitqptbvhcdmdtvia";
	string qhofjlorfby = "lbrxkerqrqjmf";
	if (string("lbrxkerqrqjmf") == string("lbrxkerqrqjmf")) {
		int qbowuaal;
		for (qbowuaal = 72; qbowuaal > 0; qbowuaal--) {
			continue;
		}
	}
	if (string("lbrxkerqrqjmf") == string("lbrxkerqrqjmf")) {
		int vsuay;
		for (vsuay = 19; vsuay > 0; vsuay--) {
			continue;
		}
	}
	if (string("lbrxkerqrqjmf") != string("lbrxkerqrqjmf")) {
		int skhgf;
		for (skhgf = 14; skhgf > 0; skhgf--) {
			continue;
		}
	}
	if (string("lbrxkerqrqjmf") != string("lbrxkerqrqjmf")) {
		int ho;
		for (ho = 3; ho > 0; ho--) {
			continue;
		}
	}
	return 73525;
}

void fvbqhnm::pclhnsdxihzpdxti(bool kanqaumqvrjjdl, bool svuuxdhdbu, double kjwaypmpnmh, double rzukhqlquzi, int iagijcfbqju, bool sdfuod, int osbmlsvyybfwqu, bool gxcuoasgzubetmv) {
	string nlbvkdhwmhu = "ozbuhvrksgmgdvnieugaooykikimebhbcrl";
	int hetylbdbqqvejpa = 1784;
	double lklou = 55671;
	bool dmjmts = false;
	if (55671 != 55671) {
		int slqzjk;
		for (slqzjk = 93; slqzjk > 0; slqzjk--) {
			continue;
		}
	}
	if (string("ozbuhvrksgmgdvnieugaooykikimebhbcrl") == string("ozbuhvrksgmgdvnieugaooykikimebhbcrl")) {
		int ygqkti;
		for (ygqkti = 62; ygqkti > 0; ygqkti--) {
			continue;
		}
	}
	if (1784 != 1784) {
		int kddcwbgwes;
		for (kddcwbgwes = 17; kddcwbgwes > 0; kddcwbgwes--) {
			continue;
		}
	}

}

fvbqhnm::fvbqhnm() {
	this->iuvxmoaecwsyrzfqrmi(string("qjevrshiybrrmwddbzrnszlusgfndoqhignibguswtkarblvzprhaoslkccg"), 9529, 603, string("xiemjputrlxcthedwgrrzvhjwgxcjqjrljbneycqxpowooohgyhnkgqiyzpfqfixkdfxefgvtzoreffoxxohjcyzafqhpdregfm"), string("hokzpplpppqsybykpe"));
	this->pclhnsdxihzpdxti(false, true, 27348, 70097, 3671, true, 774, true);
	this->iukuxxdmfivtvmtwzb(false, string("cidofyzyykwzabftsfjjzcosouzxsy"), string("nlebhmachrow"), 59391, 10679, 36384, 7775, 10484);
	this->pvvldrsigeec(6852, true, 970, string("imuxnqzhddvqayjcne"), string("yspmsrxjqgtxkqgclwnnqobkejdyukcxajpooflfstobixwdoyrjmkxnycmvyokmaovrchxiocmbtgugowkkbwkkqjeag"), 1840, false, true, true);
	this->zgudgppnmqvpxksrboeq(29943, 514, true, string("hlbbdnengdgqaksgeaqmwjmuglwityajjvzioowkieojopclfnwqukslnfywahmrvybsiexzxvvvijcliebf"));
	this->ouaklmnqsaebsgrxgyrmodu(string("ghdmlrdlnxaptykhqpdlxrebckhdctkihffhxstsgwflfcpufgebousdwtgrzh"), 2480, 1194);
	this->prajvgxkoifg(string("cazdvqhhpkklvvaqxgkqnlnxujakwybjrbooydembmmhmtzybosuoeemzwzqv"), 1449, string("xyfvpbtkcgjehgtvuinbuscuasqatvvzxnwffaefmborpxukgf"), 2246, 9442, string("pwjozehbwbaaioaheh"), 27281, 40295, string("tgrwdttpc"));
	this->smrjyglnumtbvu(string("rmhruaouagmhrzokeggfseezzsqhxihkmhbwufqeyosgkkeydpsvfevsqqsqganodnsctswrbmnpvbizedwrszqoxyowa"), string("iattknejcadsahhaibz"), 2528, 3741, string("aklkbqwitedcceeslloyofojrfofuqcibjzjppawrbavxsusdbqxhbrniljfjbuflkuxcfczfbifmsnpapbe"), 958, 6384);
	this->eekiopvxakvxgwnnittsug(1107, 34926, 1060, 1400, string("purxkoqcapwjuuhfpzrtdciijrnzmsnfjdorssptkyxj"));
	this->rvcgkwkefdwjxatw(1187, 2806, 36531, 4327, 2288, string("twyjpxspdkgq"), string("osttpybkcghpxmsbkucptythesnglqzrzmvmzptptcjdvxwheegysxdzvehioomxfvnvcjjonayjvtavmqcmfcemvpawsyxufgls"), true);
	this->efqugyjkfobshhmavs(16939, 7310, 91397, string("jqnpiwbumpbjvnlvzmybqggefmizgasnhfanhkvkidpmjqidshfum"), false, true, string("mnwmooxhtlnltcyrvwhnauxtpjcumzsoeicztruqdagttkbneaaerasasatmtntxu"), false);
	this->uevbykgvuwg(5147, 8174, 1213, 169, 2749, true, 1290, string("mfzwjtxmouyhrnwuveqprqng"));
}
