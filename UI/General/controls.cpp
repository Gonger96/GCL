#include "stdafx.h"
#include "controls.h"

namespace gcl { namespace ui {

// Button
button::button()
{
	set_title(L"button");
	border_width = 1.f;
	supress_keypress = true;
	set_captures_keyboard(false);
	set_captures_mouse_wheel(false);
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

button::~button() 
{
	key_up -= make_func_ptr(this, &button::this_key_up);
}

void button::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_bk->set_opacity(f);
		br_hot->set_opacity(f);
		br_pr->set_opacity(f);
		br_fn->set_opacity(f);
		br_border->set_opacity(f);
		br_disabled->set_opacity(f);
		pn_border->update();
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
	back_colour_changed(br_bk->get_colour());
	font_colour_changed(br_fn->get_colour());
	hot_colour_changed(br_hot->get_colour());
	pressed_colour_changed(br_pr->get_colour());
	dynamic_drawsurface::on_syscolour_changed();
}

void button::render(graphics* g)
{
	g->push_clip(clip(get_rect()));
	bool hc = app::is_high_contrast_app();
	if(!has_resources())
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
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_bk->set_opacity(f);
		br_fn->set_opacity(f);
		br_hot->set_opacity(f);
		br_pr->set_opacity(f);
		br_disabled->set_opacity(f);
		br_border->set_opacity(f);
		pn_border->update();
	}
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

// Edit
edit::edit()
{
	set_min_size(size(20, 20));
	caret_running = false;
	caret_display = false;
	max_count = numeric_limits<short int>::max();
	hide_selection = false;
	pw_char = L'\u2022';
	use_pw_char = false;
	selection_start = selection_end = 0;
	read_only = false;
	m_dwn = false;
	character_casing = character_casing_types::normal;
	back_colour = colour::gcl_dark_gray;
	caret_colour = colour::white;
	font_colour = colour::white;
	cl_border = colour::gray;
	cl_border_f = colour::gcl_border;
	border_width = 1.f;
	mve_sel = clk_sel = false;
	set_captures_keyboard(true);
	set_captures_mouse_wheel(false);
	set_cursor(cursor_surface(system_cursor::i_beam));

	HINSTANCE hinst = GetModuleHandle(L"user32.dll");
	HMENU hmn = LoadMenu(hinst, MAKEINTRESOURCE(1));
	strp_cut.set_title(load_text(768, hmn, L"Cut"));
	strp_copy.set_title(load_text(769, hmn, L"Copy"));
	strp_paste.set_title(load_text(770, hmn, L"Paste"));
	strp_delete.set_title(load_text(771, hmn, L"Delete"));
	strp_select.set_title(load_text(177, hmn, L"Select all"));
	strp_cut.click += make_func_ptr(this, &edit::cut_click);
	strp_copy.click += make_func_ptr(this, &edit::copy_click);
	strp_paste.click += make_func_ptr(this, &edit::paste_click);
	strp_delete.click += make_func_ptr(this, &edit::delete_click);
	strp_delete.set_bottom_seperator(true);
	strp_select.click += make_func_ptr(this, &edit::select_click);
	DestroyMenu(hmn);

	focus_changed += make_func_ptr(this, &edit::on_focus_changed);
	char_sent += make_func_ptr(this, &edit::on_char);
	key_down += make_func_ptr(this, &edit::on_keydown);
	mouse_down += make_func_ptr(this, &edit::on_mdown);
	mouse_up += make_func_ptr(this, &edit::on_mup);
	mouse_move += make_func_ptr(this, &edit::on_mmove);
	mouse_leave += make_func_ptr(this, &edit::on_mleave);
	tmr.tick += make_func_ptr(this, &edit::caret_changed);
}

edit::~edit() 
{
	focus_changed -= make_func_ptr(this, &edit::on_focus_changed);
	char_sent -= make_func_ptr(this, &edit::on_char);
	key_down -= make_func_ptr(this, &edit::on_keydown);
	mouse_down -= make_func_ptr(this, &edit::on_mdown);
	mouse_up -= make_func_ptr(this, &edit::on_mup);
	mouse_move -= make_func_ptr(this, &edit::on_mmove);
	mouse_leave -= make_func_ptr(this, &edit::on_mleave);
	if(caret_running)
		tmr.kill();
}

wstring edit::load_text(int id, HMENU mn, const wstring& def)
{
	if(!mn)
		return move(def);
	wchar_t txt[1024];
	if(!GetMenuStringW(mn, id, txt, 1023, MF_BYCOMMAND))
		return move(def);
	wstring text(txt);
	text.shrink_to_fit();
	if(text.find(L'&') != wstring::npos)
		text.erase(remove(text.begin(), text.end(), L'&'), text.end());
	return move(text);
}

void edit::set_back_colour(const colour& c)
{
	if(!change_if_diff(back_colour, c))
		return;
	if(br_bk)
		br_bk->set_colour(c);
	back_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_font_colour(const colour& c)
{
	if(!change_if_diff(font_colour, c))
		return;
	if(br_fn)
		br_fn->set_colour(c);
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_caret_colour(const colour& c)
{
	if(!change_if_diff(caret_colour, c))
		return;
	if(br_caret)
		br_caret->set_colour(c);
	caret_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_border_colour(const colour& c)
{
	if(!change_if_diff(cl_border, c))
		return;
	border_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_border_focused_colour(const colour& c)
{
	if(!change_if_diff(cl_border_f, c))
		return;
	border_focused_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_border_width(float f)
{
	if(f < 0)
		throw invalid_argument("Borderwidth too small");
	if(!change_if_diff(border_width, f))
		return;
	if(pn)
		pn->set_width(f);
	border_width_changed(f);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_hide_selection(bool b)
{
	if(!change_if_diff(hide_selection, b))
		return;
	hide_selection_changed(b);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_max_count(int c)
{
	if(c < 0)
		throw invalid_argument("Count too small");
	if(!change_if_diff(max_count, c))
		return;
	if(title.length() > static_cast<unsigned int>(max_count))
	{
		title.erase(max_count, title.length()-max_count);
	}
	max_count_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_password_char(wchar_t cc)
{
	if(!change_if_diff(pw_char, cc))
		return;
	password_char_changed(cc);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_use_pw_char(bool b)
{
	if(!change_if_diff(use_pw_char, b))
		return;
	use_pw_char_changed(b);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_read_only(bool b)
{
	if(!change_if_diff(read_only, b))
		return;
	read_only_changed(b);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_character_casing(const character_casing_types& type)
{
	if(!change_if_diff(character_casing, type))
		return;
	character_casing_changed(type);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_title(const wstring& txt)
{
	wstring txt2 = txt;
	txt2.erase(remove_if(txt2.begin(), txt2.end(), [](wchar_t c)->bool
	{
		switch(c)
		{
		case 0x00a:
		case 0x001b:
		case L'\t':
		case L'\r':
			return true;
		default:
			return false;
		}
	}), txt2.end());
	dynamic_drawsurface::set_title(txt2);
}

void edit::caret_changed()
{
	caret_display = !caret_display;
	if(owner)
		owner->redraw(get_bounds());
}

void edit::set_display_text(const wstring& txt)
{
	if(!change_if_diff(display_text, txt))
		return;
	display_text_changed(txt);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::update_states(const point&)
{
	bool selection = selection_start != selection_end;
	strp_copy.set_enabled(selection&&!use_pw_char);
	strp_cut.set_enabled(selection&&!use_pw_char&&!read_only);
	strp_delete.set_enabled(selection&&!read_only);
	clipboard::open(owner?owner->get_handle():0);
	strp_paste.set_enabled((clipboard::is_format_available(CF_TEXT) || clipboard::is_format_available(CF_UNICODETEXT)) && !read_only);
	clipboard::close();
}

void edit::render(graphics* g)
{
	calc_text_offset(g);
	if(mve_sel && !clk_sel)
	{
		int idx = get_index_from_pos(g, input::cursor::get_client_position(owner?owner->get_handle():0));
		if(selection_start == selection_end)
		{
			if(idx > selection_start)
			{
				caret_start = false;
				selection_end = idx;
				selection_changed(selection_start, selection_end-selection_start);
			}
			else if(idx < selection_start)
			{
				caret_start = true;
				selection_start = idx;
				selection_changed(selection_start, selection_end-selection_start);
			}
		}
		else
		{
			if(caret_start)
			{
				selection_start = idx;
				selection_changed(selection_start, selection_end-selection_start);
			}
			else
			{
				selection_end = idx;
				selection_changed(selection_start, selection_end-selection_start);
			}
		}
		mve_sel = false;
	}
	else if(clk_sel)
	{
		selection_start = selection_end = get_index_from_pos(g, input::cursor::get_client_position(owner?owner->get_handle():0));
		selection_changed(selection_start, selection_end-selection_start);
		clk_sel = false;
	}
	auto st = g->get_text_rendering_mode();
	g->set_text_rendering_mode(text_rendering_modes::antialias);
	if(!has_resources())
		init_resources(g);
	if(get_focus() && get_enabled() && !read_only)
		br_border->set_colour(cl_border_f);
	else if(!get_focus() || !get_enabled())
		br_border->set_colour(cl_border);
	pn->update();
	rect rc(get_position(), get_size());
	g->fill_rect(rc, br_bk.get());
	rc.inflate(-border_width, - border_width);
	g->draw_rect(rect(get_position().x+border_width/2.f, get_position().y+border_width/2.f, get_size().width-border_width, get_size().height-border_width), pn.get());
	rect metric = get_font()->get_metrics(get_string(), size::max_size(), g);
	g->push_clip(clip(rc));
	if(selection_start != selection_end && (get_focus() && hide_selection || !hide_selection) && get_enabled())
		g->fill_rect(get_selection_rect(g), br_border.get());
	if(!get_focus() && get_title() == L"")
	{
		rect metric_c = get_font()->get_metrics(display_text, size::max_size(), g);
		if(br_fn->get_opacity() != get_opacity()*0.5f)
			br_fn->set_opacity(get_opacity()*0.5f);
		g->draw_string(display_text, point(get_position().x + 3.f+border_width-text_offset, get_position().y + get_size().height / 2 - metric_c.get_height()/2), get_font(), br_border.get());
		br_fn->set_opacity(get_opacity());
	}
	if(use_pw_char)
		g->draw_string(get_string(), point(get_position().x + 3.f+border_width-text_offset, get_position().y + get_size().height / 2 - metric.get_height()/2), get_font(), br_fn.get());
	else
		g->draw_string(get_string(), point(get_position().x + 3.f+border_width-text_offset, get_position().y + get_size().height / 2 - metric.get_height()/2), get_font(), br_fn.get());
	if(caret_display && get_enabled())
		g->fill_rect(get_caret_rect(g), br_caret.get());
	g->pop_clip();
	if(!get_enabled() || read_only)
		g->fill_rect(rect(get_position(), get_size()), br_disabled.get());
	g->set_text_rendering_mode(st);
	dynamic_drawsurface::render(g);
}

void edit::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_face));
		br_fn = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
		br_caret = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_highlight));
	}
	else
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(back_colour));
		br_fn = shared_ptr<solid_brush>(g->create_solid_brush(font_colour));
		br_caret = shared_ptr<solid_brush>(g->create_solid_brush(caret_colour));
	}
	br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
	br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(50, colour::black)));
	pn = shared_ptr<pen>(g->create_pen(br_border.get(), border_width));
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_bk->set_opacity(f);
		br_fn->set_opacity(f);
		br_caret->set_opacity(f);
		br_border->set_opacity(f);
		br_disabled->set_opacity(f);
		pn->update();
	}
	context_menu* menu = new context_menu(owner?owner->get_handle():0, g);
	menu->add_strip(&strp_cut);
	menu->add_strip(&strp_copy);
	menu->add_strip(&strp_paste);
	menu->add_strip(&strp_delete);
	menu->add_strip(&strp_select);
	menu->shown += make_func_ptr(this, &edit::update_states);
	set_menu(menu);
	dynamic_drawsurface::create_resources(g);
}

wstring edit::get_string()
{
	wstring s;
	if(use_pw_char)
		s = wstring(title.length(), pw_char);
	else
		s = title;
	if(character_casing == character_casing_types::lower)
		transform(s.begin(), s.end(), s.begin(), towlower);
	else if(character_casing == character_casing_types::upper)
		transform(s.begin(), s.end(), s.begin(), towupper);
	return s;
}

rect edit::get_caret_rect(graphics* g)
{
	if(selection_end == 0)
	{
		return rect(get_position().x+3.f+border_width-text_offset, get_position().y+1.f+border_width, 1.f, get_size().height-2.f-2*border_width); 
	}
	wstring sub = caret_start ? get_string().substr(0, selection_start) : get_string().substr(0, selection_end);
	rect metric = get_font()->get_metrics(sub, size::max_size(), g);
	return rect(get_position().x+metric.get_width()+3.f+border_width-text_offset, get_position().y+1.f+border_width, 1.f, get_size().height-2.f-2*border_width);
}

rect edit::get_selection_rect(graphics* g)
{
	if(selection_start - selection_end == 0)
		throw invalid_argument("Invalid selection");
	wstring text = get_string().substr(0, selection_end);
	wstring dub = get_string().substr(0, selection_start);
	rect metricstart = get_font()->get_metrics(dub, size::max_size(), g);
	rect metriclen = get_font()->get_metrics(text, size::max_size(), g);
	return rect(get_position().x+metricstart.get_width()+3.f+border_width-text_offset, get_position().y+1.f+border_width, metriclen.get_width()-metricstart.get_width(), get_size().height -2.f-2*border_width);
}

void edit::calc_text_offset(graphics* g)
{
	rect rc;
	if(selection_end == 0)
	{
		rc = rect(get_position().x+3.f+border_width, get_position().y+1.f+border_width, 1.f, get_size().height-2.f-2*border_width); 
	}
	else
	{
		wstring sub = caret_start ? get_string().substr(0, selection_start) : get_string().substr(0, selection_end);
		rect metric = get_font()->get_metrics(sub, size::max_size(), g);
		rc = rect(get_position().x+metric.get_width()+3.f+border_width, get_position().y+1.f+border_width, 1.f, get_size().height-2.f-2*border_width);
	}
	rect txtmetric = get_font()->get_metrics(get_string(), size::max_size(), g);
	if(sz != get_size() && get_position().x+get_size().width-2*border_width<rc.position.x)
		text_offset = (rc.get_x()+rc.get_width())-(get_position().x+get_size().width-2*border_width);
	else if(get_position().x+get_size().width-2*border_width<get_caret_rect(g).position.x)
		text_offset = (rc.get_x()+rc.get_width())-(get_position().x+get_size().width-2*border_width);
	else if(get_position().x+border_width>get_caret_rect(g).position.x)
		text_offset = (rc.get_x()+rc.get_width())-(get_position().x+2*border_width);
	if(selection_end == 0 || txtmetric.get_width() < (get_size().width - 2*border_width))
		text_offset = 0;
	sz = get_size();
}

void edit::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_bk->set_colour(colour::button_face);
		br_fn->set_colour(colour::gray_text);
		br_caret->set_colour(colour::button_highlight);
	}
	else
	{
		br_bk->set_colour(back_colour);
		br_fn->set_colour(font_colour);
		br_caret->set_colour(caret_colour);
	}
	back_colour_changed(br_bk->get_colour());
	font_colour_changed(br_fn->get_colour());
	caret_colour_changed(br_caret->get_colour());
	dynamic_drawsurface::on_syscolour_changed();
}

void edit::set_opacity(float f)
{
	if(f < 0 || f > 1.f)
		throw invalid_argument("Invalid opacity");
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_bk->set_opacity(f);
		br_border->set_opacity(f);
		br_caret->set_opacity(f);
		br_disabled->set_opacity(f);
		br_fn->set_opacity(f);
		pn->update();
	}
	dynamic_drawsurface::set_opacity(f);
}

int edit::get_index_from_pos(graphics* g, const point& p) // Nicht transformsicher! Mal mit Rects probieren
{
	matrix m = get_absolute_transform();
	m.invert();
	point pc(p);
	m.transform_points(&pc);
	float x = pc.x;
	wstring s = get_string();
	rect metric2 = get_font()->get_metrics(s, size::max_size(), g);
	if(x > (metric2.get_width()+get_position().x))
		return s.length();
	else if(x < metric2.get_x()+get_position().x)
		return 0;
	else if(x>get_size().width+get_position().x-2*border_width-get_font()->get_size() && selection_start != selection_end)
		return caret_start ? min(selection_start+1u, s.length()) : min(selection_end+1u, s.length());
	else if(x<get_position().x+border_width+get_font()->get_size() && selection_start != selection_end)
		return caret_start ? max(selection_start-1, 0) : max(selection_end-1, 0);
	for(unsigned i = 0; i < s.length(); ++i)
	{
		rect metric = get_font()->get_metrics(s.substr(0, i), size::max_size(), g);
		float cw = get_font()->get_metrics(s.substr(i, 1), size::max_size(), g).get_width();
		metric.sizef.width -= text_offset;
		if(is_between_equ((metric.sizef.width+get_position().x) - x, -cw/2.f, cw/2.f))
		{
			return i;
		}
	}
	return caret_start ? selection_start : selection_end;
}

void edit::on_mup(const mouse_buttons& b, int mod, const point& p)
{
	if(b != mouse_buttons::left)
		return;
	clk_sel = false;
	m_dwn = false;
	if(owner)
		owner->redraw(get_bounds());
}

void edit::on_mleave(const point& p2)
{
	point p(p2);
	matrix m = get_transform();
	m.invert();
	m.transform_points(&p);
	if(m_dwn)
	{
		m_dwn = false;
		mve_sel = clk_sel = false;
		if(p.x >= get_position().x+get_size().width)
		{
			selection_end = get_string().length();
			selection_changed(selection_start, selection_end-selection_start);
		}
		if(p.x <= get_position().x)
		{
			selection_start = 0;
			selection_changed(selection_start, selection_end-selection_start);
		}
		if(owner)
			owner->redraw(get_bounds());
	}
}

void edit::on_mdown(const mouse_buttons& b, int mod, const point& p)
{
	if(b != mouse_buttons::left)
		return;
	m_dwn = true;
	clk_sel = true;
	if(owner)
		owner->redraw(get_bounds());
}

void edit::on_mmove(int mod, const point& p)
{
	if(mod != mouse_modifiers::l_button || !m_dwn)
		return;
	mve_sel = true;
	if(owner)
		owner->redraw(get_bounds());
}

void edit::on_focus_changed(bool b)
{
	if(b)
	{
		if(owner)
		{
			if(caret_running) return;
			caret_display = true;
			caret_running = true;
			tmr.create(owner->get_handle(), chrono::milliseconds(GetCaretBlinkTime()));
		}
	}
	else
	{
		if(!caret_running) return;
		caret_display = false;
		caret_running = false;
		if(tmr.is_running())
			tmr.kill();
		if(owner)
			owner->redraw(get_bounds());
	}
}

void edit::on_char(wchar_t cc, const key_extended_params& params)
{
	bool b;
	if(read_only || (input::keyboard::get_key_state_async(virtual_keys::control, b) &&!(input::keyboard::get_key_state_async(virtual_keys::rmenu, b) && input::keyboard::get_key_state_async(virtual_keys::lcontrol, b))))
		return;
	if(on_char_adding(cc))
	{
		switch(cc)
		{
		case L'\b': // Backspace
			if(selection_start == selection_end)
			{
				if(selection_start != 0)
				{
					title.erase(selection_start-1, 1);
					selection_end = --selection_start;
					title_changed(title);
				}
			}
			else
				delete_selection();
			break;
		case 0x000a: // Linefeed
		case 0x001b: // Escape
		case L'\r': // Carriage return
		case L'\t': // Tab
			break;
		default:
			if(title.length()==max_count || !on_char_adding(cc))
				return;
			if(selection_start == selection_end)
			{
				wchar_t ccd[] = {cc, 0};
				title.insert(selection_start, ccd);
				selection_end = ++selection_start;
				title_changed(title);
			}
			else
			{
				wchar_t ccd[] = {cc, 0};
				delete_selection();
				title.insert(selection_start, ccd);
				selection_end = ++selection_start;
				title_changed(title);
			}
		}
		if(owner)
			owner->redraw(get_bounds());
	}
}

void edit::on_keydown(const virtual_keys& key, const key_extended_params& params)
{
	if(!is_available())
		return;
	bool last_state;
	if(input::keyboard::get_key_state_async(virtual_keys::shift, last_state))
	{
		if(key == virtual_keys::left)
		{
			if(selection_start == 0 && selection_end == 0)
				return;
			if(selection_start == selection_end)
			{
				caret_start = true;
				if(selection_start != 0)
					selection_start--;
			}
			else
			{
				if(caret_start && selection_start != 0)
					selection_start--;
				else if(!caret_start && selection_end != 0)
					selection_end--;
			}

			selection_changed(selection_start, selection_end-selection_start);
		}
		else if(key == virtual_keys::right)
		{
			if(selection_start == title.length() && selection_end == title.length())
				return;
			if(selection_start == selection_end)
			{
				caret_start = false;
				if(selection_end != title.length())
					selection_end++;
			}
			else
			{
				if(caret_start && selection_start != title.length())
					selection_start++;
				else if(!caret_start && selection_end != title.length())
					selection_end++;
			}
			selection_changed(selection_start, selection_end-selection_start);
		}
		else if(key == virtual_keys::home)
		{
			selection_start = 0;
			selection_changed(selection_start, selection_end-selection_start);
		}
		else if(key == virtual_keys::end)
		{
			selection_end = get_title().length();
			selection_changed(selection_start, selection_end-selection_start);
		}
	}
	else if(input::keyboard::get_key_state_async(virtual_keys::control, last_state))
	{
		switch(key)
		{
		case virtual_keys::key_a:
			selection_start = 0;
			selection_end = title.length();
			selection_changed(selection_start, selection_end-selection_start);
			break;
		case virtual_keys::key_c:
			if(!use_pw_char)
				copy_selection();
			break;
		case virtual_keys::key_x:
			if(!use_pw_char && !read_only)
				cut_selection();
			break;
		case virtual_keys::key_v:
			if(!read_only)
				try_paste_into_selection();
			break;
		case virtual_keys::key_h:
			if(!read_only)
				delete_selection();
		}
	}
	else
	{
		if(key == virtual_keys::left)
		{
			caret_start = true;
			if(selection_start != 0 && selection_start == selection_end)
				selection_start--;
			selection_end = selection_start;
			selection_changed(selection_start, selection_end-selection_start);
			caret_display = true;
		}
		else if(key == virtual_keys::right)
		{
			caret_start = true;
			if(selection_start != title.length() && selection_start == selection_end)
				selection_end++;
			selection_start = selection_end;
			selection_changed(selection_start, selection_end-selection_start);
			caret_display = true;
		}
		else if(key == virtual_keys::_delete)
		{
			if(read_only)
				return;
			if(selection_start != selection_end)
				delete_selection();
			else if(selection_start == selection_end && selection_start != title.length())
				title.erase(selection_start, 1);
		}
		else if(key == virtual_keys::home)
		{
			selection_start = selection_end = 0;
			selection_changed(selection_start, selection_end-selection_start);
		}
		else if(key == virtual_keys::end)
		{
			selection_start = selection_end = get_title().length();
			selection_changed(selection_start, selection_end-selection_start);
		}
	}
	if(owner)
		owner->redraw(get_bounds());
}

wstring edit::get_selection_content() const
{
	if(selection_start >= selection_end || static_cast<unsigned>((selection_end-selection_start)) > title.length())
		return L"";
	return move(title.substr(selection_start, selection_end-selection_start));
}

void edit::copy_selection()
{
	wstring sub = get_selection_content();
	HGLOBAL glmem = GlobalAlloc(GMEM_MOVEABLE, sizeof(wchar_t)*(sub.length()+1));
	if(!glmem)
		throw runtime_error("Not enough memory");
	memcpy(GlobalLock(glmem), sub.data(), sizeof(wchar_t)*(sub.length()+1));
	GlobalUnlock(glmem);
	try
	{
		clipboard::open(owner ? owner->get_handle() : 0);
	}
	catch(...)
	{
		return;
	}
	clipboard::set_data(CF_UNICODETEXT, glmem);
	clipboard::close();
}

void edit::try_paste_into_selection()
{
	try
	{
		clipboard::open(owner ? owner->get_handle() : 0);
	}
	catch(...)
	{
		return;
	}
	if(clipboard::is_format_available(CF_UNICODETEXT))
	{
		LPWSTR str = clipboard::get_data<LPWSTR>(CF_UNICODETEXT);
		wstring txt_paste(str);
		txt_paste.erase(remove_if(txt_paste.begin(), txt_paste.end(), [](wchar_t cc) -> bool 
		{
			switch(cc)
			{
			case L'\b':
			case 0x000a:
			case 0x001b:
			case L'\r':
			case L'\t':
				return true;
			default:
				return false;
			}
		}), txt_paste.end());
		if(static_cast<int>(txt_paste.length() + title.length()) - (selection_end - selection_start) <= max_count)
		{
			delete_selection();
			title.insert(selection_start, txt_paste);
			selection_start = selection_end = selection_start+txt_paste.length();
			selection_changed(selection_start, selection_end-selection_start);
			title_changed(title);
		}
	}
	else if(clipboard::is_format_available(CF_TEXT))
	{
		LPSTR str = clipboard::get_data<LPSTR>(CF_TEXT);
		wstring txt_paste = string_to_unicode_string(string(str));
		txt_paste.erase(remove_if(txt_paste.begin(), txt_paste.end(), [](wchar_t cc) -> bool 
		{
			switch(cc)
			{
			case L'\b':
			case 0x000a:
			case 0x001b:
			case L'\r':
			case L'\t':
				return true;
			default:
				return false;
			}
		}), txt_paste.end());
		if(static_cast<int>(txt_paste.length() + title.length()) - (selection_end - selection_start) <= max_count)
		{
			delete_selection();
			title.insert(selection_start, txt_paste);
			selection_start = selection_end = selection_start+txt_paste.length();
			selection_changed(selection_start, selection_end-selection_start);
			title_changed(title);
		}
	}
	clipboard::close();
	if(owner)
		owner->redraw(get_bounds());
}

void edit::cut_selection()
{
	copy_selection();
	delete_selection();
}

void edit::delete_selection()
{
	if(selection_start >= selection_end || static_cast<unsigned>((selection_end-selection_start)) > title.length())
		return;
	title.erase(selection_start, selection_end-selection_start);
	title_changed(title);
	selection_end = selection_start;
	selection_changed(selection_start, selection_end-selection_start);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::deselect()
{
	selection_end = selection_start;
	selection_changed(selection_start, selection_end-selection_start);
	if(owner)
		owner->redraw(get_bounds());
}

void edit::select(int sel_start, int sel_len)
{
	if(sel_start < 0 || sel_len < 0 || sel_start + sel_len > static_cast<int>(title.length()))
		throw invalid_argument("Invalid selection");
	selection_start = sel_start;
	selection_end = sel_len + sel_start;
	selection_changed(selection_start, selection_end-selection_start);
	if(owner)
		owner->redraw(get_bounds());
}
// Edit

// HScroll
hscroll::hscroll() : arrow_size(10.f)
{
	set_captures_keyboard(true);
	set_captures_mouse_wheel(true);
	set_min_size(size(30.f, static_cast<float>(GetSystemMetrics(SM_CYHSCROLL))));
	cl_bk = colour::gcl_gray;
	cl_fr = colour::gcl_front_gray;
	cl_hi = colour::gcl_hot_gray;
	cl_pr = colour::gcl_highlight_gray;
	large_step = 5.f;
	small_step = 1.f;
	hmin = 0;
	hmax = 10.f;
	value = 0.f;
	mouse_h_wheel += make_func_ptr(this, &hscroll::this_mouse_h_wheel);
	key_down += make_func_ptr(this, &hscroll::this_keypress);
}

hscroll::~hscroll()
{
	mouse_h_wheel -= make_func_ptr(this, &hscroll::this_mouse_h_wheel);
	key_down -= make_func_ptr(this, &hscroll::this_keypress);
}

void hscroll::set_large_step(float f)
{
	if(!change_if_diff(large_step, f)) return;
	large_step_changed(f);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_small_step(float f)
{
	if(!change_if_diff(small_step, f)) return;
	small_step_changed(f);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_back_colour(const colour& c)
{
	if(!change_if_diff(cl_bk, c)) return;
	if(br_bk)
		br_bk->set_colour(c);
	back_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_front_colour(const colour& c)
{
	if(!change_if_diff(cl_fr, c)) return;
	if(br_fr)
		br_fr->set_colour(c);
	front_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hi, c)) return;
	if(br_hi)
		br_hi->set_colour(c);
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_pressed_colour(const colour& c)
{
	if(!change_if_diff(cl_pr, c)) return;
	if(br_pr)
		br_pr->set_colour(c);
	pressed_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_max(float val)
{
	if(val <= hmin)
		throw invalid_argument("Maximum must be larger than minimum");
	if(!change_if_diff(hmax, val)) return;	
	value = min(value, val);
	max_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_min(float val)
{
	if(val >= hmax)
		throw invalid_argument("Minimum must be smaller than maximum");
	if(!change_if_diff(hmin, val)) return;
	value = max(hmin, value);
	min_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::set_value(float val)
{
	if(val < hmin || val > hmax)
		throw out_of_range("Value out of range");
	if(!change_if_diff(value, val)) return;
	value_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void hscroll::scroll(int delta)
{
	float oldh = value;
	value -= (delta/WHEEL_DELTA)*large_step;
	value = min(max(hmin, value), hmax);
	if(oldh != value)
	{
		value_changed(value);
		scrolling(value);
		if(owner)
			owner->redraw(get_bounds());
	}
}

void hscroll::render(graphics* g)
{
	float height = get_size().height;
	if(!has_resources())
		init_resources(g);
	point arr1[] = {point(get_position().x+arrow_size/5.f, get_position().y+height/2.f),
					point(get_position().x+arrow_size-arrow_size/5.f, get_position().y+height/5.f),
					point(get_position().x+arrow_size-arrow_size/5.f,get_position().y+height-height/5.f)};
	point arr2[] = {point((get_position().x+get_size().width) - arrow_size/5.f, get_position().y+height/2.f),
					point((get_position().x+get_size().width) - arrow_size + arrow_size/5.f, get_position().y+height/5.f),
					point((get_position().x+get_size().width) - arrow_size + arrow_size/5.f, get_position().y+height-height/5.f)};
	g->fill_rect(rect(get_position().x, get_position().y, max(get_size().width, 3*arrow_size+10.f), height), br_bk.get());

	if(hbar_ov && !hbar_dw)
		g->fill_rect(get_bar(), br_hi.get());
	else if(hbar_dw)
		g->fill_rect(get_bar(), br_pr.get());
	else
		g->fill_rect(get_bar(), br_fr.get());
	
	if(hleft_arr_ov && !hleft_arr_dw)
		g->fill_polygon(arr1, 3, br_hi.get());
	else if(hleft_arr_dw)
		g->fill_polygon(arr1, 3, br_pr.get());
	else
		g->fill_polygon(arr1, 3, br_fr.get());
	
	if(hright_arr_ov && !hright_arr_dw)
		g->fill_polygon(arr2, 3, br_hi.get());
	else if(hright_arr_dw)
		g->fill_polygon(arr2, 3, br_pr.get());
	else
		g->fill_polygon(arr2, 3, br_fr.get());
	dynamic_drawsurface::render(g);
}

rect hscroll::get_bar() const
{
	float height = get_size().height;
	float track = get_size().width-3*arrow_size;
	float thumb = track;
	if(hmax-hmin != 0 && large_step != 0)
		thumb = (large_step*track)/(hmax-hmin);
	thumb = max(min(thumb, track), 5.f);
	float pixel_range = track - thumb;
	float range = hmax-hmin;
	float perc = 0;
	if(range != 0)
		perc = (value-hmin) / range;
	float pos = perc*pixel_range + arrow_size*1.5f+get_position().x;
	pos = min(max(get_position().x+1.5f*arrow_size, pos), get_size().width+get_position().x-1.5f*arrow_size-thumb);
	return rect(pos,get_position().y+height/5.f, thumb, height-height/2.5f);
}

void hscroll::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::scrollbar)));
		br_fr = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::window_text)));
		br_hi = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::highlight)));
		br_pr = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::hotlight)));
	}
	else
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(cl_bk));
		br_fr = shared_ptr<solid_brush>(g->create_solid_brush(cl_fr));
		br_hi = shared_ptr<solid_brush>(g->create_solid_brush(cl_hi));
		br_pr = shared_ptr<solid_brush>(g->create_solid_brush(cl_pr));
	}
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_bk->set_opacity(f);
		br_fr->set_opacity(f);
		br_hi->set_opacity(f);
		br_pr->set_opacity(f);
	}
	dynamic_drawsurface::create_resources(g);
}

