/*
Syn's AyyWare Framework 2015
*/

#include "Offsets.h"
#include "UTIL Functions.h"

#define strenc( s ) ( s )

void Offsets::Initialise()
{
	// Modules
	Modules::Client = Utilities::Memory::WaitOnModuleHandle("client.dll");
	Modules::Engine = Utilities::Memory::WaitOnModuleHandle("engine.dll");
	Modules::VGUI2 = Utilities::Memory::WaitOnModuleHandle("vgui2.dll");
	Modules::VGUISurface = Utilities::Memory::WaitOnModuleHandle("vguimatsurface.dll");
	Modules::Material = Utilities::Memory::WaitOnModuleHandle("materialsystem.dll");
	Modules::VPhysics = Utilities::Memory::WaitOnModuleHandle("vphysics.dll");
	Modules::Stdlib = Utilities::Memory::WaitOnModuleHandle("vstdlib.dll");

	//------------------------------------------------------------------------
	// VTables
#pragma region VTables
	VMT::CHL_GetAllClasses = 8;

	VMT::Engine_GetScreenSize = 5;
	VMT::Engine_GetPlayerInfo = 8;
	VMT::Engine_GetLocalPlayer = 12;
	VMT::Engine_Time = 14;
	VMT::Engine_GetViewAngles = 18;
	VMT::Engine_SetViewAngles = 19;
	VMT::Engine_GetMaxClients = 20;
	VMT::Engine_IsConnected = 27;
	VMT::Engine_IsInGame = 26;
	VMT::Engine_WorldToScreenMatrix = 37;
	VMT::Engine_ClientCmd_Unrestricted = 114;

	VMT::Panel_GetName = 36;
	VMT::Panel_PaintTraverse = 41;

	VMT::Surface_DrawSetColorA = 14;
	VMT::Surface_DrawSetColorB = 15;
	VMT::Surface_DrawFilledRect = 16;
	VMT::Surface_DrawOutlinedRect = 18;
	VMT::Surface_DrawLine = 19;
	VMT::Surface_DrawSetTextFont = 23;
	VMT::Surface_DrawSetTextColorA = 24;
	VMT::Surface_DrawSetTextColorB = 25;
	VMT::Surface_DrawSetTextPos = 26;
	VMT::Surface_DrawPrintText = 28;
	VMT::Surface_DrawSetTextureRGBA = 37;
	VMT::Surface_DrawSetTexture = 38;
	VMT::Surface_CreateNewTextureID  = 43;
	VMT::Surface_FontCreate = 71;
	VMT::Surface_SetFontGlyphSet = 72;
	VMT::Surface_GetTextSize = 79;
	VMT::Surface_DrawOutlinedCircle = 103;
	VMT::Surface_SurfaceGetCursorPos = 66;
	VMT::Surface_DrawTexturedPolygon = 106;

	VMT::Material_GetName = 0;
	VMT::Material_SetMaterialVarFlag = 30;
	VMT::Material_GetMaterialVarFlag = 31;
	VMT::Material_AlphaModulate = 28;
	VMT::Material_ColorModulate = 29;
	VMT::Material_IncrementReferenceCount = 14;
	
	VMT::MaterialSystem_FindMaterial = 84;
	VMT::MaterialSystem_CreateMaterial = 83;
	
	VMT::ModelRender_ForcedMaterialOverride = 1;
	VMT::ModelRender_DrawModelExecute = 21;
	
	VMT::ModelInfo_GetModelName = 3;
	VMT::ModelInfo_GetStudiomodel = 30;

	VMT::RenderView_SetBlend = 4;
	VMT::RenderView_SetColorModulation = 6;

#pragma endregion Contains the VTable Indexs

	// I cbf trying to get the KeyValues part of the SDK working solo, so we'll just
	// Do some dirty shit
	Functions::KeyValues_KeyValues = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x68\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x89\x45\xFC\xEB\x07\xC7\x45\x00\x00\x00\x00\x00\x8B\x03\x56", "x????xxx????xxxxxxx?????xxx");
	Functions::KeyValues_KeyValues += 7;
	Functions::KeyValues_KeyValues = Functions::KeyValues_KeyValues + *reinterpret_cast< PDWORD_PTR >(Functions::KeyValues_KeyValues + 1) + 5;

	Functions::KeyValues_LoadFromBuffer = Utilities::Memory::FindPattern("client.dll", (PBYTE)"\xE8\x00\x00\x00\x00\x80\x7D\xF8\x00\x00\x00\x85\xDB", "x????xxxx??xx");
	Functions::KeyValues_LoadFromBuffer = Functions::KeyValues_LoadFromBuffer + *reinterpret_cast< PDWORD_PTR >(Functions::KeyValues_LoadFromBuffer + 1) + 5;

	Functions::dwCalcPlayerView =  Utilities::Memory::FindPattern("client.dll", (PBYTE)"\x84\xC0\x75\x08\x57\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x06", "xxxxxxxx????xx");

	Functions::dwGetPlayerCompRank = GameUtils::FindPattern1(strenc("client.dll"), strenc("55 8B EC 8B 0D ? ? ? ? 68 ? ? ? ? "));

	Functions::dwIsReady = GameUtils::FindPattern1(strenc("client.dll"), strenc("55 8B EC 51 56 8B 35 ? ? ? ? 80 7E 58 00"));

	Utilities::Log("Offsets/Indexes Up to Date");
}

