#pragma once
#include "pch.hpp"

namespace BCDEdit {
	struct BootloaderEntry {
		std::string identifier;
		std::string description;
		int optionID;
	};

	extern std::vector<BootloaderEntry> vBootloaders;

	BootloaderEntry* GetEntry(int optionID);

	void RebootToInstall(int optionID);

	bool Setup(std::function<int(std::string name, bool current)> callback);
}