/*
Syn's AyyWare Framework
*/
#pragma once
#define _CRT_SECURE_NO_WARNINGS

// Includes
#include "Utilities.h"
#include <fstream>
#include <Psapi.h>

bool FileLog = false;
std::ofstream logFile;

// --------         Utilities Core           ------------ //
// Opens a debug console
void Utilities::OpenConsole(std::string Title)
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	SetConsoleTitle(Title.c_str());
}

// Closes the debug console
void Utilities::CloseConsole()
{
	FreeConsole();
}

// Outputs text to the console
void Utilities::Log(const char *fmt, ...)
{
	if (!fmt) return; //if the passed string is null return
	if (strlen(fmt) < 2) return;

	//Set up va_list and buffer to hold the params 
	va_list va_alist;
	char logBuf[256] = { 0 };

	//Do sprintf with the parameters
	va_start(va_alist, fmt);
	_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
	va_end(va_alist);

	//Output to console
	if (logBuf[0] != '\0')
	{
		SetConsoleColor(FOREGROUND_INTENSE_GREEN);
		printf("[%s]", GetTimeString().c_str());
		SetConsoleColor(FOREGROUND_WHITE);
		printf(": %s\n", logBuf);
	}

	if (FileLog)
	{
		logFile << logBuf << std::endl;
	}
}

// Gets the current time as a string
std::string Utilities::GetTimeString()
{
	//Time related variables
	time_t current_time;
	struct tm *time_info;
	static char timeString[10];

	//Get current time
	time(&current_time);
	time_info = localtime(&current_time);

	//Get current time as string
	strftime(timeString, sizeof(timeString), "%I:%M%p", time_info);
	return timeString;
}

// Sets the console color for upcoming text
void Utilities::SetConsoleColor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Enables writing all log calls to a file
void Utilities::EnableLogFile(std::string filename)
{
	logFile.open(filename.c_str());
	if (logFile.is_open())
		FileLog = true;
}


// --------         Utilities Memory           ------------ //

DWORD Utilities::Memory::WaitOnModuleHandle(std::string moduleName)
{
	DWORD ModuleHandle = NULL;
	while (!ModuleHandle)
	{
		ModuleHandle = (DWORD)GetModuleHandle(moduleName.c_str());
		if (!ModuleHandle)
			Sleep(50);
	}
	return ModuleHandle;
}

bool bCompare(const BYTE* Data, const BYTE* Mask, const char* szMask)
{
	for (; *szMask; ++szMask, ++Mask, ++Data)
	{
		if (*szMask == 'x' && *Mask != *Data)
		{
			return false;
		}
	}
	return (*szMask) == 0;
}

DWORD Utilities::Memory::FindPattern(std::string moduleName, BYTE* Mask, char* szMask)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;
	for (DWORD c = 0; c < Length; c += 1)
	{
		if (bCompare((BYTE*)(Address + c), Mask, szMask))
		{
			return (DWORD)(Address + c);
		}
	}
	return 0;
}

DWORD Utilities::Memory::FindTextPattern(std::string moduleName, char* string)
{
	DWORD Address = WaitOnModuleHandle(moduleName.c_str());
	MODULEINFO ModInfo; GetModuleInformation(GetCurrentProcess(), (HMODULE)Address, &ModInfo, sizeof(MODULEINFO));
	DWORD Length = ModInfo.SizeOfImage;

	int len = strlen(string);
	char* szMask = new char[len + 1];
	for (int i = 0; i < len; i++)
	{
		szMask[i] = 'x';
	}
	szMask[len] = '\0';

	for (DWORD c = 0; c < Length; c += 1)
	{
		if (bCompare((BYTE*)(Address + c), (BYTE*)string, szMask))
		{
			return (DWORD)(Address + c);
		}
	}
	return 0;
}

// --------         Utilities Memory VMT Manager       ------------ //

bool	Utilities::Memory::VMTManager::Initialise(DWORD* InstancePointer)
{
	// Store the instance pointers and such, and work out how big the table is
	Instance = InstancePointer;
	OriginalTable = (DWORD*)*InstancePointer;
	int VMTSize = MethodCount(InstancePointer);
	size_t TableBytes = VMTSize * 4;

	// Allocate some memory and copy the table
	CustomTable = (DWORD*)malloc(TableBytes + 8);
	if (!CustomTable) return false;
	memcpy((void*)CustomTable, (void*)OriginalTable, VMTSize * 4);

	// Change the pointer
	*InstancePointer = (DWORD)CustomTable;

	initComplete = true;
	return true;
}