namespace Offsets
{
	// Addresses of loaded game modules
	namespace Modules
	{
		DWORD Client;
		DWORD Engine;
		DWORD VGUI2;
		DWORD VGUISurface;
		DWORD Material;
		DWORD VPhysics;
		DWORD Stdlib;
	};

	// Virtual Method Table Indexes
	namespace VMT
	{
		//CHL Client
		DWORD CHL_GetAllClasses;

		//Engine Client
		DWORD Engine_GetScreenSize;
		DWORD Engine_GetPlayerInfo;
		DWORD Engine_GetLocalPlayer;
		DWORD Engine_Time;
		DWORD Engine_GetViewAngles;
		DWORD Engine_SetViewAngles;
		DWORD Engine_GetMaxClients;
		DWORD Engine_IsConnected;
		DWORD Engine_IsInGame;
		DWORD Engine_WorldToScreenMatrix;
		DWORD Engine_GetNetChannelInfo;
		DWORD Engine_ClientCmd_Unrestricted;

		// Panels
		DWORD Panel_GetName;
		DWORD Panel_PaintTraverse;

		// Surface
		DWORD Surface_DrawSetColorA;
		DWORD Surface_DrawSetColorB;
		DWORD Surface_DrawFilledRect;
		DWORD Surface_DrawOutlinedRect;
		DWORD Surface_DrawLine;
		DWORD Surface_DrawSetTextFont;
		DWORD Surface_DrawSetTextColorA;
		DWORD Surface_DrawSetTextColorB;
		DWORD Surface_DrawSetTextPos;
		DWORD Surface_DrawPrintText;
		DWORD Surface_DrawSetTextureRGBA;
		DWORD Surface_DrawSetTexture;
		DWORD Surface_CreateNewTextureID;
		DWORD Surface_FontCreate;
		DWORD Surface_SetFontGlyphSet;
		DWORD Surface_GetTextSize;
		DWORD Surface_DrawOutlinedCircle;
		DWORD Surface_SurfaceGetCursorPos;
		DWORD Surface_DrawTexturedPolygon;

		DWORD Material_GetName;
		DWORD Material_SetMaterialVarFlag;
		DWORD Material_GetMaterialVarFlag;
		DWORD Material_AlphaModulate;
		DWORD Material_ColorModulate;
		DWORD Material_IncrementReferenceCount;

		DWORD MaterialSystem_FindMaterial;
		DWORD MaterialSystem_CreateMaterial;

		DWORD ModelRender_ForcedMaterialOverride;
		DWORD ModelRender_DrawModelExecute;

		DWORD ModelInfo_GetModelName;
		DWORD ModelInfo_GetStudiomodel;

		DWORD RenderView_SetBlend;
		DWORD RenderView_SetColorModulation;

		// Weapon entities
		DWORD Weapon_GetSpread;
	};

	// Addresses of engine functions to call
	namespace Functions
	{
		DWORD KeyValues_KeyValues;
		DWORD KeyValues_LoadFromBuffer;
		DWORD dwCalcPlayerView;
		DWORD dwGetPlayerCompRank;
		DWORD dwIsReady;
	};

};

//JUNK CODE STARTS HERE!!

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ffgvuia {
public:
	string fzuoztwsows;
	string zqqpje;
	ffgvuia();
	void dzxyxkipmgpbr(int wcayzndoczeif, int yeysqn);
	double cgpjagmtnqzealdhpvohdg(bool wyqmaoajbc, double gpvyhedcdvrcvo, double lpkew, double ovhqxckrteticj, double dvmpv);
	bool pphbfnbxkazzyonoorreqkf(double bsndr, string ndhtdii, string ogodltjgwrn, string bemlvskmufv, bool bvvzfvudj, bool aiuvrmxfl, int wdaskibbuergr);
	void swnepfduvzpexuyjxqyrxzp();
	string rypryseukp(double ihppfxppvxrc, bool jrecl, double kctypufidcnglz, int bcrsowxgzctt);
	string lejjrtflggmmu(int yoxra, string hfsyrgwfvbnnqzu, double xwztiqtwdgrdbw, int qpcapebwn, string pgdex, int fwotb, bool qeyna, double tgzgpeyay, string lkfmrlyyneva);
	void iahmvrgucyzzduztriv(double lvyerjauow, string xavxazpctat, string qsnxsrwktoqr, double psuubhjauqjdn);

protected:
	double tpgchwgbavowpjx;
	int kbwvdntaqcnpl;

	void hsbpvaicunicyvtijgk(int myeuim, double bsmnxniv, double ekehqwjtjjj, double qlhnghl, string zzxch, int zryfn, bool pgzbegxoyxwk, bool khfth, double umxiboxkontgkh);
	int xxfwnxvmpglzyvywnvtomot(bool ecyethnvra, bool fijbtimnileh, int rxxjfftwl, int dpdzjttevc);
	double vazaihrjaw(double ldlsoe);
	string zlsjfxwczadj();
	void dgyeqnjjhvgqwlzpzh(double djbcshthwmzobdj, double dlnbfvmdpkcyco, int tmrwhd, bool xcbgvrcelmu, int akloimwunpuc, string wktwcjje, int iqoolzw, int yxcxrhcfnctgki);
	bool udjoocyzlocrjctmtuskd(double zmwxtopxgdy, string jkguk, string byfzan, double woafymejoimz);
	void aoibqmcepcxvitksksxwmkac(double bxsnd);
	void xpvdqnnfbfennntmnfgowy(int bnfyimleexnqdu, string tivxtnfhc, double cmvqqqjubpotq, int niztyd, string zxkfhyt, double cgqdmy, double weavz, string jsuzfwch);
	int capfeuoqbmizfejx(double rwkggqrkvnuq, double pudlk);
	string cofbarcxmanzs();

private:
	int jnrxerl;
	double puwwcivbd;

	double kyqmdbxlzvbqcn(bool kvpua, double xyohqbtswjynudp, double skemntjrmw, bool xekjbmorajm, string wvlptqr, int cxcmlmsghh);
	int duzculnehlviadjssgxffliz(double zhaziw, string knfxtkrxtclaml, string psdrzvha);
	int vfipvfwsietrhbecj(bool atjsgdpbkbqmwe);
	string ttbmmaumwzgs(double brsmixntlo);
	double slmlbieojiitjfudwdtq();
	void wsclywobucvn(double xecorjzfz, bool uydabhmmbyr, int mtswjp, bool awvgewhtewpjt, int jrzqcwu, int avwtmprxyxg);
	void xqoikyjaklbfyyhqsusmvkdz(double wzpav);
	bool hsjjidwcnaitbzubzkfex(double zrmlykzoq, int htajmr, bool fmuifggfwa, string uhluonjbnmwswul, double lyqksjt, string kiswztgvcsc, string dlgzkaadhb, string pcaibugjee, double ylyqjdnb, int qdwcklhqxfdknpz);
	string fdjbsqjrtgxlljtzbjsibyydw(string lwrxxakponp);

};


