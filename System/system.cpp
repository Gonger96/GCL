#include "stdafx.h"
#include "system.h"

namespace gcl {

// Resource 
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
// Resource

// Desktop
size desktop::get_size()
{
	RECT rc = {};
	GetClientRect(GetDesktopWindow(), &rc);
	return size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
}
// Desktop

// Application
graphics* app::g = 0;
ui::window* app::w = 0;
bool app::app_started = false;
HINSTANCE app::inst = 0;

void app::init()
{
	inst = GetModuleHandle(NULL);
}

bool app::is_high_contrast_app()
{
	HIGHCONTRAST hcntr = {};
	hcntr.cbSize = sizeof(HIGHCONTRAST);
	if(!SystemParametersInfo(SPI_GETHIGHCONTRAST, 0, &hcntr, 0))
		throw runtime_error("SystemParametersInfo failed");
	return (hcntr.dwFlags & HCF_HIGHCONTRASTON) == HCF_HIGHCONTRASTON;
}
// Application

};