/*****************************************************************************
*                           gcl - input.h                                    *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#include "stdafx.h"
#include "graphics.h"

#ifndef INPUT_H
#define INPUT_H
#ifdef _MSC_VER
#	pragma once
#endif

using namespace gcl;

namespace gcl { namespace input {

	namespace cursor {
	
	point get_position();
	point get_client_position(HWND client);
	void set_position(const point& p);
	void set_client_position(HWND client, const point& p);
	void hide();
	void show();
	void set_cursor(const render_objects::cursor_surface& cur, const render_objects::system_cursor& id);
	void swap_buttons(bool swap);
	void set_double_click_time(const chrono::milliseconds& tme);
	chrono::milliseconds get_double_click_time();
	};

	namespace keyboard {
		
	// Converts a key into a printable string. Set "right_left" to true to not distinguish between left and right CTRL and SHIFT keys, for example.
	wstring key_to_string(char scan_code, bool extend_, bool right_left = false);
	void block_input(bool block_it);
	bool get_key_state(const virtual_keys& key, bool& toggled);
	bool get_key_state_async(const virtual_keys& key, bool& toggled);
	namespace keyboard_input_flags {const int extended_key = KEYEVENTF_EXTENDEDKEY, key_up = KEYEVENTF_KEYUP, key_down = 0, scan_code = KEYEVENTF_SCANCODE, unicode = KEYEVENTF_UNICODE;};
	void send_input(const virtual_keys& key, char scan_code, const int flags, const chrono::milliseconds& timestamp = chrono::milliseconds(0)); 
	};

}; 
};

#endif