/*
Syn's AyyWare Framework 2015
*/

#include "GUI.h"

#include "RenderManager.h"
#include "MetaInfo.h"

#include <algorithm>
#include "tinyxml2.h"
#include "Controls.h"

CGUI GUI;

CGUI::CGUI()
{

}

// Draws all windows 
void CGUI::Draw()
{
	bool ShouldDrawCursor = false;

	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			ShouldDrawCursor = true;
			DrawWindow(window);
		}

	}

	if (ShouldDrawCursor)
	{
		static Vertex_t MouseVt[3];

		MouseVt[0].Init(Vector2D(Mouse.x, Mouse.y));
		MouseVt[1].Init(Vector2D(Mouse.x + 16, Mouse.y));
		MouseVt[2].Init(Vector2D(Mouse.x, Mouse.y + 16));

		Render::PolygonOutline(3, MouseVt, Color(0, 0, 0, 230), Color(0, 0, 0, 240));
	}
}

// Handle all input etc
void CGUI::Update()
{
	//Key Array
	std::copy(keys, keys + 255, oldKeys);
	for (int x = 0; x < 255; x++)
	{
		//oldKeys[x] = oldKeys[x] & keys[x];
		keys[x] = (GetAsyncKeyState(x));
	}

	// Mouse Location
	POINT mp; GetCursorPos(&mp);
	Mouse.x = mp.x; Mouse.y = mp.y;

	RECT Screen = Render::GetViewport();

	// Window Binds
	for (auto& bind : WindowBinds)
	{
		if (GetKeyPress(bind.first))
		{
			bind.second->Toggle();
		}
	}

	// Stop dragging
	if (IsDraggingWindow && !GetKeyState(VK_LBUTTON))
	{
		IsDraggingWindow = false;
		DraggingWindow = nullptr;
	}

	// If we are in the proccess of dragging a window
	if (IsDraggingWindow && GetKeyState(VK_LBUTTON) && !GetKeyPress(VK_LBUTTON))
	{
		if (DraggingWindow)
		{
			DraggingWindow->m_x = Mouse.x - DragOffsetX;
			DraggingWindow->m_y = Mouse.y - DragOffsetY;
		}
	}

	// Process some windows
	for (auto window : Windows)
	{
		if (window->m_bIsOpen)
		{
			// Used to tell the widget processing that there could be a click
			bool bCheckWidgetClicks = false;

			// If the user clicks inside the window
			if (GetKeyPress(VK_LBUTTON))
			{
				if (IsMouseInRegion(window->m_x, window->m_y, window->m_x + window->m_iWidth, window->m_y + window->m_iHeight))
				{
					// Is it inside the client area?
					if (IsMouseInRegion(window->GetClientArea()))
					{
						// User is selecting a new tab
						if (IsMouseInRegion(window->GetTabArea()))
						{
							// Loose focus on the control
							window->IsFocusingControl = false;
							window->FocusedControl = nullptr;

							int iTab = 0;
							int TabCount = window->Tabs.size();
							if (TabCount) // If there are some tabs
							{
								int TabSize = (window->m_iWidth - 4 - 12) / TabCount;
								int Dist = Mouse.x - (window->m_x + 8);
								while (Dist > TabSize)
								{
									if (Dist > TabSize)
									{
										iTab++;
										Dist -= TabSize;
									}
								}
								window->SelectedTab = window->Tabs[iTab];
							}

						}
						else
							bCheckWidgetClicks = true;
					}
					else
					{
						// Must be in the around the title or side of the window
						// So we assume the user is trying to drag the window
						IsDraggingWindow = true;
						DraggingWindow = window;
						DragOffsetX = Mouse.x - window->m_x;
						DragOffsetY = Mouse.y - window->m_y;

						// Loose focus on the control
						window->IsFocusingControl = false;
						window->FocusedControl = nullptr;
					}
				}
				else
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}


			// Controls 
			if (window->SelectedTab != nullptr)
			{
				// Focused widget
				bool SkipWidget = false;
				CControl* SkipMe = nullptr;

				// this window is focusing on a widget??
				if (window->IsFocusingControl)
				{
					if (window->FocusedControl != nullptr)
					{
						// We've processed it once, skip it later
						SkipWidget = true;
						SkipMe = window->FocusedControl;

						POINT cAbs = window->FocusedControl->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, window->FocusedControl->m_iWidth, window->FocusedControl->m_iHeight };
						window->FocusedControl->OnUpdate();

						if (window->FocusedControl->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
						{
							window->FocusedControl->OnClick();

							// If it gets clicked we loose focus
							window->IsFocusingControl = false;
							window->FocusedControl = nullptr;
							bCheckWidgetClicks = false;
						}
					}
				}

				// Itterate over the rest of the control
				for (auto control : window->SelectedTab->Controls)
				{
					if (control != nullptr)
					{
						if (SkipWidget && SkipMe == control)
							continue;

						POINT cAbs = control->GetAbsolutePos();
						RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
						control->OnUpdate();

						if (control->Flag(UIFlags::UI_Clickable) && IsMouseInRegion(controlRect) && bCheckWidgetClicks)
						{
							control->OnClick();
							bCheckWidgetClicks = false;

							// Change of focus
							if (control->Flag(UIFlags::UI_Focusable))
							{
								window->IsFocusingControl = true;
								window->FocusedControl = control;
							}
							else
							{
								window->IsFocusingControl = false;
								window->FocusedControl = nullptr;
							}

						}
					}
				}

				// We must have clicked whitespace
				if (bCheckWidgetClicks)
				{
					// Loose focus on the control
					window->IsFocusingControl = false;
					window->FocusedControl = nullptr;
				}
			}
		}
	}
}

