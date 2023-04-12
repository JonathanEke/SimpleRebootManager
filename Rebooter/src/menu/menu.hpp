#pragma once
#include "pch.hpp"

namespace Menu {
	class TrayWindow;
	class SubMenu;

	extern std::vector<TrayWindow*>vTrayWindows;

	class TrayWindow {
	public:
		TrayWindow(std::string windowName, std::string className, int iconid = 32512/*IDI_APPLICATION*/) {
			this->windowName = windowName;
			this->className = className;
			vTrayWindows.push_back(this);

			Initilize(iconid);
		}

		~TrayWindow() {
			OptionCallbacks.clear();

			for (auto item : vSubmenus)
				delete item;

			vSubmenus.clear();

			Shell_NotifyIconA(NIM_DELETE, &nid);
			DestroyWindow(hwnd);

			auto found = find(vTrayWindows.begin(), vTrayWindows.end(), this);
			if (found != vTrayWindows.end()) {
				vTrayWindows.erase(found);
			}
		}

		void AddCallback(int id, std::function<void(int)> function) {
			OptionCallbacks[id] = function;
		}

		void ClearCallbackRange(int start, int end) {
			auto first = OptionCallbacks.find(start);
			auto last = OptionCallbacks.find(end);
			OptionCallbacks.erase(first, last);
		}


		HWND GetHWND() {
			return hwnd;
		}

		SubMenu* GetMenu() {
			return pMenu;
		}

		void WndProcHandle();
	private:
		bool Initilize(int iconid);

		HWND hwnd;
		
		SubMenu* pMenu = nullptr;
		NOTIFYICONDATAA nid = {};

		std::string windowName;
		std::string className;

		std::unordered_map<int, std::function<void(int)>> OptionCallbacks;
		std::vector<SubMenu*> vSubmenus;
	};

	class SubMenu {
	public:
		SubMenu(TrayWindow* pParent, HMENU hMenu) {
			pParentWindow = pParent;
			this->hMenu = hMenu;
		}

		~SubMenu() {
			for (auto item : vChildSubMenus)
				delete item;
			vChildSubMenus.clear();
		}

		void ClearOptions();

		int AddOption(std::string name, std::function<void(int)> function = {});
		void AddBreak();
		SubMenu* AddSubmenu(std::string name);

		void UpdateOptionFlags(int id, UINT flags);
		bool Toggle(int id);

		HMENU GetHMenu() {
			return hMenu;
		}

	private:
		TrayWindow* pParentWindow = nullptr;
		std::vector<SubMenu*> vChildSubMenus = {};
		int iOptionCount = 0;
		HMENU hMenu = 0;

		MENUITEMINFOA GetItemFromID(int id);
		void SetItemInfoFromID(int id, MENUITEMINFOA iteminfo);
	};

	TrayWindow* GetTrayMenuFromHWND(HWND hwnd);
	void Cleanup();
	void Run();
}