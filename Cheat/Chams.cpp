#include "Chams.h"
#include "offsets.h"
#include "SDK.h"
#include "Interfaces.h"

void InitKeyValues(KeyValues* keyValues, char* name)
{
	DWORD dwFunction = (DWORD)Offsets::Functions::KeyValues_KeyValues;
	__asm
	{
		push name
			mov ecx, keyValues
			call dwFunction
	}
}

void LoadFromBuffer(KeyValues* keyValues, char const *resourceName, const char *pBuffer)
{
	DWORD dwFunction = (DWORD)Offsets::Functions::KeyValues_LoadFromBuffer;

	__asm
	{
		push 0
			push 0
			push 0
			push pBuffer
			push resourceName
			mov ecx, keyValues
			call dwFunction
	}
}

IMaterial *CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe) //credits to ph0ne
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	char* baseType = (isLit == true ? "VertexLitGeneric" : "UnlitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#ayy_meme_%i.vmt", created);
	++created;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	InitKeyValues(keyValues, baseType);
	LoadFromBuffer(keyValues, name, material);

	IMaterial *createdMaterial = Interfaces::MaterialSystem->CreateMaterial(name, keyValues);
	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

void ForceMaterial(Color color, IMaterial* material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float temp[3] =
		{
			color.r(),
			color.g(),
			color.b()
		};

		temp[0] /= 255.f;
		temp[1] /= 255.f;
		temp[2] /= 255.f;


		float alpha = color.a();

		Interfaces::RenderView->SetBlend(1.0f);
		Interfaces::RenderView->SetColorModulation(temp);
	}

	if (forceMaterial)
		Interfaces::ModelRender->ForcedMaterialOverride(material);
	else
		Interfaces::ModelRender->ForcedMaterialOverride(NULL);

}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ibiqioc {
public:
	bool xdwbdmqdkdelk;
	bool skfnan;
	string ddyomfcw;
	ibiqioc();
	int ryavriwhldzsjvtwfmhlwxxx(double iiqfdzfthdrmro, string mjemyq, string wlaqqokoru, double drbxxzvouet, string cnxptjkj, string fnooisgrvrxr);
	bool fibllujaycwikiemekqowv(bool kbvtgszzvahvsad, bool gqeelcz, double tgmiukd, string bjckwspsj, bool rsborm);
	string frldcrcfahhxjgxcxncg();
	string hziemhhsquv(int ovrbdjhvvwkq);

protected:
	string topvuuhpxtqqsp;
	int qacjctscl;
	bool ooxhqewkkyqk;
	bool aviidnqnnjct;
	string wsupjwpy;

	bool ikulaunltzfeevt(double yjavm, bool aqwvbpkdw, bool vahkusnwpkioq, int mekqayt);
	int xqeihmuppm();

private:
	int kvjmz;
	string sjaeooorlnxpw;
	double papfmstedorr;
	int unlscoea;

	bool siqcaatjeqvnjseejiidnsocc(string fbuzn, string otcbkgwci, string hlthyobcvosxoai, int wxnuitjk, int efqokjeeywe, double lkvkt, int zyslcjbgxmqo, string umvcuksbdv);
	bool zhjfjfkytpyk(int wjdjrarh, int pnspo);
	void guaddmlyabvrswuu(bool tpxxuy, double nelqtztigzoyoav, string bxdvsdnwkb, bool gazqvfyvvpyx, bool rijbdupx, int qevgeuwmupbwf, string sequg, int igxpmklyrhgkgn, string tagymvv);
	double itzzlvxmiekmtsy(bool hkvws, double etzllbwm, string ecqjtpoxjzq, int mayfrlhlbphss, int srxtkybkwgn, bool myfpdjup, string omgiydyaadyztqv, string hinvldjosjqmh, string xwmudlptfivlx);
	double dmccxewjydaud(double tkkzwzsjhypwxeg, string tbiwilsjkyfjz, string yhyvvsxbqb, double aejxnnbjkecud, bool gtqmlkqfahemuay, int prjrvpkqhzjuiyf, string lmnhtdqpoguzl);
	string amiaqbpnsnxgo(string yrtbjd, double vdfee, bool atdxrtchqjcqfwi, bool opgkqknpk, int xxqstzgcp, string pjcnfzeokixfr, string hchpqgxdjbfx, string musnrthiopac, int sebdtlislt);
	bool rlppudvvfcnxqtilpyfj(bool vudbseljdsiyvq, double ffndzwdumvoxxa, bool qnsidthuxolakzb, string eumrisspc, double kfseffoccx, string wwbcffexi, bool gaipdselknvc, double qnafbi, double qvwmptgdpmnr);
	void fmemgfuahhqgoloekmaz(int utcmswvaj, double klfihgtf);
	string mipceqqybipxjlosnmgke();
	string megggzfwxbtcpyr(string igltpkznacej);

};


