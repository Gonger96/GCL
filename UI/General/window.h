/*****************************************************************************
*							 GCL - window.h                                  *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"
#include "graphics.h"
#include "callback.h"

#ifndef WINDOW_H
#define WINDOW_H
#ifdef _MSC_VER 
#	pragma once
#endif

using namespace gcl::render_objects;

namespace gcl { namespace ui {

enum class window_borderstyles {none, fixed_single, fixed_3D, fixed_dialog, sizeable, fixed_toolwindow, sizeable_toolwindow};
enum class window_startpositions {default_location, center_screen, manual};
enum class window_states {normal, maximized, minimized};
enum class flash_modes {all = FLASHW_ALL, caption = FLASHW_CAPTION, stop_timer = FLASHW_STOP, start_timer = FLASHW_TIMER, until_foreground = FLASHW_TIMERNOFG, tray = FLASHW_TRAY};

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

	virtual int show(window* win = 0, bool use_handle = false);
	virtual void render(graphics* g);
	virtual bool contains(const point& p) const;
	bool flash(bool invert);
	bool flash(const flash_modes& mode, unsigned int flash_count, const chrono::milliseconds& flash_duration = chrono::milliseconds(0));
	void redraw();
	void redraw(const rect& s);
	void layout();
	void set_min_size(const size& s);
	void set_max_size(const size& s);
	void set_padding(const padding& p);
	
	HWND get_handle() const {return handle;}
	graphics* get_graphics() const {return _graphics;}
	void set_graphics(graphics* g);
	wstring get_title() const {return title;}
	void set_title(const wstring& s);
	colour get_colour() const {return erase_colour;}
	void set_colour(const colour& c);
	window_borderstyles get_borderstyle() const {return borderstyle;}
	void set_borderstyle(const window_borderstyles style);
	window_states get_state() const {return state;}
	void set_state(const window_states st);
	window_startpositions get_startposition() const {return startposition;}
	void set_startposition(const window_startpositions pos) {startposition = pos;}
	bool get_maximizebox() const {return maximizebox;}
	void set_maximizebox(bool b);
	bool get_minimizebox() const {return minimizebox;}
	void set_minimizebox(bool b);
	bool get_closebox() const {return closebox;}
	void set_closebox(bool b);
	size get_size() const;
	void set_size(const size& sz, bool redraw = true);
	point get_position() const;
	void set_position(const point& p, bool redraw = false);
	rect get_rect() const;
	void set_opacity(float val);
	float get_opacity() const {return opacity;}
	void set_visible(bool b);
	void set_enabled(bool b);
    void set_focus(bool);
	bool get_focus() const {return focus;}
	void set_focused_surface(dynamic_drawsurface*);
	dynamic_drawsurface* get_focused_surface() {return focused_surf;}
	void close();
	icon* get_icon() const {return ico.get();}
	// Window deletes icon
	void set_icon(icon* ico);
	icon* get_small_icon() const {return ico_sm.get();}
	void set_small_icon(icon* ico);
	void set_cursor(const cursor_surface& cur);
	cursor_surface get_cursor() const;
	LRESULT send_message(UINT msg, LPARAM lParam, WPARAM wParam);
	font* get_font() const {return m_font.get();}
	// Window deletes the font
	void set_font(font* f);
	dragdrop::drop_target* get_drop_target() {return &drop_handler;}
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
	shared_ptr<icon> ico, ico_sm;
	shared_ptr<font> m_font;
	cursor_surface cur, surf_cur;
	static bool queue_running;
	dragdrop::drop_target drop_handler;
	IDataObject* last_data_object;
protected:
	void add_surface(dynamic_drawsurface*);
	void remove_surface(dynamic_drawsurface*);
	virtual LRESULT message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void on_create_params(unsigned long& style, unsigned long& exstyle, unsigned int& class_style, wstring& classname) {}
	virtual void create_resources(graphics*);
	virtual void on_drag_enter(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect);
	virtual void on_drag_over(DWORD keystate, const point& pt, dragdrop::drop_effects* effect);
	virtual void on_drag_leave();
	virtual void on_drop(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect);
	HWND handle;
};

class windowed_timer
{
public:
	windowed_timer() : running(false) {};
	~windowed_timer();
	callback<void()> tick;
	void create(HWND owner, const chrono::milliseconds& ticks);
	void kill();

	HWND get_handle() const {if(!owner) throw logic_error("No owner"); return owner;}
	chrono::milliseconds get_interval() const {return interval;}
	bool is_running() const {return running;}
private:
	chrono::milliseconds interval;
	bool running;
	static void CALLBACK timer_proc(HWND hWnd, UINT msg, UINT_PTR event_id, DWORD time);
	HWND owner;
};

};
};
#endif