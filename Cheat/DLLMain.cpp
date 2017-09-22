/*
Syn's AyyWare Framework 2015
*/
//#define AYY_DEBUG

// General shit
//#include "DLLMain.h" //Comment this otherwise fails building - dont know what it does yet xD
#include "Utilities.h"

// Injection stuff
#include "INJ/ReflectiveLoader.h"

// Stuff to initialise
#include "Offsets.h"
#include "Interfaces.h"
#include "Hooks.h"
#include "RenderManager.h"
#include "Hacks.h"
#include "Menu.h"
#include "AntiAntiAim.h"

#include "Dumping.h"

// Used as part of the reflective DLL injection
extern HINSTANCE hAppInstance;

// Our DLL Instance
HINSTANCE HThisModule;
bool DoUnload;

// Our thread we use to setup everything we need
// Everything appart from code in hooks get's called from inside 
// here.

int InitialThread()
{
#ifdef AYY_DEBUG
	Utilities::OpenConsole("AyyWare");
#endif
	// Intro banner with info
	PrintMetaHeader();

	//---------------------------------------------------------
	// Initialise all our shit
	Offsets::Initialise(); // Set our VMT offsets and do any pattern scans
	Interfaces::Initialise(); // Get pointers to the valve classes
	NetVar.RetrieveClasses(); // Setup our NetVar manager (thanks shad0w bby)
	NetvarManager::Instance()->CreateDatabase();
	Render::Initialise();
	Hacks::SetupHacks();
	Menu::SetupMenu();
	Hooks::Initialise();
	ApplyAAAHooks();

	//GUI.LoadWindowState(&Menu::Window, "config.xml");

	// Dumping
	//Dump::DumpClassIds();

	//---------------------------------------------------------
	Utilities::Log("Ready");

	// While our cheat is running
	while (DoUnload == false)
	{
		Sleep(1000);
	}

	Hooks::UndoHooks();
	Sleep(2000); // Make sure none of our hooks are running
	FreeLibraryAndExitThread(HThisModule, 0);

	return 0;
}

// DllMain
// Entry point for our module
BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	switch (fdwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpvReserved != NULL)
			*(HMODULE *)lpvReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		HThisModule = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitialThread, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

//JUNK CODE STARTS FROM HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class cptokxo {
public:
	double kajtyfbgqr;
	bool akfhu;
	cptokxo();
	int dxlliuqqmhtrnadkibcmm();
	string qfksnrxlslwp(int tfhomawdy, string xaqrn);
	double gttffsldert(double vfmwylmws, double nltzslmv, bool vepgyrftoqpgtma);
	int izgwzbhkiqnofdnnuysqg(string depypmygie, string ridtfgazkojdu, bool czpinmrxbra, bool poklhudgnbisgnj, string hinqgbrh, int zrazflypi, string ahevl, string qvvkekxyuqsx, double puwhts);
	double rbggcvtgsswdtfxpwlhgnyi(string ojcchnj, double oprjedafcahqr, int hjhxqslegdlfj);
	string hcezrdlxncmnorqbywfykhz(string arauctltbennw, string kmthwqupzcdepv, int mwbbjjz, string mlaumxtho, double dytplhxoodhfgqt, string tuupzuuljolh, bool zgsbv, double rawpjjacgdffs);
	int xqrnebqhvvkrzeborflnwfofr(bool wvjfkbbawftv, double lfcswtfwgmhpedy);
	double pqwymavcsirtzburawxnyraq(double aiqpmnszfwuoizn, double xbgkamjrpfetkdn, int gwezmxjmidnu, bool lydxagx, string jyediyocawebkk, double lyunmdyxeyfis, double kkehz, string miauom, string oxtjeruqluiccw);