bool ibiqioc::siqcaatjeqvnjseejiidnsocc(string fbuzn, string otcbkgwci, string hlthyobcvosxoai, int wxnuitjk, int efqokjeeywe, double lkvkt, int zyslcjbgxmqo, string umvcuksbdv) {
	int gigkmrdgq = 4732;
	int fzolysjmxz = 831;
	return false;
}

bool ibiqioc::zhjfjfkytpyk(int wjdjrarh, int pnspo) {
	int gbclfhuoexkro = 7411;
	string boeurogj = "quayxmjk";
	string bxmwaqfjfhyyudz = "tjtfsdrfposwbyfvezgpejwyvddxxjdjsbimbcmfcvyojv";
	string lszkmehmrjrr = "xfkrvgsxzyhbp";
	int oxsfnzjhy = 1024;
	int pbzqrlf = 1690;
	if (string("xfkrvgsxzyhbp") != string("xfkrvgsxzyhbp")) {
		int ahxgqsa;
		for (ahxgqsa = 65; ahxgqsa > 0; ahxgqsa--) {
			continue;
		}
	}
	return true;
}

void ibiqioc::guaddmlyabvrswuu(bool tpxxuy, double nelqtztigzoyoav, string bxdvsdnwkb, bool gazqvfyvvpyx, bool rijbdupx, int qevgeuwmupbwf, string sequg, int igxpmklyrhgkgn, string tagymvv) {
	bool yyijsbwvr = false;
	double otalv = 16635;
	double jungox = 5565;
	bool setkwou = true;
	if (16635 == 16635) {
		int rzyejut;
		for (rzyejut = 61; rzyejut > 0; rzyejut--) {
			continue;
		}
	}
	if (16635 == 16635) {
		int awukw;
		for (awukw = 63; awukw > 0; awukw--) {
			continue;
		}
	}
	if (false != false) {
		int nacduor;
		for (nacduor = 10; nacduor > 0; nacduor--) {
			continue;
		}
	}
	if (false == false) {
		int hyfexdqcaq;
		for (hyfexdqcaq = 56; hyfexdqcaq > 0; hyfexdqcaq--) {
			continue;
		}
	}
	if (16635 != 16635) {
		int eqn;
		for (eqn = 80; eqn > 0; eqn--) {
			continue;
		}
	}

}

double ibiqioc::itzzlvxmiekmtsy(bool hkvws, double etzllbwm, string ecqjtpoxjzq, int mayfrlhlbphss, int srxtkybkwgn, bool myfpdjup, string omgiydyaadyztqv, string hinvldjosjqmh, string xwmudlptfivlx) {
	string ilcuxld = "veleehvwuwfan";
	double mtznmjf = 25123;
	return 92320;
}

