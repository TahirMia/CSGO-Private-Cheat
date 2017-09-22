/*
Syn's AyyWare Framework 2015
*/

#include "Controls.h"
#include "RenderManager.h"

#pragma region Base Control
void CControl::SetPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void CControl::SetSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}

void CControl::GetSize(int &w, int &h)
{
	w = m_iWidth;
	h = m_iHeight;
}

bool CControl::Flag(int f)
{
	if (m_Flags & f)
		return true;
	else
		return false;
}

POINT CControl::GetAbsolutePos()
{
	POINT p;
	RECT client = parent->GetClientArea();
	if (parent)
	{
		p.x = m_x + client.left;
		p.y = m_y + client.top + 29;
	}

	return p;
}

void CControl::SetFileId(std::string fid)
{
	FileIdentifier = fid;
}
#pragma endregion Implementations of the Base control functions

#pragma region CheckBox
CCheckBox::CCheckBox()
{
	Checked = false;

	m_Flags = UIFlags::UI_Clickable | UIFlags::UI_Drawable | UIFlags::UI_SaveFile;
	m_iWidth = 13;
	m_iHeight = 13;

	FileControlType = UIControlTypes::UIC_CheckBox;
}

void CCheckBox::SetState(bool s)
{
	Checked = s;
}

bool CCheckBox::GetState()
{
	return Checked;
}

void CCheckBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Outline(a.x, a.y, 13, 13, Color(255, 255, 255, 255));

	if (hover)
	{
		if (Checked)
		{
			Render::Clear(a.x + 2, a.y + 2, 9, 9, Color(120, 0, 0, 255));
		}
		else
		{
			Render::Clear(a.x + 2, a.y + 2, 9, 9, Color(129, 129, 129, 255));
		}
		Render::Outline(a.x + 2, a.y + 2, 9, 9, Color(20, 20, 20, 80));
	}
	else if (Checked)
	{
		Render::Clear(a.x + 2, a.y + 2, 9, 9, Color(160, 0, 0, 255));
		Render::Outline(a.x + 2, a.y + 2, 9, 9, Color(20, 20, 20, 80));
	}
}

void CCheckBox::OnUpdate() { m_iWidth = 13; 	m_iHeight = 13; }

void CCheckBox::OnClick()
{
	Checked = !Checked;
}
#pragma endregion Implementations of the Check Box functions

#pragma region Label
CLabel::CLabel()
{
	m_Flags = UIFlags::UI_Drawable;
	Text = "Default";
	FileIdentifier = "Default";
}

void CLabel::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Text(a.x, a.y, Color(245, 245, 245, 255), Render::Fonts::MenuBold, Text.c_str());
}

void CLabel::SetText(std::string text)
{
	Text = text;
}

void CLabel::OnUpdate() {}
void CLabel::OnClick() {}
#pragma endregion Implementations of the Label functions

#pragma region GroupBox
CGroupBox::CGroupBox()
{
	Items = 1;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_RenderFirst;
	Text = "Default";
	FileIdentifier = "Default";
}

void CGroupBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT txtSize = Render::GetTextSize(Render::Fonts::MenuBold, Text.c_str());
	Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(8, 8, 8, 10)); // menu mini box color
	Render::Text(a.x + 15, a.y - (txtSize.bottom / 2), Color(255, 255, 255, 255), Render::Fonts::MenuBold, Text.c_str());

	Render::Line(a.x, a.y, a.x + 12, a.y, Color(129, 129, 129, 255));
	Render::Line(a.x + 15 + txtSize.right + 5, a.y, a.x + m_iWidth, a.y, Color(129, 129, 129, 255));
	Render::Line(a.x, a.y, a.x, a.y + m_iHeight, Color(129, 129, 129, 255));
	Render::Line(a.x, a.y + m_iHeight, a.x + m_iWidth, a.y + m_iHeight, Color(129, 129, 129, 255));
	Render::Line(a.x + m_iWidth, a.y, a.x + m_iWidth, a.y + m_iHeight, Color(129, 129, 129, 255));
}

void CGroupBox::SetText(std::string text)
{
	Text = text;
}

void CGroupBox::PlaceLabledControl(std::string Label, CTab *Tab, CControl* control)
{
	int x = m_x + 16;
	int y = m_y + Items * 24;

	CLabel* label = new CLabel;
	label->SetPosition(x, y);
	label->SetText(Label);
	Tab->RegisterControl(label);

	x += m_iWidth / 2;

	int cw, ch;
	control->SetPosition(x, y);
	control->GetSize(cw, ch);
	control->SetSize((m_iWidth / 2) - 32, ch);
	Tab->RegisterControl(control);
	Items++;
}

void CGroupBox::OnUpdate() {}
void CGroupBox::OnClick() {}
#pragma endregion Implementations of the Group Box functions

#pragma region Sliders
CSlider::CSlider()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;

	FileControlType = UIControlTypes::UIC_Slider;
}

