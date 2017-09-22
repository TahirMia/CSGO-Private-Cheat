/*
Syn's AyyWare Framework 2015
*/

// Credits to Valve and Shad0w
#include "Interfaces.h"
#include "Menu.h"

// Shad0ws Yaw fix
// (FIX ME UP LATER)
void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;
	static bool last[128];
	bool tmp = last[((IClientEntity*)(pStruct))->GetIndex()];
	float yaw = pData->m_Value.m_Float;
	yaw -= 0.087929;

	switch (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 5)
	{
	case 0:
		// No resolver needed
		break;
	case 1:
		// Normal Fix
		yaw -= 0.087929;

		last[((IClientEntity*)(pStruct))->GetIndex()] = (yaw >= 180 && yaw <= 360);

		if (tmp && (yaw >= 0 && yaw <= 180))
			yaw += 359;

		yaw -= (int)(yaw / 360) * 360;
		if (yaw < -180)
			yaw += 360;
		else if (yaw > 180)
			yaw -= 360;

		break;
	case 2:
		// Anglefix
		bHasAA = ((*flPitch == 90.0f) || (*flPitch == 270.0f));
		bSpinbot = false;

		if (!bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
			&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f) && !bHasAA)
		{
			flYaw = vLast[((IClientEntity*)(pStruct))->GetIndex()].y;
			bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
		}
		else
		{
			if (bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
				&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f))
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
				bSpinbot = true;
			}
			else
			{
				bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = false;
			}
		}

		vLast[((IClientEntity*)(pStruct))->GetIndex()].y = flYaw;


		bool bTmp = bJitterFix[((IClientEntity*)(pStruct))->GetIndex()];

		bJitterFix[((IClientEntity*)(pStruct))->GetIndex()] = (flYaw >= 180.0f && flYaw <= 360.0f);

		if (bTmp && (flYaw >= 0.0f && flYaw <= 180.0f))
		{
			flYaw += 359.0f;
		}
		break;
	}

	*(float*)(pOut) = flYaw;
}

// Simple fix for some Fake-Downs
void FixX(const CRecvProxyData* pData, void* pStruct, void* pOut) // Clamp other player angles to fix fakedown or lisp
{
	float* ang = (float*)pOut;
	*ang = pData->m_Value.m_Float;
	DWORD hex = *(DWORD*)(&ang);

	switch (Menu::Window.RageBotTab.AccuracyResolver.GetIndex() == 5)
	{
	case 0:
		// No resolver needed
		break;
	case 1:
		// Regular Pitch Resolver
		if (hex >= 0x43330000)
		{
			*ang -= 360.f;
		}
		else if (hex <= -0x43330000)
		{
			*ang += 360.f;
		}

		if (hex == 0x42b40000)
		{
			*ang = 90.f;
		}
		else if (hex == -0x42b40000)
		{
			*ang = -90.f;
		}

		*(float*)(pOut) = *ang;
		break;
	case 2:
		if (pData->m_Value.m_Float > 180.0f)
			*ang -= 360.0f;
		else if (pData->m_Value.m_Float < -180.0f)
			*ang += 360.0f; 
		break;
	}
}

RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	static int default_t = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	static int default_ct = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	static int bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	static int karam = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");

	// Get local player (just to stop replacing spectators knifes)
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Menu::Window.MiscTab.KnifeEnable.GetState() && pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (pData->m_Value.m_Int == default_t || pData->m_Value.m_Int == default_ct))
		{
			// Set whatever knife we want
			if (Menu::Window.MiscTab.KnifeModel.GetIndex() == 0)
				pData->m_Value.m_Int = karam;
			else if (Menu::Window.MiscTab.KnifeModel.GetIndex() == 1)
				pData->m_Value.m_Int = bayonet;
		}
	}

	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}

void ApplyAAAHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Pitch Fix
				if (!strcmp(name, "m_angEyeAngles[0]"))
				{
					pProp->m_ProxyFn = FixX;
				}

				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[1]"))
				{
					Utilities::Log("Yaw Fix Applied");
					pProp->m_ProxyFn = FixY;
				}
			}
		}
		else if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class wkqsphu {
public:
	double rpavpqvuphyk;
	bool lolrwfhwkgz;
	wkqsphu();
	string llpxvdmudnau(int ldeorl, double luszfzgzufyzl, double whxumcrekd, string vjqwje, int mbssrrc);

protected:
	bool eatpimuuv;
	string ojktqtassxoaee;

	void ftpgdpttrhuzoif(double qiuahrzxtkql, bool zgrydymq, bool jgdwwyehbzzok, double ggmcgbkzf, bool zmoadaiquzy, int ehhqutenjrvn, string akslolsn);
	int vgvwvsldenwae(bool wdsuar, double iurawcijrjqoobx, string krmvamb, double ptrgwxusleaxtty, double dgnvozr, double yypkqycztbqv, string saujrc, double vhqxp, string mwbvhccv);
	double pamavsgfdpz(bool idbofv, string ruigwwyvbbnknp, bool losxeewsd, int jtdtcg, bool iegnotfnbvgjkzz, double stobalwcnf, string dilgkhvodzt);
	int adqarrqgqkehqenrj(double mlfygqij, string jszhscunkcf, bool yzkgoydhaxujx, double oykivetawejv);
	double itigdsgoosqkznzruakvjdxz(double rfbbzef, double fnqvp, bool yzkfiokcgyqeb, string wolhtpbpxdtk, int cnqyj, int soxvyhyxycor);
	bool ptzoeqkutqlnjrcgwsnzda(string wemupuci, bool rbpcpbbphf, double xywypsdaszkmq, string delcsxbnhnbwlm, double egtwelsyhzz, bool mdbwoozrrhsiafw, int cerdjvims, bool uvkuhup);

private:
	double qmkitt;

	double grsnmijulyhigehsgcj(double spyalynxqc, string keieagls);
	double csnjkxcwcr(int seuzvpresirqnpj, int cthqqrdgk, string eqafvzpn, double bzfzwrijguia, bool ysaexdnxklstq, int ywtcaexayqmqgvk, bool sdwehaymv, int btlnwmvjko, string rydxtzsgtq, int jsltrkflvu);
	string ydajroefshlhnzqdqnfbrjugt(string kqtfqeujrvo, string cjdvrwlfwmfryx, string afuida, double qipabyqbarp, int nhkruezhybqim, string edqjiwsctu, double estoef, double tyxvplpnxdktbg);
	void gvgircgrsgjradgieltkt(string odxbpfaosprnoaz, double vzbiallrhyvipi, string lbcxhjmhpbot, int uhsbiedq, int iifouvgxrbgyf, double yrzqetqnvsll, double kltgvjlqubirqi, string twqzeesw, double onrxtetof);
	int rzhilkgetjkgu(bool exxhvjhgxhrt, bool cviapdnrkjgok);
	void gcbzmmcbmmqntkvozbxbehvw(int jwzherfhy, int ljoxtczj, bool pkpkousd, int swhwulcammyh);
	void iekpofohpboc(string dcgaele, string ivpmiswybse, double mcukbxrgkje, int euecndqt, bool hrwsp);

};


double wkqsphu::grsnmijulyhigehsgcj(double spyalynxqc, string keieagls) {
	bool mlslcpp = false;
	int ecraybkupksekp = 5779;
	return 97662;
}

double wkqsphu::csnjkxcwcr(int seuzvpresirqnpj, int cthqqrdgk, string eqafvzpn, double bzfzwrijguia, bool ysaexdnxklstq, int ywtcaexayqmqgvk, bool sdwehaymv, int btlnwmvjko, string rydxtzsgtq, int jsltrkflvu) {
	return 71918;
}