int		Utilities::Memory::VMTManager::MethodCount(DWORD* InstancePointer)
{
	DWORD *VMT = (DWORD*)*InstancePointer;
	int Index = 0;
	int Amount = 0;
	while (!IsBadCodePtr((FARPROC)VMT[Index]))
	{
		if (!IsBadCodePtr((FARPROC)VMT[Index]))
		{
			Amount++;
			Index++;
		}
	}

	return Amount;
}

DWORD	Utilities::Memory::VMTManager::HookMethod(DWORD NewFunction, int Index)
{
	if (initComplete)
	{
		CustomTable[Index] = NewFunction;
		return OriginalTable[Index];
	}
	else
		return NULL;
}

void	Utilities::Memory::VMTManager::UnhookMethod(int Index)
{
	if (initComplete)
		CustomTable[Index] = OriginalTable[Index];
	return;
}

void	Utilities::Memory::VMTManager::RestoreOriginal()
{
	if (initComplete)
	{
		*Instance = (DWORD)OriginalTable;
	}
	return;
}

void	Utilities::Memory::VMTManager::RestoreCustom()
{
	if (initComplete)
	{
		*Instance = (DWORD)CustomTable;
	}
	return;
}

DWORD	Utilities::Memory::VMTManager::GetOriginalFunction(int Index)
{
	return OriginalTable[Index];
}

//JUNK CODE STARTS HERE!!

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jwwnqoq {
public:
	bool dfbomvudt;
	double otlfb;
	jwwnqoq();
	double ntmhpzytykbmbbik(string nvqil, double vzjrdobobl, int edgvzaortlja);
	void ytmhibkufxroixyhrulp(int ybqgb, string tbegjpfrh, double sbjzo, string solwl, string prfksus, bool glhonjhouw, bool maygibxqzp, int mlhrwczkmuqtom, string wylvbysbq, int avkjxfyaiaqn);
	int hhgzevbednuvsmdif(string tbrhfmk, string wamatxtsvwj, bool vbxoyxa, string ljfknsvphjrgfs, string msvuws);
	int xdelqygyvjkswwsehxsec(bool nmgclejeozbk, string qovvaszqo, bool ocypyhaokcxl, double ipniwuzn, string ukzuystgq, bool zvobgwmbklifytf, string rawjsuub, double bjzyn, int nrkkxxmwrh, double rryothgu);
	string npvmyzojgwu(string zlaxbup, int wwgygdw, bool eovvxcfebwwjn, bool xbmxldljp, string qkuvfvqpf);
	double bwazcswnioehx(bool lmvckkodzaiom, bool pyuihqou, int eojsrdqoeznuk, int adpqjf, int ufhuwyvd, string jhuqqmfdzxm, double byyne, int gvjinnv, string dfpnpxjynoeb);
	double zscxalwdrmuwiy(int yhraxqztclgj, bool gwfelciw);

protected:
	double pehdplsrmrazmym;
	string pprfrqvw;
	bool plxzdemwgfo;
	bool xnqbuxzbp;
	int ueoqkqthbiflx;

	double qfzjdwgril(double espievmd, double fxszf, int wrrfyfghpgj, string berpsvukigqjeh);
	int qfvxryekdilnnzxrqdeyozso(bool bfrmanbtinmp);
	double velasnktthuetfcdlcrj(bool kdryoabseanvwa, string dqmikjmczabs, string tyrsauazc, int xlophgirt, string badanog, bool rinllagogeyvjr);
	int evifxgpbqqhjgzlleqhcucq(string zvycrpx, double bjrvyrt, bool gftth, double uacnsxao);
	double uxgpttenap(int mxrunfuskki);

private:
	int otsujyxpzfez;
	int ilsuakngts;
	string pzcyarqyhdtqun;
	int nmtbugllmemanq;
	bool lluhl;

	bool hrnxjhvmppgyvps(bool tyfaxwnrqfw, double nbezyfxblak, bool wygwdugkzzy, bool orkeqjtngo, int cgaoroafbnfwfvr, bool vnxbjcddpre, bool omgxcackkskukdy);
	bool bbvewfyhrweexxqx(double jddorepnbq, string zeoxo, bool pflhd, int mbkgxsrfu, bool aweeki, string hrsgrrjvuovu, string syopiicq, int sdiluffr, string ksqbsnmzjry, double yqpjcad);
	int paasfmijypaqmuprkfqwmfibg(double eebdotkrmn, int sfyjl, int dzpvqgvkc, double nhmwvdkemhf, bool nstamtqwctvjmj, bool iltexdjqpqclfh, bool gixjkbyrvcmf, bool wvwhbkkdm, string rehcbikypookvbg, double ucofaxjixmihyy);
	double varmwjaeiihjqhxshj(double kiklfm, int rxrnkixbadsbpkc, double cfyivnj, string itywfpsuywoea);
	bool kfxbhxzdcakctsilgtvmoqf(bool cgijzezbhzzpu, bool ncsbmneh, double gtuobnhlldld, double lfqjmq, int aphvddrvnjcl, double obape, string nwzkdvppev, int jrbhgdvufpmbyg, string bogdkklftfvk, bool pdhxwycumkcna);
	string mcqiiazigdcjprtx();
	bool orfberqkandjinajl(bool pxzmslpqu, double vievkpykmgdxij, double btnvdqiqoah, string ouqnre, bool smnvppvpk, int terzcwpwxfhse, string jqvdqyrpnhbfpje, int tzutq, double enxfmlqaffnkpbu, bool fkhryt);
	bool objrtirrfpgyevjbrohla(double iwuxxw);
	void bsnrayilueqyezb(int brqjjlxhn, bool nmtpyayp, double huruke, string tnvgaigatjkggi, string clqovecxz, string ghzlhjahsksusb, string igifz, bool amreuo, string oqzbdmeyuqneb, string czswvuckoa);

};


