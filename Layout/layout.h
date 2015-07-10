/*****************************************************************************
*                           gcl - layout.h                                   *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#include "stdafx.h"
#include "graphics.h"
#include "window.h"
#include "system.h"

#ifndef LAYOUT_H
#define LAYOUT_H
#if _MSC_VER
   #pragma once
#endif

using namespace gcl::render_objects;

namespace gcl { namespace ui { namespace layout {

class layout_container :
	public dynamic_drawsurface
{
public:
	layout_container();
	virtual ~layout_container();
	void render(graphics* g);
	bool is_rectangular() const {return true;}
	void create_resources(graphics* g);

	// Raised when the offsets are recalculated. void (float min, float value, float max);
	callback<void(float, float, float)> vscroll_offs_changed, hscroll_offs_changed;
	callback<void(float)> vscrolling, hscrolling;
	callback<void(float)> hsmall_step_changed, vsmall_step_changed, hlarge_step_changed, vlarge_step_changed;
	callback<void(bool)> scrollable_changed;
	callback<void(const colour&)> scroll_front_colour_changed, scroll_back_colour_changed, scroll_hot_colour_changed, scroll_pressed_colour_changed;
	callback<void(const horizontal_scroll_align&)> horizontal_scroll_align_changed;
	callback<void(const vertical_scroll_align&)> vertical_scroll_align_changed;

	float get_hscroll_value() const {return hvalue*10.f;}
	float get_vscroll_value() const {return vvalue*10.f;}
	float get_vmax() const {return vmax*10.f;}
	float get_hmax() const {return hmax*10.f;}
	float get_vmin() const {return vmin*10.f;}
	float get_hmin() const {return hmin*10.f;}
	bool get_scrollable() const {return scrollable;}
	void set_scrollable(bool b);
	float get_vsmall_step() const {return vsmall_step;}
	void set_vsmall_step(float f);
	float get_hsmall_step() const {return small_step;}
	void set_hsmall_step(float f);
	float get_vlarge_step() const {return vlarge_step;}
	void set_vlarge_step(float f);
	float get_hlarge_step() const {return large_step;}
	void set_hlarge_step(float f);
	colour get_scroll_back_colour() const {return cl_bk;}
	void set_scroll_back_colour(const colour& c);
	colour get_scroll_front_colour() const {return cl_fr;}
	void set_scroll_front_colour(const colour& c);
	colour get_scroll_hot_colour() const {return cl_hi;}
	void set_scroll_hot_colour(const colour& c);
	colour get_scroll_pressed_colour() const {return cl_pr;}
	void set_scroll_pressed_colour(const colour& c);
	horizontal_scroll_align get_horizontal_scroll_align() const {return halign;}
	void set_horizontal_scroll_align(const horizontal_scroll_align& align);
	vertical_scroll_align get_vertical_scroll_align() const {return valign;}
	void set_vertical_scroll_align(const vertical_scroll_align& align);
	virtual void set_opacity(float f);
protected:
	virtual void on_syscolour_changed();
	void on_mouse_move(const int m, const point& p);
	void on_mouse_dbl_click(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_down(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_up(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_enter(const point& p);
	void on_mouse_leave(const point& p);
private:
	float hfix, vfix;
	float vwidth, hheight;
	const float arrow_size;
	bool scrollable;
	bool visible_scr;
	horizontal_scroll_align halign;
	vertical_scroll_align valign;
	float vmax, hmax;
	float vvalue, hvalue;
	float vmin, hmin;
	shared_ptr<solid_brush> br_bk, br_fr, br_hi, br_pr;
	colour cl_bk, cl_fr, cl_hi, cl_pr;
	bool hright_arr_ov, hleft_arr_ov;
	bool hright_arr_dw, hleft_arr_dw;
	bool hbar_dw, hbar_ov;
	bool vbottom_arr_ov, vtop_arr_ov;
	bool vbottom_arr_dw, vtop_arr_dw;
	bool vbar_dw, vbar_ov;
	float large_step, small_step;
	float vlarge_step, vsmall_step;

	void draw_hscroll(graphics* g);
	void draw_vscroll(graphics* g);
	rect get_hbar();
	rect get_vbar();
	void calc_offsets();
	void this_size_changed(const size&, const resizing_types&);
	void this_mouse_wheel(const int m, const point& p, int delta);
	void this_mouse_h_wheel(const int m, const point& p, int delta);
	void this_keypress(const virtual_keys& key, const key_extended_params& params);
};

// Orders its elements by their fixed position
typedef dynamic_drawsurface canvas;

enum class orientation {vertical, horizontal};
enum class child_orientation {top, bottom, right, left};

// Orders its elements either from right to left or from top to bottom and vice versa
class stack_panel :
	public layout_container
{
public:
	stack_panel();

	callback<void(const child_orientation&)> child_orient_changed;
	callback<void(const orientation&)> orient_changed;

	virtual void layout();

	void set_child_orient(const child_orientation& ort);
	child_orientation get_child_orient() const {return child_orient;};
	void set_orient(const orientation& ort);
	orientation get_orient() const {return orient;}
private:
	orientation orient;
	child_orientation child_orient;
};

class group_box :
	public layout_container
{
public:
	group_box();
	virtual ~group_box();
	void render(graphics* g);
	void create_resources(graphics* g);

	virtual void layout();
	virtual void set_opacity(float f);
protected:
	void on_syscolour_changed();
private:
	shared_ptr<solid_brush> br_back, br_font;
	shared_ptr<pen> pn_back;
	colour cl_back, cl_fback, cl_font;
	float border_width, text_dist;
};

};
};
};
#endif