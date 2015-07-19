#include "stdafx.h"
#include "layout.h"
namespace gcl { namespace ui { namespace layout {

// LayoutContainer
layout_container::layout_container() : arrow_size(10.f)
{
	vwidth = static_cast<float>(GetSystemMetrics(SM_CXVSCROLL));
	hheight = static_cast<float>(GetSystemMetrics(SM_CYHSCROLL));
	scrollable = true;
	halign = horizontal_scroll_align::bottom;
	valign = vertical_scroll_align::right;
	cl_bk = colour::gcl_gray;
	cl_fr = colour::gcl_front_gray;
	cl_hi = colour::gcl_hot_gray;
	cl_pr = colour::gcl_highlight_gray;
	large_step = 5.f;
	small_step = 1.f;
	vlarge_step = 5.f;
	vsmall_step = 1.f;
	set_captures_keyboard(true);
	set_captures_mouse_wheel(true);
	size_changed += make_func_ptr(this, &layout_container::this_size_changed);
	mouse_wheel += make_func_ptr(this, &layout_container::this_mouse_wheel);
	mouse_h_wheel += make_func_ptr(this, &layout_container::this_mouse_h_wheel);
	key_down += make_func_ptr(this, &layout_container::this_keypress);
}

layout_container::~layout_container()
{
	size_changed -= make_func_ptr(this, &layout_container::this_size_changed);
	mouse_wheel -= make_func_ptr(this, &layout_container::this_mouse_wheel);
	mouse_h_wheel -= make_func_ptr(this, &layout_container::this_mouse_h_wheel);
	key_down -= make_func_ptr(this, &layout_container::this_keypress);
}

void layout_container::create_resources(graphics* g)
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

void layout_container::set_opacity(float f)
{
	if(get_opacity() == f) return;
	if(has_resources())
	{
		br_bk->set_opacity(f);
		br_fr->set_opacity(f);
		br_hi->set_opacity(f);
		br_pr->set_opacity(f);
		if(owner)
			owner->redraw(get_bounds());
	}
	dynamic_drawsurface::set_opacity(f);
}

void layout_container::set_horizontal_scroll_align(const horizontal_scroll_align& align)
{
	if(!change_if_diff(halign, align)) return;
	layout();
	horizontal_scroll_align_changed(align);
}

void layout_container::set_vertical_scroll_align(const vertical_scroll_align& align)
{
	if(!change_if_diff(valign, align)) return;
	layout();
	vertical_scroll_align_changed(align);
}

void layout_container::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_bk->set_colour(colour::scrollbar);
		br_fr->set_colour(colour::window_text);
		br_hi->set_colour(colour::highlight);
		br_pr->set_colour(colour::hotlight);
		scroll_back_colour_changed(colour::scrollbar);
		scroll_front_colour_changed(colour::window_text);
		scroll_hot_colour_changed(colour::highlight);
		scroll_pressed_colour_changed(colour::hotlight);
	}
	else
	{
		br_bk->set_colour(cl_bk);
		br_fr->set_colour(cl_fr);
		br_hi->set_colour(cl_hi);
		br_pr->set_colour(cl_pr);
	}
	dynamic_drawsurface::on_syscolour_changed();
	if(owner)owner->redraw(get_bounds());
}

void layout_container::set_scrollable(bool b)
{
	if(!change_if_diff(scrollable, b)) return;
	scrollable_changed(b);
	if(owner)owner->redraw(get_bounds());
}

void layout_container::set_vsmall_step(float f)
{
	if(!change_if_diff(vsmall_step, f))
		return;
	vsmall_step_changed(f);
	calc_offsets();
}

void layout_container::set_hsmall_step(float f)
{
	if(!change_if_diff(small_step, f))
		return;
	hsmall_step_changed(f);
	calc_offsets();
}

void layout_container::set_vlarge_step(float f)
{
	if(!change_if_diff(vlarge_step, f))
		return;
	vlarge_step_changed(f);
	calc_offsets();
}

void layout_container::set_hlarge_step(float f)
{
	if(!change_if_diff(large_step, f))
		return;
	hlarge_step_changed(f);
	calc_offsets();
}

void layout_container::set_scroll_back_colour(const colour& c)
{
	if(!change_if_diff(cl_bk, c)) return;
	if(br_bk)
		br_bk->set_colour(c);
	if(owner)
		owner->redraw(get_bounds());
	scroll_back_colour_changed(c);
}

void layout_container::set_scroll_front_colour(const colour& c)
{
	if(!change_if_diff(cl_fr, c)) return;
	if(br_fr)
		br_fr->set_colour(c);
	if(owner)
		owner->redraw(get_bounds());
	scroll_front_colour_changed(c);
}

void layout_container::set_scroll_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hi, c)) return;
	if(br_hi)
		br_hi->set_colour(c);
	if(owner)
		owner->redraw(get_bounds());
	scroll_hot_colour_changed(c);
}

void layout_container::set_scroll_pressed_colour(const colour& c)
{
	if(!change_if_diff(cl_pr, c)) return;
	if(br_pr)
		br_pr->set_colour(c);
	if(owner)
		owner->redraw(get_bounds());
	scroll_pressed_colour_changed(c);
}

void layout_container::render(graphics* g)
{
	init_resources(g);
	if(surfaces.size() == 0)
		return;

	if(get_clip_childs())
		g->push_clip(clip(get_rect()));
	
	for(auto& surf : surfaces)
	{
		if(surf->get_visible())
		{
			if(surf->is_transformed())
				g->set_transform(surf->get_absolute_transform());
			else
				g->set_transform(get_absolute_transform());
			surf->render(g);
		}
	}
	if(visible_scr && get_visible() && scrollable)
	{
		g->set_transform(get_absolute_transform());
		draw_hscroll(g);
		draw_vscroll(g);
	}
	if(get_clip_childs())
		g->pop_clip();
}

rect layout_container::get_hbar()
{
	rect rc_f(valign == vertical_scroll_align::left ? get_position().x +  vwidth : get_position().x, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, max(get_size().width - vwidth, 3*arrow_size+10.f), hheight);
	float track = rc_f.get_width()-3*arrow_size;
	float thumb = track;
	if(hmax-hmin != 0 && large_step != 0)
		thumb = (large_step*track)/(hmax-hmin);
	thumb = max(min(thumb, track), 5.f);
	float pixel_range = track - thumb;
	float range = hmax-hmin;
	float perc = 0;
	if(range != 0)
		perc = (hvalue-hmin) / range;
	float pos = perc*pixel_range + arrow_size*1.5f+rc_f.get_left();
	pos = min(max(rc_f.get_left()+1.5f*arrow_size, pos), rc_f.get_right()-1.5f*arrow_size-thumb);
	return rect(pos, rc_f.get_y()+hheight/5.f, thumb, hheight-hheight/2.5f);
}

rect layout_container::get_vbar()
{
	rect rc_c(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::bottom ? get_position().y : get_position().y + hheight, vwidth, max(get_size().height - hheight, 3*arrow_size+10.f));
	float track = rc_c.get_height()-3*arrow_size;
	float thumb = track;
	if(vmax-vmin != 0 && large_step != 0)
		thumb = (large_step*track)/(vmax-vmin);
	thumb = max(min(thumb, track), 5.f);
	float pixel_range = track - thumb;
	float range = vmax-vmin;
	float perc = 0;
	if(range != 0)
		perc = (vvalue-vmin) / range;
	float pos = perc*pixel_range + arrow_size*1.5f+rc_c.get_top();
	pos = min(max(rc_c.get_top()+1.5f*arrow_size, pos), rc_c.get_bottom()-1.5f*arrow_size-thumb);
	return rect(rc_c.get_x()+vwidth/5.f, pos, vwidth-vwidth/2.5f, thumb);
}

void layout_container::this_mouse_wheel(const int m, const point& p, int delta)
{
	if(visible_scr && is_available() && scrollable)
	{
		float oldv = vvalue;
		vvalue -= (delta/WHEEL_DELTA)*large_step;
		vvalue = min(max(vmin, vvalue), vmax);
		if(vvalue != oldv)
		{
			vscrolling(vvalue);
			layout();
		}
	}
}

void layout_container::this_mouse_h_wheel(const int m, const point& p, int delta)
{
	if(visible_scr && is_available() && scrollable)
	{
		float oldh = hvalue;
		hvalue -= (delta/WHEEL_DELTA)*large_step;
		hvalue = min(max(hmin, hvalue), hmax);
		if(hvalue != oldh)
		{
			hscrolling(hvalue);
			layout();
		}
	}
}

