#include "stdafx.h"
#include "controls.h"
#include "system.h"
namespace gcl { namespace ui {

// button
button::button() : dynamic_drawsurface()
{
	shape = 0;
	pn = 0;
	br = 0;
	s_br = 0;
	b_br = 0;
	f_br = 0;
	set_effect_colour(colour(colour::white));
	set_border_colour(colour(colour::gray));
	set_title(L"button");
	set_back_colour(colour(colour::dim_gray));
	layouted += make_func_ptr(this, &button::this_layouted);
	minsize = size(50, 50);
};

button::~button(){}

void button::create_resources(graphics* g)
{
	shape = g->create_geometry();
	shape->begin_geometry(point(0,0));
	shape->add_rounded_rect(rect(get_position(), get_size()), 5, 5);
	shape->end_geometry();
	f_br = g->create_solid_brush(get_font_colour());
	br = g->create_linear_gradient_brush(get_position(), point(get_position().x, get_position().y + get_size().height), gradient_stop(coll_f<2>(0.0f, 1.0f), coll_c<2>(colour(get_effect_colour()), colour(get_back_colour()))));
	s_br = g->create_solid_brush(get_border_colour());
	pn = g->create_pen(s_br);
	b_br = g->create_solid_brush(colour(colour::black));
	dynamic_drawsurface::create_resources(g);
}

void button::render(graphics* g)
{
	dynamic_drawsurface::render(g);
	if(!visible) return;
	
	g->fill_geometry(shape, br);
	g->draw_geometry(shape, pn);
	b_br->set_opacity(1);
	g->draw_string(get_title(), get_redraw_rc(), get_font(), f_br, string_format::direction_left_to_right, horizontal_string_align::middle, vertical_string_align::middle);
	if(get_enabled()) 
	{
		switch(draw_state)
		{		
		case drawing_state::pressed:
			b_br->set_opacity(0.12f);
			g->fill_geometry(shape, b_br);	
		case drawing_state::hover:
			b_br->set_opacity(0.06f);
			g->fill_geometry(shape, b_br);	
		}
	}
	for(auto& surf : surfaces)
		if(surf->get_visible())surf->render(g);
}

bool button::contains(const point& p) const
{
	if(!shape) return false;
	return shape->contains(p);
}

void button::this_layouted()
{
	if(!hs_resources) return;
	br->set_rect(rect(get_position(), size(get_position().x, get_position().y + get_size().height)));
	shape->begin_geometry(point(0,0));
	shape->add_rounded_rect(rect(get_position(), get_size()), 5, 5);
	shape->end_geometry();
}

void button::set_back_colour(const colour& c)
{
	if(!change_if_diff(back_colour, c)) return;
	if(br) br->set_gradients(gradient_stop(coll_f<2>(0.0f, 1.0f), coll_c<2>(colour(get_effect_colour()), colour(get_back_colour()))));
	back_colour_changed(c);
	if(get_owner()) get_owner()->redraw(get_redraw_rc());
}

void button::set_effect_colour(const colour& c)
{
	if(!change_if_diff(effect_colour, c)) return;
	if(br) br->set_gradients(gradient_stop(coll_f<2>(0.0f, 1.0f), coll_c<2>(colour(get_effect_colour()), colour(get_back_colour()))));
	effect_colour_changed(c);
	if(get_owner()) get_owner()->redraw(get_redraw_rc());
}

void button::set_font_colour(const colour& c)
{
	if(!change_if_diff(font_colour, c)) return;
	if(f_br) f_br->set_colour(c);
	font_colour_changed(c);
	if(get_owner()) get_owner()->redraw(get_redraw_rc());
}

void button::set_border_colour(const colour& c)
{
	if(!change_if_diff(border_colour, c)) return;
	if(s_br) s_br->set_colour(c);
	border_colour_changed(c);
	if(get_owner()) get_owner()->redraw(get_redraw_rc());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static_text

static_text::static_text()
{
	bk_br = 0;
	br = 0;
	set_back_colour(colour(0));
	minsize = size(50, 50);
}

void static_text::set_back_colour(const colour& c)
{
	if(!change_if_diff(back_colour, c)) return;
	back_colour_changed(c);
	if(bk_br) bk_br->set_colour(c);
	if(owner) owner->redraw(get_redraw_rc());
}

void static_text::set_font_colour(const colour& c)
{
	if(!change_if_diff(font_colour, c)) return;
	font_colour_changed(c);
	if(br) br->set_colour(c);
	if(owner) owner->redraw(get_redraw_rc());
}

void static_text::render(graphics* g)
{
	dynamic_drawsurface::render(g);
	g->fill_rect(get_redraw_rc(), bk_br);
	g->draw_string(title, rect(position, get_redraw_rc().sizef), get_font(), br);
	for(auto& surf : surfaces)
		if(surf->get_visible())surf->render(g);
}

bool static_text::contains(const point& p) const
{
	return get_redraw_rc().contains(p);
}

static_text::~static_text(){}

void static_text::create_resources(graphics* g)
{
	br = g->create_solid_brush(get_font_colour());
	bk_br = g->create_solid_brush(get_back_colour());
	dynamic_drawsurface::create_resources(g);
}

};
};