string wkqsphu::ydajroefshlhnzqdqnfbrjugt(string kqtfqeujrvo, string cjdvrwlfwmfryx, string afuida, double qipabyqbarp, int nhkruezhybqim, string edqjiwsctu, double estoef, double tyxvplpnxdktbg) {
	string jxxaqqaqcwst = "wrvdaecpyehupwzurdkfdnqsyfimpvbyhthvbmeluhozsvfuscnqpzlkybtfhl";
	double yeporvgmiebwqn = 14240;
	bool uwfvrcdt = true;
	double zwuzeqhemgav = 4221;
	if (4221 != 4221) {
		int dxnaff;
		for (dxnaff = 87; dxnaff > 0; dxnaff--) {
			continue;
		}
	}
	if (string("wrvdaecpyehupwzurdkfdnqsyfimpvbyhthvbmeluhozsvfuscnqpzlkybtfhl") == string("wrvdaecpyehupwzurdkfdnqsyfimpvbyhthvbmeluhozsvfuscnqpzlkybtfhl")) {
		int mrvisglrp;
		for (mrvisglrp = 85; mrvisglrp > 0; mrvisglrp--) {
			continue;
		}
	}
	if (true == true) {
		int dj;
		for (dj = 56; dj > 0; dj--) {
			continue;
		}
	}
	return string("d");
}

void wkqsphu::gvgircgrsgjradgieltkt(string odxbpfaosprnoaz, double vzbiallrhyvipi, string lbcxhjmhpbot, int uhsbiedq, int iifouvgxrbgyf, double yrzqetqnvsll, double kltgvjlqubirqi, string twqzeesw, double onrxtetof) {
	double vwgscv = 44049;
	int dplqwunm = 1493;
	double hlrohsxcpianedl = 24949;
	bool rfbmwe = false;
	string ctmosdqwnwfk = "fftqrjutrickyvknovnydqejbpaqysapcxlrpcxhdkucjeacgdcqlucmbdzbcyviaqykkqgrcie";
	string neophsv = "mzninbvsghrwutppbjgiojskhfoqitcpzkbbtctfbhuxjijwzlndmsfxcvjrvzekkpisqqqasrdzcsrkjlchpaqrbwkbeo";
	bool tzldo = false;
	int xsqqcjwjbcy = 401;
	bool bbxidzrfujrk = false;
	if (string("fftqrjutrickyvknovnydqejbpaqysapcxlrpcxhdkucjeacgdcqlucmbdzbcyviaqykkqgrcie") == string("fftqrjutrickyvknovnydqejbpaqysapcxlrpcxhdkucjeacgdcqlucmbdzbcyviaqykkqgrcie")) {
		int wtdkofl;
		for (wtdkofl = 15; wtdkofl > 0; wtdkofl--) {
			continue;
		}
	}
	if (24949 == 24949) {
		int pxy;
		for (pxy = 41; pxy > 0; pxy--) {
			continue;
		}
	}
	if (false != false) {
		int rmav;
		for (rmav = 91; rmav > 0; rmav--) {
			continue;
		}
	}
	if (string("mzninbvsghrwutppbjgiojskhfoqitcpzkbbtctfbhuxjijwzlndmsfxcvjrvzekkpisqqqasrdzcsrkjlchpaqrbwkbeo") != string("mzninbvsghrwutppbjgiojskhfoqitcpzkbbtctfbhuxjijwzlndmsfxcvjrvzekkpisqqqasrdzcsrkjlchpaqrbwkbeo")) {
		int vp;
		for (vp = 9; vp > 0; vp--) {
			continue;
		}
	}

}

int wkqsphu::rzhilkgetjkgu(bool exxhvjhgxhrt, bool cviapdnrkjgok) {
	string hycifiyoiaipv = "zzzaodksubnqefuutudzfcqrufhxfjeftzliwtnesujyhza";
	bool njdhij = false;
	int fftfhw = 4665;
	bool abonzdjnf = true;
	double gwmxlsxt = 77673;
	if (string("zzzaodksubnqefuutudzfcqrufhxfjeftzliwtnesujyhza") != string("zzzaodksubnqefuutudzfcqrufhxfjeftzliwtnesujyhza")) {
		int dhhvtzqey;
		for (dhhvtzqey = 1; dhhvtzqey > 0; dhhvtzqey--) {
			continue;
		}
	}
	if (true != true) {
		int aqwa;
		for (aqwa = 22; aqwa > 0; aqwa--) {
			continue;
		}
	}
	if (false == false) {
		int hkxj;
		for (hkxj = 31; hkxj > 0; hkxj--) {
			continue;
		}
	}
	return 15943;
}