void hscroll::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_bk->set_colour(colour::scrollbar);
		br_fr->set_colour(colour::window_text);
		br_hi->set_colour(colour::highlight);
		br_pr->set_colour(colour::hotlight);
	}
	else
	{
		br_bk->set_colour(cl_bk);
		br_fr->set_colour(cl_fr);
		br_hi->set_colour(cl_hi);
		br_pr->set_colour(cl_pr);
	}
	dynamic_drawsurface::on_syscolour_changed();
}

void hscroll::set_opacity(float f)
{
	if(get_opacity() == f) return;
	if(has_resources())
	{
		br_bk->set_opacity(f);
		br_fr->set_opacity(f);
		br_hi->set_opacity(f);
		br_pr->set_opacity(f);
	}
	dynamic_drawsurface::set_opacity(f);
}

void hscroll::set_min_size(const size& sz)
{
	if(sz.height < get_min_size().height || sz.width < 30)
		throw invalid_argument("Size too small");
	dynamic_drawsurface::set_min_size(sz);
}

void hscroll::on_mouse_move(const int m, const point& p)
{
	float height = get_size().height;
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	bool rdr = false;	
	if(change_if_diff(hleft_arr_ov, rect(get_position().x, get_position().y, arrow_size, height).contains(pn)))
		rdr = true;
	if(change_if_diff(hright_arr_ov, rect(get_position().x+get_size().width - arrow_size, get_position().y, arrow_size, height).contains(pn)))
		rdr = true;
	if(change_if_diff(hbar_ov, get_bar().contains(pn)))
		rdr = true;
	if(hbar_dw)
	{
		float oldvalue = value;
		float track = get_size().width-3*arrow_size;
		float thumb = track;
		if(hmax-hmin != 0 && large_step != 0)
			thumb = (large_step*track)/(hmax-hmin);
		thumb = max(min(thumb, track), 5.f);
		float pixel_range = track - thumb;
		float thumb_pos = pn.x - fix;
		float perc = 0.f;
		if(pixel_range != 0)
			perc = (thumb_pos)/pixel_range;
		value = perc*(hmax-hmin) + hmin;
		value = max(min(value, hmax), hmin);
		if(value != oldvalue) {scrolling(value); value_changed(value); rdr=true;}
	}
	if(rdr && owner) 
		owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_move(m, p);
}

