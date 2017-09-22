#include "Resolver.h"

int Globals::Shots = 0;
bool Globals::change;
CUserCmd* Globals::UserCmd;
int Globals::TargetID;
IClientEntity* Globals::Target;

void R::Resolve()
{
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

		if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
			continue;

		if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
			continue;

		Vector* eyeAngles = pEntity->GetEyeAnglesPointer();

		if (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 1 || Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 2)
		{
			if (pEntity != Globals::Target) continue;

			Vector AimAngle;
			CalcAngleYawOnly(pEntity->GetEyePosition(), pLocal->GetAbsOrigin2(), AimAngle);
			NormalizeVector(AimAngle);
			static float StoredYaw = 0;
			static bool bLowerBodyIsUpdated = false;
			if (pEntity->GetLowerBodyYaw() != StoredYaw) bLowerBodyIsUpdated = true;
			else bLowerBodyIsUpdated = false;
			if (bLowerBodyIsUpdated) StoredYaw = pEntity->GetLowerBodyYaw();
			if (pEntity->GetFlags() & FL_ONGROUND && pEntity->GetVelocity().Length2D() != 0) eyeAngles->y = pEntity->GetLowerBodyYaw();
			else
			{
				if (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 1 || Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 2)
				{
					int num = Globals::Shots % 5;
					switch (num) {
					case 0:eyeAngles->y = AimAngle.y - 0; break;
					case 1:eyeAngles->y = AimAngle.y - 15; break;
					case 2:eyeAngles->y = AimAngle.y + 40; break;
					case 3:eyeAngles->y = AimAngle.y - 40; break;
					case 4:eyeAngles->y = AimAngle.y + 15; break;
					}
				}
			/*	if (Menu::Window.RageBotTab.AccuracyAutoResolver.GetIndex() == 4 && eyeAngles->y != pEntity->GetLowerBodyYaw())
				{
					int num = Globals::Shots % 9;
					switch (num) {
					case 0:eyeAngles->y = AimAngle.y - 0; break;
					case 1:eyeAngles->y = AimAngle.y + 45; break;
					case 2:eyeAngles->y = AimAngle.y + 90; break;
					case 3:eyeAngles->y = AimAngle.y + 135; break;
					case 4:eyeAngles->y = AimAngle.y + 180; break;
					case 5:eyeAngles->y = AimAngle.y + 225; break;
					case 6:eyeAngles->y = AimAngle.y + 270; break;
					case 7:eyeAngles->y = AimAngle.y + 315; break;
					case 8:eyeAngles->y = AimAngle.y + 360; break;
					}
				} */
			}

			if (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 2) //pitch
			{
				std::string strPitch = std::to_string(eyeAngles->x);

				if (eyeAngles->x < -179.f) eyeAngles->x += 360.f;
				else if (eyeAngles->x > 90.0 || eyeAngles->x < -90.0) eyeAngles->x = 89.f;
				else if (eyeAngles->x > 89.0 && eyeAngles->x < 91.0) eyeAngles->x -= 90.f;
				else if (eyeAngles->x > 179.0 && eyeAngles->x < 181.0) eyeAngles->x -= 180;
				else if (eyeAngles->x > -179.0 && eyeAngles->x < -181.0) eyeAngles->x += 180;
				else if (fabs(eyeAngles->x) == 0) eyeAngles->x = std::copysign(89.0f, eyeAngles->x);
			}
		}
	}
}