protected:
	int ggazxtgawwoyk;
	double chcmcmxt;
	string wzrcigomw;
	string xzqhmlhruia;

	int xbcclvcloaceizomzparucc(bool uqvucd, bool rzqzxpe, int yuhojbypqtvvi, bool zjmxeve, bool gtjmxo, string uqndmmew, string prgzkobxtspvgwv, double yswrstrgpzc);
	double vatjgfkvvmjfty(bool vhpmbnjlsssnte, int nswwlhzezgr, int siazgamizzws, bool jsafd);
	int udiiezecrudyfynufodaw(string zxqzw, string zllyd, double pbujhfd, string oowgxhhze);
	double xhvysutaoadfzvtnikexghy(bool oauxdyun, bool qsnoooyexkbqadj, bool lgfmbeajryc, string nqtqtvhcpmxulz, double cqgddz, int qewlan, int ypyxcochqa);
	void kqwrnokbbpjftjlvvp(bool ctxfffkojssgz, bool aipnfupozezt, string odhnunulnid, bool bknaupfplf, double nhssq, bool gdrpdivzkju, bool gwhisshtfa);

private:
	int lqzvufnrkatl;
	double jxkwdnb;
	bool bkhsg;
	bool kqyvcln;
	bool wqtqbbqlqgol;

	int nqxhbouhlazokxrfrbjyj();
	string pgpkyanyznhwwopfktturf(bool rwnedjscjil, int dsmljbvclbaiaj, int gjpxma, string fphzzxnqcomqzq, int xwfkewutumstpu, int ywrexfxwzefdg, bool wrjbbgxdib);
	bool blnbbnxkdd(bool qpnmyjqebp, int kqgpkuibfpji, double syzqynlvhmilx);
	void nhzumzpqoowut();
	int njipxoplsbavl(bool xmcceeufpfqlab, string vsujaqkwth, int wfmabtsiyaa, string qgfenyj, int fncaxwgvsu, int dlxzvdutyrlna, double wcbtrvx, string gbejcfjzijufekr, bool tulqgtzqvm, string vbnuehfms);
	string uorqstejao(double ptnrciklkxjgg);
	void oyhciuljoyx(string lzodnltvojbweog, int lwqsaelqdqg);
	void cdzgitewzitjkfqfv(string kfetxdmwk, int nhrbqhokfnopye, bool yllclkbu, bool dyjnfwuitfywvn, double btfde, string bghvtew);
	double uhfpawpjhcvwlnhyozwod(bool gunyqpbza);

};


int cptokxo::nqxhbouhlazokxrfrbjyj() {
	double wdbqfu = 33761;
	bool zyzgzry = true;
	string hlfumyfzakjqx = "jnhsjukdiqjbhchwzkgivrbfxpmhujogdyymloljwzxophsopzskmq";
	bool atwbyv = true;
	bool ssqtapdzj = true;
	int pypieoyg = 6246;
	int zsxzu = 4026;
	bool dxvnzetswz = true;
	if (true != true) {
		int qqvxvqhps;
		for (qqvxvqhps = 62; qqvxvqhps > 0; qqvxvqhps--) {
			continue;
		}
	}
	if (true == true) {
		int ev;
		for (ev = 32; ev > 0; ev--) {
			continue;
		}
	}
	if (33761 == 33761) {
		int gtetcoeeer;
		for (gtetcoeeer = 63; gtetcoeeer > 0; gtetcoeeer--) {
			continue;
		}
	}
	if (true != true) {
		int cr;
		for (cr = 12; cr > 0; cr--) {
			continue;
		}
	}
	if (true != true) {
		int idyuiqcskx;
		for (idyuiqcskx = 67; idyuiqcskx > 0; idyuiqcskx--) {
			continue;
		}
	}
	return 27975;
}

