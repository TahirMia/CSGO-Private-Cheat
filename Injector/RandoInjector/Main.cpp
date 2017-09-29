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
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class taxqzjw {
public:
	double wazlhhlchiwnihz;
	bool bfgegy;
	int gmftmf;
	int dqtgfczt;
	double gyvmmuetcqqsth;
	taxqzjw();
	double txrxzgolmvcjdga(double mtklyfs, bool gaxmdrr, int ycgiwdg, bool jocja, string byqvlo, int jqlloovmos, bool xupwmccn);
	void chmgcpmecdglbyjpur(string jnrneu, bool nbbeulywnyp, bool kgyormud, string yqpfav, int tqmzgjkc);
	bool jigioczedj();
	int jdmxwarqmsznwuxewookc(string byxwtsevbvf, string tqrzynf, string yburax, bool ctmgctmc, int uikkesj, string yithbkxcspfq, double dmwhauabxaobsja, bool bolyjotatfcqad, int krhodsgwlugkmso, double jmrnwqltopjkq);
	double naneprakzoxtax(int cabeljttfdf, int andufuxypi, bool wbokzppxvprzh, bool ofkbgwfr, double advbt, string gpbknjotmyp, double ofgyiiqrqxic);
	bool scqdkiyicvjnyru(string qhewpqeemvoxcee);
	double imsbqqtmjfwhilumjybhubv();
	double asvgdwsyfe(double srxbqlptbjx, string hdflqkuyvqain, int zgjswuf, bool imqso, double pltjgku, string lrrgtwpcxdks, bool snupn, double uanayvpydu);
	bool qknphypkcipmvzjoh(bool actjbq, bool wxvkej, int yvacn, bool rxceorph, string ebpwru);

protected:
	string prodlk;
	int kgueftx;

	void iibdihhmnmiggqaddtewsmdzy(double miqzunpsdlk, int lcnkcxk, string fatqmnmuuansfh);
	double xgorkpgvmwb();

private:
	double xvitoeyyfw;

	string kwytshurumhlaoox(double wbzmqmbex, double ohubhfegpci, double wezufda);
	int nsujwsicghgabbmu(string eljrcxqehyibri, int vgylfubxquazklq, bool zdwjogmrbfivjuz, string krcmbwvoljo, string dsvmgsnvnzd, string liswm, int hhwspsjs);

};


string taxqzjw::kwytshurumhlaoox(double wbzmqmbex, double ohubhfegpci, double wezufda) {
	double xtcns = 16509;
	bool zfvgiunu = false;
	double kkwdcgiww = 79274;
	int jiymfyurfss = 9184;
	int reduwnrkbueoqun = 844;
	int htdcju = 5634;
	int clbam = 381;
	int muloiduldgjudkt = 1645;
	string dhagvbigehy = "nvyshwyltmxozlgbddyewxgabpxlocpriyl";
	bool clgezclnnnbtqsi = false;
	if (9184 != 9184) {
		int ncoyhg;
		for (ncoyhg = 6; ncoyhg > 0; ncoyhg--) {
			continue;
		}
	}
	return string("t");
}

int taxqzjw::nsujwsicghgabbmu(string eljrcxqehyibri, int vgylfubxquazklq, bool zdwjogmrbfivjuz, string krcmbwvoljo, string dsvmgsnvnzd, string liswm, int hhwspsjs) {
	return 97807;
}

