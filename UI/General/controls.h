/*****************************************************************************
*                           GCL - controls.h                                 *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"
#include "callback.h"
#include "graphics.h"
#include "system.h"
#include "window.h"
#include "input.h"

#ifndef CONTROL_H
#define CONTROL_H
#ifdef _MSC_VER 
#	pragma once
#endif

using namespace gcl::render_objects;
using namespace gcl;

namespace gcl { namespace ui {

class ui_colour
{
public:
	virtual ~ui_colour() {};
	callback<void(const colour&)> font_colour_changed, back_colour_changed;

	colour get_back_colour() const {return back_colour;}
	colour get_font_colour() const {return font_colour;}
	virtual void set_back_colour(const colour& c) = 0;
	virtual void set_font_colour(const colour& c) = 0;
protected:
	colour back_colour, font_colour;
};

enum class image_display_modes {seperate, overlay};
typedef horizontal_string_align horizontal_content_align;
typedef vertical_string_align vertical_content_align;

class button :
	public dynamic_drawsurface, public ui_colour
{
public:
	button();
	virtual ~button();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(const colour&)> hot_colour_changed, pressed_colour_changed, border_colour_changed, border_focused_colour_changed;
	callback<void(const image_display_modes&)> image_display_mode_changed;
	callback<void(float)> border_width_changed;
	callback<void(const horizontal_content_align&)> text_halign_changed, image_halign_changed;
	callback<void(const vertical_content_align&)> text_valign_changed, image_valign_changed;
	callback<void(texture*)> image_changed;

	virtual void set_back_colour(const colour& c);
	virtual void set_font_colour(const colour& c);
	image_display_modes get_image_display_mode() const {return disp_mode;}
	void set_image_display_mode(const image_display_modes& disp);
	horizontal_string_align get_text_halign() const {return text_halign;}
	void set_text_halign(const horizontal_string_align& align);
	vertical_string_align get_text_valign() const {return text_valign;}
	void set_text_valign(const vertical_string_align& align);
	horizontal_content_align get_image_halign() const {return img_halign;}
	void set_image_halign(const horizontal_content_align& align);
	vertical_content_align get_image_valign() const {return img_valign;}
	void set_image_valign(const vertical_content_align& align);
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& c);
	colour get_pressed_colour() const {return cl_pr;}
	void set_pressed_colour(const colour& c);
	colour get_border_colour() const {return cl_border;}
	void set_border_colour(const colour& c);	
	colour get_border_focused_colour() const {return cl_borderf;}
	void set_border_focused_colour(const colour& c);
	float get_border_width() const {return border_width;}
	void set_border_width(float f);
	shared_ptr<texture> get_image() const {return image;}
	// Sets an optional image. It's hold by a shared_ptr<>
	void set_image(texture* img);
	bool get_supress_keypress() const {return supress_keypress;}
	void set_supress_keypress(bool b) {supress_keypress = b; set_captures_keyboard(!b);}
	virtual void set_opacity(float f);

protected:
	virtual void on_syscolour_changed();
private:
	bool supress_keypress; // std true, ob bei enter klick gefeuert wird
	image_display_modes disp_mode;
	horizontal_string_align text_halign, img_halign;
	vertical_string_align text_valign, img_valign; // Setter mit content_align

	shared_ptr<solid_brush> br_bk, br_hot, br_pr, br_fn, br_border, br_disabled;
	colour cl_hot, cl_pr, cl_border, cl_borderf;
	shared_ptr<pen> pn_border;
	shared_ptr<texture> image;
	float border_width;
	rect get_text_rect();
	rect get_image_rect();
	void this_key_up(const virtual_keys& key, const key_extended_params& params);
};

enum class character_casing_types {normal, lower, upper};

class edit :
	public dynamic_drawsurface, public ui_colour
{
public:
	edit();
	virtual ~edit();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(const colour&)> caret_colour_changed, border_colour_changed, border_focused_colour_changed;
	callback<void(int)> max_count_changed;
	callback<void(bool)> hide_selection_changed, use_pw_char_changed, read_only_changed;
	callback<void(wchar_t)> password_char_changed;
	callback<void(float)> border_width_changed;
	// int start, int length
	callback<void(int, int)> selection_changed;
	callback<void(const character_casing_types&)> character_casing_changed;
	callback<void(const wstring&)> display_text_changed;
		
	void set_back_colour(const colour& c);
	void set_font_colour(const colour& c);
	void set_caret_colour(const colour& c);
	colour get_caret_colour() const {return caret_colour;}
	void set_border_colour(const colour& c);
	colour get_border_colour() const {return cl_border;}
	void set_border_focused_colour(const colour& c);
	colour get_border_focused_colour() const {return cl_border_f;}
	float get_border_width() const {return border_width;}
	void set_border_width(float f);
	int get_max_count() const {return max_count;}
	void set_max_count(int c);
	void set_hide_selection(bool b);
	bool get_hide_selection() const {return hide_selection;}
	wchar_t get_password_char() const {return pw_char;}
	void set_password_char(wchar_t cc);
	bool get_use_pw_char() const {return use_pw_char;}
	void set_use_pw_char(bool b);
	bool get_read_only() const {return read_only;}
	void set_read_only(bool b);
	wstring get_display_text() const {return display_text;}
	void set_display_text(const wstring& txt);
	character_casing_types get_character_casing() const {return character_casing;}
	void set_character_casing(const character_casing_types& type);
	virtual void set_opacity(float f);

	void copy_selection();
	void try_paste_into_selection();
	void cut_selection();
	void delete_selection();
	void deselect();
	void select(int sel_start, int sel_len);
	wstring get_selection_content() const;
	int get_index_from_pos(graphics* g, const point& p);
	virtual void set_title(const wstring& txt);
protected:
	virtual void on_syscolour_changed();
	// Override and return false if the char shall not be added
	virtual bool on_char_adding(wchar_t cc) {return true;}
	void on_char(wchar_t c, const key_extended_params& params);
	void on_keydown(const virtual_keys& key, const key_extended_params& params);
	void on_focus_changed(bool b);
	void on_mdown(const mouse_buttons& b, int mod, const point& p);
	void on_mup(const mouse_buttons& b, int mod, const point& p);
	void on_mmove(int mod, const point& p);
	void on_mleave(const point& p);
	rect get_caret_rect(graphics* g);
	rect get_selection_rect(graphics* g);
	void calc_text_offset(graphics* g);
	int get_text_length() const {return title.length();}
	wstring get_string();
	void caret_changed();
	void update_states(const point&);
	void copy_click(int mod) {copy_selection();}
	void cut_click(int mod) {cut_selection();}
	void paste_click(int mod) {try_paste_into_selection();}
	void delete_click(int mod) {delete_selection();}
	void select_click(int mod) {select(0, get_string().length());}
	wstring load_text(int id, HMENU menu, const wstring& def);
	menu_strip strp_cut, strp_copy, strp_paste, strp_delete, strp_select;
private:
	shared_ptr<solid_brush> br_bk, br_fn, br_border, br_disabled, br_caret;
	shared_ptr<pen> pn;
	float border_width;
	bool mve_sel, clk_sel;
	float text_offset;
	size sz;
	colour caret_colour, cl_border_f, cl_border;
	int max_count;
	bool hide_selection;
	bool caret_running, caret_display;
	wchar_t pw_char;
	bool use_pw_char;
	bool m_dwn;
	int selection_start, selection_end;
	bool caret_start;
	wstring display_text;
	bool read_only;
	character_casing_types character_casing;
	windowed_timer tmr;
};

class hscroll :
	public dynamic_drawsurface
{
public:
	hscroll();
	virtual ~hscroll();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(float)> scrolling;
	callback<void(const colour&)> back_colour_changed, front_colour_changed, hot_colour_changed, pressed_colour_changed;
	callback<void(float)> large_step_changed, small_step_changed, value_changed, max_changed, min_changed;

	colour get_back_colour() const {return cl_bk;}
	void set_back_colour(const colour& c);
	colour get_front_colour() const {return cl_fr;}
	void set_front_colour(const colour& c);
	colour get_hot_colour() const {return cl_hi;}
	void set_hot_colour(const colour& c);
	colour get_pressed_colour() const {return cl_pr;}
	void set_pressed_colour(const colour& c);
	float get_max() const {return hmax;}
	void set_max(float val);
	float get_min() const {return hmin;}
	void set_min(float val);
	float get_value() const {return value;}
	void set_value(float val);
	float get_large_step() const {return large_step;}
	void set_large_step(float f);
	float get_small_step() const {return small_step;}
	void set_small_step(float f);
	void scroll(int delta);
	void set_min_size(const size& s);
	virtual void set_opacity(float f);
protected:
	rect get_bar() const;
	void this_mouse_h_wheel(const int m, const point& p, int delta);
	void this_keypress(const virtual_keys& key, const key_extended_params& params);
	void on_mouse_move(const int m, const point& p);
	void on_mouse_down(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_up(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_leave(const point& p);
	void on_syscolour_changed();
private:
	float fix;
	const float arrow_size;
	float hmax;
	float value;
	float hmin;
	shared_ptr<solid_brush> br_bk, br_fr, br_hi, br_pr;
	colour cl_bk, cl_fr, cl_hi, cl_pr;
	bool hright_arr_ov, hleft_arr_ov;
	bool hright_arr_dw, hleft_arr_dw;
	bool hbar_dw, hbar_ov;
	float large_step, small_step;
};

class vscroll :
	public dynamic_drawsurface
{
public:
	vscroll();
	virtual ~vscroll();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(float)> scrolling;
	callback<void(const colour&)> back_colour_changed, front_colour_changed, hot_colour_changed, pressed_colour_changed;
	callback<void(float)> large_step_changed, small_step_changed, value_changed, max_changed, min_changed;

	colour get_back_colour() const {return cl_bk;}
	void set_back_colour(const colour& c);
	colour get_front_colour() const {return cl_fr;}
	void set_front_colour(const colour& c);
	colour get_hot_colour() const {return cl_hi;}
	void set_hot_colour(const colour& c);
	colour get_pressed_colour() const {return cl_pr;}
	void set_pressed_colour(const colour& c);
	float get_max() const {return vmax;}
	void set_max(float val);
	float get_min() const {return vmin;}
	void set_min(float val);
	float get_value() const {return value;}
	void set_value(float val);
	float get_large_step() const {return large_step;}
	void set_large_step(float f);
	float get_small_step() const {return small_step;}
	void set_small_step(float f);
	void scroll(int delta);
	void set_min_size(const size& s);
	virtual void set_opacity(float f);
protected:
	rect get_bar() const;
	void this_mouse_wheel(const int m, const point& p, int delta);
	void this_keypress(const virtual_keys& key, const key_extended_params& params);
	void on_mouse_move(const int m, const point& p);
	void on_mouse_down(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_up(const mouse_buttons& b, const int m, const point& p);
	void on_mouse_leave(const point& p);
	void on_syscolour_changed();
private:
	float fix;
	const float arrow_size;
	float vmax;
	float value;
	float vmin;
	shared_ptr<solid_brush> br_bk, br_fr, br_hi, br_pr;
	colour cl_bk, cl_fr, cl_hi, cl_pr;
	bool vtop_arr_ov, vbottom_arr_ov;
	bool vtop_arr_dw, vbottom_arr_dw;
	bool vbar_dw, vbar_ov;
	float large_step, small_step;
};

enum class selection_modes {none, single, multi};

template <typename data>
class list_box :
	public dynamic_drawsurface, public ui_colour
{
public:
	struct list_member
	{
	friend class list_box<data>;
	public:
		list_member(unsigned index, data member, const wstring& nm, list_box<data>& lb) : idx(index), mem(member), name(nm), lb_(lb), sel(false) {}
		unsigned get_index() const {return idx;}
		bool get_selected() const {return sel;}
		void set_selected(bool b) 
		{
			if(lb_.select_mode != selection_modes::multi)
				throw logic_error("list_box is not multiselectable");
			sel = b; 
			lb_.redraw(lb_.get_bounds());
		}
		data get_data() const {return mem;}
		wstring get_display_title() const {return name;}
	private:
		unsigned idx;
		data mem;
		bool sel;
		wstring name;
		list_box<data>& lb_;
	};
	typedef typename vector<list_member>::iterator list_iterator;
	typedef typename vector<list_member>::const_iterator const_list_iterator;
	typedef typename vector<list_member>::reverse_iterator reverse_list_iterator;
	typedef typename vector<list_member>::const_reverse_iterator const_reverse_list_iterator;
public:
	list_box()
	{
		select_mode = selection_modes::single;
		set_captures_keyboard(true);
		set_captures_mouse_wheel(true);
		border_width = 1.f;
		hide_sel = false;
		hover_index = selected_index = -1;
		row_height = 20;
		cl_border = colour::gray;
		cl_border_f = colour::gcl_border;
		back_colour = colour::gcl_dark_gray;
		font_colour = colour::white;
		hscr.set_parent(this);
		vscr.set_parent(this);
		vscr.set_tab_stop(false);
		hscr.set_tab_stop(false);
		vscr.set_captures_keyboard(false);
		hscr.set_captures_keyboard(false);
		set_min_size(size(vscr.get_size().width+border_width+10, hscr.get_size().height+border_width+10));
		large_step = 5;
		valign = vertical_scroll_align::right;
		halign = horizontal_scroll_align::bottom;
		set_scrollbar_props();
		owner_changed += make_func_ptr(this, &list_box<data>::this_owner_changed);
		mouse_down += make_func_ptr(this, &list_box<data>::this_mouse_down);
		key_down += make_func_ptr(this, &list_box<data>::this_keypress);
		size_changed += make_func_ptr(this, &list_box<data>::this_size_changed);
		vscr.value_changed += make_func_ptr(this, &list_box<data>::scr_value_changed);
		hscr.value_changed += make_func_ptr(this, &list_box<data>::scr_value_changed);
		mouse_wheel += make_func_ptr(this, &list_box<data>::this_mouse_wheel);
		mouse_h_wheel += make_func_ptr(this, &list_box<data>::this_h_mouse_wheel);
		font_changed += make_func_ptr(this, &list_box<data>::this_font_changed);
	}
	virtual ~list_box()
	{
		owner_changed -= make_func_ptr(this, &list_box<data>::this_owner_changed);
		mouse_down -= make_func_ptr(this, &list_box<data>::this_mouse_down);
		key_down -= make_func_ptr(this, &list_box<data>::this_keypress);
		size_changed -= make_func_ptr(this, &list_box<data>::this_size_changed);
		vscr.value_changed -= make_func_ptr(this, &list_box<data>::scr_value_changed);
		hscr.value_changed -= make_func_ptr(this, &list_box<data>::scr_value_changed);
		mouse_wheel -= make_func_ptr(this, &list_box<data>::this_mouse_wheel);
		mouse_h_wheel -= make_func_ptr(this, &list_box<data>::this_h_mouse_wheel);
		font_changed -= make_func_ptr(this, &list_box<data>::this_font_changed);
	}
	virtual void render(graphics* g)
	{
		if(!has_resources())
			init_resources(g);
		if(recalc_hval)
			calc_h_values(g);
		rect rc(get_position().x + border_width + get_pos_offset().x, get_position().y + border_width + get_pos_offset().y,
			get_size().width - 2*border_width - get_size_offset().width, get_size().height - 2*border_width - get_size_offset().height);
		br_border->set_colour(get_focus() ? cl_border_f : cl_border);
		pn->update();
		g->fill_rect(rect(get_position(), get_size()), br_back.get());
		g->draw_rect(rect(get_position().x+border_width/2.f, get_position().y+border_width/2.f, get_size().width-border_width, get_size().height-border_width), pn.get());
		g->push_clip(clip(rc));
		g->fill_rect(rect(get_position(), get_size()), br_back.get());
		for(unsigned i = min<int>(coll.size(), static_cast<int>(vscr.get_value())); i < coll.size(); ++i)
		{
			float y = (i-vscr.get_value())*row_height;
			auto mem = &coll[i];
			if(y > get_size().height-2*border_width)
				break;
			rect metric = get_font()->get_metrics(mem->get_display_title(), size::max_size(), g);
			if(mem->get_selected() && ((get_focus() && hide_sel) || !hide_sel))
			{
				if(select_mode != selection_modes::multi && selected_index != i)
					throw logic_error("list_box in singleselectmode cannot select multiple items");
				g->fill_rect(rect(rc.get_x()+2-hscr.get_value()*10, y+rc.get_y()+2+1, metric.get_width()+4, row_height-2), br_border.get());
			}
			g->draw_string(mem->get_display_title(), point(rc.get_x()+4-hscr.get_value()*10, y+rc.get_y()+1+row_height/2.f-metric.get_height()/2.f), get_font(), br_fn.get());
			if(static_cast<int>(coll.size()) > hover_index && hover_index >= 0 && i == hover_index)
			{
				g->set_antialias(get_absolute_transform().is_identity());
				g->draw_rect(rect(rc.get_x()+2-hscr.get_value()*10, row_height*(hover_index-vscr.get_value())+rc.get_y()+2+1, metric.get_width()+4, row_height-2), pn_hover.get());
				g->set_antialias(false);
			}
		}
		g->pop_clip();
		if(!get_enabled())
			g->fill_rect(rect(get_position(), get_size()), br_disabled.get());
		dynamic_drawsurface::render(g);
	}
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g)
	{
		dynamic_drawsurface::create_resources(g);
		if(app::is_high_contrast_app())
		{
			br_back = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_face));
			br_fn = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
		}
		else
		{
			br_back = shared_ptr<solid_brush>(g->create_solid_brush(back_colour));
			br_fn = shared_ptr<solid_brush>(g->create_solid_brush(font_colour));
		}
		br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
		br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(50, colour::black)));
		pn = shared_ptr<pen>(g->create_pen(br_border.get(), border_width));
		pn_hover = shared_ptr<pen>(g->create_pen(br_fn.get(), 1.0f, dash_cap::flat, dash_cap::flat, dash_cap::flat, dash_style::dash));
		if(get_opacity() != 1.f)
		{
			br_border->set_opacity(get_opacity());
			br_back->set_opacity(get_opacity());
			br_fn->set_opacity(get_opacity());
			br_disabled->set_opacity(get_opacity());
		}
	}

	callback<void(const colour&)> border_colour_changed, border_focused_colour_changed;
	callback<void(bool)> hide_selection_changed;
	callback<void(float)> border_width_changed, row_height_changed;
	callback<void(const vertical_scroll_align&)> valign_changed;
	callback<void(const horizontal_scroll_align&)> halign_changed;
	callback<void(const selection_modes&)> selection_mode_changed;
	callback<void(int)> selected_index_changed;
	callback<void()> selection_changed;
	callback<void(const function<wstring(const data&)>&)> translator_changed;

	list_iterator begin() {return coll.begin();}
	list_iterator end() {return coll.end();}
	const_list_iterator cbegin() const {return coll.cbegin();}
	const_list_iterator cend() {return coll.cend();}
	reverse_list_iterator rbegin() {return coll.rbegin();}
	reverse_list_iterator rend() {return coll.rend();}
	const_reverse_list_iterator crbegin() const {return coll.crbegin();}
	const_reverse_list_iterator crend() const {return coll.crend();}
	list_member* operator[](unsigned index) {return &coll[index];}
	template <typename itr>
	void set_data(const itr _begin, const itr _end)
	{
		coll.clear();
		for(itr ip = _begin; ip != _end; ++ip)
		{
			coll.push_back(list_member(coll.size(), *ip, translator ? translator(*ip) : L"", *this));
		}
		calc_scrollbar_values();
		recalc_hval = true;
		if(owner)
			owner->redraw(get_bounds());
	}
	int get_selected_index() const
	{
		if(select_mode != selection_modes::single)
			throw logic_error("Invalid selectionmode");
		return selected_index;
	}
	void set_selected_index(int idx)
	{
		if(select_mode != selection_modes::single)
			throw logic_error("Invalid selectionmode");
		if(!is_between_equ(idx, -1, static_cast<int>(coll.size())-1))
			throw out_of_range("Index must be between -1 and max");
		if(selected_index == idx)
			return;
		if(selected_index != -1)
			coll[selected_index].sel = false;
		selected_index = idx;
		if(selected_index != -1)
			coll[selected_index].sel = true;
		if(vscr.get_visible() && selected_index != -1)
		{
			float y = selected_index*row_height+border_width+2*get_size_offset().height;
			float diff_down = y+row_height - (get_size().height-2*border_width)-get_size_offset().height;
			float diff_up = y - border_width - 2*get_size_offset().height;
			if(diff_down-vscr.get_value()*row_height > 0)
				vscr.set_value(diff_down/row_height);
			else if(diff_up-vscr.get_value()*row_height < 0)
				vscr.set_value(diff_up/row_height);
		}
		selected_index_changed(selected_index);
		if(owner)
			owner->redraw(get_bounds());
	}
	int get_index_from_pos(const point& p)
	{
		point pt = p;
		matrix m = get_absolute_transform();
		m.invert();
		m.transform_points(&pt);
		pt.y -= get_position().y;
		unsigned index = static_cast<unsigned>((p.y-border_width-2+(vscr.get_value()*row_height)-get_pos_offset().y)/row_height);
		if(coll.size() <= index)
			return -1;
		return static_cast<int>(index);
	}

	hscroll& get_hscroll() {return hscr;}
	vscroll& get_vscroll() {return vscr;}
	void set_back_colour(const colour& c)
	{
		if(!change_if_diff(back_colour, c)) return;
		if(br_back)
			br_back->set_colour(c);
		back_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	void set_font_colour(const colour& c)
	{
		if(!change_if_diff(font_colour, c)) return;
		if(br_fn)
			br_fn->set_colour(c);
		font_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	void set_border_colour(const colour& c)
	{
		if(!change_if_diff(cl_border, c)) return;
		border_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	colour get_border_colour() const {return cl_border;}
	void set_border_focused_colour(const colour& c)
	{
		if(!change_if_diff(cl_border_f, c)) return;
		border_focused_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	colour get_border_focused_colour() const {return cl_border_f;}
	float get_border_width() const {return border_width;}
	void set_border_width(float f)
	{
		if(!change_if_diff(border_width, f)) return;
		if(pn)
			pn->set_width(f);
		border_width_changed(f);
		if(owner)
			owner->redraw(get_bounds());
	}
	vertical_scroll_align get_vscroll_align() const {return valign;}
	void set_vscroll_align(const vertical_scroll_align& align)
	{
		if(!change_if_diff(valign, align)) return;
		valign_changed(align);
		set_scrollbar_props();
		if(owner)
			owner->redraw(get_bounds());
	}
	horizontal_scroll_align get_hscroll_align() const {return halign;}
	void set_hscroll_align(const horizontal_scroll_align& align)
	{
		if(!change_if_diff(halign, align)) return;
		halign_changed(align);
		set_scrollbar_props();
		if(owner)
			owner->redraw(get_bounds());
	}
	selection_modes get_selection_mode() const {return select_mode;}
	void set_selection_mode(const selection_modes& mode)
	{
		if(!change_if_diff(select_mode, mode)) return;
		selected_index = -1;
		hover_index = -1;
		for(auto& itm : coll)
			itm.sel = false;
		selection_mode_changed(mode);
		if(owner)
			owner->redraw(get_bounds());
	}
	bool get_hide_selection() const {return hide_sel;}
	void set_hide_selection(bool b)
	{
		if(!change_if_diff(hide_sel, b)) return;
		hide_selection_changed(b);
		if(owner)
			owner->redraw(get_bounds());
	}
	float get_row_height() const {return row_height;}
	void set_row_height(float f)
	{
		if(!change_if_diff(row_height, f)) return;
		calc_scrollbar_values();
		row_height_changed(f);
		if(owner)
			owner->redraw(get_bounds());
	}
	function<wstring(const data&)> get_translator() const {return translator;}
	void set_translator(const function<wstring(const data&)>& func)
	{
		translator = func;
		translator_changed(func);
		for(auto& itm : coll)
			itm.name = translator(itm.mem);
		recalc_hval = true;
		if(owner)
			owner->redraw(get_bounds());
	}
	void set_display_hscroll(bool b)
	{
		if(hscr.get_visible() == b) return;
		hscr.set_visible(b);
		set_scrollbar_props();
		calc_scrollbar_values();
	}
	bool get_display_hscroll() const {return hscr.get_visible();}
	void set_display_vscroll(bool b)
	{
		if(vscr.get_visible() == b) return;
		vscr.set_visible(b);
		set_scrollbar_props();
		recalc_hval = true;
		if(owner)
			owner->redraw(get_bounds());
	}
	bool get_display_vscroll() const {return vscr.get_visible();}
	virtual void set_opacity(float f)
	{
		if(get_opacity() == f) return;
		if(f < 0 || f > 1.f)
			throw invalid_argument("Invalid opacity");
		if(has_resources())
		{
			br_back->set_opacity(f);
			br_fn->set_opacity(f);
			br_border->set_opacity(f);
			br_disabled->set_opacity(f);
		}
		hscr.set_opacity(f);
		vscr.set_opacity(f);
		dynamic_drawsurface::set_opacity(f);
	}
protected:
	void this_owner_changed(drawsurface* d)
	{
		hscr.set_owner(d);
		vscr.set_owner(d);
	}
	void this_mouse_down(const mouse_buttons& btns, int mod, const point& pos)
	{
		if(btns != mouse_buttons::left)
			return;
		point p = pos;
		p.x += get_position().x;
		p.y += get_position().y; // Important! Use absolute position to transform!
		matrix transf = get_absolute_transform();
		transf.invert();
		transf.transform_points(&p);
		p.y -= get_position().y;
		unsigned index = static_cast<unsigned>((p.y-border_width-2+(vscr.get_value()*row_height)-get_pos_offset().y)/row_height);
		if(coll.size() <= index)
			return;
		switch(select_mode)
		{
		case selection_modes::multi:
			coll[index].sel = !coll[index].sel;
			selection_changed();
			hover_index = index;
			break;
		case selection_modes::single:
			{
				if(static_cast<int>(coll.size()) > selected_index && selected_index >= 0)
					coll[selected_index].sel = false;
				coll[index].sel = true;
				selected_index = index;
				selected_index_changed(selected_index);
				break;
			}
		case selection_modes::none:
			{
				if(static_cast<int>(coll.size()) > hover_index)
					hover_index = index;
			}
		}
		redraw(get_bounds());
	}
	void this_keypress(const virtual_keys& key, const key_extended_params& params)
	{
		if(coll.size() == 0)
			return;
		switch(select_mode)
		{
		case selection_modes::multi:
			handle_multi_key(key);
			break;
		case selection_modes::single:
			handle_single_key(key);
			break;
		case selection_modes::none:
			handle_none_key(key);
		}
		calc_scroll_pos();
		if(!hscr.get_visible())
		{
			if(owner)
				owner->redraw(get_bounds());
			return;
		}
		if(key == virtual_keys::left)
			hscr.scroll(params.repeat_count*WHEEL_DELTA);
		else if(key == virtual_keys::right)
			hscr.scroll(-params.repeat_count*WHEEL_DELTA);
		if(owner)
			owner->redraw(get_bounds());
	}
	void this_size_changed(const size& sz, const resizing_types&)
	{
		calc_scrollbar_values();
		float diff_h = max_str_width - (get_size().width-2*border_width-get_size_offset().width-10); // -10 Tolerance
		if(diff_h > 0)
		{
			hscr.set_max(diff_h/10);
		}
		else
		{
			hscr.set_max(1);
			hscr.set_value(0);
		}
	}
	void this_mouse_wheel(const int m, const point& p, int delta)
	{
		if(!vscr.get_visible())
			return;
		vscr.scroll(delta);
	}
	void this_h_mouse_wheel(const int m, const point& p, int delta)
	{
		if(!hscr.get_visible())
			return;
		hscr.scroll(delta);
	}
	void this_font_changed(const font* f)
	{
		recalc_hval = true;
		if(owner)
			owner->redraw(get_bounds());
	}
	void scr_value_changed(float val)
	{
		if(owner)
			owner->redraw(get_bounds());
	}
	void handle_multi_key(const virtual_keys& key)
	{
		if(coll.size() == 0)
			return;
		switch(key)
		{
		case virtual_keys::up:
			if(hover_index > 0)
				hover_index--;
			break;
		case virtual_keys::down:
			if(hover_index != static_cast<int>(coll.size())-1)
				hover_index++;
			break;
		case virtual_keys::home:
			hover_index = 0;
			break;
		case virtual_keys::end:
			hover_index = static_cast<int>(coll.size()-1);
			break;
		case virtual_keys::prior:
			hover_index = max(hover_index-large_step, 0);
			break;
		case virtual_keys::next:
			hover_index = min(hover_index+large_step, static_cast<int>(coll.size()-1));
			break;
		case virtual_keys::space:
			if(is_between_equ(hover_index, 0, static_cast<int>(coll.size()-1)))
			{
				coll[hover_index].sel = !coll[hover_index].sel;
				selection_changed();
			}
		}
	}
	void handle_single_key(const virtual_keys& key)
	{
		if(coll.size() == 0)
			return;
		switch(key)
		{
		case virtual_keys::up:
			if(is_between_equ(selected_index, 1, static_cast<int>(coll.size()-1)))
				coll[selected_index].sel = false;
			if(selected_index > 0)
			{	
				selected_index--;
				coll[selected_index].sel = true;
				selected_index_changed(selected_index);
			}
			break;
		case virtual_keys::down:
			if(is_between_equ(selected_index, 0, static_cast<int>(coll.size()-2)))
				coll[selected_index].sel = false;
			if(selected_index != static_cast<int>(coll.size())-1)
			{	
				selected_index++;
				coll[selected_index].sel = true;
				selected_index_changed(selected_index);
			}
			break;
		case virtual_keys::home:
			if(is_between_equ(selected_index, 0, static_cast<int>(coll.size()-1)))
				coll[selected_index].sel = false;
			selected_index = 0;
			coll[selected_index].sel = true;
			selected_index_changed(selected_index);
			break;
		case virtual_keys::end:
			if(is_between_equ(selected_index, 0, static_cast<int>(coll.size()-1)))
				coll[selected_index].sel = false;
			selected_index = static_cast<int>(coll.size()-1);
			coll[selected_index].sel = true;
			selected_index_changed(selected_index);
			break;
		case virtual_keys::prior:
			if(is_between_equ(selected_index, 0, static_cast<int>(coll.size()-1)))
				coll[selected_index].sel = false;
			selected_index = max(selected_index-large_step, 0);
			coll[selected_index].sel = true;
			selected_index_changed(selected_index);
			break;
		case virtual_keys::next:
			if(is_between_equ(selected_index, 0, static_cast<int>(coll.size()-1)))
				coll[selected_index].sel = false;
			selected_index = min(selected_index+large_step, static_cast<int>(coll.size()-1));
			coll[selected_index].sel = true;
			selected_index_changed(selected_index);
		}
	}
	void handle_none_key(const virtual_keys& key)
	{
		if(coll.size() == 0)
			return;
		switch(key)
		{
		case virtual_keys::up:
			if(hover_index > 0)
				hover_index--;
			break;
		case virtual_keys::down:
			if(hover_index != static_cast<int>(coll.size())-1)
				hover_index++;
			break;
		case virtual_keys::home:
			hover_index = 0;
			break;
		case virtual_keys::end:
			hover_index = static_cast<int>(coll.size()-1);
			break;
		case virtual_keys::prior:
			hover_index = max(hover_index-large_step, 0);
			break;
		case virtual_keys::next:
			hover_index = min(hover_index+large_step, static_cast<int>(coll.size()-1));
			break;
		}
	}
	void calc_scroll_pos()
	{
		if(!vscr.get_visible())
			return;
		switch(select_mode)
		{
		case selection_modes::none:
		case selection_modes::multi:
			{
				if(hover_index == -1)
					return;
				float y = hover_index*row_height+border_width+2*get_size_offset().height;
				float diff_down = y+row_height - (get_size().height-2*border_width)-get_size_offset().height;
				float diff_up = y - border_width - 2*get_size_offset().height;
				if(diff_down-vscr.get_value()*row_height > 0)
					vscr.set_value(diff_down/row_height);
				else if(diff_up-vscr.get_value()*row_height < 0)
					vscr.set_value(diff_up/row_height);
				break;
			}
		case selection_modes::single:
			{
				if(selected_index == -1)
					return;
				float y = selected_index*row_height+border_width+2*get_size_offset().height;
				float diff_down = y+row_height - (get_size().height-2*border_width)-get_size_offset().height;
				float diff_up = y - border_width - 2*get_size_offset().height;
				if(diff_down-vscr.get_value()*row_height > 0)
					vscr.set_value(diff_down/row_height);
				else if(diff_up-vscr.get_value()*row_height < 0)
					vscr.set_value(diff_up/row_height);
			}
		}
	}
	point get_pos_offset()
	{
		point p;
		if(!vscr.get_visible())
			p.x = 0;
		else
			p.x = valign == vertical_scroll_align::left ? vscr.get_size().width : 0;
		if(!hscr.get_visible())
			p.y = 0;
		else
			p.y = halign == horizontal_scroll_align::top ? hscr.get_size().height : 0;
		return p;
	}
	size get_size_offset()
	{
		size sz;
		if(!vscr.get_visible())
			sz.width = 0;
		else
			sz.width =  vscr.get_size().width;
		if(!hscr.get_visible())
			sz.height = 0;
		else
			sz.height = hscr.get_size().height;
		return sz;
	}
	float max_str_width;
	virtual void on_syscolour_changed()
	{
		if(app::is_high_contrast_app())
		{
			br_back->set_colour(colour::button_face);
			br_fn->set_colour(colour::gray_text);
		}
		else
		{
			br_back->set_colour(back_colour);
			br_fn->set_colour(font_colour);
		}
		dynamic_drawsurface::on_syscolour_changed();
	}
private:
	void set_scrollbar_props()
	{
		vscr.set_horizontal_align((valign == vertical_scroll_align::left) ? horizontal_align::left : horizontal_align::right);
		vscr.set_vertical_align(vertical_align::stretch);
		if(hscr.get_visible())
			vscr.set_margin(margin((valign == vertical_scroll_align::left) ? border_width : -border_width, -border_width, halign == horizontal_scroll_align::top ? hscr.get_size().height+border_width : border_width, (halign == horizontal_scroll_align::bottom) ? hscr.get_size().height+border_width : border_width));
		else
			vscr.set_margin(margin((valign == vertical_scroll_align::left) ? border_width : -border_width, -border_width, border_width, border_width));
		hscr.set_vertical_align((halign == horizontal_scroll_align::bottom) ? vertical_align::bottom : vertical_align::top);
		hscr.set_margin(margin(border_width, border_width, (halign == horizontal_scroll_align::top) ? border_width : -border_width, -border_width));
		hscr.set_horizontal_align(horizontal_align::stretch);
	}
	bool recalc_hval;
	void calc_scrollbar_values()
	{
		float itms = static_cast<float>(coll.size())-((get_size().height-2*border_width-get_size_offset().height)/row_height)+0.5f;
		if(itms < 1)
		{
			itms = 1;
			vscr.set_value(0);
		}
		vscr.set_max(itms);
	}
	void calc_h_values(graphics* g)
	{
		float width = 1.f;
		for(auto& itm : coll)
			width = max(width, get_font()->get_metrics(itm.get_display_title(), size::max_size(), g).get_width());
		max_str_width = width;
		this_size_changed(get_size(), resizing_types::restored);
	}
	shared_ptr<solid_brush> br_border, br_back, br_fn, br_disabled;
	colour cl_border, cl_border_f;
	shared_ptr<pen> pn, pn_hover;
	hscroll hscr;
	vscroll vscr; // set_display_scrollbars! h&v seperat und dann value auf 0 setzen!
	vertical_scroll_align valign;
	horizontal_scroll_align halign;
	
	selection_modes select_mode; // Bei change selected und hover index )= -1
	bool hide_sel;
	float border_width;
	float row_height;
	int large_step;
	int selected_index, hover_index;
	vector<list_member> coll;
	function<wstring(const data&)> translator;
};

enum class check_state {none, checked, intermediate};
enum class text_align {before_button, after_button};

class check_button :
	public dynamic_drawsurface
{
public:
	check_button();
	virtual ~check_button();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(const check_state&)> check_state_changed;
	callback<void(const text_align&)> text_align_changed;
	callback<void(const colour&)> hot_colour_changed, border_colour_changed, border_focused_colour_changed, font_colour_changed, state_colour_changed;

	bool get_is_three_state() const {return is_three_state;}
	void set_three_state(bool b);
	check_state get_check_state() const {return state;}
	void set_check_state(const check_state& st);
	bool get_checked() const {return state != check_state::none;}
	void set_checked(bool checked);
	text_align get_text_align() const {return txt_algn;}
	void set_text_align(const text_align& align);
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& c);
	colour get_font_colour() const {return cl_font;}
	void set_font_colour(const colour& c);
	colour get_state_colour() const {return cl_state;}
	void set_state_colour(const colour& c);
	colour get_border_colour() const {return cl_border;}
	void set_border_colour(const colour& c);
	colour get_border_focused_colour() const {return cl_fborder;}
	void set_border_focused_colour(const colour& c);
	virtual void set_size(const size& sz, bool redraw = true);
	virtual void set_title(const wstring& txt);
	virtual void set_opacity(float f);
protected:
	virtual void on_syscolour_changed();
	point* get_hook(float x, float y);
	void this_clicked(const mouse_buttons& mb, int mod, const point& p);
	void this_key_up(const virtual_keys& key, const key_extended_params& params);
private:
	shared_ptr<solid_brush> br_border, br_font, br_hot, br_disabled, br_state;
	colour cl_border, cl_fborder, cl_hot, cl_font, cl_state;
	shared_ptr<pen> pn_border;
	bool is_three_state;
	check_state state;
	text_align txt_algn;
	size msz;
	bool relayout;
	float box_width;
};

class radio_button :
	public dynamic_drawsurface
{
public:
	radio_button();
	virtual ~radio_button();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(bool)> checked_changed;
	callback<void(const text_align&)> text_align_changed;
	callback<void(const colour&)> hot_colour_changed, border_colour_changed, border_focused_colour_changed, font_colour_changed;

	bool get_checked() const {return checked;}
	void set_checked(bool checked);
	text_align get_text_align() const {return txt_algn;}
	void set_text_align(const text_align& align);
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& c);
	colour get_font_colour() const {return cl_font;}
	void set_font_colour(const colour& c);
	colour get_border_colour() const {return cl_border;}
	void set_border_colour(const colour& c);
	colour get_border_focused_colour() const {return cl_fborder;}
	void set_border_focused_colour(const colour& c);
	virtual void set_size(const size& sz, bool redraw = true);
	virtual void set_title(const wstring& txt);
	virtual void set_opacity(float f);
protected:
	virtual void on_syscolour_changed();
	virtual void internal_check(bool val);
	void uncheck_buttons() const;
	void this_clicked(const mouse_buttons& mb, int mod, const point& p);
	void this_key_up(const virtual_keys& key, const key_extended_params& params);
private:
	shared_ptr<solid_brush> br_border, br_font, br_hot, br_disabled;
	colour cl_border, cl_fborder, cl_hot, cl_font;
	shared_ptr<pen> pn_border;
	text_align txt_algn;
	size msz;
	bool relayout;
	bool checked;
	float box_width;
};

class static_text :
	public dynamic_drawsurface
{
public:
	static_text();
	virtual ~static_text() {}
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(const colour&)> font_colour_changed;

	colour get_font_colour() const {return cl_font;}
	void set_font_colour(const colour& c);
	virtual void set_opacity(float f);
protected:
	virtual void on_syscolour_changed();
private:
	shared_ptr<solid_brush> br_font;
	colour cl_font;
	size msz;
};

class link_text :
	public dynamic_drawsurface
{
public:
	link_text();
	virtual ~link_text();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(const colour&)> font_colour_changed, hot_colour_changed, visited_colour_changed;

	colour get_font_colour() const {return cl_normal;}
	void set_font_colour(const colour& c);
	colour get_hot_colour() const {return cl_over;}
	void set_hot_colour(const colour& c);
	colour get_visited_colour() const {return cl_visited;}
	void set_visited_colour(const colour& c);
	virtual void set_opacity(float f);
protected:
	void this_clicked(const mouse_buttons&, int, const point&);
private:
	shared_ptr<solid_brush> br_font;
	shared_ptr<pen> pn;
	colour cl_normal, cl_over, cl_visited;
	size msz;
	bool visited;
};

template <typename data>
class combo_box :
	public dynamic_drawsurface, public ui_colour
{
private:
	struct combo_member
	{
	friend class combo_box;
	public:
		combo_member(unsigned index, const data& member, const wstring& nm, combo_box& lb) : idx(index), mem(member), name(nm), lb_(lb) {}
		unsigned get_index() const {return idx;}
		int get_data() const {return mem;}
		wstring get_display_title() const {return name;}
	private:
		unsigned idx;
		data mem;
		wstring name;
		combo_box& lb_;
	};
	typedef typename vector<combo_member>::iterator combo_iterator;
	typedef typename vector<combo_member>::const_iterator const_combo_iterator;
	typedef typename vector<combo_member>::reverse_iterator reverse_combo_iterator;
	typedef typename vector<combo_member>::const_reverse_iterator const_reverse_combo_iterator;
public:
	combo_box()
	{
		set_captures_keyboard(true);
		border_width = 1.f;
		cl_border = colour::gray;
		cl_border_f = colour::gcl_border;
		back_colour = colour::gcl_dark_gray;
		cl_hot = colour::gcl_gray;
		font_colour = colour::white;
		set_min_size(size(40, 20));
		opened = false;
		m_closed = false;
		sel_idx = -1;
		mouse_click += make_func_ptr(this, &combo_box::this_mouse_click);
		key_down += make_func_ptr(this, &combo_box::this_key_dwn);
		syscolour_changed += make_func_ptr(this, &combo_box::on_syscolour_changed);
	}
	virtual ~combo_box() 
	{
		mouse_click -= make_func_ptr(this, &combo_box::this_mouse_click);
		key_down -= make_func_ptr(this, &combo_box::this_key_dwn);
		syscolour_changed -= make_func_ptr(this, &combo_box::on_syscolour_changed);
		if(menu)
			menu->closed -= make_func_ptr(this, &combo_box::menu_closed);
	}
	virtual void render(graphics* g)
	{
		g->push_clip(clip(get_rect()));
		if(!has_resources())
			init_resources(g);
		if(get_focus() && get_enabled())
			br_border->set_colour(cl_border_f);
		else if(!get_focus() || !get_enabled())
			br_border->set_colour(cl_border);
		p_border->update();
		if(get_drawing_state() == drawing_state::hot || get_drawing_state() == drawing_state::pressed)
			br_back->set_colour(cl_hot);
		else
			br_back->set_colour(back_colour);
		if(opened)
			br_arrow->set_colour(cl_border_f);
		else
			br_arrow->set_colour(cl_border);
		g->fill_rect(rect(get_position(), get_size()), br_back.get());
		g->draw_rect(rect(get_position().x+border_width/2.f, get_position().y+border_width/2.f, get_size().width-border_width, get_size().height-border_width), p_border.get());

		float arrow_size = 8.f, height = get_size().height;
		if(sel_idx != -1)
		{
			g->push_clip(clip(rect(point(get_position().x+border_width, get_position().y+border_width), size(max(get_size().width - 2*border_width -arrow_size-arrow_size/2.f, 5.f), max(get_size().height-2*border_width, 5.f)))));
			wstring display = get_selected_member()->get_display_title();
			rect rc = get_font()->get_metrics(display, size::max_size(), g);
			g->draw_string(display, point(get_position().x+border_width, get_position().y + border_width + (get_size().height - 2*border_width)/2.f-rc.get_height()/2.f), get_font(), br_font.get());
			g->pop_clip();
		}
		if(!opened)
		{
			point arr1[] = {point((get_position().x+get_size().width)-arrow_size/2.f, get_position().y+height/2.f-arrow_size/4.f),
							point((get_position().x+get_size().width)-arrow_size-arrow_size/2.f, get_position().y+height/2.f-arrow_size/4.f),
							point((get_position().x+get_size().width) - arrow_size, get_position().y+height/2.f+arrow_size/4.f)};
			g->fill_polygon(arr1, 3, br_arrow.get());
		}
		else
		{
			point arr1[] = {point((get_position().x+get_size().width)-arrow_size/2.f, get_position().y+height/2.f+arrow_size/4.f),
							point((get_position().x+get_size().width)-arrow_size-arrow_size/2.f, get_position().y+height/2.f+arrow_size/4.f),
							point((get_position().x+get_size().width) - arrow_size, get_position().y+height/2.f-arrow_size/4.f)};
			g->fill_polygon(arr1, 3, br_arrow.get());
		}
		if(!get_enabled())
			g->fill_rect(rect(get_position(), get_size()), br_disabled.get());
		g->pop_clip();
		dynamic_drawsurface::render(g);
	}
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g)
	{
		if(app::is_high_contrast_app())
		{
			br_back = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_face));
			br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
		}
		else
		{
			br_back = shared_ptr<solid_brush>(g->create_solid_brush(back_colour));
			br_font = shared_ptr<solid_brush>(g->create_solid_brush(font_colour));
		}
		br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
		br_arrow = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
		br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(50, colour::black)));
		p_border = shared_ptr<pen>(g->create_pen(br_border.get(), border_width));
		if(get_opacity() != 1.f)
		{
			float f = get_opacity();
			br_back->set_opacity(f);
			br_border->set_opacity(f);
			br_arrow->set_opacity(f);
			br_disabled->set_opacity(f);
			br_font->set_opacity(f);
			p_border->update();
		}
		menu = shared_ptr<context_menu>(new context_menu(owner?owner->get_handle():0, g));
		menu->closed += make_func_ptr(this, &combo_box::menu_closed);
		for(auto& itr : strips)
			menu->add_strip(&itr);
	}

	callback<void(int)> selected_index_changed;
	callback<void(const colour&)> border_colour_changed, border_focused_colour_changed, hot_colour_changed;
	callback<void(float)> border_width_changed;
	callback<void(const function<wstring(const data&)>&)> translator_changed;

	combo_iterator begin() {return coll.begin();}
	combo_iterator end() {return coll.end();}
	const_combo_iterator cbegin() const {return coll.cbegin();}
	const_combo_iterator cend() {return coll.cend();}
	reverse_combo_iterator rbegin() {return coll.rbegin();}
	reverse_combo_iterator rend() {return coll.rend();}
	const_reverse_combo_iterator crbegin() const {return coll.crbegin();}
	const_reverse_combo_iterator crend() const {return coll.crend();}
	combo_member* operator[](unsigned index) {return &coll[index];}
	template <typename itr>
	void set_data(const itr _begin, const itr _end)
	{
		coll.clear();
		strips.clear();
		if(has_resources())
			menu->clear_strips();
		sel_idx = -1;
		for(itr ip = _begin; ip != _end; ++ip)
		{
			coll.push_back(combo_member(coll.size(), *ip, translator ? translator(*ip) : L"", *this));
			menu_strip ms;
			ms.set_title(translator ? translator(*ip) : L"");
			ms.click += make_func_ptr(this, &combo_box::strip_selected);
			strips.push_back(ms);
			if(has_resources())
				menu->add_strip(&ms);
		}
		if(owner)
			owner->redraw(get_bounds());
	}
	function<wstring(const data&)> get_translator() const {return translator;}
	void set_translator(const function<wstring(const data&)>& trnslator) 
	{
		translator = trnslator;
		translator_changed(translator);
		int i = 0;
		for(auto& itm : coll)
		{
			itm.name = translator(itm.mem);
			strips[i].set_title(itm.name);
			++i;
		}
		if(owner)
			owner->redraw(get_bounds());
	}  
	void set_back_colour(const colour& c) 
	{
		if(!change_if_diff(back_colour, c))
			return;
		if(has_resources())
		{
			br_back->set_colour(c);
			menu->set_back_colour(c);
		}
		back_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	void set_font_colour(const colour& c) 
	{
		if(!change_if_diff(font_colour, c))
			return;
		if(has_resources())
		{
			br_font->set_colour(c);
			menu->set_front_colour(c);
		}
		font_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	void set_border_width(float width) 
	{
		if(!change_if_diff(border_width, width))
			return;
		if(p_border)
			p_border->set_width(width);
		border_width_changed(width);
		if(owner)
			owner->redraw(get_bounds());
	}
	float get_border_width() const {return border_width;}
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& c)
	{
		if(!change_if_diff(cl_hot, c))
			return;
		if(has_resources())
		{
			menu->set_hot_colour(c);
		}
		hot_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	colour get_border_colour() const {return cl_border;}
	void set_border_colour(const colour& c)
	{
		if(!change_if_diff(cl_border, c))
			return;
		border_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	colour get_border_focused_colour() const {return cl_border_f;}
	void set_border_focused_colour(const colour& c)
	{
		if(!change_if_diff(cl_border_f, c))
			return;
		border_focused_colour_changed(c);
		if(owner)
			owner->redraw(get_bounds());
	}
	int get_selected_index() const {return sel_idx;}
	void set_selected_index(unsigned int idx)
	{
		if(idx == sel_idx)
			return;
		if(idx == -1)
			sel_idx = idx;
		else
		{
			if(idx >= coll.size())
				throw invalid_argument("Index out of bounds");
			sel_idx = idx;
		}
		selected_index_changed(sel_idx);
		if(owner)
			owner->redraw(get_rect());
	}
	void select(const data& member)
	{
		auto itr = find_if(coll.begin(), coll.end(), [member](const combo_member& mem)-> bool {return mem.get_data() == member;});
		size_t idx = distance(coll.begin(), itr);
		if(idx >= coll.size() || idx < 0)
			throw logic_error("Member not found");
		if(idx == sel_idx)
			return;
		sel_idx = idx;
		selected_index_changed(idx);
		if(owner)
			owner->redraw(get_rect());
	}
	combo_member* get_selected_member() 
	{
		if(sel_idx == -1)
			throw logic_error("No member selected");
		return &coll[sel_idx];
	}
	virtual void set_opacity(float f)
	{
		if(f < 0 || f > 1.f)
			throw invalid_argument("Invalid opacity");
		if(f == get_opacity())
			return;
		if(has_resources())
		{
			br_back->set_opacity(f);
			br_border->set_opacity(f);
			br_arrow->set_opacity(f);
			br_disabled->set_opacity(f);
			br_font->set_opacity(f);
			p_border->update();
		}
		dynamic_drawsurface::set_opacity(f);
	}
protected:
	virtual void on_syscolour_changed() 
	{
		if(app::is_high_contrast_app())
		{
			br_font->set_colour(colour::button_face);
			br_font->set_colour(colour::gray_text);
		}
		else
		{
			br_back->set_colour(back_colour);
			br_font->set_colour(font_colour);
		}
		back_colour_changed(br_back->get_colour());
		font_colour_changed(br_font->get_colour());
		dynamic_drawsurface::on_syscolour_changed();
	}
	void this_mouse_click(const mouse_buttons& mb, int, const point& p)
	{
		if(mb != mouse_buttons::left)
			return;
		opened = !opened;
		if(opened)
		{
			RECT rc;
			GetClientRect(owner->get_handle(), &rc);
			POINT ps = {rc.left, rc.top};
			MapWindowPoints(owner->get_handle(), GetDesktopWindow(), &ps, 1);
			point pos(get_position().x, get_position().y + get_size().height);
			get_absolute_transform().transform_points(&pos);
			pos.x += ps.x;
			pos.y += ps.y;
			menu->show(pos, static_cast<int>(get_size().width));
		}
		else
		{
			menu->close(false);
		}
		if(owner)
			owner->redraw(get_rect());
	}
	void this_key_dwn(const virtual_keys& key, const key_extended_params& params)
	{
		if(coll.size() == 0)
			return;
		if(key == virtual_keys::up)
		{
			if(sel_idx == -1)
				sel_idx = 0;
			else
				sel_idx = max(0, --sel_idx);
		}
		else if(key == virtual_keys::down)
		{
			if(sel_idx == -1)
				sel_idx = 0;
			else
				sel_idx = min(static_cast<int>(coll.size()-1), ++sel_idx);
		}
		else
			return;
		selected_index_changed(sel_idx);
		if(owner)
			owner->redraw();
	}
	void strip_selected(int idx)
	{
		if(sel_idx == idx)
			return;
		sel_idx = idx;
		selected_index_changed(sel_idx);
	}
	void menu_closed()
	{
		bool b = false;
		opened = false;
		if(rect(get_position(), get_size()).contains(input::cursor::get_client_position(owner->get_handle()), get_absolute_transform()) && input::keyboard::get_key_state_async(virtual_keys::lbutton, b))
			opened = true;
		if(owner)
			owner->redraw(get_rect());
	}
	shared_ptr<context_menu> menu;
	vector<menu_strip> strips;
	bool opened;
	bool m_closed;
	int sel_idx;
	function<wstring(const data&)> translator;
	vector<combo_member> coll;
private:
	float border_width;
	colour cl_border, cl_border_f, cl_hot;
	shared_ptr<solid_brush> br_back, br_font, br_border, br_arrow, br_disabled;
	shared_ptr<pen> p_border;
};

enum class progress_state {normal, paused, cancelled};

class progress_bar :
	public dynamic_drawsurface
{
public:
	progress_bar();
	virtual ~progress_bar() {}
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	progress_bar& operator++();
	progress_bar& operator--();
	progress_bar& operator++(int);
	progress_bar& operator--(int);
	void set_value(float val);
	float get_value() const {return value;}
	void set_minimum(float val);
	float get_minimum() const {return minimum;}
	void set_maximum(float val);
	float get_maximum() const {return maximum;}
	virtual void set_opacity(float f);
	progress_state get_progress_state() const {return state;}
	void set_progress_state(const progress_state& state);
	colour get_front_colour() const {return front_colour;}
	void set_front_colour(colour c);
	colour get_back_colour() const {return back_colour;}
	void set_back_colour(colour c);
protected:
	float maximum, value, minimum;
private:
	shared_ptr<solid_brush> br_back, br_front;
	colour front_colour, back_colour;
	progress_state state;
};

enum class track_orientation {horizontal, vertical};

class track_bar :
	public dynamic_drawsurface
{
public:
	track_bar();
	virtual ~track_bar();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(float)> value_changed, maximum_changed, minimum_changed;
	callback<void(const colour&)> thumb_colour_changed, hot_colour_changed, pressed_colour_changed, finished_colour_changed, unfinished_colour_changed;

	void set_value(float v);
	float get_value() const {return value;}
	void set_maximum(float v);
	float get_maximum() const {return maximum;}
	void set_minimum(float v);
	float get_minimum() const {return minimum;}
	virtual void set_opacity(float f);
	colour get_thumb_colour() const {return cl_thumb;}
	void set_thumb_colour(const colour& c);
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& c);
	colour get_pressed_colour() const {return cl_down;}
	void set_pressed_colour(const colour& c);
	colour get_finished_colour() const {return cl_done;}
	void set_finished_colour(const colour& c);
	colour get_unfinished_colour() const {return cl_undone;}
	void set_unfinished_colour(const colour& c);
protected:
	float thumb_width, track_height;
	float minimum, maximum, value;
	track_orientation orient;
	void draw_track(graphics* g);
	void draw_thumb(graphics* g);
	rect get_thumb();
	void this_mouse_move(const int, const point& p);
	void this_mouse_down(const mouse_buttons& mb, const int, const point& p);
	void this_mouse_up(const mouse_buttons& mb, const int, const point& p);
	void this_mouse_leave(const point& p);
	void this_key_down(const virtual_keys& key, const key_extended_params& params);
private:
	colour cl_thumb, cl_hot, cl_down, cl_done, cl_undone;
	bool hot, down;
	bool allow_precision;
	point starting;
	shared_ptr<solid_brush> br_thumb, br_hot, br_down, br_done, br_undone;
	shared_ptr<pen> pn_done;
};

};
};

#endif