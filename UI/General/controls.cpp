#include "stdafx.h"
#include "controls.h"

namespace gcl { namespace ui {

// Button
button::button()
{
	set_title(L"button");
	border_width = 2.f;
	supress_keypress = true;
	set_captures_keyboard(false);
	disp_mode = image_display_modes::seperate;
	text_halign = horizontal_string_align::middle;
	text_valign = vertical_string_align::middle;
	img_halign = horizontal_content_align::left;
	img_valign = vertical_content_align::middle;
	back_colour = colour::gcl_dark_gray;
	cl_hot = colour::gcl_gray;
	cl_pr = colour::gray;
	cl_border = colour::gray;
	cl_borderf = colour::gcl_border;
	font_colour = colour::white;
	key_up += make_func_ptr(this, &button::this_key_up);
}

void button::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(hs_resources)
	{
		br_bk->set_opacity(f);
		br_hot->set_opacity(f);
		br_pr->set_opacity(f);
		br_fn->set_opacity(f);
		br_border->set_opacity(f);
		br_disabled->set_opacity(f);
	}
	dynamic_drawsurface::set_opacity(f);
}

void button::this_key_up(const virtual_keys& key, const key_extended_params& params)
{
	if(!supress_keypress && key == virtual_keys::_return)
		mouse_click(mouse_buttons::left, 0, get_position());
}

void button::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_bk->set_colour(colour::button_face);
		br_fn->set_colour(colour(colour::gray_text));
		br_hot->set_colour(colour::highlight);
		br_pr->set_colour(colour::hotlight);
	}
	else
	{
		br_bk->set_colour(back_colour);
		br_fn->set_colour(font_colour);
		br_hot->set_colour(cl_hot);
		br_pr->set_colour(cl_pr);
	}
	dynamic_drawsurface::on_syscolour_changed();
	if(owner)owner->redraw(get_bounds());
}

void button::render(graphics* g)
{
	g->push_clip(clip(get_rect()));
	bool hc = app::is_high_contrast_app();
	init_resources(g);
	if(get_focus() && get_enabled())
		br_border->set_colour(cl_borderf);
	else if(!get_focus() || !get_enabled())
		br_border->set_colour(cl_border);
	pn_border->update();
	if(get_enabled())
	{
		switch(get_drawing_state())
		{
		case drawing_state::normal:
			g->fill_rect(get_rect(), br_bk.get());
			break;
		case drawing_state::hot:
			g->fill_rect(get_rect(), br_hot.get());
			break;
		case drawing_state::pressed:
			g->fill_rect(get_rect(), br_pr.get());
		}
		if(image)
			g->draw_texture(image.get(), get_image_rect(), static_cast<unsigned char>(get_opacity()*255));
		g->draw_string(get_title(), get_text_rect(), get_font(), br_fn.get(), string_format::direction_left_to_right, text_halign, text_valign);
		g->pop_clip();
		g->draw_rect(rect(get_position().x+border_width/2.f, get_position().y+border_width/2.f, get_size().width-border_width, get_size().height-border_width), pn_border.get());
	}
	else
	{
		g->fill_rect(get_rect(), br_bk.get());
		if(image)
			g->draw_texture(image.get(), get_image_rect(), static_cast<unsigned char>(get_opacity()*255));
		g->draw_rect(get_rect(), pn_border.get());
		g->draw_rect(rect(get_position().x+border_width/2.f, get_position().y+border_width/2.f, get_size().width-border_width, get_size().height-border_width), pn_border.get());
		g->draw_string(get_title(), get_text_rect(), get_font(), br_fn.get(), string_format::direction_left_to_right, text_halign, text_valign);
		g->pop_clip();
		g->fill_rect(get_rect(), br_disabled.get());
	}
	dynamic_drawsurface::render(g);
}

void button::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_face));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(colour::highlight));
		br_pr = shared_ptr<solid_brush>(g->create_solid_brush(colour::hotlight));
		br_fn = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
	}
	else
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(back_colour));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(cl_hot));
		br_pr = shared_ptr<solid_brush>(g->create_solid_brush(cl_pr));
		br_fn = shared_ptr<solid_brush>(g->create_solid_brush(font_colour));
	}
	br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(50, colour::black)));
	br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
	pn_border = shared_ptr<pen>(g->create_pen(br_border.get(), border_width));
	dynamic_drawsurface::create_resources(g);
}

