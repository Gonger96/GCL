#include "stdafx.h"
#include "layout.h"
namespace gcl { namespace ui { namespace layout {

stack_panel::stack_panel() : orient(orientation::vertical), child_orient(child_orientation::top) {}

void stack_panel::render(render_objects::graphics* g)
{
	for(auto& surf : surfaces)
		if(surf->get_visible()) surf->render(g);
}

bool stack_panel::contains(const point& p) const
{
	return get_redraw_rc().contains(p);
}

void stack_panel::layout()
{
	dynamic_drawsurface* last_surf = 0;

	if(child_orient == child_orientation::top || child_orient == child_orientation::left)
	{
		for(auto itr = surfaces.begin(); itr != surfaces.end(); itr++)
		{
			auto surf = *itr;
			margin m = surf->get_margin();
			float x = 0, y = 0;
			float width = 0, height = 0;
			if(orient == orientation::vertical)
			{
				switch(surf->get_horinzontal_align())
				{
				case horizontal_align::left:
					x = pddng.left + m.left;
					break;
				case horizontal_align::center:
					x = get_size().width / 2.f - surf->get_size().width / 2.f + pddng.left - pddng.right + m.left - m.right;
					break;
				case horizontal_align::right:
					x = get_size().width - surf->get_size().width - pddng.right - m.right;
					break;
				default: // stretch
					x = pddng.left + m.left;
					width = get_size().width - x - pddng.right - m.right;
				}
			}
			else // horizontal
			{
				switch(surf->get_vertical_align())
				{
				case vertical_align::top:
					y = pddng.top + m.top;
					break;
				case vertical_align::center:
					y = get_size().height / 2.f - surf->get_size().height / 2.f + pddng.top + m.top - m.bottom - pddng.bottom;
					break;
				case vertical_align::bottom:
					y = get_size().height - surf->get_size().height - pddng.bottom - m.bottom;
					break;
				default: // stretch
					y = pddng.top + m.top;
					height = get_size().height - y - pddng.bottom - m.bottom;
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
					y = pddng.top + m.top;
				}
				else
				{
					x = pddng.left + m.left;
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
			float width = 0, height = 0;
			if(orient == orientation::vertical)
			{
				switch(surf->get_horinzontal_align())
				{
				case horizontal_align::left:
					x = pddng.left + m.left;
					break;
				case horizontal_align::center:
					x = get_size().width / 2.f - surf->get_size().width / 2.f + pddng.left - pddng.right + m.left - m.right;
					break;
				case horizontal_align::right:
					x = get_size().width - surf->get_size().width - pddng.right - m.right;
					break;
				default: // stretch
					x = pddng.left + m.left;
					width = get_size().width - x - pddng.right - m.right;
				}
			}
			else // horizontal
			{
				switch(surf->get_vertical_align())
				{
				case vertical_align::top:
					y = pddng.top + m.top;
					break;
				case vertical_align::center:
					y = get_size().height / 2.f - surf->get_size().height / 2.f + pddng.top + m.top - m.bottom - pddng.bottom;
					break;
				case vertical_align::bottom:
					y = get_size().height - surf->get_size().height - pddng.bottom - m.bottom;
					break;
				default: // stretch
					y = pddng.top + m.top;
					height = get_size().height - y - pddng.bottom - m.bottom;
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
					y = get_size().height - surf->get_size().height - pddng.bottom - m.bottom;
				}
				else
				{
					x = get_size().width - surf->get_size().width - pddng.right - m.right;
				}
			}
			last_surf = surf;
			if(surf->get_auto_size()) {surf->set_size(size(width, height), false);}
			if(surf->get_auto_position()) {surf->set_position(point(x, y), false);}
			surf->layout();
		}
	}
	if(owner) owner->redraw(get_redraw_rc());
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

};
};
};