//JUNK CODE STARTS HERE
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class zbmpkum {
public:
	bool galosbnkl;
	string qkzovcajczgvg;
	int jxujojlbr;
	double mqybenjsr;
	bool gssko;
	zbmpkum();
	int lioxgfmqpcsi(int bbjvkalg, bool chytjo, string pvycdigrazefdv);
	string rwxfrpeyjqmss(double funusnm, string wjgsz, int bcjzcktabowjco, int hewzo, double agfnaskhkrf);
	double lcbauswrcjs(int yehmeguofkbvh, string govhatk, string euodzlpcvy, double clrjxhnb, int llddpx, bool teoqdddaecl);
	int wztxieqlmmlbxelqxhax(string hymcfnanrqfoenb);
	double ckdraeliiprc(bool mefonoiqm, bool eiyyqnpf, double kfupeljypm, string zepxkiwl, string fukjghvd, int hlyrtkmtofst, bool lpklw, int fgjwymhqyzoyne, string whnhou, int ranmw);
	string rwqqtbswedianrbmvgq(double bfxjhcmurojr, bool ixqzhzjl);

protected:
	double bsjpyy;
	int fwsapibkxmojxu;

	void mfivtfwwwam(int hxokvidycc, bool pumiqvhyq);
	int gchvaohiuipphncpigajmn(double dlzpaqhma, int pdumwyy);
	double lgmjotskwbabb(double nerqaecvxow, string subvxtilue, double uvhdqisszqht, string rhsjodzxdkdepq, double acghj, int urjyjmaczmyxr, string wobuz, double sjmxmdyj, double yikscoa, string affgn);

private:
	string tdwrlhfwwrtzqcu;

	void lhoenwtrddty();
	int xgruuyhdgfzpjyyy(bool qjnca, int vicgfekq, bool sbvictmcwmc, string frenajnpb, int xnufmtvmr, bool wheoqs, int edjuiy);
	string gzympcmgdljprjuhlqmkkicm(string lohnfu, bool lycvbkwrhhowh, string cebzyibh, double hgkanvuajfzbr, int raaablv, double ovibowmuvexcgh, string gemftuib, bool tjykvqhguorjp, string tcxdracl);
	void npzukbsxokdwenmapfrwu(bool tvkyv, string cvqottugpqr, bool nrsauxjvcoquhp, double keotdend, bool icuxdcrveuy, int upmvmxljascl, string wossjvmtv, bool crquytzzulz);
	int uxtwodmcxntih(bool tmbbtuxnq, double rhjhq, bool eqgpzx, double uwwsx);
	bool tdcayjjnreorkrhcosymac(bool lzmoibkcrxn, double pupjvqnsgxtl, int iggebltypnvec);
	bool eqgsqvfqfjhawr(bool drzjrmzingdzjhj, int kmmvpnl, int srribcjgqcd, double pqxalplm, string rdndjroheeslrj, int ckxzohhdeu, int rmmyg, double fyyaockatmwm, int smqwvassuz, int rcjiylodmkubv);

};


void zbmpkum::lhoenwtrddty() {
	string mpmotrfxyaz = "gocyqihqlt";
	int unlduduwbplzgv = 1927;
	double ljytbhbqtvrocq = 24511;
	double wnskieb = 44153;
	int nqqgntt = 1809;
	double mjxkxgznecyuz = 43400;
	bool jxntgamtj = true;
	bool rfoxa = false;
	if (1809 != 1809) {
		int sftpybyva;
		for (sftpybyva = 22; sftpybyva > 0; sftpybyva--) {
			continue;
		}
	}

}

int zbmpkum::xgruuyhdgfzpjyyy(bool qjnca, int vicgfekq, bool sbvictmcwmc, string frenajnpb, int xnufmtvmr, bool wheoqs, int edjuiy) {
	return 40615;
}

string zbmpkum::gzympcmgdljprjuhlqmkkicm(string lohnfu, bool lycvbkwrhhowh, string cebzyibh, double hgkanvuajfzbr, int raaablv, double ovibowmuvexcgh, string gemftuib, bool tjykvqhguorjp, string tcxdracl) {
	double pbvyvpz = 14999;
	double aplohmiioiqokf = 20010;
	int vynzcujnkpf = 4670;
	double koiqp = 41399;
	int zaeanug = 3487;
	double kovquebiqsdep = 26380;
	int tjevxzrg = 2668;
	if (4670 != 4670) {
		int lsffo;
		for (lsffo = 88; lsffo > 0; lsffo--) {
			continue;
		}
	}
	if (20010 == 20010) {
		int iyp;
		for (iyp = 50; iyp > 0; iyp--) {
			continue;
		}
	}
	if (41399 == 41399) {
		int fe;
		for (fe = 92; fe > 0; fe--) {
			continue;
		}
	}
	return string("qirhtwzeae");
}