double ffgvuia::kyqmdbxlzvbqcn(bool kvpua, double xyohqbtswjynudp, double skemntjrmw, bool xekjbmorajm, string wvlptqr, int cxcmlmsghh) {
	double junsqb = 23641;
	bool waugh = false;
	bool zdzcrhfyzch = true;
	if (true == true) {
		int gigho;
		for (gigho = 80; gigho > 0; gigho--) {
			continue;
		}
	}
	return 74362;
}

int ffgvuia::duzculnehlviadjssgxffliz(double zhaziw, string knfxtkrxtclaml, string psdrzvha) {
	string tcjilj = "ttluafrvjrkpmuqbykttmqioesmjklmpspmhrzddvzpuhmumirqrqzkgofwaiflvxjbycjtyyzxxescbzvlwgrwro";
	bool lknruimexcqge = false;
	int flzoadkajq = 4391;
	int mqvpfohhvznsev = 1609;
	string ylyzjgapzrw = "pjmnvakhgjjbvjjedapq";
	if (4391 == 4391) {
		int vmke;
		for (vmke = 20; vmke > 0; vmke--) {
			continue;
		}
	}
	if (string("pjmnvakhgjjbvjjedapq") != string("pjmnvakhgjjbvjjedapq")) {
		int tu;
		for (tu = 94; tu > 0; tu--) {
			continue;
		}
	}
	if (string("pjmnvakhgjjbvjjedapq") != string("pjmnvakhgjjbvjjedapq")) {
		int oasuyfzbp;
		for (oasuyfzbp = 66; oasuyfzbp > 0; oasuyfzbp--) {
			continue;
		}
	}
	if (string("ttluafrvjrkpmuqbykttmqioesmjklmpspmhrzddvzpuhmumirqrqzkgofwaiflvxjbycjtyyzxxescbzvlwgrwro") == string("ttluafrvjrkpmuqbykttmqioesmjklmpspmhrzddvzpuhmumirqrqzkgofwaiflvxjbycjtyyzxxescbzvlwgrwro")) {
		int gtmfssgqxe;
		for (gtmfssgqxe = 41; gtmfssgqxe > 0; gtmfssgqxe--) {
			continue;
		}
	}
	return 33950;
}

int ffgvuia::vfipvfwsietrhbecj(bool atjsgdpbkbqmwe) {
	int vghxmpxeomnv = 2532;
	string niqgkpkbtig = "hcfvdgivwywcoszmgmgbcoadlyjnfsptohruevsipeydzrbfyigmwdmlspmuiqan";
	bool lpnbwizylszct = true;
	string oexlnpxr = "zoxssscrdmqqrkvxyoqyxlwotoqtvzpjinspdhpaksbyvmvgxrsrygzmuopqrbzbcglthklqeothttpcinadamtrqieddcaem";
	bool qigdcgbm = true;
	double aetmofcrlvikzt = 11926;
	bool qrzeftndw = false;
	if (true != true) {
		int hosrdxjg;
		for (hosrdxjg = 9; hosrdxjg > 0; hosrdxjg--) {
			continue;
		}
	}
	if (11926 != 11926) {
		int yek;
		for (yek = 82; yek > 0; yek--) {
			continue;
		}
	}
	if (false != false) {
		int oiw;
		for (oiw = 86; oiw > 0; oiw--) {
			continue;
		}
	}
	return 97275;
}

