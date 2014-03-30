/*****************************************************************************
*                           gcl - window.h                                   *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#ifndef WINDOW_H
#define WINDOW_H
#ifdef _MSC_VER 
#pragma once
#endif
#ifdef max 
#undef max
#endif
#ifdef min
#undef min
#endif
#include "stdafx.h"
#include "graphics.h"
#include "callback.h"
#include "controls.h"
using namespace gcl::render_objects;

namespace gcl { namespace ui {

enum class window_borderstyles {none, fixed_single, fixed_3D, fixed_dialog, sizeable, fixed_toolwindow, sizeable_toolwindow};
enum class window_startpositions {default_location, center_screen, manual};
enum class window_states {normal, maximized, minimized};

class window : 
	public drawsurface,
		private _cl_hlp
{
public:
	window(void);
	virtual ~window();
	window(const window&) = delete;

	callback<void(const HWND)> handle_created;
	callback<void(const HWND)> handle_destroyed;

	int show_dialog(HWND hWnd = 0);
	// redraw -> überladung für i_drawsurface allein oder mit rectangle  
	virtual void render(graphics* g);
	virtual bool contains(const point& p) const;
	void redraw();
	void redraw(const rect& s);
	void layout();
	void add_surface(dynamic_drawsurface*);
	void remove_surface(dynamic_drawsurface*);
	void set_min_size(const size& s);
	void set_max_size(const size& s);
	void set_padding(const padding& p);
	
	HWND get_handle() const {return handle;};
	graphics* get_graphics() const {return _graphics;};
	void set_graphics(graphics* g);
	wstring get_title() const {return title;};
	void set_title(const wstring& s);
	colour get_colour() const {return erase_colour;};
	void set_colour(const colour& c);
	window_borderstyles get_borderstyle() const {return borderstyle;};
	void set_borderstyle(const window_borderstyles style);
	window_states get_state() const {return state;};
	void set_state(const window_states st);
	window_startpositions get_startposition() const {return startposition;};
	void set_startposition(const window_startpositions pos) {startposition = pos;};
	bool get_maximizebox() const {return maximizebox;};
	void set_maximizebox(bool b);
	bool get_minimizebox() const {return minimizebox;};
	void set_minimizebox(bool b);
	bool get_closebox() const {return closebox;};
	void set_closebox(bool b);
	size get_size() const;
	void set_size(const size& sz, bool redraw = true);
	point get_position() const;
	void set_position(const point& p, bool redraw = false);
	rect get_rect() const;
	void set_opacity(float val);
	float get_opacity() const {return opacity;};
	void set_visible(bool b);
	void set_enabled(bool b);
    void set_focus(bool);
	bool get_focus() const {return focus;};
	void set_focused_surface(dynamic_drawsurface*);
	dynamic_drawsurface* get_focused_surface() {return focused_surf;};
	void close();
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	colour erase_colour;
	bool maximizebox, minimizebox, closebox;
	graphics* _graphics;
	window_borderstyles borderstyle;
	window_startpositions startposition;
	window_states state;
	float opacity;
	bool is_mouse_over, is_mouse_dwn;
	int bottom_s, top_s, topmost_s;
	bool focus;
	dynamic_drawsurface* focused_surf;
	bool shown;
	bool has_resources;
protected:
	virtual LRESULT message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void on_create_params(unsigned long& style, unsigned long& exstyle, unsigned int& class_style, wstring& classname) {};
	virtual void create_resources(graphics*);
	HWND handle;
};


};
};
#endif