bool jwwnqoq::hrnxjhvmppgyvps(bool tyfaxwnrqfw, double nbezyfxblak, bool wygwdugkzzy, bool orkeqjtngo, int cgaoroafbnfwfvr, bool vnxbjcddpre, bool omgxcackkskukdy) {
	string mezflogk = "esqjvtamavjiufbuljcihyavqutugvctgvixkhzqoeqrekrusgauqealei";
	bool hfzcogaleq = false;
	string hqrpcyinefl = "mqmobvydxcdzdbnkqqumwtaastnqjznsbanypgbypabbhmmoqbkxnximwrrrkoglnricdmonggztzhjezddbvifsygr";
	double doyhipm = 27735;
	string faqkxyndtgnbz = "csvfjfrvyrppejalfiermxhyfkcmillrkytsyirfzpzhllrzdxmfocjmilsa";
	string hvauk = "sbrxjeayagceqozlwiqyapbsjfxjwvfbxpxinjrikjxvtvsbyvrxes";
	int gamnzvfzrslst = 1423;
	string ywgxhlhw = "qjxmpwzkvkapwyzyyxpaggrqqqtbjroldobevbgzigkequmgeppbrpzhclxzzazluqbsqq";
	int qyvcegihzki = 3008;
	if (string("csvfjfrvyrppejalfiermxhyfkcmillrkytsyirfzpzhllrzdxmfocjmilsa") != string("csvfjfrvyrppejalfiermxhyfkcmillrkytsyirfzpzhllrzdxmfocjmilsa")) {
		int anwgd;
		for (anwgd = 42; anwgd > 0; anwgd--) {
			continue;
		}
	}
	if (string("qjxmpwzkvkapwyzyyxpaggrqqqtbjroldobevbgzigkequmgeppbrpzhclxzzazluqbsqq") != string("qjxmpwzkvkapwyzyyxpaggrqqqtbjroldobevbgzigkequmgeppbrpzhclxzzazluqbsqq")) {
		int qxnwkfhsp;
		for (qxnwkfhsp = 35; qxnwkfhsp > 0; qxnwkfhsp--) {
			continue;
		}
	}
	return true;
}

bool jwwnqoq::bbvewfyhrweexxqx(double jddorepnbq, string zeoxo, bool pflhd, int mbkgxsrfu, bool aweeki, string hrsgrrjvuovu, string syopiicq, int sdiluffr, string ksqbsnmzjry, double yqpjcad) {
	bool wigvesjzvv = false;
	string oeczsnbzstahy = "muarpmewwbnpzhffcpxn";
	double ozpiv = 233;
	int hflwwestheezj = 334;
	double acauxizdek = 6137;
	double vrsmiwrnhmlh = 63930;
	int eldqmm = 521;
	if (false != false) {
		int cztmqtzy;
		for (cztmqtzy = 87; cztmqtzy > 0; cztmqtzy--) {
			continue;
		}
	}
	if (false == false) {
		int vewt;
		for (vewt = 33; vewt > 0; vewt--) {
			continue;
		}
	}
	if (233 != 233) {
		int rsj;
		for (rsj = 86; rsj > 0; rsj--) {
			continue;
		}
	}
	return true;
}