void taxqzjw::iibdihhmnmiggqaddtewsmdzy(double miqzunpsdlk, int lcnkcxk, string fatqmnmuuansfh) {
	string mdstqpbrngte = "pvrxjqesrmqjfzlhniqylvoiiotfknximhcismkbuzfmcptdbzhumnkktqbzjuox";
	bool eaiszkmpywsev = false;
	int nkxxkeawszdkt = 1310;
	int ixzwdhbhjxop = 2166;
	string hggfnrurfwfrp = "snyqnwkwjjoinaimxuyijbmdvhsdtxeszzikcikkdejilatmtvknmkbvignwsxmzyrxiryymtssr";
	if (string("pvrxjqesrmqjfzlhniqylvoiiotfknximhcismkbuzfmcptdbzhumnkktqbzjuox") != string("pvrxjqesrmqjfzlhniqylvoiiotfknximhcismkbuzfmcptdbzhumnkktqbzjuox")) {
		int hsnlrrv;
		for (hsnlrrv = 46; hsnlrrv > 0; hsnlrrv--) {
			continue;
		}
	}
	if (string("snyqnwkwjjoinaimxuyijbmdvhsdtxeszzikcikkdejilatmtvknmkbvignwsxmzyrxiryymtssr") != string("snyqnwkwjjoinaimxuyijbmdvhsdtxeszzikcikkdejilatmtvknmkbvignwsxmzyrxiryymtssr")) {
		int cdlrkhi;
		for (cdlrkhi = 72; cdlrkhi > 0; cdlrkhi--) {
			continue;
		}
	}

}

double taxqzjw::xgorkpgvmwb() {
	double pooprslkvuds = 31137;
	bool cgrtlcxblc = true;
	if (31137 == 31137) {
		int wfufzx;
		for (wfufzx = 59; wfufzx > 0; wfufzx--) {
			continue;
		}
	}
	return 88093;
}

double taxqzjw::txrxzgolmvcjdga(double mtklyfs, bool gaxmdrr, int ycgiwdg, bool jocja, string byqvlo, int jqlloovmos, bool xupwmccn) {
	string kndpii = "ygpfeiqosifsy";
	double ozvxzsbfkspvnz = 78488;
	string qvjnofqeeep = "lyksovwyigmumusgokaipnnnnrjnqliqkjlxg";
	if (string("lyksovwyigmumusgokaipnnnnrjnqliqkjlxg") == string("lyksovwyigmumusgokaipnnnnrjnqliqkjlxg")) {
		int nfjojw;
		for (nfjojw = 60; nfjojw > 0; nfjojw--) {
			continue;
		}
	}
	if (78488 != 78488) {
		int dxcp;
		for (dxcp = 13; dxcp > 0; dxcp--) {
			continue;
		}
	}
	return 6351;
}

void taxqzjw::chmgcpmecdglbyjpur(string jnrneu, bool nbbeulywnyp, bool kgyormud, string yqpfav, int tqmzgjkc) {
	string nxxkgpttwjfgjd = "cqoeimjertpptqootxipedmkrqfdgsxztcwrhifrimgxsxxidgjgxlaawillqbdofipuawlhvqfkooz";
	string iypdlmawt = "qtovxnvlfmwksnfjxhfoisezw";
	double phzdbsvpstjrgc = 26899;
	if (string("cqoeimjertpptqootxipedmkrqfdgsxztcwrhifrimgxsxxidgjgxlaawillqbdofipuawlhvqfkooz") != string("cqoeimjertpptqootxipedmkrqfdgsxztcwrhifrimgxsxxidgjgxlaawillqbdofipuawlhvqfkooz")) {
		int mb;
		for (mb = 91; mb > 0; mb--) {
			continue;
		}
	}
	if (26899 == 26899) {
		int hg;
		for (hg = 53; hg > 0; hg--) {
			continue;
		}
	}
	if (26899 != 26899) {
		int estifpsq;
		for (estifpsq = 3; estifpsq > 0; estifpsq--) {
			continue;
		}
	}

}

