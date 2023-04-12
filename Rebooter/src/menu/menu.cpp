#include "menu.hpp"
#include "utils/utils.hpp"

namespace Menu {
#pragma region Tray Window
    std::vector<TrayWindow*>vTrayWindows;

    void Cleanup() { //Close and cleanup all created system tray menus
        for (auto item : vTrayWindows) {
            if (item)
                delete item;
        }
        vTrayWindows.clear();
        PostQuitMessage(0);
    }

    void Run() {
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) 
                break;
        }
    }

    TrayWindow* GetTrayMenuFromHWND(HWND hwnd) {
        for (auto item : vTrayWindows) {
            if (item->GetHWND() == hwnd)
                return item;
        }
        return nullptr;
    }

    LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { //used for all created windows
        auto ptr = GetTrayMenuFromHWND(hwnd);
        if (ptr != nullptr) {
            if (LOWORD(lParam) == WM_RBUTTONUP) {
                ptr->WndProcHandle();
            }
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void TrayWindow::WndProcHandle() {
        POINT pt;
        GetCursorPos(&pt);

        SetForegroundWindow(hwnd);

        auto selection = TrackPopupMenu(GetMenu()->GetHMenu(), TPM_RIGHTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
        if (selection > 0) {
            auto function = OptionCallbacks[selection];
            if (function)
                function(selection);
        }
    }

	bool TrayWindow::Initilize(int iconid) {
        WNDCLASSEXA wcex = { 0 };
        wcex.cbSize = sizeof(wcex);
        wcex.lpfnWndProc = GlobalWndProc;
        wcex.hInstance = NULL;
        wcex.lpszClassName = className.c_str();
        wcex.lpszMenuName = windowName.c_str();

        if (!RegisterClassExA(&wcex)) {
            Utils::DisplayMessageBox(Utils::VA("Failed to register class 0x%x.", GetLastError()), "Error", MB_ICONERROR);
            return false;
        }

        hwnd = CreateWindowExA(0, className.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);
        if (hwnd == 0) {
            Utils::DisplayMessageBox(Utils::VA("Failed to create window 0x%x.", GetLastError()), "Error", MB_ICONERROR);
            return false;
        }


        nid.cbSize = sizeof(NOTIFYICONDATAA);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER + 1;

        if (iconid != 32512)
            nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(iconid));
        else 
            nid.hIcon = LoadIcon(nullptr, MAKEINTRESOURCE(32512));

        strcpy_s(nid.szTip, windowName.c_str());

        if (!Shell_NotifyIconA(NIM_ADD, &nid)) {
            Utils::DisplayMessageBox(Utils::VA("Failed to add tray icon 0x%x.", GetLastError()), "Error", MB_ICONERROR);
            return false;
        }

        pMenu = new SubMenu(this, CreatePopupMenu());
		return true;
	}
#pragma endregion

#pragma region Submenu
    void SubMenu::ClearOptions() {
        pParentWindow->ClearCallbackRange((int)hMenu, iOptionCount);
        while (GetMenuItemCount(hMenu) > 0) 
            RemoveMenu(hMenu, 0, MF_BYPOSITION);
    }

    int SubMenu::AddOption(std::string name, std::function<void(int)> function) {
        iOptionCount++;
        int id = (int)hMenu + iOptionCount;
        AppendMenuA(hMenu, MF_STRING, id, name.c_str());
        pParentWindow->AddCallback(id, function);
        return id;
    }

    void SubMenu::AddBreak() {
        AppendMenuA(hMenu, MF_SEPARATOR, 0, nullptr);
    }

    void SubMenu::UpdateOptionFlags(int id, UINT flags) {
        MENUITEMINFOA itemInfo = {};
        GetMenuItemInfoA(hMenu, id, TRUE, &itemInfo);
        itemInfo.fState = flags;
        SetMenuItemInfoA(hMenu, id, TRUE, &itemInfo);
    }

    bool SubMenu::Toggle(int id) {
        auto item = GetItemFromID(id);
        bool checked = (item.fState & MF_CHECKED);
        if (checked)
            item.fState &= ~MF_CHECKED;
        else
            item.fState |= MF_CHECKED;

        SetItemInfoFromID(id, item);
        return !checked; 
    }

    MENUITEMINFOA SubMenu::GetItemFromID(int id) {
        MENUITEMINFOA itemInfo = {};
        itemInfo.cbSize = sizeof(MENUITEMINFOA);
        itemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA | MIIM_STATE | MIIM_FTYPE;
        for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
            if (GetMenuItemInfoA(hMenu, i, TRUE, &itemInfo)) {
                if (itemInfo.wID == id) {
                    itemInfo.cch = GetMenuStringA(hMenu, i, NULL, 0, MF_BYPOSITION);
                    itemInfo.dwTypeData = new char[itemInfo.cch + 1]; //remember to free
                    GetMenuStringA(hMenu, i, itemInfo.dwTypeData, itemInfo.cch + 1, MF_BYPOSITION);
                    return itemInfo;
                }
            }
        }

        return {};
    }

    void SubMenu::SetItemInfoFromID(int id, MENUITEMINFOA iteminfo) {
        MENUITEMINFOA itemInfo = {};
        itemInfo.cbSize = sizeof(MENUITEMINFOA);
        itemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA | MIIM_STATE | MIIM_FTYPE;
        for (int i = 0; i < GetMenuItemCount(hMenu); i++) {
            GetMenuItemInfoA(hMenu, i, TRUE, &itemInfo);
            if (itemInfo.wID == id) {
                SetMenuItemInfoA(hMenu, i, TRUE, &iteminfo);
                if (iteminfo.dwTypeData)
                    delete[] iteminfo.dwTypeData;
                break;
            }
        }
    }

    SubMenu* SubMenu::AddSubmenu(std::string name) {
        auto ptr = new SubMenu(pParentWindow, CreatePopupMenu());
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)ptr->GetHMenu(), name.c_str());
        vChildSubMenus.push_back(ptr);
        return ptr;
    }
#pragma endregion
}