string ffgvuia::ttbmmaumwzgs(double brsmixntlo) {
	string cuyyggecblu = "wpduehnoqg";
	bool hsukhzwadyzgqzw = true;
	double meqbacdpwd = 3077;
	bool oifbqk = true;
	int zwjwjhtpaopbuqj = 8271;
	double yrznfj = 24822;
	string dzkavr = "bhlmhhgkfcnzpimbzimqxpossvkayillwuzjuryu";
	if (3077 != 3077) {
		int cotdod;
		for (cotdod = 2; cotdod > 0; cotdod--) {
			continue;
		}
	}
	if (3077 == 3077) {
		int tult;
		for (tult = 85; tult > 0; tult--) {
			continue;
		}
	}
	return string("gjnvscbaeqwrpfr");
}

double ffgvuia::slmlbieojiitjfudwdtq() {
	bool elelu = false;
	bool vmzwloagi = false;
	int bxlmtynjquqippv = 976;
	if (976 != 976) {
		int zzkh;
		for (zzkh = 19; zzkh > 0; zzkh--) {
			continue;
		}
	}
	if (976 != 976) {
		int opabvvdxwt;
		for (opabvvdxwt = 17; opabvvdxwt > 0; opabvvdxwt--) {
			continue;
		}
	}
	if (976 != 976) {
		int jvvejadu;
		for (jvvejadu = 25; jvvejadu > 0; jvvejadu--) {
			continue;
		}
	}
	if (976 == 976) {
		int bobpqbxs;
		for (bobpqbxs = 5; bobpqbxs > 0; bobpqbxs--) {
			continue;
		}
	}
	if (false == false) {
		int szfmgnnc;
		for (szfmgnnc = 74; szfmgnnc > 0; szfmgnnc--) {
			continue;
		}
	}
	return 93648;
}

void ffgvuia::wsclywobucvn(double xecorjzfz, bool uydabhmmbyr, int mtswjp, bool awvgewhtewpjt, int jrzqcwu, int avwtmprxyxg) {

}

void ffgvuia::xqoikyjaklbfyyhqsusmvkdz(double wzpav) {

}

bool ffgvuia::hsjjidwcnaitbzubzkfex(double zrmlykzoq, int htajmr, bool fmuifggfwa, string uhluonjbnmwswul, double lyqksjt, string kiswztgvcsc, string dlgzkaadhb, string pcaibugjee, double ylyqjdnb, int qdwcklhqxfdknpz) {
	string dggiauicqrijdw = "zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef";
	int bnzvgchxfzvj = 4672;
	if (string("zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef") != string("zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef")) {
		int vhfmon;
		for (vhfmon = 21; vhfmon > 0; vhfmon--) {
			continue;
		}
	}
	if (string("zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef") == string("zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef")) {
		int mcppvqm;
		for (mcppvqm = 10; mcppvqm > 0; mcppvqm--) {
			continue;
		}
	}
	if (4672 != 4672) {
		int oeeozftit;
		for (oeeozftit = 31; oeeozftit > 0; oeeozftit--) {
			continue;
		}
	}
	if (string("zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef") == string("zkxpvmherzzhpfmaihzsgacsvxdxebvgirungomjbacpxrucaeayhpserjdcrqnphdokktfspsotzzbfoibpmymeef")) {
		int zfpi;
		for (zfpi = 84; zfpi > 0; zfpi--) {
			continue;
		}
	}
	if (4672 != 4672) {
		int dwvz;
		for (dwvz = 53; dwvz > 0; dwvz--) {
			continue;
		}
	}
	return false;
}

string ffgvuia::fdjbsqjrtgxlljtzbjsibyydw(string lwrxxakponp) {
	int hzmwmswafuag = 1106;
	int uhzdxjxhwj = 975;
	int yylge = 2454;
	return string("mfoprlwgnfljp");
}

void ffgvuia::hsbpvaicunicyvtijgk(int myeuim, double bsmnxniv, double ekehqwjtjjj, double qlhnghl, string zzxch, int zryfn, bool pgzbegxoyxwk, bool khfth, double umxiboxkontgkh) {
	string lgosla = "wrshoezbpcwblfpjbskteknniaazlkwpiptwkqxilsngdxsvyjvrhmqma";
	double clwxbfycggr = 20592;
	int yfjyrqmnwd = 7273;
	int jvrvdmv = 2129;
	string kivwrdolg = "qfjzoyrjqzyrdiyjovgntotvhxfust";
	int amvuojy = 1576;
	double hhwrwdnjfu = 71963;
	if (7273 != 7273) {
		int xu;
		for (xu = 0; xu > 0; xu--) {
			continue;
		}
	}
	if (2129 != 2129) {
		int hmcvvqhhkn;
		for (hmcvvqhhkn = 25; hmcvvqhhkn > 0; hmcvvqhhkn--) {
			continue;
		}
	}
	if (71963 == 71963) {
		int kyizyf;
		for (kyizyf = 35; kyizyf > 0; kyizyf--) {
			continue;
		}
	}
	if (string("wrshoezbpcwblfpjbskteknniaazlkwpiptwkqxilsngdxsvyjvrhmqma") == string("wrshoezbpcwblfpjbskteknniaazlkwpiptwkqxilsngdxsvyjvrhmqma")) {
		int wdvqjf;
		for (wdvqjf = 58; wdvqjf > 0; wdvqjf--) {
			continue;
		}
	}

}

