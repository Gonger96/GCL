#include "stdafx.h"
#include "window.h"
namespace gcl { namespace ui {


window::window(void) : _cl_hlp(mouse_click, mouse_down, mouse_up, is_mouse_over), handle(0), erase_colour(0x000000), _graphics(0), maximizebox(true), minimizebox(true), closebox(true), borderstyle(window_borderstyles::sizeable), state(window_states::normal), startposition(window_startpositions::default_location)
{
	set_min_size(size(0,0));
	set_max_size(size::max_size());
	set_enabled(true);
	bottom_s = 0;
	top_s = 0;
	topmost_s = 0;
}

void window::create_resources(graphics* g)
{
	_graphics->set_antialias(true);
	_graphics->set_text_rendering_mode(text_rendering_modes::antialias);
};

void window::release_resources()
{
	_graphics->release_resources();
}

void window::render(graphics* g)
{
	if(!g) return;
	g->begin();
	g->clear(erase_colour);
	for(auto& surf : surfaces)
		if(surf->get_visible())surf->render(g);
	g->end();
}

void window::redraw() {InvalidateRect(handle, 0, FALSE);}

void window::redraw(const rect& s)
{
	RECT rc;
	rc.bottom = static_cast<int>(s.sizef.height+s.position.y+1);
	rc.right = static_cast<int>(s.sizef.width+s.position.x+1);
	rc.left = static_cast<int>(s.position.x+1);
	rc.top = static_cast<int>(s.position.y+1);
	InvalidateRect(handle, &rc, FALSE);
}

void window::set_enabled(bool b)
{
	if(!change_if_diff(enabled, b)) return;
	enabled_changed(b);
	if(handle)EnableWindow(handle, b);
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
	redraw();
	layouted();
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
	try {
	switch (msg)
	{
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
			for(auto& surf : surfaces)
			{
				if(surf->contains(p) && surf->is_available())
				{
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
			is_mouse_dwn = false;
			for(auto& surf : surfaces)
			{
				if((surf->contains(p) || surf->get_mouse_down()) && surf->is_available())
				{
					surf->on_mouse_up(btns, static_cast<int>(wParam), p);
					risen = true;
				}
			}
			if(!risen) mouse_up(btns, static_cast<int>(wParam), p);
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
			for(auto& surf : surfaces)
			{
				if(surf->contains(p) && surf->is_available())
				{
					surf->on_mouse_dbl_click(mouse_buttons::middle, static_cast<int>(wParam), p);
					risen = true;
				}
			}
			if(!risen) mouse_dbl_click(mouse_buttons::middle, static_cast<int>(wParam), p);
			break;
		}
	case WM_MOUSEMOVE:
		{
			point p((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			bool risen = false;
			for(auto& surf : surfaces)
			{
				if(surf->contains(p) && surf->is_available())
				{
					if(!surf->get_mouse_over())
						surf->on_mouse_enter(static_cast<int>(wParam), p);
					surf->on_mouse_move(static_cast<int>(wParam), p);
					risen = true;
				}
				else if(surf->get_mouse_over() && surf->is_available())
					surf->on_mouse_leave(static_cast<int>(wParam), p);
				if(!is_mouse_over)
				{
					mouse_enter(static_cast<int>(wParam), p);
					if(surf->get_mouse_down() && !(((wParam & mouse_modifiers::l_button) == mouse_modifiers::l_button) || ((wParam & mouse_modifiers::r_button) == mouse_modifiers::r_button) || ((wParam & mouse_modifiers::m_button) == mouse_modifiers::m_button)) )
						surf->on_mouse_up(mouse_buttons::left, static_cast<int>(wParam), p);
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
		mouse_leave(static_cast<int>(wParam), point((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam)));
		break;
	case WM_SIZE: // wm_exitmovesize
		size_changed(get_size());
		layout();
		for(auto& surf : surfaces) surf->size_changed(surf->get_size());
		break;
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO* p = reinterpret_cast<MINMAXINFO*>(lParam);
			p->ptMaxTrackSize.x = static_cast<long>(maxsize.width == size::max_size().width ? GetSystemMetrics(SM_CXMAXTRACK) : maxsize.width);
			p->ptMaxTrackSize.y = static_cast<long>(maxsize.height == size::max_size().height ? GetSystemMetrics(SM_CYMAXTRACK) : maxsize.height);
			p->ptMinTrackSize.x = static_cast<long>(minsize.width);
			p->ptMinTrackSize.y = static_cast<long>(minsize.height);
			break;
		}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		handle_destroyed(hWnd);
		break;
	//case WM_DISPLAYCHANGE:
		
	case WM_DESTROY:
		release_resources();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}}
	catch(const exception&)
	{
		release_resources();
		throw;
	}
	return 0;
}

int window::show_dialog()
{
	time_t tm;
	time(&tm);
	wstring classname = L"GCL_hWndWrapp&" + to_wstring(tm);
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
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
	HWND hWnd = CreateWindowEx(exstyle, classname.c_str(), title.c_str(), styles, static_cast<int>(location.x), static_cast<int>(location.y), static_cast<int>(sizef.width), static_cast<int>(sizef.height), 0, 0, GetModuleHandle(NULL), this); 
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
	UpdateWindow(hWnd);
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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
	size_changed(sz); 
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
	if(val < 0 || val > 1) throw invalid_argument("Parameter \"val\" must be greater than 0 and lesser 1");
	SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(handle, 0, static_cast<BYTE>(255*val), LWA_ALPHA);
}

};
};