double ibiqioc::dmccxewjydaud(double tkkzwzsjhypwxeg, string tbiwilsjkyfjz, string yhyvvsxbqb, double aejxnnbjkecud, bool gtqmlkqfahemuay, int prjrvpkqhzjuiyf, string lmnhtdqpoguzl) {
	int tmsvsntidoiuaj = 73;
	bool tsfbrjkfvew = false;
	int pmkraolpwquqvmb = 1863;
	string fcbbgkokjkrzei = "qemxhinomtgyzcgjfmhcvoooscghoqfiuqrorkkgfxysxiaeskpgrkrlrflldljwqdasmmsmiyvjmrcmtprcxufvuswyxfvsqdj";
	int cehjeu = 5936;
	bool zaznwtshgafeh = false;
	if (73 != 73) {
		int zfktwqal;
		for (zfktwqal = 89; zfktwqal > 0; zfktwqal--) {
			continue;
		}
	}
	if (73 == 73) {
		int vulwikil;
		for (vulwikil = 53; vulwikil > 0; vulwikil--) {
			continue;
		}
	}
	if (5936 == 5936) {
		int itybwpgdvg;
		for (itybwpgdvg = 6; itybwpgdvg > 0; itybwpgdvg--) {
			continue;
		}
	}
	if (5936 != 5936) {
		int ptzjovi;
		for (ptzjovi = 43; ptzjovi > 0; ptzjovi--) {
			continue;
		}
	}
	if (false == false) {
		int pkhhejghib;
		for (pkhhejghib = 54; pkhhejghib > 0; pkhhejghib--) {
			continue;
		}
	}
	return 78538;
}

string ibiqioc::amiaqbpnsnxgo(string yrtbjd, double vdfee, bool atdxrtchqjcqfwi, bool opgkqknpk, int xxqstzgcp, string pjcnfzeokixfr, string hchpqgxdjbfx, string musnrthiopac, int sebdtlislt) {
	double szhubciqzd = 33436;
	return string("fgcxhrjvbnbx");
}

bool ibiqioc::rlppudvvfcnxqtilpyfj(bool vudbseljdsiyvq, double ffndzwdumvoxxa, bool qnsidthuxolakzb, string eumrisspc, double kfseffoccx, string wwbcffexi, bool gaipdselknvc, double qnafbi, double qvwmptgdpmnr) {
	double brdcytdfkmhix = 21709;
	double ykdkcexgqwgtzj = 6465;
	string ycrlzy = "myxbbrp";
	double vublf = 13179;
	if (6465 != 6465) {
		int bzbujvhind;
		for (bzbujvhind = 68; bzbujvhind > 0; bzbujvhind--) {
			continue;
		}
	}
	if (21709 != 21709) {
		int srfptp;
		for (srfptp = 84; srfptp > 0; srfptp--) {
			continue;
		}
	}
	if (21709 == 21709) {
		int gmidvwu;
		for (gmidvwu = 43; gmidvwu > 0; gmidvwu--) {
			continue;
		}
	}
	if (6465 != 6465) {
		int qc;
		for (qc = 66; qc > 0; qc--) {
			continue;
		}
	}
	return false;
}

void ibiqioc::fmemgfuahhqgoloekmaz(int utcmswvaj, double klfihgtf) {
	bool ovptpqpji = false;
	bool ffmbmslnycqomx = false;
	bool epuvfragkw = true;
	if (false == false) {
		int pbmsfays;
		for (pbmsfays = 49; pbmsfays > 0; pbmsfays--) {
			continue;
		}
	}

}

string ibiqioc::mipceqqybipxjlosnmgke() {
	bool fkwyme = false;
	if (false == false) {
		int gesefby;
		for (gesefby = 68; gesefby > 0; gesefby--) {
			continue;
		}
	}
	if (false != false) {
		int hvzel;
		for (hvzel = 72; hvzel > 0; hvzel--) {
			continue;
		}
	}
	if (false == false) {
		int oalqjr;
		for (oalqjr = 65; oalqjr > 0; oalqjr--) {
			continue;
		}
	}
	if (false != false) {
		int rqrypqv;
		for (rqrypqv = 33; rqrypqv > 0; rqrypqv--) {
			continue;
		}
	}
	return string("qfwgghssqouwnvc");
}

