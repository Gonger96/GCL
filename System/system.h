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
	run(ui::window& w_p)
	{
		if(app_started)
			throw invalid_argument("Unable to start app twice");
		CoInitialize(0);
		w = & w_p;
		w->handle_created += [] (HWND hWnd)
		{
			g = new _gr(hWnd, w->size_changed);
			g->set_text_rendering_mode(text_rendering_modes::antialias);
			w->set_graphics(g);
		};
		app_started = true;
		int num = EXIT_FAILURE;
		try
		{
			num = w->show_dialog();
		}
		catch(...)
		{
			delete g;
			g = 0;
			CoUninitialize();
			throw;
		}
		delete g;
		g = 0;
		w = 0;
		if(num != EXIT_SUCCESS)
			ui::msg_box::show(L"Window exited with code: " + to_wstring(num), L"Unsuccessfull termination", ui::msg_box::ok, ui::msg_box::error); 
		CoUninitialize();
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
