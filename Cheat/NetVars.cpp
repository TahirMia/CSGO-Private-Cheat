

// Credits to Valve and Shad0w

#include "NetVars.h"
#include "ClientRecvProps.h"
#include "CRC32.h"
#include "Utilities.h"

#include "SDK.h"

using namespace std;

CNetVar NetVar;

const char* AlignText(int align)
{
	static char buffer[256];
	int i = 0;
	for (i = 0; i < align; i++)
	{
		buffer[i] = ' ';
	}
	buffer[i + 1] = 0;
	return buffer;
}

void CNetVar::RetrieveClasses()
{
#ifdef DUMP_NETVARS_TO_FILE
	U::EnableLogFile(NETVAR_FILENAME);
#endif

	ClientClass *clientClass = Interfaces::Client->GetAllClasses();

	if (!clientClass)
		return;

	//Clear netvar vector incase of another call, not necesarry as it doesnt add duplicates

	vars.clear();

	while (clientClass != 0)
	{
		if (clientClass == 0)
			break;

		LogNetVar(clientClass->m_pRecvTable, 0);

		clientClass = clientClass->m_pNext;
	}
}

void CNetVar::LogNetVar(RecvTable *table, int align)
{
	if (table->m_nProps < 0)
		return;

#ifdef DUMP_NETVARS_TO_FILE
	if (align)
		U::Log("%s===%s===", AlignText(20 + align), table->m_pNetTableName);
	else
		U::Log(table->m_pNetTableName);
#endif

	for (auto i = 0; i < table->m_nProps; ++i)
	{
		RecvProp *prop = &table->m_pProps[i];

		if (!prop)
			continue;

		char szCRC32[150];

		sprintf_s(szCRC32, "%s%s", table->m_pNetTableName, prop->m_pVarName);

		DWORD_PTR dwCRC32 = CRC32((void*)szCRC32, strlen(szCRC32));

#ifdef DUMP_NETVARS_TO_FILE
		U::Log("%s%s [0x%X] [CRC32::0x%X]", AlignText(15 + align), prop->m_pVarName, prop->m_Offset, dwCRC32);
#endif

		//Dont add duplicates

		bool bAddNetVar = true;

		for (auto netvar = 0; netvar < (int)vars.size(); ++netvar)
		{
			netvar_info_s *netvars = &vars[netvar];

			if (netvars->dwCRC32 == dwCRC32)
				bAddNetVar = false;

#ifdef DUMP_NETVARS_TO_FILE

			if (netvars->dwCRC32 == dwCRC32 && netvars->dwOffset != prop->m_Offset) //just a test if any crc collide with another (didnt happen obviously)
			{
				U::Log("^^^^ ERROR HASH %s%s::%s [0x%X] [CRC32::0x%X] ^^^^", AlignText(15 + align), table->m_pNetTableName, prop->m_pVarName, prop->m_Offset, dwCRC32);
				U::Log("^^^^ CONFLICT %s%s::%s [0x%X] [CRC32::0x%X] ^^^^", AlignText(15 + align), netvars->szTableName, netvars->szPropName, netvars->dwOffset, netvars->dwCRC32);
			}
#endif
		}

		if (bAddNetVar) //avoid adding duplicates (faster lookup)
		{
			netvar_info_s tmp;
#ifdef DUMP_NETVARS_TO_FILE
			strcpy_s(tmp.szTableName, table->m_pNetTableName);
			strcpy_s(tmp.szPropName, prop->m_pVarName);
#endif
			tmp.dwCRC32 = dwCRC32;

			tmp.dwOffset = prop->m_Offset;

			vars.push_back(tmp);
		}

		if (prop->m_pDataTable)
			LogNetVar(prop->m_pDataTable, 5);
	}
}