int ffgvuia::xxfwnxvmpglzyvywnvtomot(bool ecyethnvra, bool fijbtimnileh, int rxxjfftwl, int dpdzjttevc) {
	return 31320;
}

double ffgvuia::vazaihrjaw(double ldlsoe) {
	string sewityqiatwe = "iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof";
	if (string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof") != string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof")) {
		int yvznqv;
		for (yvznqv = 90; yvznqv > 0; yvznqv--) {
			continue;
		}
	}
	if (string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof") == string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof")) {
		int kn;
		for (kn = 19; kn > 0; kn--) {
			continue;
		}
	}
	if (string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof") != string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof")) {
		int uiautsy;
		for (uiautsy = 32; uiautsy > 0; uiautsy--) {
			continue;
		}
	}
	if (string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof") != string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof")) {
		int yqa;
		for (yqa = 16; yqa > 0; yqa--) {
			continue;
		}
	}
	if (string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof") != string("iwqivlxktzlaazmzwzfnnszkjsvfvqbgadoazyijof")) {
		int sxxxsstcnt;
		for (sxxxsstcnt = 84; sxxxsstcnt > 0; sxxxsstcnt--) {
			continue;
		}
	}
	return 79179;
}

string ffgvuia::zlsjfxwczadj() {
	bool gpnpy = true;
	double hgpchjkexaqymx = 26577;
	bool vfndjuyb = false;
	double xogzpcq = 50445;
	int xubsqfxuetkma = 211;
	bool xhgpjap = true;
	bool bougfad = true;
	bool tpriz = false;
	string bxgswuj = "jngvecwigcigntinsiefkypwjyruym";
	if (false != false) {
		int ff;
		for (ff = 10; ff > 0; ff--) {
			continue;
		}
	}
	if (false != false) {
		int hj;
		for (hj = 50; hj > 0; hj--) {
			continue;
		}
	}
	if (true == true) {
		int mpxhxerer;
		for (mpxhxerer = 14; mpxhxerer > 0; mpxhxerer--) {
			continue;
		}
	}
	if (true != true) {
		int it;
		for (it = 59; it > 0; it--) {
			continue;
		}
	}
	return string("plt");
}

void ffgvuia::dgyeqnjjhvgqwlzpzh(double djbcshthwmzobdj, double dlnbfvmdpkcyco, int tmrwhd, bool xcbgvrcelmu, int akloimwunpuc, string wktwcjje, int iqoolzw, int yxcxrhcfnctgki) {
	string yieee = "abnjyjrtnezeirchzjqhktnaaixplbbiuxttkqrxpqltlqwdhsukizuvhsyuaaidvkbtotkqyzzbrlxwsyzfznpt";
	bool rlvdxiyrapg = true;
	double zzsyecuh = 2318;
	int csqfliqp = 1642;
	int uwcpvehrfoclbme = 2315;
	double looynope = 87089;
	string pnwqvelyxyixca = "rbwqjzocslfddkiydxfrdsbqjglkmypljjvvsnphkvvkriahgyiofolxqgelkbpnrnvzcbgubvyecgynxkqkmbc";
	if (string("abnjyjrtnezeirchzjqhktnaaixplbbiuxttkqrxpqltlqwdhsukizuvhsyuaaidvkbtotkqyzzbrlxwsyzfznpt") != string("abnjyjrtnezeirchzjqhktnaaixplbbiuxttkqrxpqltlqwdhsukizuvhsyuaaidvkbtotkqyzzbrlxwsyzfznpt")) {
		int ez;
		for (ez = 60; ez > 0; ez--) {
			continue;
		}
	}

}

bool ffgvuia::udjoocyzlocrjctmtuskd(double zmwxtopxgdy, string jkguk, string byfzan, double woafymejoimz) {
	double ueguatorschfxp = 6569;
	int dgjqs = 1133;
	bool bttpsfyzbo = false;
	string enaszpn = "oifriieepidwhasmxgzwwknidcuoldtsiwufhipaxlmbpfnivdii";
	if (string("oifriieepidwhasmxgzwwknidcuoldtsiwufhipaxlmbpfnivdii") == string("oifriieepidwhasmxgzwwknidcuoldtsiwufhipaxlmbpfnivdii")) {
		int ozokm;
		for (ozokm = 94; ozokm > 0; ozokm--) {
			continue;
		}
	}
	if (string("oifriieepidwhasmxgzwwknidcuoldtsiwufhipaxlmbpfnivdii") != string("oifriieepidwhasmxgzwwknidcuoldtsiwufhipaxlmbpfnivdii")) {
		int qjmrzc;
		for (qjmrzc = 23; qjmrzc > 0; qjmrzc--) {
			continue;
		}
	}
	return false;
}

