#include "pch.hpp"
#include "resource.h"
#include "menu/menu.hpp"
#include "utils/utils.hpp"
#include "bcdedit/bcdedit.hpp"


Menu::TrayWindow* pMainTrayWindow = nullptr;
Menu::SubMenu* pMainSubmenu = nullptr;

int InitList(std::string name, bool current) { //callback for adding install to the menu
    auto optionID = pMainSubmenu->AddOption(name, [](int id) { //add the option id to storage
        BCDEdit::RebootToInstall(id);
    });

    if (current) //set a tick next to the current running install
        pMainSubmenu->Toggle(optionID);

    return optionID;
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE); //hide console window

    Sleep(3000); //fix icon registation error if auto launched on login

    if (!IsUserAnAdmin()) { //make sure running as admin (otherwise bcdedit command will fail)
        Utils::DisplayMessageBox("Application must be ran with admin perms.", "Error", MB_ICONERROR);
        return 0;
    }

    pMainTrayWindow = new Menu::TrayWindow("Simple Reboot Manager", "YEET", IDI_ICON1); //create tray window

    pMainSubmenu = pMainTrayWindow->GetMenu(); //get my submenu for the created system tray window
    if (pMainSubmenu == nullptr)
        return 0;

    if (!BCDEdit::Setup(InitList)) { //add all the entries to the menu
        Utils::DisplayMessageBox("Failed to find bootloader entries.", "Error", MB_ICONERROR);
        Menu::Cleanup();
        return 0;
    }

    pMainSubmenu->AddBreak();

    pMainSubmenu->AddOption("Exit", [](int id) { //cleans up created windows and allows program to exit
        Menu::Cleanup();
    });

    Menu::Run();

    return 0;
}