void hscroll::on_mouse_down(const mouse_buttons& b, const int m, const point& p)
{
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	if(b == mouse_buttons::left)
	{
		if(change_if_diff(hleft_arr_dw, hleft_arr_ov))
		{
			value -= large_step;
			if(value < hmin) value = hmin;
			scrolling(value);
			value_changed(value);
			if(owner)
				owner->redraw(get_bounds());
		}
		if(change_if_diff(hright_arr_dw, hright_arr_ov))
		{
			value += large_step;
			if(value > hmax) value = hmax;
			scrolling(value);
			value_changed(value);
			if(owner)
				owner->redraw(get_bounds());
		}
		if(change_if_diff(hbar_dw, hbar_ov))
		{
			fix = pn.x-(get_bar().get_x()-get_position().x-1.5f*arrow_size);
		}
	}
	dynamic_drawsurface::on_mouse_down(b, m, p);
}

void hscroll::on_mouse_up(const mouse_buttons& b, const int m, const point& p)
{
	if(change_if_diff(hleft_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(hright_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(hbar_dw, false))
		if(owner)owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_up(b, m, p);
}

void hscroll::this_mouse_h_wheel(const int m, const point& p, int delta)
{
	if(!is_available())
		return;
	float oldh = value;
	value -= (delta/WHEEL_DELTA)*large_step;
	value = min(max(hmin, value), hmax);
	if(oldh != value)
	{
		value_changed(value);
		scrolling(value);
		if(owner)
			owner->redraw(get_bounds());
	}
}

void hscroll::on_mouse_leave(const point& p)
{
	hleft_arr_dw = hright_arr_dw = false;
	hleft_arr_ov = hright_arr_ov = false;
	hbar_ov = hbar_dw = false;
	if(owner)
		owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_leave(p);
}

void hscroll::this_keypress(const virtual_keys& key, const key_extended_params& params)
{
	float old_val = value;
	switch(key)
	{
	case virtual_keys::left:
		value = max(min(hmax, value - small_step*params.repeat_count), hmin);
		break;
	case virtual_keys::right:
		value = max(min(hmax, value + small_step*params.repeat_count), hmin);
	}
	if(old_val != value)
	{
		scrolling(value);
		value_changed(value);
		if(owner)
			owner->redraw(get_bounds());
	}
}
// HScroll

// VScroll
vscroll::vscroll() : arrow_size(10.f)
{
	set_captures_keyboard(true);
	set_captures_mouse_wheel(true);
	set_min_size(size(static_cast<float>(GetSystemMetrics(SM_CXVSCROLL)), 30.f));
	cl_bk = colour::gcl_gray;
	cl_fr = colour::gcl_front_gray;
	cl_hi = colour::gcl_hot_gray;
	cl_pr = colour::gcl_highlight_gray;
	large_step = 5.f;
	small_step = 1.f;
	vmin = 0;
	vmax = 10.f;
	value = 0.f;
	mouse_wheel += make_func_ptr(this, &vscroll::this_mouse_wheel);
	key_down += make_func_ptr(this, &vscroll::this_keypress);
}

vscroll::~vscroll()
{
	mouse_wheel -= make_func_ptr(this, &vscroll::this_mouse_wheel);
	key_down -= make_func_ptr(this, &vscroll::this_keypress);
}

void vscroll::set_large_step(float f)
{
	if(!change_if_diff(large_step, f)) return;
	large_step_changed(f);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_small_step(float f)
{
	if(!change_if_diff(small_step, f)) return;
	small_step_changed(f);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_back_colour(const colour& c)
{
	if(!change_if_diff(cl_bk, c)) return;
	if(br_bk)
		br_bk->set_colour(c);
	back_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_front_colour(const colour& c)
{
	if(!change_if_diff(cl_fr, c)) return;
	if(br_fr)
		br_fr->set_colour(c);
	front_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hi, c)) return;
	if(br_hi)
		br_hi->set_colour(c);
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_pressed_colour(const colour& c)
{
	if(!change_if_diff(cl_pr, c)) return;
	if(br_pr)
		br_pr->set_colour(c);
	pressed_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_max(float val)
{
	if(val <= vmin)
		throw invalid_argument("Maximum must be larger than minimum");
	if(!change_if_diff(vmax, val)) return;	
	value = min(value, val);
	max_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_min(float val)
{
	if(val >= vmax)
		throw invalid_argument("Minimum must be smaller than maximum");
	if(!change_if_diff(vmin, val)) return;
	value = max(vmin, value);
	min_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::set_value(float val)
{
	if(val < vmin || val > vmax)
		throw out_of_range("Value out of range");
	if(!change_if_diff(value, val)) return;
	value_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void vscroll::scroll(int delta)
{
	float oldh = value;
	value -= (delta/WHEEL_DELTA)*large_step;
	value = min(max(vmin, value), vmax);
	if(oldh != value)
	{
		value_changed(value);
		scrolling(value);
		if(owner)
			owner->redraw(get_bounds());
	}
}

void vscroll::render(graphics* g)
{
	if(!has_resources())
		init_resources(g);
	float width = get_size().width;
	point arr1[] = {point(get_position().x + width/2.f, get_position().y+arrow_size/5.f), 
					point(get_position().x+width-width/5.f, get_position().y+arrow_size-arrow_size/5.f),
					point(get_position().x+width/5.f, get_position().y+arrow_size-arrow_size/5.f)};
	point arr2[] = {point(get_position().x + width/2.f, get_position().y+get_size().height-arrow_size/5.f), 
					point(get_position().x+width-width/5.f, get_position().y+get_size().height-arrow_size+arrow_size/5.f),
					point(get_position().x+width/5.f, get_position().y+get_size().height-arrow_size+arrow_size/5.f)};
	g->fill_rect(rect(get_position().x, get_position().y, width, max(get_size().height, 3*arrow_size+10.f)), br_bk.get());

	if(vbar_ov && !vbar_dw)
		g->fill_rect(get_bar(), br_hi.get());
	else if(vbar_dw)
		g->fill_rect(get_bar(), br_pr.get());
	else
		g->fill_rect(get_bar(), br_fr.get());
	if(vtop_arr_ov && !vtop_arr_dw)
		g->fill_polygon(arr1, 3, br_hi.get());
	else if(vtop_arr_dw)
		g->fill_polygon(arr1, 3, br_pr.get());
	else
		g->fill_polygon(arr1, 3, br_fr.get());
	
	if(vbottom_arr_ov && !vbottom_arr_dw)
		g->fill_polygon(arr2, 3, br_hi.get());
	else if(vbottom_arr_dw)
		g->fill_polygon(arr2, 3, br_pr.get());
	else
		g->fill_polygon(arr2, 3, br_fr.get());
	dynamic_drawsurface::render(g);
}

rect vscroll::get_bar() const
{
	float width = get_size().width;
	float track = get_size().height-3*arrow_size;
	float thumb = track;
	if(vmax-vmin != 0 && large_step != 0)
		thumb = (large_step*track)/(vmax-vmin);
	thumb = max(min(thumb, track), 5.f);
	float pixel_range = track - thumb;
	float range = vmax-vmin;
	float perc = 0;
	if(range != 0)
		perc = (value-vmin) / range;
	float pos = perc*pixel_range + arrow_size*1.5f+get_position().y;
	pos = min(max(get_position().y+1.5f*arrow_size, pos), get_position().y+get_size().height-1.5f*arrow_size-thumb);
	return rect(get_position().x+width/5.f, pos, width-width/2.5f, thumb);
}

void vscroll::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::scrollbar)));
		br_fr = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::window_text)));
		br_hi = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::highlight)));
		br_pr = shared_ptr<solid_brush>(g->create_solid_brush(colour(colour::hotlight)));
	}
	else
	{
		br_bk = shared_ptr<solid_brush>(g->create_solid_brush(cl_bk));
		br_fr = shared_ptr<solid_brush>(g->create_solid_brush(cl_fr));
		br_hi = shared_ptr<solid_brush>(g->create_solid_brush(cl_hi));
		br_pr = shared_ptr<solid_brush>(g->create_solid_brush(cl_pr));
	}
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_bk->set_opacity(f);
		br_fr->set_opacity(f);
		br_hi->set_opacity(f);
		br_pr->set_opacity(f);
	}
	dynamic_drawsurface::create_resources(g);
}