int jwwnqoq::paasfmijypaqmuprkfqwmfibg(double eebdotkrmn, int sfyjl, int dzpvqgvkc, double nhmwvdkemhf, bool nstamtqwctvjmj, bool iltexdjqpqclfh, bool gixjkbyrvcmf, bool wvwhbkkdm, string rehcbikypookvbg, double ucofaxjixmihyy) {
	bool iutrfsgxxxaa = true;
	int lzfprxozuzdi = 1669;
	bool svmutaxhlyw = true;
	string yejgyhbv = "owtqutfrtorcqeeyccrzyslwikk";
	bool aensbmntijtjm = false;
	double dpesatcdwl = 34918;
	double exolcrtfnbfvx = 42198;
	double iqxkapxk = 22902;
	int gtfzafimkxk = 7409;
	bool immzkunvdfbdk = true;
	return 87687;
}

double jwwnqoq::varmwjaeiihjqhxshj(double kiklfm, int rxrnkixbadsbpkc, double cfyivnj, string itywfpsuywoea) {
	return 16634;
}

bool jwwnqoq::kfxbhxzdcakctsilgtvmoqf(bool cgijzezbhzzpu, bool ncsbmneh, double gtuobnhlldld, double lfqjmq, int aphvddrvnjcl, double obape, string nwzkdvppev, int jrbhgdvufpmbyg, string bogdkklftfvk, bool pdhxwycumkcna) {
	bool hzwjqqyrlks = true;
	string uvyuqzfolihe = "brnncnkjdvidskfktnwpnbxenqjdblomqpzowkvjwvzgcvu";
	if (true != true) {
		int oddsjut;
		for (oddsjut = 5; oddsjut > 0; oddsjut--) {
			continue;
		}
	}
	return true;
}

string jwwnqoq::mcqiiazigdcjprtx() {
	string utpxcxmyrwnhf = "rhyevorueifqfuimjnaczacqlfyxbwrkrraoetvyupwfidnta";
	double wumgowrxp = 28280;
	string ebwnvnkqcyv = "yfhlmurkpnentxssvufwrgabojxwxqfwadzljqixylwwnoxknjarzgcjq";
	string ygojvtei = "";
	bool pindeevjfbali = false;
	bool kawfixqda = true;
	if (string("") != string("")) {
		int bb;
		for (bb = 67; bb > 0; bb--) {
			continue;
		}
	}
	if (true == true) {
		int pl;
		for (pl = 18; pl > 0; pl--) {
			continue;
		}
	}
	if (true != true) {
		int pxwnfnll;
		for (pxwnfnll = 22; pxwnfnll > 0; pxwnfnll--) {
			continue;
		}
	}
	if (string("") == string("")) {
		int amf;
		for (amf = 14; amf > 0; amf--) {
			continue;
		}
	}
	if (28280 == 28280) {
		int nnaajvhf;
		for (nnaajvhf = 55; nnaajvhf > 0; nnaajvhf--) {
			continue;
		}
	}
	return string("rafvxkxogyzc");
}

bool jwwnqoq::orfberqkandjinajl(bool pxzmslpqu, double vievkpykmgdxij, double btnvdqiqoah, string ouqnre, bool smnvppvpk, int terzcwpwxfhse, string jqvdqyrpnhbfpje, int tzutq, double enxfmlqaffnkpbu, bool fkhryt) {
	int aanazeycn = 2986;
	bool xshmehbbfosb = false;
	int monwsmlhcegdx = 3235;
	string pfylns = "ehizinhefyhoyxspjcgbhiyxfrzxiujdpaqowasjpt";
	string phivboolthtmm = "yfhjdugzwjwesshqoqhfwzlespjqtepwzfqnxfpixncnbtkiotyqzojgmshynkgpajelwgurvdtzwfyosyovpdbhhuznmtaqfs";
	int lsgtpsbhuxkiwf = 2001;
	int xipfbib = 6966;
	double xknhlqsoyaj = 13940;
	if (6966 != 6966) {
		int pz;
		for (pz = 74; pz > 0; pz--) {
			continue;
		}
	}
	return false;
}

bool jwwnqoq::objrtirrfpgyevjbrohla(double iwuxxw) {
	string binasnwn = "xdetxarulxxwwndbwygrufgikknwxxwbfxrdwyiwiedqbjnrpugvvsrqbpasiylztab";
	bool pohqaornzftfaw = true;
	return false;
}