void layout_container::this_keypress(const virtual_keys& key, const key_extended_params& params)
{
	if(!visible_scr || ! is_available() || !scrollable)
		return;
	switch(key)
	{
	case virtual_keys::down:
		vvalue = max(min(vmax, vvalue + vsmall_step*params.repeat_count), vmin);
		layout();
		break;
	case virtual_keys::end:
		vvalue = vmax;
		layout();
		break;
	case virtual_keys::home:
		vvalue = vmin;
		layout();
		break;
	case virtual_keys::left:
		hvalue = max(min(hmax, hvalue - small_step*params.repeat_count), hmin);
		layout();
		break;
	case virtual_keys::next:
		vvalue = max(min(vmax, vvalue + vlarge_step*params.repeat_count), vmin);
		layout();
		break;
	case virtual_keys::prior:
		vvalue = max(min(vmax, vvalue - vlarge_step*params.repeat_count), vmin);
		layout();
		break;
	case virtual_keys::right:
		hvalue = max(min(hmax, hvalue + small_step*params.repeat_count), hmin);
		layout();
		break;
	case virtual_keys::up:
		vvalue = max(min(vmax, vvalue - vsmall_step*params.repeat_count), vmin);
		layout();
		break;
	}
}

void layout_container::calc_offsets()
{
	if(!scrollable || !get_visible())
		return;
	float lft = 0, rght = 0;
	float bottom = 0, top = 0;
	for(auto& surf: surfaces)
	{
		if(!surf->get_visible())
			continue;
		rect rc = surf->get_bounds();
		lft = min(lft, -get_rect().get_left()+rc.get_left()+get_hscroll_value());
		rght = max(rght, rc.get_right()-get_rect().get_right()+get_hscroll_value());
		bottom = max(bottom, rc.get_bottom()-get_rect().get_bottom()+get_vscroll_value());
		top = min(top, rc.get_top()-get_rect().get_top()+get_vscroll_value());
	}
	float old_h_range = hmax-hmin;
	float old_v_range = vmax-vmin;
	hmax = rght/10.f;
	hmin = lft/10.f;
	vmax = bottom/10.f;
	vmin = top/10.f;
	bool b = visible_scr;
	visible_scr = hmax != 0 || hmin != 0 || vmax != 0 || vmin != 0;
	if(visible_scr)
	{
		if(valign == vertical_scroll_align::right)
			hmax += vwidth/10.f;
		else
			hmin -= vwidth/10.f;
		if(halign == horizontal_scroll_align::bottom)
			vmax += hheight/10.f;
		else
			vmin -= hheight/10.f;
	}
	if(old_h_range != 0)
		hvalue = min(max(hvalue/old_h_range*(hmax-hmin), hmin), hmax);
	else
		hvalue = 0;
	if(old_v_range != 0)
		vvalue = min(max(vvalue/old_v_range*(vmax-vmin), vmin), vmax);
	else
		vvalue = 0;
	if(b != visible_scr)
		vvalue = hvalue = 0;
	if(old_h_range != hmax-hmin)
		hscroll_offs_changed(hmin*10.f, hvalue*10.f, hmax*10.f);
	if(old_v_range != vmax-vmin)
		vscroll_offs_changed(vmin*10.f, vvalue*10.f, vmax*10.f);
}

void layout_container::this_size_changed(const size& s, const resizing_types& rt)
{
	calc_offsets();
}

void layout_container::draw_hscroll(graphics* g)
{
	rect rc_f(valign == vertical_scroll_align::left ? get_position().x +  vwidth : get_position().x, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, max(get_size().width - vwidth, 3*arrow_size+10.f), hheight);
	point arr1[] = {point(rc_f.get_x()+arrow_size/5.f, rc_f.get_y()+hheight/2.f),
					point(rc_f.get_x()+arrow_size-arrow_size/5.f, rc_f.get_y()+hheight/5.f),
					point(rc_f.get_x()+arrow_size-arrow_size/5.f, rc_f.get_y()+hheight-hheight/5.f)};
	point arr2[] = {point(rc_f.get_right() - arrow_size/5.f, rc_f.get_y()+hheight/2.f),
					point(rc_f.get_right() - arrow_size + arrow_size/5.f, rc_f.get_y()+hheight/5.f),
					point(rc_f.get_right() - arrow_size + arrow_size/5.f, rc_f.get_y()+hheight-hheight/5.f)};
	g->fill_rect(rect(get_position().x, rc_f.get_y(), max(get_size().width, 3*arrow_size+10.f), hheight), br_bk.get());

	if(hbar_ov && !hbar_dw)
		g->fill_rect(get_hbar(), br_hi.get());
	else if(hbar_dw)
		g->fill_rect(get_hbar(), br_pr.get());
	else
		g->fill_rect(get_hbar(), br_fr.get());
	
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
}

void layout_container::draw_vscroll(graphics* g)
{
	rect rc_c(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::bottom ? get_position().y : get_position().y + hheight, vwidth, max(get_size().height - hheight, 3*arrow_size+10.f));
	point arr1[] = {point(rc_c.get_left() + vwidth/2.f, rc_c.get_top()+arrow_size/5.f), 
					point(rc_c.get_left()+vwidth-vwidth/5.f, rc_c.get_top()+arrow_size-arrow_size/5.f),
					point(rc_c.get_left()+vwidth/5.f, rc_c.get_top()+arrow_size-arrow_size/5.f)};
	point arr2[] = {point(rc_c.get_left() + vwidth/2.f, rc_c.get_bottom()-arrow_size/5.f), 
					point(rc_c.get_left()+vwidth-vwidth/5.f, rc_c.get_bottom()-arrow_size+arrow_size/5.f),
					point(rc_c.get_left()+vwidth/5.f, rc_c.get_bottom()-arrow_size+arrow_size/5.f)};
	g->fill_rect(rect(rc_c.get_x(), get_position().y, vwidth, max(get_size().height, 3*arrow_size+10.f)), br_bk.get());

	if(vbar_ov && !vbar_dw)
		g->fill_rect(get_vbar(), br_hi.get());
	else if(vbar_dw)
		g->fill_rect(get_vbar(), br_pr.get());
	else
		g->fill_rect(get_vbar(), br_fr.get());
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
}

void layout_container::on_mouse_move(const int m, const point& p)
{
	rect rc_betw(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, vwidth, hheight);
	rect rc_f(valign == vertical_scroll_align::left ? get_position().x +  vwidth : get_position().x, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, max(get_size().width - vwidth, 3*arrow_size+10.f), hheight);
	rect rc_c(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::bottom ? get_position().y : get_position().y + hheight, vwidth, max(get_size().height - hheight, 3*arrow_size+10.f));
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	if(visible_scr && is_available() && scrollable)
	{
		bool rdr = false;
		if(change_if_diff(vtop_arr_ov, rect(rc_c.get_x(), rc_c.get_y(), vwidth, arrow_size).contains(pn)))
			rdr = true;
		if(change_if_diff(vbottom_arr_ov, rect(rc_c.get_x(), rc_c.get_bottom()-arrow_size, vwidth, arrow_size).contains(pn)))
			rdr = true;
		if(change_if_diff(vbar_ov, get_vbar().contains(pn)))
			rdr = true;		
		
		if(change_if_diff(hleft_arr_ov, rect(rc_f.get_x(), rc_f.get_y(), arrow_size, hheight).contains(pn)))
			rdr = true;
		if(change_if_diff(hright_arr_ov, rect(rc_f.get_right() - arrow_size, rc_f.get_y(), arrow_size, hheight).contains(pn)))
			rdr = true;
		if(change_if_diff(hbar_ov, get_hbar().contains(pn)))
			rdr = true;
		if((valign == vertical_scroll_align::left ? pn.x >= (rc_c.get_right()+horizontal_leave_offset) : (pn.x <= rc_c.get_left()-horizontal_leave_offset))) // If Cursor out of range, release capture
		{
			if(vbar_dw)
			{
				vbar_dw = false;
				rdr = true;
			}
		}
		if((halign == horizontal_scroll_align::top ? pn.y >= (rc_f.get_bottom()+vertical_leave_offset) : (pn.y <= rc_f.get_top()-vertical_leave_offset))) // If Cursor out of range, release capture
		{
			if(hbar_dw)
			{
				hbar_dw = false;
				rdr = true;
			}
		}
		if(vbar_dw)
		{
			float oldvalue = vvalue;
			float track = rc_c.get_height()-3*arrow_size;
			float thumb = track;
			if(vmax-vmin != 0 && vlarge_step != 0)
				thumb = (vlarge_step*track)/(vmax-vmin);
			thumb = max(min(thumb, track), 5.f);
			float pixel_range = track - thumb;
			float thumb_pos = pn.y - vfix;
			if(halign == horizontal_scroll_align::top)
				thumb_pos -= hheight;
			float perc = 0.f;
			if(pixel_range != 0)
				perc = (thumb_pos)/pixel_range;
			vvalue = perc*(vmax-vmin) + vmin;
			vvalue = max(min(vvalue, vmax), vmin);
			if(vvalue != oldvalue) {vscrolling(vvalue*10.f); layout();}
		}
		if(hbar_dw)
		{
			float oldvalue = hvalue;
			float track = rc_f.get_width()-3*arrow_size;
			float thumb = track;
			if(hmax-hmin != 0 && large_step != 0)
				thumb = (large_step*track)/(hmax-hmin);
			thumb = max(min(thumb, track), 5.f);
			float pixel_range = track - thumb;
			float thumb_pos = pn.x - hfix;
			if(valign == vertical_scroll_align::left)
				thumb_pos -= vwidth;
			float perc = 0.f;
			if(pixel_range != 0)
				perc = (thumb_pos)/pixel_range;
			hvalue = perc*(hmax-hmin) + hmin;
			hvalue = max(min(hvalue, hmax), hmin);
			if(hvalue != oldvalue) {hscrolling(hvalue*10.f); layout();}
		}
		if(rdr && owner) owner->redraw(get_bounds());
	}
	if(((!rc_f.contains(pn) && !rc_c.contains(pn) && !rc_betw.contains(pn) && !hbar_dw && !vbar_dw) || !visible_scr || !scrollable))
	{
		dynamic_drawsurface::on_mouse_move(m, p);
		return;
	}
}