bool taxqzjw::jigioczedj() {
	string tggkgctkv = "aahqlvfyxdntmopfucpamcjxkwfqysziysrkvuwhminlzlteylmuejwqiwoqh";
	double czaywi = 27601;
	bool swufzqvlyflaaug = true;
	bool dattpw = true;
	string muwgkxjrjterpkr = "gcgvvuihkdvoilkzybiapgzcvogounqzvitqoj";
	bool crwmwgbeeooa = true;
	double ocfxyssglkqqf = 86809;
	int ngbqqws = 182;
	double vjpgal = 719;
	bool zsrtvtwr = false;
	if (719 == 719) {
		int eejqfiusox;
		for (eejqfiusox = 49; eejqfiusox > 0; eejqfiusox--) {
			continue;
		}
	}
	if (27601 == 27601) {
		int ctmch;
		for (ctmch = 71; ctmch > 0; ctmch--) {
			continue;
		}
	}
	if (string("aahqlvfyxdntmopfucpamcjxkwfqysziysrkvuwhminlzlteylmuejwqiwoqh") != string("aahqlvfyxdntmopfucpamcjxkwfqysziysrkvuwhminlzlteylmuejwqiwoqh")) {
		int guqi;
		for (guqi = 8; guqi > 0; guqi--) {
			continue;
		}
	}
	if (true != true) {
		int tfotlyq;
		for (tfotlyq = 18; tfotlyq > 0; tfotlyq--) {
			continue;
		}
	}
	if (true == true) {
		int ryebbnkj;
		for (ryebbnkj = 49; ryebbnkj > 0; ryebbnkj--) {
			continue;
		}
	}
	return false;
}

int taxqzjw::jdmxwarqmsznwuxewookc(string byxwtsevbvf, string tqrzynf, string yburax, bool ctmgctmc, int uikkesj, string yithbkxcspfq, double dmwhauabxaobsja, bool bolyjotatfcqad, int krhodsgwlugkmso, double jmrnwqltopjkq) {
	int jpadrjdqqc = 3411;
	string gpxcgxicg = "nmtbrzbmjtcnbpxjysjuporefuwziljtyytwyrbmujrecsrttgfvkjcoibhbwjecfmuoruouwjfgobgm";
	bool fcekzugccyubn = true;
	string othukxcjbkhqj = "c";
	int lwdhkdyydiknrm = 5849;
	bool miibuwcikjeyly = true;
	string uyasixs = "yzncrocesholzebkdkmnjijtnlilmcjkivlzhifrkoxatrkwlcewrpvqlgihlacvvcojdiq";
	double mkegmcgfe = 26477;
	if (true != true) {
		int uyqfm;
		for (uyqfm = 38; uyqfm > 0; uyqfm--) {
			continue;
		}
	}
	if (string("nmtbrzbmjtcnbpxjysjuporefuwziljtyytwyrbmujrecsrttgfvkjcoibhbwjecfmuoruouwjfgobgm") != string("nmtbrzbmjtcnbpxjysjuporefuwziljtyytwyrbmujrecsrttgfvkjcoibhbwjecfmuoruouwjfgobgm")) {
		int skvj;
		for (skvj = 20; skvj > 0; skvj--) {
			continue;
		}
	}
	if (string("c") != string("c")) {
		int yhkcp;
		for (yhkcp = 1; yhkcp > 0; yhkcp--) {
			continue;
		}
	}
	return 91667;
}

double taxqzjw::naneprakzoxtax(int cabeljttfdf, int andufuxypi, bool wbokzppxvprzh, bool ofkbgwfr, double advbt, string gpbknjotmyp, double ofgyiiqrqxic) {
	int etguwfbwefnir = 5459;
	double mfipribsivd = 35177;
	string yyabzfyanzi = "zdweljbsdpqbucyjvozoaxoubondczxeojaicfveumnvfanzxsaqvhejfvzsnulmmaybqnd";
	string yxscq = "furwaahkakdiyyz";
	string brkbxedhcvpx = "wekjlnmkuioxalojtclgmikmvqandnjihxtnrhpzzhqnvjbxzmkqsxrx";
	int lhcxmcuzasao = 6010;
	int jplosivu = 6175;
	int hitfwxyrisuvq = 1599;
	string wrqenpevjkfkbc = "npgbxtzzoddaehtsuveoogqimntflzxxqhzgofwdqfulqpyfuqrkohejxzjhchvbfpfouoantprblmbkthasweb";
	if (1599 != 1599) {
		int brrmyidm;
		for (brrmyidm = 9; brrmyidm > 0; brrmyidm--) {
			continue;
		}
	}
	if (1599 != 1599) {
		int znx;
		for (znx = 95; znx > 0; znx--) {
			continue;
		}
	}
	if (5459 == 5459) {
		int nojxwglu;
		for (nojxwglu = 84; nojxwglu > 0; nojxwglu--) {
			continue;
		}
	}
	if (6010 == 6010) {
		int ir;
		for (ir = 17; ir > 0; ir--) {
			continue;
		}
	}
	return 72439;
}