void ffgvuia::aoibqmcepcxvitksksxwmkac(double bxsnd) {
	bool fkckozeuuqwjxt = true;
	double hgkiegahnn = 70132;
	int mrsfb = 1959;
	double ojjkrurweba = 8156;
	double vuvsxu = 15217;
	double oyydpomos = 70044;
	int dsiab = 3583;
	string hlpubb = "nmdkakeeahdnfosspvpvdsufbxpwjssgnzahv";
	double bgfodqwj = 743;
	if (8156 == 8156) {
		int ebxlzeb;
		for (ebxlzeb = 100; ebxlzeb > 0; ebxlzeb--) {
			continue;
		}
	}
	if (true != true) {
		int bbdlcdc;
		for (bbdlcdc = 78; bbdlcdc > 0; bbdlcdc--) {
			continue;
		}
	}
	if (743 != 743) {
		int iqltyo;
		for (iqltyo = 81; iqltyo > 0; iqltyo--) {
			continue;
		}
	}
	if (3583 == 3583) {
		int oj;
		for (oj = 39; oj > 0; oj--) {
			continue;
		}
	}

}

void ffgvuia::xpvdqnnfbfennntmnfgowy(int bnfyimleexnqdu, string tivxtnfhc, double cmvqqqjubpotq, int niztyd, string zxkfhyt, double cgqdmy, double weavz, string jsuzfwch) {
	double gzcymzqua = 1193;
	string fqevvtcfp = "lokyfxdvtdoplcxyvmuubjtfsoqzbbphfcml";
	bool rzyfn = true;
	double tanlrqyigyzhmlp = 3896;
	bool ithsyzpgz = false;
	int mvfif = 763;
	string ynmisebkgc = "";
	double yofjzta = 55109;
	string pypuzumgcmne = "tkjzljarlhaihdofcmsehscuffxfvkur";
	int xeqyhdcbu = 6770;
	if (1193 == 1193) {
		int woei;
		for (woei = 49; woei > 0; woei--) {
			continue;
		}
	}
	if (string("") == string("")) {
		int ccjh;
		for (ccjh = 14; ccjh > 0; ccjh--) {
			continue;
		}
	}
	if (763 != 763) {
		int tue;
		for (tue = 94; tue > 0; tue--) {
			continue;
		}
	}

}

int ffgvuia::capfeuoqbmizfejx(double rwkggqrkvnuq, double pudlk) {
	string wvopzmqsbvg = "czfqsnqxrxmbdxedoxrhmprqsvxxviksgmuoukkvgcohvbxsmgejzyuavkbajyqpdnsnitvzxjxpbzrxfbkyakhjbfjiigxkzfvs";
	bool hdtytnbsegaea = true;
	double gvoai = 27611;
	bool sxthaktqqo = false;
	int uniqs = 7902;
	int hnvamdfmmt = 3061;
	bool wavqfgadjjrkkk = false;
	double oiljnceejd = 26786;
	if (26786 != 26786) {
		int ajukakrctf;
		for (ajukakrctf = 44; ajukakrctf > 0; ajukakrctf--) {
			continue;
		}
	}
	if (26786 != 26786) {
		int nszretfzv;
		for (nszretfzv = 47; nszretfzv > 0; nszretfzv--) {
			continue;
		}
	}
	if (false != false) {
		int ixkphrkeuz;
		for (ixkphrkeuz = 62; ixkphrkeuz > 0; ixkphrkeuz--) {
			continue;
		}
	}
	if (26786 == 26786) {
		int ttri;
		for (ttri = 3; ttri > 0; ttri--) {
			continue;
		}
	}
	if (false == false) {
		int gppojqxqy;
		for (gppojqxqy = 2; gppojqxqy > 0; gppojqxqy--) {
			continue;
		}
	}
	return 24825;
}

string ffgvuia::cofbarcxmanzs() {
	int holnvclgohsmkf = 5370;
	bool pfkrsr = false;
	int cytaof = 1350;
	bool lrhivi = false;
	string urkahu = "kfrhpuzrbedllasicrxhimrduxongcsduewbtpsophccpwoeysuejmzjsmxhpzjxxsynygjxadrzfdeysfzv";
	double oycidcffiz = 14787;
	bool wlekpgdgea = false;
	bool tmadzz = false;
	int rvrvt = 487;
	if (string("kfrhpuzrbedllasicrxhimrduxongcsduewbtpsophccpwoeysuejmzjsmxhpzjxxsynygjxadrzfdeysfzv") != string("kfrhpuzrbedllasicrxhimrduxongcsduewbtpsophccpwoeysuejmzjsmxhpzjxxsynygjxadrzfdeysfzv")) {
		int pessttar;
		for (pessttar = 4; pessttar > 0; pessttar--) {
			continue;
		}
	}
	return string("w");
}

void ffgvuia::dzxyxkipmgpbr(int wcayzndoczeif, int yeysqn) {
	double uoewqoqfreqnknm = 5636;
	double uvhampthzcxt = 19985;
	bool ysaeicawftepmbn = true;
	double spatqviiogrkh = 5270;
	string venhjmfv = "scoutuxodtajizjeeywqngsyabzrtqzibwkjnlolnxvnpugoctzicpuktelibzmu";
	string qusdtuvtcvdk = "wmy";
	string htglatbcjx = "knrfgicwwpzpvizabowwydterqcxk";
	int kuusjio = 187;
	double lnuoudfpuycblny = 47444;
	if (19985 != 19985) {
		int ldiwcgmwso;
		for (ldiwcgmwso = 73; ldiwcgmwso > 0; ldiwcgmwso--) {
			continue;
		}
	}
	if (47444 == 47444) {
		int dnrw;
		for (dnrw = 60; dnrw > 0; dnrw--) {
			continue;
		}
	}

}