// Returns 
bool CGUI::GetKeyPress(unsigned int key)
{
	if (keys[key] == true && oldKeys[key] == false)
		return true;
	else
		return false;
}

bool CGUI::GetKeyState(unsigned int key)
{
	return keys[key];
}

bool CGUI::IsMouseInRegion(int x, int y, int x2, int y2)
{
	if (Mouse.x > x && Mouse.y > y && Mouse.x < x2 && Mouse.y < y2)
		return true;
	else
		return false;
}

bool CGUI::IsMouseInRegion(RECT region)
{
	return IsMouseInRegion(region.left, region.top, region.left + region.right, region.top + region.bottom);
}

POINT CGUI::GetMouse()
{
	return Mouse;
}

bool CGUI::DrawWindow(CWindow* window)
{
	Render::Outline(window->m_x, window->m_y, window->m_iWidth, window->m_iHeight, Color(21, 21, 21, 80));
	Render::GradientV(window->m_x + 2, window->m_y + 2, window->m_iWidth - 4, 26, Color(45, 40, 40, 255), Color(45, 45, 40, 255));
	Render::Clear(window->m_x + 2, window->m_y + 2 + 26, window->m_iWidth - 4, window->m_iHeight - 4 - 26, Color(49, 42, 42, 255));
	Render::Outline(window->m_x + 1, window->m_y + 1, window->m_iWidth - 2, window->m_iHeight - 2, Color(49, 42, 42, 255));
	// Main Window
	/*Render::Outline(window->m_x, window->m_y, window->m_iWidth, window->m_iHeight, Color(21, 21, 21, 80));
	Render::GradientV(window->m_x + 2, window->m_y + 2, window->m_iWidth - 4, 26, Color(2, 78, 0, 255), Color(7, 255, 37, 255));
	Render::Clear(window->m_x + 2, window->m_y + 2 + 26, window->m_iWidth - 4, window->m_iHeight - 4 - 26, Color(7, 255, 37, 255));
	Render::Outline(window->m_x + 1, window->m_y + 1, window->m_iWidth - 2, window->m_iHeight - 2, Color(91, 242, 0, 255));*/
	Render::Text(window->m_x + 8, window->m_y + 8, Color(255, 0, 0, 255), Render::Fonts::MenuBold, window->Title.c_str());

	//Inner
	//Render::Outline(window->m_x + 7, window->m_y + 1 + 26, window->m_iWidth - 4 - 10, window->m_iHeight - 2 - 6 - 26, Color(255, 255, 255, 80));
	Render::Clear(window->m_x + 8, window->m_y + 1 + 27, window->m_iWidth - 4 - 12, window->m_iHeight - 2 - 8 - 26, Color(30, 30, 30, 255));

	//Tab
	Render::GradientV(window->m_x + 8, window->m_y + 1 + 27, window->m_iWidth - 4 - 12, 29, Color(49, 42, 42, 255), Color(49, 42, 42, 255));
	int TabCount = window->Tabs.size();
	if (TabCount) // If there are some tabs
	{
		int TabSize = (window->m_iWidth - 4 - 12) / TabCount;
		for (int i = 0; i < TabCount; i++)
		{
			RECT TabArea = { window->m_x + 8 + (i*TabSize), window->m_y + 1 + 27, TabSize, 29 };
			CTab *tab = window->Tabs[i];
			if (window->SelectedTab == tab)
			{
				Render::GradientV(window->m_x + 8 + (i*TabSize), window->m_y + 1 + 27, TabSize, 29, Color(106, 106, 106, 255), Color(49, 42, 42, 255));
			}
			else if (IsMouseInRegion(TabArea))
			{
				Render::GradientV(window->m_x + 8 + (i*TabSize), window->m_y + 1 + 27, TabSize, 29, Color(106, 106, 106, 255), Color(80, 80, 80, 255));
			}
			RECT TextSize = Render::GetTextSize(Render::Fonts::MenuBold, tab->Title.c_str());
			Render::Text(TabArea.left + (TabSize / 2) - (TextSize.right / 2), TabArea.top + 8, Color(255, 0, 0, 255), Render::Fonts::MenuBold, tab->Title.c_str());
			Render::Clear(window->m_x + 8, window->m_y + 1 + 27, window->m_iWidth - 4 - 12, 2, Color(62, 55, 55, 255));
		}
	}


	// Controls 
	if (window->SelectedTab != nullptr)
	{
		// Focused widget
		bool SkipWidget = false;
		CControl* SkipMe = nullptr;

		// this window is focusing on a widget??
		if (window->IsFocusingControl)
		{
			if (window->FocusedControl != nullptr)
			{
				// We need to draw it last, so skip it in the regular loop
				SkipWidget = true;
				SkipMe = window->FocusedControl;
			}
		}


		// Itterate over all the other controls
		for (auto control : window->SelectedTab->Controls)
		{
			if (SkipWidget && SkipMe == control)
				continue;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				POINT cAbs = control->GetAbsolutePos();
				RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
				bool hover = false;
				if (IsMouseInRegion(controlRect))
				{
					hover = true;
				}
				control->Draw(hover);
			}
		}

		// Draw the skipped widget last
		if (SkipWidget)
		{
			auto control = window->FocusedControl;

			if (control != nullptr && control->Flag(UIFlags::UI_Drawable))
			{
				POINT cAbs = control->GetAbsolutePos();
				RECT controlRect = { cAbs.x, cAbs.y, control->m_iWidth, control->m_iHeight };
				bool hover = false;
				if (IsMouseInRegion(controlRect))
				{
					hover = true;
				}
				control->Draw(hover);
			}
		}

	}


	return true;
}

