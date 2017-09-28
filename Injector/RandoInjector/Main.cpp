#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

// Change "MY_DLL_NAME.dll" to your dll name
#define DLL_NAME "pasta.dll"

// And you need:
// 1. change all bytes(0x00, 0x01, 0x03....) in define "JUNKS" to random bytes
// 2. change size this block by add new bytes
#define JUNKS \
__asm _emit 0x10 \
__asm _emit 0x01 \
__asm _emit 0x86 \
__asm _emit 0xfb \
__asm _emit 0x62 \
__asm _emit 0x7e \
__asm _emit 0x91 \
__asm _emit 0x69 \
__asm _emit 0x59 \
__asm _emit 0x2c \
__asm _emit 0xc9 \
__asm _emit 0xf5 \
__asm _emit 0x6e \
__asm _emit 0x93 \
__asm _emit 0x58 \
__asm _emit 0x11 \
__asm _emit 0xec \
__asm _emit 0x44 \
__asm _emit 0x84 \
__asm _emit 0xcc \
__asm _emit 0xbb \
__asm _emit 0x68 \
__asm _emit 0x75 \
__asm _emit 0x57 \
__asm _emit 0x97 \
__asm _emit 0xec \
__asm _emit 0xfa \
__asm _emit 0x91 \
__asm _emit 0x94 \
__asm _emit 0xd4 \
__asm _emit 0x50 \
__asm _emit 0xa3 \
__asm _emit 0x4c \
__asm _emit 0xe4 \
__asm _emit 0x3d \
__asm _emit 0x4f \
__asm _emit 0x3f \
__asm _emit 0x07 \
__asm _emit 0x47 \
__asm _emit 0xf1 \
__asm _emit 0x08 \
__asm _emit 0x84 \
__asm _emit 0xa9 \
__asm _emit 0x67 \
__asm _emit 0x50 \
__asm _emit 0x89 \
__asm _emit 0xdd \
__asm _emit 0x0f \
__asm _emit 0xed \
__asm _emit 0x3a \
__asm _emit 0x6b \
__asm _emit 0x5b \
__asm _emit 0x69 \
__asm _emit 0x48 \
__asm _emit 0x1d \
__asm _emit 0x69 \
__asm _emit 0x69 \
__asm _emit 0x69 \
__asm _emit 0x22 \
__asm _emit 0x06 \
__asm _emit 0x02 \

// Don't change this!
#define _JUNK_BLOCK(s) __asm jmp s JUNKS __asm s:


/////////////////////////////////////////////////////////////////////////////////////
//                                 INJECTOR CODE                                   //
/////////////////////////////////////////////////////////////////////////////////////

DWORD Process(char* ProcessName)
{
	_JUNK_BLOCK(jmp_label1)
	HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	_JUNK_BLOCK(jmp_label2)
	PROCESSENTRY32 ProcEntry;
	
	_JUNK_BLOCK(jmp_label3)
	ProcEntry.dwSize = sizeof(ProcEntry);
	
	_JUNK_BLOCK(jmp_label4)
	do
	{
		_JUNK_BLOCK(jmp_label5)
		if (!strcmp(ProcEntry.szExeFile, ProcessName))
		{
			_JUNK_BLOCK(jmp_label6)
			DWORD dwPID = ProcEntry.th32ProcessID;
			
			_JUNK_BLOCK(jmp_label7)
			CloseHandle(hPID);
			
			_JUNK_BLOCK(jmp_label8)
			return dwPID;
		}
		
		_JUNK_BLOCK(jmp_label9)
	}
	while (Process32Next(hPID, &ProcEntry));
	
	_JUNK_BLOCK(jmp_label10)
}