void wkqsphu::gcbzmmcbmmqntkvozbxbehvw(int jwzherfhy, int ljoxtczj, bool pkpkousd, int swhwulcammyh) {
	int bzcpwjb = 5453;
	int prcflfjxrqqtvcc = 824;
	bool tnqrzb = true;
	string gietuaoorpdujsd = "jvqjhhndmrntmevjarrhjskqoeeeopipiqewztddcmastareyjsirrcumifcwdrqdaxqccslegrc";
	double sgvfyiibrpbn = 1672;
	double lxfxvhhqxmh = 36667;
	string vswdcfoi = "wjjtconxngklmyyovihcntvoqngrqzgwumsczzttdxlxfoovvo";
	string uzayowmjbdtywyp = "cpkzaksnieuvjvwfxljrspamgqpt";
	if (5453 != 5453) {
		int fkipvfrjs;
		for (fkipvfrjs = 9; fkipvfrjs > 0; fkipvfrjs--) {
			continue;
		}
	}

}

void wkqsphu::iekpofohpboc(string dcgaele, string ivpmiswybse, double mcukbxrgkje, int euecndqt, bool hrwsp) {
	bool biuji = false;
	int isphdbnauuqbpey = 7314;
	double cenzshr = 26293;
	double ejqnmsnaeboq = 8392;
	int uzbelflibtux = 20;
	string usvwjle = "sxzubjhbnofiluofgmrcvxitskpzupxphihbpsnpdg";
	string agucs = "bbmonsvlcxtdglotbagyqkyvnmeeyitblmjkuoyudkfpdkfluvxzy";
	int prsfpj = 5257;
	bool vawduihhunblkzy = true;
	if (string("bbmonsvlcxtdglotbagyqkyvnmeeyitblmjkuoyudkfpdkfluvxzy") == string("bbmonsvlcxtdglotbagyqkyvnmeeyitblmjkuoyudkfpdkfluvxzy")) {
		int zycojkxo;
		for (zycojkxo = 41; zycojkxo > 0; zycojkxo--) {
			continue;
		}
	}
	if (string("bbmonsvlcxtdglotbagyqkyvnmeeyitblmjkuoyudkfpdkfluvxzy") == string("bbmonsvlcxtdglotbagyqkyvnmeeyitblmjkuoyudkfpdkfluvxzy")) {
		int rphfrcci;
		for (rphfrcci = 74; rphfrcci > 0; rphfrcci--) {
			continue;
		}
	}
	if (20 != 20) {
		int vfofhnn;
		for (vfofhnn = 17; vfofhnn > 0; vfofhnn--) {
			continue;
		}
	}

}

void wkqsphu::ftpgdpttrhuzoif(double qiuahrzxtkql, bool zgrydymq, bool jgdwwyehbzzok, double ggmcgbkzf, bool zmoadaiquzy, int ehhqutenjrvn, string akslolsn) {
	string nmrizypv = "kwlloaufftpvlrkfybfwdzitxwprdfjieiflcfvpmvnory";
	double mohlwubptcd = 14586;
	bool oppadcvcfs = true;
	double prpwvwp = 9982;
	double psshhltcerlc = 35625;
	string riyotvofsxspv = "rfjxyutmazuaplqxcrjtvkerkjpjelrvzykowffflgaulhmcmjgtmqenppen";
	if (string("kwlloaufftpvlrkfybfwdzitxwprdfjieiflcfvpmvnory") == string("kwlloaufftpvlrkfybfwdzitxwprdfjieiflcfvpmvnory")) {
		int cqubadhjw;
		for (cqubadhjw = 51; cqubadhjw > 0; cqubadhjw--) {
			continue;
		}
	}
	if (9982 != 9982) {
		int rxjdvtvtf;
		for (rxjdvtvtf = 52; rxjdvtvtf > 0; rxjdvtvtf--) {
			continue;
		}
	}
	if (string("rfjxyutmazuaplqxcrjtvkerkjpjelrvzykowffflgaulhmcmjgtmqenppen") == string("rfjxyutmazuaplqxcrjtvkerkjpjelrvzykowffflgaulhmcmjgtmqenppen")) {
		int srexix;
		for (srexix = 38; srexix > 0; srexix--) {
			continue;
		}
	}

}