void layout_container::on_mouse_dbl_click(const mouse_buttons& b, const int m, const point& p)
{
	rect rc_betw(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, vwidth, hheight);
	rect rc_f(valign == vertical_scroll_align::left ? get_position().x +  vwidth : get_position().x, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, max(get_size().width - vwidth, 3*arrow_size+10.f), hheight);
	rect rc_c(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::bottom ? get_position().y : get_position().y + hheight, vwidth, max(get_size().height - hheight, 3*arrow_size+10.f));
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	if((!rc_f.contains(pn) && !rc_c.contains(pn) && !rc_betw.contains(pn))|| !visible_scr || !scrollable)
	{
		dynamic_drawsurface::on_mouse_dbl_click(b, m, p);
		return;
	}
	else if(rc_betw.contains(pn))
	{
		auto s = get_focused_surface();
		if(s)
			s->set_focus(false);
		set_focus(true);
	}
}

void layout_container::on_mouse_down(const mouse_buttons& b, const int m, const point& p)
{
	rect rc_betw(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, vwidth, hheight);
	rect rc_f(valign == vertical_scroll_align::left ? get_position().x +  vwidth : get_position().x, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, max(get_size().width - vwidth, 3*arrow_size+10.f), hheight);
	rect rc_c(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::bottom ? get_position().y : get_position().y + hheight, vwidth, max(get_size().height - hheight, 3*arrow_size+10.f));
	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	if(visible_scr && is_available() && scrollable && b == mouse_buttons::left)
	{
		if(change_if_diff(vtop_arr_dw, vtop_arr_ov))
		{
			vvalue -= vlarge_step;
			if(vvalue < vmin) vvalue = vmin;
			vscrolling(vvalue*10.f);
			layout();
		}
		if(change_if_diff(vbottom_arr_dw, vbottom_arr_ov))
		{
			vvalue += vlarge_step;
			if(vvalue > vmax) vvalue = vmax;
			vscrolling(vvalue*10.f);
			layout();
		}
		if(change_if_diff(vbar_dw, vbar_ov))
		{
			vfix = pn.y-(get_vbar().get_y()-get_position().y-1.5f*arrow_size);
		}

		if(change_if_diff(hleft_arr_dw, hleft_arr_ov))
		{
			hvalue -= large_step;
			if(hvalue < hmin) hvalue = hmin;
			hscrolling(hvalue*10.f);
			layout();
		}
		if(change_if_diff(hright_arr_dw, hright_arr_ov))
		{
			hvalue += large_step;
			if(hvalue > hmax) hvalue = hmax;
			hscrolling(hvalue*10.f);
			layout();
		}
		if(change_if_diff(hbar_dw, hbar_ov))
		{
			hfix = pn.x-(get_hbar().get_x()-get_position().x-1.5f*arrow_size);
		}
	}
	if((!rc_f.contains(pn) && !rc_c.contains(pn) && !rc_betw.contains(pn)) || !visible_scr || !scrollable)
		dynamic_drawsurface::on_mouse_down(b, m, p);
	else if(!get_focus())
		set_focus(true);
}

