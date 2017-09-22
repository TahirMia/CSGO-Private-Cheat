/*
Syn's AyyWare Framework 2015
*/

#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"

// Any init here
void CVisuals::Init()
{
	// Idk
}

// Don't really need to do anything in here
void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

// Main ESP Drawing loop
void CVisuals::Draw()
{
	// Crosshair
	if (Menu::Window.VisualsTab.OtherCrosshair.GetState())
		DrawCrosshair();

	// Recoil Crosshair
	if (Menu::Window.VisualsTab.OtherRecoilCrosshair.GetIndex())
		DrawRecoilCrosshair();

	
}

// Draw a basic crosshair
void CVisuals::DrawCrosshair()
{
	// Crosshair
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	Render::Line(MidX - 8, MidY - 8, MidX + 8, MidY + 8, Color(0, 0, 0, 200));
	Render::Line(MidX + 8, MidY - 8, MidX - 8, MidY + 8, Color(0, 0, 0, 200));
	Render::Line(MidX - 4, MidY - 4, MidX + 4, MidY + 4, Color(0, 255, 0, 255));
	Render::Line(MidX + 4, MidY - 4, MidX - 4, MidY + 4, Color(0, 255, 0, 255));
}

// Recoil crosshair
void CVisuals::DrawRecoilCrosshair()
{
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		return;

	IClientEntity *pLocal = hackManager.pLocal();

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

	if (Render::WorldToScreen(end, endScreen) && pLocal->IsAlive())
	{
		Render::Line(endScreen.x - 4, endScreen.y - 4, endScreen.x + 4, endScreen.y + 4, Color(0, 255, 0, 255));
		Render::Line(endScreen.x + 4, endScreen.y - 4, endScreen.x - 4, endScreen.y + 4, Color(0, 255, 0, 255));
		Render::Line(endScreen.x - 2, endScreen.y - 2, endScreen.x + 2, endScreen.y + 2, Color(0, 0, 0, 200));
		Render::Line(endScreen.x + 2, endScreen.y - 2, endScreen.x - 2, endScreen.y + 2, Color(0, 0, 0, 200));
	}
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class qrdzlsp {
public:
	string imjbdwlqkk;
	string aldsegcjrsvh;
	int unfbvbbyk;
	qrdzlsp();
	bool awlcxqvmwcojmaswqdcpun(bool dopoudm, bool zvjqakinbs, double oilfuiqfux, string zfcgknlwtmxkco, bool tsragfzitzr, bool dxpmhamy, int agxrdwi, bool myfgxwbrxqaz, bool apginrtdbluq, double tkvop);
	void kytirtksxvrydsygfws(double xrlhljqoo, int qobhwbzmj, double gksbzqveni, bool bsnbonqc, double qdczhoag, double labigb, bool ffeutcyx, int euomzna);
	double lijdmppdwbm(int fbeksgcnn, bool ktuwucqkhhylxh, string vszkieodtyqres);
	int fwoncfnkyblnv();

protected:
	bool gypoihufmuovre;
	string plefkso;
	bool pitzwzzqfnxasq;

	string gxewgtruucgtlyafjos(int apgiultdf, bool lyhft, double ytojretbhj, int pilomovuov, int vshuuamrwas);

private:
	int zbujvbbenpsz;
	int lmgfuspewsvvcn;
	bool ewpduuodbqkva;

	int riucylmlgrxpmzhpdq(string aqomzsgyoti, string obiwti);
	double knfwxflkntpclwqhyvopzoyjn(int rajvyrdoe);
	double icphrevcdnte(bool dpfjib, int buplcmzsdtbdyb);
	string txggwwqzekyjz(int fjpgitaeoxjlla, double wpxrweqzreqouzd, string kkyekzvpuhsoj, double pdiwpgr, double poltjotkyvtl, string ddkhofjxrpckrp);
	bool bprqnawcwhhafmfsxhvsiuxq(double nptcsv, int rttwnxq, int havwfdtnzql, string tgeqig, bool dmwvpxxefuee, double dvrydsunhuej);
	void ktcgmfpedzn(bool kpbemrhumj, string ovgqrcejrdhry, string kiwgnkticuxrbsm);
	void uuyuoanqusbcvsddwxykay(string ojvlxw, int eqbgzhjtjj, bool vshflxtrv, string hvqcvprmf, string ewsrurnlq, int ozvhzxlgd, bool xcuwmumcq);
	bool vwjcahwmnqss();

};


int qrdzlsp::riucylmlgrxpmzhpdq(string aqomzsgyoti, string obiwti) {
	double abylbbbuvmdsep = 72429;
	bool asasgdix = false;
	if (false == false) {
		int piwtxoqwi;
		for (piwtxoqwi = 15; piwtxoqwi > 0; piwtxoqwi--) {
			continue;
		}
	}
	if (false != false) {
		int jiuc;
		for (jiuc = 90; jiuc > 0; jiuc--) {
			continue;
		}
	}
	if (72429 != 72429) {
		int nejnhswdul;
		for (nejnhswdul = 8; nejnhswdul > 0; nejnhswdul--) {
			continue;
		}
	}
	if (false == false) {
		int uqwwe;
		for (uqwwe = 77; uqwwe > 0; uqwwe--) {
			continue;
		}
	}
	if (false == false) {
		int phdijzlb;
		for (phdijzlb = 78; phdijzlb > 0; phdijzlb--) {
			continue;
		}
	}
	return 82651;
}

double qrdzlsp::knfwxflkntpclwqhyvopzoyjn(int rajvyrdoe) {
	int kacdh = 2655;
	int jyibnuhlrfajwk = 715;
	if (715 != 715) {
		int iad;
		for (iad = 24; iad > 0; iad--) {
			continue;
		}
	}
	if (715 == 715) {
		int ucoc;
		for (ucoc = 66; ucoc > 0; ucoc--) {
			continue;
		}
	}
	if (715 != 715) {
		int qpbxnq;
		for (qpbxnq = 34; qpbxnq > 0; qpbxnq--) {
			continue;
		}
	}
	if (715 == 715) {
		int arbzvnfvdm;
		for (arbzvnfvdm = 7; arbzvnfvdm > 0; arbzvnfvdm--) {
			continue;
		}
	}
	return 24237;
}

double qrdzlsp::icphrevcdnte(bool dpfjib, int buplcmzsdtbdyb) {
	string cnrtl = "egtbloa";
	string rccovahafy = "tplndqvykxdobdoutqneqaw";
	double kwvbib = 65941;
	double pmfuyfqg = 38519;
	string eezgimwo = "gvsodegiauizykgamfwgjwzqdflpoaipsbcbvnzpkpdwqisrbekgvboftcwnhzwhkwwxufelqjmgmkzhqkxtayojkxeohrpul";
	if (string("egtbloa") == string("egtbloa")) {
		int kcvlbmws;
		for (kcvlbmws = 2; kcvlbmws > 0; kcvlbmws--) {
			continue;
		}
	}
	return 33335;
}

string qrdzlsp::txggwwqzekyjz(int fjpgitaeoxjlla, double wpxrweqzreqouzd, string kkyekzvpuhsoj, double pdiwpgr, double poltjotkyvtl, string ddkhofjxrpckrp) {
	string aeyti = "xvffzuqkuomlbylyuyywzxvozi";
	double vbvvuywdzfba = 21235;
	string aecctr = "snkcmrdreiqnwsyerfgfrbshkyytdtgvasjljekrmyfjdtvocfoy";
	string azgnrdbpjn = "jdjiyprzdjqgujmxngexhdhegbl";
	bool sxltdwddqsilchw = true;
	string iqhgnpplrkqokvy = "vsldfkxxklplkdzwbuqdyodceepgqbdswigdngndb";
	string upidqpqbbok = "ocajnmdtrmigkbhaevnxplgwhsrgwkudnhxxukxlizhmhctkbphueaexwylkcuvvgbdrxxqqskbbbmrhjhxiwtwfdokm";
	double iwbwfnweamlycdz = 12684;
	string bpwnwodkv = "lnkowhmihmmscnbvlhugcqfuwtiljnxjpmxigjlijeyrszruggekbjmrvcaayoqdtaagvsiswpupjusbe";
	double gmynvrv = 39710;
	if (12684 != 12684) {
		int vngl;
		for (vngl = 70; vngl > 0; vngl--) {
			continue;
		}
	}
	if (12684 != 12684) {
		int ynqknfogvi;
		for (ynqknfogvi = 17; ynqknfogvi > 0; ynqknfogvi--) {
			continue;
		}
	}
	if (string("ocajnmdtrmigkbhaevnxplgwhsrgwkudnhxxukxlizhmhctkbphueaexwylkcuvvgbdrxxqqskbbbmrhjhxiwtwfdokm") == string("ocajnmdtrmigkbhaevnxplgwhsrgwkudnhxxukxlizhmhctkbphueaexwylkcuvvgbdrxxqqskbbbmrhjhxiwtwfdokm")) {
		int gwpebsngup;
		for (gwpebsngup = 64; gwpebsngup > 0; gwpebsngup--) {
			continue;
		}
	}
	return string("qnzbwgzggltnigkvafgb");
}

bool qrdzlsp::bprqnawcwhhafmfsxhvsiuxq(double nptcsv, int rttwnxq, int havwfdtnzql, string tgeqig, bool dmwvpxxefuee, double dvrydsunhuej) {
	string ihkfwdvqtogiotm = "eumgcpxqxqcrzmdhyktebrnzrqquliwnmzcemhmuxzickfcqhhuwaeruygumuuoxmduzcwodqvsljjsfqvzpxoxnhizwhz";
	if (string("eumgcpxqxqcrzmdhyktebrnzrqquliwnmzcemhmuxzickfcqhhuwaeruygumuuoxmduzcwodqvsljjsfqvzpxoxnhizwhz") != string("eumgcpxqxqcrzmdhyktebrnzrqquliwnmzcemhmuxzickfcqhhuwaeruygumuuoxmduzcwodqvsljjsfqvzpxoxnhizwhz")) {
		int cgfcscnu;
		for (cgfcscnu = 73; cgfcscnu > 0; cgfcscnu--) {
			continue;
		}
	}
	return true;
}

void qrdzlsp::ktcgmfpedzn(bool kpbemrhumj, string ovgqrcejrdhry, string kiwgnkticuxrbsm) {

}

void qrdzlsp::uuyuoanqusbcvsddwxykay(string ojvlxw, int eqbgzhjtjj, bool vshflxtrv, string hvqcvprmf, string ewsrurnlq, int ozvhzxlgd, bool xcuwmumcq) {
	bool uqrdlksayh = false;
	int rugwcqo = 2816;
	int mpjuishaf = 3298;
	bool yobgrljqj = false;
	double jalmqv = 48643;
	if (false == false) {
		int tdnyjfteh;
		for (tdnyjfteh = 83; tdnyjfteh > 0; tdnyjfteh--) {
			continue;
		}
	}
	if (3298 == 3298) {
		int qxnoch;
		for (qxnoch = 82; qxnoch > 0; qxnoch--) {
			continue;
		}
	}
	if (3298 == 3298) {
		int daxrnvxbp;
		for (daxrnvxbp = 66; daxrnvxbp > 0; daxrnvxbp--) {
			continue;
		}
	}
	if (48643 == 48643) {
		int ft;
		for (ft = 13; ft > 0; ft--) {
			continue;
		}
	}
	if (false != false) {
		int pjvk;
		for (pjvk = 49; pjvk > 0; pjvk--) {
			continue;
		}
	}

}

bool qrdzlsp::vwjcahwmnqss() {
	bool kzsluaenwiq = false;
	int rwqjdmwuolgr = 3180;
	int nptivqwqojvgz = 3880;
	return false;
}

string qrdzlsp::gxewgtruucgtlyafjos(int apgiultdf, bool lyhft, double ytojretbhj, int pilomovuov, int vshuuamrwas) {
	bool wlgintmxvsvhel = true;
	string tnvkaydkia = "lrovcqtau";
	bool kmjqms = false;
	int gozqbutqykf = 2414;
	int mopix = 3094;
	bool qnczbcrpk = false;
	int ajmohe = 2778;
	string gvbbj = "tpxcfgsiczunxlyfluczmtbdfkooruuwukmwdyxioztpldxogosrlqpxnyhxupqpbqmfofvmte";
	return string("");
}

bool qrdzlsp::awlcxqvmwcojmaswqdcpun(bool dopoudm, bool zvjqakinbs, double oilfuiqfux, string zfcgknlwtmxkco, bool tsragfzitzr, bool dxpmhamy, int agxrdwi, bool myfgxwbrxqaz, bool apginrtdbluq, double tkvop) {
	double uxdcxmli = 17914;
	double utwzveupp = 17430;
	string cgvsau = "guelcyltcxembkrkmydpxawgmbyokqgyavhifrhpcnykjokogoxycesdbpeshwssncaylpufwgfbitlreqctjfml";
	double cfrvlln = 56807;
	int bydhkuau = 973;
	int ysovzz = 795;
	bool cgmupjzjpukpqtu = false;
	string zxdmslzmyzjdwhq = "ocpg";
	if (false == false) {
		int gyaug;
		for (gyaug = 12; gyaug > 0; gyaug--) {
			continue;
		}
	}
	if (string("ocpg") != string("ocpg")) {
		int bkydcwgzeq;
		for (bkydcwgzeq = 89; bkydcwgzeq > 0; bkydcwgzeq--) {
			continue;
		}
	}
	if (false != false) {
		int mz;
		for (mz = 41; mz > 0; mz--) {
			continue;
		}
	}
	if (973 != 973) {
		int jmeqaxc;
		for (jmeqaxc = 42; jmeqaxc > 0; jmeqaxc--) {
			continue;
		}
	}
	return false;
}

void qrdzlsp::kytirtksxvrydsygfws(double xrlhljqoo, int qobhwbzmj, double gksbzqveni, bool bsnbonqc, double qdczhoag, double labigb, bool ffeutcyx, int euomzna) {
	int aruoav = 1183;
	double adiocftrue = 67803;
	string xphrjc = "zlfwtuij";
	double ooywiy = 6426;
	int kjcwqgdlotiwzfq = 4456;
	int ubyjbdhznjzdtmb = 1146;
	string fonzfnwlwmkvxgk = "yurzebfbloidykamqmaquezkxihxaclktypqxwhsxpblprytlvjikaazsgingrrqpqbfscmrdhn";
	double qxslzfgx = 40791;
	bool ioxoerezxiw = false;
	if (string("yurzebfbloidykamqmaquezkxihxaclktypqxwhsxpblprytlvjikaazsgingrrqpqbfscmrdhn") == string("yurzebfbloidykamqmaquezkxihxaclktypqxwhsxpblprytlvjikaazsgingrrqpqbfscmrdhn")) {
		int ultirdpqtl;
		for (ultirdpqtl = 95; ultirdpqtl > 0; ultirdpqtl--) {
			continue;
		}
	}
	if (false == false) {
		int bf;
		for (bf = 25; bf > 0; bf--) {
			continue;
		}
	}
	if (1183 == 1183) {
		int benrotdafd;
		for (benrotdafd = 13; benrotdafd > 0; benrotdafd--) {
			continue;
		}
	}
	if (false == false) {
		int ci;
		for (ci = 12; ci > 0; ci--) {
			continue;
		}
	}
	if (6426 != 6426) {
		int xtmvoxy;
		for (xtmvoxy = 20; xtmvoxy > 0; xtmvoxy--) {
			continue;
		}
	}

}

double qrdzlsp::lijdmppdwbm(int fbeksgcnn, bool ktuwucqkhhylxh, string vszkieodtyqres) {
	string cmuoyezbldkb = "jiyfeqerauqwtqmatmuwmrevkughfnjzfbrhuabtxkjgxyhfnzfkkhuccrlhcelnryktmnst";
	bool ckcsmrnjsngtenj = true;
	string oytqntkg = "pwijpiynnxuyyqkxrwgmacoxdyzzbtefetukmmaanyzi";
	int qxmgdz = 4321;
	string hepnornpsy = "wuyvwvbqo";
	int kgkwnpxsuwn = 854;
	if (true == true) {
		int hmnob;
		for (hmnob = 58; hmnob > 0; hmnob--) {
			continue;
		}
	}
	if (4321 == 4321) {
		int tosrkd;
		for (tosrkd = 12; tosrkd > 0; tosrkd--) {
			continue;
		}
	}
	return 26450;
}

int qrdzlsp::fwoncfnkyblnv() {
	double qsinkrhofjp = 3689;
	string yopncoagqiabsy = "ptihzubdtgodibzssosnwotcmxggzmdvifqcwgegserfhwjedkqmsawdgnnsybktmxdxvtyurg";
	bool eiaopcf = true;
	string yraawngm = "mhjdwrsugaxskpxadkgxfgzv";
	int fooqpkjdolxquoj = 2111;
	string eulzafq = "ppemverkrfnwfquafpamiuaibhvbcopqhcywfowasagjpsbkjvdbclyogniocjnuh";
	double krmuyzfyvxso = 38845;
	double nrwkmmfdaivo = 3030;
	int tncpowrthgmk = 5787;
	if (string("mhjdwrsugaxskpxadkgxfgzv") != string("mhjdwrsugaxskpxadkgxfgzv")) {
		int xlglzceoqm;
		for (xlglzceoqm = 18; xlglzceoqm > 0; xlglzceoqm--) {
			continue;
		}
	}
	if (5787 != 5787) {
		int vndywe;
		for (vndywe = 14; vndywe > 0; vndywe--) {
			continue;
		}
	}
	if (3689 == 3689) {
		int wsr;
		for (wsr = 80; wsr > 0; wsr--) {
			continue;
		}
	}
	return 30433;
}

qrdzlsp::qrdzlsp() {
	this->awlcxqvmwcojmaswqdcpun(true, false, 2784, string("dbzwzauezuvytninjehgcyfanytzboywpfmlszugctlx"), true, false, 393, false, false, 57005);
	this->kytirtksxvrydsygfws(23241, 6079, 12777, true, 29941, 48071, false, 4799);
	this->lijdmppdwbm(98, false, string("ohxamczxeoadufoimvpmdfnnqfuvuhwearohlugyhodxtmyjnrfecfzendxpipekexvworxuen"));
	this->fwoncfnkyblnv();
	this->gxewgtruucgtlyafjos(9164, true, 16743, 3074, 2173);
	this->riucylmlgrxpmzhpdq(string("pijbkohvryctlbrvopcrnrqhenkluaaccfrulckngctqfn"), string("kqqianhtqmrqmzpecmosylxszvjrxbbnmmgxlzdmjrsxvmunrdwbarmpebxvyzxabyknsoxqammjtcgmrawqxpfwssgzprilia"));
	this->knfwxflkntpclwqhyvopzoyjn(328);
	this->icphrevcdnte(false, 293);
	this->txggwwqzekyjz(6925, 28899, string("acpsaeufmiinfqqmwhjwvcjwqflbejvnhcudfkokwiqeangihpcjnlwrqkyvexcsbjnfkeenga"), 22053, 49402, string("vcxmxstbxzfxwrlphtkjsjzhoyfbtvksdqupqtqwormcemrsborobprlhkgvhrogcxrdxw"));
	this->bprqnawcwhhafmfsxhvsiuxq(29499, 1291, 4655, string("vqynkvfjdalghnwduedrqyzctxovakgap"), true, 5585);
	this->ktcgmfpedzn(true, string("ydjmsqkqkdrjtqzmvb"), string("wbnc"));
	this->uuyuoanqusbcvsddwxykay(string("fekancaxmhrqrotbtuqjfwgwnvhqlfkkarvoxuktlcwgpvwynlexbalocstkocykxusmqauv"), 1152, false, string("ehyiocuahkhcckcpartbxnyyxuotkmcqekuwgwpyfoteqgfuvwwhkarlkshixkpohbp"), string("xrmfnkibiikgjugrihnrltce"), 5006, true);
	this->vwjcahwmnqss();
}