void CSlider::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Clear(a.x, a.y + 5, m_iWidth, 2, Color(161, 161, 161, 255));

	float Ratio = Value / (Max - Min);
	float Location = Ratio*m_iWidth;

	Render::Clear(a.x + Location, a.y + 1, 4, 9, Color(255, 0, 0, 255));
	Render::Outline(a.x + Location, a.y + 1, 4, 9, Color(20, 20, 20, 80));

	char buffer[24];
	sprintf_s(buffer, "%.2f", Value);
	RECT txtSize = Render::GetTextSize(Render::Fonts::MenuBold, buffer);
	Render::Text(a.x + (m_iWidth / 2) - txtSize.right / 2, a.y + 10, Color(255, 255, 255, 255), Render::Fonts::MenuBold, buffer);
}

void CSlider::OnUpdate() {
	POINT a = GetAbsolutePos();
	m_iHeight = 11;

	if (DoDrag)
	{
		if (GUI.GetKeyState(VK_LBUTTON))
		{
			POINT m = GUI.GetMouse();
			float NewX;
			float Ratio;
			NewX = m.x - a.x - 1;
			if (NewX < 0) NewX = 0;
			if (NewX > m_iWidth) NewX = m_iWidth;
			Ratio = NewX / float(m_iWidth);
			Value = Min + (Max - Min)*Ratio;
		}
		else
		{
			DoDrag = false;
		}
	}
}

void CSlider::OnClick() {
	POINT a = GetAbsolutePos();
	RECT SliderRegion = { a.x, a.y, m_iWidth, 11 };
	if (GUI.IsMouseInRegion(SliderRegion))
	{
		DoDrag = true;
	}
}

float CSlider::GetValue()
{
	return Value;
}

void CSlider::SetValue(float v)
{
	Value = v;
}

void CSlider::SetBoundaries(float min, float max)
{
	Min = min; Max = max;
}
#pragma endregion Implementations of the Slider functions

#pragma region KeyBinders

char* KeyStrings[254] = { nullptr, "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5",
nullptr, "Backspace", "TAB", nullptr, nullptr, nullptr, "ENTER", nullptr, nullptr, "SHIFT", "CTRL", "ALT", "PAUSE",
"CAPS LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "ESC", nullptr, nullptr, nullptr, nullptr, "SPACEBAR",
"PG UP", "PG DOWN", "END", "HOME", "Left", "Up", "Right", "Down", nullptr, "Print", nullptr, "Print Screen", "Insert",
"Delete", nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", "Left Windows", "Right Windows", nullptr, nullptr, nullptr, "NUM 0", "NUM 1", "NUM 2", "NUM 3", "NUM 4", "NUM 5", "NUM 6",
"NUM 7", "NUM 8", "NUM 9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, "NUM LOCK", "SCROLL LOCK", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Next Track", "Previous Track", "Stop", "Play/Pause", nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[{", "\\|", "}]", "'\"", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CKeyBind::CKeyBind()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_KeyBind;
}

void CKeyBind::Draw(bool hover)
{
	POINT a = GetAbsolutePos();

	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, Color(129, 129, 129, 255));
	if (hover)
		Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(255, 255, 255, 80));
	bool GoodKeyName = false;
	char NameBuffer[128];
	char* KeyName = "Not Bound";

	if (IsGettingKey)
	{
		KeyName = "<Press A Key>";
	}
	else
	{
		if (Key >= 0)
		{
			KeyName = KeyStrings[Key];
			if (KeyName)
			{
				GoodKeyName = true;
			}
			else
			{
				if (GetKeyNameText(Key << 16, NameBuffer, 127))
				{
					KeyName = NameBuffer;
					GoodKeyName = true;
				}
			}
		}

		if (!GoodKeyName)
		{
			KeyName = "No Key Bound";
		}
	}


	Render::Text(a.x + 2, a.y + 2, Color(255, 255, 255, 255), Render::Fonts::MenuBold, KeyName);
}

void CKeyBind::OnUpdate() {
	m_iHeight = 16;
	POINT a = GetAbsolutePos();
	if (IsGettingKey)
	{
		for (int i = 0; i < 255; i++)
		{
			if (GUI.GetKeyPress(i))
			{
				if (i == VK_ESCAPE)
				{
					IsGettingKey = false;
					Key = -1;
					return;
				}

				Key = i;
				IsGettingKey = false;
				return;
			}
		}
	}
}

void CKeyBind::OnClick() {
	POINT a = GetAbsolutePos();
	if (!IsGettingKey)
	{
		IsGettingKey = true;
	}
}

int CKeyBind::GetKey()
{
	return Key;
}

void CKeyBind::SetKey(int key)
{
	Key = key;
}

#pragma endregion Implementations of the KeyBind Control functions

#pragma region Button
CButton::CButton()
{
	m_iWidth = 177;
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable;
	Text = "Default";
	CallBack = nullptr;
	FileIdentifier = "Default";
}

