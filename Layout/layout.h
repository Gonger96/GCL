/*****************************************************************************
*                           GCL - layout.h			                         *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
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

enum class flow_direction {left_to_right, right_to_left, top_to_bottom, bottom_to_top};

// Same as wrap_panel, but wrapps the Elements
class wrap_panel :
	public dynamic_drawsurface
{
typedef list<dynamic_drawsurface*>::iterator lst_itr;
public:
	wrap_panel();
	virtual void layout();
	flow_direction get_flow_direction() const {return fl_dir;}
	void set_flow_direction(const flow_direction& fd);
	child_orientation get_child_orient() const {return orient;}
	void set_child_orient(const child_orientation& co);
protected:
	float get_row_height(lst_itr bgn, lst_itr end, int& cnt);
	float get_row_width(lst_itr bgn, lst_itr end, int& cnt);
	void layout_l_r(bool bottom);
	void layout_r_l(bool bottom);
	void layout_t_b(bool right);
	void layout_b_t(bool right);
	void position_surf(const margin& m, const vertical_align& valign, const horizontal_align& halign, bool sec, float r_height, float& x, float& y, float& width, float& height); 
private:
	flow_direction fl_dir;
	child_orientation orient;
};

class dock_panel :
	public dynamic_drawsurface
{
private:
	enum class dock_style {top, left, right, bottom, fill};
public:
	dock_panel() : auto_min(true) {}
	virtual void layout();
	bool get_auto_min_size() const {return auto_min;}
	void set_auto_min_size(bool b);
protected:
	bool auto_min;
	dock_style get_dock_style(dynamic_drawsurface* dd);
};

class group_panel :
	public layout_container
{
public:
	group_panel();
	virtual ~group_panel();
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

class tab_panel;

class tab_page :                  
	public dynamic_drawsurface
{
friend class tab_panel;
public:
	tab_page() {tab_owner = 0; width = height = 0; set_padding(padding(5, 5, 5, 5));set_captures_mouse_wheel(true);set_captures_keyboard(true);}
	virtual ~tab_page() {}
protected:
	shared_ptr<solid_brush> sb;
	tab_panel* tab_owner;
	float width, height;
private:
	void set_visible(bool b) {dynamic_drawsurface::set_visible(b);}
};

typedef child_orientation tab_orientation;
// Für enabled & visible tabs auch ändern
class tab_panel :
	public dynamic_drawsurface
{
public:
	tab_panel();
	virtual ~tab_panel();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);
	virtual void layout();

	callback<void(tab_page*)> selected_tab_changed;
	callback<void(const colour&)> back_colour_changed, hot_colour_changed, font_colour_changed, tab_colour_changed, border_colour_changed, border_focused_colour_changed;

	virtual void select_tab(int index);
	void add_tab(tab_page* tab);
	void remove_tab(tab_page* tab);
	virtual void set_opacity(float f);
	colour get_back_colour() const {return cl_back;}
	void set_back_colour(const colour& c);
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& c);
	colour get_font_colour() const {return cl_font;}
	void set_font_colour(const colour& c);
	colour get_tab_colour() const {return cl_tab;}
	void set_tab_colour(const colour& c);
	colour get_border_colour() const {return cl_border;}
	void set_border_colour(const colour& c);
	colour get_border_focused_colour() const {return cl_borderf;}
	void set_border_focused_colour(const colour& c);
protected:
	vector<tab_page*> tabs;
	rect get_tab_area();
	float get_tab_width(int idx, graphics* g);
	int get_tab_count();
	// Start with position.x+border
	virtual void add_surface(dynamic_drawsurface* surf);
	virtual void remove_surface(dynamic_drawsurface* surf);
	void on_syscolour_changed();
	bool tab_contains(float start, int idx, const point& p);
	void draw_tab_seperator(graphics* g);
	void draw_tab(graphics* g, float start, int idx);
	void this_mouse_move(const int mod, const point& p);
	void this_mouse_leave(const point& p);
	void this_mouse_down(const mouse_buttons& mb, const int mod, const point& p);
	void this_key_down(const virtual_keys& vk, const key_extended_params& param);
private:
	shared_ptr<solid_brush> br_back, br_hot, br_font, br_tab, br_border, br_disabled;
	shared_ptr<pen> pn_border;
	colour cl_back, cl_hot, cl_font, cl_tab, cl_border, cl_borderf;
	tab_orientation orient;
	float tab_height, arrow_width, border_width, text_space;
	int hidx;
	tab_page* selected_tab;
	
};

};
};
};
#endif