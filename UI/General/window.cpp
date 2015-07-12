#include "stdafx.h"
#include "window.h"
namespace gcl { namespace ui {

// Window
bool window::queue_running = false;

window::window(void) : _cl_hlp(mouse_click, mouse_down, mouse_up, is_mouse_over), handle(0), ico(0), surf_cur(0), ico_sm(0), erase_colour(0x000000), _graphics(0), maximizebox(true), minimizebox(true), closebox(true), borderstyle(window_borderstyles::sizeable), state(window_states::normal), startposition(window_startpositions::default_location), drop_handler(this)
{
	cur = LoadCursor(NULL, IDC_ARROW);
	set_min_size(size(0,0));
	has_resources = false;
	set_max_size(size::max_size());
	set_enabled(true);
	bottom_s = 0;
	top_s = 0;
	topmost_s = 0;
	opacity = 1.f;
	shown = false;
	focused_surf = 0;
	erase_colour = colour(0xFF323232);
}

void window::create_resources(graphics* g)
{
	has_resources = true;
	drop_handler.register_dragdrop();
	if(!ico)
		ico = shared_ptr<icon>(g->create_icon(system_icon::app));
	if(!ico_sm)
		ico_sm = shared_ptr<icon>(g->create_icon(system_icon::app));
	if(!m_font)
		m_font = shared_ptr<font>(g->get_system_font());
	for(auto surf : surfaces)
		surf->init_resources(g);
};

window::~window()
{
	has_resources = false;
	if(handle)
	{
		SendMessage(handle, WM_CLOSE, 0, 0);
		handle = 0;
	}
	for(auto& surf : surfaces)
	{
		surface_removed(surf);
	}
	surfaces.clear();
}

void window::render(graphics* g)
{
	if(!g) return;
	g->begin();
	g->clear(erase_colour);
	for(auto& surf : surfaces)
	{
		if(surf->get_visible())
		{
			g->set_transform(surf->get_transform());
			surf->render(g);
		}
	}
	g->end();
}

void window::redraw() 
{
	InvalidateRect(handle, 0, FALSE);
}

void window::redraw(const rect& s)
{
	RECT rc;
	rc.bottom = static_cast<int>(s.sizef.height+5+s.position.y-5);
	rc.right = static_cast<int>(s.sizef.width+5+s.position.x-5);
	rc.left = static_cast<int>(s.position.x-5);
	rc.top = static_cast<int>(s.position.y-5);
	InvalidateRect(handle, &rc, FALSE);
}

void window::close()
{
	SendMessage(handle, WM_CLOSE, 0, 0);
}

void window::set_enabled(bool b)
{
	if(!change_if_diff(enabled, b)) return;
	enabled_changed(b);
	if(handle)
		EnableWindow(handle, b);
	redraw();
};

void window::layout()
{
	for(auto& surf : surfaces)
	{
		if(surf->get_auto_position())
		{
			float x = 0, y = 0;
			float w = 0, h = 0;
			margin m = surf->get_margin();
			switch(surf->get_horinzontal_align())
			{
			case horizontal_align::left:
				x = pddng.left;
				w = 0;
				break;
			case horizontal_align::right:
				x = get_size().width - surf->get_size().width + pddng.left;
				w = 0;
				break;
			case horizontal_align::center:
				x = get_size().width / 2.f - surf->get_size().width / 2.f + pddng.left;
				w = 0;
				break;
			case horizontal_align::stretch:
				w = get_size().width;
				x = 0;
			}
			switch(surf->get_vertical_align())
			{
			case vertical_align::top:
				y = 0;
				h = 0;
				break;
			case vertical_align::bottom:
				y = get_size().height - surf->get_size().height;
				h = 0;
				break;
			case vertical_align::center:
				y = get_size().height / 2.f - surf->get_size().height / 2.f;
				h = 0;
				break;
			case vertical_align::stretch:
				y = 0;
				h = get_size().height;
			}
			surf->set_position(point(x+pddng.left+m.left, y+pddng.top+m.top), false);
			if(surf->get_auto_size())surf->set_size(size(w-pddng.right-pddng.left-m.right-m.left, h-pddng.top-pddng.bottom-m.bottom-m.top), false);
		}
		surf->layout();
	}
	layouted();
	redraw();
}

void window::set_padding(const padding& p)
{
	if(pddng == p) return;
	pddng = p;
	padding_changed(p);
	layout();
}

void window::add_surface(dynamic_drawsurface* surf)
{
	if(!surf) throw invalid_argument("Invalid surface");
	switch(surf->get_z_position())
	{
	case z_layer::bottom:
		surfaces.insert(std::next(surfaces.begin(), bottom_s), surf);
		bottom_s++;
		break;
	case z_layer::top:
		surfaces.insert(std::next(surfaces.begin(), bottom_s+top_s), surf);
		top_s++;
		break;
	case z_layer::top_most:
		surfaces.push_front(surf);
		topmost_s++;
	}
	if(surf->get_tab_index() == 0)
		surf->set_tab_index(surfaces.size()-1);
	layout();
	surface_added(surf);
}

void window::remove_surface(dynamic_drawsurface* surf)
{
	surfaces.remove(surf);
	switch(surf->get_z_position())
	{
	case z_layer::bottom:
		bottom_s--;
		break;
	case z_layer::top:
		top_s--;
		break;
	case z_layer::top_most:
		topmost_s--;
	}
	layout();
	surface_removed(surf);
}

void window::set_min_size(const size& s)
{
	if(minsize == s) return;
	minsize = s;
	min_size_changed(s);
	if(get_size().height < s.height)
		set_size(size(sizef.width, s.height), false);
	if(get_size().width < s.width)
		set_size(size(s.width, sizef.height));
	layout();
}

void window::set_max_size(const size& s)
{
	if(maxsize == s) return;
	maxsize = s;
	max_size_changed(s);
	if(get_size().height > s.height)
		set_size(size(sizef.width, s.height), false);
	if(get_size().width > s.width)
		set_size(size(s.width, sizef.height));
	layout();
}

void window::set_graphics(graphics* g)
{
	_graphics = g;
	create_resources(g);
	render(g);
}

void window::set_title(const wstring& title)
{
	if(this->title == title) return;
	this->title = title;
	title_changed(title);
	if(!handle) return;
	SetWindowText(handle, title.c_str());
}

void window::set_colour(const colour& c)
{
	erase_colour = c;
	render(_graphics);
}

void window::set_focus(bool b)
{
	if(!change_if_diff(focus, b)) return;
	SetFocus(b ? handle : GetDesktopWindow());
	focus_changed(b);
}

void window::set_focused_surface(dynamic_drawsurface* surf)
{
	if(focused_surf == surf) return;
	if(focused_surf)
	{
		focused_surf->set_focus(false);
	}
	if(surf)
	{
		surf->set_focus(true);
	}
	focused_surf = surf;
}

void window::set_borderstyle(const window_borderstyles style)
{
	borderstyle = style;
	if(!handle) return;
	DWORD exstyle = 0, styles = 0;
	switch(style)
	{
	case window_borderstyles::fixed_3D:
		styles = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_CLIENTEDGE;
		break;
	case window_borderstyles::fixed_dialog:
		styles = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME;
		break;
	case window_borderstyles::fixed_single:
		styles = WS_CAPTION | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
		break;
	case window_borderstyles::fixed_toolwindow:
		styles = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_TOOLWINDOW | WS_EX_CLIENTEDGE;
		break;
	case window_borderstyles::none:
		styles = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		break;
	case window_borderstyles::sizeable:
		styles = WS_VISIBLE | WS_THICKFRAME | WS_SYSMENU | WS_CAPTION;
		break;
	case window_borderstyles::sizeable_toolwindow:
		styles = WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_TOOLWINDOW | WS_EX_CLIENTEDGE;
		break;
	};
	SetWindowLong(handle, GWL_EXSTYLE, exstyle);
	SetWindowLong(handle, GWL_STYLE, styles);
}

void window::set_state(const window_states st)
{
	state = st;
	if(!handle) return;
	switch (st)
	{
	case window_states::normal:
		ShowWindow(handle, SW_NORMAL);
		break;
	case window_states::minimized:
		ShowWindow(handle, SW_MINIMIZE);
		break;
	case window_states::maximized:
		ShowWindow(handle, SW_MAXIMIZE);
		break;
	};
}

LRESULT CALLBACK window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	window* inst = 0;
	if(msg == WM_CREATE)
	{
		TRACKMOUSEEVENT tre = {};
		tre.cbSize = sizeof(tre);
		tre.dwFlags = TME_LEAVE | TME_HOVER;
		tre.hwndTrack = hWnd;
		TrackMouseEvent(&tre);
		inst = static_cast<window*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(inst));
	}
	else
	{
		inst = reinterpret_cast<window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if(!inst) return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return inst->message_received(hWnd, msg, wParam, lParam);
}

LRESULT window::message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SETFOCUS:
		focus = true;
		focus_changed(true);
		break;
	case WM_KILLFOCUS:
		focus = false;
		focus_changed(false);
		break;
	case WM_ERASEBKGND:
	/*	render(_graphics);*/
		return TRUE;
	case WM_PAINT:
		{
			if(_graphics)render(_graphics);
			break;
		}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
			point p((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			mouse_buttons btns = mouse_buttons::left;
			if(msg == WM_RBUTTONDOWN) 
				btns = mouse_buttons::right;
			else if(msg == WM_MBUTTONDOWN) 
				btns = mouse_buttons::middle;
			bool risen = false;
			is_mouse_dwn = true;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->contains(p) && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_mouse_down(btns, static_cast<int>(wParam), p);
					risen = true;
				}
			}
			if(!risen) mouse_down(btns, static_cast<int>(wParam), p);
			InvalidateRect(hWnd, 0, FALSE);
			break;
		}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		{
			point p((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			mouse_buttons btns = mouse_buttons::left;
			if(msg == WM_RBUTTONUP) 
				btns = mouse_buttons::right;
			else if(msg == WM_MBUTTONUP) 
				btns = mouse_buttons::middle;
			bool risen = false;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if((surf->contains(p) || surf->is_mouse_down()) && surf->get_visible() && is_mouse_dwn)
				{
					if(surf->get_enabled())
						surf->on_mouse_up(btns, static_cast<int>(wParam), p);
					risen = true;
				}
			}
			if(!risen) mouse_up(btns, static_cast<int>(wParam), p);
			is_mouse_dwn = false;
			InvalidateRect(hWnd, 0, FALSE);
			break;
		}
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		{
			point p((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			mouse_buttons btns = mouse_buttons::left;
			if(msg == WM_RBUTTONDBLCLK) 
				btns = mouse_buttons::right;
			else if(msg == WM_MBUTTONDBLCLK) 
				btns = mouse_buttons::middle;
			bool risen = false;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->contains(p) && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_mouse_dbl_click(mouse_buttons::middle, static_cast<int>(wParam), p);
					risen = true;
				}
			}
			if(!risen)
			{
				auto s = get_focused_surface();
				if(s)
					s->set_focus(false);
				set_focus(true);
				mouse_dbl_click(mouse_buttons::middle, static_cast<int>(wParam), p);
			}
			break;
		}
	case WM_MOUSEMOVE:
		{
			point p((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			bool risen = false;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->contains(p) && surf->get_visible())
				{
					if(!surf->is_mouse_over() && surf->get_enabled())
						surf->on_mouse_enter(p);
					if(surf->get_enabled())
						surf->on_mouse_move(static_cast<int>(wParam), p);
					risen = true;
				}
				else if(surf->is_mouse_over() && surf->is_available())
					surf->on_mouse_leave(p);
				if(!is_mouse_over)
				{
					mouse_enter(p);
					if(surf->is_mouse_down() && !(((wParam & mouse_modifiers::l_button) == mouse_modifiers::l_button) || ((wParam & mouse_modifiers::r_button) == mouse_modifiers::r_button) || ((wParam & mouse_modifiers::m_button) == mouse_modifiers::m_button)) )
						surf->crt_up();
					InvalidateRect(hWnd, 0, FALSE);
				}
			}
			if(!risen) mouse_move(static_cast<int>(wParam), p);
			if(!is_mouse_over)
			{
				TRACKMOUSEEVENT tre = {};
				tre.cbSize = sizeof(tre);
				tre.dwFlags = TME_LEAVE | TME_HOVER;
				tre.hwndTrack = hWnd;
				TrackMouseEvent(&tre);
				is_mouse_over = true;
			}
			break;
		}
	case WM_MOUSELEAVE:
		is_mouse_over = false;
		is_mouse_dwn = false;
		for(auto& surf : surfaces) surf->on_mouse_leave(point(-1.f, -1.f));;
		InvalidateRect(hWnd, 0, TRUE);
		mouse_leave(point((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)));
		break;
	case WM_SIZE:
		{
			switch(wParam)
			{
			case SIZE_MAXIMIZED:
				state = window_states::maximized;
				break;
			case SIZE_MINIMIZED:
				state = window_states::minimized;
				break;
			case SIZE_RESTORED:
				state = window_states::normal;
			}
			layout();
			size_changed(get_size(), static_cast<resizing_types>(wParam));
			for(auto& surf : surfaces) 
			{
				surf->size_changed(surf->get_size(), static_cast<resizing_types>(wParam));
			}
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* p = reinterpret_cast<MINMAXINFO*>(lParam);
			p->ptMaxTrackSize.x = static_cast<long>(maxsize.width == size::max_size().width ? GetSystemMetrics(SM_CXMAXTRACK) : maxsize.width);
			p->ptMaxTrackSize.y = static_cast<long>(maxsize.height == size::max_size().height ? GetSystemMetrics(SM_CYMAXTRACK) : maxsize.height);
			p->ptMinTrackSize.x = static_cast<long>(minsize.width);
			p->ptMinTrackSize.y = static_cast<long>(minsize.height);
			break;
		}
	case WM_MOUSEWHEEL:
		{
			bool risen = false;
			point pos = point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_mouse_wheel() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_mouse_wheel(GET_KEYSTATE_WPARAM(wParam), pos, GET_WHEEL_DELTA_WPARAM(wParam));
					risen = true;
				}
			}
			if(!risen)
				mouse_wheel(GET_KEYSTATE_WPARAM(wParam), pos, GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		}
	case WM_MOUSEHWHEEL:
		{
			bool risen = false;
			point pos = point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_mouse_wheel() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_mouse_h_wheel(GET_KEYSTATE_WPARAM(wParam), pos, GET_WHEEL_DELTA_WPARAM(wParam));
					risen = true;
				}
			}
			if(!risen)
				mouse_h_wheel(GET_KEYSTATE_WPARAM(wParam), pos, GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		}
	case WM_KEYDOWN:
		{
			if(static_cast<virtual_keys>(wParam) == virtual_keys::tab)
			{
				auto surf = get_focused_surface();
				if(surf)
				{
					if(surf->on_tab_pressed())
					{
						unsigned last_tab = surf->get_tab_index();
						auto nexts = surfaces.end();
						for(auto itr = surfaces.begin(); itr != surfaces.end(); ++itr)
						{
							if((*itr)->get_tab_stop() && (*itr)->is_available())
							{
								if(nexts == surfaces.end())
								{
									if((*itr)->get_tab_index() > last_tab)
									{nexts = itr;}
								}
								else
								{
									if((*itr)->get_tab_index() > last_tab && (*itr)->get_tab_index() < (*nexts)->get_tab_index())
									{nexts == itr;}
								}
							}
						}
						if(nexts != surfaces.end()) 
						{
							(*nexts)->set_focus(true);
						}
						else
						{
							surf->set_focus(false);
						}
					}
				}
				else
				{
					auto sitr = surfaces.end();
					for(auto itr = surfaces.begin(); itr!=surfaces.end(); ++itr)
					{
						if((*itr)->get_tab_stop() && (*itr)->is_available())
						{
							if(sitr == surfaces.end())
								sitr = itr;
							else
							{
								if((*itr)->get_tab_index() < (*sitr)->get_tab_index())
									sitr = itr;
							}
						}
					}
					if(sitr != surfaces.end())
						(*sitr)->set_focus(true);
				}
			}
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_key_down(static_cast<virtual_keys>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				key_down(static_cast<virtual_keys>(wParam), params);
			auto surf = get_focused_surface();
			if(surf && wParam == VK_APPS)
				surf->on_menu_opening();
			break;
		}
	case WM_KEYUP:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_key_up(static_cast<virtual_keys>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				key_up(static_cast<virtual_keys>(wParam), params);
			break;
		}
	case WM_SYSKEYDOWN:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_syskey_down(static_cast<virtual_keys>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				syskey_down(static_cast<virtual_keys>(wParam), params);
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	case WM_SYSKEYUP:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_syskey_up(static_cast<virtual_keys>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				syskey_up(static_cast<virtual_keys>(wParam), params);
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	case WM_CHAR:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_char_sent(static_cast<wchar_t>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				char_sent(static_cast<wchar_t>(wParam), params);
			break;
		}
	case WM_DEADCHAR:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_deadchar_sent(static_cast<wchar_t>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				deadchar_sent(static_cast<wchar_t>(wParam), params);
			break;
		}
	case WM_SYSCHAR:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_syschar_sent(static_cast<wchar_t>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				syschar_sent(static_cast<wchar_t>(wParam), params);
			break;
		}
	case WM_SYSDEADCHAR:
		{
			bool risen = false;
			key_extended_params params;
			bitset<32> b(lParam);
			params.transition_state = b.test(31);
			params.previous_state = b.test(30);
			params.context_code = b.test(29);
			params.extended_key = b.test(24);
			params.scan_code = static_cast<char>((lParam & 0xff0000) >> 16); 
			params.repeat_count = lParam & 0xffff;
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
				{
					if(surf->get_enabled())
						surf->on_sysdeadchar_sent(static_cast<wchar_t>(wParam), params);
					risen = true;
				}
			}
			if(!risen)
				sysdeadchar_sent(static_cast<wchar_t>(wParam), params);
			break;
		}
	case WM_SETCURSOR:
		if(LOWORD(lParam) == HTCLIENT) 
		{
			SetCursor(surf_cur.get_cursor() ? surf_cur.get_cursor() : cur.get_cursor());
			return TRUE;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_GCL_CURSORCHANGED:
		{
			surf_cur = cursor_surface(reinterpret_cast<HCURSOR>(wParam));
			SendMessage(hWnd, WM_SETCURSOR, 0, MAKELONG(HTCLIENT, 0));
			break;
		}
	case WM_CLOSE:
		drop_handler.deregister_dragdrop();
		drop_handler.deregister_helper();
		DestroyWindow(hWnd);
		break;
	case WM_SYSCOLORCHANGE: // Gucke nach älteren Systemen
		{
			syscolour_changed();
			for(int i = surfaces.size()-1; i >= 0; --i)
			{
				auto surf = *next(surfaces.begin(), i);
				surf->on_syscolour_changed();
			}
			break;
		}
	case WM_DESTROY:
		handle_destroyed(hWnd);
		shown = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int window::show(window* win, bool use_handle)
{
	if(shown) throw invalid_argument("Window already exists");
	if(_graphics && !has_resources) create_resources(_graphics);
	wstring classname = gcl_create_classname(L"GCL_hWndWrapp&");
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.hCursor = cur.get_cursor();
	wcex.hIcon = ico ? ico->get_icon() : LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = ico_sm ? ico_sm->get_icon() : wcex.hIcon;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.lpszClassName  = classname.c_str();
	wcex.style = CS_DROPSHADOW | CS_DBLCLKS;
	if(!closebox) wcex.style |= CS_NOCLOSE;
	wcex.lpfnWndProc = WndProc;
	DWORD exstyle = 0, styles = 0;
	switch(borderstyle)
	{
	case window_borderstyles::fixed_3D:
		styles = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_CLIENTEDGE;
		break;
	case window_borderstyles::fixed_dialog:
		styles = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME;
		break;
	case window_borderstyles::fixed_single:
		styles = WS_CAPTION | WS_VISIBLE | WS_CLIPCHILDREN | WS_SYSMENU;
		break;
	case window_borderstyles::fixed_toolwindow:
		styles = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_TOOLWINDOW | WS_EX_CLIENTEDGE;
		break;
	case window_borderstyles::none:
		styles = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		break;
	case window_borderstyles::sizeable:
		styles = WS_VISIBLE | WS_THICKFRAME | WS_SYSMENU | WS_CAPTION;
		break;
	case window_borderstyles::sizeable_toolwindow:
		styles = WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		exstyle = WS_EX_TOOLWINDOW | WS_EX_CLIENTEDGE;
		break;
	};
	if(maximizebox) styles |= WS_MAXIMIZEBOX;
	if(minimizebox) styles |= WS_MINIMIZEBOX;
	on_create_params(styles, exstyle, wcex.style, classname);
	point location;
	switch(startposition)
	{
	case window_startpositions::center_screen:
		{
			RECT rc;
			GetWindowRect(GetDesktopWindow(), &rc);
			location = point((rc.right - rc.left) / 2.0f - sizef.width / 2.0f, (rc.bottom - rc.top) / 2.0f - sizef.height / 2.0f);
			break;
		}
	case window_startpositions::default_location:
		location = point(CW_USEDEFAULT, CW_USEDEFAULT);
		break;
	case window_startpositions::manual:
		location = position;
		break;
	}
	if(!RegisterClassEx(&wcex))
		throw runtime_error("Registering windowclass failed. (" + to_string(GetLastError()) + ")");
	HWND hWnd = CreateWindowEx(exstyle, classname.c_str(), title.c_str(), styles, static_cast<int>(location.x), static_cast<int>(location.y), static_cast<int>(sizef.width), static_cast<int>(sizef.height), (win&&use_handle) ? win->get_handle() : 0, 0, GetModuleHandle(NULL), this); 
	if(!hWnd) throw runtime_error("Creating window failed. (" + to_string(GetLastError()) + ")");
	handle = hWnd;
	handle_created(hWnd);
	layout();
	switch (state)
	{
	case window_states::normal:
		ShowWindow(handle, SW_SHOWNORMAL);
		break;
	case window_states::minimized:
		ShowWindow(handle, SW_SHOWMINIMIZED);
		break;
	case window_states::maximized:
		ShowWindow(handle, SW_SHOWMAXIMIZED);
		break;
	};
	shown = true;
	if(win)
		win->set_enabled(false);
	UpdateWindow(hWnd);
	if(queue_running)
		return 0;
	MSG msg;
	queue_running = true;
	while(GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnregisterClass(classname.c_str(), GetModuleHandle(NULL));
	if(win)
	{
		win->set_enabled(true);
		win->set_focus(true);
	}
	return msg.wParam;
}

void window::set_maximizebox(bool b)
{
	if(maximizebox == b) return;
	maximizebox = b;
	if(handle)
		SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) ^ WS_MAXIMIZEBOX);
}

void window::set_minimizebox(bool b)
{
	if(minimizebox == b) return;
	minimizebox = b;
	if(handle)
		SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) ^ WS_MINIMIZEBOX);
}