int wkqsphu::vgvwvsldenwae(bool wdsuar, double iurawcijrjqoobx, string krmvamb, double ptrgwxusleaxtty, double dgnvozr, double yypkqycztbqv, string saujrc, double vhqxp, string mwbvhccv) {
	return 90959;
}

double wkqsphu::pamavsgfdpz(bool idbofv, string ruigwwyvbbnknp, bool losxeewsd, int jtdtcg, bool iegnotfnbvgjkzz, double stobalwcnf, string dilgkhvodzt) {
	bool midsdfoprc = false;
	double hljsoppzxdo = 10337;
	int moukwyvifsuw = 660;
	int mfmvcz = 1933;
	bool edarn = false;
	string bxtodrjnnyqt = "osgxtwzwfgimsxdpfubyupvpmnelpymrdxpknzxbjzmsibhryraaofpywrtgdcvnclfhuzfnnmgptpzgrrnbytznxn";
	string dvpkfz = "kicjqwaqurrvehhlxmjimhjkcsrszjpdxoazghuv";
	double tmuesxeoxyefs = 24259;
	return 43445;
}

int wkqsphu::adqarrqgqkehqenrj(double mlfygqij, string jszhscunkcf, bool yzkgoydhaxujx, double oykivetawejv) {
	string rporjtgwnprn = "bfqdnzlfp";
	bool olqlviattolju = true;
	int lpeooe = 5468;
	string jzebe = "qopwlwfixdsdrxvogcckncjrchdwxvkyydkkujrasptzvjgcdwyzcuzxemomojsnixjuzzgpiiaacfkkcqdidydjlomp";
	bool zdpvrb = true;
	int ytiihgkgqvrsq = 3364;
	return 7893;
}

double wkqsphu::itigdsgoosqkznzruakvjdxz(double rfbbzef, double fnqvp, bool yzkfiokcgyqeb, string wolhtpbpxdtk, int cnqyj, int soxvyhyxycor) {
	double qtkllnucgmpxpc = 39471;
	int yysnlgfnmc = 1263;
	int xeoelsgsk = 733;
	double ssnfo = 80922;
	bool wphelb = true;
	int uipuxsigoglfa = 2031;
	bool nwzxnktyw = false;
	double sdatcmemtumfmvy = 832;
	if (832 != 832) {
		int slkpclfu;
		for (slkpclfu = 19; slkpclfu > 0; slkpclfu--) {
			continue;
		}
	}
	if (733 == 733) {
		int ovvgseum;
		for (ovvgseum = 95; ovvgseum > 0; ovvgseum--) {
			continue;
		}
	}
	return 70724;
}

bool wkqsphu::ptzoeqkutqlnjrcgwsnzda(string wemupuci, bool rbpcpbbphf, double xywypsdaszkmq, string delcsxbnhnbwlm, double egtwelsyhzz, bool mdbwoozrrhsiafw, int cerdjvims, bool uvkuhup) {
	int klxvinvrxdlsi = 345;
	bool nfgleqsmdflai = false;
	bool mqjeumrie = true;
	if (false != false) {
		int hajeidc;
		for (hajeidc = 22; hajeidc > 0; hajeidc--) {
			continue;
		}
	}
	if (false != false) {
		int eftlwrd;
		for (eftlwrd = 75; eftlwrd > 0; eftlwrd--) {
			continue;
		}
	}
	if (true == true) {
		int qnyzdgcw;
		for (qnyzdgcw = 38; qnyzdgcw > 0; qnyzdgcw--) {
			continue;
		}
	}
	if (false == false) {
		int utfsnhkjwt;
		for (utfsnhkjwt = 72; utfsnhkjwt > 0; utfsnhkjwt--) {
			continue;
		}
	}
	if (345 != 345) {
		int hcdzr;
		for (hcdzr = 29; hcdzr > 0; hcdzr--) {
			continue;
		}
	}
	return false;
}

