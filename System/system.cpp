#include "stdafx.h"
#include "system.h"

namespace gcl {

HRSRC resource::alloc(HINSTANCE inst, int id, LPWSTR type, void** data)
{
	HRSRC res = FindResource(inst, MAKEINTRESOURCE(id), type);
	if(!res)
		throw invalid_argument("Unable to find resource");
	HGLOBAL resData = LoadResource(inst, res);
	if(!resData)
		throw runtime_error("Unable to load resource");
	*data = LockResource(resData);
	return res;
}

void resource::free(HRSRC res)
{
	if(FreeResource(res)) throw invalid_argument("Unable to free resource");
}

size desktop::get_size()
{
	RECT rc = {};
	GetClientRect(GetDesktopWindow(), &rc);
	return size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
}


graphics* app::g = 0;
ui::window* app::w = 0;
bool app::app_started = false;
HINSTANCE app::inst = 0;
vector<wstring> app::cmd_line = vector<wstring>();

void app::init()
{
	inst = GetModuleHandle(NULL);
	int cnt = 0;
	LPWSTR cmdw = GetCommandLineW();
	LPWSTR* lpCmdLine = CommandLineToArgvW(cmdw, &cnt);
	for(int i = 0; i < cnt; i++)
		cmd_line.push_back(wstring(lpCmdLine[i]));
	LocalFree(lpCmdLine);
	LocalFree(cmdw);
}


};