bool taxqzjw::scqdkiyicvjnyru(string qhewpqeemvoxcee) {
	return true;
}

double taxqzjw::imsbqqtmjfwhilumjybhubv() {
	int eaorvs = 638;
	if (638 != 638) {
		int cpxgnwmc;
		for (cpxgnwmc = 57; cpxgnwmc > 0; cpxgnwmc--) {
			continue;
		}
	}
	if (638 == 638) {
		int ldachssb;
		for (ldachssb = 66; ldachssb > 0; ldachssb--) {
			continue;
		}
	}
	if (638 == 638) {
		int pjfpjb;
		for (pjfpjb = 48; pjfpjb > 0; pjfpjb--) {
			continue;
		}
	}
	if (638 == 638) {
		int bdqcxgd;
		for (bdqcxgd = 0; bdqcxgd > 0; bdqcxgd--) {
			continue;
		}
	}
	if (638 == 638) {
		int xfdfvuxjny;
		for (xfdfvuxjny = 61; xfdfvuxjny > 0; xfdfvuxjny--) {
			continue;
		}
	}
	return 80369;
}

double taxqzjw::asvgdwsyfe(double srxbqlptbjx, string hdflqkuyvqain, int zgjswuf, bool imqso, double pltjgku, string lrrgtwpcxdks, bool snupn, double uanayvpydu) {
	string gvebixlo = "shihugockycvrpoycufklvobwshgecbjccvsdudskchofnfodijvfymppaykfnuofnpvdvjotnuc";
	string qaxlma = "xztyfsmpwunujunltrviuxhioyk";
	int ofssrmen = 3752;
	if (string("shihugockycvrpoycufklvobwshgecbjccvsdudskchofnfodijvfymppaykfnuofnpvdvjotnuc") == string("shihugockycvrpoycufklvobwshgecbjccvsdudskchofnfodijvfymppaykfnuofnpvdvjotnuc")) {
		int dihz;
		for (dihz = 67; dihz > 0; dihz--) {
			continue;
		}
	}
	if (3752 != 3752) {
		int ljrxbaqc;
		for (ljrxbaqc = 41; ljrxbaqc > 0; ljrxbaqc--) {
			continue;
		}
	}
	if (string("shihugockycvrpoycufklvobwshgecbjccvsdudskchofnfodijvfymppaykfnuofnpvdvjotnuc") != string("shihugockycvrpoycufklvobwshgecbjccvsdudskchofnfodijvfymppaykfnuofnpvdvjotnuc")) {
		int sobxdcxh;
		for (sobxdcxh = 61; sobxdcxh > 0; sobxdcxh--) {
			continue;
		}
	}
	if (string("xztyfsmpwunujunltrviuxhioyk") == string("xztyfsmpwunujunltrviuxhioyk")) {
		int xvmeteyuy;
		for (xvmeteyuy = 33; xvmeteyuy > 0; xvmeteyuy--) {
			continue;
		}
	}
	return 93878;
}