void vscroll::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_bk->set_colour(colour::scrollbar);
		br_fr->set_colour(colour::window_text);
		br_hi->set_colour(colour::highlight);
		br_pr->set_colour(colour::hotlight);
	}
	else
	{
		br_bk->set_colour(cl_bk);
		br_fr->set_colour(cl_fr);
		br_hi->set_colour(cl_hi);
		br_pr->set_colour(cl_pr);
	}
	dynamic_drawsurface::on_syscolour_changed();
}

void vscroll::set_opacity(float f)
{
	if(get_opacity() == f) return;
	if(has_resources())
	{
		br_bk->set_opacity(f);
		br_fr->set_opacity(f);
		br_hi->set_opacity(f);
		br_pr->set_opacity(f);
	}
	dynamic_drawsurface::set_opacity(f);
}

void vscroll::set_min_size(const size& sz)
{
	if(sz.width < get_min_size().width || sz.height < 30)
		throw invalid_argument("Size too small");
	dynamic_drawsurface::set_min_size(sz);
}

void vscroll::on_mouse_move(const int m, const point& p)
{
	float width = get_size().width;
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	bool rdr = false;	
	if(change_if_diff(vtop_arr_ov, rect(get_position().x, get_position().y, width, arrow_size).contains(pn)))
		rdr = true;
	if(change_if_diff(vbottom_arr_ov, rect(get_position().x, get_position().y+get_size().height-arrow_size, width, arrow_size).contains(pn)))
		rdr = true;
	if(change_if_diff(vbar_ov, get_bar().contains(pn)))
			rdr = true;	
	if(vbar_dw)
	{
		float oldvalue = value;
		float track = get_size().height-3*arrow_size;
		float thumb = track;
		if(vmax-vmin != 0 && large_step != 0)
			thumb = (large_step*track)/(vmax-vmin);
		thumb = max(min(thumb, track), 5.f);
		float pixel_range = track - thumb;
		float thumb_pos = pn.y - fix;
		float perc = 0.f;
		if(pixel_range != 0)
			perc = (thumb_pos)/pixel_range;
		value = perc*(vmax-vmin) + vmin;
		value = max(min(value, vmax), vmin);
		if(value != oldvalue) {scrolling(value); value_changed(value);rdr = true;}
	}
	if(rdr && owner) 
		owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_move(m, p);
}