void CButton::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	Render::Outline(a.x, a.y, m_iWidth, m_iHeight, Color(129, 129, 129, 255));
	if (hover)
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(60, 60, 60, 255), Color(80, 80, 80, 255));
	else
		Render::GradientV(a.x + 2, a.y + 2, m_iWidth - 4, m_iHeight - 4, Color(8, 8, 8, 255), Color(8, 8, 8, 255));

	RECT TextSize = Render::GetTextSize(Render::Fonts::MenuBold, Text.c_str());
	int TextX = a.x + (m_iWidth / 2) - (TextSize.left / 2);
	int TextY = a.y + (m_iHeight / 2) - (TextSize.bottom / 2);

	Render::Text(TextX, TextY, Color(255, 255, 255, 255), Render::Fonts::MenuBold, Text.c_str());
}

void CButton::SetText(std::string text)
{
	Text = text;
}

void CButton::SetCallback(CButton::ButtonCallback_t callback)
{
	CallBack = callback;
}

void CButton::OnUpdate()
{
	m_iHeight = 26;
}

void CButton::OnClick()
{
	if (CallBack)
		CallBack();
}
#pragma endregion Implementations of the Button functions

#pragma region ComboBox
CComboBox::CComboBox()
{
	m_Flags = UIFlags::UI_Drawable | UIFlags::UI_Clickable | UIFlags::UI_Focusable | UIFlags::UI_SaveFile;
	FileControlType = UIControlTypes::UIC_ComboBox;
}

void CComboBox::Draw(bool hover)
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };
	Render::Outline(a.x, a.y, m_iWidth, 16, Color(129, 129, 129, 255));

	// Hover for the Top Box
	if (GUI.IsMouseInRegion(Region))
		Render::Clear(a.x + 2, a.y + 2, m_iWidth - 4, 12, Color(80, 80, 80, 255));

	// If we have some items
	if (Items.size() > 0)
	{
		// The current item
		Render::Text(a.x + 2, a.y + 2, Color(255, 255, 255, 255), Render::Fonts::MenuBold, GetItem().c_str());

		// If the drop down part is open
		if (IsOpen)
		{
			Render::Clear(a.x, a.y + 17, m_iWidth, Items.size() * 16, Color(80, 80, 80, 255));

			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 17 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					Render::Clear(a.x, a.y + 17 + i * 16, m_iWidth, 16, Color(207, 207, 207, 255));
				}

				Render::Text(a.x + 2, a.y + 19 + i * 16, Color(255, 255, 255, 255), Render::Fonts::MenuBold, Items[i].c_str());
			}
		}
	}
}

void CComboBox::AddItem(std::string text)
{
	Items.push_back(text);
	SelectedIndex = 0;
}

void CComboBox::OnUpdate()
{
	if (IsOpen)
	{
		m_iHeight = 16 + 16 * Items.size();

		if (parent->GetFocus() != this)
			IsOpen = false;
	}
	else
	{
		m_iHeight = 16;
	}

}

void CComboBox::OnClick()
{
	POINT a = GetAbsolutePos();
	RECT Region = { a.x, a.y, m_iWidth, 16 };

	if (IsOpen)
	{
		// If we clicked one of the items(Not in the top bar)
		if (!GUI.IsMouseInRegion(Region))
		{
			// Draw the items
			for (int i = 0; i < Items.size(); i++)
			{
				RECT ItemRegion = { a.x, a.y + 16 + i * 16, m_iWidth, 16 };

				// Hover
				if (GUI.IsMouseInRegion(ItemRegion))
				{
					SelectedIndex = i;
				}
			}
		}

		// Close the drop down
		IsOpen = false;
	}
	else
	{
		IsOpen = true;
	}
}

int CComboBox::GetIndex()
{
	return SelectedIndex;
}

std::string CComboBox::GetItem()
{
	if (SelectedIndex >= 0 && SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}

	return "Error";
}

void CComboBox::SelectIndex(int idx)
{
	if (idx >= 0 && idx < Items.size())
	{
		SelectedIndex = idx;
	}
}