double ffgvuia::cgpjagmtnqzealdhpvohdg(bool wyqmaoajbc, double gpvyhedcdvrcvo, double lpkew, double ovhqxckrteticj, double dvmpv) {
	bool amtrwtfmbyvmr = false;
	bool cuhggjnkwbrg = true;
	int nndrlnpkpc = 6768;
	int dweupnl = 930;
	int eleowld = 7419;
	string kvgbwsb = "nxnggdschxckcdejscitrgenscsfbqfcjfrvtkspqvytkumnenvbowwyfns";
	bool wbwlfrnlj = true;
	string woikmtiw = "apubdxrkdtkpgnxoxqjybxgdjwywhyxpjrdwpntiabhpyybcykvmwxzdlwydojewzhiehbilzliuhbukvdixevneopkitrkhj";
	string icbclisohwbfgrd = "ynwna";
	if (930 != 930) {
		int pfltakzaxk;
		for (pfltakzaxk = 44; pfltakzaxk > 0; pfltakzaxk--) {
			continue;
		}
	}
	if (string("nxnggdschxckcdejscitrgenscsfbqfcjfrvtkspqvytkumnenvbowwyfns") == string("nxnggdschxckcdejscitrgenscsfbqfcjfrvtkspqvytkumnenvbowwyfns")) {
		int nq;
		for (nq = 55; nq > 0; nq--) {
			continue;
		}
	}
	if (true != true) {
		int bp;
		for (bp = 93; bp > 0; bp--) {
			continue;
		}
	}
	return 17000;
}

bool ffgvuia::pphbfnbxkazzyonoorreqkf(double bsndr, string ndhtdii, string ogodltjgwrn, string bemlvskmufv, bool bvvzfvudj, bool aiuvrmxfl, int wdaskibbuergr) {
	double fmshbjjrbzpz = 3182;
	if (3182 != 3182) {
		int ket;
		for (ket = 35; ket > 0; ket--) {
			continue;
		}
	}
	if (3182 != 3182) {
		int zcmtentvc;
		for (zcmtentvc = 68; zcmtentvc > 0; zcmtentvc--) {
			continue;
		}
	}
	return false;
}

void ffgvuia::swnepfduvzpexuyjxqyrxzp() {
	string yzfzjabjbhvkwmw = "ialjcoxvhhunhuvhasaadknhqyfertqtsspemwpfjoitfowkhkeajotmxrbhxxqgeikwotzgssz";
	bool egybdzznxdvo = false;
	bool fumkb = true;
	string bnijxzs = "p";
	bool wnoulsoilnujo = false;
	int ccdtigzr = 1633;
	bool axfryo = false;
	if (false != false) {
		int ebyv;
		for (ebyv = 43; ebyv > 0; ebyv--) {
			continue;
		}
	}

}

string ffgvuia::rypryseukp(double ihppfxppvxrc, bool jrecl, double kctypufidcnglz, int bcrsowxgzctt) {
	bool ancpozs = true;
	double qngexnfnu = 1587;
	bool jvzqqhnjlkjolg = true;
	string behowponawcgco = "zerugwwybvavpl";
	bool hjunmxdmlcqx = false;
	double gdpofzkchuyn = 3842;
	bool lhibvwuj = false;
	bool nlufztsnwits = false;
	if (false == false) {
		int hhg;
		for (hhg = 19; hhg > 0; hhg--) {
			continue;
		}
	}
	if (1587 == 1587) {
		int xpoq;
		for (xpoq = 60; xpoq > 0; xpoq--) {
			continue;
		}
	}
	return string("aihr");
}

string ffgvuia::lejjrtflggmmu(int yoxra, string hfsyrgwfvbnnqzu, double xwztiqtwdgrdbw, int qpcapebwn, string pgdex, int fwotb, bool qeyna, double tgzgpeyay, string lkfmrlyyneva) {
	int szhlydtckayuyz = 1007;
	double pshkmikr = 3654;
	double osiswvcdksuo = 48871;
	double zvrzbbvzphtpvmr = 21977;
	bool sxxppacouiyzesd = true;
	return string("fprd");
}

void ffgvuia::iahmvrgucyzzduztriv(double lvyerjauow, string xavxazpctat, string qsnxsrwktoqr, double psuubhjauqjdn) {
	int lrmtti = 7434;
	int yihxydqizitdq = 1580;
	double pxtmboonftfq = 31635;
	string nczxbutnyampu = "whqxwyxoiwpvzwxwaxsdahqpizvwdkbcnnwagpbocofczoohgoghmxuxute";
	string nburingitcvaz = "jpynca";
	bool swxujytz = false;
	if (false != false) {
		int irhcftpbcs;
		for (irhcftpbcs = 62; irhcftpbcs > 0; irhcftpbcs--) {
			continue;
		}
	}
	if (string("jpynca") != string("jpynca")) {
		int rx;
		for (rx = 1; rx > 0; rx--) {
			continue;
		}
	}
	if (1580 == 1580) {
		int wr;
		for (wr = 4; wr > 0; wr--) {
			continue;
		}
	}
	if (string("jpynca") != string("jpynca")) {
		int recvqbbv;
		for (recvqbbv = 45; recvqbbv > 0; recvqbbv--) {
			continue;
		}
	}
	if (1580 == 1580) {
		int ue;
		for (ue = 70; ue > 0; ue--) {
			continue;
		}
	}

}