void vscroll::on_mouse_down(const mouse_buttons& b, const int m, const point& p)
{
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	if(b == mouse_buttons::left)
	{
		if(change_if_diff(vtop_arr_dw, vtop_arr_ov))
		{
			value -= large_step;
			if(value < vmin) value = vmin;
			scrolling(value);
			value_changed(value);
			if(owner)
				owner->redraw(get_bounds());
		}
		if(change_if_diff(vbottom_arr_dw, vbottom_arr_ov))
		{
			value += large_step;
			if(value > vmax) value = vmax;
			scrolling(value);
			value_changed(value);
			if(owner)
				owner->redraw(get_bounds());
		}
		if(change_if_diff(vbar_dw, vbar_ov))
		{
			fix = pn.y-(get_bar().get_y()-get_position().y-1.5f*arrow_size);
		}
	}
	dynamic_drawsurface::on_mouse_down(b, m, p);
}

void vscroll::on_mouse_up(const mouse_buttons& b, const int m, const point& p)
{
	if(change_if_diff(vtop_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(vbottom_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(vbar_dw, false))
		if(owner)owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_up(b, m, p);
}

void vscroll::this_mouse_wheel(const int m, const point& p, int delta)
{
	if(!is_available())
		return;
	float oldv = value;
	value -= (delta/WHEEL_DELTA)*large_step;
	value = min(max(vmin, value), vmax);
	if(oldv != value)
	{
		value_changed(value);
		scrolling(value);
		if(owner)
			owner->redraw(get_bounds());
	}
}

void vscroll::on_mouse_leave(const point& p)
{
	vtop_arr_dw = vbottom_arr_dw = false;
	vtop_arr_ov = vbottom_arr_ov = false;
	vbar_ov = vbar_dw = false;
	if(owner)
		owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_leave(p);
}

void vscroll::this_keypress(const virtual_keys& key, const key_extended_params& params)
{
	float old_val = value;
	switch(key)
	{
	case virtual_keys::down:
		value = max(min(vmax, value + small_step*params.repeat_count), vmin);
		break;
	case virtual_keys::end:
		value = vmax;
		break;
	case virtual_keys::home:
		value = vmin;
		break;
	case virtual_keys::next:
		value = max(min(vmax, value + large_step*params.repeat_count), vmin);
		break;
	case virtual_keys::prior:
		value = max(min(vmax, value - large_step*params.repeat_count), vmin);
		break;
	case virtual_keys::up:
		value = max(min(vmax, value - small_step*params.repeat_count), vmin);
		break;
	}
	if(old_val != value)
	{
		scrolling(value);
		value_changed(value);
		if(owner)
			owner->redraw(get_bounds());
	}
}
// VScroll

// CheckButton
check_button::check_button()
{
	set_captures_keyboard(true);
	set_captures_mouse_wheel(false);
	box_width = 15.f;
	cl_border = colour::gray;
	cl_fborder = colour::gcl_border;
	cl_font = colour::white;
	cl_hot = colour(60, colour::gcl_hot_gray);
	cl_state = colour::white;
	txt_algn = text_align::after_button;
	state = check_state::none;
	is_three_state = true;
	set_min_size(size(20, 20));
	msz = size(100, 20);
	set_title(L"Checkbutton");
	mouse_click += make_func_ptr(this, &check_button::this_clicked);
	key_up += make_func_ptr(this, &check_button::this_key_up);
}

check_button::~check_button()
{
	mouse_click -= make_func_ptr(this, &check_button::this_clicked);
	key_up -= make_func_ptr(this, &check_button::this_key_up);
}

void check_button::set_size(const size& sz, bool redraw)
{
	dynamic_drawsurface::set_size(msz, redraw);
}

void check_button::set_title(const wstring& txt)
{
	relayout = true;
	dynamic_drawsurface::set_title(txt);
}

void check_button::this_clicked(const mouse_buttons& mb, int mod, const point& p)
{
	if(is_three_state)
	{
		if(state == check_state::checked)
			state = check_state::intermediate;
		else if(state == check_state::intermediate)
			state = check_state::none;
		else if(state == check_state::none)
			state = check_state::checked;
	}
	else
	{
		if(state == check_state::checked)
			state = check_state::none;
		else if(state == check_state::none)
			state = check_state::checked;
	}
	check_state_changed(state);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::this_key_up(const virtual_keys& key, const key_extended_params& params)
{
	if(key != virtual_keys::space)
		return;
	if(is_three_state)
	{
		if(state == check_state::checked)
			state = check_state::intermediate;
		else if(state == check_state::intermediate)
			state = check_state::none;
		else if(state == check_state::none)
			state = check_state::checked;
	}
	else
	{
		if(state == check_state::checked)
			state = check_state::none;
		else if(state == check_state::none)
			state = check_state::checked;
	}
	check_state_changed(state);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::render(graphics* g)
{
	float border_width = 1.5f;
	float arrow_size = 5.f;
	if(!has_resources())
		init_resources(g);
	if(get_focus() && get_enabled())
		br_border->set_colour(cl_fborder);
	else if(!get_focus() || !get_enabled())
		br_border->set_colour(cl_border);
	pn_border->update();
	g->push_clip(rect(get_position(), get_size()));
	rect metric = get_font()->get_metrics(get_title(), size::max_size(), g);
	if(txt_algn == text_align::after_button)
	{
		g->draw_rect(rect(get_position().x + border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width), pn_border.get());
		if(state == check_state::intermediate)
		{
			g->fill_rect(rect(get_position().x + border_width/2.f+2, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f+2, box_width - border_width-4, box_width - border_width-4), br_state.get());
		}
		else if(state == check_state::checked)
		{
			point origin(get_position().x, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f);
			auto arr = get_hook(origin.x, origin.y);
			g->fill_polygon(arr, 6, br_state.get());
			delete[] arr;
		}
		g->draw_string(get_title(), point(get_position().x + box_width*1.5f, get_position().y + get_size().height/2.f-metric.get_height()/2.f), get_font(), br_font.get());
		if(get_drawing_state() == drawing_state::hot && get_enabled())
			g->fill_rect(rect(get_position().x + border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width), br_hot.get());
		if(!get_enabled())
		{
			g->fill_rect(rect(get_position().x + border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width), br_disabled.get());
			g->draw_string(get_title(), point(get_position().x + box_width*1.5f, get_position().y + get_size().height/2.f-metric.get_height()/2.f), get_font(), br_disabled.get());
		}
	}
	else
	{
		g->draw_rect(rect(get_position().x +get_size().width - box_width - border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width), pn_border.get());
		if(state == check_state::intermediate)
		{
			g->fill_rect(rect(get_position().x +get_size().width - box_width - border_width/2.f+2, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f+2, box_width - border_width-4, box_width - border_width-4), br_state.get());
		}
		else if(state == check_state::checked)
		{
			point origin(get_position().x + get_size().width - box_width - border_width/2.f-1.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f);
			auto arr = get_hook(origin.x, origin.y);
			g->fill_polygon(arr, 6, br_state.get());
			delete[] arr;
		}
		g->draw_string(get_title(), point(get_position().x + get_size().width - box_width*1.5f - metric.get_width(), get_position().y + get_size().height/2.f-metric.get_height()/2.f), get_font(), br_font.get());
		if(get_drawing_state() == drawing_state::hot && get_enabled())
			g->fill_rect(rect(get_position().x +get_size().width - box_width - border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width), br_hot.get());
	}
	g->pop_clip();
	msz.width = metric.get_width()+1.5f*box_width +5.f;
	if(relayout)
	{
		owner->layout();
		relayout = false;
	}
	dynamic_drawsurface::render(g);
}

void check_button::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(colour(60, colour::highlight)));
		br_state = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
	}
	else
	{
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_font));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(cl_hot));
		br_state = shared_ptr<solid_brush>(g->create_solid_brush(cl_state));
	}

	br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
	pn_border = shared_ptr<pen>(g->create_pen(br_border.get(), 1.5f));
	br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(70, colour::black)));
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_border->set_opacity(f);
		br_font->set_opacity(f);
		br_hot->set_opacity(f);
		br_disabled->set_opacity(f);
		br_state->set_opacity(f);
		pn_border->update();
	}
	dynamic_drawsurface::create_resources(g);
}