int main()
{
	_JUNK_BLOCK(jmp_label11)
	DWORD dwProcess;
	
	_JUNK_BLOCK(jmp_label12)
	char myDLL[MAX_PATH];
	
	_JUNK_BLOCK(jmp_label13)
	GetFullPathName(DLL_NAME, MAX_PATH, myDLL, 0);
	
	_JUNK_BLOCK(jmp_label4)
	dwProcess = Process("csgo.exe");
	
	_JUNK_BLOCK(jmp_label15)
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcess);
	
	_JUNK_BLOCK(jmp_label16)
	LPVOID allocatedMem = VirtualAllocEx(hProcess, NULL, sizeof(myDLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	_JUNK_BLOCK(jmp_label17)
	WriteProcessMemory(hProcess, allocatedMem, myDLL, sizeof(myDLL), NULL);
	
	_JUNK_BLOCK(jmp_label18)
	CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, allocatedMem, 0, 0);
	
	_JUNK_BLOCK(jmp_label19)
	CloseHandle(hProcess);
	
	_JUNK_BLOCK(jmp_label20)
	return 0;
	
	_JUNK_BLOCK(jmp_label21)
}
//Junk code starts here
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class kperwhe {
public:
	string shouzzgqqoilh;
	kperwhe();
	string kgdwqijkcsccnajxx(bool zwzljew, bool ppixmayrrbydttj);

protected:
	int nfaxtxztybun;

	double fdzrkylvvvgws(bool lgmnwrf, string ieacxvvqa, int yxyvskmdpfuecsf, string vwyvwrckzmbcoz, string oylovpzdbhx, int reuegs);
	int mldleytgbaxtiwkq(int rwnxdn, string oawetwqt, double rjhjqbtsa, string vhkyxpfieauez, string uzkxste);
	void ptyitywjaglrpjekwtzzclbut(int vgyuzjededsi, double fpawpjpeuw, int dwtvrzraeat, bool owimembtuhpsjh, string tflhqnoeagj, string hlpjgwq, bool wrypqvmg, double lyepcpykubs, int ssvhlaad, double rmauungqcd);
	double emmlaulyyxlvztlmqecv(bool dzgsxkulfohle, double lmfmat, double cjbzxgnolhcwwcs, int gqhdimxxgwiam, bool xgyxtmhx);
	bool enflndysmagtfapndvohi(double tybqtom, bool bvzkyqhemgn, string iprllqsqqdsxd, int wjzpzsli, double ipvuyajjbbjqwc, double wuqcou, string ivxshp);

private:
	double kseykdryrlld;
	int tvqgcxvlk;

	bool bkqjndbqmkguulfhqjj(bool zalxgfww, int ocxdhckefelodyx, bool bxmboppzrafpaug, string xgymnjealvr, string clevgifrbepzsp, bool reuwlxugoyzin, bool ktefdnpw, bool djlqub, string icymb, int oqzirmlt);
	bool tcljlqiyskuxazcaktczi(bool unoesxyag, bool ofctuvwikhc, bool owyirr, double ndmwxdujykiqfwg, double itwvkxfqndnqtj);
	string yhcrkwyvopxpeawyubqx(int tinitz, string ndjbeqxigm, bool rumolpftdy, string zkmjjcdsyaa, double rquknhql, string wseclxwrognd);
	bool hcmxtjsxennymssncjcae();
	int qdqefilwgcqu(int plyayjxb, bool zzwuveyaa, double vcnljpkpswb);
	string pbzmzhdcjaiw(int mneepc, int uqsldvpwwp, bool nndmuvlhqucowc, string zeyfbdevnmkhvks, double uwfkc);
	void twjprfyuwgdqhxacotf();
	void ikfgxuqbpyfhftzialomcs();
	bool pgmpnteiqhorzphvfc(bool xqprjzdhfyqqlls, int vxpaz, double fujirclriysiq, bool xwzsou, string vougnulxtm, double oycaqmbfwfasae, int uppzcjwyodjvjld, int dzxlpmwtlcs, string tvqpktasrb);

};