void window::set_closebox(bool b)
{
	if(closebox == b) return;
	closebox = b;
	if(handle)
		SetClassLong(handle, GCL_STYLE, GetClassLong(handle, GCL_STYLE) ^ CS_NOCLOSE);
}

size window::get_size() const
{
	if(!handle) return sizef;
	RECT rc;
	GetClientRect(handle, &rc);
	return size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
}

void window::set_size(const size& sz, bool redraw)
{
	if(sizef == sz) return; 
	sizef.width =  max(min(maxsize.width, sz.width), minsize.width);
	sizef.height =  max(min(maxsize.height, sz.height), minsize.height);
	size_changed(sz, resizing_types::restored); 
	if(redraw) layout();
	if(!handle) return;
	SetWindowPos(handle, 0, 0, 0, static_cast<int>(sz.width), static_cast<int>(sz.height), SWP_NOMOVE | SWP_NOZORDER);
}

point window::get_position() const
{
	if(!handle) return position;
	RECT rc;
	GetWindowRect(handle, &rc);
	return point(static_cast<float>(rc.left), static_cast<float>(rc.top));
}

void window::set_position(const point& p, bool redraw)
{
	position = p;
	position_changed(p);
	if(!handle) return;
	SetWindowPos(handle, 0, static_cast<int>(p.x), static_cast<int>(p.y), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void window::set_visible(bool b)
{
	if(visible == b) return;
	visible = b;
	visible_changed(b);
	b ? ShowWindow(handle, SW_SHOW) : ShowWindow(handle, SW_HIDE);
}

rect window::get_rect() const
{
	return rect(point(0, 0), get_size());
}

bool window::contains(const point& p) const
{
	return get_rect().contains(p);
}

void window::set_opacity(float val)
{
	if(opacity == val) return;
	if(val < 0 || val > 1) throw invalid_argument("Parameter \"val\" must be greater than 0 and lesser 1");
	opacity = val;
	SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(handle, 0, static_cast<BYTE>(255*val), LWA_ALPHA);
	opacity_changed(val);
}

LRESULT window::send_message(UINT msg, LPARAM lParam, WPARAM wParam)
{
	return SendMessage(handle, msg, wParam, lParam);
}

void window::set_icon(icon* ico_)
{
	if(ico.get() == ico_) return;
	ico.reset(ico_);
	if(!handle) return;
	if(ico)
	{
		SetClassLong(handle, GCL_HICON, reinterpret_cast<LONG>(ico->get_icon()));
	}
	else
	{
		SetClassLong(handle, GCL_HICON, 0);
	}
}

void window::set_small_icon(icon* ico_)
{
	if(ico_sm.get() == ico_) return;
	ico_sm.reset(ico_);
	if(!handle) return;
	if(ico_sm)
	{
		SetClassLong(handle, GCL_HICONSM, reinterpret_cast<LONG>(ico_sm->get_icon()));
	}
	else
	{
		SetClassLong(handle, GCL_HICONSM, 0);
	}
}

void window::set_cursor(const cursor_surface& cur_)
{
	if(cur.get_cursor() == cur_.get_cursor() || cur_.get_cursor() == 0)
		return;
	cur = cur_;
	SendMessage(handle, WM_SETCURSOR, reinterpret_cast<WPARAM>(handle), MAKELONG(HTCLIENT, 0));
}

cursor_surface window::get_cursor() const
{
	return cur;
}

bool window::flash(bool invert)
{
	return FlashWindow(handle, invert) != 0;
}

bool window::flash(const flash_modes& mode, unsigned int flash_count, const chrono::milliseconds& flash_duration)
{
	FLASHWINFO inf;
	inf.cbSize = sizeof(FLASHWINFO);
	inf.dwFlags = static_cast<DWORD>(mode);
	inf.dwTimeout = static_cast<DWORD>(flash_duration.count());
	inf.hwnd = handle;
	inf.uCount = flash_count;
	return FlashWindowEx(&inf) != 0;
}

void window::set_font(font* f)
{
	if(f == m_font.get()) return;
	if(f)
	{
		m_font.reset(f);
		InvalidateRect(handle, 0, FALSE);
	}
	else
	{
		if(m_font)
		m_font.reset(get_graphics()->get_system_font());
	}
}

void window::on_drag_enter(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect)
{
	bool risen = false;
	POINT pn;
	GetCursorPos(&pn);
	MapWindowPoints(GetDesktopWindow(), handle, &pn, 1);
	last_data_object = data_object;
	point p(static_cast<float>(pn.x), static_cast<float>(pn.y));
	for(auto surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available())
		{
			surf->on_drag_enter(data_object, keystate, pt, effect);
			surf->is_drop_entered = true;
			risen = true;
		}
	}
	if(!risen && get_enable_dragdrop())
		drag_enter(data_object, keystate, pt, effect);
}

void window::on_drag_over(DWORD keystate, const point& pt, dragdrop::drop_effects* effect)
{
	bool risen = false;
	POINT pn;
	GetCursorPos(&pn);
	MapWindowPoints(GetDesktopWindow(), handle, &pn, 1);
	point p(static_cast<float>(pn.x), static_cast<float>(pn.y));
	for(auto surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available() && surf->is_drop_entered)
		{
			surf->on_drag_over(keystate, pt, effect);
			risen = true;
		}
		else if(surf->contains(p) && surf->is_available() && !surf->is_drop_entered)
		{
			surf->on_drag_enter(last_data_object, keystate, pt, effect);
			surf->is_drop_entered = true;
		}
		else if(surf->is_drop_entered && !surf->contains(p))
		{
			surf->on_drag_leave();
			surf->is_drop_entered = false;
		}
	}
	if(!risen && get_enable_dragdrop())
		drag_over(keystate, pt, effect);
}