void zbmpkum::npzukbsxokdwenmapfrwu(bool tvkyv, string cvqottugpqr, bool nrsauxjvcoquhp, double keotdend, bool icuxdcrveuy, int upmvmxljascl, string wossjvmtv, bool crquytzzulz) {
	double gfnntprkwnybly = 6007;
	int rztgcennizxvl = 741;
	string moaapt = "yxtcqmkrguoqgfccxgjledkxqchdwlmasfduhiqcmmnelcycixjidnoaqvvjbqkgghebzzudyanvnxdmcckllpzhmbfjrd";
	bool krlfopuollf = true;
	double eqjcxo = 79448;
	string dnmtewtlstli = "yldplxvmnraytmljfjwijsedmjfqwuiejuyanjrghgmykssygbguiyfqfhmcd";
	int qirdhqowbfqpbo = 4836;
	bool lmumafappai = false;
	int jtkxhpwoauko = 220;
	if (false != false) {
		int iuwhcezbit;
		for (iuwhcezbit = 45; iuwhcezbit > 0; iuwhcezbit--) {
			continue;
		}
	}
	if (string("yldplxvmnraytmljfjwijsedmjfqwuiejuyanjrghgmykssygbguiyfqfhmcd") != string("yldplxvmnraytmljfjwijsedmjfqwuiejuyanjrghgmykssygbguiyfqfhmcd")) {
		int lcm;
		for (lcm = 55; lcm > 0; lcm--) {
			continue;
		}
	}
	if (79448 == 79448) {
		int hrdxpkhgm;
		for (hrdxpkhgm = 23; hrdxpkhgm > 0; hrdxpkhgm--) {
			continue;
		}
	}
	if (true != true) {
		int zdpjgwy;
		for (zdpjgwy = 31; zdpjgwy > 0; zdpjgwy--) {
			continue;
		}
	}
	if (220 != 220) {
		int ivpuz;
		for (ivpuz = 99; ivpuz > 0; ivpuz--) {
			continue;
		}
	}

}

int zbmpkum::uxtwodmcxntih(bool tmbbtuxnq, double rhjhq, bool eqgpzx, double uwwsx) {
	double egiwcmzy = 6159;
	return 26493;
}

bool zbmpkum::tdcayjjnreorkrhcosymac(bool lzmoibkcrxn, double pupjvqnsgxtl, int iggebltypnvec) {
	bool perllljxwin = true;
	int nefeoq = 2308;
	if (2308 != 2308) {
		int oiltb;
		for (oiltb = 33; oiltb > 0; oiltb--) {
			continue;
		}
	}
	if (true != true) {
		int cgsmzgzxmv;
		for (cgsmzgzxmv = 21; cgsmzgzxmv > 0; cgsmzgzxmv--) {
			continue;
		}
	}
	if (2308 == 2308) {
		int vwjjaiasuv;
		for (vwjjaiasuv = 94; vwjjaiasuv > 0; vwjjaiasuv--) {
			continue;
		}
	}
	return false;
}

bool zbmpkum::eqgsqvfqfjhawr(bool drzjrmzingdzjhj, int kmmvpnl, int srribcjgqcd, double pqxalplm, string rdndjroheeslrj, int ckxzohhdeu, int rmmyg, double fyyaockatmwm, int smqwvassuz, int rcjiylodmkubv) {
	double bwggazqsmdewwe = 10731;
	double ahwjeyeqcvumr = 23742;
	string onweg = "lbjllgtkwuwbbbwcnzyqkygxuzkrjirfhpaw";
	string lfkbynlmi = "geshcfnfyincjq";
	int qrdtpfwktyhp = 6378;
	int yybcbfvf = 1090;
	bool fenftim = true;
	double efhafswzbc = 39985;
	bool lfjowzakfnfy = false;
	double efqfbyk = 59048;
	if (1090 == 1090) {
		int thoxq;
		for (thoxq = 80; thoxq > 0; thoxq--) {
			continue;
		}
	}
	if (23742 != 23742) {
		int ntp;
		for (ntp = 54; ntp > 0; ntp--) {
			continue;
		}
	}
	if (true == true) {
		int qzjfjo;
		for (qzjfjo = 14; qzjfjo > 0; qzjfjo--) {
			continue;
		}
	}
	if (10731 != 10731) {
		int etqibnjc;
		for (etqibnjc = 13; etqibnjc > 0; etqibnjc--) {
			continue;
		}
	}
	return false;
}

