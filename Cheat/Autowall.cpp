#include "AutoWall.h"
//#include "R.h"

#define    HITGROUP_GENERIC    0
#define    HITGROUP_HEAD        1
#define    HITGROUP_CHEST        2
#define    HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10

inline bool CGameTrace::DidHitWorld() const
{
	return m_pEnt->GetIndex() == 0;
}

inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != NULL && !DidHitWorld();
}

bool HandleBulletPenetration(CSWeaponInfo *wpn_data, FireBulletData &data);

float GetHitgroupDamageMult(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 0.5f;
	case HITGROUP_HEAD:
		return 2.0f;
	case HITGROUP_CHEST:
		return 0.5f;
	case HITGROUP_STOMACH:
		return 0.75f;
	case HITGROUP_LEFTARM:
		return 0.5f;
	case HITGROUP_RIGHTARM:
		return 0.5f;
	case HITGROUP_LEFTLEG:
		return 0.375f;
	case HITGROUP_RIGHTLEG:
		return 0.375f;
	case HITGROUP_GEAR:
		return 0.5f;
	default:
		return 1.0f;

	}

	return 1.0f;
}

void ScaleDamage(int hitgroup, IClientEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMult(hitgroup);

	if (enemy->ArmorValue() > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (enemy->HasHelmet())
				current_damage *= (weapon_armor_ratio);
		}
		else
		{
			current_damage *= (weapon_armor_ratio);
		}
	}
}

bool SimulateFireBullet(IClientEntity *local, CBaseCombatWeapon *weapon, FireBulletData &data)
{
	data.penetrate_count = 4; // Max Amount Of Penitration
	data.trace_length = 0.0f; // wow what a meme
	auto *wpn_data = weapon->GetCSWpnData(); // Get Weapon Info
	data.current_damage = (float)wpn_data->iDamage;// Set Damage Memes
	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = wpn_data->flRange - data.trace_length;
		Vector End_Point = data.src + data.direction * data.trace_length_remaining;
		UTIL_TraceLine(data.src, End_Point, 0x4600400B, local, 0, &data.enter_trace);
		UTIL_ClipTraceToPlayers(data.src, End_Point * 40.f, 0x4600400B, &data.filter, &data.enter_trace);
		if (data.enter_trace.fraction == 1.0f) break;
		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0) && (local->GetTeamNum() != data.enter_trace.m_pEnt->GetTeamNum()))
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow(wpn_data->flRangeModifier, data.trace_length * 0.002);
			ScaleDamage(data.enter_trace.hitgroup, data.enter_trace.m_pEnt, wpn_data->flArmorRatio, data.current_damage);
			return true;
		}
		if (!HandleBulletPenetration(wpn_data, data)) break;
	}
	return false;
}

bool HandleBulletPenetration(CSWeaponInfo *wpn_data, FireBulletData &data)
{
	surfacedata_t *enter_surface_data = Interfaces::PhysProps->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->flRangeModifier, (data.trace_length * 0.002));
	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))data.penetrate_count = 0;
	if (data.penetrate_count <= 0)return false;
	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit)) return false;
	surfacedata_t *exit_surface_data = Interfaces::PhysProps->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;
	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71)) { combined_penetration_modifier = 3.0f; final_damage_modifier = 0.05f; }
	else { combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f; }
	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)combined_penetration_modifier = 2.0f;
	}
	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->flPenetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage)return false;
	if (lost_damage >= 0.0f)data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f) return false;
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}


