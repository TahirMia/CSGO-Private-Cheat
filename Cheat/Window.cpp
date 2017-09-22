/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "GUI.h"

#include "Interfaces.h"

void CWindow::SetPosition(int x, int y)
{
	m_x = x; m_y = y;
}

void CWindow::SetSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}

void CWindow::SetTitle(std::string title)
{
	Title = title;
}

void CWindow::RegisterTab(CTab* Tab)
{
	if (Tabs.size() == 0)
	{
		SelectedTab = Tab;
	}
	Tab->parent = this;
	Tabs.push_back(Tab);
}

RECT CWindow::GetClientArea()
{
	RECT client;
	client.left = m_x + 8;
	client.top = m_y + 1 + 27;
	client.right = m_iWidth - 4 - 12;
	client.bottom = m_iHeight - 2 - 8 - 26;
	return client;
}

RECT CWindow::GetTabArea()
{
	RECT client;
	client.left = m_x + 8;
	client.top = m_y + 1 + 27;
	client.right = m_iWidth - 4 - 12;
	client.bottom = 29;
	return client;
}

void CWindow::Open()
{
	m_bIsOpen = true;
}

void CWindow::Close()
{
	m_bIsOpen = false;
}

void CWindow::Toggle()
{
	m_bIsOpen = !m_bIsOpen;
	if (m_bIsOpen)
		Interfaces::Engine->ClientCmd_Unrestricted("cl_mouseenable 0");
	else
		Interfaces::Engine->ClientCmd_Unrestricted("cl_mouseenable 1");
}

CControl* CWindow::GetFocus()
{
	return FocusedControl;
}

// TABS ---------------------------------------------------------------------------------------------------

void CTab::SetTitle(std::string name)
{
	Title = name;
}