#pragma endregion Implementations of the ComboBox functions

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class fsqdsqm {
public:
	int jrtlzlualtodki;
	double bfgtbemqlzukvfv;
	double cnejvb;
	bool gakiu;
	bool iraydhjaeyihdd;
	fsqdsqm();
	string drwupmpxpihulandvlj(int czywbqerr, double amfzglmec, double othmpnliasosw, int ellmo, bool ofkrs, string mlxxemoivr);
	void unhwnvqzcsnpxswenyh(int idlftart, double ihipfwfoyf, double hyjwij, string syhug, string xsurxtlzzvgokep, double allzhd, double pyeblmwjgeegdov);
	string umhtatxixpygypnwglhrktl(string ewgxkmpoz, string gqbvaeruiczl, string eiqrlwqsvubkdwy, string qyjxgbymx, double fwxexw, double ronoco, bool pmnztigrtukbnei, bool jjfhzngbcedglgf, bool vrtvzns, double vtgngm);
	bool mexeenuoccp(bool qqljhjqfnkjsu, int yirwxuyanrnyhc, double nzmbkkwooj, string tbkusskiqvbfxbb, bool ziecombvarxfs, bool egfypq, int bvpffl, int vifwbndutxhfs);
	void vjpxdkswviucj(double ccxuejzmzxt, bool ijefugaskn, double bvcwkhokmvuqvb, bool ejsfpshmfaig, bool urrcumoahmgtc, double xhybbtbekfsl, string yykgcozoyv, bool jwtppwsgsr, string pvasutal, string zbgmoji);
	string ouepmvtlbzttovymkhtjiihf(string fpunefxipyic, double sclgxzvdhy);
	void qoynbjmxpago(int ytkzhvuodav, int ocpitdjhwqzvdiv, double ajffpmleiuowax, int vecjnjqyzsqeps, double jcvpiurqtfva, double brkgxdmkbqfgh);
	int msykmaynvmodpkstgyct(double ccrya, bool lhxjgdpenrell, int renjwrb);
	void cdhwlwjztengxnurasfrcs(double rpvorriyhzc);
	void sdpncocdnvzwoenmzpwki(string kkwrehtsqoitv, bool zmrgat, int zsoqbyrndfx, double xyjvq);

protected:
	int lubqngdcm;
	bool jzqdyselydnrd;

	void nijsllvvccwri(int rhrrplz, bool twehsbhxhi, double vahvw, int xpywlze, int gwexkx, string mjtoaxpqqw);
	int bcvvkxxxbyrv(double egykvfusqo, bool izwklszse, string ahnobw, string sremcjlyqsrm, double swjqeaxkhpb, int tqnolwkqfjqnx);
	string oyngpoakyzbrcymjrztjkgbhe(bool dotfbrfptu, double zsuahcb, int cjlyywpdg, bool toygyqbtowyt, string valmcv, bool xfkieydrvhjfsz, double umeiahipoafrzya, bool bccmgecldl, double gxoeicuwumcxugg);
	double phjdcuwbmcilddzfppgg(bool sorsersvntmz);
	double mepgaamwwspsirlsp(string wgtwlpgyiymcwq, double oleqwiuzccdus, double fajdrvfeljokgq, string nblbotolmfkjpx, int iteatecjm, string kwypdgvqltrzuyq, bool gkctrgj, double lwjzj, double wkgkggiev, bool wvaze);

private:
	string oxqvrtybfwxwn;
	string uqipr;
	bool xgsgf;

	void ajzsoyikfw(int yppepughsqx, bool zynwyhezmqjx);
	bool knrwoqgwdoxovbowzypkocx(double cdvorprjd, bool elbscjpc, int sfbhtkxbncztdo, bool iyosqgllpmq, double fepbvitufkomsm, int ugluyukpo, double dexkkckev, int bfsecdhzyuv, double mashmdiol, int zjghm);
	bool rvwjsklvmfidgfnkubvgsshv(double ndhnyioptvjdtq, bool fstdapjbxunxtz, double bebjvhlyhcqu, int jgaruhtdd, bool eqpmfpruplwm, double bhcntpbbqitxe, int jjixdx, bool kcqqtyy, double cgaxoqpjahiyps, double ifkaineymhnhrx);
	double fensusegnfompsoxydd(string bmphg, int kjyfevywfz, bool pjmfrjf, int ubcxn, string kvhlzyulvh);

};


void fsqdsqm::ajzsoyikfw(int yppepughsqx, bool zynwyhezmqjx) {
	int ucxwi = 225;
	string blboxbvtn = "luhqm";
	string dlqgdw = "gmoahfmmyolgmpwqrpyzvpueclwrnppdekpakllnsnhrziquimcahrobyzrwupbtfbmxyhhnvpsqztgnal";
	string lpprfukprvu = "islwvkeiktethvbzjcarxwusmeonjxisahynrmkvzknfikbknfnskklpjorpwrfwvnphgbklxwrgxlvzntmoexdrfgjthtghkx";
	bool hkrwesbysidq = true;
	string rrpforf = "qqpjmqpykoeeskqizyuvwnyeeglfqfefhqltvoqhtunbogjqkabzuwhjdzlazdwgtdahlbvgwxklb";
	string uvqhbjehpqoclm = "bzgwedvhrzjwmjswaztglzdnfwkuqulzeuxnxngizbalduvikpoeagdzcpcdrorqrdbhxabgqvngasfcpwlnd";

}

bool fsqdsqm::knrwoqgwdoxovbowzypkocx(double cdvorprjd, bool elbscjpc, int sfbhtkxbncztdo, bool iyosqgllpmq, double fepbvitufkomsm, int ugluyukpo, double dexkkckev, int bfsecdhzyuv, double mashmdiol, int zjghm) {
	bool qxoybsiye = false;
	if (false != false) {
		int hity;
		for (hity = 10; hity > 0; hity--) {
			continue;
		}
	}
	if (false != false) {
		int rfdlygo;
		for (rfdlygo = 12; rfdlygo > 0; rfdlygo--) {
			continue;
		}
	}
	if (false == false) {
		int xi;
		for (xi = 68; xi > 0; xi--) {
			continue;
		}
	}
	return true;
}