point* check_button::get_hook(float x, float y)
{
	float wcheck = 2.f;
	point* hook = new point[6];
	hook[0] = point(3+x, 6+y);
	hook[1] = point(3+x, 6+y+wcheck);
	hook[2] = point(6+x, 10+y);
	hook[3] = point(12+x, 4+y);
	hook[4] = point(12+x, 4+y-wcheck);
	hook[5] = point(6+x, 10+y-wcheck);
	return hook;
}

void check_button::set_three_state(bool b)
{
	if(!change_if_diff(is_three_state, b))
		return;
	if(b == false && state == check_state::intermediate)
	{
		state = check_state::none;
		check_state_changed(state);
	}
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_check_state(const check_state& st)
{
	if(!change_if_diff(state, st))
		return;
	check_state_changed(st);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_checked(bool checked)
{
	if((state == check_state::checked && checked) || (state == check_state::none && !checked))
		return;
	if(checked)
		state = check_state::checked;
	else
		state = check_state::none;
	check_state_changed(state);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_text_align(const text_align& align)
{
	if(!change_if_diff(txt_algn, align))
		return;
	text_align_changed(align);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hot, c))
		return;
	if(br_hot)
		br_hot->set_colour(c);
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_font_colour(const colour& c)
{
	if(!change_if_diff(cl_font, c))
		return;
	if(br_font)
		br_font->set_colour(c);
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_state_colour(const colour& c)
{
	if(!change_if_diff(cl_state, c))
		return;
	if(br_state)
		br_state->set_colour(c);
	state_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_border_colour(const colour& c)
{
	if(!change_if_diff(cl_border, c))
		return;
	border_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_border_focused_colour(const colour& c)
{
	if(!change_if_diff(cl_fborder, c))
		return;
	border_focused_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void check_button::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_hot->set_opacity(f);
		br_font->set_opacity(f);
		br_state->set_opacity(f);
		br_border->set_opacity(f);
		br_disabled->set_opacity(f);
		pn_border->update();
	}
	dynamic_drawsurface::set_opacity(f);
}

void check_button::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_font->set_colour(colour::gray_text);
		br_hot->set_colour(colour(60, colour::highlight));
		br_state->set_colour(colour::gray_text);
	}
	else
	{
		br_font->set_colour(cl_font);
		br_hot->set_colour(cl_hot);
		br_state->set_colour(cl_state);
	}
	state_colour_changed(br_state->get_colour());
	font_colour_changed(br_font->get_colour());
	hot_colour_changed(br_hot->get_colour());
	dynamic_drawsurface::on_syscolour_changed();
}
// CheckButton

// RadioButton
radio_button::radio_button()
{
	set_captures_keyboard(true);
	set_captures_mouse_wheel(false);
	box_width = 15.f;
	cl_border = colour::gray;
	cl_fborder = colour::gcl_border;
	cl_font = colour::white;
	cl_hot = colour(60, colour::gcl_hot_gray);
	txt_algn = text_align::after_button;
	set_min_size(size(20, 20));
	msz = size(100, 20);
	set_title(L"Radiobutton");
	mouse_click += make_func_ptr(this, &radio_button::this_clicked);
	key_up += make_func_ptr(this, &radio_button::this_key_up);
}

radio_button::~radio_button()
{
	mouse_click -= make_func_ptr(this, &radio_button::this_clicked);
	key_up -= make_func_ptr(this, &radio_button::this_key_up);
}

void radio_button::set_size(const size& sz, bool redraw)
{
	dynamic_drawsurface::set_size(msz, redraw);
}

void radio_button::set_title(const wstring& txt)
{
	relayout = true;
	dynamic_drawsurface::set_title(txt);
}

void radio_button::this_clicked(const mouse_buttons& mb, int mod, const point& p)
{
	if(checked)
		return;
	uncheck_buttons();
	checked = true;
	checked_changed(checked);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::this_key_up(const virtual_keys& key, const key_extended_params& params)
{
	if(key != virtual_keys::space || checked)
		return;
	uncheck_buttons();
	checked = true;
	checked_changed(checked);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::render(graphics* g)
{
	float border_width = 1.5f;
	float arrow_size = 5.f;
	if(!has_resources())
		init_resources(g);
	if(get_focus() && get_enabled())
		br_border->set_colour(cl_fborder);
	else if(!get_focus() || !get_enabled())
		br_border->set_colour(cl_border);
	pn_border->update();
	g->push_clip(rect(get_position(), get_size()));
	rect metric = get_font()->get_metrics(get_title(), size::max_size(), g);
	if(txt_algn == text_align::after_button)
	{
		g->draw_ellipse(ellipse(rect(get_position().x + border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width)), pn_border.get());
		if(checked)
		{
			g->fill_ellipse(ellipse(rect(get_position().x + border_width/2.f+2, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f+2, box_width - border_width-4, box_width - border_width-4)), br_font.get());
		}
		g->draw_string(get_title(), point(get_position().x + box_width*1.5f, get_position().y + get_size().height/2.f-metric.get_height()/2.f-border_width), get_font(), br_font.get());
		if(get_drawing_state() == drawing_state::hot && get_enabled())
			g->fill_ellipse(ellipse(rect(get_position().x + border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width)), br_hot.get());
		if(!get_enabled())
		{
			g->fill_ellipse((rect(get_position().x + border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width)), br_disabled.get());
			g->draw_string(get_title(), point(get_position().x + box_width*1.5f, get_position().y + get_size().height/2.f-metric.get_height()/2.f-border_width), get_font(), br_disabled.get());
		}
	}
	else
	{
		g->draw_ellipse(ellipse(rect(get_position().x +get_size().width - box_width - border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width)), pn_border.get());
		if(checked)
		{
			g->fill_ellipse(ellipse(rect(get_position().x +get_size().width - box_width - border_width/2.f+2, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f+2, box_width - border_width-4, box_width - border_width-4)), br_font.get());
		}
		g->draw_string(get_title(), point(get_position().x + get_size().width - box_width*1.5f - metric.get_width(), get_position().y + get_size().height/2.f-metric.get_height()/2.f-border_width), get_font(), br_font.get());
		if(get_drawing_state() == drawing_state::hot && get_enabled())
			g->fill_ellipse(ellipse(rect(get_position().x +get_size().width - box_width - border_width/2.f, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f, box_width - border_width, box_width - border_width)), br_hot.get());
		if(!get_enabled())
		{
			g->fill_ellipse(ellipse(rect(get_position().x +get_size().width - box_width - border_width/2.f+2, get_position().y + get_size().height/2.f - box_width/2.f + border_width/2.f+2, box_width - border_width-4, box_width - border_width-4)), br_disabled.get());
			g->draw_string(get_title(), point(get_position().x + get_size().width - box_width*1.5f - metric.get_width(), get_position().y + get_size().height/2.f-metric.get_height()/2.f-border_width), get_font(), br_disabled.get());
		}
	}
	g->pop_clip();
	msz.width = metric.get_width()+1.5f*box_width +5.f;
	if(relayout)
	{
		owner->layout();
		relayout = false;
	}
	dynamic_drawsurface::render(g);
}

void radio_button::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(colour(60, colour::highlight)));
	}
	else
	{
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_font));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(cl_hot));
	}

	br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
	pn_border = shared_ptr<pen>(g->create_pen(br_border.get(), 1.5f));
	br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(70, colour::black)));
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_border->set_opacity(f);
		br_font->set_opacity(f);
		br_hot->set_opacity(f);
		br_disabled->set_opacity(f);
		pn_border->update();
	}
	dynamic_drawsurface::create_resources(g);
}

