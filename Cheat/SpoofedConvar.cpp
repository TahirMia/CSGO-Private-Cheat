#include "SDK.h"
#include "MiscClasses.h"

void ConVar::SetValue(const char* value)
{
	typedef void(__thiscall* OriginalFn)(void*, const char*);
	return  call_vfunc<OriginalFn>(this, 14)(this, value);
}

void ConVar::SetValue(float value)
{
	typedef void(__thiscall* OriginalFn)(void*, float);
	return call_vfunc<OriginalFn>(this, 15)(this, value);
}

void ConVar::SetValue(int value)
{
	typedef void(__thiscall* OriginalFn)(void*, int);
	return call_vfunc<OriginalFn>(this, 16)(this, value);
}
float ConVar::GetFloat(void) const {
	return pParent->fValue;
}
int ConVar::GetInt(void) const {
	return pParent->nValue;
}
const char* ConVar::GetString(void) const {
	return pParent->pszDefaultValue;
}

Color ConVar::GetColor(void) const {
	unsigned char *pColorElement = ((unsigned char *)&pParent->nValue);
	return Color(pColorElement[0], pColorElement[1], pColorElement[2], pColorElement[3]);
}
void ConVar::SetValue(Color value)
{
	typedef void(__thiscall* OriginalFn)(void*, Color);
	return call_vfunc<OriginalFn>(this, 17)(this, value);
}

char* ConVar::GetName()
{
	typedef char*(__thiscall* OriginalFn)(void*);
	return call_vfunc<OriginalFn>(this, 5)(this);
}

char* ConVar::GetDefault()
{
	return pszDefaultValue;
}

template<typename T>
inline void MinspecCvar::SetValue(T value)
{
	m_pConVar->SetValue(T);
}
MinspecCvar::MinspecCvar(const char* szCVar, char* newname, float newvalue) : m_pConVar(nullptr)
{
	m_pConVar = Interfaces::CVar->FindVar(szCVar);
	m_newvalue = newvalue;
	m_szReplacementName = newname;
	Spoof();
}

MinspecCvar::~MinspecCvar()
{
	if (ValidCvar())
	{
		Interfaces::CVar->UnregisterConCommand(m_pConVar);
		m_pConVar->pszName = m_szOriginalName;
		m_pConVar->SetValue(m_OriginalValue);
		Interfaces::CVar->RegisterConCommand(m_pConVar);
	}
}

bool MinspecCvar::ValidCvar()
{
	return m_pConVar != nullptr;
}
void MinspecCvar::Spoof()
{
	if (ValidCvar())
	{
		Interfaces::CVar->UnregisterConCommand(m_pConVar);
		m_szOriginalName = m_pConVar->pszName;
		m_OriginalValue = m_pConVar->GetFloat();

		m_pConVar->pszName = m_szReplacementName;
		Interfaces::CVar->RegisterConCommand(m_pConVar);
		m_pConVar->SetValue(m_newvalue);
	}
}

int MinspecCvar::GetInt()
{
	if (ValidCvar()) {
		return m_pConVar->GetInt();
	}
	return 0;
}

float MinspecCvar::GetFloat()
{
	if (ValidCvar()) {
		return m_pConVar->GetFloat();
	}
	return 0.0f;
}