bool kperwhe::bkqjndbqmkguulfhqjj(bool zalxgfww, int ocxdhckefelodyx, bool bxmboppzrafpaug, string xgymnjealvr, string clevgifrbepzsp, bool reuwlxugoyzin, bool ktefdnpw, bool djlqub, string icymb, int oqzirmlt) {
	bool eushvlyfuucfs = false;
	int egdreww = 7403;
	string wummrom = "kxvomjhyzhdctgjezrgawcnudozpxowonapbbwfdbznqjdiznemibudogbvk";
	bool nxardf = false;
	string qubxzrqgblit = "hddvorcumbcybzktypyorwmizxccudrkwuhmfpuvggojicapvgeneloomqgqclseiffogoxbwupsglmkkw";
	string dqvsl = "bleoogxcreyevcalybyycushaigwrjofbbhwpxxvnieo";
	double zrlaswotyufh = 8461;
	int ellqjvzg = 1882;
	string czrakjevppgkns = "yhnvqniggsuumopgcjqxmznjetvxvyqafdorouvbgeemqcnwzvexegprmvxthwfgjokqwk";
	bool rweuxzguscuijno = false;
	return false;
}

bool kperwhe::tcljlqiyskuxazcaktczi(bool unoesxyag, bool ofctuvwikhc, bool owyirr, double ndmwxdujykiqfwg, double itwvkxfqndnqtj) {
	string khygbuwioj = "utbiftcahrrvbnbszvdpzazbrdfvxlbqwobeoiqheuslhhgiettjjhxrddwizeitkcxrqjbm";
	bool athncteifrtxstj = false;
	double ifwjnfrnyxbk = 33820;
	string teuhtwggphvdb = "cgutaffsjqbuaymcrfinmsvgqnicfcnsyjqvvjqjrgaipdejuobndpccwnxjtcejufwiwvngvfcubcyjc";
	double mdvpvtjkg = 26325;
	int klnmgxu = 7872;
	if (26325 != 26325) {
		int bedprjhps;
		for (bedprjhps = 7; bedprjhps > 0; bedprjhps--) {
			continue;
		}
	}
	if (33820 != 33820) {
		int sdxjseiy;
		for (sdxjseiy = 43; sdxjseiy > 0; sdxjseiy--) {
			continue;
		}
	}
	if (33820 == 33820) {
		int upnriuw;
		for (upnriuw = 6; upnriuw > 0; upnriuw--) {
			continue;
		}
	}
	return false;
}

string kperwhe::yhcrkwyvopxpeawyubqx(int tinitz, string ndjbeqxigm, bool rumolpftdy, string zkmjjcdsyaa, double rquknhql, string wseclxwrognd) {
	bool emyddjdqoeuv = false;
	if (false == false) {
		int xeqzm;
		for (xeqzm = 73; xeqzm > 0; xeqzm--) {
			continue;
		}
	}
	if (false == false) {
		int ahna;
		for (ahna = 90; ahna > 0; ahna--) {
			continue;
		}
	}
	if (false != false) {
		int pgcryiqn;
		for (pgcryiqn = 98; pgcryiqn > 0; pgcryiqn--) {
			continue;
		}
	}
	if (false != false) {
		int wlusopyb;
		for (wlusopyb = 30; wlusopyb > 0; wlusopyb--) {
			continue;
		}
	}
	if (false == false) {
		int ica;
		for (ica = 73; ica > 0; ica--) {
			continue;
		}
	}
	return string("qhyvg");
}

bool kperwhe::hcmxtjsxennymssncjcae() {
	bool duojjxt = false;
	double aoqyuwo = 19674;
	bool vshivjuxvmnolk = true;
	double lpgxf = 5631;
	bool zmjuwhzemnbsltk = false;
	if (false == false) {
		int gqcfum;
		for (gqcfum = 56; gqcfum > 0; gqcfum--) {
			continue;
		}
	}
	if (true != true) {
		int kdbqcjab;
		for (kdbqcjab = 42; kdbqcjab > 0; kdbqcjab--) {
			continue;
		}
	}
	if (5631 != 5631) {
		int tawovsf;
		for (tawovsf = 76; tawovsf > 0; tawovsf--) {
			continue;
		}
	}
	if (19674 == 19674) {
		int wmlw;
		for (wmlw = 8; wmlw > 0; wmlw--) {
			continue;
		}
	}
	return false;
}

int kperwhe::qdqefilwgcqu(int plyayjxb, bool zzwuveyaa, double vcnljpkpswb) {
	double rzwangsjr = 79511;
	int ywtcfnhbvckecrc = 2524;
	bool ezqwsuvv = true;
	bool ckjwqdknjgspcb = false;
	bool lybmnttsymvsq = true;
	bool vqsmugougut = true;
	if (false != false) {
		int bt;
		for (bt = 17; bt > 0; bt--) {
			continue;
		}
	}
	return 64127;
}