ffgvuia::ffgvuia() {
	this->dzxyxkipmgpbr(1760, 1900);
	this->cgpjagmtnqzealdhpvohdg(false, 71907, 5840, 10518, 71836);
	this->pphbfnbxkazzyonoorreqkf(12624, string("ndbgjkjeawdhnhdqygxcnfnrjogowxvxqovxpazpudxjzblvlwhpxegfjcceayel"), string("fkfagqhxylvyxmlclcojhiddpyfklnhxctdggojrpcanbwvc"), string("grfgosmarrzjrugoeqowbqvdwilaggtojwscsnhljwwtxwulnkykapfxncinnw"), true, true, 2193);
	this->swnepfduvzpexuyjxqyrxzp();
	this->rypryseukp(71292, false, 47984, 6422);
	this->lejjrtflggmmu(6529, string("ycuqgpxhescxpmfvfotinpzfopqugdwyfrujxadpcynbptmukwqlnsxztudeufcqjvpiielqakdyszmamfgbljjretgwusnlmrh"), 10163, 45, string("fmwijpnppstfv"), 3657, false, 2638, string("wqozrhblsiphnusgijucmywjaddtxlikzfznkdmxzvfhqpcsamwakgznqluxwoslfwyamrkdtvqhpgiumjjyaxtnk"));
	this->iahmvrgucyzzduztriv(13256, string("abheciracnffszsibmuetbui"), string("nhdvvnpftwbjpqmvevqxhwldwdfizfaejtqbeaqvylomr"), 56426);
	this->hsbpvaicunicyvtijgk(7701, 21302, 31662, 56744, string("hfxcirghryoznqsqvricqxdhyihw"), 3261, true, true, 68923);
	this->xxfwnxvmpglzyvywnvtomot(true, false, 1853, 1659);
	this->vazaihrjaw(15871);
	this->zlsjfxwczadj();
	this->dgyeqnjjhvgqwlzpzh(5930, 33855, 6983, false, 1009, string("lnegrivjcxrmiymhyrtjxnuzwsghrltpacyumkaiiblwggkkrcftcadvizdl"), 7830, 3587);
	this->udjoocyzlocrjctmtuskd(8713, string("bpzncmuumiodmeqbrlfrnalqorandkvlhgmonuoqah"), string("tsvvovcrzsauvcajotdixgdchiuewilfoczaysdlvlrofzbdtoxiokhpxajnaikppuahrmezvovclgnkzyqnmqjlt"), 73709);
	this->aoibqmcepcxvitksksxwmkac(40805);
	this->xpvdqnnfbfennntmnfgowy(2728, string("ullgpkndmwflkdgpgpvvtfdlbbqbxooymkprmsmvbx"), 5674, 3422, string("fogrmyh"), 6594, 95509, string("uhvsnqtmbovujhhegrxuckkkzdelcwwpkvvvnxpmivpaxikpjjqcbgkfpijngtorxjtfofrwjztklfzevgnlnbubegfm"));
	this->capfeuoqbmizfejx(50419, 66523);
	this->cofbarcxmanzs();
	this->kyqmdbxlzvbqcn(false, 11978, 59580, false, string("zhhgquxcbmgqadabrmvgcnjgitufjrkikomubmxqyfwsfsytfdrgztlpztccqornxocuglja"), 1160);
	this->duzculnehlviadjssgxffliz(15366, string("jpzyauigrozgxuajpcyuwouadimzhqrcqkagtfbsxxxhpvhbshwzxvblqsxojgnpgnljwjuzrbj"), string("qbuhgkldnhzismjyhclzlhrsqddnwvewxaisphbaauyownaxalyahfluyalahllnigwyuoxcezbtrjhwiymrhoepkkuijnatdwqg"));
	this->vfipvfwsietrhbecj(false);
	this->ttbmmaumwzgs(10608);
	this->slmlbieojiitjfudwdtq();
	this->wsclywobucvn(48239, true, 1621, true, 962, 3932);
	this->xqoikyjaklbfyyhqsusmvkdz(52004);
	this->hsjjidwcnaitbzubzkfex(18061, 6341, true, string("gwqszshzrprrzwiygvzlstezyyndojdqhabsaesrevttodvwmqvtzjtopjuevgvsly"), 1647, string("higlwvwzabhrgbiijzqgwxuxxtxahmpcfmxsovlbtkliltagltnsa"), string("fpagyxtpijuzqhxhyixlwfgimvycjhqdovfzxbjmuhqiplyhasvqisjdunixrxhzkjpixiquyufzkyadvjdxwu"), string("uvnomayzxoovf"), 9118, 2612);
	this->fdjbsqjrtgxlljtzbjsibyydw(string("ciudbivvspyzyaedmcdprccixiicordxikubvgsomaoaiyhxscn"));
}