void layout_container::on_mouse_up(const mouse_buttons& b, const int m, const point& p)
{
	if(change_if_diff(vtop_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(vbottom_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(vbar_dw, false))
		if(owner)owner->redraw(get_bounds());

	if(change_if_diff(hleft_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(hright_arr_dw, false))
		if(owner)owner->redraw(get_bounds());
	if(change_if_diff(hbar_dw, false))
		if(owner)owner->redraw(get_bounds());
	dynamic_drawsurface::on_mouse_up(b, m, p);
}

void layout_container::on_mouse_enter(const point& p)
{
	rect rc_betw(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, vwidth, hheight);
	rect rc_f(valign == vertical_scroll_align::left ? get_position().x +  vwidth : get_position().x, halign == horizontal_scroll_align::top ? get_position().y : get_position().y + get_size().height - hheight, max(get_size().width - vwidth, 3*arrow_size+10.f), hheight);
	rect rc_c(valign == vertical_scroll_align::left ? get_position().x : get_position().x + get_size().width - vwidth, halign == horizontal_scroll_align::bottom ? get_position().y : get_position().y + hheight, vwidth, max(get_size().height - hheight, 3*arrow_size+10.f));

	point pn = p;
	auto ma = get_absolute_transform();
	ma.invert();
	ma.transform_points(&pn);
	if((!rc_f.contains(pn) && !rc_c.contains(pn) && !rc_betw.contains(pn)) || !visible_scr || !scrollable)
		dynamic_drawsurface::on_mouse_enter(p);
	is_mover = true;
}

void layout_container::on_mouse_leave(const point& p)
{
	hleft_arr_dw = hright_arr_dw = false;
	hleft_arr_ov = hright_arr_ov = false;
	hbar_ov = hbar_dw = false;
	vtop_arr_dw = vbar_dw = false;
	vtop_arr_ov = vbottom_arr_ov = false;
	vbar_ov = vbar_dw = false;
	dynamic_drawsurface::on_mouse_leave(p);
}
// LayoutContainer

// StackPanel
stack_panel::stack_panel() : orient(orientation::vertical), child_orient(child_orientation::top) {}

void stack_panel::layout()
{
	dynamic_drawsurface* last_surf = 0;
	if(has_resources() && !is_rectangular())
		update_shape(get_shape());
	if(child_orient == child_orientation::top || child_orient == child_orientation::left)
	{
		for(auto itr = surfaces.begin(); itr != surfaces.end(); itr++)
		{
			auto surf = *itr;
			margin m = surf->get_margin();
			float x = 0, y = 0;
			float width = surf->get_size().width, height = surf->get_size().height;
			if(orient == orientation::vertical)
			{
				switch(surf->get_horizontal_align())
				{
				case horizontal_align::left:
					x = pddng.left + m.left+get_position().x-get_hscroll_value();
					break;
				case horizontal_align::center:
					x = get_size().width / 2.f - surf->get_size().width / 2.f + pddng.left - pddng.right + m.left - m.right+get_position().x-get_hscroll_value();
					break;
				case horizontal_align::right:
					x = get_size().width - surf->get_size().width - pddng.right - m.right+get_position().x-get_hscroll_value();
					break;
				default: // stretch
					x = pddng.left + m.left+get_position().x-get_hscroll_value();
					width = get_size().width - pddng.right - m.right - pddng.left - m.left;
				}
			}
			else // horizontal
			{
				switch(surf->get_vertical_align())
				{
				case vertical_align::top:
					y = pddng.top + m.top+get_position().y-get_vscroll_value();
					break;
				case vertical_align::center:
					y = get_size().height / 2.f - surf->get_size().height / 2.f + pddng.top + m.top - m.bottom - pddng.bottom+get_position().y-get_vscroll_value();
					break;
				case vertical_align::bottom:
					y = get_size().height - surf->get_size().height - pddng.bottom - m.bottom+get_position().y-get_vscroll_value();
					break;
				default: // stretch
					y = pddng.top + m.top+get_position().y-get_vscroll_value();
					height = get_size().height - pddng.bottom - m.bottom - pddng.top - m.bottom;
				}
			}

			if(last_surf)
			{
				if(orient == orientation::vertical)
				{
					y = m.top + last_surf->get_position().y + last_surf->get_size().height + last_surf->get_margin().bottom;
				}
				else
				{
					x = m.left + last_surf->get_position().x + last_surf->get_size().width + last_surf->get_margin().right;
				}
			}
			else
			{
				if(orient == orientation::vertical)
				{
					y = pddng.top + m.top + get_position().y-get_vscroll_value();
				}
				else
				{
					x = pddng.left + m.left + get_position().x-get_hscroll_value();
				}
			}

			last_surf = surf;
			if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
			if(surf->get_auto_position()) {surf->set_position(point(x, y), false);}
			surf->layout();
		}
	}
	else
	{
		for(auto itr = surfaces.rbegin(); itr != surfaces.rend(); itr++)
		{
			auto surf = *itr;
			margin m = surf->get_margin();
			float x = 0, y = 0;
			float width = surf->get_size().width, height = surf->get_size().height;
			if(orient == orientation::vertical)
			{
				switch(surf->get_horizontal_align())
				{
				case horizontal_align::left:
					x = pddng.left + m.left+get_position().x-get_hscroll_value();
					break;
				case horizontal_align::center:
					x = get_size().width / 2.f - surf->get_size().width / 2.f + pddng.left - pddng.right + m.left - m.right+get_position().x-get_hscroll_value();
					break;
				case horizontal_align::right:
					x = get_size().width - surf->get_size().width - pddng.right - m.right+get_position().x-get_hscroll_value();
					break;
				default: // stretch
					x = pddng.left + m.left+get_position().x-get_hscroll_value();
					width = get_size().width - pddng.right - m.right - pddng.left - m.left;
				}
			}
			else // horizontal
			{
				switch(surf->get_vertical_align())
				{
				case vertical_align::top:
					y = pddng.top + m.top+get_position().y-get_vscroll_value();
					break;
				case vertical_align::center:
					y = get_size().height / 2.f - surf->get_size().height / 2.f + pddng.top + m.top - m.bottom - pddng.bottom+get_position().y-get_vscroll_value();
					break;
				case vertical_align::bottom:
					y = get_size().height - surf->get_size().height - pddng.bottom - m.bottom+get_position().y-get_vscroll_value();
					break;
				default: // stretch
					y = pddng.top + m.top+get_position().y-get_vscroll_value();
					height = (get_size().height - pddng.bottom - m.bottom - pddng.left - m.left);
				}
			}

			if(last_surf)
			{
				if(orient == orientation::vertical)
				{
					y = last_surf->get_position().y - surf->get_size().height - m.bottom - last_surf->get_margin().top;
				}
				else
				{
					x = last_surf->get_position().x - surf->get_size().width - m.right - last_surf->get_margin().left;
				}
			}
			else
			{
				if(orient == orientation::vertical)
				{
					y = get_size().height - surf->get_size().height - pddng.bottom - m.bottom+get_position().y-get_vscroll_value();
				}
				else
				{
					x = get_size().width - surf->get_size().width - pddng.right - m.right+get_position().x-get_hscroll_value();
				}
			}
			last_surf = surf;
			if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
			if(surf->get_auto_position()) {surf->set_position(point(x, y), false);}
			surf->layout();
		}
	}
	if(owner) owner->redraw(get_bounds());
	layouted();
}

void stack_panel::set_child_orient(const child_orientation& ort)
{
	if(ort == child_orient) return;
	if( ((orient == orientation::vertical) && (ort == child_orientation::left || ort == child_orientation::right)) || ((orient == orientation::horizontal) && (ort == child_orientation::top || ort == child_orientation::bottom)))
		throw invalid_argument("Invalid option");
	child_orient = ort;
	child_orient_changed(ort);
	layout();
}

void stack_panel::set_orient(const orientation& ort)
{
	if(!change_if_diff(orient, ort)) return;
	orient_changed(ort);
	layout();
}
// StackPanel

// WrapPanel
wrap_panel::wrap_panel()
{
	fl_dir = flow_direction::left_to_right;
	orient = child_orientation::top;
}

void wrap_panel::set_flow_direction(const flow_direction& fd)
{
	if(!change_if_diff(fl_dir, fd))
		return;
	if(fd == flow_direction::bottom_to_top || fd == flow_direction::top_to_bottom)
	{
		if(orient == child_orientation::bottom || orient == child_orientation::top)
			orient = child_orientation::left;
	}
	else
	{
		if(orient == child_orientation::left || orient == child_orientation::right)
			orient = child_orientation::top;
	}
	layout();
}

void wrap_panel::set_child_orient(const child_orientation& co)
{
	if(co == orient)
		return;
	if(fl_dir == flow_direction::bottom_to_top || fl_dir == flow_direction::top_to_bottom)
	{
		if(co == child_orientation::bottom || co == child_orientation::top)
			throw invalid_argument("Invalid orientation");
	}
	else
	{
		if(co == child_orientation::left || co == child_orientation::right)
			throw invalid_argument("Invalid orientation");
	}
	orient = co;
	layout();
}

void wrap_panel::layout()
{
	if(has_resources() && !is_rectangular())
		update_shape(get_shape());
	
	switch(fl_dir)
	{
	case flow_direction::left_to_right:
		layout_l_r(orient == child_orientation::bottom);
		break;
	case flow_direction::right_to_left:
		layout_r_l(orient == child_orientation::bottom);
		break;
	case flow_direction::top_to_bottom:
		layout_t_b(orient == child_orientation::right);
		break;
	case flow_direction::bottom_to_top:
		layout_b_t(orient == child_orientation::right);
	}

	if(owner) owner->redraw(get_bounds());
	layouted();
}

void wrap_panel::layout_t_b(bool right)
{
	float curr_width = right ? get_size().width-pddng.left-pddng.right : 0.f;
	float curr_height = 0.f;
	float max_width = 0.f;

	int cnt = 0;
	float r_width = get_row_width(surfaces.begin(), surfaces.end(), cnt);

	for(auto itr = surfaces.begin(); itr != surfaces.end(); ++itr)
	{
		auto surf = *itr;
		margin m = surf->get_margin();
		float width = surf->get_min_size().width, height = surf->get_min_size().height;
		float x = pddng.left, y = pddng.top;

		if(curr_height+height+m.top+m.bottom > get_size().height-pddng.top-pddng.bottom && itr != surfaces.begin())
		{
			curr_height = height+m.top+m.bottom;
			curr_width += right ? -max_width : max_width;
			max_width = width+m.left+m.right;
			if(right)
				x += curr_width+get_position().x-(surf->get_size().width+m.left+m.right);
			else
				x += curr_width+get_position().x;
			y += get_position().y;
			r_width = get_row_width(itr, surfaces.end(), cnt);
		}
		else
		{
			if(right)
				x += curr_width+get_position().x-(surf->get_size().width+m.left+m.right);
			else
				x += curr_width+get_position().x;
			y += curr_height+get_position().y;
			curr_height += height+m.top+m.bottom;
			max_width = max(max_width, width+m.left+m.right);
		}

		if(width+m.left+m.right != r_width && cnt > 1)
			position_surf(surf->get_margin(), surf->get_vertical_align(), surf->get_horizontal_align(), right, r_width, x, y, width, height);

		if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
		if(surf->get_auto_position()) {surf->set_position(point(x+m.left, y+m.top), false);}
		surf->layout();
	}
}

void wrap_panel::layout_b_t(bool right)
{
	float curr_width = right ? get_size().width-pddng.left-pddng.right : 0.f;
	float curr_height = get_size().height-pddng.top-pddng.bottom;
	float max_width = 0.f;

	int cnt = 0;
	float r_width = get_row_width(surfaces.begin(), surfaces.end(), cnt);

	for(auto itr = surfaces.begin(); itr != surfaces.end(); ++itr)
	{
		auto surf = *itr;
		margin m = surf->get_margin();
		float width = surf->get_min_size().width, height = surf->get_min_size().height;
		float x = pddng.left, y = pddng.top;

		if(curr_height-height-m.top-m.bottom < 0 && itr != surfaces.begin())
		{
			curr_height = get_size().height-pddng.top-pddng.bottom-height-m.top-m.bottom;
			curr_width += right ? -max_width : max_width;
			max_width = width+m.left+m.right;
			if(right)
				x += curr_width+get_position().x-(surf->get_size().width+m.left+m.right);
			else
				x += curr_width+get_position().x;
			y += get_position().y+get_size().height-pddng.top-pddng.bottom-height-m.top-m.bottom;
			r_width = get_row_width(itr, surfaces.end(), cnt);
		}
		else
		{
			if(right)
				x += curr_width+get_position().x-(surf->get_size().width+m.left+m.right);
			else
				x += curr_width+get_position().x;
			y = y+curr_height+get_position().y-m.top-m.bottom-height;
			curr_height -= height+m.top+m.bottom;
			max_width = max(max_width, width+m.left+m.right);
		}

		if(width+m.left+m.right != r_width && cnt > 1)
			position_surf(surf->get_margin(), surf->get_vertical_align(), surf->get_horizontal_align(), right, r_width, x, y, width, height);

		if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
		if(surf->get_auto_position()) {surf->set_position(point(x+m.left, y+m.top), false);}
		surf->layout();
	}
}

void wrap_panel::layout_l_r(bool bottom)
{
	float curr_width = 0.f;
	float curr_height = bottom ? get_size().height-pddng.top-pddng.bottom : 0.f;
	float max_height = 0.f;

	int cnt = 0;
	float r_height = get_row_height(surfaces.begin(), surfaces.end(), cnt);

	for(auto itr = surfaces.begin(); itr != surfaces.end(); ++itr)
	{
		auto surf = *itr;
		margin m = surf->get_margin();
		float width = surf->get_min_size().width, height = surf->get_min_size().height;
		float x = pddng.left, y = pddng.top;

		if(curr_width+width+m.left+m.right > get_size().width-pddng.left-pddng.right && itr != surfaces.begin())
		{
			curr_width = width+m.left+m.right;
			curr_height += bottom ? -max_height : max_height;
			max_height = height+m.top+m.bottom;
			x += get_position().x;
			if(bottom)
				y += curr_height+get_position().y-(surf->get_size().height+m.top+m.bottom);
			else
				y += curr_height+get_position().y;
			r_height = get_row_height(itr, surfaces.end(), cnt);
		}
		else
		{
			x += curr_width+get_position().x;
			if(bottom)
				y += curr_height+get_position().y-(surf->get_size().height+m.top+m.bottom);
			else
				y += curr_height+get_position().y;
			curr_width += width+m.left+m.right;
			max_height = max(max_height, height+m.top+m.bottom);
		}

		if(height+m.top+m.bottom != r_height && cnt > 1)
			position_surf(surf->get_margin(), surf->get_vertical_align(), surf->get_horizontal_align(), bottom, r_height, x, y, width, height);

		if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
		if(surf->get_auto_position()) {surf->set_position(point(x+m.left, y+m.top), false);}
		surf->layout();
	}
}

void wrap_panel::layout_r_l(bool bottom)
{
	float curr_width = get_size().width-pddng.left-pddng.right;
	float curr_height = bottom ? get_size().height-pddng.top-pddng.bottom : 0.f;
	float max_height = 0.f;

	int cnt = 0;
	float r_height = get_row_height(surfaces.begin(), surfaces.end(), cnt);

	for(auto itr = surfaces.begin(); itr != surfaces.end(); ++itr)
	{
		auto surf = *itr;
		margin m = surf->get_margin();
		float width = surf->get_min_size().width, height = surf->get_min_size().height;
		float x = pddng.left, y = pddng.top;

		if(curr_width-width-m.left-m.right < 0 && itr != surfaces.begin())
		{
			curr_width = get_size().width-pddng.left-pddng.right-width-m.left-m.right;
			curr_height += bottom ? -max_height : max_height;
			max_height = height+m.top+m.bottom;
			x = get_position().x+pddng.left+get_size().width-pddng.left-pddng.right-width-m.left-m.right;
			if(bottom)
				y += curr_height+get_position().y-(surf->get_size().height+m.top+m.bottom);
			else
				y += curr_height+get_position().y;
			r_height = get_row_height(itr, surfaces.end(), cnt);
		}
		else
		{
			x = x+curr_width+get_position().x-m.left-m.right-width;
			if(bottom)
				y += curr_height+get_position().y-(surf->get_size().height+m.top+m.bottom);
			else
				y += curr_height+get_position().y;
			curr_width -= width+m.left+m.right;
			max_height = max(max_height, height+m.top+m.bottom);
		}

		if(height+m.top+m.bottom != r_height && cnt > 1)
			position_surf(surf->get_margin(), surf->get_vertical_align(), surf->get_horizontal_align(), bottom, r_height, x, y, width, height);

		if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
		if(surf->get_auto_position()) {surf->set_position(point(x+m.left, y+m.top), false);}
		surf->layout();
	}
}

void wrap_panel::position_surf(const margin& m, const vertical_align& valign, const horizontal_align& halign, bool sec, float r_height, float& x, float& y, float& width, float& height)
{
	if(fl_dir == flow_direction::left_to_right || fl_dir == flow_direction::right_to_left)
	{
		switch(valign)
		{
		case vertical_align::top:
			if(sec)
				y = min(y, y-r_height+height);
			break;
		case vertical_align::bottom:
			if(!sec)
				y = max(y, y + r_height - height);
			break;
		case vertical_align::center:
			y = sec ? min(y, y - r_height/2.f + height/2.f) : max(y, y + r_height/2.f - height/2.f);
			break;
		case vertical_align::stretch:
			height = r_height-m.top-m.bottom;
		}
	}
	else
	{
		switch(halign)
		{
		case horizontal_align::left:
			if(sec)
				x = min(x, x-r_height+width);
			break;
		case horizontal_align::center:
			x = sec ? min(x, x - r_height/2.f + width/2.f) : max(x, x+r_height/2.f-width/2.f);
			break;
		case horizontal_align::right:
			if(!sec)
				x = max(x, x + r_height - width);
			break;
		case horizontal_align::stretch:
			width = r_height-m.right-m.left;
			break;
		}
	}
}

float wrap_panel::get_row_height(lst_itr bgn, lst_itr end, int& cnt)
{
	float max_height = 0.f;
	float width = 0.f;
	cnt = 0;
	while(bgn != end)
	{
		auto surf = *bgn;
		width += surf->get_min_size().width+surf->get_margin().left+surf->get_margin().right;
		if(width > get_size().width-pddng.left-pddng.right)
			break;
		max_height = max(max_height, surf->get_min_size().height+surf->get_margin().top+surf->get_margin().bottom);
		bgn++;
		cnt++;
	}
	return max_height;
}

float wrap_panel::get_row_width(lst_itr bgn, lst_itr end, int& cnt)
{
	float max_width = 0.f;
	float height = 0.f;
	cnt = 0;
	while(bgn != end)
	{
		auto surf = *bgn;
		height += surf->get_min_size().height+surf->get_margin().top+surf->get_margin().bottom;
		if(height > get_size().height-pddng.top-pddng.bottom)
			break;
		max_width = max(max_width, surf->get_min_size().width+surf->get_margin().left+surf->get_margin().right);
		cnt++;
		bgn++;
	}
	return max_width;
}
// WrapPanel

// DockPanel
void dock_panel::set_auto_min_size(bool b)
{
	if(b == auto_min)
		return;
	auto_min = b;
	if(!b)
		minsize = size(5, 5);
	get_absolute_owner()->layout();
}

void dock_panel::layout()
{
	if(has_resources() && !is_rectangular())
		update_shape(get_shape());
	rect bounds = rect(get_position(), get_size());
	bounds.sizef.width -= pddng.right+pddng.left;
	bounds.sizef.height -= pddng.bottom+pddng.top;
	bounds.position.x += pddng.left;
	bounds.position.y += pddng.top;
	float mwidth = 0.f, mheight = 0.f;
	for(auto itr = surfaces.begin(); itr != surfaces.end(); itr++)
	{
		auto surf = *itr;
		float width = surf->get_min_size().width, height = surf->get_min_size().height;
		float x = 0.f, y = 0.f;
		margin m = surf->get_margin();
		mwidth += width;
		mheight += height;

		switch(get_dock_style(surf))
		{
		case dock_style::bottom:
			{
				x = bounds.get_x();
				y = bounds.get_y()+bounds.get_height()-height-m.bottom-m.top;
				width = max(width, bounds.get_width()-m.right-m.left);
				bounds.sizef.height -= height+m.top+m.bottom;
				break;
			}
		case dock_style::top:
			{
				x = bounds.get_x();
				y = bounds.get_y();
				width = max(width, bounds.get_width()-m.right-m.left);
				bounds.sizef.height -= height+m.top+m.bottom;
				bounds.position.y += height+m.top+m.bottom;
				break;
			}
		case dock_style::left:
			{
				x = bounds.get_x();
				y = bounds.get_y();
				height = max(height, bounds.get_height()-m.top-m.bottom);
				bounds.sizef.width -= width+m.left+m.right;
				bounds.position.x += width+m.left+m.right;
				break;
			}
		case dock_style::right:
			{
				x = bounds.get_x()+bounds.get_width()-width-m.right-m.left;
				y = bounds.get_y();
				height = max(height, bounds.get_height()-m.top-m.bottom);
				bounds.sizef.width -= width+m.left+m.right;
				break;
			}
		case dock_style::fill:
			{
				x = bounds.get_x();
				y = bounds.get_y();
				width = max(width, bounds.get_width()-m.right-m.left);
				height = max(height, bounds.get_height()-m.top-m.bottom);
				bounds.position.x += width+m.left;
				bounds.position.y += height+m.top;
				bounds.sizef.width -= width+m.left+m.right;
				bounds.sizef.height -= height+m.top+m.bottom;
				break;
			}
		}

		if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
		if(surf->get_auto_position()) {surf->set_position(point(x+m.left, y+m.top), false);}
		surf->layout();
	}
	if(owner) owner->redraw(get_bounds());
	layouted();
	if(auto_min)
	{
		minsize.width = max(mwidth, 5.f);
		minsize.height = max(mheight, 5.f);	
	}
	// Works fine, tested 18.7.15 (Margin & Padding inluded)
}

dock_panel::dock_style dock_panel::get_dock_style(dynamic_drawsurface* dd)
{
	dock_style ds = dock_style::left;
	switch(dd->get_horizontal_align())
	{
	case horizontal_align::left:
		ds = dock_style::left;
		break;
	case horizontal_align::right:
		ds = dock_style::right;
		break;
	case horizontal_align::stretch:
		ds = dock_style::fill;
	}
	switch(dd->get_vertical_align())
	{
	case vertical_align::bottom:
		ds = dock_style::bottom;
		break;
	case vertical_align::stretch:
		ds = dock_style::fill;
		break;
	case vertical_align::top:
		ds = dock_style::top;
	}
	return ds;
}
// DockPanel

// Grouppanel
group_panel::group_panel()
{
	set_padding(padding(5, 5, 15, 5));
	border_width = 1.f;
	text_dist = 5.f;
	cl_back = colour::gray;
	cl_fback = colour::gcl_border;
	cl_font = colour::white;
	set_title(L"This is some Content");
}

group_panel::~group_panel()
{}

void group_panel::render(graphics* g)
{
	init_resources(g);
	if(get_focus() && get_enabled())
		br_back->set_colour(cl_fback);
	else
		br_back->set_colour(cl_back);
	pn_back->update();
	rect text_bounds = get_font()->get_metrics(get_title(), size::max_size(), g);
	g->draw_line(get_position(), point(get_position().x, get_size().height + get_position().y), pn_back.get());
	g->draw_line(point(get_position().x, get_size().height + get_position().y), point(get_position().x+get_size().width, get_size().height + get_position().y), pn_back.get());
	g->draw_line(point(get_position().x+get_size().width, get_size().height + get_position().y), point(get_position().x+get_size().width, get_position().y), pn_back.get());
	if(get_position().x+3*text_dist+text_bounds.get_width() < get_position().x+get_size().width-text_dist)
		g->draw_line(point(get_position().x+get_size().width, get_position().y), point(get_position().x+3*text_dist+text_bounds.get_width(), get_position().y),  pn_back.get());
	else
		g->draw_line(point(get_position().x+get_size().width, get_position().y), point(get_position().x+get_size().width-text_dist, get_position().y),  pn_back.get());
	g->draw_line(get_position(), point(get_position().x + text_dist, get_position().y), pn_back.get());
	g->push_clip(rect(point(max(get_position().x + text_dist, 0.1f), max(0.1f, get_position().y-text_bounds.get_height() / 2.f)), size(max(0.1f, get_size().width - 3*text_dist), text_bounds.get_height()+5)));
	g->draw_string(get_title(), point(get_position().x + text_dist*2, get_position().y - text_bounds.get_height()/2.f), get_font(), br_font.get());
	g->pop_clip();
	layout_container::render(g);
}

void group_panel::create_resources(graphics* g)
{
	layout_container::create_resources(g);
	if(app::is_high_contrast_app())
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
	else
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_font));
	br_back = shared_ptr<solid_brush>(g->create_solid_brush(cl_back));
	if(get_opacity() != 1)
	{
		float f = get_opacity();
		br_font->set_opacity(f);
		br_back->set_opacity(f);
	}
	pn_back = shared_ptr<pen>(g->create_pen(br_back.get(), border_width));
}

void group_panel::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_back->set_opacity(f);
		br_font->set_opacity(f);
	}
	layout_container::set_opacity(f);
}