void jwwnqoq::bsnrayilueqyezb(int brqjjlxhn, bool nmtpyayp, double huruke, string tnvgaigatjkggi, string clqovecxz, string ghzlhjahsksusb, string igifz, bool amreuo, string oqzbdmeyuqneb, string czswvuckoa) {
	bool iziobcridzah = false;
	bool qnpvnx = true;
	double qnjro = 4710;
	double nfuidpceyb = 39436;
	double cirwrr = 16727;
	double jwcqopqgsbp = 24159;
	string hqazr = "paydonsscoqsxsfwyroeqaoiqnvtteuiwetloraxooyccaheazaereiepvfiwlgvflwvotxqogokhifdz";
	string bgrvvtslkuyb = "cqxvxqpszscrzpeckzmrwnkzdchxhdbkkgmgoahbxqhdfnqomrwwqzfostdiojtdxkmdudymvbqhontvpgrml";
	if (4710 == 4710) {
		int kzad;
		for (kzad = 72; kzad > 0; kzad--) {
			continue;
		}
	}
	if (true == true) {
		int hfj;
		for (hfj = 24; hfj > 0; hfj--) {
			continue;
		}
	}
	if (24159 != 24159) {
		int tdxa;
		for (tdxa = 61; tdxa > 0; tdxa--) {
			continue;
		}
	}
	if (string("cqxvxqpszscrzpeckzmrwnkzdchxhdbkkgmgoahbxqhdfnqomrwwqzfostdiojtdxkmdudymvbqhontvpgrml") != string("cqxvxqpszscrzpeckzmrwnkzdchxhdbkkgmgoahbxqhdfnqomrwwqzfostdiojtdxkmdudymvbqhontvpgrml")) {
		int gxdiuzwwys;
		for (gxdiuzwwys = 89; gxdiuzwwys > 0; gxdiuzwwys--) {
			continue;
		}
	}

}

double jwwnqoq::qfzjdwgril(double espievmd, double fxszf, int wrrfyfghpgj, string berpsvukigqjeh) {
	double mlwrfhcmvacr = 22047;
	string dzhqizwdzy = "rulcvudfjjgcxglrclejynvkimdyxomsvlxiifaawvssqfavllhnvmygxcmqoofvmwciovmrasbnbwpvxjpxzoroebvmrhbol";
	string dsjnton = "kmyjsckeycvupclkxfevdpeiwcjqmrb";
	bool hhdbukqgm = true;
	int svhiukww = 8753;
	double yhlhkhb = 6881;
	if (8753 != 8753) {
		int tsrxwy;
		for (tsrxwy = 84; tsrxwy > 0; tsrxwy--) {
			continue;
		}
	}
	if (8753 == 8753) {
		int lyskwxky;
		for (lyskwxky = 36; lyskwxky > 0; lyskwxky--) {
			continue;
		}
	}
	return 55754;
}

int jwwnqoq::qfvxryekdilnnzxrqdeyozso(bool bfrmanbtinmp) {
	double mbkshlspedz = 79031;
	int oqxbsosqqb = 2018;
	double qsusr = 74474;
	double hrffmuthli = 32856;
	string rytnpc = "cndjaftauobbg";
	int eqehgkmunlxjyfy = 1501;
	if (74474 == 74474) {
		int gdhekyxx;
		for (gdhekyxx = 27; gdhekyxx > 0; gdhekyxx--) {
			continue;
		}
	}
	if (string("cndjaftauobbg") == string("cndjaftauobbg")) {
		int xtqmmhoh;
		for (xtqmmhoh = 12; xtqmmhoh > 0; xtqmmhoh--) {
			continue;
		}
	}
	if (2018 == 2018) {
		int zvisoif;
		for (zvisoif = 21; zvisoif > 0; zvisoif--) {
			continue;
		}
	}
	if (1501 == 1501) {
		int qakd;
		for (qakd = 37; qakd > 0; qakd--) {
			continue;
		}
	}
	return 84248;
}

double jwwnqoq::velasnktthuetfcdlcrj(bool kdryoabseanvwa, string dqmikjmczabs, string tyrsauazc, int xlophgirt, string badanog, bool rinllagogeyvjr) {
	bool trzfwjiggirmbrt = false;
	bool nmwjvslh = true;
	double gaiqmvfztng = 60688;
	double ikczzz = 4179;
	string mslmyxtdxx = "vflayjvgtidkqfowuloopxxudosaikcsaeseluhxiujjihfi";
	double kybwyuqvaeuevgz = 28246;
	return 65988;
}

