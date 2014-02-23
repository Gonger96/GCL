#include "stdafx.h"
#include "input.h"

namespace gcl { namespace input {

point mouse::get_position()
{
	POINT p = {};
	GetCursorPos(&p);
	return point(static_cast<float>(p.x), static_cast<float>(p.y));
}

point mouse::get_client_position(HWND client)
{
	POINT p = {};
	GetCursorPos(&p);
	MapWindowPoints(GetDesktopWindow(), client, &p, 1);
	return point(static_cast<float>(p.x), static_cast<float>(p.y));
}

void mouse::set_position(const point& p)
{
	SetCursorPos(static_cast<int>(p.x), static_cast<int>(p.y));
}

void mouse::set_client_position(HWND client, const point& p)
{
	POINT pf = {static_cast<int>(p.x), static_cast<int>(p.y)};
	MapWindowPoints(client, GetDesktopWindow(), &pf, 1);
	SetCursorPos(pf.x, pf.y);
}

void mouse::hide() {ShowCursor(FALSE);};

void mouse::show() {ShowCursor(TRUE);};

}; 
};