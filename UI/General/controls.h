/*****************************************************************************
*                           gcl - controls.h                                 *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#include "stdafx.h"
#include "callback.h"
#include "graphics.h"
#include "system.h"

#ifndef CONTROL_H
#define CONTROL_H
#ifdef _MSC_VER 
#	pragma once
#endif

using namespace gcl::render_objects;

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
	virtual ~button() {}
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

};
};

#endif