int jwwnqoq::evifxgpbqqhjgzlleqhcucq(string zvycrpx, double bjrvyrt, bool gftth, double uacnsxao) {
	int hmzwyrkik = 8628;
	int zmyvohxznoxpxnt = 260;
	string bathmdi = "wwchkihasuujkkdqlkcalurznhfecnzsiujegzyfkumimqhh";
	string kzeiauma = "lhteoaaaqzqobzhrkmqluazmtwsslzpdwjbqbnoirlabnveylywxexaieocjittawazggeuvxltunrbc";
	double lrvcumazikdb = 11737;
	return 49585;
}

double jwwnqoq::uxgpttenap(int mxrunfuskki) {
	string wvajnyexy = "zrovgfmigvlhrikraxqylschhkjauxllswkxobghhpojeomkzxpejgccydyvbiouxvhozbjwmkxqxtjlhmvnsnhgnaksrflotly";
	double dbjjhixmmctpz = 9193;
	string tnknkcibho = "ohcuuekdgbqogiocgcwpwqotdtzoibkazpmhezbssrkiaywvvxrztmi";
	return 41026;
}

double jwwnqoq::ntmhpzytykbmbbik(string nvqil, double vzjrdobobl, int edgvzaortlja) {
	string yczhoukffe = "hddtspezisrdueriydqfhtgpwlhyphxwauhqbiabyijxahhymmfvalwuplenomyvhjqgjtewacjoymbngprz";
	bool modwwaxsqczdgu = true;
	int znhgiodaehlpmq = 2935;
	double nploqtotepgkgak = 30373;
	string ezxlfotrrajyzox = "xaxqizvokvibddsqtsseocrazlytxbfsxlrlwz";
	string azrisdaxcj = "qjyzxefqbqgsjygdlfyfnwpcqcddrxpgumbnegdulooqqerdrzahnmfuywknpqmzkqplvfyovwy";
	bool qglgcyomgsnhebw = false;
	if (30373 != 30373) {
		int ffowtd;
		for (ffowtd = 28; ffowtd > 0; ffowtd--) {
			continue;
		}
	}
	return 66683;
}

void jwwnqoq::ytmhibkufxroixyhrulp(int ybqgb, string tbegjpfrh, double sbjzo, string solwl, string prfksus, bool glhonjhouw, bool maygibxqzp, int mlhrwczkmuqtom, string wylvbysbq, int avkjxfyaiaqn) {
	bool vdemdf = false;
	bool gevugtfqtu = false;
	string afdtntaqeapk = "gceezmqhgpueyyrekfipenrkispbdxgxgxdhksghxeeefbhliuvpkmkcdhvcjhnyvkd";
	int shmsar = 1783;

}

int jwwnqoq::hhgzevbednuvsmdif(string tbrhfmk, string wamatxtsvwj, bool vbxoyxa, string ljfknsvphjrgfs, string msvuws) {
	return 11529;
}

int jwwnqoq::xdelqygyvjkswwsehxsec(bool nmgclejeozbk, string qovvaszqo, bool ocypyhaokcxl, double ipniwuzn, string ukzuystgq, bool zvobgwmbklifytf, string rawjsuub, double bjzyn, int nrkkxxmwrh, double rryothgu) {
	int ceigmu = 6621;
	string uvnarbqjdfzb = "derlnyzlanbczhchtjwygwvhdcdwgzbsexesnlbuxuwdyu";
	if (6621 != 6621) {
		int vneazneo;
		for (vneazneo = 35; vneazneo > 0; vneazneo--) {
			continue;
		}
	}
	if (string("derlnyzlanbczhchtjwygwvhdcdwgzbsexesnlbuxuwdyu") == string("derlnyzlanbczhchtjwygwvhdcdwgzbsexesnlbuxuwdyu")) {
		int hnwc;
		for (hnwc = 62; hnwc > 0; hnwc--) {
			continue;
		}
	}
	if (6621 == 6621) {
		int wahibpay;
		for (wahibpay = 5; wahibpay > 0; wahibpay--) {
			continue;
		}
	}
	if (6621 == 6621) {
		int pam;
		for (pam = 28; pam > 0; pam--) {
			continue;
		}
	}
	return 51319;
}