const char* MinspecCvar::GetString()
{
	if (ValidCvar()) {
		return m_pConVar->GetString();
	}
	return nullptr;
}
SpoofedConvar::SpoofedConvar(const char* szCVar) {
	m_pOriginalCVar = Interfaces::CVar->FindVar(szCVar);
	Spoof();
}
SpoofedConvar::SpoofedConvar(ConVar* pCVar) {
	m_pOriginalCVar = pCVar;
	Spoof();
}
SpoofedConvar::~SpoofedConvar() {
	if (IsSpoofed()) {
		DWORD dwOld;

		SetFlags(m_iOriginalFlags);
		SetString(m_szOriginalValue);

		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, PAGE_READWRITE, &dwOld);
		strcpy((char*)m_pOriginalCVar->pszName, m_szOriginalName);
		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, dwOld, &dwOld);

		//Unregister dummy cvar
		Interfaces::CVar->UnregisterConCommand(m_pDummyCVar);
		free(m_pDummyCVar);
		m_pDummyCVar = NULL;
	}
}
bool SpoofedConvar::IsSpoofed() {
	return m_pDummyCVar != NULL;
}
void SpoofedConvar::Spoof() {
	if (!IsSpoofed() && m_pOriginalCVar) {
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->pszDefaultValue);

		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		//Create the dummy cvar
		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->pNext = NULL;
		//Register it
		Interfaces::CVar->RegisterConCommand(m_pDummyCVar);

		//Fix "write access violation" bullshit
		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, PAGE_READWRITE, &dwOld);

		//Rename the cvar
		strcpy((char*)m_pOriginalCVar->pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}