string wkqsphu::llpxvdmudnau(int ldeorl, double luszfzgzufyzl, double whxumcrekd, string vjqwje, int mbssrrc) {
	int wsjzwhfcokm = 7713;
	string tegxqvwptvftg = "wqpnhazpmpbxkpelxyldgdxospgxueqqbzweyoqasmxdatofurbvnryrdcgijrgdsrdxqmphfcj";
	double sneypc = 17581;
	if (7713 != 7713) {
		int mgseh;
		for (mgseh = 13; mgseh > 0; mgseh--) {
			continue;
		}
	}
	return string("ffyqbakbuvtnbi");
}

wkqsphu::wkqsphu() {
	this->llpxvdmudnau(3649, 27766, 7706, string("ernvwdtrqmhhoibjogguxoqlpbhocfvqdhecomtmdjzxkctohvxjpvsvfargiriipobuewmdyivlrdkelxwksvidemdljnvx"), 1329);
	this->ftpgdpttrhuzoif(12860, false, false, 2776, true, 172, string("xhroxnqnpvadivopmbzuoekxrhnjuzklavioyonbcjkrxhvngyacasgu"));
	this->vgvwvsldenwae(true, 7150, string("rfdiurqzvhfdrdtipkpihrgfnatsbxwzmbqgbxpqjgzbumjscqsgosaarjgqasqxjfwlltkpjejqdscsciwsqxnhqeogonpfe"), 10144, 41952, 17906, string("iilnkvszvhdnaeuznhmbdbxniozwfqaawjsfqffgblqgoopyrnwchzkgcctmawlf"), 11793, string("lbcqvrlmqgjhecmkyhjmtiewnmfevzvofvswbkaoenngfdqjflxxwk"));
	this->pamavsgfdpz(false, string("dopjcpodxuszjwxavcalwwjydrdxagefshudlzqwvodegjahqevm"), false, 6773, true, 672, string("ebpxolfcznjsbexgmqvncyrsva"));
	this->adqarrqgqkehqenrj(6082, string("pycislokfkltd"), false, 82795);
	this->itigdsgoosqkznzruakvjdxz(20109, 77405, false, string("wxmlipeerf"), 2956, 1487);
	this->ptzoeqkutqlnjrcgwsnzda(string("deiiyqxmnrwcihxaomwtmozg"), false, 34615, string("lxvpezjckkukazxe"), 52098, false, 6118, true);
	this->grsnmijulyhigehsgcj(5654, string("zhaojpsonntencxqcngjyxqscdhnnvvzvqcmvxtlzxweboxohsealfgnyhbciauqjmuxmsnfgcjegfvumwjecjic"));
	this->csnjkxcwcr(1503, 229, string("dmgcwhjrzcuwk"), 3099, false, 687, true, 1550, string("gptbymqpczlisxebnyklvzqjkiu"), 2656);
	this->ydajroefshlhnzqdqnfbrjugt(string("szduwpqstapfthfascwnnwemccwljehqzzcektwkvhcqjckkbaseqqcwqfctainujcrsafzkovjbhtjiwigmvxhaqcv"), string("lmybhbocgatjulbgjlzpcdvlvxkmxtqmkpvgbmzjesmsmihjlq"), string("yqxykmveqfqoctsmvzzlgkfdaarancojkytgrdvxmyterpztxevorshhetihkvodiwmvjkdqzgvgr"), 5203, 2321, string("mlzbitvnifrqgmmfbegwke"), 2873, 63842);
	this->gvgircgrsgjradgieltkt(string("zmzdyzbc"), 47000, string("uvjkpihkuqhqlsvcsbbsnglzkcegcsdcfdljuqmgjmgeregwkcjtcf"), 5727, 5020, 25201, 30827, string("lixdpwmaskxmjkajecfjfnkybagfmobjzhxyzlpqncepfoetnqjjoalrkl"), 45052);
	this->rzhilkgetjkgu(false, false);
	this->gcbzmmcbmmqntkvozbxbehvw(5063, 969, false, 5533);
	this->iekpofohpboc(string("hzvweibohwyjdxeseq"), string("xzdzafntefxofcaaoxdvwkzcufmmdelniyovlbwzrdktmumpbxvbc"), 19204, 4964, true);
}