string cptokxo::pgpkyanyznhwwopfktturf(bool rwnedjscjil, int dsmljbvclbaiaj, int gjpxma, string fphzzxnqcomqzq, int xwfkewutumstpu, int ywrexfxwzefdg, bool wrjbbgxdib) {
	string jgrmj = "ynqgkwxdxdijwgbvsbvmwjhiifnddligseypwbbhmzctdwhwswpvmwtwl";
	int mokvggpsxfzzs = 2196;
	int zsbnltmnfrxv = 8481;
	string hfntn = "mcqthgcjlsjpuvqizrhdzcffkjrhwdepespgjetnmsfelpohypywlaopwcmkyzkmvwtr";
	string sroszgc = "qylywa";
	string hrsjcqvuzkdjwco = "bdcghikwmgkegpinyzqcwazuxteqccujusybnfoalqcprbfqlxmafqpfzocmmvutkpate";
	if (string("ynqgkwxdxdijwgbvsbvmwjhiifnddligseypwbbhmzctdwhwswpvmwtwl") == string("ynqgkwxdxdijwgbvsbvmwjhiifnddligseypwbbhmzctdwhwswpvmwtwl")) {
		int lmeli;
		for (lmeli = 44; lmeli > 0; lmeli--) {
			continue;
		}
	}
	return string("");
}

bool cptokxo::blnbbnxkdd(bool qpnmyjqebp, int kqgpkuibfpji, double syzqynlvhmilx) {
	string adhfordpcbyn = "itfnqqxpeqxtteagyia";
	double aawrcbnjjbfatu = 10575;
	string eipnuycdeqnv = "tkdpughfjrdczugkuyvozeqjzfnhabpqldkhisbqbzjcbkto";
	double ldoulznhrceqj = 8711;
	bool kyxnnt = true;
	int wmyczhiu = 1657;
	double vtpnxt = 22719;
	if (string("tkdpughfjrdczugkuyvozeqjzfnhabpqldkhisbqbzjcbkto") != string("tkdpughfjrdczugkuyvozeqjzfnhabpqldkhisbqbzjcbkto")) {
		int qwvmh;
		for (qwvmh = 17; qwvmh > 0; qwvmh--) {
			continue;
		}
	}
	if (string("tkdpughfjrdczugkuyvozeqjzfnhabpqldkhisbqbzjcbkto") == string("tkdpughfjrdczugkuyvozeqjzfnhabpqldkhisbqbzjcbkto")) {
		int ztqggxbi;
		for (ztqggxbi = 89; ztqggxbi > 0; ztqggxbi--) {
			continue;
		}
	}
	return true;
}

void cptokxo::nhzumzpqoowut() {

}

int cptokxo::njipxoplsbavl(bool xmcceeufpfqlab, string vsujaqkwth, int wfmabtsiyaa, string qgfenyj, int fncaxwgvsu, int dlxzvdutyrlna, double wcbtrvx, string gbejcfjzijufekr, bool tulqgtzqvm, string vbnuehfms) {
	return 87930;
}

string cptokxo::uorqstejao(double ptnrciklkxjgg) {
	string wplgwnoolervq = "cgnjlqdybullvfciaajyourxaakpnxhmdsdzrqzcmvbxijelc";
	string qstlkvewqg = "maxfdnhgazasqfwgcfwshyabhmmmgocdktxhzqjkyhszmeougxzutjtvddmzrcvztxqnlbmnuykqlutjxyegtrt";
	string mlrbetem = "supghsgkcmdvheiempeqwqxrbphrloyuwubeqnykmjkwqt";
	double uknsk = 54090;
	double pnmroacjjzx = 43334;
	double oqlox = 19653;
	int pgacbxyuyxnfluo = 132;
	double kbwpjjigi = 1278;
	int cloeagzuozye = 530;
	if (19653 != 19653) {
		int gyzsg;
		for (gyzsg = 40; gyzsg > 0; gyzsg--) {
			continue;
		}
	}
	return string("vkdrtp");
}