void group_panel::layout()
{
	dynamic_drawsurface::layout();
	for(auto itr = surfaces.begin(); itr != surfaces.end(); itr++)
	{
		auto surf = *itr;
		margin m = surf->get_margin();
		float x = surf->get_position().x - get_hscroll_value(), y = surf->get_position().y - get_vscroll_value();
		float width = surf->get_size().width, height = surf->get_size().height;
		if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
		if(surf->get_auto_position()) {surf->set_position(point(x, y), false);}
		surf->layout();
	}
	if(owner) owner->redraw(get_bounds());
	layouted();
}

void group_panel::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
		br_font->set_colour(colour::gray_text);
	else
		br_font->set_colour(cl_font);
	layout_container::on_syscolour_changed();
}
// Grouppanel

// TabPanel
tab_panel::tab_panel()
{
	cl_back = colour::gcl_dark_gray;
	cl_hot = colour::gcl_hot_gray;
	cl_font = colour::white;
	cl_tab = colour::gcl_gray;
	cl_border = colour::gray;
	cl_borderf = colour::gcl_border;
	set_captures_mouse_wheel(true);
	set_captures_keyboard(true);
	tab_height = 20.f;
	arrow_width = 15.f;
	border_width = 1.f;
	set_min_size(size(35, 35));
	orient = tab_orientation::top;
	text_space = 10.f;
	selected_tab = 0;
	hidx = -1;
	mouse_move += make_func_ptr(this, & tab_panel::this_mouse_move);
	mouse_leave += make_func_ptr(this, &tab_panel::this_mouse_leave);
	mouse_down += make_func_ptr(this, &tab_panel::this_mouse_down);
	key_down += make_func_ptr(this, &tab_panel::this_key_down);
}