rect button::get_text_rect()
{
	if(!image || disp_mode == image_display_modes::overlay)
		return rect(get_position().x + border_width, get_position().y + border_width, get_size().width - 2.f*border_width, get_size().height-2.f*border_width);
	else
	{
		rect rc = get_rect();
		rc.position.x += border_width;
		rc.position.y += border_width;
		rc.sizef.width -= 2.f*border_width;
		rc.sizef.height -= 2.f*border_width;
		if(img_halign != horizontal_content_align::middle)
			rc.sizef.width -= image->get_width();
		if(img_halign == horizontal_content_align::left)
			rc.position.x += image->get_width();
		
		if(img_halign == horizontal_content_align::middle)
		{
			switch(img_valign)
			{
			case vertical_content_align::top:
				rc.position.y += image->get_height();
				rc.sizef.height -= image->get_height();
				break;
			case vertical_content_align::middle:
				rc.position.y += image->get_height()/2.f + (get_size().height - 2.f*border_width)/2.f;
				rc.sizef.height -= image->get_height()/2.f + (get_size().height - 2.f*border_width)/2.f;
				break;
			case vertical_content_align::bottom:
				rc.sizef.height -= image->get_height();
			}
		}
		return rc;
	}
}

rect button::get_image_rect()
{
	float x, y;
	switch(img_halign)
	{
	case horizontal_content_align::left:
		x = get_position().x + border_width;
		break;
	case horizontal_content_align::middle:
		x = get_position().x+border_width+(get_size().width-2.f*border_width)/2.f-image->get_width()/2.f;
		break;
	case horizontal_content_align::right:
		x = get_position().x + border_width + get_size().width - 2.f*border_width - image->get_width();
	}
	switch(img_valign)
	{
	case vertical_content_align::top:
		y = get_position().y + border_width;
		break;
	case vertical_content_align::middle:
		y = get_position().y+border_width + (get_size().height-2.f*border_width)/2.f-image->get_height()/2.f;
		break;
	case vertical_content_align::bottom:
		y = get_position().y + border_width + get_size().height - 2.f*border_width - image->get_height();
	}
	return rect(x, y, static_cast<float>(image->get_width()), static_cast<float>(image->get_height()));
}

void button::set_back_colour(const colour& c)
{
	if(!change_if_diff(back_colour, c))
		return;
	if(br_bk)
		br_bk->set_colour(c);
	back_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_font_colour(const colour& c) 
{
	if(!change_if_diff(font_colour, c))
		return;
	if(br_fn)
		br_fn->set_colour(c);
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_image_display_mode(const image_display_modes& disp)
{
	if(!change_if_diff(disp_mode, disp))
		return;
	image_display_mode_changed(disp);
	if(owner && image)
		owner->redraw(get_bounds());
}

void button::set_text_halign(const horizontal_string_align& align)
{
	if(!change_if_diff(text_halign, align))
		return;
	text_halign_changed(align);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_text_valign(const vertical_string_align& align)
{
	if(!change_if_diff(text_valign, align))
		return;
	text_valign_changed(align);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_image_halign(const horizontal_content_align& align)
{
	if(!change_if_diff(img_halign, align))
		return;
	image_halign_changed(align);
	if(owner && image)
		owner->redraw(get_bounds());
}

void button::set_image_valign(const vertical_content_align& align)
{
	if(!change_if_diff(img_valign, align))
		return;
	image_valign_changed(align);
	if(owner && image)
		owner->redraw(get_bounds());
}

void button::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hot, c))
		return;
	if(br_hot)
		br_hot->set_colour(c);
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_pressed_colour(const colour& c)
{
	if(!change_if_diff(cl_pr, c))
		return;
	if(br_pr)
		br_pr->set_colour(c);
	pressed_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_border_colour(const colour& c)
{
	if(!change_if_diff(cl_border, c))
		return;
	border_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_border_focused_colour(const colour& c)
{
	if(!change_if_diff(cl_borderf, c))
		return;
	border_focused_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_border_width(float f)
{
	if(!change_if_diff(border_width, f))
		return;
	if(pn_border)
		pn_border->set_width(border_width);
	border_width_changed(f);
	if(owner)
		owner->redraw(get_bounds());
}

void button::set_image(texture* img)
{
	if(image.get() == img)
		return;
	image.reset(img);
	image_changed(img);
	if(owner && image)
		owner->redraw(get_bounds());
}
// Button

};
};