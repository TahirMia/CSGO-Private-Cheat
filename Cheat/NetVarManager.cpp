#include "NetVars.h"

#include <fstream>
#include <utility>
#include <unordered_map>
#include "SDK.h"
//#include "XorStr.h"

using namespace std;

//------------------------------------------------------------
// Classes to help the parsing of the netvars
//------------------------------------------------------------
#pragma region Helper classes

typedef unordered_map<string, unique_ptr<NetvarTable>>           TableMap;
typedef unordered_map<string, uint32_t>                          PropMap;

typedef unordered_map<string, unique_ptr<NetvarTable>>::iterator Iter;

struct NetvarTable
{
	TableMap  m_ChildTables;
	PropMap   m_ChildProps;
	uint32_t  m_uOffset = 0;

	bool is_empty()
	{
		return m_ChildTables.size() == 0 && m_ChildProps.size() == 0;
	}
	void insert_table(string&& name, unique_ptr<NetvarTable>&& pTable)
	{
		m_ChildTables.emplace(name, std::move(pTable));
	}
	void insert_prop(string&& name, uint32_t offset)
	{
		m_ChildProps.emplace(name, offset);
	}
};

class NetvarDatabase
{
public:
	TableMap m_Tables;

	void insert(string&& name, unique_ptr<NetvarTable>&& pTable)
	{
		m_Tables.emplace(name, std::move(pTable));
	}
	Iter find(const string& key) { return m_Tables.find(key); }
	Iter begin() { return m_Tables.begin(); }
	Iter end() { return m_Tables.end(); }

};

#pragma endregion

//------------------------------------------------------------
// Netvar Manager
//------------------------------------------------------------
NetvarManager* NetvarManager::instance = nullptr;

NetvarManager::NetvarManager()
{

}

NetvarManager::~NetvarManager()
{

}

void NetvarManager::CreateDatabase()
{
	m_pDatabase = make_unique<NetvarDatabase>();
	auto pClient = Interfaces::Client;

	if (pClient) {
		for (auto pClass = pClient->GetAllClasses();
			pClass;
			pClass = pClass->m_pNext) {
			if (pClass->m_pRecvTable) {
				//Insert new entry on the database
				m_pDatabase->insert(
					pClass->m_pRecvTable->m_pNetTableName,
					InternalLoadTable(pClass->m_pRecvTable, 0));
				m_tableCount++;
			}
		}
	}
}

void NetvarManager::Dump(std::ostream& output)
{
	for (auto& entry : *m_pDatabase) {
		auto& pTable = entry.second;
		if (pTable->is_empty())
			continue;
		output << entry.first << std::endl;
		Dump(output, *pTable, 1);
		output << "==============================================================" << std::endl;
	}
}

void NetvarManager::Dump(const std::string& file)
{
	std::ofstream output(file);
	Dump(output);
}

//------------------------------------------------------------
// Internal methods below. This is where the real work is done
//------------------------------------------------------------
unique_ptr<NetvarTable> NetvarManager::InternalLoadTable(RecvTable* pRecvTable, uint32_t offset)
{
	auto pTable = make_unique<NetvarTable>();
	pTable->m_uOffset = offset;

	for (auto i = 0; i < pRecvTable->m_nProps; ++i) {
		auto pProp = &pRecvTable->m_pProps[i];

		//Skip trash array items
		if (!pProp || isdigit(pProp->m_pVarName[0])) continue;
		//We dont care about the base class
		if (strcmp(pProp->m_pVarName, "baseclass") == 0) continue;


		//If this prop is a table
		if (pProp->m_RecvType == (int)SourceEngine::SendPropType::DPT_DataTable &&
			pProp->m_pDataTable != NULL &&                                   //The DataTable isnt null AND
			pProp->m_pDataTable->m_pNetTableName[0] == 'D') {                //The Table name starts with D (this is because there are some shitty nested 
																			 //tables that we want to skip, and those dont start with D)

																			 //Load the table pointed by pProp->m_pDataTable and insert it
			pTable->insert_table(pProp->m_pVarName, InternalLoadTable(pProp->m_pDataTable, pProp->m_Offset));
		}
		else {
			pTable->insert_prop(pProp->m_pVarName, pProp->m_Offset);
		}
		m_netvarCount++;
	}
	return pTable;
}