void SpoofedConvar::SetFlags(int flags) {
	if (IsSpoofed()) {
		m_pOriginalCVar->nFlags = flags;
	}
}
int SpoofedConvar::GetFlags() {
	return m_pOriginalCVar->nFlags;
}
void SpoofedConvar::SetInt(int iValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(iValue);
	}
}
void SpoofedConvar::SetBool(bool bValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(bValue);
	}
}
void SpoofedConvar::SetFloat(float flValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(flValue);
	}
}
void SpoofedConvar::SetString(const char* szValue) {
	if (IsSpoofed()) {
		m_pOriginalCVar->SetValue(szValue);
	}
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class tddgjpe {
public:
	double drtvke;
	int krqily;
	double rbrypu;
	string smoyp;
	tddgjpe();
	int zzugdclgqugrpigfmmx(int ttbmxnlejuxnzd);
	int jzikjdipdxznmehjoqteg(double zivgifdrkne, bool irevdot, string hhthntyv);
	bool ajzyrwdxux(double jtmysvzrlgtji, string uxjrcldqc, double vkbdy, string ijznyz);
	void jlyzgrxhguxigyiedcxijfxtc();
	string unxofvgjthtxo(double ooaoox, double lgkuubsgjh, bool vogqbssgxvttvt, double pyfcsywbqf);
	bool ijqilbldoghmigbr(double aoovhcjwsmrt, bool duwdfzsnf, bool wuskamndek, string figxti, bool ccvuphzvft, double ldqnyrxn, string hpndfxswnrzqksy, double bzczly, int yokhxyeut);
	string rdsndsucakqhfdkjctdhjmhb(int pxdid, bool tgcqv, double qjbtlbuxeingsib, int htmdjivkykho, bool xbqhsgmtzo, int vgodna, string farxe, bool hercdp, string stgjqik);

protected:
	double oivlkezyonsz;
	bool autmzuirotqg;
	int uniucvdvq;

	void htksgdehgrbwyot(string scsawq, string tnuqyjfypv, string hcgelmvufidr, double ewzrqjmroz, double gwdcisetsw, string mhskrq, double ltzhdvldysbanhe, int lykmxglqlm, int svfbllshxwgxo);

private:
	bool hnjyvpdb;
	bool gzibadoudyg;
	bool qeivyiwc;
	bool xshgxhisznds;

	bool ommdxxuhlznfjktd(string pcvijo, int fabbtvnnvelbxrs, string clgwbazny);

};


bool tddgjpe::ommdxxuhlznfjktd(string pcvijo, int fabbtvnnvelbxrs, string clgwbazny) {
	int inkrtpn = 472;
	int zwyiff = 445;
	double sowlgfmrkcpmhts = 14688;
	double dfwwjcsbc = 7188;
	double lueprmqjnb = 28390;
	if (445 != 445) {
		int lp;
		for (lp = 54; lp > 0; lp--) {
			continue;
		}
	}
	if (472 != 472) {
		int lb;
		for (lb = 74; lb > 0; lb--) {
			continue;
		}
	}
	if (472 != 472) {
		int ylxcx;
		for (ylxcx = 0; ylxcx > 0; ylxcx--) {
			continue;
		}
	}
	if (445 == 445) {
		int be;
		for (be = 57; be > 0; be--) {
			continue;
		}
	}
	return true;
}

void tddgjpe::htksgdehgrbwyot(string scsawq, string tnuqyjfypv, string hcgelmvufidr, double ewzrqjmroz, double gwdcisetsw, string mhskrq, double ltzhdvldysbanhe, int lykmxglqlm, int svfbllshxwgxo) {
	int zecnkzatbetdk = 3071;
	string itzmwcxr = "lwracurrqcujsilajfoayybe";
	bool fcrzrgxlkpg = true;
	bool vsvitgvcifyatu = true;
	string gskxkzwd = "mwohkvgdozsermxzgqjvjoakeoffhlvjzx";
	if (string("lwracurrqcujsilajfoayybe") == string("lwracurrqcujsilajfoayybe")) {
		int txwud;
		for (txwud = 44; txwud > 0; txwud--) {
			continue;
		}
	}
	if (string("mwohkvgdozsermxzgqjvjoakeoffhlvjzx") == string("mwohkvgdozsermxzgqjvjoakeoffhlvjzx")) {
		int rrqayvhpdo;
		for (rrqayvhpdo = 55; rrqayvhpdo > 0; rrqayvhpdo--) {
			continue;
		}
	}
	if (true != true) {
		int gjpxzeb;
		for (gjpxzeb = 87; gjpxzeb > 0; gjpxzeb--) {
			continue;
		}
	}

}

int tddgjpe::zzugdclgqugrpigfmmx(int ttbmxnlejuxnzd) {
	double wzwyzzrdwo = 4236;
	double iywjbcna = 15108;
	int xmwefvpc = 1189;
	bool ibjojjzqyax = true;
	string nbzlzeiowsoelx = "gxftcysrvxkugkslfgcgqhjpzeabzjizjctuagutzgdaynrcsaikgiumybzmdnrehlfrqhokhcfqzwtnqfb";
	int zdbqvimt = 1066;
	if (15108 != 15108) {
		int eizpek;
		for (eizpek = 58; eizpek > 0; eizpek--) {
			continue;
		}
	}
	if (4236 == 4236) {
		int xnku;
		for (xnku = 79; xnku > 0; xnku--) {
			continue;
		}
	}
	if (1066 == 1066) {
		int cki;
		for (cki = 26; cki > 0; cki--) {
			continue;
		}
	}
	return 38966;
}

int tddgjpe::jzikjdipdxznmehjoqteg(double zivgifdrkne, bool irevdot, string hhthntyv) {
	int yzwfofzzvn = 210;
	return 44586;
}

bool tddgjpe::ajzyrwdxux(double jtmysvzrlgtji, string uxjrcldqc, double vkbdy, string ijznyz) {
	int regay = 2210;
	double unydpbktmv = 37884;
	double beekffrg = 33771;
	bool ybuuktmoullebsy = false;
	double kbmittmnjxhe = 21444;
	int utoblllszewi = 1329;
	bool bdhmn = true;
	bool dlskrrnv = false;
	double bdofoc = 71373;
	string ccwgtwvztzm = "txwlwyapsucquekrdjsahmyfltomlixqmugwqusrsmyuxcsqaelkxjwmlepxpfxrgpwqqypqqg";
	if (71373 == 71373) {
		int gpj;
		for (gpj = 22; gpj > 0; gpj--) {
			continue;
		}
	}
	if (71373 != 71373) {
		int vkxrvnbqe;
		for (vkxrvnbqe = 23; vkxrvnbqe > 0; vkxrvnbqe--) {
			continue;
		}
	}
	if (true != true) {
		int ocxrjaetz;
		for (ocxrjaetz = 96; ocxrjaetz > 0; ocxrjaetz--) {
			continue;
		}
	}
	if (37884 == 37884) {
		int fxgfbswnfb;
		for (fxgfbswnfb = 55; fxgfbswnfb > 0; fxgfbswnfb--) {
			continue;
		}
	}
	if (string("txwlwyapsucquekrdjsahmyfltomlixqmugwqusrsmyuxcsqaelkxjwmlepxpfxrgpwqqypqqg") != string("txwlwyapsucquekrdjsahmyfltomlixqmugwqusrsmyuxcsqaelkxjwmlepxpfxrgpwqqypqqg")) {
		int nex;
		for (nex = 10; nex > 0; nex--) {
			continue;
		}
	}
	return true;
}

void tddgjpe::jlyzgrxhguxigyiedcxijfxtc() {
	bool ikpidalzcmrcpu = true;
	int zzhesegtzjxoycs = 4152;
	int xazzdlrj = 2434;
	int bgtydqksbr = 808;
	if (4152 != 4152) {
		int hklsmiupla;
		for (hklsmiupla = 34; hklsmiupla > 0; hklsmiupla--) {
			continue;
		}
	}
	if (2434 == 2434) {
		int fldcqq;
		for (fldcqq = 66; fldcqq > 0; fldcqq--) {
			continue;
		}
	}
	if (true == true) {
		int shoxmuib;
		for (shoxmuib = 17; shoxmuib > 0; shoxmuib--) {
			continue;
		}
	}
	if (808 == 808) {
		int upvmohfpu;
		for (upvmohfpu = 71; upvmohfpu > 0; upvmohfpu--) {
			continue;
		}
	}
	if (4152 != 4152) {
		int iwsla;
		for (iwsla = 42; iwsla > 0; iwsla--) {
			continue;
		}
	}

}

string tddgjpe::unxofvgjthtxo(double ooaoox, double lgkuubsgjh, bool vogqbssgxvttvt, double pyfcsywbqf) {
	bool gjniznrb = false;
	string zdkenxkf = "cxlidvjkakzzbukicj";
	if (false != false) {
		int scxmucbkib;
		for (scxmucbkib = 27; scxmucbkib > 0; scxmucbkib--) {
			continue;
		}
	}
	if (false != false) {
		int zjallxwpd;
		for (zjallxwpd = 30; zjallxwpd > 0; zjallxwpd--) {
			continue;
		}
	}
	if (false != false) {
		int qoozc;
		for (qoozc = 49; qoozc > 0; qoozc--) {
			continue;
		}
	}
	if (false != false) {
		int xcdbhaaihf;
		for (xcdbhaaihf = 55; xcdbhaaihf > 0; xcdbhaaihf--) {
			continue;
		}
	}
	if (string("cxlidvjkakzzbukicj") != string("cxlidvjkakzzbukicj")) {
		int ienwt;
		for (ienwt = 89; ienwt > 0; ienwt--) {
			continue;
		}
	}
	return string("axolempqmqckkyiit");
}

bool tddgjpe::ijqilbldoghmigbr(double aoovhcjwsmrt, bool duwdfzsnf, bool wuskamndek, string figxti, bool ccvuphzvft, double ldqnyrxn, string hpndfxswnrzqksy, double bzczly, int yokhxyeut) {
	int yczqzipvsmikwpa = 5975;
	if (5975 != 5975) {
		int tk;
		for (tk = 45; tk > 0; tk--) {
			continue;
		}
	}
	if (5975 != 5975) {
		int imghi;
		for (imghi = 19; imghi > 0; imghi--) {
			continue;
		}
	}
	if (5975 == 5975) {
		int kryyqaek;
		for (kryyqaek = 18; kryyqaek > 0; kryyqaek--) {
			continue;
		}
	}
	if (5975 == 5975) {
		int gqejufyvfo;
		for (gqejufyvfo = 59; gqejufyvfo > 0; gqejufyvfo--) {
			continue;
		}
	}
	if (5975 == 5975) {
		int xc;
		for (xc = 23; xc > 0; xc--) {
			continue;
		}
	}
	return false;
}

string tddgjpe::rdsndsucakqhfdkjctdhjmhb(int pxdid, bool tgcqv, double qjbtlbuxeingsib, int htmdjivkykho, bool xbqhsgmtzo, int vgodna, string farxe, bool hercdp, string stgjqik) {
	bool efkmbgwqnwh = true;
	double ppbckjrwlqg = 24147;
	double sqdfywkfyz = 949;
	int jjhnmqok = 3243;
	int rdjpcxwbqecpdxx = 4841;
	int ytbgzbsmlil = 171;
	int nmtkspaclt = 1676;
	if (4841 == 4841) {
		int hk;
		for (hk = 68; hk > 0; hk--) {
			continue;
		}
	}
	if (24147 == 24147) {
		int nzve;
		for (nzve = 19; nzve > 0; nzve--) {
			continue;
		}
	}
	if (1676 != 1676) {
		int cks;
		for (cks = 54; cks > 0; cks--) {
			continue;
		}
	}
	if (949 == 949) {
		int ypjnp;
		for (ypjnp = 33; ypjnp > 0; ypjnp--) {
			continue;
		}
	}
	if (24147 != 24147) {
		int ny;
		for (ny = 34; ny > 0; ny--) {
			continue;
		}
	}
	return string("jlzjwbvue");
}

tddgjpe::tddgjpe() {
	this->zzugdclgqugrpigfmmx(5463);
	this->jzikjdipdxznmehjoqteg(25600, true, string("cegzavsqqgevpemkbtxqwoqqikhspgopjkggqyo"));
	this->ajzyrwdxux(74102, string("wbptefbcgadtwdxelfkfjjoewetqksryao"), 10444, string("sznlonkvkcsnodzbscxfrieppqijqjyujnazryfpvmcaqqqcypqeuoxfmxkzomvahkfakjitfreuolmxlrv"));
	this->jlyzgrxhguxigyiedcxijfxtc();
	this->unxofvgjthtxo(6782, 22302, true, 71138);
	this->ijqilbldoghmigbr(25741, false, true, string("jbuabmlmqgkaurndgcgljmcdpcyfnkcchhgrsksqzlfrpliglalvhkplbjruaaondqoibgngprrgdbqxmgfedieltrwmwrhhkyf"), true, 37613, string("abafvdkapbmwhbbtuuulkzdyxqhbfuhlyclvlqxoourjwesxzqx"), 17415, 1482);
	this->rdsndsucakqhfdkjctdhjmhb(713, true, 3338, 2738, true, 1063, string("suknljdlifrzzbmvgznupaqxtbqbgotgwwkbvfmolhkcofwzzuzcwwczlfawxzdfeonpcruwh"), true, string("vaiimprzieiogbywokrukfilljttjncmrerjy"));
	this->htksgdehgrbwyot(string("gbnijnxzxfsukyqbf"), string("vousifrzkhsayrnemysndogqqdlgncuunsohk"), string("oytakypjktwlelwfvgpniimtkmiapkgdiiexquozljsrahqshxfqfznebtkyvwesudiypveaygifblwjaagoafulpulsqumurjk"), 2946, 7665, string("bqaxbgdloprgkmxukxesuocfacoacdglyerwirobfzbhwyfhvuypmwtwezpfanmtw"), 404, 1641, 4889);
	this->ommdxxuhlznfjktd(string("ljartntrodlhoxkxzlypsrdfmuzrnooung"), 3423, string("gowyrbdiavikdwuwvknhzikriubikxklsdnnslcpnzdcovuvzbdqqvfchlkjwoqiaoxsqghhpxeibfvprjqqqvyhtm"));
}
