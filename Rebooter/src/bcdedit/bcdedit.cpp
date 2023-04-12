#include "bcdedit.hpp"
#include "utils/utils.hpp"

namespace BCDEdit {
	std::vector<BootloaderEntry> vBootloaders;

	BootloaderEntry* GetEntry(int optionID) {
		auto res = find_if(vBootloaders.begin(), vBootloaders.end(), [&](BootloaderEntry element) {
			return (element.optionID == optionID);
		});

		if (res != vBootloaders.end()) 
			return &vBootloaders[static_cast<int>(distance(vBootloaders.begin(), res))];
		return nullptr;
	}

	void RebootToInstall(int optionID) {
		auto entry = GetEntry(optionID);
		if (entry != nullptr) {
			auto out = Utils::CMDCommandWithOutputA(Utils::VA("bcdedit /bootsequence {%s}", entry->identifier.c_str())); //set entry for next boot sequence
			if (out.find("success") == std::string::npos) {
				Utils::DisplayMessageBox("Failed to set boot sequence", "Error", MB_ICONERROR);
				return;
			}

			Utils::CMDCommandWithOutputA("shutdown /r /f /t 0"); //force reboot
		}
	}

	bool Setup(std::function<int(std::string name, bool current)> callback) {
		auto cmdout = Utils::CMDCommandWithOutputA("bcdedit /enum");
		auto WBLPos = cmdout.find("Windows Boot Loader"); //issue is computer languages effect the text output
		while (WBLPos != std::string::npos) {
			BootloaderEntry entry = {};

			//find identifier
			{
				auto start = cmdout.find("{", WBLPos);
				auto end = cmdout.find("}", WBLPos);
				if (start == std::wstring::npos || end == std::wstring::npos) {
					break;
				}

				entry.identifier = cmdout.substr(start + 1, end - start - 1);
			}

			//find description
			{
				auto start = cmdout.find("description", WBLPos);
				auto end = cmdout.find("\n", start);
				if (start == std::wstring::npos || end == std::wstring::npos) {
					break;
				}

				std::string description = cmdout.substr(start + 12, end - start - 12);
				entry.description = Utils::RemoveStringStartPaddingA(description);
			}

			entry.optionID = callback(entry.description, (entry.identifier == "current"));

			vBootloaders.push_back(entry);

			WBLPos = cmdout.find("Windows Boot Loader", WBLPos + 1);
		}

		return (!vBootloaders.empty()); //make sure it found entries
	}
}