tab_panel::~tab_panel() 
{
	mouse_move -= make_func_ptr(this, & tab_panel::this_mouse_move);
	mouse_leave -= make_func_ptr(this, &tab_panel::this_mouse_leave);
	mouse_down -= make_func_ptr(this, &tab_panel::this_mouse_down);
	key_down -= make_func_ptr(this, &tab_panel::this_key_down);
}

void tab_panel::this_key_down(const virtual_keys& vk, const key_extended_params& param)
{
	if(tabs.empty())
		return;
	if(vk == virtual_keys::right)
	{
		auto itr = find(tabs.begin(), tabs.end(), selected_tab);
		if(itr != tabs.end())
		{
			int idx = itr - tabs.begin();
			if(++idx < static_cast<int>(tabs.size()))
				select_tab(idx);
			else
				select_tab(0);
		}
	}
	else if(vk == virtual_keys::left)
	{
		auto itr = find(tabs.begin(), tabs.end(), selected_tab);
		if(itr != tabs.end())
		{
			int idx = itr - tabs.begin();
			if(--idx >= 0)
				select_tab(idx);
			else
				select_tab(tabs.size()-1);
		}
	}
}

void tab_panel::layout()
{
	if(has_resources() && !is_rectangular())
		update_shape(get_shape());
	for(auto& surf : surfaces)
	{
		tab_page* tab = dynamic_cast<tab_page*>(surf);
		if(tab)
		{
			rect area = get_tab_area();
			tab->set_position(area.position, false);
			tab->set_size(area.sizef, false);
		}
		else
		{
			if(surf->get_auto_position())
			{
				float x = 0, y= 0;
				float w = 0, h = 0;
				margin m = surf->get_margin();
				switch(surf->get_horizontal_align())
				{
				case horizontal_align::left:
					x = get_position().x;
					w = 0;
					break;
				case horizontal_align::right:
					x = get_size().width + get_position().x - surf->get_size().width;
					w = 0;
					break;
				case horizontal_align::center:
					x = (get_size().width/2.f + get_position().x) - surf->get_size().width / 2.f;
					w = 0;
					break;
				case horizontal_align::stretch:
					w = get_size().width;
					x = get_position().x;
				}
				switch(surf->get_vertical_align())
				{
				case vertical_align::top:
					y = get_position().y;
					h = 0;
					break;
				case vertical_align::bottom:
					y = get_size().height + get_position().y - surf->get_size().height;
					h = 0;
					break;
				case vertical_align::center:
					y = (get_size().height / 2.f+ get_position().y) - surf->get_size().height / 2.f;
					h = 0;
					break;
				case vertical_align::stretch:
					y = get_position().y;
					h = get_size().height;
				}
				surf->set_position(point(x+pddng.left+m.left, y+pddng.top+m.top), false);
				if(surf->get_auto_size())surf->set_size(size(w-pddng.right-pddng.left-m.right-m.left, h-pddng.top-pddng.bottom-m.bottom-m.top), false);
			}
		}
		surf->layout();
	}
	redraw(get_bounds());
	layouted();
}