void cptokxo::oyhciuljoyx(string lzodnltvojbweog, int lwqsaelqdqg) {
	double thxigvxednm = 16323;
	string jsvmtugqp = "cbxjxnanjvunbmudmndovpighzhvzbybgxdhdcmtibzfxpfj";
	int fxgxsz = 449;
	int wooizmr = 3615;
	double qbcdjcbvqaghp = 37213;
	int qxwpgxzg = 4006;
	bool uvszijssdo = false;
	string lojjtjyw = "ciebtkwwopzzgdoprgejbpsdbmpsalznypxxgruchfosyqsjpbhxznwkgrxvilc";
	int zcxamykmk = 1174;

}

void cptokxo::cdzgitewzitjkfqfv(string kfetxdmwk, int nhrbqhokfnopye, bool yllclkbu, bool dyjnfwuitfywvn, double btfde, string bghvtew) {
	string ihmnezaoi = "kiwkgedcr";
	string lacigspa = "xjrzomygkynxxvormmuvyigmisqqelmgbhsixylhsltxbinnsswinmrnwqpxqjbu";
	bool splrzmgzdswn = true;
	int ndsjfixflioya = 2652;
	if (string("kiwkgedcr") == string("kiwkgedcr")) {
		int rnrjsy;
		for (rnrjsy = 54; rnrjsy > 0; rnrjsy--) {
			continue;
		}
	}
	if (true != true) {
		int tyqka;
		for (tyqka = 31; tyqka > 0; tyqka--) {
			continue;
		}
	}
	if (true == true) {
		int xzyociu;
		for (xzyociu = 83; xzyociu > 0; xzyociu--) {
			continue;
		}
	}
	if (string("xjrzomygkynxxvormmuvyigmisqqelmgbhsixylhsltxbinnsswinmrnwqpxqjbu") == string("xjrzomygkynxxvormmuvyigmisqqelmgbhsixylhsltxbinnsswinmrnwqpxqjbu")) {
		int esrwm;
		for (esrwm = 84; esrwm > 0; esrwm--) {
			continue;
		}
	}

}

double cptokxo::uhfpawpjhcvwlnhyozwod(bool gunyqpbza) {
	double xjqpqkqsnrwbosq = 10395;
	double hkxsnevw = 10892;
	string rlwzifapthvosei = "zsubg";
	bool nmafspwukljlt = true;
	bool gznhvkmddgfge = true;
	bool apbbjpembnqyh = true;
	bool hhmaetkoxmuxdo = true;
	if (true != true) {
		int pqso;
		for (pqso = 82; pqso > 0; pqso--) {
			continue;
		}
	}
	return 71790;
}

int cptokxo::xbcclvcloaceizomzparucc(bool uqvucd, bool rzqzxpe, int yuhojbypqtvvi, bool zjmxeve, bool gtjmxo, string uqndmmew, string prgzkobxtspvgwv, double yswrstrgpzc) {
	int stpdafxaxmacvf = 2513;
	bool vsfiscrp = true;
	double gzambba = 34493;
	bool viuaynvmmqujay = true;
	return 7516;
}

double cptokxo::vatjgfkvvmjfty(bool vhpmbnjlsssnte, int nswwlhzezgr, int siazgamizzws, bool jsafd) {
	string rselcd = "sabairgckawdlmgqkpwkilqceodbatoaasqnsbptxdqfujrizscgqilvckpgrdfjuotjsk";
	string ieukelmotyvxk = "gzgnkrewkjgizjgjnwdkgqrrbczspgtwgdzuwqskufqcfanxftfvrwzrqolrgoqwfvnbvsshudtsamjtjqsagjpyoujfzkreip";
	int cbzrszrtjsiy = 2814;
	bool ogbddobdtbukoy = false;
	bool ovotaubonolhz = false;
	string oamofgzqtpe = "vqfahnzhyiukjttexqiveycimlsxrqhxcrysbw";
	if (2814 == 2814) {
		int tn;
		for (tn = 3; tn > 0; tn--) {
			continue;
		}
	}
	return 75647;
}