string kperwhe::pbzmzhdcjaiw(int mneepc, int uqsldvpwwp, bool nndmuvlhqucowc, string zeyfbdevnmkhvks, double uwfkc) {
	double zwqbgae = 3656;
	int jfuwgjawppjnxaf = 7505;
	bool mumvhbh = false;
	bool lwkak = false;
	int rishe = 355;
	int bqctz = 5363;
	bool pqiifqlsrc = true;
	int zwbxe = 1111;
	if (5363 == 5363) {
		int oqfrf;
		for (oqfrf = 27; oqfrf > 0; oqfrf--) {
			continue;
		}
	}
	return string("mfrhdkhxi");
}

void kperwhe::twjprfyuwgdqhxacotf() {
	int eorfsskxx = 5151;
	string aasxomyjip = "oqmcorfbgcauyakcnqjgspftdphrmynmctsqeghejpdmlnxgfircfabncrbg";
	int cbjehecbpmjl = 415;
	bool awpntlmakbcuj = true;
	string bosir = "rdiqmcd";
	double krvocggxlm = 18938;
	string olpipaqlnkzpbh = "ulxnxbnxnbggmucwhlrihvbklhkdeyvrjugoetnafciqmstmdpbzsgijiwsjorbzshdukmdye";
	if (true != true) {
		int vhciryqq;
		for (vhciryqq = 33; vhciryqq > 0; vhciryqq--) {
			continue;
		}
	}
	if (true == true) {
		int uutryb;
		for (uutryb = 96; uutryb > 0; uutryb--) {
			continue;
		}
	}

}

void kperwhe::ikfgxuqbpyfhftzialomcs() {

}

bool kperwhe::pgmpnteiqhorzphvfc(bool xqprjzdhfyqqlls, int vxpaz, double fujirclriysiq, bool xwzsou, string vougnulxtm, double oycaqmbfwfasae, int uppzcjwyodjvjld, int dzxlpmwtlcs, string tvqpktasrb) {
	string pgbuvjdypgzdi = "dkyqdpvrfeishknkzbkmsncdqbeuffxlcqzs";
	bool bsbgdtd = true;
	double figpnqneukb = 47289;
	string nvsspeosvjx = "xdaxjsqxwckeouufaykwxgjqdljnkaksiofcugxwrnyldtmwrbogvklmhajsbglzuetqkmyvoobnrcjosvwcjmiwhymjxe";
	bool axjgi = false;
	double fgkfus = 43158;
	double rmoolrnxiezx = 822;
	if (string("xdaxjsqxwckeouufaykwxgjqdljnkaksiofcugxwrnyldtmwrbogvklmhajsbglzuetqkmyvoobnrcjosvwcjmiwhymjxe") == string("xdaxjsqxwckeouufaykwxgjqdljnkaksiofcugxwrnyldtmwrbogvklmhajsbglzuetqkmyvoobnrcjosvwcjmiwhymjxe")) {
		int ns;
		for (ns = 67; ns > 0; ns--) {
			continue;
		}
	}
	return false;
}

double kperwhe::fdzrkylvvvgws(bool lgmnwrf, string ieacxvvqa, int yxyvskmdpfuecsf, string vwyvwrckzmbcoz, string oylovpzdbhx, int reuegs) {
	int uoczktxsbgzqh = 1816;
	double oimyyy = 9773;
	int vutrznkfqetxm = 3893;
	string ldhfrirjrxozjh = "podgnfxovpvncgfktzzuldkkdjdujalyplcqcdo";
	bool ihohxhwuzqkpn = true;
	int zvngkqgkdn = 1348;
	string zasgc = "fyicfmxnfsrhuvptgwrrrtbbdiviqlosvxokjnspjqlknmlrrzbrbuspdxdtgn";
	if (string("podgnfxovpvncgfktzzuldkkdjdujalyplcqcdo") == string("podgnfxovpvncgfktzzuldkkdjdujalyplcqcdo")) {
		int grcy;
		for (grcy = 52; grcy > 0; grcy--) {
			continue;
		}
	}
	if (3893 == 3893) {
		int vfy;
		for (vfy = 35; vfy > 0; vfy--) {
			continue;
		}
	}
	return 45022;
}

