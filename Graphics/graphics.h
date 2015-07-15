/*****************************************************************************
*                           GCL - graphics.h                                 *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"
#include "callback.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef _MSC_VER 
#	pragma once
#endif

#define WM_GCL_CURSORCHANGED WM_USER + 0x1

namespace gcl {

namespace ui {class window;};
struct matrix;

static const float horizontal_leave_offset = 100, vertical_leave_offset = 100;

template <typename _src, typename _dst>
bool is_type(_src* source) {return (dynamic_cast<_dst*>(source) != 0);};
template <typename t>
wstring string_to_unicode_string(const basic_string<t>& _fpr, const locale& loc = locale())
{
	vector<wchar_t> buffer(_fpr.size());
	use_facet<ctype<wchar_t>>(loc).widen(_fpr.data(), _fpr.data()+_fpr.size(), buffer.data());
	return wstring(buffer.data(), buffer.size());
}
template <typename t>
string string_to_multibyte_string(const basic_string<t>& _fpr, const locale& loc = locale(), char replacement = '?')
{
	vector<char> buffer(_fpr.size());
	use_facet<ctype<char>>(loc).narrow(_fpr.data(), _fpr.data()+_fpr.size(), replacement, buffer.data());
	return string(buffer.data(), buffer.size());
}

wstring gcl_create_classname(const wstring& name);

template <typename f>
inline bool change_if_diff(f& prop, const f& val) {if(prop == val) return false; prop = val; return true;};
template <typename f>
inline bool change_if_diff(f* prop, f* val) {if(!val) return false; if(*prop == *val) return false; prop = val; return true;};

template <typename t>
bool is_between_equ(const t& val, const t& from, const t& till)
{
	return val >= from && val <= till;
}

template <typename t>
bool is_between(const t& val, const t& from, const t& till)
{
	return val > from && val < till;
}

template <typename t>
const t& extend(const t& value, const t& min, const t& max)
{
	if(value < min) return min;
	if(value > max) return max;
	return value;
}

template <typename func1, typename cl, typename func, typename ret, typename... args>
unsigned gcl_add_binding(cl* ptr, func fn_source, callback<ret(args...)>& callback_dest)
{
	callback_dest += gcl_bind<args...>(static_cast<func1*>(fn_source), ptr, placeholder::_1);
	return callback_dest.size();
}

// Defines a coordinate in 2D space
struct point
{
public:
	float x, y;
	point(float _x, float _y) : x(_x), y(_y) {}
	point() : x(0), y(0) {}

	point& operator= (const point& p);
	wstring to_wstring() const;
	friend wostream& operator<<(wostream& stream, const point& p)
	{
		stream << wstring(L"{") + std::to_wstring(p.x) + L", " + std::to_wstring(p.y) + L"}";
		return stream;
	}
	bool operator==(const point& p) const;
	bool operator!=(const point& p) const;
};

enum class ui_metrics {window_border = SM_CXBORDER, cursor = SM_CXCURSOR, dialog_frame = SM_CXDLGFRAME, window_3d_edge = SM_CXEDGE, window_fixed_border = SM_CXFIXEDFRAME, window_maximized = SM_CXMAXIMIZED, window_max = SM_CXMAXTRACK, 
						window_minimized = SM_CXMINIMIZED, nc_button = SM_CXSIZE, window_border_szframe = SM_CXSIZEFRAME};

// Defines a size (width & height) in 2D space
struct size
{
public:
	float width, height;
	size(float _width, float _height) : width(_width), height(_height) {}
	size(const ui_metrics& m); 
	static const size max_size();
	size() : width(0), height(0) {}
	point to_point() const;
	size& operator= (const size& sz);
	wstring to_wstring() const;
	friend wostream& operator<<(wostream& stream, const size& p)
	{
		stream << wstring(L"{") + std::to_wstring(p.width) + L", " + std::to_wstring(p.height) + L"}";
		return stream;
	}
	bool operator==(const size& sz) const;
	bool operator!=(const size& sz) const;
};

template <typename t>
point get_center(const t* inst)
{
	return point(inst->get_position().x + inst->get_size().width / 2.f, inst->get_position().y + inst->get_size().height/2.f);
}

// Defines a rectangle in 2D space
struct rect
{
public:
	point position;
	size sizef;
	rect() : position(point(0,0)), sizef(size(0,0)) {}
	rect(const point& p, const size& sz) : position(p), sizef(sz) {}
	rect(float x, float y, float width, float height) : position(x, y), sizef(width, height) {}
	static rect from_ltrb(float lft, float top, float rght, float bottom) {return rect(lft, top, rght-lft, bottom-top);}
	void inflate(float _width, float _height);
	void inflate(const size& sz);
	bool contains(const point& p) const;
	bool contains(const point& p, const matrix& m) const;
	rect get_bounds(const matrix& m) const;
	inline float get_x() const {return position.x;}
	inline float get_y() const {return position.y;}
	inline float get_width() const {return sizef.width;}
	inline float get_height() const {return sizef.height;}
	inline float get_left() const {return position.x;}
	inline float get_right() const {return position.x+sizef.width;}
	inline float get_top() const {return position.y;}
	inline float get_bottom() const {return position.y+sizef.height;}
	rect& operator= (const rect& sz);
	bool operator== (const rect& rc) const;
	bool operator!=(const rect& rc) const;
	wstring to_wstring() const;
	friend wostream& operator<<(wostream& stream, const rect& rc)
	{
		stream << L"{" << rc.position << L", " << rc.sizef << L"}";
		return stream;
	}
};

// Defines an ellipse in 2D space
struct ellipse // contains machen
{
public:
	point position;
	float radius_x;
	float radius_y;
	rect get_rect() const;
	ellipse(const point& p, float radiusx, float radiusy) : position(p), radius_x(radiusx), radius_y(radiusy) {}
	ellipse(float x, float y, float radiusx, float radiusy) : position(x, y), radius_x(radiusx), radius_y(radiusy) {}
	ellipse(const rect& rc) : position(static_cast<float>(rc.position.x + rc.sizef.width / 2.0f), static_cast<float>(rc.position.y + rc.sizef.height / 2.0f)), radius_x(static_cast<float>(rc.sizef.width / 2.0f)), radius_y(static_cast<float>(rc.sizef.height / 2.0)) {}
	ellipse() : position(0, 0), radius_x(0), radius_y(0) {}
	ellipse& operator=(const ellipse& sz);
	bool operator==(const ellipse& e) const;
};

// Defines a colour
struct colour
{
	typedef unsigned char uchar;
	typedef unsigned int uint;
public:
	enum known_colour
    {
        alice_blue = 0xF0F8FF,
        antique_white = 0xFAEBD7,
        aqua = 0x00FFFF,
        aquamarine = 0x7FFFD4,
        azure = 0xF0FFFF,
        beige = 0xF5F5DC,
        bisque = 0xFFE4C4,
        black = 0x000000,
        blanched_almond = 0xFFEBCD,
        blue = 0x0000FF,
        blue_violet = 0x8A2BE2,
        brown = 0xA52A2A,
        burly_wood = 0xDEB887,
        cadet_blue = 0x5F9EA0,
        chartreuse = 0x7FFF00,
        chocolate = 0xD2691E,
        coral = 0xFF7F50,
        cornflower_blue = 0x6495ED,
        cornsilk = 0xFFF8DC,
        crimson = 0xDC143C,
        cyan = 0x00FFFF,
        dark_blue = 0x00008B,
        dark_cyan = 0x008B8B,
        dark_goldenrod = 0xB8860B,
        dark_gray = 0xA9A9A9,
        dark_green = 0x006400,
        dark_khaki = 0xBDB76B,
        dark_magenta = 0x8B008B,
        dark_olive_green = 0x556B2F,
        dark_orange = 0xFF8C00,
        dark_orchid = 0x9932CC,
        dark_red = 0x8B0000,
        dark_salmon = 0xE9967A,
        dark_sea_green = 0x8FBC8F,
        dark_slate_blue = 0x483D8B,
        dark_slate_gray = 0x2F4F4F,
        dark_turquoise = 0x00CED1,
        dark_violet = 0x9400D3,
        deep_pink = 0xFF1493,
        deep_sky_blue = 0x00BFFF,
        dim_gray = 0x696969,
        dodger_blue = 0x1E90FF,
        firebrick = 0xB22222,
        floral_white = 0xFFFAF0,
        forest_green = 0x228B22,
        fuchsia = 0xFF00FF,
        gainsboro = 0xDCDCDC,
		gcl_gray = 0x3e3e42,
		gcl_back = 0x373738,
		gcl_menu_gray = 0x333333,
		gcl_dark_gray = 0x252526,
		gcl_front_gray = 0x686868,
		gcl_hot_gray = 0x9e9e9e,
		gcl_highlight_gray = 0xefebef,
		gcl_border = 0x007acc,
        ghost_white = 0xF8F8FF,
        gold = 0xFFD700,
        goldenrod = 0xDAA520,
        gray = 0x808080,
        green = 0x008000,
        green_yellow = 0xADFF2F,
        honeydew = 0xF0FFF0,
        hot_pink = 0xFF69B4,
        indian_red = 0xCD5C5C,
        indigo = 0x4B0082,
        ivory = 0xFFFFF0,
        khaki = 0xF0E68C,
        lavender = 0xE6E6FA,
        lavender_blush = 0xFFF0F5,
        lawn_green = 0x7CFC00,
        lemon_chiffon = 0xFFFACD,
        light_blue = 0xADD8E6,
        light_coral = 0xF08080,
        light_cyan = 0xE0FFFF,
        light_goldenrod_yellow = 0xFAFAD2,
        light_green = 0x90EE90,
        light_gray = 0xD3D3D3,
        light_pink = 0xFFB6C1,
        light_salmon = 0xFFA07A,
        light_sea_green = 0x20B2AA,
        light_sky_blue = 0x87CEFA,
        light_slate_gray = 0x778899,
        light_steel_blue = 0xB0C4DE,
        light_yellow = 0xFFFFE0,
        lime = 0x00FF00,
        lime_green = 0x32CD32,
        linen = 0xFAF0E6,
        magenta = 0xFF00FF,
        maroon = 0x800000,
        medium_aquamarine = 0x66CDAA,
        medium_blue = 0x0000CD,
        medium_orchid = 0xBA55D3,
        medium_purple = 0x9370DB,
        medium_sea_green = 0x3CB371,
        medium_Slate_blue = 0x7B68EE,
        medium_spring_green = 0x00FA9A,
        medium_turquoise = 0x48D1CC,
        medium_violet_red = 0xC71585,
        midnight_blue = 0x191970,
        mint_cream = 0xF5FFFA,
        misty_rose = 0xFFE4E1,
        moccasin = 0xFFE4B5,
        navajo_white = 0xFFDEAD,
        navy = 0x000080,
        old_lace = 0xFDF5E6,
        olive = 0x808000,
        olive_drab = 0x6B8E23,
        orange = 0xFFA500,
        orange_red = 0xFF4500,
        orchid = 0xDA70D6,
        pale_goldenrod = 0xEEE8AA,
        pale_green = 0x98FB98,
        pale_turquoise = 0xAFEEEE,
        pale_violet_red = 0xDB7093,
        papaya_whip = 0xFFEFD5,
        peach_puff = 0xFFDAB9,
        peru = 0xCD853F,
        pink = 0xFFC0CB,
        plum = 0xDDA0DD,
        powder_blue = 0xB0E0E6,
        purple = 0x800080,
        red = 0xFF0000,
        rosy_brown = 0xBC8F8F,
        royal_blue = 0x4169E1,
        saddle_brown = 0x8B4513,
        salmon = 0xFA8072,
        sandy_brown = 0xF4A460,
        sea_green = 0x2E8B57,
        sea_shell = 0xFFF5EE,
        sienna = 0xA0522D,
        silver = 0xC0C0C0,
        sky_blue = 0x87CEEB,
        slate_blue = 0x6A5ACD,
        slate_gray = 0x708090,
        snow = 0xFFFAFA,
        spring_green = 0x00FF7F,
        steel_blue = 0x4682B4,
        tan = 0xD2B48C,
        teal = 0x008080,
        thistle = 0xD8BFD8,
        tomato = 0xFF6347,
        turquoise = 0x40E0D0,
        violet = 0xEE82EE,
        wheat = 0xF5DEB3,
        white = 0xFFFFFF,
        white_smoke = 0xF5F5F5,
        yellow = 0xFFFF00,
        yellow_green = 0x9ACD32
    };
	enum system_colour
	{
		dark_shadow_3d = 21,
		face_3d = 15,
		highlight_3d = 20,
		light_3d = 22,
		shadow_3d = 16,
		active_border = 10,
		active_caption = 2,
		app_workspace = 12,
		background = 1,
		button_face = 15,
		button_highlight = 20,
		button_shadow = 16,
		button_text = 18,
		caption_text = 9,
		desktop = 1,
		gradient_active_caption = 27,
		gradient_inactive_caption = 28,
		gray_text = 17,
		highlight = 13,
		highlight_text = 14,
		hotlight = 26,
		inactive_border = 11,
		inactive_caption = 3,
		inactive_captiontext = 19,
		info_background = 24,
		info_text = 23,
		menu = 4,
		menu_highlight = 29,
		menu_bar= 30,
		menu_text = 7,
		scrollbar = 0,
		window = 5,
		window_frame = 6,
		window_text = 8
	};
	uchar a, r, g, b;
	uint get_value() const;
	colour() : a(255), r(0), g(0), b(0) {};
	colour(uchar r, uchar g, uchar b, uchar a = 255) : a(a), r(r), g(g), b(b) {};
	colour(uchar a, const colour& c) : a(a), r(c.r), g(c.g), b(c.b) {};
	colour(uint value) : a((value & 0xff000000) >> 24), r((value & 0x00ff0000) >> 16), g((value & 0x0000ff00) >> 8), b(value & 0x000000ff) {};
	colour(uchar a, uint value) : a(a), r((value & 0xff0000) >> 16), g((value & 0x00ff00) >> 8), b(value & 0x0000ff) {};
	colour(known_colour c) : a(255), r(((uint)c & 0xff0000) >> 16), g(((uint)c & 0x00ff00) >> 8), b(uint(c) & 0x0000ff) {};
	colour(uchar a_, known_colour c) : a(a_), r(((uint)c & 0xff0000) >> 16), g(((uint)c & 0x00ff00) >> 8), b(uint(c) & 0x0000ff) {};
	colour(system_colour c);
	colour(uchar _a, system_colour c);
	colour& operator= (const colour& c);
	bool operator==(const colour& c) const;
	bool operator!=(const colour& c) const;
	colour opacity(float fac) const;
	colour invert(bool alpha);
	void differ(uchar value);
	bool operator==(const colour& c);
};

template <typename ip, typename...>
struct are_same;

template <typename ip, typename T>
struct are_same<ip, T>
{
public:
	static const bool value = is_same<ip, T>::value;
};

template <typename ip, typename T, typename... Args>
struct are_same<ip, T, Args...>
{
public:
	static const bool value = are_same<ip, T>::value && are_same<ip, Args...>::value;
};

template <typename t>
const t& gcl_min(const t& arg0, const t& arg1) {return arg0 < arg1 ? arg0 : arg1;}

template <typename t_, typename... t>
typename enable_if<are_same<t_, t...>::value, const t_&>::type  gcl_min(const t_& arg0, const t_& arg1, const t&... args)
{
	const t_& sm = arg0 < arg1 ? arg0 : arg1;
	return gcl_min(sm, args...);
}

template <typename t>
const t& gcl_max(const t& arg0, const t& arg1) {return arg0 > arg1 ? arg0 : arg1;}

template <typename t_, typename... t>
typename enable_if<are_same<t_, t...>::value, const t_&>::type  gcl_max(const t_& arg0, const t_& arg1, const t&... args)
{
	const t_& sm = arg0 > arg1 ? arg0 : arg1;
	return gcl_max(sm, args...);
}

template <typename item, unsigned int n>
class collection
{
public:
	typedef item* iterator;
	template <typename t_a, typename = enable_if<is_same<item, t_a>::value && n==1>::type>
	explicit collection(const t_a& arg1) : cnt(0) {add(arg1);};
	
	template <typename t_a, typename... _prm, typename = enable_if<is_same<item, t_a>::value && are_same<item, _prm...>::value && (n==sizeof...(_prm)+1)>::type>
	collection(const t_a& arg1, const _prm&... args) : cnt(0) {add(arg1); add(args...);};
	collection(item ptv_coll[n]) {for(unsigned int i = 0; i < n; i++) coll[i] = ptv_coll[i];};
	iterator begin() {return coll;};
	iterator end() {return &coll[n];};
	static const int count = n;

	float operator[](unsigned int idx) const {return coll[idx];};
	collection(const collection<item, n>&) = delete;
private:
	void add() {};
	template <typename f, typename... f_u>
	void add(const f& ifr, const f_u&... ufr) {coll[cnt++] = ifr; add(ufr...);};
	int cnt;
	item coll[n];
};

// Defines a 3x2matrix for a 2D-transformation
// |	ScaleX,		ShearX		|
// |	ShearY,		ScaleY		|
// |	TranslateX, TranslateY	|
struct matrix
{
public:
	float m11, m12, m21, m22, m31, m32;

	matrix() : m11(1), m12(0), m21(0), m22(1), m31(0), m32(0) {};
	matrix(float _m11, float _m12, float _m21, float _m22, float _m31, float _m32) : m11(_m11), m12(_m12), m21(_m21), m22(_m22), m31(_m31), m32(_m32) {};

	static matrix identity();
	static bool is_identity(const matrix& m);
	bool is_identity() const;
	void reset();

	bool operator==(const matrix& m) const;
	bool operator!=(const matrix& m) const;
	matrix operator~() const;
	matrix operator*(const matrix& m);
	matrix operator*(float scalar);
	matrix operator/(const matrix& m);
	matrix operator/(float scalar);
	void transform_points(point* pts, unsigned int count = 1) const;
	void transform_vectors(point* pts, unsigned int count = 1) const;
	void invert();
	static matrix rotation(float angle);
	void rotate(float angle);
	static matrix rotation(float angle, const point& p);
	void rotate(float angle, const point& p);
	static matrix translation(float x, float y);
	void translate(float x, float y);
	static matrix scalation(float fac_x, float fac_y);
	void scale(float fac_x, float fac_y);
	static matrix shearing(float fac_x, float fac_y);
	void shear(float fac_x, float fac_y);
	static matrix skewing(float x, float y, float angle_x, float angle_y);
	void skew(float x, float y, float angle_x, float angle_y);
	static matrix mirroring_x();
	static matrix mirroring_x(float mirror_axis);
	void mirror_x();
	void mirror_x(float mirror_axis);
	static matrix mirroring_y();
	static matrix mirroring_y(float mirror_axis);
	void mirror_y();
	void mirror_y(float mirror_axis);
	float get_determinant() const;
	bool is_invertible() const {return get_determinant() != 0;}
private:
	inline float determinant(float m_11, float m_12, float m_21, float m_22) {return m_11*m_22 - m_12*m_21;}
};

namespace clipboard
{
	typedef HANDLE generic_data;
	void clear();
	void open(HWND wind);
	void close();
	HWND get_owner();
	bool is_format_available(UINT format);
	generic_data get_data(UINT format);
	UINT register_format(const wstring& frmt);
	template <typename dest>
	dest get_data(UINT format)
	{
		auto data = GetClipboardData(format);
		if(!data)
			throw runtime_error("Invalid data");
		return reinterpret_cast<dest>(data);
	}
	vector<UINT> get_available_formats();
	void set_data(UINT format, generic_data data);
	IDataObject* ole_get_data();
	void ole_set_data(IDataObject* data_object);
	void ole_flush();
};

enum class resizing_types {max_hide = SIZE_MAXHIDE, maximized = SIZE_MAXIMIZED, max_show = SIZE_MAXSHOW, minimized = SIZE_MINIMIZED, restored = SIZE_RESTORED};

namespace render_objects {

enum class brush_types {solid_brush, texture_brush, linear_gradient_brush, radial_gradient_brush};
enum class fill_modes {alternate, winding};
enum class wrap_modes {wrap, mirror_x, mirror_y, mirror_xy, clamp};

// Pen dependencies
enum class dash_cap {flat = 0, round = 2, triangle = 3};
enum class dash_style {dash = 1, dash_dot = 3, dash_dot_dot = 4, dot = 2, solid = 0}; 
//
// Font
namespace font_style {const int regular = 0, bold = 1, italic = 2;};
enum class string_format {direction_left_to_right, direction_right_to_left};
enum class horizontal_string_align {left, middle, right};
enum class vertical_string_align {top, middle, bottom};
//

class graphics;

class font
{
public:
	virtual ~font() {};
	virtual int get_style() const = 0;
	virtual float get_size() const = 0;
	virtual wstring get_family() const = 0;
	virtual bool operator==(font* f) = 0;
	virtual rect get_metrics(const wstring& str, const size& clip, graphics* g) const = 0;
	virtual vector<wstring> get_available_font_families() = 0;
};

class texture
{
public:
	virtual ~texture() {};
	virtual int get_width() const = 0;
	virtual int get_height() const = 0;
	virtual texture* clone() = 0;
	virtual unsigned char* alloc() = 0;
	virtual void free() = 0;
	// Returns the stride (Width*BitsPerpixel) after the texture has been locked
	virtual int get_stride() = 0;
};

enum class system_icon {app = 32512, error = 32513, info = 32516, question = 32514, shield = 32518, warning = 32515, win_logo = 32517};
enum class oem_bitmap {close = OBM_CLOSE, up_arrow = OBM_UPARROW, down_arrow = OBM_DNARROW, right_arrow = OBM_RGARROW, left_arrow = OBM_LFARROW, 
reduce = OBM_REDUCE, zoom = OBM_ZOOM, restore = OBM_RESTORE, reduced = OBM_REDUCED, zoomed = OBM_ZOOMD, restored = OBM_RESTORED, up_arrowd = OBM_UPARROWD, down_arrowd = OBM_DNARROWD, right_arrowd = OBM_RGARROWD, left_arrowd = OBM_LFARROWD, 
mn_arrow = OBM_MNARROW, combo = OBM_COMBO, up_arrowi = OBM_UPARROWI, down_arrrowi = OBM_DNARROWI, right_arrowi = OBM_RGARROWI, left_arrowi = OBM_RGARROWI, old_close = OBM_OLD_CLOSE, size = OBM_SIZE, old_up_arrow = OBM_OLD_UPARROW, 
old_down_arrow = OBM_OLD_DNARROW, old_right_arrow = OBM_OLD_RGARROW, old_left_arrow = OBM_OLD_LFARROW, button_size = OBM_BTSIZE, check = OBM_CHECK, checkboxes = OBM_CHECKBOXES, button_corners = OBM_BTNCORNERS, old_reduce = OBM_OLD_REDUCE, old_zoom = OBM_OLD_ZOOM, old_restore = OBM_OLD_RESTORE};

class icon
{
public:
	virtual ~icon() {};
	virtual HICON get_icon() const = 0;
	virtual size get_size() const = 0;
	static size get_small_icon_size();
	static size get_icon_size();
};

enum class system_cursor {app_starting = OCR_APPSTARTING, arrow = OCR_NORMAL, cross = OCR_CROSS, hand = OCR_HAND, help = 32651, i_beam = OCR_IBEAM, no = OCR_NO, size_all = OCR_SIZEALL, size_nesw = OCR_SIZENESW,
size_ns = OCR_SIZENS, size_nwse = OCR_SIZENWSE, size_we = OCR_SIZEWE, up_arrow = OCR_UP, wait = OCR_WAIT, pen = 32631, wait_cd = 32663};

class cursor_surface
{
public:
	cursor_surface();
	cursor_surface(HCURSOR cur_);
	cursor_surface(const system_cursor& sys_cur);
	cursor_surface(const cursor_surface&& cs) : cur(cs.get_cursor()) {}
	cursor_surface(HINSTANCE inst, int id);
	cursor_surface(const wstring& filename);
	~cursor_surface();
	HCURSOR get_cursor() const;
private:
	int count;
	HCURSOR cur;
};

class brush 
{
public:
	brush() {};
	virtual ~brush() {};
	virtual brush_types get_type() = 0;
	virtual void set_transform(const matrix& m) = 0;
	virtual matrix get_transform() const = 0;
	virtual float get_opacity() const = 0;
	virtual void set_opacity(float val) = 0;
};

class solid_brush : 
	public brush
{
public:
	virtual ~solid_brush() {};
	virtual colour get_colour() = 0;
	virtual void set_colour(const colour& c) = 0;
	virtual void set_transform(const matrix& m) {};
	virtual matrix get_transform() const {return matrix::identity();};
};

class texture_brush : 
	public brush
{
public:
	virtual ~texture_brush() {};
	virtual texture* get_texture() const = 0;
	virtual void set_wrap_mode(const wrap_modes& mode) = 0;
	virtual wrap_modes get_wrap_mode() const = 0;
};

template <unsigned int n>
using coll_f = collection<float, n>;

template <unsigned int n>
using coll_c = collection<colour, n>;

// A fixxed collection of floats and colours
class gradient_stop
{
public:
	template <unsigned int n>
	explicit gradient_stop(coll_f<n>& positions, coll_c<n>& colors) : count(n) {f = positions.begin(); c = colors.begin();};
	unsigned int get_count() const {return count;};
	float* get_positions() {return f;};
	colour* get_colours() {return c;};
private:
	float* f;
	colour* c;
	unsigned int count;
};


class linear_gradient_brush : 
	public brush
{
public:
	virtual ~linear_gradient_brush() {};
	virtual void set_rect(const point& p1, const point& p2) = 0;
	virtual void set_rect(const rect& rc) {set_rect(rc.position, rc.sizef.to_point());};
	virtual rect get_rect() const = 0;
	virtual gradient_stop get_gradients() const = 0;
	virtual void set_gradients(gradient_stop& gradients) = 0;
};

class radial_gradient_brush : 
	public brush
{
public:
	virtual ~radial_gradient_brush() {};
	virtual void set_ellipse(const ellipse& e) = 0;
	virtual ellipse get_ellipse() const = 0;
	virtual void set_gradient_origin_offset(const point& p)  = 0;
	virtual point get_gradient_origin_offset() const = 0;
	virtual gradient_stop get_gradients() const = 0;
	virtual void set_gradients(gradient_stop& gradients) = 0;
};

class pen
{
public:
	virtual ~pen() {};
	virtual brush* get_brush() const = 0;
	// Updates the pen when its brush changed
	virtual void update() = 0;
	virtual dash_cap get_start_cap() const = 0;
	virtual void set_start_cap(const dash_cap& p) = 0;
	virtual dash_cap get_end_cap() const = 0;
	virtual void set_end_cap(const dash_cap& p) = 0;
	virtual dash_cap get_dash_cap() const = 0;
	virtual void set_dash_cap(const dash_cap& c) = 0;
	virtual dash_style get_dash_style() const = 0;
	virtual void set_dash_style(const dash_style& d) = 0;
	virtual float get_dash_offs() const = 0;
	virtual void set_dash_offs(float f) = 0;
	virtual float get_width() const = 0;
	virtual void set_width(float f) = 0;
};

class geometry
{
public:
	virtual ~geometry() {};
	virtual void begin_geometry(const point& p) = 0;
	virtual void end_geometry() = 0;
	virtual bool contains(const point& p, const matrix& m = matrix::identity()) = 0; 
	virtual bool outline_contains(const point& p, pen* pe, const matrix& m = matrix::identity()) = 0;

	virtual void add_polygon(const point* ps, int count) = 0;
	virtual void add_bezier(const point& p1, const point& p2, const point& p3) = 0;
	virtual void add_beziers(const point* ps, int count) = 0;
	//virtual void add_curve(const point* ps, int count, float tension) = 0;
	virtual void add_line(const point& p1) = 0;
	virtual void add_lines(const point* ps, int count) = 0;
	virtual void add_rect(const rect& rc) = 0;
	virtual void add_ellipse(const ellipse& e) = 0;
	virtual void add_rounded_rect(const rect& rc, float radiusX, float radiusY) = 0;
	virtual void add_geometry(geometry* geo) = 0;
	virtual rect get_bounds(const matrix& transform) const = 0;
};

class clip
{
public:
	clip(const rect& area);
	clip(geometry* area);
	clip(const clip&&);

	bool is_rectangle_clip() const {return is_rectangular;}
	rect get_rect() const;
	geometry* get_geometry() const;
private:
	bool is_rectangular;
	rect rc;
	geometry* geo;
};

enum class text_rendering_modes {system_default, antialias, cleartype};
enum class graphics_type {dc, handle, texture};

class graphics
{
public:
	virtual ~graphics(void) {};

	virtual void begin() = 0;
	virtual void clear(const colour& c)= 0;
	virtual void end() = 0;
	virtual void bind_dc(HDC dc) = 0;

	virtual void fill_rect(const rect& rc, brush* b) = 0;
	virtual void fill_rects(const rect* rcs, int count, brush* b) = 0;
	virtual void fill_ellipse(const ellipse& e, brush* b) = 0;
	virtual void fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b) = 0;
	virtual void fill_geometry(geometry* geo, brush* b) = 0;
	virtual void fill_polygon(const point* ps, int count, brush* b) = 0; 

	virtual void draw_ellipse(const ellipse& e, pen* p) = 0;
	virtual void draw_line(const point& p1, const point& p2, pen* p) = 0;
	virtual void draw_lines(const point* ps, int count, pen* p) = 0;
	virtual void draw_rect(const rect rc, pen* p) = 0;
	virtual void draw_rects(const rect* rcs, int count, pen* p) = 0;
	virtual void draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p) = 0;
	virtual void draw_polygon(const point* ps, int count, pen* p) = 0;
	virtual void draw_geometry(geometry* geo, pen* p) = 0;
	virtual void draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format = string_format::direction_left_to_right) = 0;
	virtual void draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format = string_format::direction_left_to_right, const horizontal_string_align& cliping_h = horizontal_string_align::left, const vertical_string_align& clipping_v = vertical_string_align::top) = 0;
	virtual void draw_texture(texture* image, const point& p, unsigned char opacity = 255) = 0;
	virtual void draw_texture(texture* image, const point& p, const size& sz, unsigned char opacity = 255) = 0;
	virtual void draw_texture(texture* image, const rect& rc, unsigned char opacity = 255) = 0;
	virtual void draw_texture(texture* image, const rect& src, const point& dst, unsigned char opacity = 255) = 0;
	virtual void draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest, unsigned char opacity = 255) = 0;
	virtual void draw_texture(texture* image, const rect& src, const rect& dest, unsigned char opacity = 255) = 0;
	virtual void draw_icon(icon* ico, const point& pos) = 0;
	virtual void draw_icon(icon* ico, const point& pos, const size& sz) = 0;

	virtual solid_brush* create_solid_brush(const colour& c) = 0;
	virtual texture_brush* create_texture_brush(texture* image) = 0;
	virtual linear_gradient_brush* create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma = false, const wrap_modes& wrapmode = wrap_modes::wrap) = 0;
	virtual pen* create_pen(brush* b, float width = 1.0f, dash_cap startcap = dash_cap::flat, dash_cap endcap = dash_cap::flat, dash_cap dash_cap = dash_cap::flat, dash_style dash_style = dash_style::solid, float offset = 0.0f) = 0;
	virtual texture* create_texture(const wstring& filename) = 0;
	virtual texture* create_texture(int id, LPWSTR type, HINSTANCE inst) = 0;
	virtual texture* create_texture(const size& sz) = 0;
	virtual texture* create_texture(HBITMAP hbmp, HPALETTE hP = 0) = 0;
	virtual texture* create_texture(HICON ico) = 0;
	virtual texture* create_texture(const oem_bitmap& bmp) = 0;
	virtual icon* create_icon(const wstring& filename) = 0;
	virtual icon* create_icon(HICON ico) = 0;
	virtual icon* create_icon(const system_icon& ico) = 0;
	virtual icon* create_icon(int id, HINSTANCE inst) = 0;
	virtual icon* create_icon(const wstring& filename, const size& sz) = 0;
	virtual icon* create_icon(int id, HINSTANCE inst, const size& sz) = 0;
	virtual font* create_font(const wstring& family_name, float size, int fstyle = font_style::regular) = 0;
	virtual geometry* create_geometry() = 0;
	//virtual radial_gradient_brush* create_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma = false) = 0;
	virtual font* get_system_font(float sz = 12.66f, int fstyle = font_style::regular) const = 0;
	virtual graphics* create_graphics(HWND handle, callback<void(const size&, const resizing_types&)>& cb) = 0;
	// Region does not get deleted
	virtual graphics* create_graphics(HDC dc) = 0;
	virtual graphics* create_graphics(texture* txt) = 0;

	virtual void rotate(float angle) = 0;
	virtual void rotate_at(float angle, const point& p) = 0;
	virtual void translate(float x, float y) = 0;
	virtual void scale(float fac_x, float fac_y) = 0;
	virtual void shear(float fac_x, float fac_y) = 0;
	virtual void skew(float x, float y, float angle_x, float angle_y) = 0;
	virtual matrix get_transform() const = 0;
	virtual void set_transform(const matrix& m) = 0;

	virtual void set_text_rendering_mode(const text_rendering_modes& mode) = 0;
	virtual text_rendering_modes get_text_rendering_mode() const = 0;
	virtual void set_antialias(bool val) = 0;
	virtual bool get_antialias() const = 0;
	virtual void push_clip(const clip& cl) = 0;
	virtual void pop_clip() = 0;
protected:
	virtual void create_resources() = 0;
};

};

class dynamic_drawsurface;
enum class z_layer {bottom, top, top_most};
struct margin 
{
public:
	float left, right, top, bottom;
	margin(float _left, float _right, float _top, float _bottom) : left(_left), right(_right), top(_top), bottom(_bottom) {};
	margin() : left(0), right(0), top(0), bottom(0) {};
	bool operator==(const margin& m)
	{
		return (left == m.left && right == m.right && top == m.top && bottom == m.bottom);
	};
};
typedef margin padding;
enum class mouse_buttons {left, middle, right};
namespace mouse_modifiers { const int control = 0x8, l_button = 0x1, m_button = 0x10, r_button = 0x2, shift = 0x4, x_button_1 = 0x20, x_button_2 = 0x40;};
enum class virtual_keys {
lbutton   =   0x01,
rbutton   =   0x02,
cancel   =    0x03,
mbutton   =   0x04,
xbutton1   =    0x05,
xbutton2   =    0x06,
back   =      0x08,
tab   =       0x09,
clear   =     0x0c,
_return   =    0x0d,
shift   =     0x10,
control   =   0x11,
menu   =      0x12,
pause   =     0x13,
capital   =   0x14,
kana   =      0x15,
hangeul   =   0x15,
hangul   =    0x15,
junja   =     0x17,
final   =     0x18,
hanja   =     0x19,
kanji   =     0x19,
escape   =    0x1b,
convert   =   0x1c,
nonconvert  =   0x1d,
accept   =    0x1e,
modechange  =   0x1f,
space   =     0x20,
prior   =     0x21,
next   =      0x22,
end   =       0x23,
home   =      0x24,
left   =      0x25,
up   =        0x26,
right   =     0x27,
down   =      0x28,
select   =    0x29,
key_0	=	0x30,
key_1	=	0x31,
key_2	=	0x32,
key_3	=	0x33,
key_4	=	0x34,
key_5	=	0x35,
key_6	=	0x36,
key_7	=	0x37,
key_8	=	0x38,
key_9	=	0x39,
key_a	=	0x41,
key_b	=	0x42,
key_c	=	0x43,
key_d	=	0x44,
key_e	=	0x45,
key_f	=	0x46,
key_g	=	0x47,
key_h	=	0x48,
key_i	=	0x49,
key_j	=	0x4a,
key_k	=	0x4b,
key_l	=	0x4c,
key_m	=	0x4d,
key_n	=	0x4e,
key_o	=	0x4f,
key_p	=	0x50,
key_q	=	0x51,
key_r	=	0x52,
key_s	=	0x53,
key_t	=	0x54,
key_u	=	0x55,
key_v	=	0x56,
key_w	=	0x57,
key_x	=	0x58,
key_y	=	0x59,
key_z	=	0x5a,
print   =     0x2a,
execute   =   0x2b,
snapshot   =    0x2c,
insert   =    0x2d,
_delete   =    0x2e,
help   =      0x2f,
lwin   =      0x5b,
rwin   =      0x5c,
apps   =      0x5d,
sleep   =     0x5f,
numpad0   =   0x60,
numpad1   =   0x61,
numpad2   =   0x62,
numpad3   =   0x63,
numpad4   =   0x64,
numpad5   =   0x65,
numpad6   =   0x66,
numpad7   =   0x67,
numpad8   =   0x68,
numpad9   =   0x69,
multiply   =    0x6a,
add   =       0x6b,
separator   =   0x6c,
subtract    =   0x6d,
decimal   =   0x6e,
divide   =    0x6f,
f1   =        0x70,
f2   =        0x71,
f3   =        0x72,
f4   =        0x73,
f5   =        0x74,
f6   =        0x75,
f7   =        0x76,
f8   =        0x77,
f9   =        0x78,
f10   =       0x79,
f11   =       0x7a,
f12   =       0x7b,
f13   =       0x7c,
f14   =       0x7d,
f15   =       0x7e,
f16   =       0x7f,
f17   =       0x80,
f18   =       0x81,
f19   =       0x82,
f20   =       0x83,
f21   =       0x84,
f22   =       0x85,
f23   =       0x86,
f24   =       0x87,
numlock   =   0x90,
scroll   =    0x91,
oem_nec_equal = 0x92,
oem_fj_jisho  = 0x92,
oem_fj_masshou = 0x93,
oem_fj_touroku = 0x94,
oem_fj_loya  =  0x95,
oem_fj_roya  =  0x96,
lshift   =    0xa0,
rshift   =    0xa1,
lcontrol   =    0xa2,
rcontrol   =    0xa3,
lmenu   =     0xa4,
rmenu   =     0xa5,
browser_back   =   0xa6,
browser_forward  =   0xa7,
browser_refresh  =   0xa8,
browser_stop   =   0xa9,
browser_search   =   0xaa,
browser_favorites =  0xab,
browser_home   =   0xac,
volume_mute   =    0xad,
volume_down   =    0xae,
volume_up   =      0xaf,
media_next_track  =  0xb0,
media_prev_track  =  0xb1,
media_stop   =     0xb2,
media_play_pause  =  0xb3,
launch_mail   =    0xb4,
launch_media_select = 0xb5,
launch_app1   =    0xb6,
launch_app2   =    0xb7,
oem_1   =     0xba,
oem_plus    =   0xbb,
oem_comma   =   0xbc,
oem_minus   =   0xbd,
oem_period  =   0xbe,
oem_2   =     0xbf,
oem_3   =     0xc0,
oem_4   =     0xdb,
oem_5   =     0xdc,
oem_6   =     0xdd,
oem_7   =     0xde,
oem_8   =     0xdf,
oem_ax   =    0xe1,
oem_102   =   0xe2,
ico_help   =    0xe3,
ico_00   =    0xe4,
processkey  =   0xe5,
ico_clear   =   0xe6,
packet   =    0xe7,
oem_reset   =   0xe9,
oem_jump    =   0xea,
oem_pa1   =   0xeb,
oem_pa2   =   0xec,
oem_pa3   =   0xed,
oem_wsctrl  =   0xee,
oem_cusel   =   0xef,
oem_attn    =   0xf0,
oem_finish  =   0xf1,
oem_copy    =   0xf2,
oem_auto    =   0xf3,
oem_enlw    =   0xf4,
oem_backtab  =  0xf5,
attn   =      0xf6,
crsel   =     0xf7,
exsel   =     0xf8,
ereof   =     0xf9,
play   =      0xfa,
zoom   =      0xfb,
noname   =    0xfc,
pa1   =       0xfd,
oem_clear   =   0xfe};

struct key_extended_params
{
	uint16_t repeat_count;
	char scan_code;
	bool extended_key;
	bool context_code;
	bool previous_state;
	bool transition_state;
};

// Layout enums and references
enum class horizontal_align {left, center, right, stretch};
enum class vertical_align {top, center, bottom, stretch};
enum class horizontal_scroll_align {top, bottom};
enum class vertical_scroll_align {left, right};
enum class drawing_state {normal, pressed, hot};
//

namespace ui {
using namespace render_objects;
class context_menu;
class menu;

class menu_graphics
{
public:
	virtual ~menu_graphics() {}
	callback<void(const colour&)> front_colour_changed, grayed_colour_changed, hot_colour_changed, back_colour_changed;
	graphics* get_graphics() const {return window_graphics.get();}
	font* get_title_font() const {return title_font.get();}
	solid_brush* get_brush() const {return highlight_brush.get();}
	solid_brush* get_hot_brush() const {return hot_brush.get();}
	solid_brush* get_grayed_brush() const {return grayed_brush.get();}
	virtual size get_hsize() const = 0;
	pen* get_pen() const {return seperator_pen.get();}
	virtual void int_redraw(rect rc) = 0;
	virtual HWND get_howner() const {return owner;}
	virtual void int_redraw() = 0;
	virtual void close(bool parent) = 0;
	virtual bool is_focused() const = 0;
	HWND get_hhandle() const {return handle;}
	virtual menu_graphics* get_parent() const = 0;
	bool get_child_shown() const {return child_shown;}
	void _set_child_shown(bool b) {child_shown = false;}

	colour get_front_colour() const {return cl_hi;}
	void set_front_colour(const colour& cl);
	colour get_back_colour() const {return cl_back;}
	void set_back_colour(const colour& cl);
	colour get_hot_colour() const {return cl_hot;}
	void set_hot_colour(const colour& cl);
	colour get_grayed_colour() const {return cl_gray;}
	void set_grayed_colour(const colour& cl);
	static bool is_high_contrast();
protected:
	shared_ptr<font> title_font;
	bool child_shown;
	HWND owner, handle;
	shared_ptr<solid_brush> highlight_brush, grayed_brush, hot_brush;
	colour cl_hi, cl_gray, cl_hot, cl_back;
	shared_ptr<pen> seperator_pen;
	shared_ptr<graphics> window_graphics;
};

class menu_strip :
	public menu_graphics
{
friend class context_menu;
friend class menu;
public:
	menu_strip();
	virtual ~menu_strip();
	// void(int index)
	callback<void(int)> click;
	callback<void(const point&)> shown;
	callback<void(const size&, const resizing_types&)> size_changed;
	virtual void render(graphics* g, drawing_state draw_state, const point& origin);

	// Call init() to initialize the menu_strip if it'll have childs
	void init(menu_graphics* parent);
	void set_height(float v);
	float get_height() const {return max(height, icon::get_small_icon_size().height+4);}
	void set_checked(bool b);
	bool get_checked() const {return checked;}
	void set_checkable(bool b);
	bool get_checkable() const {return checkable;}
	void set_enabled(bool b);
	bool get_enabled() const {return enabled;}
	void set_title(const wstring& text);
	wstring get_title() const {return title;}
	void set_top_seperator(bool b);
	bool get_top_seperator() const {return seperator_top;}
	void set_bottom_seperator(bool b);
	bool get_bottom_seperator() const {return seperator_bottom;}
	texture* get_image() const {return image.get();}
	void set_image(texture* img);
	void int_redraw(rect rc);
	void int_redraw();
	size get_hsize() const;
	void add_strip(menu_strip* strip);
	HWND get_howner() const {return p ? p->get_howner() : 0;}
	bool is_focused() const {return focus;}
	menu_graphics* get_parent() const {return p;}
	menu_strip* get_strip(int index) {return childs[index];}
	menu_strip* operator[](int idx) {return childs[idx];}
	int get_strip_count() const {return childs.size();}
protected:
	menu_graphics* get_owning_window() const {return p;}
	bool test_handle(HWND) const;
	// Overwrite for ownerdrawing
	virtual void create_resources(graphics* g) {hs_resources = true;}
	virtual void on_syscolour_changed();
	virtual LRESULT message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	bool focus;
	bool hs_childs;
	bool hs_resources;
	float arrow_size;
	void render_childs(graphics* gr);
	float height;
	vector<menu_strip*> childs;
	menu_graphics* p;
	bool checked, checkable;
	bool enabled;
	bool seperator_top, seperator_bottom;
	float seperator_width;
	wstring title;
	point* get_hook(float x, float y);
	shared_ptr<texture> image;
	wstring classname;
	int hidx;
	void update_resources();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool mouse_over, mouse_down;
	void show(const point& p, bool key);
	void close(bool parent);
	unsigned showed_idx;
};

class context_menu :
	public menu_graphics
{
public:
	context_menu(HWND owner, graphics* owner_graphics);
	virtual ~context_menu();
	callback<void(const size&, const resizing_types&)> size_changed;
	callback<void(const point&)> shown;
	callback<void()> closed;

	virtual void show(const point& p, int custom_width = 0); 
	virtual void render(graphics* g);
	void int_redraw(rect rc);
	void int_redraw();
	void add_strip(menu_strip* strip);
	void remove_strip(menu_strip* strip);
	void clear_strips();
	menu_strip* get_strip(int index) {return childs[index];}
	menu_strip* operator[](int idx) {return childs[idx];}
	int get_strip_count() const {return childs.size();}
	size get_hsize() const;
	HWND get_hhandle() {return handle;}
	void close(bool parent);
	bool is_focused() const {return focus;}
	menu_graphics* get_parent() const {return 0;}
	bool is_shown() const {return opened;}
protected:
	vector<menu_strip*> childs;
	virtual void on_syscolour_changed();
	colour font_colour;
	bool test_handle(HWND);
	virtual LRESULT message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	bool focus;
	int hidx;
	bool opened;
	wstring classname;
	bool mouse_over;
	float child_arrow;
	bool mouse_down;
	unsigned showed_idx;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

};

namespace dragdrop {class drop_target; enum class drop_effects : DWORD {none = DROPEFFECT_NONE, copy = DROPEFFECT_COPY, move = DROPEFFECT_MOVE, link = DROPEFFECT_LINK, scroll = DROPEFFECT_SCROLL};};

class drawsurface
{
friend class dragdrop::drop_target;
public:
	drawsurface() : visible(true), _dragdrop(false) {};
	virtual ~drawsurface() {};
	callback<void(const wstring&)> title_changed;
	callback<void(const dynamic_drawsurface*)> surface_added;
	callback<void(const dynamic_drawsurface*)> surface_removed;
	callback<void(const size&, const resizing_types&)> size_changed;
	callback<void(bool)> visible_changed;
	callback<void(const point&)> position_changed;
	callback<void(const size&)> min_size_changed, max_size_changed;
	callback<void()> layouted;
	callback<void(const padding&)> padding_changed;
	callback<void(const int, const point&)> mouse_move;
	callback<void(const mouse_buttons&, const int, const point&)> mouse_dbl_click, mouse_click;
	callback<void(const mouse_buttons&, const int, const point&)> mouse_up, mouse_down;
	callback<void(const point&)> mouse_enter, mouse_leave;
	callback<void(bool)> enabled_changed;
	callback<void(float)> opacity_changed;
	// Raises when the User rotates the mousewheel. (Modifierkeys, Cursorposition, Delta)
	callback<void(const int, const point&, int)> mouse_wheel, mouse_h_wheel;
	callback<void(bool)> focus_changed;
	callback<void()> syscolour_changed;
	callback<void(const virtual_keys&, const key_extended_params&)> key_down, key_up; 
	callback<void(const virtual_keys&, const key_extended_params&)> syskey_down, syskey_up; 
	callback<void(wchar_t, const key_extended_params&)> char_sent, deadchar_sent, syschar_sent, sysdeadchar_sent;
	// Dragdrop
	callback<void(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect)> drag_enter;
	callback<void(DWORD keystate, const point& pt, dragdrop::drop_effects* effect)> drag_over;
	callback<void()> drag_leave;
	callback<void(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect)> drop;

	virtual void render(render_objects::graphics* g)  = 0;
	virtual bool contains(const point& p) const = 0;
	virtual void add_surface(dynamic_drawsurface*) = 0;
	virtual void remove_surface(dynamic_drawsurface*) = 0;

	virtual void set_focus(bool) = 0;
	virtual bool get_focus() const = 0;
	virtual void set_focused_surface(dynamic_drawsurface*) = 0;
	virtual dynamic_drawsurface* get_focused_surface() = 0;
	virtual float get_opacity() const = 0;
	virtual void set_opacity(float f)  = 0;
	virtual wstring get_title() const {return title;};
	virtual void set_title(const wstring& s) = 0;
	virtual void set_size(const size& sz, bool redraw = true) = 0;
	virtual size get_size() const {return sizef;};
	virtual void set_position(const point& p, bool redraw = false) = 0;
	virtual point get_position() const {return position;};
	virtual bool get_visible() const {return visible;};
	virtual void set_visible(bool b) = 0;
	virtual size get_max_size() const {return maxsize;};
	virtual size get_min_size() const {return minsize;};
	virtual void set_min_size(const size& s) = 0;
	virtual void set_max_size(const size& s) = 0;
	virtual margin get_padding() const {return pddng;};
	virtual void set_padding(const padding& p) = 0;
	virtual bool get_enabled() const {return enabled;};
	virtual void set_enabled(bool b) = 0;
	virtual void redraw() = 0;
	virtual void redraw(const rect& bounds) = 0;
	virtual void layout() = 0;
	virtual HWND get_handle() const = 0;
	virtual render_objects::font* get_font() const = 0;
	virtual bool get_enable_dragdrop() const {return _dragdrop;}
	virtual void set_enable_dragdrop(bool b) {_dragdrop = b;}
	virtual list<dynamic_drawsurface*>::const_iterator get_children_begin() {return surfaces.cbegin();}
	virtual list<dynamic_drawsurface*>::const_iterator get_children_end() {return surfaces.cend();}
	render_objects::cursor_surface get_cursor() const {return move(cur);}
	virtual void set_cursor(const render_objects::cursor_surface& cur_s) = 0;
protected:
	render_objects::cursor_surface cur;
	bool visible;
	bool _dragdrop;
	bool enabled;
	wstring title;
	point position;
	size sizef;
	size minsize;
	size maxsize;
	virtual void create_resources(render_objects::graphics*) = 0;
	padding pddng;
	list<dynamic_drawsurface*> surfaces;

	virtual void on_drag_enter(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect) = 0;
	virtual void on_drag_over(DWORD keystate, const point& pt, dragdrop::drop_effects* effect) = 0;
	virtual void on_drag_leave() = 0;
	virtual void on_drop(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect) = 0;
};

namespace dragdrop
{
	class data_object :
		public IDataObject
	{
	public:
		static IDataObject* create_data_object(FORMATETC* fmt, STGMEDIUM* med, int cnt) {return new data_object(fmt, med, cnt);}
		~data_object();
		HRESULT WINAPI GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium);
		HRESULT WINAPI GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium);
		HRESULT WINAPI QueryGetData(FORMATETC* pFormatEtc);
		HRESULT WINAPI GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut);
		HRESULT WINAPI SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease);
		HRESULT WINAPI EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc);
		HRESULT WINAPI DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection);
		HRESULT WINAPI DUnadvise(DWORD dwConnection);
		HRESULT WINAPI EnumDAdvise(IEnumSTATDATA** ppEnumAdvise);
		// Heap only - Call to Release() is required
		ULONG WINAPI Release();
	protected:
		data_object(FORMATETC* fmt, STGMEDIUM* med, int cnt);
		HRESULT WINAPI QueryInterface(REFIID iid, void** ppvObject);
		ULONG WINAPI AddRef();
		int get_format_index(FORMATETC* frmt);
	private:
		volatile long ref_count;
		FORMATETC* formats;
		STGMEDIUM* mediums;
		long format_cnt;
	};

	class drop_target :
		public IDropTarget
	{
	public:
		drop_target(drawsurface* target);
		virtual ~drop_target();

		void register_dragdrop();
		void deregister_dragdrop();
		void register_helper();
		void deregister_helper();
		IDropTargetHelper* get_helper() const {return drop_helper;}
	protected:
		HRESULT WINAPI QueryInterface(REFIID iid, void** ppvObject);
		ULONG WINAPI AddRef();
		// Calling Release() won't call delete! drop_target is used on Stack, for use one Heap call 'delete' after calling Release()
		ULONG WINAPI Release();

		HRESULT WINAPI DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
		HRESULT WINAPI DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
		HRESULT WINAPI DragLeave();
		HRESULT WINAPI Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	private:
		drawsurface* wnd;
		volatile long ref_count;
		IDropTargetHelper* drop_helper;
	};

	class drop_source :
		public IDropSource
	{
	public:
		drop_source(); 
	protected:
		HRESULT WINAPI QueryInterface(REFIID iid, void** ppvObject);
		ULONG WINAPI AddRef();
		ULONG WINAPI Release();
		HRESULT WINAPI QueryContinueDrag (BOOL fEscapePressed, DWORD grfKeyState);
		HRESULT WINAPI GiveFeedback(DWORD dwEffect);
	private:
		volatile long ref_count;
	};

	drop_effects get_effect_from_keys(DWORD keystate);
	HGLOBAL data_to_global(const void* src, size_t len);
};

class _cl_hlp
{
public:
	typedef callback<void(const mouse_buttons&, const int, const point&)> pr_event;
	_cl_hlp(pr_event& pr_cl, pr_event& pr_dwn, pr_event& pr_up, bool& condition);
private:
	pr_event& cb_cl;
	mouse_buttons btn;
	bool is_prsd;
	bool& cond;
	void on_dwn(const mouse_buttons& b, const int, const point&);
	void on_up(const mouse_buttons& b, const int modifier, const point& pos);
};

class dynamic_drawsurface : 
	public drawsurface, 
		private _cl_hlp
{
public:
	friend class ui::window;
	dynamic_drawsurface();
	virtual ~dynamic_drawsurface();

	callback<void(dynamic_drawsurface*)> parent_changed;
	callback<void(drawsurface*)> owner_changed;
	callback<void(const horizontal_align&)> horizontal_align_changed;
	callback<void(const vertical_align&)> vertical_align_changed;
	callback<void(const margin&)> margin_changed;
	callback<void(const render_objects::font*)> font_changed;
	callback<void(const matrix&)> transform_changed;
	callback<void(ui::context_menu*)> menu_changed;

	virtual void render(render_objects::graphics* g);
	virtual bool contains(const point& p) const;
	virtual inline point calc_position(const point& p) {return point(p.x - position.x, p.y - position.y);}
	virtual void set_size(const size& sz, bool redraw = true);
	virtual void set_position(const point& p, bool redraw = true);
	virtual void set_visible(bool b);
	virtual void set_min_size(const size& s);
	virtual void set_max_size(const size& s);
	virtual void set_padding(const padding& p);
	virtual void set_title(const wstring& s);

	virtual render_objects::font* get_font() const {if(m_font){return m_font.get();} else {return owner ? owner->get_font() : 0;}}
	virtual void set_font(render_objects::font* f);
	// Returns null if the surface is rectangular
	virtual render_objects::geometry* get_shape() const {return geo.get();}
	virtual void set_focus(bool b);
	virtual bool get_focus() const {return focus;}
	void set_focused_surface(dynamic_drawsurface*);
	dynamic_drawsurface* get_focused_surface() {return focused_surf;}

	virtual void redraw() {if(owner) owner->redraw();}
	virtual void redraw(const rect& bounds) {if(owner){ rect b(bounds); b.inflate(5.f, 5.f); owner->redraw(b);}}
	virtual void layout();

	// Override either update_shape(geometry*) or update_shape(const rect&)
	virtual void update_shape(render_objects::geometry* shape) {};
	virtual void update_shape(const rect& shape) {};
	dynamic_drawsurface* get_parent() const {return parent;}
	virtual void set_parent(dynamic_drawsurface* new_parent);
	drawsurface* get_owner() const {return owner;}
	void set_owner(drawsurface* ownr);
	inline drawsurface* get_absolute_owner() const {return parent ? parent : owner;}
	z_layer get_z_position() const {return z_position;}
	virtual	rect get_bounds() const;
	virtual rect get_rect() const {return rect(get_position(), get_size());}
	virtual bool get_enabled() const;
	virtual void set_enabled(bool b);
	bool get_auto_position() const {return auto_pos;}
	void set_auto_position(bool b);
	bool get_auto_size() const {return auto_size;}
	void set_auto_size(bool b);
	horizontal_align get_horinzontal_align() const {return hor_align;}
	void set_horizontal_align(const horizontal_align& h);
	vertical_align get_vertical_align() const {return vert_align;}
	void set_vertical_align(const vertical_align& h);
	margin get_margin() const {return mrgn;}
	void set_margin(const margin& m);
	virtual bool is_available() const {return visible&&get_enabled();};
	virtual matrix get_transform() const {return trnsfrm;};
	virtual void set_transform(const matrix& m);
	virtual float get_opacity() const {return opacity;}
	virtual void set_opacity(float f);
	bool is_transformed() const {return is_transf;}
	inline bool is_mouse_down() const {return is_mdown;}
	inline bool is_mouse_over() const {return is_mover;}

	virtual bool is_window_cursor() const {return cur.get_cursor() == 0;}
	virtual void set_cursor(const render_objects::cursor_surface& cur_s);
	virtual void erase_cursor();
	virtual bool is_rectangular() const {return true;}
	virtual bool get_captures_keyboard() const {return key_capture;}
	virtual bool get_captures_tab() const {return tab_capture;}
	virtual bool get_captures_mouse_wheel() const {return mouse_capture;}
	virtual void set_captures_keyboard(bool b) {key_capture = b;}
	virtual void set_captures_mouse_wheel(bool b) {mouse_capture = b;}
	virtual void set_captures_tab(bool b) {tab_capture = b;}
	matrix get_absolute_transform() const;
	inline point point_to_surface(const point& p) {return point(p.x - get_position().x, p.y - get_position().y);}
	inline point point_from_surface(const point& p) {return point(p.x + get_position().x, p.y + get_position().y);}
	bool get_clip_childs() const {return clip_childs;}
	virtual void set_clip_childs(bool b);
	unsigned get_tab_index() const {return tabidx;} 
	void set_tab_index(unsigned i);
	bool get_tab_stop() const {return tabstop;}
	void set_tab_stop(bool b) {tabstop = b;}
	drawing_state get_drawing_state() const {return draw_state;}
	// Sets the contextmenu for the surface. Hold by a shared_ptr<>
	void set_menu(ui::context_menu* m);
	ui::context_menu* get_menu() const {return mmenu.get();}
	bool is_in_range(const point& p)
	{
		matrix m = get_absolute_transform();
		point pc = p;
		m.invert();
		m.transform_points(&pc);
		rect rc = get_bounds();
		return contains(p) || (is_between_equ(pc.x, rc.get_left()-horizontal_leave_offset, rc.get_right()+horizontal_leave_offset) && is_between_equ(pc.y, rc.get_top()-vertical_leave_offset, rc.get_bottom()+vertical_leave_offset)); 
	}
	bool has_resources() const {return hs_resources;}
protected:
	virtual void add_surface(dynamic_drawsurface* surf);
	virtual void remove_surface(dynamic_drawsurface* surf);
	virtual void on_mouse_move(const int m, const point& p);
	virtual void on_mouse_dbl_click(const mouse_buttons& b, const int m, const point& p);
	virtual void on_mouse_down(const mouse_buttons& b, const int m, const point& p);
	virtual void on_mouse_up(const mouse_buttons& b, const int m, const point& p);
	virtual void on_mouse_wheel(const int m, const point& p, int delta);
	virtual void on_mouse_h_wheel(const int m, const point& p, int delta);
	virtual void crt_up();
	virtual void on_mouse_enter(const point& p);
	virtual void on_mouse_leave(const point& p);
	virtual void on_syscolour_changed();
	virtual void on_key_down(const virtual_keys& key, const key_extended_params& params);
	virtual void on_key_up(const virtual_keys& key, const key_extended_params& params);
	virtual void on_syskey_down(const virtual_keys& key, const key_extended_params& params);
	virtual void on_syskey_up(const virtual_keys& key, const key_extended_params& params);
	virtual void on_char_sent(wchar_t c, const key_extended_params& params);
	virtual void on_deadchar_sent(wchar_t c, const key_extended_params& params);
	virtual void on_syschar_sent(wchar_t c, const key_extended_params& params);
	virtual void on_sysdeadchar_sent(wchar_t c, const key_extended_params& params);
	virtual void on_menu_opening();
	virtual bool on_tab_pressed();
	virtual void on_drag_enter(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect);
	virtual void on_drag_over(DWORD keystate, const point& pt, dragdrop::drop_effects* effect);
	virtual void on_drag_leave();
	virtual void on_drop(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect);
	virtual void on_cursor_changed(HCURSOR curr_current, const point& p);
	void init_resources(gcl::render_objects::graphics* g);
	virtual void create_resources(render_objects::graphics* g);
	dynamic_drawsurface* parent; // may be null
	drawsurface* owner;
	bool is_mover;
	bool hs_resources;
	float opacity;
	bool is_transf, is_abs_trnsf;
	z_layer z_position;
private:
	drawing_state draw_state;
	IDataObject* last_data_object;
	bool tabstop;
	int tabidx;
	bool key_capture, mouse_capture, tab_capture;
	int bottom_s, top_s, topmost_s;
	horizontal_align hor_align;
	vertical_align vert_align;
	bool auto_pos, auto_size;
	bool clip_childs;
	margin mrgn;
	bool is_mdown;
	bool is_drop_entered;
	bool focus;
	shared_ptr<render_objects::font> m_font;
	shared_ptr<render_objects::geometry> geo;
	matrix trnsfrm;
	HWND get_handle() const {return 0;}
	dynamic_drawsurface* focused_surf;
	shared_ptr<ui::context_menu> mmenu;
};

namespace ui
{
class menu;
struct menu_strip_m
{
friend class menu;
public:
	menu_strip_m() : enabled(true) {}
	wstring get_title() const {return title;}
	void set_title(const wstring& s);
	context_menu* get_menu() const {return mmenu.get();}
	void set_menu(context_menu* menu);
	bool get_enabled() const {return enabled;}
	void set_enabled(bool b);
protected:
	virtual void measure(graphics* g, const point& p);
	rect get_bounds() const {return bounds;}
	rect get_intersect_bounds(float y, float height, float space) const {return rect(bounds.get_x()-space/2.f, y, bounds.get_width()+space, height);}
private:
	rect bounds;
	menu* owner;
	wstring title;
	bool enabled;
	shared_ptr<context_menu> mmenu;
};

class menu :
	public dynamic_drawsurface
{
public:
	menu();
	virtual ~menu();
	virtual void render(graphics* g);
	virtual bool is_rectangular() const {return true;}
	virtual void create_resources(graphics* g);

	callback<void(const colour&)> back_colour_changed, pressed_colour_changed, font_colour_changed, hot_colour_changed;
	
	void menu::add_strip(menu_strip_m* strip);
	void menu::remove_strip(menu_strip_m* strip);
	void menu::clear_strips();
	colour get_back_colour() const {return cl_back;}
	void set_back_colour(const colour& c);
	colour get_hot_colour() const {return cl_hot;} 
	void set_hot_colour(const colour& c);
	colour get_pressed_colour() const {return cl_down;}
	void set_pressed_colour(const colour& c);
	colour get_font_colour() const {return cl_font;}
	void set_font_colour(const colour& c);
	virtual void set_opacity(float f);
protected:
	virtual void on_syscolour_changed();
	void this_mouse_move(const int mod, const point& p);
	void this_mouse_leave(const point&);
	void this_mouse_down(const mouse_buttons& mb, int modd, const point& p) {}//opened = false;}
	void this_mouse_up(const mouse_buttons& mb, int modd, const point& p);
	vector<menu_strip_m*> strips;
	colour cl_back, cl_down, cl_hot, cl_font, cl_gray;
	shared_ptr<solid_brush> br_back, br_font, br_hot, br_down, br_gray;
	int get_opened();
private:
	int hidex;
	float space;
};

};

};

#endif