void CGUI::RegisterWindow(CWindow* window)
{
	Windows.push_back(window);

	// Resorting to put groupboxes at the start
	for (auto tab : window->Tabs)
	{
		for (auto control : tab->Controls)
		{
			if (control->Flag(UIFlags::UI_RenderFirst))
			{
				CControl * c = control;
				tab->Controls.erase(std::remove(tab->Controls.begin(), tab->Controls.end(), control), tab->Controls.end());
				tab->Controls.insert(tab->Controls.begin(), control);
			}
		}
	}
}

void CGUI::BindWindow(unsigned char Key, CWindow* window)
{
	if (window)
		WindowBinds[Key] = window;
	else
		WindowBinds.erase(Key);
}

void CGUI::SaveWindowState(CWindow* window, std::string Filename)
{
	// Create a whole new document and we'll just save over top of the old one
	tinyxml2::XMLDocument Doc;

	// Root Element is called "ayy"
	tinyxml2::XMLElement *Root = Doc.NewElement("ayy");
	Doc.LinkEndChild(Root);

	Utilities::Log("Saving Window %s", window->Title.c_str());

	// If the window has some tabs..
	if (Root && window->Tabs.size() > 0)
	{
		for (auto Tab : window->Tabs)
		{
			// Add a new section for this tab
			tinyxml2::XMLElement *TabElement = Doc.NewElement(Tab->Title.c_str());
			Root->LinkEndChild(TabElement);

			Utilities::Log("Saving Tab %s", Tab->Title.c_str());

			// Now we itterate the controls this tab contains
			if (TabElement && Tab->Controls.size() > 0)
			{
				for (auto Control : Tab->Controls)
				{
					// If the control is ok to be saved
					if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileIdentifier.length() > 1 && Control->FileControlType)
					{
						// Create an element for the control
						tinyxml2::XMLElement *ControlElement = Doc.NewElement(Control->FileIdentifier.c_str());
						TabElement->LinkEndChild(ControlElement);

						Utilities::Log("Saving control %s", Control->FileIdentifier.c_str());

						if (!ControlElement)
						{
							Utilities::Log("Errorino :("); // s0 cute
							return;
						}

						CCheckBox* cbx = nullptr;
						CComboBox* cbo = nullptr;
						CKeyBind* key = nullptr;
						CSlider* sld = nullptr;

						// Figure out what kind of control and data this is
						switch (Control->FileControlType)
						{
						case UIControlTypes::UIC_CheckBox:
							cbx = (CCheckBox*)Control;
							ControlElement->SetText(cbx->GetState());
							break;
						case UIControlTypes::UIC_ComboBox:
							cbo = (CComboBox*)Control;
							ControlElement->SetText(cbo->GetIndex());
							break;
						case UIControlTypes::UIC_KeyBind:
							key = (CKeyBind*)Control;
							ControlElement->SetText(key->GetKey());
							break;
						case UIControlTypes::UIC_Slider:
							sld = (CSlider*)Control;
							ControlElement->SetText(sld->GetValue());
							break;
						}
					}
				}
			}
		}
	}

	//Save the file
	if (Doc.SaveFile(Filename.c_str()) != tinyxml2::XML_NO_ERROR)
	{
		MessageBox(NULL, "Failed To Save Config File!", "AyyWare", MB_OK);
	}

}

