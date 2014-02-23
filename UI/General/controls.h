/*****************************************************************************
*                           gcl - control.h                                 *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*****************************************************************************/
#ifndef CONTROL_H
#define CONTROL_H
#ifdef _MSC_VER 
#pragma once
#endif
#include "stdafx.h"
#include "callback.h"
#include "graphics.h"
using namespace gcl::render_objects;

namespace gcl { namespace ui {

class button : 
	public dynamic_drawsurface
{
public:
	button();

	callback<void(const colour&)> border_colour_changed;

	virtual void render(graphics* g);
	virtual bool contains(const point& p) const;
	virtual void release_resources();

	virtual void set_back_colour(const colour& c);
	virtual void set_effect_colour(const colour& c);
	virtual void set_font_colour(const colour& c);
	virtual colour get_border_colour() const {return border_colour;};
	virtual void set_border_colour(const colour& c);
private:
    geometry* shape;
	linear_gradient_brush* br;
	pen* pn;
	solid_brush* s_br;
	solid_brush* b_br;
	solid_brush* f_br;
protected:
	virtual void create_resources(graphics* g);
	virtual void on_size(const size&);
	colour border_colour;
};

};
};

#endif