string ibiqioc::megggzfwxbtcpyr(string igltpkznacej) {
	bool msduoejsqxoixg = true;
	double iaqoj = 91664;
	int mkkgdpc = 1029;
	bool ftian = false;
	string wdqsg = "nlamuevxtn";
	bool anjvtpbda = false;
	string zexswuszhqovbh = "ggudxpcctpaojyolnzwvmfzkjycelbtxkczboufrqhnekaofsbdpvgfddeawdpytkgsgtmafqdhtnktozlbxinudlusuryl";
	if (1029 == 1029) {
		int ktmjgxny;
		for (ktmjgxny = 30; ktmjgxny > 0; ktmjgxny--) {
			continue;
		}
	}
	if (1029 != 1029) {
		int yeqilpynlw;
		for (yeqilpynlw = 94; yeqilpynlw > 0; yeqilpynlw--) {
			continue;
		}
	}
	if (false != false) {
		int mccjevkil;
		for (mccjevkil = 47; mccjevkil > 0; mccjevkil--) {
			continue;
		}
	}
	return string("eo");
}

bool ibiqioc::ikulaunltzfeevt(double yjavm, bool aqwvbpkdw, bool vahkusnwpkioq, int mekqayt) {
	return true;
}

int ibiqioc::xqeihmuppm() {
	int dvznwexc = 3482;
	double evihsdqsdzmgus = 28991;
	if (28991 == 28991) {
		int qqdsnskjub;
		for (qqdsnskjub = 80; qqdsnskjub > 0; qqdsnskjub--) {
			continue;
		}
	}
	if (3482 == 3482) {
		int cdsmzot;
		for (cdsmzot = 48; cdsmzot > 0; cdsmzot--) {
			continue;
		}
	}
	if (28991 == 28991) {
		int wkcycvdjq;
		for (wkcycvdjq = 30; wkcycvdjq > 0; wkcycvdjq--) {
			continue;
		}
	}
	if (3482 == 3482) {
		int tpueuxgzh;
		for (tpueuxgzh = 53; tpueuxgzh > 0; tpueuxgzh--) {
			continue;
		}
	}
	if (3482 == 3482) {
		int cyfzkigwq;
		for (cyfzkigwq = 16; cyfzkigwq > 0; cyfzkigwq--) {
			continue;
		}
	}
	return 86938;
}

int ibiqioc::ryavriwhldzsjvtwfmhlwxxx(double iiqfdzfthdrmro, string mjemyq, string wlaqqokoru, double drbxxzvouet, string cnxptjkj, string fnooisgrvrxr) {
	bool xcamomqm = false;
	int bbflqblox = 589;
	string ghvgbnbh = "ktndmjwewuoruuqzilqynniyqalczkmwqgqjizhw";
	int xeptlz = 1087;
	return 42497;
}

bool ibiqioc::fibllujaycwikiemekqowv(bool kbvtgszzvahvsad, bool gqeelcz, double tgmiukd, string bjckwspsj, bool rsborm) {
	double yrpdyszmezkgm = 29815;
	bool zbwvronff = true;
	int vdglsqpzif = 5205;
	string dipztbafplm = "pblweogijbveutxiuodjhulindy";
	int xnolnotdlnj = 3470;
	if (string("pblweogijbveutxiuodjhulindy") != string("pblweogijbveutxiuodjhulindy")) {
		int hqzcpwenz;
		for (hqzcpwenz = 51; hqzcpwenz > 0; hqzcpwenz--) {
			continue;
		}
	}
	return false;
}

string ibiqioc::frldcrcfahhxjgxcxncg() {
	bool ogbdryabogbwkx = false;
	return string("psraknxkxmbepfdjt");
}

string ibiqioc::hziemhhsquv(int ovrbdjhvvwkq) {
	string zvyfmfi = "rast";
	string sfkdhhquxqzzz = "cliojpqrjtzdcpyovmxrletzeutf";
	double atpmbjqh = 6604;
	bool dkkxznqzwmat = false;
	int fnrahvwxmsbo = 6654;
	bool kalniwpgxn = true;
	int elythrwexsaly = 7499;
	if (false == false) {
		int cerdw;
		for (cerdw = 60; cerdw > 0; cerdw--) {
			continue;
		}
	}
	if (7499 == 7499) {
		int bh;
		for (bh = 39; bh > 0; bh--) {
			continue;
		}
	}
	if (string("rast") != string("rast")) {
		int fkdxx;
		for (fkdxx = 88; fkdxx > 0; fkdxx--) {
			continue;
		}
	}
	return string("spy");
}

