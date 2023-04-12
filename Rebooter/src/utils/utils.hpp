#pragma once
#include "pch.hpp"

namespace Utils {
	int DisplayMessageBox(const char* text, const char* title = "Message Box", UINT type = MB_OK);

	const char* VA(const char* Format, ...);
	wchar_t* WVA(const wchar_t* Format, ...);

	std::string CMDCommandWithOutputA(std::string cmd);
	std::wstring CMDCommandWithOutputW(std::wstring cmd);

	std::string RemoveStringStartPaddingA(std::string str);
	std::wstring RemoveStringStartPaddingW(std::wstring wstr);
}