bool taxqzjw::qknphypkcipmvzjoh(bool actjbq, bool wxvkej, int yvacn, bool rxceorph, string ebpwru) {
	int sutvqdkwo = 3986;
	int wcnawcqle = 4144;
	bool lkxjqawugbetsou = true;
	bool imqvmtfwvck = true;
	string aqptvr = "jviavtu";
	int mfcnlt = 301;
	if (4144 != 4144) {
		int zrwm;
		for (zrwm = 23; zrwm > 0; zrwm--) {
			continue;
		}
	}
	if (301 != 301) {
		int kjwrxnbklh;
		for (kjwrxnbklh = 92; kjwrxnbklh > 0; kjwrxnbklh--) {
			continue;
		}
	}
	if (3986 == 3986) {
		int barwwkmm;
		for (barwwkmm = 29; barwwkmm > 0; barwwkmm--) {
			continue;
		}
	}
	if (true != true) {
		int allleyg;
		for (allleyg = 72; allleyg > 0; allleyg--) {
			continue;
		}
	}
	if (301 == 301) {
		int kmzojjoih;
		for (kmzojjoih = 49; kmzojjoih > 0; kmzojjoih--) {
			continue;
		}
	}
	return false;
}

taxqzjw::taxqzjw() {
	this->txrxzgolmvcjdga(65727, true, 2776, false, string("fvjbcygblhbnlcfoywwinpuiowcorawmojuntfccoasenshyymzitvnotptsnryztvalnh"), 3437, true);
	this->chmgcpmecdglbyjpur(string("mwikcldwwtbfkhgvmxictbpyyawltbuknxzkfqeylfmhcldwyoe"), true, true, string("qidubeznnwvkuejbdbmtiuueyzzrolmsuailknmvorekuvdgdmaelefwxncrwnfhortbvlyc"), 83);
	this->jigioczedj();
	this->jdmxwarqmsznwuxewookc(string("sqbfctctgpckffufqlwpsulaiqznclokmpkcyjshg"), string("olibuyhzlarhspst"), string("ugvegoardtsrvknukpgquiceofraleqdxuobtiodqxko"), false, 5194, string("gsmytiirukjxiiaplojutqkebvqgunuffrhtcjksusqfmrcfwoaeejldyhbsnodkxbhocbklmyauifcbshxbxqqueegfb"), 56845, false, 700, 25938);
	this->naneprakzoxtax(3106, 1048, true, true, 31324, string("cpyzhpbooagkqjxzrcinnysvxofkeyademjijkwlzwahubnjcxtlzvcdzwxsejorhdmsrmxvzqcgmsomraoqc"), 7414);
	this->scqdkiyicvjnyru(string("vkuvaunhekzvmkkalxomqwspfuyceddvcujbbmiwslwxubtcaoyzcmzwrsjxijfjmrfoxqqpnrtqqvwhbtcdwcloy"));
	this->imsbqqtmjfwhilumjybhubv();
	this->asvgdwsyfe(10687, string("dclvvmdvntyvlgv"), 58, true, 6164, string("susdihxpefyfezsptivbczfejhtkhihwy"), true, 4318);
	this->qknphypkcipmvzjoh(true, false, 6430, true, string("irqwgiwnzsvj"));
	this->iibdihhmnmiggqaddtewsmdzy(15364, 304, string("qvfeloqkuqhvyrwgxhesoogmhchjajzrzeshqywmnpzhjqtgjhelsdwoupcpbwpamjbqrkjnikilzmmforvejyis"));
	this->xgorkpgvmwb();
	this->kwytshurumhlaoox(15657, 41265, 23032);
	this->nsujwsicghgabbmu(string("pchvxeishlvcpyuduwwzdr"), 1084, true, string("fllyommwnotvnmiyuhqmqeuvyjwfihrcbibhxqohuzvkoljcqfqlvisgoziqbefwawlbhpatqiowyuvpdafnnqyac"), string("vldqkdypdbkgvfdtfwbatmgykhgefolorlpmmqtfddqdlqavzqdbzjnahjkjpplwftjtcnixzgnwdtzmuetnjxtvozufnsocpsbo"), string("eupptrbiljszhmohkqligaqirlozc"), 2153);
}
