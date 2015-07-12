/*****************************************************************************
*							 GCL - system.h                                  *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"
#include "graphics.h"
#include "window.h"
#include "win32_dlgs.h"

#ifndef SYSTEM_D2D_H
#define SYSTEM_D2D_H
#ifdef _MSC_VER
#	pragma once
#endif

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
	app(const app&&) = delete;
	~app() = delete;

	template <typename _gr> static typename enable_if<is_base_of<graphics, _gr>::value, int>::type
	run(ui::window* w_p)
	{
		if(app_started)
			throw invalid_argument("Unable to start app twice");
		if(FAILED(CoInitialize(0)))
			throw runtime_error("Unable to initialize COM");
		if(FAILED(OleInitialize(0)))
			throw runtime_error("Unable to initialize OLE"); // OLE
		w = w_p;
		w->handle_created += [] (HWND hWnd)
		{
			g = new _gr(hWnd, w->size_changed);
			g->set_text_rendering_mode(text_rendering_modes::cleartype);
			g->set_antialias(false);
			w->set_graphics(g);
		};
		app_started = true;
		int num = EXIT_FAILURE;
		try
		{
			num = w->show();
		}
		catch(...)
		{
			delete w_p;
			w_p = w = 0;
			delete g;
			g = 0;
			CoUninitialize();
			OleUninitialize();
			throw;
		}
		delete w_p;
		w_p = w = 0;
		delete g;
		g = 0;
		if(num != EXIT_SUCCESS)
			ui::msg_box::show(L"Window exited with code: " + to_wstring(num), L"Unsuccessfull termination", ui::msg_box::ok, ui::msg_box::error); 
		CoUninitialize();
		OleUninitialize();
		return num;
	}
	static void init();
	static HINSTANCE get_instance() {return inst;}
	static bool is_composition_enabled() {return IsCompositionActive() != 0;}
	static bool is_high_contrast_app();
private:
	static graphics* g;
	static ui::window* w;
	static bool app_started;

	static HINSTANCE inst;
};

};
#endif