int cptokxo::udiiezecrudyfynufodaw(string zxqzw, string zllyd, double pbujhfd, string oowgxhhze) {
	string epyslwr = "eyhcpwxvnozawkfwxsrcqupkfqehygpsmtekzchdumauohauealjexnqhrijpncpluxhknkbxdzyqyiwzvxeugelfkdjwnw";
	int tpkgke = 6762;
	if (string("eyhcpwxvnozawkfwxsrcqupkfqehygpsmtekzchdumauohauealjexnqhrijpncpluxhknkbxdzyqyiwzvxeugelfkdjwnw") == string("eyhcpwxvnozawkfwxsrcqupkfqehygpsmtekzchdumauohauealjexnqhrijpncpluxhknkbxdzyqyiwzvxeugelfkdjwnw")) {
		int otv;
		for (otv = 14; otv > 0; otv--) {
			continue;
		}
	}
	if (string("eyhcpwxvnozawkfwxsrcqupkfqehygpsmtekzchdumauohauealjexnqhrijpncpluxhknkbxdzyqyiwzvxeugelfkdjwnw") == string("eyhcpwxvnozawkfwxsrcqupkfqehygpsmtekzchdumauohauealjexnqhrijpncpluxhknkbxdzyqyiwzvxeugelfkdjwnw")) {
		int szfcgymh;
		for (szfcgymh = 24; szfcgymh > 0; szfcgymh--) {
			continue;
		}
	}
	return 85747;
}

double cptokxo::xhvysutaoadfzvtnikexghy(bool oauxdyun, bool qsnoooyexkbqadj, bool lgfmbeajryc, string nqtqtvhcpmxulz, double cqgddz, int qewlan, int ypyxcochqa) {
	string ozdwvctgrsnoz = "zrgzlpmwlgas";
	double gvaxfgk = 95877;
	bool xjzjsjqibu = false;
	string dvqgxosuxttsb = "oijjtivnrewcusdtgcurkdtgzjjdfgjfqzkuwanp";
	double potpzhdy = 34573;
	string mwbjbjhvdnxzgpp = "wyxkwcelpiqfgdodtvurtmamvyiwajinajqtozabxgjifbwkufstotadifmpbzujftslnalsfaaewikl";
	string rpdghtfcxz = "kbszjmxodfnlqrzkanskobmkxhqybkjmlvehfjemsjsgmpggjbsoedvrzvvvduhyhft";
	int utbufghvcywhej = 2177;
	double bnkjjzu = 14795;
	bool fufyiqzklnorudv = false;
	if (string("kbszjmxodfnlqrzkanskobmkxhqybkjmlvehfjemsjsgmpggjbsoedvrzvvvduhyhft") == string("kbszjmxodfnlqrzkanskobmkxhqybkjmlvehfjemsjsgmpggjbsoedvrzvvvduhyhft")) {
		int ysvkpcql;
		for (ysvkpcql = 40; ysvkpcql > 0; ysvkpcql--) {
			continue;
		}
	}
	if (string("kbszjmxodfnlqrzkanskobmkxhqybkjmlvehfjemsjsgmpggjbsoedvrzvvvduhyhft") != string("kbszjmxodfnlqrzkanskobmkxhqybkjmlvehfjemsjsgmpggjbsoedvrzvvvduhyhft")) {
		int paty;
		for (paty = 22; paty > 0; paty--) {
			continue;
		}
	}
	if (34573 != 34573) {
		int gdlm;
		for (gdlm = 64; gdlm > 0; gdlm--) {
			continue;
		}
	}
	if (2177 != 2177) {
		int gbnfhqzu;
		for (gbnfhqzu = 72; gbnfhqzu > 0; gbnfhqzu--) {
			continue;
		}
	}
	if (false == false) {
		int phvkfoemjv;
		for (phvkfoemjv = 83; phvkfoemjv > 0; phvkfoemjv--) {
			continue;
		}
	}
	return 84995;
}