/*
*    CanHit() - example of how to use the code
*     @in  point: target hitbox vector
*     @out damage_given: amount of damage the shot would do
*/
bool CanHit(const Vector &point, float *damage_given)
{
	//Utils::ToLog("CANHIT");
	auto *local = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	auto data = FireBulletData(local->GetOrigin() + local->GetViewOffset());
	data.filter = CTraceFilter();
	data.filter.pSkip = local;

	Vector angles;
	CalcAngle(data.src, point, angles);
	AngleVectors(angles, &data.direction);
	VectorNormalize(data.direction);

	if (SimulateFireBullet(local, (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle((HANDLE)local->GetActiveWeaponHandle()), data))
	{
		*damage_given = data.current_damage;
		//Utils::ToLog("CANHIT END");
		return true;
	}

	return false;
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ljdktck {
public:
	int sfsveephz;
	bool oxhfsashezlzuph;
	double kctipynu;
	ljdktck();
	string kkstmfnikgyakiztfuhnkge(int bqdyqrknrgnfs, bool stljldq, string wsoiswtpftwaof);
	void qkexmaketzbebkwdod(bool ungtynxwhro, double gpyoxcp, int ggdhzxek, string msqnsmoshfeh, int aucud, int yukfxgmqz, int dlwthmdi, int zymxxq, bool cwdgxsxmborl);
	void gmioasaxhtobsnusjvw(double mktmukyrxsosb, string alfzydhzof, string yloml, int gojsrgtsk, int fpxjtkz, bool daphikuvxqbkp);
	void vezzubnsataidf(string sbxwkzxnu, int dtiokwaym, bool ulbzceepawqyr, int mxcqjl, bool ybwctcxlxyz, bool cgyfp, int xcdxixnltqsvtpb, string hthqsxsf, double imnzjfno, string gqlyths);

protected:
	double fvyficmcis;
	double vfyhsmskoadaxyl;
	double conha;
	double mfhsdxppgaczvol;
	string dqasa;

	double gpsrtpcutcjf(bool wopwsz, string tbpabul, double jatgmtvkoc, double qlujofumty, string tmzkaogxelnkemc, bool hbvhtsfioqmst, double njjrlawxtrpw, double iqjmauezku);
	string wkedpjsjhrbzwbbfwzpnestc(string cpjttldm, int skochkvfkf, double fkcwykrrjuloiel, string qcvmguxktibznfv, string wnrzjdiwippjix, int qijqblyeul, string ovsembpswsdm, int qjsjmbhhph, bool obkskqrcd, string vfdwvbqrqsptj);
	void gdnhkbrfzqbqzjlkzfkbryqlk(int kwvyw, bool hzejjg, string clrewdavjegn, double jqepo, string dwkojjkwfxmq, double iyiwbitp, bool jqanzzyc, int fowukkpzvp, bool jxxvn);
	string etulppjlpawjrcv(bool epnrhppx, string rrlvjoomtnyr, double xcvkxbgg);
	void ncaeaqghclkdgsfrfqxa(double wxfddocc, bool xmvxwowwprd, int axzskldwmq, bool tfmtcrdxqwmriri, bool zixkihifev, string tzrqroixbgwzc);
	double ilmwlckwyrifaodkijxicva(bool ardqxjn);
	void vzfketynhsulhnqar(string kdddeglvoumbx, string nrydpehjihexi, double eezmqwczfztcv, double lsrvab);
	int xbxqbsamlwxsvxaggoe(int yqsbyo, int fgilub);
	bool xrcuzoyppr(double cuzrkcrfmdulmt);
	int nuotkokmyvsotoy();

private:
	double ylhmwpvsm;
	double yutxrnrzkym;
	int enxgbbim;

	bool bocnazwlre(bool wixbyqthwd);
	void qbhncdmnhdlbscziq();
	double wdciabvsqzpsoefibbynupfsj(double ahesxwbg, string fcaxhgqyub, double fmwpsgu);
	string qbddxlzbdmrbfrbe(int nsgfigggbsm, bool pmocj, bool igpajjwmxzkl, bool gmxwgqmyoz, string pzgzeg, double lhmbjvlde, bool yrltz, string ziuhuqxwoxyttp, bool vkvxlzzlysbuew);
	bool nslxwewfxynifxfivjrhti(double eicfludpw, double pskdngj, bool tnvzp, double rbzcmzucozpprf);
	void jpljaktqyrc();
	double ayjkgefflijbqqbgm(string qpvajx, int azddkqtvi, string uorkv, int yurgafatacpr);
	int whrqczruizenkgjrxjoyrsref(double iesnufggzufkq, double grmmztm, string ekwzrpqmowf);
	int ttjwazwmpdgzlaxzogrn(string uyplw, string rbjllhioyewett, int ctchgi, int ocurqx, bool xdcgaqa, int czwsxzziy, int zrzxyaah, bool enxkhsvyqsypmwn, int yuphhnyhcielip, bool usvqjlkrbhwz);
	void mwnncvhptbhib(int ixcwkyzczixqz, int jjxssmglx, double zatwkgpbbivtwtl, bool zhildecmc, int guuumq, string hgzuqq, bool vualees, int vkcobe);

};


bool ljdktck::bocnazwlre(bool wixbyqthwd) {
	double jjfpbk = 57900;
	if (57900 == 57900) {
		int xpdolkkxqc;
		for (xpdolkkxqc = 71; xpdolkkxqc > 0; xpdolkkxqc--) {
			continue;
		}
	}
	if (57900 == 57900) {
		int ii;
		for (ii = 18; ii > 0; ii--) {
			continue;
		}
	}
	return false;
}

void ljdktck::qbhncdmnhdlbscziq() {
	int bckvgtssrseqr = 987;
	bool dqyppr = false;
	double slgnynwuln = 18079;
	string xkspicgmdwiezvp = "bazwvovshqyzsirqdowxsd";
	bool ltcti = false;
	double gdphsp = 65646;
	bool hzfxjo = true;
	string djfytb = "mnbopavcbdspxfgzctlyegvgukhirbxjpphpvgciplnfgspfnlvngflsfhiwjgoprkpvwuwnomoablvgroadofzfffnkflu";
	if (string("mnbopavcbdspxfgzctlyegvgukhirbxjpphpvgciplnfgspfnlvngflsfhiwjgoprkpvwuwnomoablvgroadofzfffnkflu") == string("mnbopavcbdspxfgzctlyegvgukhirbxjpphpvgciplnfgspfnlvngflsfhiwjgoprkpvwuwnomoablvgroadofzfffnkflu")) {
		int zubec;
		for (zubec = 16; zubec > 0; zubec--) {
			continue;
		}
	}

}

double ljdktck::wdciabvsqzpsoefibbynupfsj(double ahesxwbg, string fcaxhgqyub, double fmwpsgu) {
	string ybculik = "ieprlxgznrjbzpnjsalwfkyvtaurdllyqwzyenlaglxtuzyhewqgjhihctainmvfhrxmoueclsnritimigypfhd";
	string eqjclagsibqv = "bkolqwcjdamnbckashqclqqw";
	bool qlxqhzip = false;
	bool cngktxofzqlwtx = true;
	int cbztajxm = 1146;
	string rspkhx = "whbwlbqvlsuscikuvbmyacnfqshfjhejnfafpdmmmzcrduhcuiy";
	int iaifqfy = 3517;
	int nzrtagfz = 1683;
	bool lrxzdmwaioyk = true;
	if (string("ieprlxgznrjbzpnjsalwfkyvtaurdllyqwzyenlaglxtuzyhewqgjhihctainmvfhrxmoueclsnritimigypfhd") != string("ieprlxgznrjbzpnjsalwfkyvtaurdllyqwzyenlaglxtuzyhewqgjhihctainmvfhrxmoueclsnritimigypfhd")) {
		int mfd;
		for (mfd = 69; mfd > 0; mfd--) {
			continue;
		}
	}
	if (1683 == 1683) {
		int kqsvn;
		for (kqsvn = 14; kqsvn > 0; kqsvn--) {
			continue;
		}
	}
	if (string("whbwlbqvlsuscikuvbmyacnfqshfjhejnfafpdmmmzcrduhcuiy") != string("whbwlbqvlsuscikuvbmyacnfqshfjhejnfafpdmmmzcrduhcuiy")) {
		int fjccowe;
		for (fjccowe = 87; fjccowe > 0; fjccowe--) {
			continue;
		}
	}
	if (string("whbwlbqvlsuscikuvbmyacnfqshfjhejnfafpdmmmzcrduhcuiy") == string("whbwlbqvlsuscikuvbmyacnfqshfjhejnfafpdmmmzcrduhcuiy")) {
		int zumgccmhil;
		for (zumgccmhil = 45; zumgccmhil > 0; zumgccmhil--) {
			continue;
		}
	}
	if (string("bkolqwcjdamnbckashqclqqw") == string("bkolqwcjdamnbckashqclqqw")) {
		int iopxfz;
		for (iopxfz = 15; iopxfz > 0; iopxfz--) {
			continue;
		}
	}
	return 45204;
}

string ljdktck::qbddxlzbdmrbfrbe(int nsgfigggbsm, bool pmocj, bool igpajjwmxzkl, bool gmxwgqmyoz, string pzgzeg, double lhmbjvlde, bool yrltz, string ziuhuqxwoxyttp, bool vkvxlzzlysbuew) {
	bool dztfjttao = false;
	int lccjzrmkoo = 9112;
	if (9112 != 9112) {
		int phdhg;
		for (phdhg = 29; phdhg > 0; phdhg--) {
			continue;
		}
	}
	if (9112 != 9112) {
		int xerfvqigpa;
		for (xerfvqigpa = 87; xerfvqigpa > 0; xerfvqigpa--) {
			continue;
		}
	}
	if (9112 == 9112) {
		int qcj;
		for (qcj = 92; qcj > 0; qcj--) {
			continue;
		}
	}
	return string("krdclixdcn");
}

bool ljdktck::nslxwewfxynifxfivjrhti(double eicfludpw, double pskdngj, bool tnvzp, double rbzcmzucozpprf) {
	string xjclapv = "qbswvqvvgcyrfyropyamtqjdmclyfyflorruukrexycxtfmhwuzpqsuksxesqakrwujht";
	if (string("qbswvqvvgcyrfyropyamtqjdmclyfyflorruukrexycxtfmhwuzpqsuksxesqakrwujht") != string("qbswvqvvgcyrfyropyamtqjdmclyfyflorruukrexycxtfmhwuzpqsuksxesqakrwujht")) {
		int kocorae;
		for (kocorae = 56; kocorae > 0; kocorae--) {
			continue;
		}
	}
	return true;
}

void ljdktck::jpljaktqyrc() {
	bool gsquljbcq = true;
	double lrorp = 35660;
	string cptxcnh = "";
	if (35660 == 35660) {
		int yezsudaju;
		for (yezsudaju = 0; yezsudaju > 0; yezsudaju--) {
			continue;
		}
	}
	if (string("") != string("")) {
		int toikkfuvw;
		for (toikkfuvw = 79; toikkfuvw > 0; toikkfuvw--) {
			continue;
		}
	}
	if (string("") == string("")) {
		int vnzduec;
		for (vnzduec = 86; vnzduec > 0; vnzduec--) {
			continue;
		}
	}
	if (35660 == 35660) {
		int sec;
		for (sec = 81; sec > 0; sec--) {
			continue;
		}
	}
	if (true == true) {
		int bdjyyuqwbo;
		for (bdjyyuqwbo = 12; bdjyyuqwbo > 0; bdjyyuqwbo--) {
			continue;
		}
	}

}

double ljdktck::ayjkgefflijbqqbgm(string qpvajx, int azddkqtvi, string uorkv, int yurgafatacpr) {
	string yszlxfqsg = "ngmnoekegcxmqqztzyqnypixdyhxllvsqkcgsgxnqoftvkvajyfhsrrwdazqvmmdhmrxdbaaxhhsmrucmnbafvxcvzkvoivm";
	string pwleed = "hueavlctdeledoalhbxzvmlwmtzocbqeo";
	if (string("ngmnoekegcxmqqztzyqnypixdyhxllvsqkcgsgxnqoftvkvajyfhsrrwdazqvmmdhmrxdbaaxhhsmrucmnbafvxcvzkvoivm") == string("ngmnoekegcxmqqztzyqnypixdyhxllvsqkcgsgxnqoftvkvajyfhsrrwdazqvmmdhmrxdbaaxhhsmrucmnbafvxcvzkvoivm")) {
		int zbsk;
		for (zbsk = 8; zbsk > 0; zbsk--) {
			continue;
		}
	}
	if (string("hueavlctdeledoalhbxzvmlwmtzocbqeo") == string("hueavlctdeledoalhbxzvmlwmtzocbqeo")) {
		int tzwimku;
		for (tzwimku = 79; tzwimku > 0; tzwimku--) {
			continue;
		}
	}
	if (string("hueavlctdeledoalhbxzvmlwmtzocbqeo") != string("hueavlctdeledoalhbxzvmlwmtzocbqeo")) {
		int tlmpfo;
		for (tlmpfo = 88; tlmpfo > 0; tlmpfo--) {
			continue;
		}
	}
	if (string("hueavlctdeledoalhbxzvmlwmtzocbqeo") != string("hueavlctdeledoalhbxzvmlwmtzocbqeo")) {
		int vvvvq;
		for (vvvvq = 18; vvvvq > 0; vvvvq--) {
			continue;
		}
	}
	return 23949;
}

int ljdktck::whrqczruizenkgjrxjoyrsref(double iesnufggzufkq, double grmmztm, string ekwzrpqmowf) {
	string evososazwmur = "vmjghmizfzjeeorvarydlofxduft";
	double hidfvlnncdnnrmf = 54823;
	int fzkrczd = 983;
	bool flelmbcmvvrur = true;
	string yeltzuzua = "nugpkhlgdfxyrkefnhctumggvwplwsbvafwhptqzbemtlqdirusgjfadukapaprwabqpmnljxpdnbzfnydcegndobyqyhks";
	if (string("nugpkhlgdfxyrkefnhctumggvwplwsbvafwhptqzbemtlqdirusgjfadukapaprwabqpmnljxpdnbzfnydcegndobyqyhks") == string("nugpkhlgdfxyrkefnhctumggvwplwsbvafwhptqzbemtlqdirusgjfadukapaprwabqpmnljxpdnbzfnydcegndobyqyhks")) {
		int fdlxirndi;
		for (fdlxirndi = 87; fdlxirndi > 0; fdlxirndi--) {
			continue;
		}
	}
	if (string("vmjghmizfzjeeorvarydlofxduft") == string("vmjghmizfzjeeorvarydlofxduft")) {
		int omxzmzicz;
		for (omxzmzicz = 46; omxzmzicz > 0; omxzmzicz--) {
			continue;
		}
	}
	if (983 == 983) {
		int yjahq;
		for (yjahq = 20; yjahq > 0; yjahq--) {
			continue;
		}
	}
	if (true == true) {
		int av;
		for (av = 29; av > 0; av--) {
			continue;
		}
	}
	return 69291;
}

int ljdktck::ttjwazwmpdgzlaxzogrn(string uyplw, string rbjllhioyewett, int ctchgi, int ocurqx, bool xdcgaqa, int czwsxzziy, int zrzxyaah, bool enxkhsvyqsypmwn, int yuphhnyhcielip, bool usvqjlkrbhwz) {
	return 66236;
}

void ljdktck::mwnncvhptbhib(int ixcwkyzczixqz, int jjxssmglx, double zatwkgpbbivtwtl, bool zhildecmc, int guuumq, string hgzuqq, bool vualees, int vkcobe) {
	string vdntxfsnpif = "hnzoytcoks";
	int pseeruo = 897;
	bool cbjikmglqcz = true;
	if (true != true) {
		int shyu;
		for (shyu = 19; shyu > 0; shyu--) {
			continue;
		}
	}

}

double ljdktck::gpsrtpcutcjf(bool wopwsz, string tbpabul, double jatgmtvkoc, double qlujofumty, string tmzkaogxelnkemc, bool hbvhtsfioqmst, double njjrlawxtrpw, double iqjmauezku) {
	double vczhnhexmkwb = 5602;
	if (5602 == 5602) {
		int ujdoo;
		for (ujdoo = 71; ujdoo > 0; ujdoo--) {
			continue;
		}
	}
	if (5602 == 5602) {
		int uooelv;
		for (uooelv = 68; uooelv > 0; uooelv--) {
			continue;
		}
	}
	if (5602 != 5602) {
		int biphuapa;
		for (biphuapa = 3; biphuapa > 0; biphuapa--) {
			continue;
		}
	}
	if (5602 == 5602) {
		int ktkrnmdrv;
		for (ktkrnmdrv = 50; ktkrnmdrv > 0; ktkrnmdrv--) {
			continue;
		}
	}
	return 7629;
}

string ljdktck::wkedpjsjhrbzwbbfwzpnestc(string cpjttldm, int skochkvfkf, double fkcwykrrjuloiel, string qcvmguxktibznfv, string wnrzjdiwippjix, int qijqblyeul, string ovsembpswsdm, int qjsjmbhhph, bool obkskqrcd, string vfdwvbqrqsptj) {
	bool nvkqc = true;
	int pslnunnwtcyza = 475;
	string svppomojhqwqg = "zzqybomdkzhzhieedphhenhsjrrhrecjahgwxykgsikrsqdzfumxmnnqqbadionedghbnhucdtfdbjafcklg";
	bool gllsgn = false;
	if (false == false) {
		int rppqpeb;
		for (rppqpeb = 78; rppqpeb > 0; rppqpeb--) {
			continue;
		}
	}
	if (false == false) {
		int smgncjhn;
		for (smgncjhn = 13; smgncjhn > 0; smgncjhn--) {
			continue;
		}
	}
	if (true == true) {
		int oqaelly;
		for (oqaelly = 71; oqaelly > 0; oqaelly--) {
			continue;
		}
	}
	if (true != true) {
		int bfggpxw;
		for (bfggpxw = 9; bfggpxw > 0; bfggpxw--) {
			continue;
		}
	}
	if (true != true) {
		int vmnthp;
		for (vmnthp = 48; vmnthp > 0; vmnthp--) {
			continue;
		}
	}
	return string("rrfxqupwlfcgawtpe");
}

void ljdktck::gdnhkbrfzqbqzjlkzfkbryqlk(int kwvyw, bool hzejjg, string clrewdavjegn, double jqepo, string dwkojjkwfxmq, double iyiwbitp, bool jqanzzyc, int fowukkpzvp, bool jxxvn) {

}

string ljdktck::etulppjlpawjrcv(bool epnrhppx, string rrlvjoomtnyr, double xcvkxbgg) {
	string kgsqwzxzlwydgut = "ffuxqxjkboqoxwzbhfsitiuiwhissgchgpuwsfqgmzgppgixnccitkkwefwzgizxtwcqfibddvuzlkb";
	int autkenim = 3682;
	int difqkvsurym = 6023;
	int whfka = 2722;
	double zqafpihroesl = 18729;
	bool wzoiynkvyfzbi = true;
	double ahruryvk = 3642;
	bool dayrvdmmqii = false;
	if (string("ffuxqxjkboqoxwzbhfsitiuiwhissgchgpuwsfqgmzgppgixnccitkkwefwzgizxtwcqfibddvuzlkb") != string("ffuxqxjkboqoxwzbhfsitiuiwhissgchgpuwsfqgmzgppgixnccitkkwefwzgizxtwcqfibddvuzlkb")) {
		int hfdsjqd;
		for (hfdsjqd = 89; hfdsjqd > 0; hfdsjqd--) {
			continue;
		}
	}
	if (18729 == 18729) {
		int tknse;
		for (tknse = 74; tknse > 0; tknse--) {
			continue;
		}
	}
	if (6023 != 6023) {
		int vdjngitri;
		for (vdjngitri = 28; vdjngitri > 0; vdjngitri--) {
			continue;
		}
	}
	if (18729 == 18729) {
		int aipltgn;
		for (aipltgn = 11; aipltgn > 0; aipltgn--) {
			continue;
		}
	}
	if (3682 != 3682) {
		int rjq;
		for (rjq = 62; rjq > 0; rjq--) {
			continue;
		}
	}
	return string("jmijxwyxlonsvlyolni");
}

void ljdktck::ncaeaqghclkdgsfrfqxa(double wxfddocc, bool xmvxwowwprd, int axzskldwmq, bool tfmtcrdxqwmriri, bool zixkihifev, string tzrqroixbgwzc) {
	string xyxbvksaftmnp = "buzjiwpxbqaibunxfqnwyf";
	if (string("buzjiwpxbqaibunxfqnwyf") == string("buzjiwpxbqaibunxfqnwyf")) {
		int nr;
		for (nr = 36; nr > 0; nr--) {
			continue;
		}
	}
	if (string("buzjiwpxbqaibunxfqnwyf") == string("buzjiwpxbqaibunxfqnwyf")) {
		int fduex;
		for (fduex = 32; fduex > 0; fduex--) {
			continue;
		}
	}
	if (string("buzjiwpxbqaibunxfqnwyf") == string("buzjiwpxbqaibunxfqnwyf")) {
		int xagev;
		for (xagev = 80; xagev > 0; xagev--) {
			continue;
		}
	}
	if (string("buzjiwpxbqaibunxfqnwyf") != string("buzjiwpxbqaibunxfqnwyf")) {
		int hgc;
		for (hgc = 87; hgc > 0; hgc--) {
			continue;
		}
	}
	if (string("buzjiwpxbqaibunxfqnwyf") == string("buzjiwpxbqaibunxfqnwyf")) {
		int ajwa;
		for (ajwa = 1; ajwa > 0; ajwa--) {
			continue;
		}
	}

}

double ljdktck::ilmwlckwyrifaodkijxicva(bool ardqxjn) {
	int eqwzx = 4720;
	bool epctrfpedhzpez = false;
	double uucofmowtiz = 31739;
	int amzmarqefxamz = 2465;
	int qmsuvrgw = 4840;
	bool pqkmkllnjfqwkn = true;
	double wylsp = 10499;
	return 15078;
}

void ljdktck::vzfketynhsulhnqar(string kdddeglvoumbx, string nrydpehjihexi, double eezmqwczfztcv, double lsrvab) {

}

int ljdktck::xbxqbsamlwxsvxaggoe(int yqsbyo, int fgilub) {
	double itjabwernenfq = 35761;
	bool cykgjn = true;
	double dekvni = 12180;
	double ofmcukeliyzyr = 28706;
	string hktzha = "paqtazvxurqnugnxdsowdejucnvkydetlwfjrtguljdnwxjdbxlwriudopzsnngggdt";
	string auparaxkewghwvt = "fxjtlldgpssutiuewotojhuuycwkhzpztmitolrbkitlyzcnezipcwjwrtbcjbsgrzgydkklhpeltnpitruiaapluajkqwif";
	bool tjjpaltjkzirri = false;
	bool tkskkqj = false;
	int xbmrdvn = 5996;
	string tkabjuvsklzzc = "pzwvtjuhycgwwzgyzelrdkfumcvdhkkhqoykclobqevwewprlvbyizfffgpofvrniqnulq";
	if (5996 == 5996) {
		int mfhlq;
		for (mfhlq = 27; mfhlq > 0; mfhlq--) {
			continue;
		}
	}
	if (5996 == 5996) {
		int fpc;
		for (fpc = 67; fpc > 0; fpc--) {
			continue;
		}
	}
	if (12180 != 12180) {
		int qemnn;
		for (qemnn = 46; qemnn > 0; qemnn--) {
			continue;
		}
	}
	return 72063;
}

bool ljdktck::xrcuzoyppr(double cuzrkcrfmdulmt) {
	double pzfursr = 35549;
	bool ghndyjxlkqhtmi = true;
	bool rgpohknvgz = false;
	int cgjty = 2065;
	int mgtozxzcey = 949;
	string lhodxwuk = "puggnumymcsmbkjsmmaibnmkwljpljsjjeozrzngfr";
	int objjafvxvut = 1957;
	if (949 != 949) {
		int arcsi;
		for (arcsi = 4; arcsi > 0; arcsi--) {
			continue;
		}
	}
	return true;
}

int ljdktck::nuotkokmyvsotoy() {
	double vyvadwuutuvja = 25580;
	string pjvxrqxv = "ogdrqidhtwizsqtnaalbpokkarbutqctorptzrwjgefyrsksifhnojvwnsti";
	string lzhdauhyztg = "fgtrdajiyzznfruhkezjhch";
	double wtgth = 18377;
	string aavraaswzlsa = "pvbzwpegukhymsfftleqbhamsecamxxurwzhyjeaexcfzddpkhxutcqjalemljqohh";
	string nwugcbgl = "zzbhuxdumncltvbquizwyczskqe";
	int sjewcyfyr = 1175;
	string zsvwpktnfm = "gfmzgifttovnaeyjwuisnowrnlttgwcqxhbjbyzf";
	double sxdeapwjddqff = 56246;
	if (string("ogdrqidhtwizsqtnaalbpokkarbutqctorptzrwjgefyrsksifhnojvwnsti") == string("ogdrqidhtwizsqtnaalbpokkarbutqctorptzrwjgefyrsksifhnojvwnsti")) {
		int lz;
		for (lz = 54; lz > 0; lz--) {
			continue;
		}
	}
	if (string("zzbhuxdumncltvbquizwyczskqe") == string("zzbhuxdumncltvbquizwyczskqe")) {
		int orfom;
		for (orfom = 17; orfom > 0; orfom--) {
			continue;
		}
	}
	if (string("fgtrdajiyzznfruhkezjhch") != string("fgtrdajiyzznfruhkezjhch")) {
		int ryiudle;
		for (ryiudle = 9; ryiudle > 0; ryiudle--) {
			continue;
		}
	}
	return 3384;
}

string ljdktck::kkstmfnikgyakiztfuhnkge(int bqdyqrknrgnfs, bool stljldq, string wsoiswtpftwaof) {
	double waiblq = 14813;
	bool cynffk = false;
	int egmziia = 673;
	int cnzkezokjumiiiz = 1084;
	bool ohcfopsdtn = false;
	bool yzkqrzv = true;
	int zhjrlxqhdns = 2963;
	return string("jqmxnodmqhllklbrgj");
}

void ljdktck::qkexmaketzbebkwdod(bool ungtynxwhro, double gpyoxcp, int ggdhzxek, string msqnsmoshfeh, int aucud, int yukfxgmqz, int dlwthmdi, int zymxxq, bool cwdgxsxmborl) {
	bool fxbkqakdkf = false;
	double cnlsagitbho = 49752;
	bool kxrrxclshd = true;
	double scwyecz = 33651;
	double ryqkqbndcuz = 64809;
	bool rxvwiklqkcq = true;
	string avjjreb = "angapauoqblhhdyeisnkrckixgiqywjfyvuwinugaopvzhckopeqdorwqpsyvykilbaevyhoryafbblsgmzjwhaeotbqmsbfh";
	bool jhnlgldge = true;
	if (false != false) {
		int laynza;
		for (laynza = 60; laynza > 0; laynza--) {
			continue;
		}
	}
	if (33651 != 33651) {
		int qzavwb;
		for (qzavwb = 56; qzavwb > 0; qzavwb--) {
			continue;
		}
	}
	if (64809 == 64809) {
		int gcth;
		for (gcth = 64; gcth > 0; gcth--) {
			continue;
		}
	}
	if (true == true) {
		int casmdfj;
		for (casmdfj = 53; casmdfj > 0; casmdfj--) {
			continue;
		}
	}
	if (string("angapauoqblhhdyeisnkrckixgiqywjfyvuwinugaopvzhckopeqdorwqpsyvykilbaevyhoryafbblsgmzjwhaeotbqmsbfh") != string("angapauoqblhhdyeisnkrckixgiqywjfyvuwinugaopvzhckopeqdorwqpsyvykilbaevyhoryafbblsgmzjwhaeotbqmsbfh")) {
		int cpu;
		for (cpu = 80; cpu > 0; cpu--) {
			continue;
		}
	}

}

void ljdktck::gmioasaxhtobsnusjvw(double mktmukyrxsosb, string alfzydhzof, string yloml, int gojsrgtsk, int fpxjtkz, bool daphikuvxqbkp) {
	string prjqybikpwfv = "xhrqezrjzrtqyyzuyxtscxzikzvnqcwukszqpdikxlnqqckbpupgwdummqbjmbaikpawxusjspjivxseofchxddruza";
	double jkgmgmtisiyu = 13421;
	string wdtjqkv = "wteqrdwnmfheajqijwegoovrmhwrohjvrti";
	int yncaeivfmw = 2013;
	double qbipsooi = 26396;
	if (string("wteqrdwnmfheajqijwegoovrmhwrohjvrti") == string("wteqrdwnmfheajqijwegoovrmhwrohjvrti")) {
		int ppaoi;
		for (ppaoi = 23; ppaoi > 0; ppaoi--) {
			continue;
		}
	}
	if (string("xhrqezrjzrtqyyzuyxtscxzikzvnqcwukszqpdikxlnqqckbpupgwdummqbjmbaikpawxusjspjivxseofchxddruza") != string("xhrqezrjzrtqyyzuyxtscxzikzvnqcwukszqpdikxlnqqckbpupgwdummqbjmbaikpawxusjspjivxseofchxddruza")) {
		int kvpzockw;
		for (kvpzockw = 7; kvpzockw > 0; kvpzockw--) {
			continue;
		}
	}
	if (26396 == 26396) {
		int ixbvlvt;
		for (ixbvlvt = 10; ixbvlvt > 0; ixbvlvt--) {
			continue;
		}
	}
	if (string("wteqrdwnmfheajqijwegoovrmhwrohjvrti") != string("wteqrdwnmfheajqijwegoovrmhwrohjvrti")) {
		int yv;
		for (yv = 31; yv > 0; yv--) {
			continue;
		}
	}
	if (26396 != 26396) {
		int hjnl;
		for (hjnl = 24; hjnl > 0; hjnl--) {
			continue;
		}
	}

}

void ljdktck::vezzubnsataidf(string sbxwkzxnu, int dtiokwaym, bool ulbzceepawqyr, int mxcqjl, bool ybwctcxlxyz, bool cgyfp, int xcdxixnltqsvtpb, string hthqsxsf, double imnzjfno, string gqlyths) {

}

ljdktck::ljdktck() {
	this->kkstmfnikgyakiztfuhnkge(2835, true, string("nrtdedtdulmchzolza"));
	this->qkexmaketzbebkwdod(true, 9683, 2665, string("cdxdybolnrathusosugjnrotvwufvbfyudzejkbvfsnvfpqhuusoxoxjoxtefrikpgytq"), 258, 1881, 2882, 1247, false);
	this->gmioasaxhtobsnusjvw(6098, string("ypwdwqwgbfkzhyjzvjrfsuzlcqaflkkezhzeswfhhzycopctbop"), string("layaktyuucslqjvjlrmtilhaopgquyljedamqyivcmtskab"), 1320, 4362, true);
	this->vezzubnsataidf(string("drhyclxzckpdhivvqrduiujawyuqsrujmryranxda"), 3130, false, 3542, false, false, 167, string("wiizyjxdrl"), 15104, string("hpfbzoojajjjlqksydvawjgcxuvafkbizmrehekbhjbljvedivjpazbfjqbkvehewequfmzrchvpiqwqxblawjeebjcsxru"));
	this->gpsrtpcutcjf(true, string("wihgrwewwcvnxlfvzgqbxmqpmtfqveilqotqaj"), 2255, 14133, string("ykljgwprgxfjaguzyqihytlyyfvvlihdnfykmxehjfilvnhzycdlvzehunoehlnuuzvvbevxmvykgyvq"), true, 15512, 10544);
	this->wkedpjsjhrbzwbbfwzpnestc(string("kthywgounvwwtrnewpxjlgshnfenkmgmvgcsoqyeoequmlnhwhovybzfwovxxuxjimnciahtdlxbybomymzcavdbuksplcssnk"), 1433, 4187, string("bctywpnklgtnxtppqkwualpjkiricnxcvhklpolhswnzsoxluzvdpdrlmcnpxflsiri"), string("fmmfqkkfsofwluofybrcayjqojkjwkfxxgijntmtkaqwatmrrysuhuqhrvzjfycewfmafvabtuykl"), 4525, string("cgbubqgmfuuupmvcijahbkpv"), 232, false, string("pjfiypfzhcyiynlaigxcklkmlrzroeypavbeuwwjrvmiuhloeyvuqnzg"));
	this->gdnhkbrfzqbqzjlkzfkbryqlk(949, true, string("gptuepvndrtibhxjybywtpigogdk"), 37955, string("tzhiihzniakklfjkkumkuyeuphlaqoradvpuokweiesqunrnujrnvememxylxui"), 20244, false, 2265, false);
	this->etulppjlpawjrcv(true, string("klexzvmkkgaweadipbzpytomozemstixjeqxixkowiqbpumvngzymzidxvylgpeccaernwyansqcscogodrsjzx"), 12288);
	this->ncaeaqghclkdgsfrfqxa(64528, true, 332, false, false, string("dppwoxhfbmumqlnenrteurtjfswmsjljjomcoqxtxqqorwcpozvzgpiyruqlbppfpswggwmabxcuaamhifkwodvuczr"));
	this->ilmwlckwyrifaodkijxicva(false);
	this->vzfketynhsulhnqar(string("iiieduqnmbxlzdahudpinxfdiahuofxodjeuswycnbtczfnkygyukpkdhsycluuxspyezrvqusyoffmtvetmadqymz"), string("vymdkehwxollityo"), 39798, 19281);
	this->xbxqbsamlwxsvxaggoe(103, 32);
	this->xrcuzoyppr(46106);
	this->nuotkokmyvsotoy();
	this->bocnazwlre(false);
	this->qbhncdmnhdlbscziq();
	this->wdciabvsqzpsoefibbynupfsj(65999, string("ekdijedarcyoiaxczxh"), 73998);
	this->qbddxlzbdmrbfrbe(7748, true, false, true, string("krxfls"), 6701, false, string("gmnoaybbcupditkqonjosxhjvuxvsdgvqomrtjmigcfqccdbbsqqrgdhffxzkabajkyhumasvrepf"), false);
	this->nslxwewfxynifxfivjrhti(33669, 42514, true, 20893);
	this->jpljaktqyrc();
	this->ayjkgefflijbqqbgm(string("ugeiuezphrvovhwhmsxqnjcsxpcvuksqyyfadujqpkmilouafdcndlyk"), 2648, string("flvxvtxpwydrrhpcmasjrbsokhgpmhhtpeoljnflhbdqewnlmglddxzpwfaoemwjciwusuygncmlew"), 1388);
	this->whrqczruizenkgjrxjoyrsref(45012, 52229, string("tvuegoorsgapzoxujyzsrjnujlqbweqtqvdvhmmlghksrpxghvokyynzyga"));
	this->ttjwazwmpdgzlaxzogrn(string("xzdmmzgygfctijwyevvwqlpxatwvbvbtlqpdrvrvyl"), string("sshjyfjnyxmvxhuwubbaapfcgysokjyhddsehwqxynlq"), 4574, 38, true, 1531, 5232, false, 4883, true);
	this->mwnncvhptbhib(1726, 449, 45292, true, 3715, string("fougysklyhavmjjjbomljscdkwyprmmhihtwqycxufsgnqcxiwedjxotvgcuoczekuiytruuhosftfedybbopjzqzr"), false, 552);
}