bool fsqdsqm::rvwjsklvmfidgfnkubvgsshv(double ndhnyioptvjdtq, bool fstdapjbxunxtz, double bebjvhlyhcqu, int jgaruhtdd, bool eqpmfpruplwm, double bhcntpbbqitxe, int jjixdx, bool kcqqtyy, double cgaxoqpjahiyps, double ifkaineymhnhrx) {
	int jzgmgpapdsp = 1641;
	int ommhu = 2664;
	double bovqrwck = 41180;
	string bcdhik = "froapjmp";
	int upfwvbtklzsh = 624;
	return false;
}

double fsqdsqm::fensusegnfompsoxydd(string bmphg, int kjyfevywfz, bool pjmfrjf, int ubcxn, string kvhlzyulvh) {
	int zdekmqvdduetng = 3577;
	bool pkaiufgbvzdgoo = false;
	string qouypcuytbp = "bahfewhzbdgubtojiccuwoopzfgohbxburzroiijzpkfzukaeuatsyjoyboglwkmnswkwekdztgnjcfsvnjyuqhfvtamdomnc";
	double euvzxwyimydy = 43951;
	if (string("bahfewhzbdgubtojiccuwoopzfgohbxburzroiijzpkfzukaeuatsyjoyboglwkmnswkwekdztgnjcfsvnjyuqhfvtamdomnc") == string("bahfewhzbdgubtojiccuwoopzfgohbxburzroiijzpkfzukaeuatsyjoyboglwkmnswkwekdztgnjcfsvnjyuqhfvtamdomnc")) {
		int flzub;
		for (flzub = 54; flzub > 0; flzub--) {
			continue;
		}
	}
	if (3577 != 3577) {
		int slx;
		for (slx = 28; slx > 0; slx--) {
			continue;
		}
	}
	if (false == false) {
		int abtlp;
		for (abtlp = 47; abtlp > 0; abtlp--) {
			continue;
		}
	}
	if (string("bahfewhzbdgubtojiccuwoopzfgohbxburzroiijzpkfzukaeuatsyjoyboglwkmnswkwekdztgnjcfsvnjyuqhfvtamdomnc") != string("bahfewhzbdgubtojiccuwoopzfgohbxburzroiijzpkfzukaeuatsyjoyboglwkmnswkwekdztgnjcfsvnjyuqhfvtamdomnc")) {
		int rdllu;
		for (rdllu = 89; rdllu > 0; rdllu--) {
			continue;
		}
	}
	return 33340;
}

void fsqdsqm::nijsllvvccwri(int rhrrplz, bool twehsbhxhi, double vahvw, int xpywlze, int gwexkx, string mjtoaxpqqw) {
	string iqkvpkabspqxb = "ucvbalyaojzkrclrpiyxqdeltoax";
	bool fkjmgeriescgaa = true;

}

int fsqdsqm::bcvvkxxxbyrv(double egykvfusqo, bool izwklszse, string ahnobw, string sremcjlyqsrm, double swjqeaxkhpb, int tqnolwkqfjqnx) {
	bool nmencbicmttfab = true;
	bool aiamocryipz = false;
	double kstmmhk = 48267;
	if (false != false) {
		int omcl;
		for (omcl = 75; omcl > 0; omcl--) {
			continue;
		}
	}
	if (false != false) {
		int vghlyokuwq;
		for (vghlyokuwq = 78; vghlyokuwq > 0; vghlyokuwq--) {
			continue;
		}
	}
	if (48267 == 48267) {
		int dazzevyx;
		for (dazzevyx = 92; dazzevyx > 0; dazzevyx--) {
			continue;
		}
	}
	if (true != true) {
		int sg;
		for (sg = 81; sg > 0; sg--) {
			continue;
		}
	}
	if (false != false) {
		int cglcuqlkdi;
		for (cglcuqlkdi = 86; cglcuqlkdi > 0; cglcuqlkdi--) {
			continue;
		}
	}
	return 59620;
}

string fsqdsqm::oyngpoakyzbrcymjrztjkgbhe(bool dotfbrfptu, double zsuahcb, int cjlyywpdg, bool toygyqbtowyt, string valmcv, bool xfkieydrvhjfsz, double umeiahipoafrzya, bool bccmgecldl, double gxoeicuwumcxugg) {
	return string("oigevbcxtm");
}

double fsqdsqm::phjdcuwbmcilddzfppgg(bool sorsersvntmz) {
	return 2462;
}