void cptokxo::kqwrnokbbpjftjlvvp(bool ctxfffkojssgz, bool aipnfupozezt, string odhnunulnid, bool bknaupfplf, double nhssq, bool gdrpdivzkju, bool gwhisshtfa) {
	string xvqxbghytovwvn = "yuufufiywugqnvjulnqakheadhdxeitfate";
	double jycknkx = 14478;
	int lrthvvicxgmbhx = 3137;
	bool hfegsmrlrf = false;
	bool dxcjhyzafcn = false;
	int lfsnrwznmpybc = 2942;
	int tpbscb = 16;
	int kjedaixipkuzdpm = 1813;
	bool taxtzbebtvfpg = true;
	bool hbxtoukpukcyem = false;
	if (true == true) {
		int llkthio;
		for (llkthio = 42; llkthio > 0; llkthio--) {
			continue;
		}
	}
	if (false != false) {
		int nxjlfi;
		for (nxjlfi = 37; nxjlfi > 0; nxjlfi--) {
			continue;
		}
	}

}

int cptokxo::dxlliuqqmhtrnadkibcmm() {
	bool qfmwuqjapujp = false;
	string zsqgiwzsme = "crbeepsegnmuadtpzolgkomvqurcxfspdegejypjsgglcufczrsvcxgmltbxxlddzisbgsiaakltylja";
	double nbhnrtiwdawhgnt = 11223;
	string jxantdjvoylloy = "hkzcnitobpufggaazhbkrbwgnmxxijbrvsrjssiwvhmwgslwtkaptymnvyevre";
	if (string("hkzcnitobpufggaazhbkrbwgnmxxijbrvsrjssiwvhmwgslwtkaptymnvyevre") != string("hkzcnitobpufggaazhbkrbwgnmxxijbrvsrjssiwvhmwgslwtkaptymnvyevre")) {
		int dwtab;
		for (dwtab = 81; dwtab > 0; dwtab--) {
			continue;
		}
	}
	if (string("hkzcnitobpufggaazhbkrbwgnmxxijbrvsrjssiwvhmwgslwtkaptymnvyevre") == string("hkzcnitobpufggaazhbkrbwgnmxxijbrvsrjssiwvhmwgslwtkaptymnvyevre")) {
		int rheixf;
		for (rheixf = 12; rheixf > 0; rheixf--) {
			continue;
		}
	}
	if (11223 == 11223) {
		int uu;
		for (uu = 20; uu > 0; uu--) {
			continue;
		}
	}
	return 82812;
}

string cptokxo::qfksnrxlslwp(int tfhomawdy, string xaqrn) {
	double oishcoeupmhz = 60869;
	int pneubf = 1936;
	bool kaqmn = true;
	string dizxo = "lhhnfwesnalhudeavrwfrlucptaotbnmtzbbcyxyelktffmjycssldyvomnuwcekmafiaenbnvrgavcsbjyoykxazuhpm";
	int dpiqlxzxtvf = 2732;
	double pmjpxugekkwvr = 6880;
	double etffpuqlo = 3732;
	bool pgprgofvcnjyyo = false;
	double khkgruy = 23495;
	if (string("lhhnfwesnalhudeavrwfrlucptaotbnmtzbbcyxyelktffmjycssldyvomnuwcekmafiaenbnvrgavcsbjyoykxazuhpm") == string("lhhnfwesnalhudeavrwfrlucptaotbnmtzbbcyxyelktffmjycssldyvomnuwcekmafiaenbnvrgavcsbjyoykxazuhpm")) {
		int voyevb;
		for (voyevb = 12; voyevb > 0; voyevb--) {
			continue;
		}
	}
	if (6880 != 6880) {
		int ogufbih;
		for (ogufbih = 88; ogufbih > 0; ogufbih--) {
			continue;
		}
	}
	if (23495 == 23495) {
		int mxbql;
		for (mxbql = 18; mxbql > 0; mxbql--) {
			continue;
		}
	}
	if (1936 != 1936) {
		int cwecsuoqb;
		for (cwecsuoqb = 13; cwecsuoqb > 0; cwecsuoqb--) {
			continue;
		}
	}
	return string("rzttklxvzurzv");
}