void window::on_drag_leave()
{
	for(auto surf : surfaces)
	{
		if(surf->is_drop_entered)
		{
			surf->on_drag_leave();
			surf->is_drop_entered = false;
		}
	}
	if(get_enable_dragdrop())
		drag_leave();
}

void window::on_drop(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect)
{
	bool risen = false;
	POINT pn;
	GetCursorPos(&pn);
	MapWindowPoints(GetDesktopWindow(), handle, &pn, 1);
	point p(static_cast<float>(pn.x), static_cast<float>(pn.y));
	for(auto surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available() && surf->is_drop_entered/* && surf->get_enable_dragdrop()*/)
		{
			surf->on_drop(data_object, keystate, pt, effect);
			surf->is_drop_entered = false;
			risen = true;
		}
	}
	if(!risen && get_enable_dragdrop())
		drop(data_object, keystate, pt, effect);
}
// Window

// WindowedTimer
windowed_timer::~windowed_timer()
{
	if(running)
	{
		KillTimer(owner, reinterpret_cast<UINT_PTR>(this));
	}
}

void windowed_timer::create(HWND wind, const chrono::milliseconds& ticks)
{
	if(running)
		throw logic_error("Timer already running");
	if(!wind)
		throw invalid_argument("Invalid handle");
	owner = wind;
	interval = ticks;
	if(!SetTimer(wind, reinterpret_cast<UINT_PTR>(this), static_cast<UINT>(ticks.count()), timer_proc))
		throw runtime_error("Unable to kill timer");
	running = true;
}

void windowed_timer::kill()
{
	if(!running)
		throw logic_error("Timer isn't running");
	KillTimer(owner, reinterpret_cast<UINT_PTR>(this));
	owner = 0;
	running = false;
}

void CALLBACK windowed_timer::timer_proc(HWND hWnd, UINT msg, UINT_PTR event_id, DWORD time)
{
	if(msg == WM_TIMER)
	{
		windowed_timer* inst = reinterpret_cast<windowed_timer*>(event_id);
		inst->tick();
	}
}
// WindowedTimer
};
};