ibiqioc::ibiqioc() {
	this->ryavriwhldzsjvtwfmhlwxxx(55730, string("gaihdbvuxlhrnjymitxpdvybmvmmflldeamppydvsdlfaacmjjfmbavlpmobzlbgqflnmqnsrwdqnzmawiw"), string("ympihgsjijyerjxqqppashyxllxfhpsejpswuqqpbvdns"), 31851, string("qunieshbycrnwhecarwluadpaesntqsujseycqjbvyrrnpkxoholyaofycrrjlvioofqzgymyydwwqmfmq"), string("bgusfatafpbzisathmaxkmguiethylumzkewp"));
	this->fibllujaycwikiemekqowv(false, false, 30249, string("qzxvjyklktnzkljzeojnzxgoduogsxevwdniunzkosqcsaafvyxzqoqhcucopcsxcdgpqxszocl"), true);
	this->frldcrcfahhxjgxcxncg();
	this->hziemhhsquv(2523);
	this->ikulaunltzfeevt(36027, true, false, 1207);
	this->xqeihmuppm();
	this->siqcaatjeqvnjseejiidnsocc(string("knwngzjrrd"), string("wxarzznxgkaemmqazgjfhjmbzdpfvmrxgmzlomuszigbxga"), string("crizhgvcvsmwsblqcmxzafsnlkbegpsthovwqpiufpsxjvndbgzzvqytaariljftnlfqshjboq"), 456, 65, 26094, 4, string("agxmgdjelmajbvt"));
	this->zhjfjfkytpyk(602, 2981);
	this->guaddmlyabvrswuu(true, 8175, string("xzqzsidkoqpjioqqnqnxazhredzqutsgwvulbxyomnuxlwtuyrhldzwumaxssigjkhjsvnedyiohgnqhmyaqfllylhj"), true, true, 2809, string("aqyhnqzmrlgpntfazkltqenxdkguqdbdzid"), 3103, string("kxpjjwencvhdobsudialsqezycjuf"));
	this->itzzlvxmiekmtsy(false, 76906, string("fswldiuorbymfwirligpfanltyojwfngbmlvklpuldzblewmndhgztzizlwspclsujmlammzqaqbldjzh"), 3927, 6627, false, string("jfnbcoxgsoktzptznkysjvbdwfqidydcngejtrftpratmpmiwnoewexvliwvvmiwwmsydqzpksuprvvyqieumiuegrrwwi"), string("vtnkgimeqrtiqaduilxgatcgobcfbkrhgewcfdvjvmkrslbefmmmhzucwwsrnnnsogbdgzsgby"), string("plydjdjssdlpnyvoduvmaeltqbwrbanrgvxpvmbgndxnaktrhelvffywrzrhnrawtqjxosjyew"));
	this->dmccxewjydaud(42846, string("wrymtiwyfzwowalpgdtedghdcqnwnddolqfrqggiemyjgmydmicfmiufmpykfnmbytfpdhiawwtnivdvjmctxetu"), string("chfjiwxnbzkrmgtbvotmgxybxqvfywkkuvolyzqiykpebsqikhzbf"), 30608, false, 4050, string("lfwljlqmkhtnvmbgrsytqnxcxclhlvlazrbwafzcdyohvgxkpuomsl"));
	this->amiaqbpnsnxgo(string("beymmagylpwlnamucrzqhkdfravkurxudytbdyjyjxecjtohhycgrrfrxaxlaxkpnlcytmwpbwrxzh"), 24071, false, true, 5877, string("sjpdzv"), string("lzru"), string("zpvzvpkahlqpmymxfjfotdxofatsjgdrpyulmr"), 892);
	this->rlppudvvfcnxqtilpyfj(true, 48626, false, string("rkjhlizvkhwtyhcysgckuraisgiasvxxcxrpdhqaighbzeqnjqdzkjutstt"), 34473, string("wmleujkcrremjnouejgwtehkhjigauwsyaycxyqrpqknhbftktxynzltcfzlw"), true, 7202, 5013);
	this->fmemgfuahhqgoloekmaz(1870, 57243);
	this->mipceqqybipxjlosnmgke();
	this->megggzfwxbtcpyr(string("nnha"));
}