void zbmpkum::mfivtfwwwam(int hxokvidycc, bool pumiqvhyq) {
	bool ohngstbvki = true;
	int oguqff = 2710;
	double tvuwysvuqqlisc = 27281;
	int lftewbbkjyyradf = 875;
	int qzdkqmiqrzbvexe = 1574;
	bool eblfmlj = true;
	int eftnymik = 1197;
	if (27281 == 27281) {
		int bn;
		for (bn = 61; bn > 0; bn--) {
			continue;
		}
	}
	if (1574 == 1574) {
		int whfwlpqle;
		for (whfwlpqle = 100; whfwlpqle > 0; whfwlpqle--) {
			continue;
		}
	}
	if (875 != 875) {
		int fvytbxdg;
		for (fvytbxdg = 82; fvytbxdg > 0; fvytbxdg--) {
			continue;
		}
	}
	if (875 != 875) {
		int cfmkhlgfze;
		for (cfmkhlgfze = 69; cfmkhlgfze > 0; cfmkhlgfze--) {
			continue;
		}
	}

}

int zbmpkum::gchvaohiuipphncpigajmn(double dlzpaqhma, int pdumwyy) {
	double geyszd = 3933;
	bool uqsxe = true;
	double hbiicbrg = 67422;
	double nynqghqpf = 32706;
	string redvvyp = "pkksyhqifcojkhxeogfvaitetjca";
	string aucvgdjwsrsanbg = "owdbdfbzzpocmphklrzbqceraebpasuecqaphq";
	if (string("owdbdfbzzpocmphklrzbqceraebpasuecqaphq") != string("owdbdfbzzpocmphklrzbqceraebpasuecqaphq")) {
		int yv;
		for (yv = 57; yv > 0; yv--) {
			continue;
		}
	}
	if (string("pkksyhqifcojkhxeogfvaitetjca") != string("pkksyhqifcojkhxeogfvaitetjca")) {
		int ou;
		for (ou = 56; ou > 0; ou--) {
			continue;
		}
	}
	if (67422 == 67422) {
		int jjwtp;
		for (jjwtp = 40; jjwtp > 0; jjwtp--) {
			continue;
		}
	}
	if (3933 != 3933) {
		int yeoniixmg;
		for (yeoniixmg = 65; yeoniixmg > 0; yeoniixmg--) {
			continue;
		}
	}
	return 42183;
}

double zbmpkum::lgmjotskwbabb(double nerqaecvxow, string subvxtilue, double uvhdqisszqht, string rhsjodzxdkdepq, double acghj, int urjyjmaczmyxr, string wobuz, double sjmxmdyj, double yikscoa, string affgn) {
	double npmhudf = 42216;
	int quvjuvgtkeca = 1546;
	double wzouoveqs = 28235;
	double zlqdxkxukeuaxjs = 7037;
	int jdgsfgreodxnvc = 6265;
	if (1546 == 1546) {
		int ezb;
		for (ezb = 26; ezb > 0; ezb--) {
			continue;
		}
	}
	if (28235 != 28235) {
		int pftbbbbxdh;
		for (pftbbbbxdh = 93; pftbbbbxdh > 0; pftbbbbxdh--) {
			continue;
		}
	}
	return 43454;
}

int zbmpkum::lioxgfmqpcsi(int bbjvkalg, bool chytjo, string pvycdigrazefdv) {
	bool ngzhpp = false;
	double lzsixqkzgdtkb = 44792;
	double tjxlnimzarhkzp = 30385;
	int ybphdpd = 4040;
	bool fltncxo = true;
	double rntaivrys = 8355;
	int nudnwlfzoz = 262;
	bool wykcqcslkggkodl = true;
	bool hqjbzsmrkgmpu = true;
	int ufhtcl = 5725;
	if (30385 == 30385) {
		int jqvbladhcr;
		for (jqvbladhcr = 10; jqvbladhcr > 0; jqvbladhcr--) {
			continue;
		}
	}
	if (true == true) {
		int nhrswoq;
		for (nhrswoq = 100; nhrswoq > 0; nhrswoq--) {
			continue;
		}
	}
	return 92463;
}

string zbmpkum::rwxfrpeyjqmss(double funusnm, string wjgsz, int bcjzcktabowjco, int hewzo, double agfnaskhkrf) {
	bool zwmbygd = false;
	if (false == false) {
		int buzslmnylq;
		for (buzslmnylq = 54; buzslmnylq > 0; buzslmnylq--) {
			continue;
		}
	}
	return string("cmjesrrmvkwgt");
}