string jwwnqoq::npvmyzojgwu(string zlaxbup, int wwgygdw, bool eovvxcfebwwjn, bool xbmxldljp, string qkuvfvqpf) {
	int mmkyfcigiqqtp = 681;
	string galrcklitvlljh = "zyhtiltgxkcutzkfolpkibafirh";
	string lwmeene = "boerbzhcwkrffvoshwzedehrshmfnvdegwbxmshmxzstanwxiswcrschuzjkltwlnguljmrluzsttymemvzcipmn";
	double sifsog = 34896;
	double phaqbdko = 28912;
	string pkoywqbjarxb = "wbejlhnhhkazswfwwuiamezpbnsdbnltkkmfznoceywnpwgrtrycamyeslwcgyphfhfwnphpqruqxmgqjhmjfyzjiokybx";
	double vrsbbfnpnmf = 28302;
	double oboyvehtjqoti = 39686;
	string knqbppzo = "turadsqjeislxadrapimnupfrrclkcooqdxizaaqnldosrtbwrikpjfhhxijmdhlkyamxc";
	bool ipntdu = false;
	if (681 != 681) {
		int bgofpk;
		for (bgofpk = 73; bgofpk > 0; bgofpk--) {
			continue;
		}
	}
	if (string("boerbzhcwkrffvoshwzedehrshmfnvdegwbxmshmxzstanwxiswcrschuzjkltwlnguljmrluzsttymemvzcipmn") == string("boerbzhcwkrffvoshwzedehrshmfnvdegwbxmshmxzstanwxiswcrschuzjkltwlnguljmrluzsttymemvzcipmn")) {
		int grrndgh;
		for (grrndgh = 96; grrndgh > 0; grrndgh--) {
			continue;
		}
	}
	if (28912 == 28912) {
		int uw;
		for (uw = 39; uw > 0; uw--) {
			continue;
		}
	}
	return string("kmmxhmkmaalxlmmni");
}

double jwwnqoq::bwazcswnioehx(bool lmvckkodzaiom, bool pyuihqou, int eojsrdqoeznuk, int adpqjf, int ufhuwyvd, string jhuqqmfdzxm, double byyne, int gvjinnv, string dfpnpxjynoeb) {
	string jygiizxl = "arqqcqsrceaoovafvwdzwykailxpigteawzqmmbxhoofrkwdngelwbxefwjxdyvbfmhbsblgxarhrgrqxhvzcrtqolvwthhwhz";
	double yohvbo = 13667;
	string llzddyizkcai = "nkpkmhdacdkjehbgymnsfvdpjftqiauddcbovdvyt";
	int fgtxfn = 2341;
	int kqxzucca = 2469;
	int ekuwkqxbyittnd = 2704;
	return 91899;
}

double jwwnqoq::zscxalwdrmuwiy(int yhraxqztclgj, bool gwfelciw) {
	string jihkqzzxqorirw = "rwwfjwxmdgyjuydwjdckdrumbcsmjwhaj";
	string khcphajwbzrxg = "svdwtzinbhtakrjlzasxjgsvrkvlmfihssdhucborjmqtiltbgnehtiifumxuevbyzs";
	string kdjcjs = "sznmlzcicjoqikdtg";
	double cqyihwd = 64532;
	string aewmwnn = "jizdcvkilngpficekmazxsmicukujixlyhgbwey";
	double hpllirymee = 8261;
	double ggdhzs = 34261;
	string nkyxha = "p";
	int ybehzto = 3264;
	bool felnwsealvp = true;
	return 79837;
}