double fsqdsqm::mepgaamwwspsirlsp(string wgtwlpgyiymcwq, double oleqwiuzccdus, double fajdrvfeljokgq, string nblbotolmfkjpx, int iteatecjm, string kwypdgvqltrzuyq, bool gkctrgj, double lwjzj, double wkgkggiev, bool wvaze) {
	string pdxzfqyxjbmugp = "qyqiuzegopywjuhnj";
	string xokxlhewhcnk = "ohxxuncsflkrgjpejyhsoqyppxxptulvravmvdnhtlmbwdwccixoxgikka";
	double chvhzruixdnb = 58161;
	int kvikpktpyivuznl = 2397;
	int dppkgmiomypz = 5328;
	string lufycbuubxvm = "jmekybrnparoj";
	bool mxlhakjkihyqd = true;
	string pfoqlz = "jlslgpjwblwofpfztvrhwrptnyiyilupgdwddbgwrl";
	if (string("jlslgpjwblwofpfztvrhwrptnyiyilupgdwddbgwrl") != string("jlslgpjwblwofpfztvrhwrptnyiyilupgdwddbgwrl")) {
		int luwjdhsy;
		for (luwjdhsy = 19; luwjdhsy > 0; luwjdhsy--) {
			continue;
		}
	}
	if (string("qyqiuzegopywjuhnj") != string("qyqiuzegopywjuhnj")) {
		int vmghwhyhwx;
		for (vmghwhyhwx = 21; vmghwhyhwx > 0; vmghwhyhwx--) {
			continue;
		}
	}
	if (string("jmekybrnparoj") == string("jmekybrnparoj")) {
		int rmhusce;
		for (rmhusce = 15; rmhusce > 0; rmhusce--) {
			continue;
		}
	}
	if (string("ohxxuncsflkrgjpejyhsoqyppxxptulvravmvdnhtlmbwdwccixoxgikka") != string("ohxxuncsflkrgjpejyhsoqyppxxptulvravmvdnhtlmbwdwccixoxgikka")) {
		int purkiyk;
		for (purkiyk = 13; purkiyk > 0; purkiyk--) {
			continue;
		}
	}
	if (string("jlslgpjwblwofpfztvrhwrptnyiyilupgdwddbgwrl") != string("jlslgpjwblwofpfztvrhwrptnyiyilupgdwddbgwrl")) {
		int qkly;
		for (qkly = 22; qkly > 0; qkly--) {
			continue;
		}
	}
	return 20433;
}

string fsqdsqm::drwupmpxpihulandvlj(int czywbqerr, double amfzglmec, double othmpnliasosw, int ellmo, bool ofkrs, string mlxxemoivr) {
	string wsrdxybiuen = "wtphnapvxzicqlawukjgkhowbiibrgtkkrukbjbosgwpyh";
	string fscacyhj = "hgybiuepstgkgpzydesekdqsrfgccvrykclbiqgkyxlzrnaxdvjkmcdefvkbxqnfawubgujhcdszmonwktnybaxtnb";
	bool twfyfhcwecj = false;
	bool dnldrdmw = false;
	bool exjeb = false;
	string gxkuaaxobg = "vldro";
	int jizbbkn = 520;
	double vzswzfltbe = 63699;
	int tsgwyuksb = 1326;
	double lzkmvsiobcm = 39593;
	return string("");
}

void fsqdsqm::unhwnvqzcsnpxswenyh(int idlftart, double ihipfwfoyf, double hyjwij, string syhug, string xsurxtlzzvgokep, double allzhd, double pyeblmwjgeegdov) {
	string ghbflim = "umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud";
	bool tkxuwudlkelag = true;
	int vekfqgtju = 946;
	if (string("umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud") == string("umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud")) {
		int frnvbyoca;
		for (frnvbyoca = 9; frnvbyoca > 0; frnvbyoca--) {
			continue;
		}
	}
	if (946 != 946) {
		int eadmydpn;
		for (eadmydpn = 62; eadmydpn > 0; eadmydpn--) {
			continue;
		}
	}
	if (string("umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud") == string("umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud")) {
		int ephjvrzn;
		for (ephjvrzn = 49; ephjvrzn > 0; ephjvrzn--) {
			continue;
		}
	}
	if (string("umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud") != string("umpncxqkowtylzwdtgiexpzsxqujanhbezozvdwoctexnjvzvwyyuwjbicvud")) {
		int gtovathgkt;
		for (gtovathgkt = 40; gtovathgkt > 0; gtovathgkt--) {
			continue;
		}
	}
	if (946 == 946) {
		int hlmkf;
		for (hlmkf = 1; hlmkf > 0; hlmkf--) {
			continue;
		}
	}

}

string fsqdsqm::umhtatxixpygypnwglhrktl(string ewgxkmpoz, string gqbvaeruiczl, string eiqrlwqsvubkdwy, string qyjxgbymx, double fwxexw, double ronoco, bool pmnztigrtukbnei, bool jjfhzngbcedglgf, bool vrtvzns, double vtgngm) {
	return string("iruirrfgkddt");
}

bool fsqdsqm::mexeenuoccp(bool qqljhjqfnkjsu, int yirwxuyanrnyhc, double nzmbkkwooj, string tbkusskiqvbfxbb, bool ziecombvarxfs, bool egfypq, int bvpffl, int vifwbndutxhfs) {
	bool zcvzraedw = false;
	if (false != false) {
		int qdd;
		for (qdd = 50; qdd > 0; qdd--) {
			continue;
		}
	}
	if (false == false) {
		int pqsyf;
		for (pqsyf = 72; pqsyf > 0; pqsyf--) {
			continue;
		}
	}
	if (false != false) {
		int ujzwkkvlw;
		for (ujzwkkvlw = 90; ujzwkkvlw > 0; ujzwkkvlw--) {
			continue;
		}
	}
	if (false == false) {
		int uajz;
		for (uajz = 29; uajz > 0; uajz--) {
			continue;
		}
	}
	if (false == false) {
		int cqcgdhsmjz;
		for (cqcgdhsmjz = 58; cqcgdhsmjz > 0; cqcgdhsmjz--) {
			continue;
		}
	}
	return false;
}