double cptokxo::gttffsldert(double vfmwylmws, double nltzslmv, bool vepgyrftoqpgtma) {
	string dkkzulajztfmpxj = "odyvqreqpbecyjdmfxkltwzwqrkiwizjirtbjxkclghbdqbhfdktcjwgcqortjchesleljeitcqdjbbb";
	string vpiphtpwtsgxvnj = "cnjyflwneglajmynjomlochxsmwcjorcbuyttnxcjamtnvczdfbwysxey";
	bool xfxcohzqtfdzwn = false;
	int vkivdcnwshaicz = 610;
	double yqproozi = 41277;
	int niedbewilzu = 9114;
	string tjacohk = "twbppcopzfelmosrjetdiedqmimyalvamj";
	if (41277 == 41277) {
		int xjgco;
		for (xjgco = 16; xjgco > 0; xjgco--) {
			continue;
		}
	}
	return 82082;
}

int cptokxo::izgwzbhkiqnofdnnuysqg(string depypmygie, string ridtfgazkojdu, bool czpinmrxbra, bool poklhudgnbisgnj, string hinqgbrh, int zrazflypi, string ahevl, string qvvkekxyuqsx, double puwhts) {
	return 15242;
}

double cptokxo::rbggcvtgsswdtfxpwlhgnyi(string ojcchnj, double oprjedafcahqr, int hjhxqslegdlfj) {
	string ogzbl = "lslmdjbqkidoupralcyoymnqtytwbapziuiyduyncgpfxwookduuzyawjvdu";
	int owjboyhcldpc = 7288;
	double yznwqfgbpoq = 12210;
	int jtivsnll = 4771;
	double vuagj = 4253;
	int yeikjcdphwagnub = 533;
	if (string("lslmdjbqkidoupralcyoymnqtytwbapziuiyduyncgpfxwookduuzyawjvdu") != string("lslmdjbqkidoupralcyoymnqtytwbapziuiyduyncgpfxwookduuzyawjvdu")) {
		int xim;
		for (xim = 5; xim > 0; xim--) {
			continue;
		}
	}
	if (4253 == 4253) {
		int vtcmnmw;
		for (vtcmnmw = 50; vtcmnmw > 0; vtcmnmw--) {
			continue;
		}
	}
	return 82005;
}

string cptokxo::hcezrdlxncmnorqbywfykhz(string arauctltbennw, string kmthwqupzcdepv, int mwbbjjz, string mlaumxtho, double dytplhxoodhfgqt, string tuupzuuljolh, bool zgsbv, double rawpjjacgdffs) {
	return string("ph");
}

int cptokxo::xqrnebqhvvkrzeborflnwfofr(bool wvjfkbbawftv, double lfcswtfwgmhpedy) {
	return 66183;
}

double cptokxo::pqwymavcsirtzburawxnyraq(double aiqpmnszfwuoizn, double xbgkamjrpfetkdn, int gwezmxjmidnu, bool lydxagx, string jyediyocawebkk, double lyunmdyxeyfis, double kkehz, string miauom, string oxtjeruqluiccw) {
	int jocpv = 227;
	int vgnltwyvrscfhhz = 250;
	double rjovtvkit = 1920;
	bool ooypiuwkjupw = false;
	double ecellnzqjox = 38192;
	string mtotvqqtfvaho = "xzessduruxijfidcsyehekppketrjyk";
	int rdzeqlwjngb = 1018;
	bool hlcqunahtk = true;
	int ogdhl = 4203;
	double edvflog = 14139;
	if (4203 != 4203) {
		int wqk;
		for (wqk = 36; wqk > 0; wqk--) {
			continue;
		}
	}
	return 78597;
}

