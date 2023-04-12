#include "utils.hpp"

namespace Utils {
	int DisplayMessageBox(const char* text, const char* title, UINT type) {
		return MessageBoxA(NULL, text, title, type | MB_TOPMOST);
	}

	const char* VA(const char* Format, ...) {
		static char buffer[0x300]; 
		ZeroMemory(buffer, 0x300);
		va_list ap;
		va_start(ap, Format);
		vsprintf_s(buffer, Format, ap);
		va_end(ap);
		return buffer;
	}

	wchar_t* WVA(const wchar_t* Format, ...) {
		static wchar_t buffer[0x300]; 
		ZeroMemory(buffer, 0x300);
		va_list ap;
		va_start(ap, Format);
		vswprintf_s(buffer, Format, ap);
		va_end(ap);
		return buffer;
	}

	std::wstring CMDCommandWithOutputW(std::wstring cmd) {
		std::wstring output = L"";

		std::FILE* handle = _wpopen(cmd.c_str(), L"r");

		if (handle == nullptr)
			return L"";

		wchar_t line[1000];
		while (std::fgetws(line, 1000, handle) != nullptr) {
			output += line;
		}

		_pclose(handle);

		return output;
	}

	std::string CMDCommandWithOutputA(std::string cmd) {
		auto out = CMDCommandWithOutputW(std::wstring(cmd.begin(), cmd.end()));
		return std::string(out.begin(), out.end());
	}

	std::string RemoveStringStartPaddingA(std::string str) {
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == L' ') {
				str.erase(0, i);
				continue;
			}
			break;
		}

		return str;
	}

	std::wstring RemoveStringStartPaddingW(std::wstring wstr) {
		for (int i = 0; i < wstr.length(); i++) {
			if (wstr[i] == L' ') {
				wstr.erase(0, i * 2);
				continue;
			}
			break;
		}

		return wstr;
	}
}