void radio_button::set_checked(bool _checked)
{
	if(checked == _checked)
		return;
	if(_checked)
	{
		uncheck_buttons();
		internal_check(true);
	}
	else
		internal_check(false);
}

void radio_button::internal_check(bool val)
{
	if(!change_if_diff(checked, val))
		return;
	checked_changed(val);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::uncheck_buttons() const
{
	drawsurface* pnt = parent ? parent : owner;
	if(!pnt)
		return;
	for(auto itr = pnt->get_children_begin(); itr != pnt->get_children_end(); ++itr)
	{
		radio_button* rb = dynamic_cast<radio_button*>(*itr);
		if(rb)
		{
			if(rb->get_checked() && rb != this)
			{
				rb->internal_check(false);
			}
		}
	}
}

void radio_button::set_text_align(const text_align& align)
{
	if(!change_if_diff(txt_algn, align))
		return;
	text_align_changed(align);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hot, c))
		return;
	if(br_hot)
		br_hot->set_colour(c);
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::set_font_colour(const colour& c)
{
	if(!change_if_diff(cl_font, c))
		return;
	if(br_font)
		br_font->set_colour(c);
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::set_border_colour(const colour& c)
{
	if(!change_if_diff(cl_border, c))
		return;
	border_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::set_border_focused_colour(const colour& c)
{
	if(!change_if_diff(cl_fborder, c))
		return;
	border_focused_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void radio_button::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_hot->set_opacity(f);
		br_font->set_opacity(f);
		br_border->set_opacity(f);
		br_disabled->set_opacity(f);
		pn_border->update();
	}
	dynamic_drawsurface::set_opacity(f);
}

void radio_button::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_font->set_colour(colour::gray_text);
		br_hot->set_colour(colour(60, colour::highlight));
	}
	else
	{
		br_font->set_colour(cl_font);
		br_hot->set_colour(cl_hot);
	}
	font_colour_changed(br_font->get_colour());
	hot_colour_changed(br_hot->get_colour());
	dynamic_drawsurface::on_syscolour_changed();
}
// RadioButton