void tab_panel::render(graphics* g)
{
	if(!has_resources())
		init_resources(g);
	if(get_focus() && get_enabled())
		br_border->set_colour(cl_borderf);
	else if(!get_focus() || !get_enabled())
		br_border->set_colour(cl_border);
	pn_border->update();
	// Child area
	g->fill_rect(get_tab_area(), br_back.get());
	draw_tab_seperator(g);
	// Tab area
	g->push_clip(get_rect());
	float dist;
	if(orient == tab_orientation::top || orient == tab_orientation::bottom)
		dist = get_size().width-2*border_width;
	else
		dist = get_size().height-2*border_width;
	float curr_width = border_width;
	for(unsigned i = 0; i < tabs.size(); ++i)
	{
		tabs[i]->width = get_tab_width(i, g);
		draw_tab(g, curr_width, i);
		curr_width += tabs[i]->width + border_width;
		if(curr_width > dist)
			break;
	}
	g->pop_clip();
	if(!get_enabled())
		g->fill_rect(get_tab_area(), br_disabled.get());
	dynamic_drawsurface::render(g);
}

void tab_panel::draw_tab_seperator(graphics* g)
{
	rect rc = get_tab_area();
	rc.position.x += border_width / 2.f;
	rc.position.y += border_width / 2.f;
	rc.sizef.width -= border_width;
	rc.sizef.height -= border_width;
	g->draw_rect(rc, pn_border.get());
}

void tab_panel::draw_tab(graphics* g, float start, int idx)
{
	bool b = g->get_antialias();
	if(!is_transformed())
		g->set_antialias(true);
	auto tab = tabs[idx];
	switch(orient)
	{
	case tab_orientation::top:
		{
			start += get_position().x;
			if(tab == selected_tab)
			{
				g->draw_line(point(start-border_width/2.f, get_position().y), point(start-border_width/2.f, get_position().y + tab_height), pn_border.get()); // Left border
				g->draw_line(point(start+tab->width-border_width/2.f, get_position().y), point(start+tab->width-border_width/2.f, get_position().y + tab_height), pn_border.get()); // Right border
				g->draw_line(point(start-border_width/2.f, get_position().y+border_width/2.f), point(start+tab->width-border_width/2.f, get_position().y + border_width / 2.f), pn_border.get()); // Top border
				g->fill_rect(rect(start, get_position().y+border_width, tab->width-border_width, tab_height+10), br_back.get());
				g->draw_line(point(get_position().x + get_size().width - border_width / 2.f, get_position().y + get_size().height-border_width/2.f), point(get_position().x + get_size().width - border_width / 2.f, get_position().y+tab_height), pn_border.get());
			}
			else
				g->fill_rect(rect(start-border_width, get_position().y, tab->width+border_width, tab_height), hidx == idx ? br_hot.get() : br_tab.get());
			g->draw_string(tab->get_title(), point(start + text_space, get_position().y+border_width/2.f+tab_height/2.f-tab->height/2.f), tab->get_font(), br_font.get());
			if(!get_enabled())
				g->fill_rect(rect(start-border_width, get_position().y, tab->width+border_width, tab_height), br_disabled.get());
			break;
		}
	case tab_orientation::bottom:
		{
			start += get_position().x;
			if(tab == selected_tab)
			{
				g->draw_line(point(start-border_width/2.f, get_position().y+get_size().height), point(start-border_width/2.f, get_position().y - tab_height +get_size().height), pn_border.get()); // Left border
				g->draw_line(point(start+tab->width-border_width/2.f, get_position().y + get_size().height), point(start+tab->width-border_width/2.f, get_position().y + get_size().height - tab_height), pn_border.get()); // Right border
				g->draw_line(point(start-border_width/2.f, get_position().y + get_size().height-border_width/2.f), point(start+tab->width-border_width/2.f, get_position().y + get_size().height - border_width / 2.f), pn_border.get()); // Top border
				g->fill_rect(rect(start, get_position().y+get_size().height-border_width/2.f-tab_height-10, tab->width-border_width, tab_height+10), br_back.get());
				g->draw_line(point(get_position().x + get_size().width - border_width / 2.f, get_position().y + get_size().height-tab_height-border_width/2.f), point(get_position().x + get_size().width - border_width / 2.f, get_position().y+tab_height), pn_border.get());
			}
			else
				g->fill_rect(rect(start-border_width, get_position().y + get_size().height-tab_height, tab->width+border_width, tab_height), hidx == idx ? br_hot.get() : br_tab.get());
			g->draw_string(tab->get_title(), point(start + text_space, get_position().y+get_size().height-border_width/2.f-tab_height/2.f-tab->height/2.f), tab->get_font(), br_font.get());
			if(!get_enabled())
				g->fill_rect(rect(start-border_width, get_position().y + get_size().height-tab_height, tab->width+border_width, tab_height), br_disabled.get());
			break;
		}
	case tab_orientation::left:
		{
			start += get_position().y;
			if(tab == selected_tab)
			{
				g->draw_line(point(get_position().x, start-border_width/2.f), point(get_position().x + tab_height, start-border_width/2.f), pn_border.get()); // Top border
				g->draw_line(point(get_position().x, start -border_width/2.f + tab->width), point(get_position().x + tab_height, start-border_width/2.f + tab->width), pn_border.get()); // Bottom border
				g->draw_line(point(get_position().x+border_width/2.f, start-border_width/2.f), point(get_position().x+border_width/2.f, start-border_width/2.f+tab->width), pn_border.get()); // Left border
				g->fill_rect(rect(get_position().x+border_width, start, tab_height+10, tab->width-border_width), br_back.get());
				g->draw_line(point(get_position().x + tab_height, get_position().y + get_size().height-border_width/2.f), point(get_position().x + get_size().width - border_width / 2.f, get_position().y + get_size().height-border_width/2.f), pn_border.get());
			}
			else
				g->fill_rect(rect(get_position().x, start-border_width, tab_height, tab->width+border_width), hidx == idx ? br_hot.get() : br_tab.get());
			matrix rot;
			point text_pos(get_position().x + tab_height/2.f+border_width/2.f, start + (tab->width)/2.f); // Rotationpoint, in the middle of the tab
			point text_def(get_position().x + tab_height/2.f+border_width/2.f-tab->width/2.f+text_space, start + (tab->width)/2.f-tab->height/2.f); // Textpoint, text centered to the tab
			matrix m = get_absolute_transform();
			rot.rotate(270, text_pos); // Rotate matrix with rotationpoint
			g->set_transform(rot*m); // Transformgraphics, rotate first, then source transform
			g->draw_string(tab->get_title(), text_def, tab->get_font(), br_font.get());
			g->set_transform(get_absolute_transform()); // Reset transform
			if(!get_enabled())
				g->fill_rect(rect(get_position().x, start-border_width, tab_height, tab->width+border_width), br_disabled.get());
			break;
		}
	case tab_orientation::right:
		{
			start += get_position().y;
			if(tab == selected_tab)
			{
				g->draw_line(point(get_position().x+get_size().width, start-border_width/2.f), point(get_position().x - tab_height + get_size().width, start-border_width/2.f), pn_border.get()); // Top border
				g->draw_line(point(get_position().x+get_size().width, start -border_width/2.f + tab->width), point(get_position().x +get_size().width - tab_height, start-border_width/2.f + tab->width), pn_border.get()); // Bottom border
				g->draw_line(point(get_position().x+get_size().width-border_width/2.f, start-border_width/2.f), point(get_position().x+get_size().width-border_width/2.f, start-border_width/2.f+tab->width), pn_border.get()); // Left border
				g->fill_rect(rect(get_position().x-border_width+get_size().width-tab_height-10, start, tab_height+10, tab->width-border_width), br_back.get());
				g->draw_line(point(get_position().x + tab_height, get_position().y + get_size().height-border_width/2.f), point(get_position().x + get_size().width - border_width / 2.f - tab_height, get_position().y + get_size().height-border_width/2.f), pn_border.get());
			}
			else
				g->fill_rect(rect(get_position().x+get_size().width-tab_height, start-border_width, tab_height, tab->width+border_width), hidx == idx ? br_hot.get() : br_tab.get());
			matrix rot;
			point text_pos(get_position().x + get_size().width - tab_height/2.f-border_width/2.f, start + (tab->width)/2.f);
			point text_def(get_position().x + get_size().width - tab_height/2.f-border_width/2.f-tab->width/2.f+text_space, start + (tab->width)/2.f-tab->height/2.f);
			matrix m = get_absolute_transform();
			rot.rotate(90, text_pos);
			g->set_transform(rot*m);
			g->draw_string(tab->get_title(), text_def, tab->get_font(), br_font.get());
			g->set_transform(get_absolute_transform());
			if(!get_enabled())
				g->fill_rect(rect(get_position().x+get_size().width-tab_height, start-border_width, tab_height, tab->width+border_width), br_disabled.get());
			break;
		}
	}
	g->set_antialias(b);
}

