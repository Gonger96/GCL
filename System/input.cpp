#include "stdafx.h"
#include "input.h"

namespace gcl { namespace input {

// Cursor
point cursor::get_position()
{
	POINT p = {};
	GetCursorPos(&p);
	return point(static_cast<float>(p.x), static_cast<float>(p.y));
}

point cursor::get_client_position(HWND client)
{
	POINT p = {};
	GetCursorPos(&p);
	MapWindowPoints(GetDesktopWindow(), client, &p, 1);
	return point(static_cast<float>(p.x), static_cast<float>(p.y));
}

void cursor::set_position(const point& p)
{
	SetCursorPos(static_cast<int>(p.x), static_cast<int>(p.y));
}

void cursor::set_client_position(HWND client, const point& p)
{
	POINT pf = {static_cast<int>(p.x), static_cast<int>(p.y)};
	MapWindowPoints(client, GetDesktopWindow(), &pf, 1);
	SetCursorPos(pf.x, pf.y);
}

void cursor::hide() {ShowCursor(FALSE);}

void cursor::show() {ShowCursor(TRUE);}

void cursor::set_cursor(const render_objects::cursor_surface& cur, const render_objects::system_cursor& id)
{
	SetSystemCursor(cur.get_cursor(), static_cast<DWORD>(id));
}

void cursor::swap_buttons(bool swap)
{
	SwapMouseButton(swap);
}

void cursor::set_double_click_time(const chrono::milliseconds& tme)
{
	SetDoubleClickTime(static_cast<UINT>(tme.count()));
}

chrono::milliseconds cursor::get_double_click_time()
{
	return chrono::milliseconds(GetDoubleClickTime());
}
// Cursor

// Keyboard
wstring keyboard::key_to_string(char scan_code, bool extend_, bool right_left)
{
	bitset<32> b(0x00ff0000 & (scan_code << 16));
	b.set(24, extend_);
	b.set(25, right_left);
	wchar_t buffer[256];
	GetKeyNameText(b.to_ulong(), buffer, 256);
	return wstring(buffer);
}	

void keyboard::block_input(bool block_it)
{
	BlockInput(block_it);
}

bool keyboard::get_key_state(const virtual_keys& key, bool& toggled)
{
	short s = GetKeyState(static_cast<int>(key));
	toggled = !!(s & 0x0001);
	return !!(s & 0x8000);
}

bool keyboard::get_key_state_async(const virtual_keys& key, bool& toggled)
{
	short s = GetAsyncKeyState(static_cast<int>(key));
	toggled = !!(s & 0x0001);
	return !!(s & 0x8000);
}

void keyboard::send_input(const virtual_keys& key, char scan_code, const int flags, const chrono::milliseconds& timestamp)
{
	KEYBDINPUT kbinp;
	kbinp.wVk = static_cast<int>(key);
	kbinp.wScan = scan_code;
	kbinp.dwFlags = flags;
	kbinp.time = static_cast<DWORD>(timestamp.count());
	INPUT inp;
	inp.type = INPUT_KEYBOARD;
	inp.ki = kbinp;
	if(SendInput(1, &inp, sizeof(inp)) == 0)
		throw runtime_error("Sending Keyboardinput failed");
}
// Keyboard

}; 
};