DWORD_PTR CNetVar::GetNetVar(DWORD_PTR dwCRC32) //returns 0xFFFFFFFF (-1) if not found (ex: if(GetNetVar(0xD34DB33F)==-1) return false;
{
	for (auto i = 0; i < (int)vars.size(); ++i)
	{
		if (vars[i].dwCRC32 == dwCRC32)
			return vars[i].dwOffset;
	}

	return 0xFFFFFFFF;
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class oekmyhi {
public:
	int rwzieqyeya;
	bool ntpgaecc;
	string jffhrks;
	int tnfecuznmjpbj;
	oekmyhi();
	double rxdyioytefgpl(int hcypvksiszvleop, string dyubdiddslz, string aldqcqjclretliq, int dtpmyqw);

protected:
	string njfhbqvpd;
	string dcnqrazre;
	double etxkkiai;
	bool pcsfhjyo;
	bool zzawdfjuw;

	void qehodrgaoqjzhtllt(int wkerktjgenfikxg, bool pwdrufehucvqq, double wdtgzch, int tlrgbzzqv, int rewvt, int cavnhosj, int tlobxk, bool tbsobuphlzqh);
	double rxuycpegdndbnd();
	bool sfyiqrkynlkewpve(int hahtcp, int htlwftgnzml);
	void rvresosdjy();
	void xjugwblvhi(double rnxwlnzbx, double cihffkdyh);
	void snjmbfkkbrdnfaruxikcoizl(int jtewborlfy, double mwudhh, int hhjkn, int aldun, double vxcdugyy, string mbddvatjmve, string pyjrzi, bool butjod, int jttbnjjfrni);
	int bzxvwveikcbfw(double unpextpyconjt);
	string eqwyzynvwqdlofeyomzfpam();
	void stfjuwmtyejzxmznfqrhudu(string bcsjeb, int gdpnypgadrvet, bool cxmiismigdb, string sochmkkbnbrcn, int lymqtwoo, int jsymgh);
	bool xnvbhopszxzhdm(bool mrbxk, double jebgmlz, string wrprzpzacgwo, double nfyvnpi, int fhlffukcwlg);

private:
	string ckjhtqjicrwc;

	void csyhxxqqvzxddhftyejrvnv(string rsqpbrcfnc, double thzlsq, double zuaswcyfo, int epbymus, bool ozkksjxuauehypx, double rcrqseyjigg, bool dyojntdh);
	void cxmolgbdypyykegcgonrunv(double wwebdnaa, string umxyihdrm, int ulvykajldx, double oyuynehjzyfdz);

};


void oekmyhi::csyhxxqqvzxddhftyejrvnv(string rsqpbrcfnc, double thzlsq, double zuaswcyfo, int epbymus, bool ozkksjxuauehypx, double rcrqseyjigg, bool dyojntdh) {
	string nduzcbgaaoerqgu = "lsydfccyhtwyogdskdjditr";
	bool abzagdt = true;
	string xgqgzq = "dmlijhn";
	int eytovf = 2478;
	bool xokrzacqhfeju = false;
	int aiiuzhp = 5077;
	int nzbleefsso = 1660;
	string rijvmxa = "aajymuqpknxvnyosjmlanitelkfhuoxjeolmbvcfjesinhwscbmwbuxtpvhkpkph";
	double soebyqqrezrvafj = 8156;
	if (8156 == 8156) {
		int mdlnhpa;
		for (mdlnhpa = 10; mdlnhpa > 0; mdlnhpa--) {
			continue;
		}
	}
	if (2478 == 2478) {
		int blomdcj;
		for (blomdcj = 32; blomdcj > 0; blomdcj--) {
			continue;
		}
	}
	if (1660 != 1660) {
		int xbala;
		for (xbala = 14; xbala > 0; xbala--) {
			continue;
		}
	}
	if (8156 != 8156) {
		int njnffsvm;
		for (njnffsvm = 7; njnffsvm > 0; njnffsvm--) {
			continue;
		}
	}

}

void oekmyhi::cxmolgbdypyykegcgonrunv(double wwebdnaa, string umxyihdrm, int ulvykajldx, double oyuynehjzyfdz) {
	bool vuxovrhqceum = false;
	int gghazznuzfoveth = 3421;
	double ctgqkbvivwd = 81820;
	bool lwyhsvdigrj = false;
	bool seled = true;
	string gcjmsywsoe = "kgarlnzgelqrvigzjrlrdslkhtuzuhrncqrqgzwrwcqsncbnkeotabhuxzuhtlmucbrvxxxoctsdwlmx";
	string thjjztqepzpdjua = "pxcrqtcsgnucgsdqpjcvfqdqbzhxhq";
	if (true != true) {
		int wwezitk;
		for (wwezitk = 10; wwezitk > 0; wwezitk--) {
			continue;
		}
	}
	if (true == true) {
		int eyvlytqwt;
		for (eyvlytqwt = 23; eyvlytqwt > 0; eyvlytqwt--) {
			continue;
		}
	}

}

void oekmyhi::qehodrgaoqjzhtllt(int wkerktjgenfikxg, bool pwdrufehucvqq, double wdtgzch, int tlrgbzzqv, int rewvt, int cavnhosj, int tlobxk, bool tbsobuphlzqh) {
	string rzgginzklhllo = "hfbajeyjobqksoybgzxlzlfkbxyjndzaqpwxqzvriehqcupybvuzimxazayfgzrsfcwuvokhjgdkyymtgafgxlcdgcnmveujsi";
	bool ouythl = false;
	double yksskfvfnizqv = 70716;
	string rxjqdxybnuql = "tkxilkzqctwwrdyxnnpavtmfynxrzaxdanwccywvysfliy";
	bool zkwibfqyudxj = true;
	int itsauowwp = 1386;
	bool suewgor = false;
	string wryqagtfapmhl = "qewpmaggnrjtmbfgcszwkaczbcejpgmgrtpvbmmdobklkviycovfjoggvgenge";
	int pphzu = 889;
	string uqshapdjdawghu = "szfyb";
	if (true != true) {
		int yqfs;
		for (yqfs = 96; yqfs > 0; yqfs--) {
			continue;
		}
	}
	if (false != false) {
		int bnh;
		for (bnh = 94; bnh > 0; bnh--) {
			continue;
		}
	}

}

double oekmyhi::rxuycpegdndbnd() {
	int brahlzxjk = 6121;
	bool wpdhjaor = false;
	if (false != false) {
		int sr;
		for (sr = 58; sr > 0; sr--) {
			continue;
		}
	}
	if (6121 == 6121) {
		int gaxs;
		for (gaxs = 11; gaxs > 0; gaxs--) {
			continue;
		}
	}
	return 30021;
}

bool oekmyhi::sfyiqrkynlkewpve(int hahtcp, int htlwftgnzml) {
	double xjmnlyx = 42598;
	string udawivy = "vianrlkcqzpnawilwcoawbohjqnodazyuekedwvnwzgdliizkwbxaigjvdblocekbgersu";
	double oydjtug = 8382;
	double erqjxyz = 18881;
	double jnjctefmovkoq = 3399;
	if (8382 != 8382) {
		int xuol;
		for (xuol = 39; xuol > 0; xuol--) {
			continue;
		}
	}
	if (string("vianrlkcqzpnawilwcoawbohjqnodazyuekedwvnwzgdliizkwbxaigjvdblocekbgersu") == string("vianrlkcqzpnawilwcoawbohjqnodazyuekedwvnwzgdliizkwbxaigjvdblocekbgersu")) {
		int pnmsjn;
		for (pnmsjn = 45; pnmsjn > 0; pnmsjn--) {
			continue;
		}
	}
	return true;
}

void oekmyhi::rvresosdjy() {
	string nygindqb = "yndsmkxzzshtxmqhpgcowtibxbsoblayf";
	string hoontnukn = "ikzhlagbltjuuowbfjtnxwxceqeonmlbxwzuvycgkekwwvkkaey";
	if (string("ikzhlagbltjuuowbfjtnxwxceqeonmlbxwzuvycgkekwwvkkaey") != string("ikzhlagbltjuuowbfjtnxwxceqeonmlbxwzuvycgkekwwvkkaey")) {
		int jkrtgrgtn;
		for (jkrtgrgtn = 39; jkrtgrgtn > 0; jkrtgrgtn--) {
			continue;
		}
	}
	if (string("yndsmkxzzshtxmqhpgcowtibxbsoblayf") != string("yndsmkxzzshtxmqhpgcowtibxbsoblayf")) {
		int gbn;
		for (gbn = 42; gbn > 0; gbn--) {
			continue;
		}
	}

}

void oekmyhi::xjugwblvhi(double rnxwlnzbx, double cihffkdyh) {
	int rewblzos = 1679;
	if (1679 == 1679) {
		int ytzxfy;
		for (ytzxfy = 34; ytzxfy > 0; ytzxfy--) {
			continue;
		}
	}

}

void oekmyhi::snjmbfkkbrdnfaruxikcoizl(int jtewborlfy, double mwudhh, int hhjkn, int aldun, double vxcdugyy, string mbddvatjmve, string pyjrzi, bool butjod, int jttbnjjfrni) {
	int orqljhxvwfkbmvt = 1036;
	int rephqozrmtv = 3655;
	bool huebsrwzon = true;
	string daihidthmnazdip = "vmktnrowcvvrezvkmoqlzeqalgznzjsglxfxjymmcnoqrhrp";
	string lstcfhifqtrq = "iuxcsqolmvfvtlxevvcjailcvrohjlkecjdyirxosfuqehcveaombypugdbcsuoocalnikogacjydllirjhaxwfshxkeutdwu";
	double fcgrgpkjxrwo = 34520;
	int zhelcdykrpodi = 2011;
	if (string("vmktnrowcvvrezvkmoqlzeqalgznzjsglxfxjymmcnoqrhrp") != string("vmktnrowcvvrezvkmoqlzeqalgznzjsglxfxjymmcnoqrhrp")) {
		int lgyzxfhov;
		for (lgyzxfhov = 86; lgyzxfhov > 0; lgyzxfhov--) {
			continue;
		}
	}

}

int oekmyhi::bzxvwveikcbfw(double unpextpyconjt) {
	int kecomwhpyiqi = 497;
	int nmsylrtsmca = 2360;
	double ksmkmbfgcjsdc = 8990;
	return 31084;
}

string oekmyhi::eqwyzynvwqdlofeyomzfpam() {
	double cnnqen = 7105;
	int zedoqx = 1966;
	string irswilannfyp = "wucmylbpfjkynpvewqfvywkyybnozkcbvegxffcuunmxlkenyhjavqxjdhghvmgfunrnxmbgewtocuwjtdpvvbppilpnf";
	return string("zbxznjpvqvptl");
}

void oekmyhi::stfjuwmtyejzxmznfqrhudu(string bcsjeb, int gdpnypgadrvet, bool cxmiismigdb, string sochmkkbnbrcn, int lymqtwoo, int jsymgh) {
	string smqffrrgrgmsxji = "rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy";
	if (string("rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy") == string("rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy")) {
		int zn;
		for (zn = 20; zn > 0; zn--) {
			continue;
		}
	}
	if (string("rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy") != string("rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy")) {
		int zdjgaomxvj;
		for (zdjgaomxvj = 77; zdjgaomxvj > 0; zdjgaomxvj--) {
			continue;
		}
	}
	if (string("rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy") != string("rkulhovcygidjdqnvzatxkyovxemnztbwtdnvwkmmroamcmgwtmqobruzgdaqvpcumyqlavneusrvifexdy")) {
		int yrttet;
		for (yrttet = 8; yrttet > 0; yrttet--) {
			continue;
		}
	}

}

bool oekmyhi::xnvbhopszxzhdm(bool mrbxk, double jebgmlz, string wrprzpzacgwo, double nfyvnpi, int fhlffukcwlg) {
	string fnvytbivkj = "qeslubsfcslxevaogjgnceivcymjkuykahvivamlaoaoisiwiecbuhunrszsijvvwjtfmnlbpkcjmabnrcyes";
	bool cosgbzlipmhjskn = false;
	string nmhrkyzkkvr = "fmrxmlkxijeepcdthfqycubzhjwqlwfbjjjqaffkixpbwsvwmflobstnthzyfdk";
	bool ubflyh = true;
	if (true != true) {
		int gruckeaua;
		for (gruckeaua = 91; gruckeaua > 0; gruckeaua--) {
			continue;
		}
	}
	if (false != false) {
		int bsyrrpni;
		for (bsyrrpni = 89; bsyrrpni > 0; bsyrrpni--) {
			continue;
		}
	}
	if (string("qeslubsfcslxevaogjgnceivcymjkuykahvivamlaoaoisiwiecbuhunrszsijvvwjtfmnlbpkcjmabnrcyes") == string("qeslubsfcslxevaogjgnceivcymjkuykahvivamlaoaoisiwiecbuhunrszsijvvwjtfmnlbpkcjmabnrcyes")) {
		int wf;
		for (wf = 97; wf > 0; wf--) {
			continue;
		}
	}
	if (true != true) {
		int qthd;
		for (qthd = 77; qthd > 0; qthd--) {
			continue;
		}
	}
	return true;
}

double oekmyhi::rxdyioytefgpl(int hcypvksiszvleop, string dyubdiddslz, string aldqcqjclretliq, int dtpmyqw) {
	int qcgzrbgqrsuwgjz = 3716;
	int fvjsjnd = 6183;
	bool cbufyx = false;
	bool jjxinqkvmc = true;
	bool wwvjq = false;
	int fuelxhs = 581;
	int ajfmzuxzqtuo = 1120;
	string fsxmrrzp = "dbpgolsexdfkzlzoxq";
	return 47542;
}

oekmyhi::oekmyhi() {
	this->rxdyioytefgpl(820, string("znnfhlbybdgq"), string("hvjzdibcjkrfrdweyturckmeuqkdmcebhoignqjahxiryhfnqqdedasfsongvzduh"), 2925);
	this->qehodrgaoqjzhtllt(4006, true, 8187, 4828, 4271, 1295, 1243, false);
	this->rxuycpegdndbnd();
	this->sfyiqrkynlkewpve(4549, 4721);
	this->rvresosdjy();
	this->xjugwblvhi(8424, 48815);
	this->snjmbfkkbrdnfaruxikcoizl(1889, 12671, 2665, 348, 4304, string("zysofnrvlzmvkubkhsrollkpdkyxfuedkdysubalujqwjizulsluekfwketqzpwmkdjptzlw"), string("rakikrhptkrfwjkbgroselcicwcqxokzgfvvcrlqsaxkcvyvkxzwudwmcdmdmcuujlmetpppkknkhmjccovejxuunve"), true, 4869);
	this->bzxvwveikcbfw(87474);
	this->eqwyzynvwqdlofeyomzfpam();
	this->stfjuwmtyejzxmznfqrhudu(string("enytzyowccyvwewnoitergdikkdtcfpc"), 500, false, string("jboemjnaerndhkvvuatvxpqwpawanqhuvuhthemzlkckscnjuumsojbgokxa"), 4275, 3033);
	this->xnvbhopszxzhdm(true, 57119, string("vdqjtjfmvgspvcogvfzxglvaelwefdlaymgunuueikkvzmqvocvrihivj"), 28585, 2243);
	this->csyhxxqqvzxddhftyejrvnv(string("aykoxqkdkbwlfoeidugqiirnhacgiufgffkkbwukqywfprckmfocqvdeihislwuilqvxefghjgbvtkqdiaeebzk"), 62292, 29984, 2827, false, 13612, false);
	this->cxmolgbdypyykegcgonrunv(14994, string("anvlyvbypazvfrujjgsptoddttuowpnd"), 2464, 54279);
}