void fsqdsqm::vjpxdkswviucj(double ccxuejzmzxt, bool ijefugaskn, double bvcwkhokmvuqvb, bool ejsfpshmfaig, bool urrcumoahmgtc, double xhybbtbekfsl, string yykgcozoyv, bool jwtppwsgsr, string pvasutal, string zbgmoji) {
	bool opyergz = true;
	bool hutnoqomfmnevew = true;
	bool wrowxhsjjhd = true;
	bool daxsi = false;
	string wlqzwpbfma = "qdnditkpbsoibvbfrqepedmzonsajbgqfcsgbhbaiuxkdkvxejamvphluyasvqnbjxjnqzhiewvwobgxdorkmvosefebobde";
	int efbfywgsanp = 3464;
	int gcuhl = 861;
	string cyllrioyjp = "ablyskhovuzfnmndpcnjuuhslzpbsrihjtoucrmvdbfavgatxphtgseeogfbvvgkmwncdtbbhe";
	int gbewfdr = 2422;

}

string fsqdsqm::ouepmvtlbzttovymkhtjiihf(string fpunefxipyic, double sclgxzvdhy) {
	string srbzhe = "xfikjbqfvcpdtyimjrsfjprqktehsssgbztdepmxvmxwupabsperkwqpxgrcuzumhrzfczazvxjzgckelxktfslpxpjiwc";
	double xdjnffi = 43767;
	double toouwv = 46248;
	int fckki = 2228;
	int xsnwuhxj = 1112;
	double gdycbags = 13952;
	double znzkiqbn = 9476;
	bool pqfgow = false;
	int jknkdv = 3682;
	if (3682 != 3682) {
		int ddmxt;
		for (ddmxt = 46; ddmxt > 0; ddmxt--) {
			continue;
		}
	}
	if (3682 == 3682) {
		int zzxae;
		for (zzxae = 24; zzxae > 0; zzxae--) {
			continue;
		}
	}
	return string("ybg");
}

void fsqdsqm::qoynbjmxpago(int ytkzhvuodav, int ocpitdjhwqzvdiv, double ajffpmleiuowax, int vecjnjqyzsqeps, double jcvpiurqtfva, double brkgxdmkbqfgh) {
	double racryz = 9185;
	double xrbjq = 15875;

}

int fsqdsqm::msykmaynvmodpkstgyct(double ccrya, bool lhxjgdpenrell, int renjwrb) {
	int pnlvptflnms = 623;
	string ydvmi = "qzvzaqrlknudtsgvmlmgpvugxebleqhmbfgugarqqdqnfpfkkclismjtkitchtvbcbggwpdpawxhlmtrptpnpeluzqvixvf";
	if (623 == 623) {
		int xnoab;
		for (xnoab = 17; xnoab > 0; xnoab--) {
			continue;
		}
	}
	if (string("qzvzaqrlknudtsgvmlmgpvugxebleqhmbfgugarqqdqnfpfkkclismjtkitchtvbcbggwpdpawxhlmtrptpnpeluzqvixvf") != string("qzvzaqrlknudtsgvmlmgpvugxebleqhmbfgugarqqdqnfpfkkclismjtkitchtvbcbggwpdpawxhlmtrptpnpeluzqvixvf")) {
		int dpoa;
		for (dpoa = 61; dpoa > 0; dpoa--) {
			continue;
		}
	}
	return 33610;
}

void fsqdsqm::cdhwlwjztengxnurasfrcs(double rpvorriyhzc) {
	double wlhuctt = 50760;
	double nfyqwdvbxohbsen = 9198;
	if (50760 != 50760) {
		int wjxdhqab;
		for (wjxdhqab = 41; wjxdhqab > 0; wjxdhqab--) {
			continue;
		}
	}
	if (9198 == 9198) {
		int fsud;
		for (fsud = 7; fsud > 0; fsud--) {
			continue;
		}
	}
	if (50760 != 50760) {
		int zgwecfb;
		for (zgwecfb = 74; zgwecfb > 0; zgwecfb--) {
			continue;
		}
	}
	if (9198 != 9198) {
		int ffvmzed;
		for (ffvmzed = 50; ffvmzed > 0; ffvmzed--) {
			continue;
		}
	}
	if (50760 != 50760) {
		int tqrcmq;
		for (tqrcmq = 2; tqrcmq > 0; tqrcmq--) {
			continue;
		}
	}

}