double zbmpkum::lcbauswrcjs(int yehmeguofkbvh, string govhatk, string euodzlpcvy, double clrjxhnb, int llddpx, bool teoqdddaecl) {
	int dizhbp = 1532;
	string jtpolnftxxjtx = "eggeykqqrqckljnjcfusikfjjkwsyrzlvxkzcetkcrqlzdvieqhbezbbcarggqhx";
	int qceksthzg = 2379;
	bool wbqwlcqlbw = false;
	bool uikbgpcmtlhtild = true;
	int qsftnefnx = 7053;
	if (2379 == 2379) {
		int gxln;
		for (gxln = 3; gxln > 0; gxln--) {
			continue;
		}
	}
	if (string("eggeykqqrqckljnjcfusikfjjkwsyrzlvxkzcetkcrqlzdvieqhbezbbcarggqhx") == string("eggeykqqrqckljnjcfusikfjjkwsyrzlvxkzcetkcrqlzdvieqhbezbbcarggqhx")) {
		int ugcquwkin;
		for (ugcquwkin = 65; ugcquwkin > 0; ugcquwkin--) {
			continue;
		}
	}
	if (false != false) {
		int tm;
		for (tm = 66; tm > 0; tm--) {
			continue;
		}
	}
	if (false != false) {
		int mztel;
		for (mztel = 37; mztel > 0; mztel--) {
			continue;
		}
	}
	if (7053 == 7053) {
		int yjwgiwnlt;
		for (yjwgiwnlt = 1; yjwgiwnlt > 0; yjwgiwnlt--) {
			continue;
		}
	}
	return 48327;
}

int zbmpkum::wztxieqlmmlbxelqxhax(string hymcfnanrqfoenb) {
	double jlxss = 39729;
	double gmsnqzhsujkjqzw = 284;
	bool ykjelrisgkt = true;
	string oxlfxk = "gkvoogogjjnzccckme";
	double olcxhxfdiginljs = 8318;
	int aihyyepcigpvy = 7390;
	int aekglawwbikpqi = 4607;
	bool asgqunmtic = true;
	bool kaywh = true;
	string nryqrhvorrjt = "pzzwdlmhjhxhmu";
	if (284 == 284) {
		int ypgbov;
		for (ypgbov = 71; ypgbov > 0; ypgbov--) {
			continue;
		}
	}
	return 82564;
}

double zbmpkum::ckdraeliiprc(bool mefonoiqm, bool eiyyqnpf, double kfupeljypm, string zepxkiwl, string fukjghvd, int hlyrtkmtofst, bool lpklw, int fgjwymhqyzoyne, string whnhou, int ranmw) {
	double btnufkeupfboep = 79892;
	int rcwzzibirckux = 3327;
	double mmfsyoixilpm = 45882;
	int majimgpeda = 6830;
	string kxcrlgyss = "blyrnsagfhkuxdpjgbgthmecydjktsavuyzapdclktllwibyayaxikzjxfmuxghjjqfdxmxuvi";
	double xxeappyxrybzg = 46731;
	double aavwbgjdyzb = 4821;
	int lxjpvw = 3164;
	double sbutizayyz = 9051;
	double lhnwfjpoxny = 8792;
	if (45882 == 45882) {
		int xvrjxdzkyw;
		for (xvrjxdzkyw = 4; xvrjxdzkyw > 0; xvrjxdzkyw--) {
			continue;
		}
	}
	if (45882 == 45882) {
		int kpzmkqv;
		for (kpzmkqv = 86; kpzmkqv > 0; kpzmkqv--) {
			continue;
		}
	}
	if (4821 == 4821) {
		int fgadvlpdx;
		for (fgadvlpdx = 9; fgadvlpdx > 0; fgadvlpdx--) {
			continue;
		}
	}
	return 89727;
}

string zbmpkum::rwqqtbswedianrbmvgq(double bfxjhcmurojr, bool ixqzhzjl) {
	int gsjhtimcnoparzi = 433;
	if (433 != 433) {
		int uwhgqf;
		for (uwhgqf = 44; uwhgqf > 0; uwhgqf--) {
			continue;
		}
	}
	if (433 != 433) {
		int asdvw;
		for (asdvw = 16; asdvw > 0; asdvw--) {
			continue;
		}
	}
	if (433 != 433) {
		int rsaffghti;
		for (rsaffghti = 63; rsaffghti > 0; rsaffghti--) {
			continue;
		}
	}
	if (433 == 433) {
		int aggjk;
		for (aggjk = 23; aggjk > 0; aggjk--) {
			continue;
		}
	}
	if (433 == 433) {
		int euoywwtmrb;
		for (euoywwtmrb = 47; euoywwtmrb > 0; euoywwtmrb--) {
			continue;
		}
	}
	return string("myqlujetlawumiutzcv");
}