jwwnqoq::jwwnqoq() {
	this->ntmhpzytykbmbbik(string("xjycrruldqfksfdzvldzpoebeqxpoouckljt"), 25054, 4718);
	this->ytmhibkufxroixyhrulp(3087, string("vpflijypcnkylgxgdzvfmxfcabifbricibaqjmblbtmepacszyfpxayplwrqtgyryaarcbbpkaab"), 16052, string("aouw"), string("rekgqjvekmwuvjwsgrqkwbnjdsngmpluqmrjndbayceqqpyoilndkqdtgssc"), false, false, 3169, string("pnxshcgqoeemwrgbadmdd"), 7707);
	this->hhgzevbednuvsmdif(string("cfxsrpvnpkmezzwcogdkgcbntspcqygdqdmufywfhwqmxwdgxtbqfhjrcemcxsburwtdqvaeeau"), string("zsqsycjcvldjpriohfxdffltetpszojrlbsfhexqxknbdxzajgxtkaxwtwjiycxdigkmqhlhrsfoelujfvgbzejdofmc"), false, string("exztznsrgrdaagybkivisaxzxnbgxjkansnt"), string("dnyljbmoyqzumycetuyhdhfgalgigdyrpwpsfztpxxplbfwtyvywubbedxxkzuhwwsybvhjjhp"));
	this->xdelqygyvjkswwsehxsec(false, string("freiqjrglwiadscabyunohbfdtrljkgsaqiyr"), false, 64385, string("ucuddtaaspwdjmwoiiglrtityutxhuxwcmwiaxagahxpcdhbdkvzxwsgkdqxwobgduygpxvwvkgfzrvtehtgycqhq"), false, string("epqjaxnsurcsfeujvgqzpqjlkxdvw"), 31401, 1740, 37760);
	this->npvmyzojgwu(string("dwzhnobluqloiumlrvgfkjjlzqbrbzzqjsapogcvcwuodavjgeehwmubnsfwtizskxyltfrcqmrysvblayelbwiqxzavnd"), 5805, true, true, string("dftuagwhwfejamjdgsaakaelutygzusqpwvmhoavknwtclsccnsvfdklzrnouzssybexoshetzew"));
	this->bwazcswnioehx(false, false, 4792, 2803, 254, string("sugoyzgvsnfupiwwjipcpsctrpxczwablzivubrqtjdiwbpimikvimhrgskbsveqhfowdtxyvsgofjipmniaxqm"), 13744, 1091, string("fsrelngrcfvvskgoxtirokodwftvqpotuidipbljmwmvxjb"));
	this->zscxalwdrmuwiy(3038, false);
	this->qfzjdwgril(34250, 47018, 2151, string("fmpdbkeihmjblghazpxfwbtknhszuptlwxmmlydjmcstgpqjfbsqkoznczihppfasqxswukvnfazxgjftqpvnprrzrwmwrvj"));
	this->qfvxryekdilnnzxrqdeyozso(true);
	this->velasnktthuetfcdlcrj(true, string("vmeqqldhfwfxxrobkrynivorbwyffncxtzfxllnswiyrkozsl"), string("vaivdkzvshxniiztfanjbmvlhfduqhqfzmihvx"), 847, string("zptnuwkfqdchqfxpxoiopubipqupjcymftwkbdf"), false);
	this->evifxgpbqqhjgzlleqhcucq(string("tkrrqqlvxrhlhjtlfhgcodvvbfypohafrlsqicyccfnpxgmalfjxfkfkfeisvtjvnjwayxsqcrojvebdujnzdcpvrsqepyhik"), 3653, false, 64797);
	this->uxgpttenap(1683);
	this->hrnxjhvmppgyvps(false, 12639, true, true, 1056, false, false);
	this->bbvewfyhrweexxqx(49050, string("nhbfhvso"), true, 1844, true, string("ejhdfrcixtlyigswntytqmyuwhnwlhcmqcblerxsqetyrqleqaqxkkbmekpqkeaojpos"), string("aqzfoiuxavtluxjusnlekcfagokcjvpwovehizgavvtajkvxdsgeuvjhdxrbsuobixdmckbljvullyhoptug"), 6256, string("lthuadikurjvvacoqahlvqchafps"), 42299);
	this->paasfmijypaqmuprkfqwmfibg(8584, 1594, 4248, 14524, true, true, true, false, string("xytbtwpdlxephqzovrnvlnjdqnzgrfjvucenuypsooyqbwtwftbmlelksbyceazzykbihqrznqw"), 47550);
	this->varmwjaeiihjqhxshj(20941, 3186, 42836, string("akmmrutwoigrcababebovqbmultutnaccmkovpymdptnyxkxyteukrgccptluxmgszpgjfrptuvwxup"));
	this->kfxbhxzdcakctsilgtvmoqf(false, false, 2724, 49083, 4465, 20130, string("ehafkmecnlxllmonscewoeuipxxfappxstsqsux"), 54, string("lxdtvwvmyxbasqzmbwsmwgofiqeplnffdyklykpyrzlkpkryzhgizleiqutdknmjfmrbxllfvtdtcoyydowtmzcjtjlqzxto"), false);
	this->mcqiiazigdcjprtx();
	this->orfberqkandjinajl(true, 8373, 46728, string("slymqtzksopverqmtwuqyjfdnkxlutxpyjcjgicfomemzafbntnqcrpmrciukqqrglvvfjn"), true, 2293, string("opgwleeggymnnvierlctehrpmlmsbpknwqyvcibhhjkobdaqdq"), 3042, 34167, true);
	this->objrtirrfpgyevjbrohla(36214);
	this->bsnrayilueqyezb(1633, false, 17569, string("zetggyoicsdqqfpzcmanj"), string("f"), string("yvbusrlawsvuzhtuqwiklovkjylrcnai"), string("rkauebotmsmvlseslvfwsolwok"), true, string("zjrdzip"), string("nthrmsxkvkjqreatuejlpoxsapdeb"));
}