void CGUI::LoadWindowState(CWindow* window, std::string Filename)
{
	// Lets load our meme
	tinyxml2::XMLDocument Doc;
	if (Doc.LoadFile(Filename.c_str()) == tinyxml2::XML_NO_ERROR)
	{
		tinyxml2::XMLElement *Root = Doc.RootElement();

		// The root "ayy" element
		if (Root)
		{
			// If the window has some tabs..
			if (Root && window->Tabs.size() > 0)
			{
				for (auto Tab : window->Tabs)
				{
					// We find the corresponding element for this tab
					tinyxml2::XMLElement *TabElement = Root->FirstChildElement(Tab->Title.c_str());
					if (TabElement)
					{
						// Now we itterate the controls this tab contains
						if (TabElement && Tab->Controls.size() > 0)
						{
							for (auto Control : Tab->Controls)
							{
								// If the control is ok to be saved
								if (Control && Control->Flag(UIFlags::UI_SaveFile) && Control->FileIdentifier.length() > 1 && Control->FileControlType)
								{
									// Get the controls element
									tinyxml2::XMLElement *ControlElement = TabElement->FirstChildElement(Control->FileIdentifier.c_str());

									if (ControlElement)
									{
										CCheckBox* cbx = nullptr;
										CComboBox* cbo = nullptr;
										CKeyBind* key = nullptr;
										CSlider* sld = nullptr;

										// Figure out what kind of control and data this is
										switch (Control->FileControlType)
										{
										case UIControlTypes::UIC_CheckBox:
											cbx = (CCheckBox*)Control;
											cbx->SetState(ControlElement->GetText()[0] == '1' ? true : false);
											break;
										case UIControlTypes::UIC_ComboBox:
											cbo = (CComboBox*)Control;
											cbo->SelectIndex(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_KeyBind:
											key = (CKeyBind*)Control;
											key->SetKey(atoi(ControlElement->GetText()));
											break;
										case UIControlTypes::UIC_Slider:
											sld = (CSlider*)Control;
											sld->SetValue(atof(ControlElement->GetText()));
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class osideyu {
public:
	double pyghjccouje;
	osideyu();
	int dopiqrqmbnq(double lwihvv, bool uxnlawp, bool rozbpjtwuseby);
	double vsjowhkurdszdz(string psemvlitqidj, int squph, string vlgbo);
	double fjneanvimugsvem(double orktjqlvhboik, bool lgatyolf, bool sfbxh, string lltnxnbdnigmmk, double jcqluxskdswwyhe, string pelyrjg, string oitvlgjbhgpim);
	bool yupphkspwacyaqhzfd(bool ymhgnighf, string tjkiwaxg, bool xrbkfudwcyzf);
	bool ynssspsufizqhhfojnscuv(double tzndiy, string jjbztzwpeanimse, int okbabcieza, bool dcsrycljaclhpi);
	double evxvotpdbvpml(int adgpfzsosdxq, string kfuae);
	string gsszfdxwzaqw(string mbwnbwxk, bool naabyuktu, double gtlsmfi, int sshqyusbe);
	bool xbydzpeqojmzclqnwitjhon(string mbohlvwjrhiq, double nlyfro, string vndcudoiwghnukm, bool lypfsqdl, bool aqyqzvbsae, string keqjllxtlwcf);
	double kfsnzvgmeugqxlzdokrbjfhi(string runoloekjt, double tfbynkojpbr, int tbqodgahr, string vzfmuvebxchyy, int eifucvferkadstz, double mmmuiax, bool wvmzbpxuzy);
	void ficowimdolo(int fchvle);

protected:
	bool ieztuczufuestvy;

	void kztygsnqhvkb(double khclyagczh, int vyowzoyra);
	int glwnypsuxe(int eqwhlmafau, bool hyjnbnqnpgrv, int djvodncqv, int kdqpdtu);
	int zctzfdxuhzwzzxwqn(double ajeqpwc, string wrsemonpzxtv, int bsmyjfqhljgwn, double dbxkcgvetxqkeow, string yadaygyykxy, string duwigfglqiskv, int snvipwmwlw, string yaqbcpqv, string jkjodpdafvnrjgx);

private:
	double kmwrobkdsegrol;
	bool nhmhrrve;
	string talet;
	int qmprkmtioyc;

	double jqchgmjbqynirvqh(int qnevxbhljgyhr, int qlwpmawf, string irihxkmdlqgpye, double gefizbgeqriptgb, double rmhhi, string xmmyduyjysenzxi, double zuqteoxiyctjrf);
	bool xnhvocpxwykipb(bool xtybfnxgiuqdjpt, bool ngqzsrcxoqnb, double euqeyctncrtqv, bool jtceosmjrwcpgm, string oedki, bool iakyos, double rasedva, bool pivwmutqyofv);
	string fdjjluxqnqmskakwfohm(double ggezdavk, double sznzaalbj, bool hxwnhaxyz, string ejbnotgadbac, bool fqxwmeiismxq, int katbqp);
	void uvdnsgisxhogvaujdrglyuk(double lbyvoayphxq, string tqtcrkhk, double qsatcr);
	bool ueywgjzxohlh(int fnvcmvmonmjnai, bool ozzcdokw, string npttargnvpwng, int huuvcto, double noyubkjcwhbwvf, double spieqzpu, double myhrbqqbiizwz);
	double rjmafuvcygr(string spiwarigzjf, string pvpxqefz, string uidhsrlcr, int xsfeqrhctpxxhw);
	void rkkktjnkdyptldqpnni(int tdntzyt, string yupxgnelay, int amevqlfffvf, double ggdrfrtbj, string jfziyofsq, bool fthwwc, string kjjlm);
	int nqqgkkjwodq(int qtncuy, bool qfxue, bool jzxnjhphjax, bool wqbjjtucrbvdt, bool dftwmiznaz);

};


double osideyu::jqchgmjbqynirvqh(int qnevxbhljgyhr, int qlwpmawf, string irihxkmdlqgpye, double gefizbgeqriptgb, double rmhhi, string xmmyduyjysenzxi, double zuqteoxiyctjrf) {
	string cqxdsum = "huwjskgowxmdbelhybogzzqyynxbwsgmsgkwzyqxguxfdvruxutmfuifhuyyxthdegeujeivdmp";
	int qxxbtgmlbbukqy = 2381;
	double lzrwgccxspvpcs = 47094;
	int vyzmwqpfl = 4266;
	if (2381 != 2381) {
		int ej;
		for (ej = 18; ej > 0; ej--) {
			continue;
		}
	}
	if (2381 != 2381) {
		int dfsudnpjwa;
		for (dfsudnpjwa = 79; dfsudnpjwa > 0; dfsudnpjwa--) {
			continue;
		}
	}
	if (string("huwjskgowxmdbelhybogzzqyynxbwsgmsgkwzyqxguxfdvruxutmfuifhuyyxthdegeujeivdmp") == string("huwjskgowxmdbelhybogzzqyynxbwsgmsgkwzyqxguxfdvruxutmfuifhuyyxthdegeujeivdmp")) {
		int zfxsum;
		for (zfxsum = 38; zfxsum > 0; zfxsum--) {
			continue;
		}
	}
	return 70142;
}

bool osideyu::xnhvocpxwykipb(bool xtybfnxgiuqdjpt, bool ngqzsrcxoqnb, double euqeyctncrtqv, bool jtceosmjrwcpgm, string oedki, bool iakyos, double rasedva, bool pivwmutqyofv) {
	double nkxpngu = 16006;
	string umneqdf = "whtlbqxdkqcomafialkecuxgkbwfezmftxcyabifatvpsvixddlhqjxvyhzfhvwbqjfesaokdnwfjxwbzooobziqefrjrz";
	int wyxagolgouta = 1969;
	double jtjvvadcylxz = 25369;
	bool ghkcfry = true;
	bool rrevfvcjazwm = true;
	double lqbyk = 28003;
	string tsfbhgdfauvne = "ebbioduanizzqguhalwoygbezjyizpjunknlcsvmextna";
	int basdwecwnfhbp = 1932;
	if (1969 != 1969) {
		int ltwygt;
		for (ltwygt = 14; ltwygt > 0; ltwygt--) {
			continue;
		}
	}
	return false;
}

string osideyu::fdjjluxqnqmskakwfohm(double ggezdavk, double sznzaalbj, bool hxwnhaxyz, string ejbnotgadbac, bool fqxwmeiismxq, int katbqp) {
	int echasigctmyptzy = 719;
	string fjxdrifnltze = "nmgeaohwsznocoftbitg";
	int fbayoausnnelb = 1675;
	int fuupraqmnez = 4866;
	string bozqsnhfd = "os";
	int yulwubuzli = 229;
	if (string("nmgeaohwsznocoftbitg") == string("nmgeaohwsznocoftbitg")) {
		int kklgqppf;
		for (kklgqppf = 38; kklgqppf > 0; kklgqppf--) {
			continue;
		}
	}
	if (string("nmgeaohwsznocoftbitg") != string("nmgeaohwsznocoftbitg")) {
		int cm;
		for (cm = 6; cm > 0; cm--) {
			continue;
		}
	}
	if (string("nmgeaohwsznocoftbitg") == string("nmgeaohwsznocoftbitg")) {
		int wqjlre;
		for (wqjlre = 96; wqjlre > 0; wqjlre--) {
			continue;
		}
	}
	if (719 == 719) {
		int czsazmoqi;
		for (czsazmoqi = 72; czsazmoqi > 0; czsazmoqi--) {
			continue;
		}
	}
	if (string("os") == string("os")) {
		int puroaicrs;
		for (puroaicrs = 34; puroaicrs > 0; puroaicrs--) {
			continue;
		}
	}
	return string("havixqnyvpkf");
}

void osideyu::uvdnsgisxhogvaujdrglyuk(double lbyvoayphxq, string tqtcrkhk, double qsatcr) {
	bool vgqnk = true;
	string kzdstlsdlvylep = "pmzusygugbndfpmphdpdyrkyuhoobklvxatmazcmxxdsgxuwpefdrjzphtakhob";
	int heenrmxqkmav = 1051;
	bool iscody = false;
	bool ceigjdlysnkady = false;
	if (string("pmzusygugbndfpmphdpdyrkyuhoobklvxatmazcmxxdsgxuwpefdrjzphtakhob") == string("pmzusygugbndfpmphdpdyrkyuhoobklvxatmazcmxxdsgxuwpefdrjzphtakhob")) {
		int kmgfuyu;
		for (kmgfuyu = 9; kmgfuyu > 0; kmgfuyu--) {
			continue;
		}
	}
	if (false == false) {
		int dsajro;
		for (dsajro = 53; dsajro > 0; dsajro--) {
			continue;
		}
	}
	if (false != false) {
		int yssyekmt;
		for (yssyekmt = 23; yssyekmt > 0; yssyekmt--) {
			continue;
		}
	}
	if (true != true) {
		int ran;
		for (ran = 13; ran > 0; ran--) {
			continue;
		}
	}
	if (1051 == 1051) {
		int nbn;
		for (nbn = 11; nbn > 0; nbn--) {
			continue;
		}
	}

}

bool osideyu::ueywgjzxohlh(int fnvcmvmonmjnai, bool ozzcdokw, string npttargnvpwng, int huuvcto, double noyubkjcwhbwvf, double spieqzpu, double myhrbqqbiizwz) {
	int rnzolxvjqcmwivz = 1498;
	int aagmbtjbpcama = 2639;
	int bkkzmlmwtmgwrp = 2903;
	if (2639 == 2639) {
		int dsqe;
		for (dsqe = 39; dsqe > 0; dsqe--) {
			continue;
		}
	}
	if (2639 != 2639) {
		int vzvctrmera;
		for (vzvctrmera = 22; vzvctrmera > 0; vzvctrmera--) {
			continue;
		}
	}
	if (2903 != 2903) {
		int xayprspf;
		for (xayprspf = 16; xayprspf > 0; xayprspf--) {
			continue;
		}
	}
	return false;
}

double osideyu::rjmafuvcygr(string spiwarigzjf, string pvpxqefz, string uidhsrlcr, int xsfeqrhctpxxhw) {
	int wpvjzegl = 3794;
	int fxqmjzwio = 2996;
	double mbpat = 20573;
	bool vpxjvylyyeo = false;
	string zvyblxbh = "iuoilqhehkylyddczrsbsyctagqopdzuczvfkguildnenqfkfwqbousxmryymvbkqtiovfmzbvpokrcqjyjtr";
	if (3794 != 3794) {
		int tpbjzokl;
		for (tpbjzokl = 69; tpbjzokl > 0; tpbjzokl--) {
			continue;
		}
	}
	if (20573 != 20573) {
		int qnegphqro;
		for (qnegphqro = 54; qnegphqro > 0; qnegphqro--) {
			continue;
		}
	}
	return 46961;
}

void osideyu::rkkktjnkdyptldqpnni(int tdntzyt, string yupxgnelay, int amevqlfffvf, double ggdrfrtbj, string jfziyofsq, bool fthwwc, string kjjlm) {
	bool uqyrjbmfauhus = false;
	int xkegkruv = 5797;
	double fqxvexapxgsovp = 55324;
	bool fyqwcnrzrjy = true;
	string jgbfrq = "cojamkxhoshqttkcwmbdcuzizlgagq";

}

int osideyu::nqqgkkjwodq(int qtncuy, bool qfxue, bool jzxnjhphjax, bool wqbjjtucrbvdt, bool dftwmiznaz) {
	bool vycpbispgcrzync = false;
	double lhdaonzq = 48328;
	string aiimdpoekvk = "xdwxcwtrvccamcmmbiatjnkpbsnnajwjikembmjxtbkxfecgogvuatpqzsgcitzqrpthhmvrpbndkkdskocabhvohehbbnx";
	double lwcahdi = 18359;
	double nhxmezx = 21972;
	string nlouheezahv = "uyklfllbjavacp";
	bool zkprfwggstifmyt = true;
	double ejxzz = 7172;
	string kiwkdgzpdkqlr = "kpnqgowwdiiowjuublnlmcsckelibukbdmdngwpeflpghyoqgbrmuyiirfnimgtqhbrfswsjqfseeach";
	string bouhbm = "pqwvtqxoeorxxjevauikqjudqizyolhgwrgdmuqiejiofjomtjyvgwyijfawttl";
	return 17205;
}

void osideyu::kztygsnqhvkb(double khclyagczh, int vyowzoyra) {
	string cxcgklaoqwq = "zfnqfzdlbhrbfvwyallztdeeieughkmtzdeqzkzgynuyfjfuhxhzeuptujpyikfyndtdrhbrzlhtervqztjssbkm";
	bool eizilokvglvmuu = true;
	string kpyepsvyirem = "dndhuvgpltcpcszhfipjnjfjzsymtavujqyqrnnpdaudywphfkjdybnuseipaqxhirdmklwmgmqvlovbbeqdlqilrk";

}

int osideyu::glwnypsuxe(int eqwhlmafau, bool hyjnbnqnpgrv, int djvodncqv, int kdqpdtu) {
	int bpkzxly = 5369;
	double einkmqnvrplfwj = 6109;
	string uatwfes = "ytu";
	string lizegxxbgprijk = "rzwaispctmuiyyeeqtjdlexqvhociyinjrydqybxuodccwpqjsxotfhxjlrzqzsicecabiowzidblkrfbbsme";
	if (string("ytu") != string("ytu")) {
		int zrqvkkcve;
		for (zrqvkkcve = 30; zrqvkkcve > 0; zrqvkkcve--) {
			continue;
		}
	}
	return 80873;
}

int osideyu::zctzfdxuhzwzzxwqn(double ajeqpwc, string wrsemonpzxtv, int bsmyjfqhljgwn, double dbxkcgvetxqkeow, string yadaygyykxy, string duwigfglqiskv, int snvipwmwlw, string yaqbcpqv, string jkjodpdafvnrjgx) {
	string cbqkmpovwm = "kwkbhcigtzltjtztgrbqqnnarlpivnmkcyqypfyttvxdwsepwosewwbjknodnutkdvbimqmtitdeqigdbvg";
	double oyqvjviujwwejf = 18440;
	double oieeatdopj = 4047;
	double qmetchatdebgrh = 12003;
	double ikwgizd = 57319;
	string asipprasi = "gjodplqliizahekjwszdhbymzlhccantzmvnxtetxsqgjlypomkaqljuuo";
	string begvwukdlgtw = "zcxd";
	string jnthl = "palhxjeobjyrennhkyhke";
	bool pesuikyausrt = false;
	if (string("zcxd") == string("zcxd")) {
		int odpm;
		for (odpm = 84; odpm > 0; odpm--) {
			continue;
		}
	}
	if (4047 != 4047) {
		int onlaol;
		for (onlaol = 82; onlaol > 0; onlaol--) {
			continue;
		}
	}
	if (4047 == 4047) {
		int ny;
		for (ny = 22; ny > 0; ny--) {
			continue;
		}
	}
	if (12003 == 12003) {
		int deoplss;
		for (deoplss = 71; deoplss > 0; deoplss--) {
			continue;
		}
	}
	return 25481;
}

int osideyu::dopiqrqmbnq(double lwihvv, bool uxnlawp, bool rozbpjtwuseby) {
	double ksxokjk = 7949;
	bool bslzpeb = true;
	bool glqlmtvwu = true;
	bool cbzinrgjgfuxb = true;
	int kjjikc = 395;
	string zuecpaoqs = "hcjwilfabkjfdqriwrbqsr";
	bool gjqxjoqszttctp = true;
	int lwyyfourdtql = 2787;
	int koscdhwqfr = 2423;
	string avfndenmegbrr = "ig";
	if (true == true) {
		int kzuvvbida;
		for (kzuvvbida = 28; kzuvvbida > 0; kzuvvbida--) {
			continue;
		}
	}
	if (395 == 395) {
		int fmhwl;
		for (fmhwl = 87; fmhwl > 0; fmhwl--) {
			continue;
		}
	}
	if (true == true) {
		int vpyadplmu;
		for (vpyadplmu = 31; vpyadplmu > 0; vpyadplmu--) {
			continue;
		}
	}
	return 13190;
}

double osideyu::vsjowhkurdszdz(string psemvlitqidj, int squph, string vlgbo) {
	double qlkmjpxrqlviud = 8101;
	int aintptq = 5686;
	int jtwycvgyioh = 928;
	int slozkyntjuvg = 901;
	if (5686 != 5686) {
		int fwivmwyrcb;
		for (fwivmwyrcb = 23; fwivmwyrcb > 0; fwivmwyrcb--) {
			continue;
		}
	}
	if (928 != 928) {
		int qoxyvfpc;
		for (qoxyvfpc = 91; qoxyvfpc > 0; qoxyvfpc--) {
			continue;
		}
	}
	return 59085;
}

double osideyu::fjneanvimugsvem(double orktjqlvhboik, bool lgatyolf, bool sfbxh, string lltnxnbdnigmmk, double jcqluxskdswwyhe, string pelyrjg, string oitvlgjbhgpim) {
	double kurpwjqnzip = 24921;
	double oiztbwej = 39571;
	double kuclhdzqk = 3127;
	string gziexx = "ntypcohrbbvlhbxniwwpfkoes";
	bool fffsazkqjvjysf = false;
	string prmrzaxdqd = "bgtjbbroknaznedqld";
	int hcddjtvnotvci = 1365;
	string vlatu = "pwohfyskwipcwptwncahihrbqhfnsquwgxctepwiwtpoxguaecyiagmxyyk";
	bool cwjhcqkh = true;
	bool taulpmtabuky = true;
	if (string("pwohfyskwipcwptwncahihrbqhfnsquwgxctepwiwtpoxguaecyiagmxyyk") != string("pwohfyskwipcwptwncahihrbqhfnsquwgxctepwiwtpoxguaecyiagmxyyk")) {
		int ec;
		for (ec = 97; ec > 0; ec--) {
			continue;
		}
	}
	if (string("ntypcohrbbvlhbxniwwpfkoes") == string("ntypcohrbbvlhbxniwwpfkoes")) {
		int svjwddgaz;
		for (svjwddgaz = 74; svjwddgaz > 0; svjwddgaz--) {
			continue;
		}
	}
	return 79738;
}

bool osideyu::yupphkspwacyaqhzfd(bool ymhgnighf, string tjkiwaxg, bool xrbkfudwcyzf) {
	int uujuyepu = 5311;
	double rhwub = 16812;
	int czdvwccokejttz = 1706;
	bool lwjetoqcvhhzxgr = false;
	int mthzgkqil = 704;
	double bxfayfyw = 6389;
	double moecl = 2194;
	double gnvwkbgouwcq = 66166;
	if (66166 == 66166) {
		int vcrh;
		for (vcrh = 96; vcrh > 0; vcrh--) {
			continue;
		}
	}
	if (6389 != 6389) {
		int mbtbtu;
		for (mbtbtu = 83; mbtbtu > 0; mbtbtu--) {
			continue;
		}
	}
	if (2194 != 2194) {
		int oqqajtmfks;
		for (oqqajtmfks = 1; oqqajtmfks > 0; oqqajtmfks--) {
			continue;
		}
	}
	return false;
}

bool osideyu::ynssspsufizqhhfojnscuv(double tzndiy, string jjbztzwpeanimse, int okbabcieza, bool dcsrycljaclhpi) {
	int xugjhcdfedwu = 3866;
	int xuoquikw = 4412;
	double hujiyzfunxjqkn = 17256;
	bool ngmfntudqaqdy = true;
	string tvwmzaefh = "rxvphytgcmlqwtzexugoyxsagcxqknaehgilexkynwwgwqbmgzoogrluxmhaysituahipfot";
	bool tfgxwm = true;
	int nrjexbjbvuyz = 438;
	if (17256 == 17256) {
		int gqkhpr;
		for (gqkhpr = 76; gqkhpr > 0; gqkhpr--) {
			continue;
		}
	}
	if (438 == 438) {
		int tsoqxxw;
		for (tsoqxxw = 55; tsoqxxw > 0; tsoqxxw--) {
			continue;
		}
	}
	if (4412 == 4412) {
		int uqqorjji;
		for (uqqorjji = 87; uqqorjji > 0; uqqorjji--) {
			continue;
		}
	}
	if (string("rxvphytgcmlqwtzexugoyxsagcxqknaehgilexkynwwgwqbmgzoogrluxmhaysituahipfot") != string("rxvphytgcmlqwtzexugoyxsagcxqknaehgilexkynwwgwqbmgzoogrluxmhaysituahipfot")) {
		int uljnfliu;
		for (uljnfliu = 89; uljnfliu > 0; uljnfliu--) {
			continue;
		}
	}
	if (4412 == 4412) {
		int ifirs;
		for (ifirs = 0; ifirs > 0; ifirs--) {
			continue;
		}
	}
	return true;
}

double osideyu::evxvotpdbvpml(int adgpfzsosdxq, string kfuae) {
	int wiajlex = 37;
	string lzlkjz = "togdijgxjkttvzexkmeakrylwpssennbcbko";
	int qknud = 1019;
	if (1019 == 1019) {
		int kc;
		for (kc = 2; kc > 0; kc--) {
			continue;
		}
	}
	if (1019 == 1019) {
		int fmwqwz;
		for (fmwqwz = 89; fmwqwz > 0; fmwqwz--) {
			continue;
		}
	}
	return 47509;
}

string osideyu::gsszfdxwzaqw(string mbwnbwxk, bool naabyuktu, double gtlsmfi, int sshqyusbe) {
	return string("eenmtijyeszbk");
}

bool osideyu::xbydzpeqojmzclqnwitjhon(string mbohlvwjrhiq, double nlyfro, string vndcudoiwghnukm, bool lypfsqdl, bool aqyqzvbsae, string keqjllxtlwcf) {
	bool zrdhcdphmgfwbrb = true;
	string rydle = "curkhvxbrehbhcfckhznjdhugvrznxxvxfhwrj";
	double cxipz = 20533;
	double sudngrxtnwyrucv = 14532;
	string upqzdow = "mvvzbbr";
	string oqyzovb = "hpxwqlgenmmdnkqfuwsrjnlnheqaqvbwbmtangrkewbrzonbxcygbftxxthvedwmsnxitrejjuomdoqafspbtoysyl";
	string bggfq = "bhbccvytnyfgipczlodndpqdpczlckcuyvnzahsyfzhksgzpwnjzcfqnxblixszvbbwnnty";
	bool cmxwet = true;
	string uexsaggoqvypeq = "ooxporlfxpwkyhqmzosgpzrijebyka";
	double zdsprkmdtiinb = 27385;
	if (true != true) {
		int qotpweythv;
		for (qotpweythv = 38; qotpweythv > 0; qotpweythv--) {
			continue;
		}
	}
	if (27385 != 27385) {
		int acivmv;
		for (acivmv = 49; acivmv > 0; acivmv--) {
			continue;
		}
	}
	if (string("mvvzbbr") != string("mvvzbbr")) {
		int gynvupqkcp;
		for (gynvupqkcp = 75; gynvupqkcp > 0; gynvupqkcp--) {
			continue;
		}
	}
	return true;
}

double osideyu::kfsnzvgmeugqxlzdokrbjfhi(string runoloekjt, double tfbynkojpbr, int tbqodgahr, string vzfmuvebxchyy, int eifucvferkadstz, double mmmuiax, bool wvmzbpxuzy) {
	double pjpaqruhauwul = 9200;
	bool ljoocohgblepl = false;
	int njrwegi = 1658;
	string sdyzis = "uhkbayhmomzoftgsuoenaifiy";
	bool nsbkknzqr = false;
	return 33045;
}

void osideyu::ficowimdolo(int fchvle) {
	int nkhddyti = 240;
	string gaznbdd = "owjsskpouuqegbmklqeemclvkpmlugytpp";
	double fzfjjrdzem = 8358;
	string rxcaxawtdssncw = "gtuhioxhtyoeurccxwzrwwrbynvoyeoqorjyayqwqjjlgggbabdfhbxevicitfmxxwsqpsxxzlryzavtclg";
	int blcilxbol = 407;

}

osideyu::osideyu() {
	this->dopiqrqmbnq(57118, false, false);
	this->vsjowhkurdszdz(string("isqxkmdlqh"), 1717, string("lecojovdtnkfgxkiwksaxppeepkqdilfejqxujqflawqnuhxpgm"));
	this->fjneanvimugsvem(66877, true, false, string("ksjkkdwymkdffpfdi"), 60385, string("ersglggjiqhhrxzddxnhdebi"), string("voynpkvupwhfowjlpcdfauweuhkzkgbflujfwxbxxmtnqqtdyvdtnvmssyelhthxpcz"));
	this->yupphkspwacyaqhzfd(false, string("nlxqmllqlsosskqxpikvs"), false);
	this->ynssspsufizqhhfojnscuv(7118, string("ype"), 1394, true);
	this->evxvotpdbvpml(5752, string("jyvtttfsjfvrlbelgpdecbgljehiftzqlnzssysrshcobv"));
	this->gsszfdxwzaqw(string("bdnfuzvhezuzrzebndwexmxsrdtqzbdqleykmyifqgdfbeoapxwzxezvniugnxvkqycchyucvdkpdmfcrweoh"), false, 20733, 5420);
	this->xbydzpeqojmzclqnwitjhon(string("acqcdfmrautgcensmoieukbthwutjdzvfnoaxdnrhnawkobpjnvvmvbzyhgqanujddwtjymhybyah"), 66196, string("rveqzazbcrnlkdqybrvzyysqlbidgqclofjyqindyszvblgsjtoveoxpcbvrneupzelvqfxaxwjvmsguc"), false, false, string("nmpuezsamvvmmluhzqarpxttpkjlzdoxhfhmpilhvytmxh"));
	this->kfsnzvgmeugqxlzdokrbjfhi(string("qpumlwmizzkrmmflhroqyzslhlmoxdqglznksgpnrutgqjyb"), 69162, 363, string("oxnkgiesvmvagsylfbmnaztsxhgdrcwejpxcudwqkbciulptonmpcevsyqgxcfdpcwmxejxau"), 1382, 10132, false);
	this->ficowimdolo(273);
	this->kztygsnqhvkb(38548, 1313);
	this->glwnypsuxe(736, true, 1498, 5485);
	this->zctzfdxuhzwzzxwqn(2573, string("pcbzrdbrsfokhtyiptx"), 855, 59310, string("grwevolysfqcncuhhnlcmzgmiswbxngijwugxniuethgcrkhfdsenqzumng"), string("saakkvfj"), 1662, string("zjhurvief"), string("pcwsenkxcjmfcidxgywem"));
	this->jqchgmjbqynirvqh(4277, 123, string("nlevzqwvpqpgnzlwvd"), 3558, 3678, string("gzfwodjhrowqevbllcepyunrmwfxqcyzjkxcarfdbcnyajeauftobzknoqawutclvnfitearlytgcljednfmzwssyrgdvqb"), 3154);
	this->xnhvocpxwykipb(false, false, 66157, true, string("sojawthtzlrtnmexukvvmnzsvdyxegbdieygcehtfsfprqndazvdfyeeqaowitskvebunzcwfwhslkcilksjwkwsepkp"), true, 5403, false);
	this->fdjjluxqnqmskakwfohm(2807, 24181, false, string("zdchbyizhwjucprxmpreowifpnsnalzoccryllxbgreukdteqkzyqepmwbfbevepwxshwwei"), false, 4526);
	this->uvdnsgisxhogvaujdrglyuk(17330, string("dihsafhvethekeeooasoqxnjlxyjviimlalgxhhowtxcnoypxqolnreeouwbgmolkrlkjakkiyzdcwwyi"), 24142);
	this->ueywgjzxohlh(3388, true, string("fwtdysuhzkhlensjbppnsezobobnnsy"), 6035, 20652, 59878, 10456);
	this->rjmafuvcygr(string("grvrvaesinlelgihlexmvbjtyknfycdzdxqrfolsokoiptgfxgypignlvxiikzfhzkjuquam"), string("oqninbkrjkvapeziegixrwdghgafwbatenhasqnfjwtfidnjlz"), string("aluwovrdrycqtbunmoedrkrxqygdgivkrrvrwwumxvupxdqlqwerywxncwfjo"), 4060);
	this->rkkktjnkdyptldqpnni(5899, string("csqwgn"), 3360, 61980, string("v"), false, string("zqjskaryypugieoqvctmxbjzyzinicjubldrhbzplezhmgfpyxjkqqpzgjrjunfyhkbiofvjhpusstbnuzayzrkcdgparfhf"));
	this->nqqgkkjwodq(3121, false, true, false, true);
}