void tab_panel::create_resources(graphics* g)
{
	if(app::is_high_contrast_app())
	{
		br_back = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_face));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_shadow));
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
		br_tab = shared_ptr<solid_brush>(g->create_solid_brush(colour::button_highlight));
	}
	else
	{
		br_back = shared_ptr<solid_brush>(g->create_solid_brush(cl_back));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(cl_hot));
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_font));
		br_tab = shared_ptr<solid_brush>(g->create_solid_brush(cl_tab));
	}
	br_border = shared_ptr<solid_brush>(g->create_solid_brush(cl_border));
	pn_border = shared_ptr<pen>(g->create_pen(br_border.get(), border_width));
	br_disabled = shared_ptr<solid_brush>(g->create_solid_brush(colour(50, colour::black)));
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_back->set_opacity(f);
		br_hot->set_opacity(f);
		br_font->set_opacity(f);
		br_tab->set_opacity(f);
		br_border->set_opacity(f);
		pn_border->update();
		br_disabled->set_opacity(f);
	}
}

void tab_panel::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_back->set_opacity(f);
		br_hot->set_opacity(f);
		br_font->set_opacity(f);
		br_tab->set_opacity(f);
		br_border->set_opacity(f);
		pn_border->update();
		br_disabled->set_opacity(f);
	}
	dynamic_drawsurface::set_opacity(f);
}

void tab_panel::on_syscolour_changed()
{
	if(app::is_high_contrast_app())
	{
		br_back->set_colour(colour::button_face);
		br_hot->set_colour(colour::button_shadow);
		br_font->set_colour(colour::gray_text);
		br_tab->set_colour(colour::button_face);
	}
	else
	{
		br_back->set_colour(cl_back);
		br_hot->set_colour(cl_hot);
		br_font->set_colour(cl_font);
		br_tab->set_colour(cl_tab);
	}
}

void tab_panel::this_mouse_move(const int mod, const point& p2)
{
	int old_idx = hidx;
	point p(p2.x+get_position().x, p2.y+get_position().y);
	hidx = -1;
	float start = border_width;
	for(unsigned i = 0; i < tabs.size(); ++i)
	{
		if(tab_contains(start, i, p))
		{
			hidx = i;
			break;
		}
		start += border_width + tabs[i]->width;
	}
	if(hidx != old_idx)
		redraw(get_bounds());
}

void tab_panel::this_mouse_leave(const point& p)
{
	if(change_if_diff(hidx, -1))
		redraw(get_bounds());
}

void tab_panel::this_mouse_down(const mouse_buttons& mb, const int mod, const point& p)
{
	if(mb != mouse_buttons::left)
		return;
	if(hidx >= 0 && hidx < static_cast<int>(tabs.size()))
	{
		if(selected_tab != tabs[hidx])
			select_tab(hidx);
	}
}

void tab_panel::select_tab(int index)
{
	if(index < 0 || index > static_cast<int>(tabs.size()))
		throw out_of_range("Index out of range");
	if(tabs[index] == selected_tab)
		return;
	if(selected_tab != 0)
		selected_tab->set_visible(false);
	selected_tab = tabs[index];
	selected_tab->set_visible(true);
	selected_tab_changed(selected_tab);
	redraw(get_bounds());
}

void tab_panel::add_tab(tab_page* tab)
{
	tab->set_parent(this);
}

void tab_panel::remove_tab(tab_page* tab)
{
	tab->set_parent(0);
	tab->set_owner(0);
}

void tab_panel::add_surface(dynamic_drawsurface* surf)
{
	if(!surf) throw invalid_argument("Invalid surface");
	tab_page* tab = dynamic_cast<tab_page*>(surf);
	if(tab)
	{
		tab->set_visible(false);
		tab->tab_owner = this;
		tabs.push_back(tab);
		if(selected_tab == 0)
			select_tab(tabs.size()-1);
	}
	dynamic_drawsurface::add_surface(surf);
}

void tab_panel::remove_surface(dynamic_drawsurface* surf)
{
	if(!surf) throw invalid_argument("Invalid surface");
	tab_page* tab = dynamic_cast<tab_page*>(surf);
	if(tab)
	{
		tabs.erase(remove(tabs.begin(), tabs.end(), tab));
		tab->tab_owner = 0;
		if(tab == selected_tab)
		{
			if(!tabs.empty())
				select_tab(0);
			else
				selected_tab = 0;
		}
	}
	dynamic_drawsurface::remove_surface(surf);
}

rect tab_panel::get_tab_area()
{
	rect area;
	switch(orient)
	{
	case tab_orientation::top:
		{
			area.position = point(get_position().x, tab_height+get_position().y);
			area.sizef = size(get_size().width, get_size().height - tab_height);
			break;
		}
	case tab_orientation::bottom:
		{
			area.position = get_position();
			area.sizef = size(get_size().width, get_size().height - tab_height);
			break;
		}
	case tab_orientation::left:
		{
			area.position = point(tab_height+get_position().x, get_position().y);
			area.sizef = size(get_size().width - tab_height, get_size().height);
			break;
		}
	case tab_orientation::right:
		{
			area.position = get_position();
			area.sizef = size(get_size().width - tab_height, get_size().height);
		}
	}
	return area;
}

float tab_panel::get_tab_width(int idx, graphics* g)
{
	tab_page* tb = tabs[idx];
	rect bounds =  tb->get_font()->get_metrics(tb->get_title(), size::max_size(), g);
	tb->height = bounds.get_height();
	return bounds.get_width()+2*text_space;
}

int tab_panel::get_tab_count()
{
	float dist;
	if(orient == tab_orientation::top || orient == tab_orientation::bottom)
		dist = get_size().width-2*border_width;
	else
		dist = get_size().height-2*border_width;
	float curr_width = 0.f;
	int cnt = 0;
	for(unsigned i = 0; i < tabs.size(); ++i, ++cnt)
	{
		curr_width += tabs[i]->width;
		if(curr_width > dist)
			break;
	}
	return cnt;
}

bool tab_panel::tab_contains(float start, int idx, const point& p)
{
	switch(orient)
	{
	case tab_orientation::bottom:
		start += get_position().x;
		return rect(start-border_width, get_position().y + get_size().height-tab_height, tabs[idx]->width+border_width, tab_height).contains(p, get_absolute_transform());
	case tab_orientation::top:
		start += get_position().x;
		return rect(start-border_width, get_position().y, tabs[idx]->width+border_width, tab_height).contains(p, get_absolute_transform());
	case tab_orientation::left:
		start += get_position().y;
		return rect(get_position().x, start-border_width, tab_height, tabs[idx]->width+border_width).contains(p, get_absolute_transform());
	case tab_orientation::right:
		start += get_position().y;
		return rect(get_position().x+get_size().width-tab_height, start-border_width, tab_height, tabs[idx]->width+border_width).contains(p, get_absolute_transform());
	default:
		return false;
	}
}

void tab_panel::set_back_colour(const colour& c)
{
	if(!change_if_diff(cl_back, c))
		return;
	if(has_resources())
		br_back->set_colour(c);
	back_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void tab_panel::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hot, c))
		return;
	if(has_resources())
		br_hot->set_colour(c);
	hot_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void tab_panel::set_font_colour(const colour& c)
{
	if(!change_if_diff(cl_font, c))
		return;
	if(has_resources())
		br_font->set_colour(c);
	font_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void tab_panel::set_tab_colour(const colour& c)
{
	if(!change_if_diff(cl_tab, c))
		return;
	if(has_resources())
		br_tab->set_colour(c);
	tab_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void tab_panel::set_border_colour(const colour& c)
{
	if(!change_if_diff(cl_border, c))
		return;
	border_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void tab_panel::set_border_focused_colour(const colour& c)
{
	if(!change_if_diff(cl_borderf, c))
		return;
	border_focused_colour_changed(c);
	if(owner)
		owner->redraw(get_bounds());
}

void tab_panel::set_tab_orientation(const tab_orientation& tab_orient)
{
	if(tab_orient == orient)
		return;
	orient = tab_orient;
	get_absolute_owner()->layout();
}
// TabPanel

};
};
};