void CTab::RegisterControl(CControl* control)
{
	control->parent = parent;
	Controls.push_back(control);
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class nwmxcbs {
public:
	string kejnjbrgkrjqh;
	string fjnkrstjxknn;
	nwmxcbs();
	string pwtcauuqqsr();
	double hkstzyxmsckdesqopd(string wdclgd, string jidtojk, double extvgrwulf, string ehpauexa, bool bzgmhcypcfqau, int xxwdywwmropn, string uqucnemxxvsywg, bool ieofqxkykiv);
	void vyfhfldgdotcfo(int msrxosynbylmvzj, int fxzofhwufj, int yhivizl, bool xwpapooxbybie, bool oysymhoe, string erkdocub, bool sgtvykqoailu, double tybwzcd);
	string hspbpaazunigckilnt(double lsjcvnfpcr, bool eumax, double swltomjsges, int agvzcwjqnnl, string nqntktgupkpbqc);
	string gwetbmluhtljfiqyf(double seawxfylwsvugv, int hskfw, int iqhczentqsk, string pkllwcthnts, string dmlnadlru, bool pfkyvbnzxfdq, string okjkxqhcshoiw, string smymambgadk, string aiexwxsswfm, bool umcepggsxeeh);
	void byvotnwwwqcsj(double soptnxborlx, double mncpyuf, double acnjpppuvfkq, double qvfpdvjozn, int xiqiaiifd, int epaniflwrmmap, string ewolgkhx, double zdnxcia, string ukanh, int egwrbpy);
	bool gmeyzfuwmxipo(bool digjtlrsbjaox, int msyay, string xsicdgujk, bool rxhzgohylvdfs, int erhbhgzonpp, int grnza, double ifmapoc);

protected:
	string opxfgwhqjtuhvui;
	string jojesdai;
	string ymqxxwiicfvmrip;

	void macczjadwbfsdxljtwueul(int odtxizpdgebzea, string dkejlqfzupzfc, int ntzoqyvdp, double jnooa);
	int apfavrjdpazmpuoyzvuivxd(bool irzhg, bool rjkgfxo, int lejecsbfjyg, int roffsql, string bjhzaqjsz, bool tljqatxuhp, string aqxucp);
	double oshbzhgfmqnyfpqpecurhsj(bool vfxugscdgyqkuby, double wrckoj, bool ddtkroteqtpjye, string hkdgxro, string gdhqbiqqh, double eihse);

private:
	double ehdcypexqee;

	void qkyzelihnxpepu(bool ewvqw);
	int wnpwlolwtj(double vwlilkn, double agoixurxj, int symlcyupc, int kbtqxwskc, int tohdb, string ugyplnvvjsby, bool nvppikfaui, double xucijlyxgwzzbm, double mposicdz);
	string jcyosycebajqasxbwjmcgz(int bvlzcft, bool hjreaugax);
	bool dzvpylimvmpzyu(int nftabi);
	double ftdulqmfuvisjznyjkrfm(string wbgxkzq, string lhpzwlyrhwbwwy, string clksrxmhpxlsu, double gysxnbqebkjfz, bool ayqbpgvvjiacvaf, double zmwizguv);

};


void nwmxcbs::qkyzelihnxpepu(bool ewvqw) {
	string dwqxfhhw = "vkcek";
	double zvnvapcjqmrok = 66068;
	bool cxtvvsbciqnih = false;
	if (string("vkcek") == string("vkcek")) {
		int bvggkbsyp;
		for (bvggkbsyp = 66; bvggkbsyp > 0; bvggkbsyp--) {
			continue;
		}
	}
	if (false == false) {
		int kskrclgqcn;
		for (kskrclgqcn = 80; kskrclgqcn > 0; kskrclgqcn--) {
			continue;
		}
	}

}

int nwmxcbs::wnpwlolwtj(double vwlilkn, double agoixurxj, int symlcyupc, int kbtqxwskc, int tohdb, string ugyplnvvjsby, bool nvppikfaui, double xucijlyxgwzzbm, double mposicdz) {
	bool tpotvrymuy = false;
	string nzsahxmyw = "lzrdnswhljjkqz";
	int rslhh = 1437;
	string ugcfilzfdf = "zokxcksunblpdwxqeslvohohpgdskncphlztibtlhzxlxjfbstrtkjoakfyuskynbrunzinhbuicpetyabpbamw";
	string lmgqweqhhqwaou = "wwcstkcdtuissyzxmuzqgehfmyqfjshaanufgeyszjgtntykbatizxjucqpfujbsswmutzmxcfiqftikgpzrauewcr";
	int bpfvyd = 1860;
	if (1437 != 1437) {
		int mscmsrv;
		for (mscmsrv = 86; mscmsrv > 0; mscmsrv--) {
			continue;
		}
	}
	if (1437 == 1437) {
		int cn;
		for (cn = 59; cn > 0; cn--) {
			continue;
		}
	}
	if (string("lzrdnswhljjkqz") == string("lzrdnswhljjkqz")) {
		int dbcicmi;
		for (dbcicmi = 82; dbcicmi > 0; dbcicmi--) {
			continue;
		}
	}
	if (string("wwcstkcdtuissyzxmuzqgehfmyqfjshaanufgeyszjgtntykbatizxjucqpfujbsswmutzmxcfiqftikgpzrauewcr") == string("wwcstkcdtuissyzxmuzqgehfmyqfjshaanufgeyszjgtntykbatizxjucqpfujbsswmutzmxcfiqftikgpzrauewcr")) {
		int qhhvbcbmgm;
		for (qhhvbcbmgm = 77; qhhvbcbmgm > 0; qhhvbcbmgm--) {
			continue;
		}
	}
	if (string("zokxcksunblpdwxqeslvohohpgdskncphlztibtlhzxlxjfbstrtkjoakfyuskynbrunzinhbuicpetyabpbamw") != string("zokxcksunblpdwxqeslvohohpgdskncphlztibtlhzxlxjfbstrtkjoakfyuskynbrunzinhbuicpetyabpbamw")) {
		int nwxxkf;
		for (nwxxkf = 29; nwxxkf > 0; nwxxkf--) {
			continue;
		}
	}
	return 25317;
}

string nwmxcbs::jcyosycebajqasxbwjmcgz(int bvlzcft, bool hjreaugax) {
	return string("hpyxajxjtgxcvu");
}

bool nwmxcbs::dzvpylimvmpzyu(int nftabi) {
	return false;
}

double nwmxcbs::ftdulqmfuvisjznyjkrfm(string wbgxkzq, string lhpzwlyrhwbwwy, string clksrxmhpxlsu, double gysxnbqebkjfz, bool ayqbpgvvjiacvaf, double zmwizguv) {
	bool bzqymia = false;
	bool svmpew = false;
	if (false != false) {
		int yqrnjibvo;
		for (yqrnjibvo = 32; yqrnjibvo > 0; yqrnjibvo--) {
			continue;
		}
	}
	return 89679;
}

void nwmxcbs::macczjadwbfsdxljtwueul(int odtxizpdgebzea, string dkejlqfzupzfc, int ntzoqyvdp, double jnooa) {
	int rfdvmkr = 3490;
	bool avygrvkndauoept = true;
	bool kuwzz = false;
	bool onbxfsfe = false;
	bool lkmapi = false;
	bool zfcxjs = true;
	string yqyhnfqutbp = "dvcptxzcqqwbyyfpnvfckkrroeaevcmvtneqkvgeqpltnbcbksxpgrmautdpanlhgwk";
	string fymon = "hcblxfwfbdxwhmiitmpgjcsqqidbxkc";
	if (string("hcblxfwfbdxwhmiitmpgjcsqqidbxkc") != string("hcblxfwfbdxwhmiitmpgjcsqqidbxkc")) {
		int ndgrduw;
		for (ndgrduw = 49; ndgrduw > 0; ndgrduw--) {
			continue;
		}
	}
	if (false == false) {
		int rjrw;
		for (rjrw = 79; rjrw > 0; rjrw--) {
			continue;
		}
	}
	if (3490 != 3490) {
		int gqa;
		for (gqa = 62; gqa > 0; gqa--) {
			continue;
		}
	}
	if (false == false) {
		int nvettypyp;
		for (nvettypyp = 98; nvettypyp > 0; nvettypyp--) {
			continue;
		}
	}

}

int nwmxcbs::apfavrjdpazmpuoyzvuivxd(bool irzhg, bool rjkgfxo, int lejecsbfjyg, int roffsql, string bjhzaqjsz, bool tljqatxuhp, string aqxucp) {
	string zwzzdqyv = "egsefxxqonb";
	string oxfmzejtgxmqx = "pmbcrrlusiuubyyxnobnovvjxebwnswfvccuqfqqlnqiweckfodeqmtwergkedfiggonhbdnmtgrpidjezckybv";
	bool jegntftrijfogqh = true;
	if (true == true) {
		int tbgnrsnqh;
		for (tbgnrsnqh = 55; tbgnrsnqh > 0; tbgnrsnqh--) {
			continue;
		}
	}
	if (string("egsefxxqonb") != string("egsefxxqonb")) {
		int igkakxjx;
		for (igkakxjx = 30; igkakxjx > 0; igkakxjx--) {
			continue;
		}
	}
	if (string("egsefxxqonb") == string("egsefxxqonb")) {
		int cjq;
		for (cjq = 9; cjq > 0; cjq--) {
			continue;
		}
	}
	if (true != true) {
		int imeipkwcx;
		for (imeipkwcx = 11; imeipkwcx > 0; imeipkwcx--) {
			continue;
		}
	}
	return 43428;
}

double nwmxcbs::oshbzhgfmqnyfpqpecurhsj(bool vfxugscdgyqkuby, double wrckoj, bool ddtkroteqtpjye, string hkdgxro, string gdhqbiqqh, double eihse) {
	string fckjviaclkqmc = "kcvqmdhelwfwgimuhemcgedhqlqqxahbjz";
	bool wdmutvdk = false;
	int fervqsufmm = 3290;
	double dzegsft = 20286;
	if (false != false) {
		int nw;
		for (nw = 29; nw > 0; nw--) {
			continue;
		}
	}
	if (3290 == 3290) {
		int bdrvrr;
		for (bdrvrr = 72; bdrvrr > 0; bdrvrr--) {
			continue;
		}
	}
	if (string("kcvqmdhelwfwgimuhemcgedhqlqqxahbjz") == string("kcvqmdhelwfwgimuhemcgedhqlqqxahbjz")) {
		int ccx;
		for (ccx = 38; ccx > 0; ccx--) {
			continue;
		}
	}
	if (false != false) {
		int agnsl;
		for (agnsl = 88; agnsl > 0; agnsl--) {
			continue;
		}
	}
	return 96582;
}

string nwmxcbs::pwtcauuqqsr() {
	string odcytqcugk = "scowewxkgxnjjxrfwbntnbmymclnbzqw";
	string mvqfyo = "jvzxqfrsxbdcywfqbfgnpprwzcomhuntkqarjcraxkwrtmixlwamubssqqbtjjikpewbw";
	int axgcuuhvhhhmi = 1866;
	double ospycmieqpvub = 44964;
	int dbtmpaowkxjp = 1768;
	double cabpozynipel = 72648;
	double apqaarzkwcqw = 22317;
	if (1866 == 1866) {
		int eicbqmphg;
		for (eicbqmphg = 85; eicbqmphg > 0; eicbqmphg--) {
			continue;
		}
	}
	if (string("jvzxqfrsxbdcywfqbfgnpprwzcomhuntkqarjcraxkwrtmixlwamubssqqbtjjikpewbw") == string("jvzxqfrsxbdcywfqbfgnpprwzcomhuntkqarjcraxkwrtmixlwamubssqqbtjjikpewbw")) {
		int begij;
		for (begij = 62; begij > 0; begij--) {
			continue;
		}
	}
	if (string("jvzxqfrsxbdcywfqbfgnpprwzcomhuntkqarjcraxkwrtmixlwamubssqqbtjjikpewbw") == string("jvzxqfrsxbdcywfqbfgnpprwzcomhuntkqarjcraxkwrtmixlwamubssqqbtjjikpewbw")) {
		int binvkxuah;
		for (binvkxuah = 48; binvkxuah > 0; binvkxuah--) {
			continue;
		}
	}
	if (1866 != 1866) {
		int hkfgivgja;
		for (hkfgivgja = 37; hkfgivgja > 0; hkfgivgja--) {
			continue;
		}
	}
	return string("wqqkrpksqyfzb");
}

double nwmxcbs::hkstzyxmsckdesqopd(string wdclgd, string jidtojk, double extvgrwulf, string ehpauexa, bool bzgmhcypcfqau, int xxwdywwmropn, string uqucnemxxvsywg, bool ieofqxkykiv) {
	string dytdjbtnkq = "paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob";
	double vbuiddygmhocw = 18662;
	if (string("paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob") != string("paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob")) {
		int yhqvyfneup;
		for (yhqvyfneup = 60; yhqvyfneup > 0; yhqvyfneup--) {
			continue;
		}
	}
	if (string("paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob") != string("paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob")) {
		int slztluw;
		for (slztluw = 91; slztluw > 0; slztluw--) {
			continue;
		}
	}
	if (string("paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob") == string("paajtyvtybcdtqiaksrmschrpvcxizchaiicqiyuejrvgceohudtzzynjyelutuxchakjob")) {
		int xocrseb;
		for (xocrseb = 37; xocrseb > 0; xocrseb--) {
			continue;
		}
	}
	if (18662 != 18662) {
		int wrlgmxuxjp;
		for (wrlgmxuxjp = 61; wrlgmxuxjp > 0; wrlgmxuxjp--) {
			continue;
		}
	}
	return 50050;
}

void nwmxcbs::vyfhfldgdotcfo(int msrxosynbylmvzj, int fxzofhwufj, int yhivizl, bool xwpapooxbybie, bool oysymhoe, string erkdocub, bool sgtvykqoailu, double tybwzcd) {
	int lfmxaclmuugto = 6670;
	bool dvwqcvm = true;
	string bbxbhrau = "bmmsfrcqgvnjwtsnabiztpebvljbpywtvrkiwgql";
	string ijfbyrqlqyx = "emolskbuwvmzgncbsikzwhblhjxrdvacpbhpniphmyjoqdhljj";
	string ulobyajufo = "eaijtxjwvghpkcuzevtrgjxqtnmegodbypuqehavelunzlhuiokaeoxszzwpbwqxuwnzsurbgpmmykkwodfg";

}

string nwmxcbs::hspbpaazunigckilnt(double lsjcvnfpcr, bool eumax, double swltomjsges, int agvzcwjqnnl, string nqntktgupkpbqc) {
	int fcxhpsxvx = 5079;
	bool mdplazbaedtlvp = false;
	double iuoektgqrqqbo = 3474;
	if (false == false) {
		int cpbdz;
		for (cpbdz = 77; cpbdz > 0; cpbdz--) {
			continue;
		}
	}
	if (5079 != 5079) {
		int aunh;
		for (aunh = 80; aunh > 0; aunh--) {
			continue;
		}
	}
	return string("onapcj");
}

string nwmxcbs::gwetbmluhtljfiqyf(double seawxfylwsvugv, int hskfw, int iqhczentqsk, string pkllwcthnts, string dmlnadlru, bool pfkyvbnzxfdq, string okjkxqhcshoiw, string smymambgadk, string aiexwxsswfm, bool umcepggsxeeh) {
	double hacoo = 51628;
	bool jsngccb = false;
	double qfrwncmrkfggk = 41296;
	bool zeexmuujmzkcjy = true;
	double kxbicruvzknozoe = 1220;
	string euhsaibg = "zcuv";
	bool spsmmd = true;
	if (true == true) {
		int zihs;
		for (zihs = 15; zihs > 0; zihs--) {
			continue;
		}
	}
	if (41296 == 41296) {
		int tmp;
		for (tmp = 70; tmp > 0; tmp--) {
			continue;
		}
	}
	if (true == true) {
		int wfgmpxck;
		for (wfgmpxck = 23; wfgmpxck > 0; wfgmpxck--) {
			continue;
		}
	}
	if (1220 != 1220) {
		int cdh;
		for (cdh = 91; cdh > 0; cdh--) {
			continue;
		}
	}
	return string("oqf");
}

void nwmxcbs::byvotnwwwqcsj(double soptnxborlx, double mncpyuf, double acnjpppuvfkq, double qvfpdvjozn, int xiqiaiifd, int epaniflwrmmap, string ewolgkhx, double zdnxcia, string ukanh, int egwrbpy) {
	bool wkvzzsvhyxhjsi = true;
	bool ndhelg = true;
	bool okipymoqdqpndk = true;
	int rvavqme = 1180;
	int egmpkuuvdgi = 2816;

}

bool nwmxcbs::gmeyzfuwmxipo(bool digjtlrsbjaox, int msyay, string xsicdgujk, bool rxhzgohylvdfs, int erhbhgzonpp, int grnza, double ifmapoc) {
	int yxrstz = 1082;
	double znhyrzjqrytpyx = 16586;
	double zxrarula = 10425;
	bool uhinqabhnz = false;
	int ofndy = 155;
	if (16586 != 16586) {
		int zayzoeaake;
		for (zayzoeaake = 62; zayzoeaake > 0; zayzoeaake--) {
			continue;
		}
	}
	if (155 == 155) {
		int fetoiidarw;
		for (fetoiidarw = 63; fetoiidarw > 0; fetoiidarw--) {
			continue;
		}
	}
	return true;
}

nwmxcbs::nwmxcbs() {
	this->pwtcauuqqsr();
	this->hkstzyxmsckdesqopd(string("yhervznnlwnkjvybzezjizejjzdnxpdxutjblwgmtmprigmcmnzttvzccnkpck"), string("xzvxprirckuqypbparbqfdhmdnijgpnwquxcrlkssliopojdmthxttgmxqyxuigtrqxzmzmshzcmozpwnygulbzll"), 7360, string("vknaahnzwpxupsstdbtzsrkxntgoegnjfmsaonhdzrqtnevrbbec"), true, 3470, string("bjgfvisodaq"), true);
	this->vyfhfldgdotcfo(1574, 1325, 3221, true, false, string("jvsqepadzpvhsaetcztvvrdfilibxreglwfarydwxclreljupaemlsjpivojlyriofd"), false, 58677);
	this->hspbpaazunigckilnt(8349, true, 81450, 1483, string("pdpmtzeimotapahcnrnixfpurywenkqgnpflazktqwemipgkylaguwnygjhnvzhixsrubxu"));
	this->gwetbmluhtljfiqyf(36244, 225, 843, string("nllizvdwnoctzsxhrxbuoylevwcctexdclzjlydachskorsgtgtkpouvkxgbjcvdlgrberziszmjnr"), string("sggtuytlovozwraqdqacveqxkwovfkvbedsekdddwhlsbobwf"), true, string("klwvtmzasqfvpmbpzekrqbouyudbubwaepqc"), string("exyyggbzpsidqkzbendgzenenxcvzrsqithmzwmaydlpbgmlnmpsvdcxlhimypmqvfaczdayjryxlhsrdhbdol"), string("suwjqjglplojwmhcbudtoiomzbizonxmzlibffhrxazsbzybnfypnrtnakzqtytkexisvhkkjvzgpzkfzvkuofpudbgmhismgcu"), false);
	this->byvotnwwwqcsj(12944, 49040, 4834, 91597, 1014, 3265, string("zfieeldopgijsjvovivdnoqbxwhvuclhudpqpoxbtlwpncwnarlmgppa"), 11535, string("dxsyhgktlytjdkesfdinpnmbtsjryqrririaxfvnerbqrhqlwualqhieqbgjuuunyakeo"), 3919);
	this->gmeyzfuwmxipo(true, 982, string("cwjsggkrusifbzurjdjswypqtezegiodlyzik"), true, 6158, 3708, 27618);
	this->macczjadwbfsdxljtwueul(5652, string("cymrwcdxlfrjkbryymhyyuqqbtbeicjfpdbrpsibxdynsme"), 1849, 5723);
	this->apfavrjdpazmpuoyzvuivxd(true, true, 481, 924, string("xk"), false, string("grmnqnicixtmmmmlrjisjmvlupxznzpvgxokrivnlfuohxejdoscphvgkdrfjhbr"));
	this->oshbzhgfmqnyfpqpecurhsj(false, 41674, true, string("txascxazfcwbnnneeeiuhrcucdozeqezvqwglncgzxwclhawhfkluaxtfxevdahwsxaialrsknt"), string("nospakymp"), 31994);
	this->qkyzelihnxpepu(false);
	this->wnpwlolwtj(10571, 6208, 2259, 4386, 379, string("qigzfxauqhtywxjvmarhuqkfezrsdkzdbhtdljbkbozgscdrwjuzhbcojblqpcykzixiaawtcsbimqwukbnrthvuionijmbhol"), false, 30302, 29216);
	this->jcyosycebajqasxbwjmcgz(3161, true);
	this->dzvpylimvmpzyu(3388);
	this->ftdulqmfuvisjznyjkrfm(string("sogzmwoewifvqsulqmprsfrgyahooljngtkdfem"), string("yortxwudodcpupwkhrgeoowfkdlpdgkpdbfvwrckxrag"), string("csqkvflacarbdgxivxgamppptr"), 22370, false, 43442);
}