// StaticText
static_text::static_text()
{
	set_captures_keyboard(false);
	set_tab_stop(true);
	set_captures_mouse_wheel(false);
	cl_font = colour::white;
	set_min_size(size(10, 10));
	set_title(L"Statictext");
}

void static_text::render(graphics* g)
{
	if(!has_resources())
		init_resources(g);
	auto txtmode = g->get_text_rendering_mode();
	g->set_text_rendering_mode(text_rendering_modes::antialias);
	g->push_clip(rect(get_position(), get_size()));
	g->draw_string(get_title(), get_position(), get_font(), br_font.get());
	rect metric = get_font()->get_metrics(get_title(), size::max_size(), g);
	set_min_size(size(metric.get_width()+2, metric.get_height()+2));
	g->pop_clip();
	g->set_text_rendering_mode(txtmode);
	dynamic_drawsurface::render(g);
}

void static_text::set_font_colour(const colour& c)
{
	if(!change_if_diff(cl_font, c))
		return;
	if(br_font)
		br_font->set_colour(c);
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void static_text::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
	else
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_font));
	if(get_opacity() != 1.f)
		br_font->set_opacity(get_opacity());
	dynamic_drawsurface::create_resources(g);
}

void static_text::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
		br_font->set_opacity(f);
	dynamic_drawsurface::set_opacity(f);
}

void static_text::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
		br_font->set_colour(colour::gray_text);
	else
		br_font->set_colour(cl_font);
	font_colour_changed(br_font->get_colour());
	dynamic_drawsurface::on_syscolour_changed();
}
// StaticText

// LinkText
link_text::link_text()
{
	set_captures_keyboard(false);
	set_tab_stop(true);
	set_captures_mouse_wheel(false);
	cl_normal = colour(54, 110, 251);
	cl_over = colour(18, 151, 251);
	cl_visited = colour(176, 102, 251);
	set_min_size(size(10, 10));
	set_cursor(cursor_surface(system_cursor::hand));
	set_title(L"Linktext");
	mouse_click += make_func_ptr(this, &link_text::this_clicked);
}

link_text::~link_text()
{
	mouse_click -= make_func_ptr(this, &link_text::this_clicked);
}

void link_text::this_clicked(const mouse_buttons&, int, const point&)
{
	visited = true;
	if(owner)
		owner->redraw(get_bounds());
}

void link_text::render(graphics* g)
{
	if(!has_resources())
		init_resources(g);
	auto txtmode = g->get_text_rendering_mode();
	g->set_text_rendering_mode(text_rendering_modes::antialias);
	if(app::is_high_contrast_app())
	{
		if(visited)
			br_font->set_colour(colour::menu_bar);
		else if(get_drawing_state() == drawing_state::hot)
			br_font->set_colour(colour::menu_highlight);
		else
			br_font->set_colour(colour::menu);
	}
	else
	{
		if(visited)
			br_font->set_colour(cl_visited);
		else if(get_drawing_state() == drawing_state::hot)
			br_font->set_colour(cl_over);
		else
			br_font->set_colour(cl_normal);
	}
	g->push_clip(rect(get_position(), get_size()));
	g->draw_string(get_title(), get_position(), get_font(), br_font.get());
	rect metric = get_font()->get_metrics(get_title(), size::max_size(), g);
	set_min_size(size(metric.get_width()+2, metric.get_height()+2));
	g->pop_clip();
	g->set_text_rendering_mode(txtmode);
	dynamic_drawsurface::render(g);
}

void link_text::create_resources(graphics* g)
{
	br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_normal));
	if(get_opacity() != 1.f)
		br_font->set_opacity(get_opacity());
	dynamic_drawsurface::create_resources(g);
}

void link_text::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
		br_font->set_opacity(f);
	dynamic_drawsurface::set_opacity(f);
}

void link_text::set_font_colour(const colour& c)
{
	if(!change_if_diff(cl_normal, c))
		return;
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void link_text::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_over, c))
		return;
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void link_text::set_visited_colour(const colour& c)
{
	if(!change_if_diff(cl_visited, c))
		return;
	visited_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}
// StaticText

// ProgressBar
progress_bar::progress_bar()
{
	set_tab_stop(true);
	maximum = 100.f;
	minimum = 0.f;
	value = 0.f;
	front_colour = colour::gcl_border;
	back_colour = colour::gcl_dark_gray;
	state = progress_state::normal;
	set_min_size(size(20, 10));
}

void progress_bar::render(graphics* g)
{
	if(!has_resources())
		init_resources(g);
	g->push_clip(rect(get_position(), get_size()));
	g->fill_rect(rect(get_position(), get_size()), br_back.get());
	g->fill_rect(rect(get_position().x, get_position().y, get_size().width * value/maximum, get_size().height), br_front.get());
	g->pop_clip();
	dynamic_drawsurface::render(g);
}

void progress_bar::create_resources(graphics* g)
{
	br_back = shared_ptr<solid_brush>(g->create_solid_brush(back_colour));
	br_front = shared_ptr<solid_brush>(g->create_solid_brush(front_colour));
	if(get_opacity() != 1.f)
	{
		br_back->set_opacity(get_opacity());
		br_front->set_opacity(get_opacity());
	}
	dynamic_drawsurface::create_resources(g);
}

progress_bar& progress_bar::operator++()
{
	if(value + 1 <= maximum)
	{
		value++;
		if(owner)
			owner->redraw(get_bounds());
	}
	return (*this);
}

progress_bar& progress_bar::operator--()
{
	if(value - 1 >= minimum)
	{
		value--;
		if(owner)
			owner->redraw(get_bounds());
	}
	return (*this);
}

progress_bar& progress_bar::operator++(int)
{
	return ++(*this);
}

progress_bar& progress_bar::operator--(int)
{
	return --(*this);
}

void progress_bar::set_value(float val)
{
	if(is_between_equ(value, minimum, maximum))
		value = val;
	else
		throw invalid_argument("Out of range");
	if(owner)
		owner->redraw(get_bounds());
}

void progress_bar::set_minimum(float val)
{
	if(maximum <= val)
		throw invalid_argument("Out of range");
	if(value < val)
		value = val;
	minimum = val;
	redraw(get_bounds());
}

void progress_bar::set_maximum(float val)
{
	if(val <= minimum)
		throw invalid_argument("Out of range");
	if(value > val)
		value = val;
	maximum = val;
	redraw(get_bounds());
}

void progress_bar::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_back->set_opacity(f);
		br_front->set_opacity(f);
	}
	dynamic_drawsurface::set_opacity(f);
}

void progress_bar::set_progress_state(const progress_state& stte)
{
	if(!change_if_diff(state, stte))
		return;
	switch(state)
	{
	case progress_state::normal:
		br_front->set_colour(front_colour);
		break;
	case progress_state::cancelled:
		br_front->set_colour(colour(127, 0, 0));
		break;
	case progress_state::paused:
		br_front->set_colour(colour(185, 171, 0));
	}
	redraw(get_bounds());
}

void progress_bar::set_front_colour(colour c)
{
	if(!change_if_diff(front_colour, c))
		return;
	if(has_resources())
		br_front->set_colour(c);
	redraw(get_bounds());
}

void progress_bar::set_back_colour(colour c)
{
	if(!change_if_diff(back_colour, c))
		return;
	if(has_resources())
		br_back->set_colour(c);
	redraw(get_bounds());
}
// ProgressBar
};
};