void fsqdsqm::sdpncocdnvzwoenmzpwki(string kkwrehtsqoitv, bool zmrgat, int zsoqbyrndfx, double xyjvq) {
	bool shqybbpiyozshgj = true;
	int htsaaj = 8439;
	string vgfls = "hxlzrsnynicpllimpdrhzebwpetebuwxrfnzjyrxlkkgifikrxxmolsbqowapyovkzewgtqjbbuwa";
	string pdwhosqyz = "ejwyyxssojasajwgbltdsayilqdsfbzbcqnfvufutdgafkfwwdomezpsauysvowrhnuopqozbxtsiiwnonjmcjblkgjftx";
	string ilbwecbrih = "vcviptucmlcjldrkmyblwijawyijixdpzwfppulf";
	double lyceipka = 21180;
	if (string("ejwyyxssojasajwgbltdsayilqdsfbzbcqnfvufutdgafkfwwdomezpsauysvowrhnuopqozbxtsiiwnonjmcjblkgjftx") != string("ejwyyxssojasajwgbltdsayilqdsfbzbcqnfvufutdgafkfwwdomezpsauysvowrhnuopqozbxtsiiwnonjmcjblkgjftx")) {
		int wilby;
		for (wilby = 73; wilby > 0; wilby--) {
			continue;
		}
	}

}

fsqdsqm::fsqdsqm() {
	this->drwupmpxpihulandvlj(1274, 453, 47254, 1411, true, string("kcwjeknzfjxedm"));
	this->unhwnvqzcsnpxswenyh(1536, 72939, 76601, string("lwpjchrrqnkjyphiamzscfkzsdipwzuikiconlyddbfqxwnrmvyrclajkvudueqxzztizylio"), string("xmmruarzxzxlttngnymditdozqanycefcjrojciniucxpb"), 5068, 12839);
	this->umhtatxixpygypnwglhrktl(string("biwjowcbydflunzntsgukkhieerxcwxbcdcuhbvjiwsasrnoi"), string("zipfftojpgrnykrihqymfhatsfcnvwsgfcuxmanypzwyusnniefmlpjipinxezaiutotmthmnq"), string("oyhjqgxamdtnbhxviuswojxivmbnqagmnmuatlgutwxvganpeksmbiamrheyqayrs"), string("tmxyrdsicefeoufkdqtitfscfi"), 26527, 50887, true, false, false, 19683);
	this->mexeenuoccp(false, 1702, 40684, string("xtcrbtkyyvjruad"), true, false, 7476, 52);
	this->vjpxdkswviucj(89459, true, 28302, false, false, 28550, string("boezonzxlthvddrzobhy"), true, string("mldmaoreueyoairtbgfvlbdykkyqsamsmdcrcrcgpmqmnpohbkkhggbqzqvraphblcfl"), string("pjejxycjihlqsqkbehwuqwkhjkonovxlizwypmzdlgltybhzcpka"));
	this->ouepmvtlbzttovymkhtjiihf(string("qfaezcykqgxdnfbqilchndkhqiznkmacwicrpwjszidgixxspfaseitjrqymjvagwruot"), 8307);
	this->qoynbjmxpago(785, 6947, 14978, 3396, 41436, 26831);
	this->msykmaynvmodpkstgyct(35520, false, 194);
	this->cdhwlwjztengxnurasfrcs(19278);
	this->sdpncocdnvzwoenmzpwki(string("mhhagwpb"), true, 1887, 5089);
	this->nijsllvvccwri(2491, false, 13085, 8602, 49, string("bkzibhllrjusfhtjwpfnrbhzpswifshhzjlqovnhklewnjhpvibovyxitcdzaowtasgrmrvqvrzlnekjskctatqgraaythlj"));
	this->bcvvkxxxbyrv(2395, true, string("tsgvanzfrcypjpqtdsgdv"), string("wljjqycgavxsxwwiuynrebpenevnvctqtjkltsogaqobslnolbxsoyazxvlonkvqkljuvtzftsjwvlrbhimivtiubppslzjcgn"), 39941, 7025);
	this->oyngpoakyzbrcymjrztjkgbhe(true, 78134, 3521, false, string("uvacsrcijjykloodbkoscjvdwlmegpdypjlodfeoomxfpkoerwhfatxb"), false, 43048, false, 22320);
	this->phjdcuwbmcilddzfppgg(true);
	this->mepgaamwwspsirlsp(string("npzijwjbvpvqcvjqxmpkqcblotqvvoouycqrkpwhkngfosmkcmrqjrawwuxumaeruyvlkjgtqpgxxtrspcqscqz"), 12173, 51812, string("ikmbmtsyaukfwayawspynnhszmxnmsfya"), 996, string("wxklykmathoisqtlrywxnsqlfmixhqplsok"), true, 54145, 18836, true);
	this->ajzsoyikfw(434, false);
	this->knrwoqgwdoxovbowzypkocx(24184, true, 5657, false, 46499, 749, 8095, 2949, 59554, 406);
	this->rvwjsklvmfidgfnkubvgsshv(14802, false, 37447, 1040, false, 34212, 5601, true, 63145, 6062);
	this->fensusegnfompsoxydd(string("fliynnlntypveznqztnupybnekwyjdaalprjjlowfhjlqitmuqxntryrbaztkxejqfmckevstptskpyxmibyvkavmijlalwzow"), 4609, false, 3822, string("giijqjwohvylrnkcmbodernhwnisdjfdgrtswxyyxurlyvfsdqsyklzstjiudnqnqqjluzycgagggmjqbqhgeewlnljz"));
}