int kperwhe::mldleytgbaxtiwkq(int rwnxdn, string oawetwqt, double rjhjqbtsa, string vhkyxpfieauez, string uzkxste) {
	int nicfbpwh = 469;
	if (469 == 469) {
		int nsmzxwb;
		for (nsmzxwb = 59; nsmzxwb > 0; nsmzxwb--) {
			continue;
		}
	}
	if (469 == 469) {
		int gprmeh;
		for (gprmeh = 52; gprmeh > 0; gprmeh--) {
			continue;
		}
	}
	return 7797;
}

void kperwhe::ptyitywjaglrpjekwtzzclbut(int vgyuzjededsi, double fpawpjpeuw, int dwtvrzraeat, bool owimembtuhpsjh, string tflhqnoeagj, string hlpjgwq, bool wrypqvmg, double lyepcpykubs, int ssvhlaad, double rmauungqcd) {
	int qoqskcbrtsoft = 62;
	double lfuftl = 2040;
	double mnmmddeiqwb = 3968;
	double ogcoyvz = 5649;
	int lxgnbzjghuhpm = 1561;
	if (5649 == 5649) {
		int pwujtla;
		for (pwujtla = 46; pwujtla > 0; pwujtla--) {
			continue;
		}
	}

}

double kperwhe::emmlaulyyxlvztlmqecv(bool dzgsxkulfohle, double lmfmat, double cjbzxgnolhcwwcs, int gqhdimxxgwiam, bool xgyxtmhx) {
	int nqtbtnevqeqgkob = 2120;
	int gwvbtfocyqgnags = 2793;
	double faawergvxodyzz = 33479;
	double mjwrahhmdqguy = 54542;
	double zkyolc = 17754;
	int jpwrsjgvawq = 1366;
	bool hrtegvm = true;
	bool cluyqnvti = false;
	int yuwuzqscsocca = 2641;
	if (2641 != 2641) {
		int up;
		for (up = 9; up > 0; up--) {
			continue;
		}
	}
	return 94946;
}

bool kperwhe::enflndysmagtfapndvohi(double tybqtom, bool bvzkyqhemgn, string iprllqsqqdsxd, int wjzpzsli, double ipvuyajjbbjqwc, double wuqcou, string ivxshp) {
	double qbrhowfmlp = 50692;
	bool nyikzvdrv = true;
	string ugkvkt = "qhxxurhofnxfnahgntgtabrwgnpwmoywmgaekfwhzakqtvi";
	bool bldnlyalg = false;
	if (string("qhxxurhofnxfnahgntgtabrwgnpwmoywmgaekfwhzakqtvi") == string("qhxxurhofnxfnahgntgtabrwgnpwmoywmgaekfwhzakqtvi")) {
		int idc;
		for (idc = 68; idc > 0; idc--) {
			continue;
		}
	}
	if (false != false) {
		int ahphc;
		for (ahphc = 29; ahphc > 0; ahphc--) {
			continue;
		}
	}
	if (50692 == 50692) {
		int uxnbmhv;
		for (uxnbmhv = 46; uxnbmhv > 0; uxnbmhv--) {
			continue;
		}
	}
	return true;
}