cptokxo::cptokxo() {
	this->dxlliuqqmhtrnadkibcmm();
	this->qfksnrxlslwp(952, string("fepqijk"));
	this->gttffsldert(59410, 23828, false);
	this->izgwzbhkiqnofdnnuysqg(string("nzwmwqumpreoabxihccq"), string("enwmlwsxrfiwxzajmowkdtfcgdfncunmzawoqodztxxkpqeuzxtg"), true, false, string("tedsxbsfebowxeginqwgyru"), 5731, string(""), string("ycldqwfewsphdfpwkjnqtdkqsbetqen"), 29751);
	this->rbggcvtgsswdtfxpwlhgnyi(string("nsinhgpqlqmui"), 7554, 1168);
	this->hcezrdlxncmnorqbywfykhz(string(""), string("ncinuwvieozbjymrjrleedxpqhafjcwcxlkiaujoxoykwqvxfaggvglyjfjldvtcjgekxyofenz"), 1272, string("lmkysouqsdhwcabixfgqkicmwwzcmntwmulotascojxs"), 4064, string("gzlzibojuchowcnkivcjhtuzbqlcrbwsmawtexittydhhschblwtmnnohfr"), true, 70813);
	this->xqrnebqhvvkrzeborflnwfofr(false, 20894);
	this->pqwymavcsirtzburawxnyraq(38621, 10833, 957, true, string("ushfumzwlnilz"), 3893, 20230, string("rrvlkscwmejtwbbhiusccrvseifxlpesnjhtghbozigpgcosidqisfhnijfpavxyh"), string("thnuiyxqkacyjpabngblgnqlmobxahexgpdbisdqdr"));
	this->xbcclvcloaceizomzparucc(true, false, 7651, false, false, string("hxvhxdqdengqulxlhjwsyoriwlbicykcdqepgprwgiimb"), string("davldpozjekwkgkjynqektaeolsfntgvffmwnskgotbxnhvgveoicyprvfbkafkiqqwkflubtktpdsgg"), 3063);
	this->vatjgfkvvmjfty(false, 8852, 6496, false);
	this->udiiezecrudyfynufodaw(string("ubhzbtvrxhmergpdlczcqhgfnzzgrtosmq"), string("xiinfskutztphksnegyakstyxfrsizsfzsszumdganevzbjtzdxsruktxivmmvqkfibllzfclmvfcs"), 7714, string("rcbltkoamziohnjbjakuzzjronqkvgxhqqmbh"));
	this->xhvysutaoadfzvtnikexghy(true, true, true, string("vxdhhhhksrfcpacgywxnnmidiwflfvjvlorebtkmqyudnkdzkubylzjbkdrfcanmuqotwutbvcwiurbapxie"), 42530, 7159, 8772);
	this->kqwrnokbbpjftjlvvp(false, true, string("ersdiieuaxzlvkgyviongmxtttpuxhv"), false, 25241, false, false);
	this->nqxhbouhlazokxrfrbjyj();
	this->pgpkyanyznhwwopfktturf(false, 1205, 1556, string("elgtqfkndovlaurefbebojmnwumakjmaappfrtfvjoijhfdndnqbpdogrcqdaqtkbtwlidwgokf"), 5561, 7347, false);
	this->blnbbnxkdd(true, 2937, 19355);
	this->nhzumzpqoowut();
	this->njipxoplsbavl(true, string("dxejpnhsilczl"), 310, string("nhgpznmtmygrexwlbyquzrrrrztb"), 5133, 2880, 32144, string("ipjmbinxhbabautpqyvfomvidsqjuavqpjm"), false, string("xzrhjxpuemtowtpcmfookrplhuubbcqwcslqb"));
	this->uorqstejao(1600);
	this->oyhciuljoyx(string("wskdcdswocvbnleaaowbdwkxbri"), 4085);
	this->cdzgitewzitjkfqfv(string("hszehlnuahkesszzreebntlqnbgmndudsqn"), 132, true, false, 36852, string("xmlcdhzttumbkfgvawrcecp"));
	this->uhfpawpjhcvwlnhyozwod(false);
}

