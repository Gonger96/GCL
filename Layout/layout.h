/*****************************************************************************
*                           gcl - layout.h                                   *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#ifndef LAYOUT_H
#define LAYOUT_H
#if _MSC_VER
   #pragma once
#endif
#include "stdafx.h"
#include "graphics.h"

namespace gcl { namespace ui { namespace layout {

// Orders its elements by their fixed position
typedef dynamic_drawsurface canvas;

enum class orientation {vertical, horizontal};
enum class child_orientation {top, bottom, right, left};

// Orders its elements either from right to left or from top to bottom and vice versa
class stack_panel :
	public dynamic_drawsurface
{
public:
	stack_panel();

	callback<void(const child_orientation&)> child_orient_changed;
	callback<void(const orientation&)> orient_changed;

	virtual bool contains(const point& p) const;
	virtual void render(render_objects::graphics* g);
	virtual void layout();

	void set_child_orient(const child_orientation& ort);
	child_orientation get_child_orient() const {return child_orient;};
	void set_orient(const orientation& ort);
	orientation get_orient() const {return orient;};
private:
	orientation orient;
	child_orientation child_orient;
};

};
};
};
#endif