zbmpkum::zbmpkum() {
	this->lioxgfmqpcsi(7970, true, string("ocuiwwlgvqdbzsgugqk"));
	this->rwxfrpeyjqmss(32548, string("nmuuhztbyzjp"), 514, 5422, 12128);
	this->lcbauswrcjs(1562, string("evkbltqpezlsvghigosiccjlaznqxiyvfcvvlh"), string("jowtgfwltvsfnkrbitujphsbfrpgzmmqvepfsorjcfvwtikh"), 30676, 2452, false);
	this->wztxieqlmmlbxelqxhax(string("obwzsbzfbjkomalwkmzdxaarwsdypagrnimwjdgbocaokelhfggsyjanbmtehydkfvsgkszgtkveg"));
	this->ckdraeliiprc(true, false, 8749, string("eialsoyzkisgxkkbdbgexryltsfiogseahahqzoloyslrpirgarrxcuxzlarktjgvrhjxoenzydbdarqjppaqeqwfawdtnpltms"), string("rwmgurriouiseriaehkyjuugklphuzzvxaksptsxajijzcyvp"), 4060, false, 6993, string("aigabkjprbnstfxobvkskwjuhsfqigtyehzddzudhadnxiknlfyctvacsvotkfesdlsnlaaoufmhfedhcg"), 3523);
	this->rwqqtbswedianrbmvgq(781, true);
	this->mfivtfwwwam(4262, false);
	this->gchvaohiuipphncpigajmn(10033, 2244);
	this->lgmjotskwbabb(61340, string("pufepftymysrbaducwkpmvcmrxaaggkxlhhjatlwsjtkfwsqavlaxodypgsaswroopdcggkphagwsyrcaygtmwlxustbumx"), 43116, string("cghvjikzamzdtyeoxgjbwhylxciklrdxnssgtgb"), 57217, 1009, string("kekbzpnlzacmghdticxyvfhdqvwqttzzmyogufsdgxlzqzvipsechfrtobcdatflvzqzittjqjjfmyudacrggrdvdnoacb"), 2262, 9989, string("umjkiybvpltwonnwqgspck"));
	this->lhoenwtrddty();
	this->xgruuyhdgfzpjyyy(false, 2483, true, string("jhvsqyoxdeqogcpslkklueiievql"), 1811, false, 3495);
	this->gzympcmgdljprjuhlqmkkicm(string("ckcjbsowvzpdgzhfyijllzagermvdcbzzjqjdzftjqrmlcqidxnkjajzrhkhxawwxlbdyh"), false, string("ksfhhgbritvkqgoqlausawwlqobzeksvojqbhquzhnzrrssbvikwhmrvxruzqcecppqnxg"), 4447, 2961, 18349, string("gxolvhqiexidueg"), false, string("bcdjbegtdybfpnxiagkszgkvbjavkllwokhjtyrrfzymxsulqqgmbywblxqfurcjuxvcogklawqzcuwlpfclurmijywpk"));
	this->npzukbsxokdwenmapfrwu(true, string("yoacslayjzvotiygyklzthgujcftgohnqtnkwkhzknetxbfbakhgmrkhnpwigjsocrtijswehlfplnmpqjmueblgowklppj"), false, 43080, true, 597, string("vqjwynzamhvycdihqvlooiphoxldwxmkekoxebmltmujrrhmsvxkyotr"), true);
	this->uxtwodmcxntih(true, 104, true, 21699);
	this->tdcayjjnreorkrhcosymac(true, 16073, 1794);
	this->eqgsqvfqfjhawr(true, 2857, 3585, 56682, string("khkvqorracbdslgvtkazcogleehqrbupvzlgklonlaztnvvyhopdsokeienecxhlfthvqxmxzcebzmgeogvpenaf"), 3205, 3509, 67817, 5381, 330);
}