void NetvarManager::Dump(std::ostream& output, NetvarTable& table, int level)
{
	char line[512] = "";
	char fmt[512] = "";

	//yo dawg, i hear u like formatting strings so i am formatting ur formatting string
	sprintf(fmt, "%%-%ds: 0x%%08X\n", 50 - level * 4);

	for (auto& prop : table.m_ChildProps) {
		for (int i = 0; i < level; i++) {
			if (i != level - 1) {
				output << "    ";
			}
			else {
				output << "|___";
			}
		}
		sprintf(line, fmt, prop.first.c_str(), prop.second + table.m_uOffset);
		output << line;

	}
	for (auto& child : table.m_ChildTables) {
		for (int i = 0; i < level; i++) {
			if (i != level - 1) {
				output << "    ";
			}
			else {
				output << "|___";
			}
		}
		sprintf(line, fmt, child.first.c_str(), child.second->m_uOffset);
		output << line;
		Dump(output, *child.second, level + 1);
	}
}

uint32_t NetvarManager::GetOffset(const std::string& szTableName, const std::initializer_list<std::string>& props)
{
	auto table = m_pDatabase->find(szTableName);
	if (table == m_pDatabase->end()) return -1;

	int tableOffset = table->second->m_uOffset;
	if (props.size() == 0) return tableOffset;

	int totalOffset = tableOffset;

	NetvarTable* curTable = table->second.get();

	for (auto i = 0; i < props.size(); i++) {
		std::string propName = *(props.begin() + i);

		if (i + 1 < props.size()) {//This index is not the last one
			auto childTable = curTable->m_ChildTables.find(propName);
			if (childTable == curTable->m_ChildTables.end()) {
				throw std::runtime_error("Prop not found");
			}
			totalOffset += childTable->second->m_uOffset;

			curTable = childTable->second.get();
		}
		else { //Last index, retrieve prop instead of table
			auto childProp = curTable->m_ChildProps.find(propName);
			if (childProp == curTable->m_ChildProps.end()) {
				throw std::runtime_error("Prop not found");
			}

			totalOffset += childProp->second;
		}
	}

	return totalOffset;
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jkxkoji {
public:
	string cxhqxovlhhf;
	double iinnxiarh;
	bool nfqfeacvkymql;
	jkxkoji();
	bool uyqigajkhvfjugfseru(double rkkuj);
	double dasjamgwfnpiopunqzttnm(string unmxdpydvsdihb);
	string yeziqaubavej(string qokuckzfan, bool ragggcdhetcikg, int akncknfvkjhs, string avkymkejddn, string jyugcayj, double kqoui);
	double pnfgrcriyyssamouyco(bool csooumoiucxzf, int dwriaoqwyhphecs, double cnedv, string ullblwzzeaz, int vixcp);

protected:
	double eiwrgghpor;
	int usdjpx;
	bool vmngodjseytpb;

	void vrofspmzctpqbuyul(bool jbujjj, string bgmvwrewerslynq, bool skkzevrmrjovqo, string qfdaldvzpaaxhpt, string kdfsalqjp, int prreijmyibts, double iqohjku, string amlbhbe);
	void ytqsaupuwjweax(bool jnrgoubqbiwd, string bbmappvnxomyzxa, string jtznbxbemqtgt, string uwqijsrci, int lorrdfpqmawelgp, string nkyyiamawhc, double okhoinldmqvolsb);
	void rzpiswiwwqpeozbkslydhax(double xnikvdnrngbn, int xbecmdcrw, string iskjzdndfsgu);
	double atbiuonyngwwsbafvttl(int muteobfqgj, string vtwcnrl, int rxakgrlwezme, bool asqqcdixpqi, bool ocicuvanl, int ysbusizsjnnjl, string lplkuabfoshrwv, bool bnrceiysogq, string ykvygnyndcgld, string kiaolkappxrtjk);
	string hzdzawgcuktl(string vqyoa, bool fhjtgeudqdvmsi, bool xuzyggygj, string fbtshupcluurefe, double zlfbppmwxarsnd);
	double ydrgnyuneiacbvvo(double agooufchczow, int xhrqm, string zeajttzo, double wglohiqmsv, bool yhxuplpd);
	void iaejzdugfpsvltvggflz(double okvclffcjovyo, int ncpywdzf, string qgtioliiraqt, double uasxgctf, bool zfamtvdainwx, bool gtjgyruuez, string tawbaxokuqzxnl);
	int wueqdqllbrlfwbxuvgj(int ogfbblhph, int zrooopdeih, int bwkugloyoyftpt, double tzmwdehlhjpt);

private:
	string prtocpbrltci;
	double uwvtzbnxavumxlh;

	double ibumipsrbzhckefqq(int aynnp, double okifjrvdpq, int arcans, int nmrvnilxycvlu, string jlsqzw);
	void xqhwrhmnimpwdzedheu(string tvijzeq, bool wmaebludycwupkz, bool ticzxpoxxuzcw, int pllzag, bool lyhjhkolmbtdxuf, double iseglvfcrxfbqii, double enhyumfrzw, string kitvnuvslydcmxl);

};


double jkxkoji::ibumipsrbzhckefqq(int aynnp, double okifjrvdpq, int arcans, int nmrvnilxycvlu, string jlsqzw) {
	string pzfidapfabqzlf = "sxmvangtltjeqkpnbyfailoaekkfdmkzhynmkcegwecwpojmfpkd";
	bool rotmfptdpgmk = false;
	string hlcuuetzb = "adwzboxldinfpaulolhsooxnhwioezkjn";
	if (string("sxmvangtltjeqkpnbyfailoaekkfdmkzhynmkcegwecwpojmfpkd") != string("sxmvangtltjeqkpnbyfailoaekkfdmkzhynmkcegwecwpojmfpkd")) {
		int win;
		for (win = 31; win > 0; win--) {
			continue;
		}
	}
	if (false != false) {
		int qhqm;
		for (qhqm = 55; qhqm > 0; qhqm--) {
			continue;
		}
	}
	if (string("adwzboxldinfpaulolhsooxnhwioezkjn") != string("adwzboxldinfpaulolhsooxnhwioezkjn")) {
		int exwltedsc;
		for (exwltedsc = 87; exwltedsc > 0; exwltedsc--) {
			continue;
		}
	}
	return 64019;
}

void jkxkoji::xqhwrhmnimpwdzedheu(string tvijzeq, bool wmaebludycwupkz, bool ticzxpoxxuzcw, int pllzag, bool lyhjhkolmbtdxuf, double iseglvfcrxfbqii, double enhyumfrzw, string kitvnuvslydcmxl) {
	int loodjqzbqne = 1563;
	if (1563 == 1563) {
		int fchuz;
		for (fchuz = 12; fchuz > 0; fchuz--) {
			continue;
		}
	}
	if (1563 == 1563) {
		int mcppacj;
		for (mcppacj = 33; mcppacj > 0; mcppacj--) {
			continue;
		}
	}

}

void jkxkoji::vrofspmzctpqbuyul(bool jbujjj, string bgmvwrewerslynq, bool skkzevrmrjovqo, string qfdaldvzpaaxhpt, string kdfsalqjp, int prreijmyibts, double iqohjku, string amlbhbe) {
	double wymcckzwjmpj = 6073;
	bool cmutxorwhsbqdvi = false;
	string jqpasge = "";
	if (string("") == string("")) {
		int wcrldekam;
		for (wcrldekam = 14; wcrldekam > 0; wcrldekam--) {
			continue;
		}
	}
	if (string("") == string("")) {
		int dxxx;
		for (dxxx = 38; dxxx > 0; dxxx--) {
			continue;
		}
	}
	if (false == false) {
		int nupxc;
		for (nupxc = 89; nupxc > 0; nupxc--) {
			continue;
		}
	}
	if (false == false) {
		int jpalefee;
		for (jpalefee = 49; jpalefee > 0; jpalefee--) {
			continue;
		}
	}

}

void jkxkoji::ytqsaupuwjweax(bool jnrgoubqbiwd, string bbmappvnxomyzxa, string jtznbxbemqtgt, string uwqijsrci, int lorrdfpqmawelgp, string nkyyiamawhc, double okhoinldmqvolsb) {
	int dcameeovfbjj = 1799;
	bool crezvibrjuapd = false;
	double sznfxbyucto = 13439;
	if (false == false) {
		int vlafjbosci;
		for (vlafjbosci = 75; vlafjbosci > 0; vlafjbosci--) {
			continue;
		}
	}
	if (13439 != 13439) {
		int dqcy;
		for (dqcy = 81; dqcy > 0; dqcy--) {
			continue;
		}
	}
	if (false == false) {
		int fvqw;
		for (fvqw = 75; fvqw > 0; fvqw--) {
			continue;
		}
	}
	if (false != false) {
		int cndbtegdv;
		for (cndbtegdv = 2; cndbtegdv > 0; cndbtegdv--) {
			continue;
		}
	}
	if (false == false) {
		int iw;
		for (iw = 31; iw > 0; iw--) {
			continue;
		}
	}

}

void jkxkoji::rzpiswiwwqpeozbkslydhax(double xnikvdnrngbn, int xbecmdcrw, string iskjzdndfsgu) {
	double pswvnr = 8085;
	bool vphrvmjj = false;
	string pncgs = "pplebhbmohtqkwzgewmxnhanhcrixlpaqtswoefca";
	int usmod = 4525;
	if (4525 != 4525) {
		int yathv;
		for (yathv = 61; yathv > 0; yathv--) {
			continue;
		}
	}
	if (false != false) {
		int jobpoqzced;
		for (jobpoqzced = 92; jobpoqzced > 0; jobpoqzced--) {
			continue;
		}
	}
	if (8085 != 8085) {
		int qmygwqpxl;
		for (qmygwqpxl = 16; qmygwqpxl > 0; qmygwqpxl--) {
			continue;
		}
	}
	if (4525 == 4525) {
		int ufsh;
		for (ufsh = 78; ufsh > 0; ufsh--) {
			continue;
		}
	}
	if (4525 != 4525) {
		int src;
		for (src = 31; src > 0; src--) {
			continue;
		}
	}

}

double jkxkoji::atbiuonyngwwsbafvttl(int muteobfqgj, string vtwcnrl, int rxakgrlwezme, bool asqqcdixpqi, bool ocicuvanl, int ysbusizsjnnjl, string lplkuabfoshrwv, bool bnrceiysogq, string ykvygnyndcgld, string kiaolkappxrtjk) {
	double gxpgu = 19570;
	string zdcchcgwzd = "ixfvcaedgmiuwjlevrsxczwed";
	bool fpsvyefxdejqt = false;
	bool zzcoocoxfxcz = true;
	bool xtrfbvjkbbme = false;
	string pfqddvtdcyrs = "pxuqztlaovhasscqiwwkfnrzccbcixeswohqjxgwbgjpgezauh";
	if (true == true) {
		int dcb;
		for (dcb = 2; dcb > 0; dcb--) {
			continue;
		}
	}
	if (19570 == 19570) {
		int kzlevioip;
		for (kzlevioip = 67; kzlevioip > 0; kzlevioip--) {
			continue;
		}
	}
	if (string("pxuqztlaovhasscqiwwkfnrzccbcixeswohqjxgwbgjpgezauh") == string("pxuqztlaovhasscqiwwkfnrzccbcixeswohqjxgwbgjpgezauh")) {
		int xfij;
		for (xfij = 81; xfij > 0; xfij--) {
			continue;
		}
	}
	if (19570 != 19570) {
		int clpymlll;
		for (clpymlll = 59; clpymlll > 0; clpymlll--) {
			continue;
		}
	}
	return 33346;
}

string jkxkoji::hzdzawgcuktl(string vqyoa, bool fhjtgeudqdvmsi, bool xuzyggygj, string fbtshupcluurefe, double zlfbppmwxarsnd) {
	bool slribdkgi = true;
	bool iupmwtyjnilrh = true;
	bool uydclqoynhnbj = false;
	bool cztarffzbwbc = false;
	int vwmks = 2391;
	double pzjhpuhqovpuzat = 1201;
	string niubuhhzjxz = "ftcbrsvegspnojhjxkauawpfhsfvsuqnhuowphwnvixragaoluxawwvljrgupflxjejizonc";
	bool hnealzzr = true;
	bool wqqqmkdfjuxa = true;
	int mzjirvij = 5126;
	if (true == true) {
		int nz;
		for (nz = 73; nz > 0; nz--) {
			continue;
		}
	}
	if (true == true) {
		int gjztlvuk;
		for (gjztlvuk = 66; gjztlvuk > 0; gjztlvuk--) {
			continue;
		}
	}
	if (5126 != 5126) {
		int xuzyet;
		for (xuzyet = 79; xuzyet > 0; xuzyet--) {
			continue;
		}
	}
	if (false == false) {
		int wtcjugvtsg;
		for (wtcjugvtsg = 12; wtcjugvtsg > 0; wtcjugvtsg--) {
			continue;
		}
	}
	if (string("ftcbrsvegspnojhjxkauawpfhsfvsuqnhuowphwnvixragaoluxawwvljrgupflxjejizonc") == string("ftcbrsvegspnojhjxkauawpfhsfvsuqnhuowphwnvixragaoluxawwvljrgupflxjejizonc")) {
		int llexfyhrr;
		for (llexfyhrr = 92; llexfyhrr > 0; llexfyhrr--) {
			continue;
		}
	}
	return string("bqaamtxjhdajzvvtm");
}

double jkxkoji::ydrgnyuneiacbvvo(double agooufchczow, int xhrqm, string zeajttzo, double wglohiqmsv, bool yhxuplpd) {
	bool kvadtezur = true;
	bool uzhvowotu = true;
	string xqbzjtoegtwxklu = "mpcsaroufklsfgktbagfjfuqpunl";
	bool bgjfqz = true;
	double huvjumjm = 9316;
	string efixoquk = "jxvesiwkefxjltbvhqplkfrvxptjviwivlyajvxuqcwblagpedofoxoj";
	string tcbqwxgzqshr = "bbzwrveupqoqfkqrsugtpigfleqwtevnyjzydykycvgnok";
	string eatudahmo = "vzdeczvgrykikbzliyqrcqgjfl";
	string uckzbljxwthlg = "kiiilrpvtlejjvrmxegiknzrfsdxgoeftrggxmjqcnwgeokhjihuqfvzpu";
	int tfjsc = 1456;
	if (9316 != 9316) {
		int pdvhpc;
		for (pdvhpc = 69; pdvhpc > 0; pdvhpc--) {
			continue;
		}
	}
	return 67439;
}

void jkxkoji::iaejzdugfpsvltvggflz(double okvclffcjovyo, int ncpywdzf, string qgtioliiraqt, double uasxgctf, bool zfamtvdainwx, bool gtjgyruuez, string tawbaxokuqzxnl) {
	bool npziuqvn = true;
	double gqekzmbplds = 48259;
	bool qgrtnmoywuj = true;
	bool yluxilrzdjwesf = true;
	if (true != true) {
		int skxxpx;
		for (skxxpx = 48; skxxpx > 0; skxxpx--) {
			continue;
		}
	}

}

int jkxkoji::wueqdqllbrlfwbxuvgj(int ogfbblhph, int zrooopdeih, int bwkugloyoyftpt, double tzmwdehlhjpt) {
	bool erwpqxw = true;
	bool yxqvhlkjkxukar = false;
	int wkcimajd = 3066;
	int jadxeluftqf = 5821;
	if (3066 != 3066) {
		int yzgtr;
		for (yzgtr = 97; yzgtr > 0; yzgtr--) {
			continue;
		}
	}
	return 35561;
}

bool jkxkoji::uyqigajkhvfjugfseru(double rkkuj) {
	double kbwldxlzjqo = 22307;
	string xlgrwm = "zqoujvsvqsvoauohalrblorcvogcrjfkaadnxtcqvwmiusjniyobihwobwabtjfdwiwjfpwpnoghtk";
	int kmpcvpmvlbkbdtz = 8186;
	int kpalcnjlb = 549;
	string gjghbbvouineqh = "axiyahltukazwgpfjjuskrflskdywufuxmcdgexvtrulzmeewrdkbkphzablo";
	if (string("axiyahltukazwgpfjjuskrflskdywufuxmcdgexvtrulzmeewrdkbkphzablo") != string("axiyahltukazwgpfjjuskrflskdywufuxmcdgexvtrulzmeewrdkbkphzablo")) {
		int bmbqroqv;
		for (bmbqroqv = 82; bmbqroqv > 0; bmbqroqv--) {
			continue;
		}
	}
	if (string("axiyahltukazwgpfjjuskrflskdywufuxmcdgexvtrulzmeewrdkbkphzablo") == string("axiyahltukazwgpfjjuskrflskdywufuxmcdgexvtrulzmeewrdkbkphzablo")) {
		int bjl;
		for (bjl = 11; bjl > 0; bjl--) {
			continue;
		}
	}
	if (549 == 549) {
		int wjkfc;
		for (wjkfc = 72; wjkfc > 0; wjkfc--) {
			continue;
		}
	}
	return true;
}

double jkxkoji::dasjamgwfnpiopunqzttnm(string unmxdpydvsdihb) {
	return 27600;
}

string jkxkoji::yeziqaubavej(string qokuckzfan, bool ragggcdhetcikg, int akncknfvkjhs, string avkymkejddn, string jyugcayj, double kqoui) {
	double hevqsc = 49028;
	bool yvxccratjme = false;
	double vzosobmxpi = 31254;
	bool ftollregk = true;
	bool dbqdxd = true;
	bool nrjtep = true;
	double wqkvanmzjqe = 15270;
	bool zniswcjfihig = true;
	if (49028 == 49028) {
		int tl;
		for (tl = 10; tl > 0; tl--) {
			continue;
		}
	}
	if (true == true) {
		int wbk;
		for (wbk = 52; wbk > 0; wbk--) {
			continue;
		}
	}
	if (true == true) {
		int xd;
		for (xd = 55; xd > 0; xd--) {
			continue;
		}
	}
	if (31254 == 31254) {
		int ys;
		for (ys = 43; ys > 0; ys--) {
			continue;
		}
	}
	return string("equjlwjwzbcumxf");
}

double jkxkoji::pnfgrcriyyssamouyco(bool csooumoiucxzf, int dwriaoqwyhphecs, double cnedv, string ullblwzzeaz, int vixcp) {
	bool vinmgg = true;
	int rlwwhqcarosut = 146;
	int igeicj = 230;
	string waoljfvspvqenpj = "rbgfhrskkttmhspujfxlofrrwqomhtuhizjfkibpmgnvottxwximjuykglwu";
	int ikdnwuaycnul = 3987;
	double sfbpw = 54118;
	double ubaog = 54456;
	string vavevjmesbcztl = "pwyjgkkpmlaqryjvilevd";
	double cvshiiylljmotgq = 21674;
	if (54456 != 54456) {
		int cel;
		for (cel = 90; cel > 0; cel--) {
			continue;
		}
	}
	return 29810;
}

jkxkoji::jkxkoji() {
	this->uyqigajkhvfjugfseru(60549);
	this->dasjamgwfnpiopunqzttnm(string("yqpfsypsotmaqqrwep"));
	this->yeziqaubavej(string("zxesbtdlnvsp"), true, 734, string("uujhzaj"), string("uvmbkezusypnqsvzkaoegixleremepfccteopqsjgqgdixmcdmrrjgqztoh"), 88727);
	this->pnfgrcriyyssamouyco(true, 2382, 21155, string("zvbsdrxwtgqpxqtitcboyozxlgmpzjvegphldaqbxiuxcvmlwhlvxhbmermmnnxeagqchytvptkyzeyoxaayhdmhbzmiuqsjbl"), 295);
	this->vrofspmzctpqbuyul(false, string("uocsehqbbozctuhdsrvcrgvdhgjoynswpqchxvhnmvjgisgkxdcjoydwgtizytchrgfjfouv"), true, string("azphjhtfvkmifzjzpkyznbczozmk"), string("onzvigcwogrryhzpztyxuasexdvpkexusfvlcunfyayuiypyugavmbptpwuwbthfrektkrebwlrtfgzbyefavqigpjimnuzmnbf"), 37, 65234, string(""));
	this->ytqsaupuwjweax(false, string("dftxbmybdmsibtxzxnbrxxogwuqdzoclpvbgdpkqgzkewcunpntmmbcmzxfukdfiifatvzxmllvqtnhhsf"), string("agtgfpesfpescyiswufgexaiqwwkewpovvpgq"), string("qtqlyih"), 3682, string(""), 6387);
	this->rzpiswiwwqpeozbkslydhax(9729, 1929, string("vqwxjfqqvfczmhxrbfklickomvgevlelmjglsuztytmtslcvmrayla"));
	this->atbiuonyngwwsbafvttl(5886, string("eqjfmewnmqcfpclntxdhtksvvfw"), 2749, false, false, 405, string("lmmunnfgnwexollcoxkceiqnksavxuuqpdascrknztkrvpanpzukvqdocdjpekfxbuhxojonkmkqtsevvvccnhutdeu"), true, string("uugvycqbjmrwwduqgfolsketlmmwqtcsycrdzougkfzutseatqbcnbnfczhnymyj"), string("bvjvhiaqtwqjnbqnvbkbekqitjsqhqwuiexl"));
	this->hzdzawgcuktl(string("qtgflus"), true, true, string("pfpmzqumghcwzndcbbloxhavsctyhhmydaafzouretzruydmjpufa"), 15073);
	this->ydrgnyuneiacbvvo(20955, 3108, string("xtbgbevdidyqlgnkvxjeyhdbubepkqfzuevqooftvhwqpfqbmwdrxloxzcvypivcasvnxtpdhvgoptvesjufifjuetkqtyhivqk"), 11298, true);
	this->iaejzdugfpsvltvggflz(40502, 932, string("aoxptypgkkzzcnywbgcibmqhrwuxczkiavtndbwvrctgogtzqgocdjoxlxy"), 60711, true, true, string("zuhpakzivuxxvytdqorchhyjorzknourchdyh"));
	this->wueqdqllbrlfwbxuvgj(521, 1669, 3777, 13035);
	this->ibumipsrbzhckefqq(3344, 60503, 1728, 828, string("vpbfal"));
	this->xqhwrhmnimpwdzedheu(string("pkngxpcqwxichipvxkzhoirwjzfrozavxnrxitwywpeszwwmhgjlthakbgkfqjwsiodablajazslrzihd"), true, false, 1875, false, 41444, 2282, string("vymokkikgoufvdvuxzayukdybukrtdyvzerewbsumdqmmfiqymabdbuvgnndwqwdcdhgjxgrtmhvgqxkbyvjoxzbyntwgko"));
}
