/*****************************************************************************
*                           gcl - system.h                                   *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#ifndef SYSTEM_D2D_H
#define SYSTEM_D2D_H
#ifdef _MSC_VER
#pragma once
#endif
#include "stdafx.h"
#include "graphics.h"
#include "window.h"
#include "win32_dlgs.h"

namespace gcl {

namespace resource
{
	HRSRC alloc(HINSTANCE inst, int id, LPWSTR type, void** data);
	void free(HRSRC res);
};

namespace desktop
{
	size get_size();
};

class app final
{
public:
	app() = delete;
	app(const app&) = delete;
	~app() = delete;

	template <typename _gr> static typename enable_if<is_base_of<graphics, _gr>::value, int>::type
	start(ui::window& w_p)
	{
	if(app_started)
		throw invalid_argument("Unable to start app twice");
	w = & w_p;
	w->handle_created += [] (HWND hWnd)
	{
		g = new _gr(hWnd, w->size_changed);
		w->set_graphics(g);
	};
	app_started = true;
	int num = EXIT_FAILURE;
	try
	{
		num = w->show_dialog();
	}
	catch(const exception& e)
	{
		ui::msg_box::show(string_to_unicode_string<char>(e.what()), L"An Exception was thrown", ui::msg_box::ok, ui::msg_box::error);
		return EXIT_FAILURE;
	}
	delete g;
	g = 0;
	if(num != EXIT_SUCCESS)
		ui::msg_box::show(L"Window exited with code: " + to_wstring(num), L"An Exception was thrown", ui::msg_box::ok, ui::msg_box::error); 
	return num;
}
	static void init();
	static vector<wstring>& get_commandline() {return cmd_line;};
	static HINSTANCE get_instance() {return inst;};
private:
	static graphics* g;
	static ui::window* w;
	static bool app_started;

	static HINSTANCE inst;
	static vector<wstring> cmd_line;
};

namespace timers {
using namespace chrono;



};

};

class not_implemented : public exception
{
public:
	not_implemented() : exception("The required function isn't available") {};
};

#endif