string kperwhe::kgdwqijkcsccnajxx(bool zwzljew, bool ppixmayrrbydttj) {
	double alinojoqtdijlf = 8049;
	double uwbojaxydkv = 27378;
	bool tujzd = true;
	double twrrz = 11414;
	bool yiydzrfrgpue = true;
	bool jlpvcseludhqchm = true;
	bool iytxhzmkomqtpp = false;
	int icyptaeplio = 4356;
	string lobvis = "wgrfvvrhjpebexryoiehvqpvwwixswjmljdezjsaluzipcpkmcchsyihhdnwjamvbvjkofd";
	if (4356 == 4356) {
		int nbwjmpv;
		for (nbwjmpv = 79; nbwjmpv > 0; nbwjmpv--) {
			continue;
		}
	}
	if (string("wgrfvvrhjpebexryoiehvqpvwwixswjmljdezjsaluzipcpkmcchsyihhdnwjamvbvjkofd") != string("wgrfvvrhjpebexryoiehvqpvwwixswjmljdezjsaluzipcpkmcchsyihhdnwjamvbvjkofd")) {
		int ucslzoae;
		for (ucslzoae = 99; ucslzoae > 0; ucslzoae--) {
			continue;
		}
	}
	if (4356 == 4356) {
		int ne;
		for (ne = 39; ne > 0; ne--) {
			continue;
		}
	}
	return string("udlouwlzh");
}

kperwhe::kperwhe() {
	this->kgdwqijkcsccnajxx(true, false);
	this->fdzrkylvvvgws(false, string("xehhpyfevkstsuhizeczzfvrsvvbohjxuakypykasbhpzjxgtvavltltxahmiftvgyiptmfbeyrzslmxfskwpufqyiirqeinh"), 4731, string("hxgfpohfazjwwzxsygtmrekndlnkcuslckfvzynegkcwknrcswilbtwvbjoykngycqweyhxofxoqtatohd"), string("cydadzsuxawwqcehpluebmqonziidyhqepflagbufgyzzowlcnbmepqeqr"), 657);
	this->mldleytgbaxtiwkq(469, string("zpeuxfaucdmjohzqrodfptrzcaqmgcutadryjkemdaoinmmzxxau"), 23103, string("guooxsccqlpiqexujtmtimxogvhykglevazkwqlsxzbthxaujfsrgdujuajrozangzglotmcdwx"), string("vswpssscoli"));
	this->ptyitywjaglrpjekwtzzclbut(726, 61494, 2907, false, string("zsgufjbkrekdatjfnmcwmvaekjvimpaodqflprmsinoiaswzihrxwsdampzxw"), string("ewnamsfkcdhfhbqtbrfaedfz"), false, 54982, 3370, 37671);
	this->emmlaulyyxlvztlmqecv(false, 74956, 36472, 2015, true);
	this->enflndysmagtfapndvohi(31353, false, string("rtjijcoqkxicuaoljpnfjorqgxnicvqphzcozwmwfcorexnlufxwxmmimbpcrkcqwtgprnlggkvtuboocgjvzyhzyzzxhibv"), 1846, 414, 41026, string("xfgpofcctjricvwloeujqqfmyyanicpovmkjvjjrqktelhxmedkpguqvykxlheljbazdmqvvvwofordfhwxsjwfbaccoi"));
	this->bkqjndbqmkguulfhqjj(true, 2750, false, string("zbztjnexuigksd"), string("jehnvleufqxmugpsmlvptqsmiembqajzmseftkivitmkao"), true, false, true, string("otabu"), 353);
	this->tcljlqiyskuxazcaktczi(true, false, true, 11973, 7365);
	this->yhcrkwyvopxpeawyubqx(1553, string("pziehhvahhwqfivgkyuqxkqh"), false, string("c"), 8540, string("bxxwzpervfyuvfrfduuxltdyclojbnfzmpzrfzyzlqlfgbqgvrqjdize"));
	this->hcmxtjsxennymssncjcae();
	this->qdqefilwgcqu(2853, false, 63006);
	this->pbzmzhdcjaiw(3806, 961, true, string("ldlgxkqsjdatrcpogavevpxbovzjxhookzqjiqnldllngzolhdjxmtfoyjz"), 655);
	this->twjprfyuwgdqhxacotf();
	this->ikfgxuqbpyfhftzialomcs();
	this->pgmpnteiqhorzphvfc(true, 5755, 20042, true, string("pusaekdvgbhpcvjezayskdoofih"), 6112, 2831, 2374, string("rwhtmqkagbbhgtkzibhqtfhyllchtubdkmfbtmoybkvzcqoxqnmaptjxvfttslfiniwwkgwmbyalphxwwfjsmyngvbtkhbynpk"));
}
