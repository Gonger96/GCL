/*****************************************************************************
*                           gcl - graphics.h                                 *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H
#ifdef _MSC_VER 
#pragma once
#endif
#include "stdafx.h"
#include "callback.h"

namespace gcl {

template <typename _src, typename _dst>
bool is_type(_src* source) {return (dynamic_cast<_dst*>(source) != 0);};
template <typename _gcl>
void release(_gcl* f) {if(f){f->release_resources(); delete f; f = 0;}};
template <typename _gcl>
void release(_gcl& f) {f.release_resources();};
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
template <typename f>
inline bool change_if_diff(f& prop, const f& val) {if(prop == val) return false; prop = val; return true;};
template <typename f>
inline bool change_if_diff(f* prop, f* val) {if(!val) return false; if(prop == val) return false; release(prop); prop = val; return true;};

//template <typename t> unused
//class gcl_deleter
//{
//public:
//	void operator() (t* _frgcl)
//	{
//		release(_frgcl);
//	};
//};

// Defines a coordinate in 2D space
struct point
{
public:
	float x, y;
	point(float _x, float _y) : x(_x), y(_y) {};
	point() : x(0), y(0) {};

	point& operator= (const point& p) {x = p.x; y = p.y; return (*this);};
	wstring to_wstring() const 
	{
		return L"{" + std::to_wstring(x) + L", " + std::to_wstring(y) + L"}";
	};
	friend wostream& operator<<(wostream& stream, const point& p)
	{
		stream << wstring(L"{") + std::to_wstring(p.x) + L", " + std::to_wstring(p.y) + L"}";
		return stream;
	};
	bool operator==(const point& p)
	{
		return p.x == x && p.y == y;
	}
};

// Defines a size (width & height) in 2D space
struct size
{
public:
	float width, height;
	size(float _width, float _height) : width(_width), height(_height) {};
#ifdef max
#undef max
#endif
	static const size max_size() { return size(numeric_limits<float>::max(), numeric_limits<float>::max());};
	size() : width(0), height(0) {};
	point to_point() const {return point(width, height);};
	size& operator= (const size& sz) {width = sz.width; height = sz.height; return (*this);};
	wstring to_wstring() {return wstring(L"{") + std::to_wstring(width) + L", " + std::to_wstring(height) + L"}";};
	friend wostream& operator<<(wostream& stream, const size& p)
	{
		stream << wstring(L"{") + std::to_wstring(p.width) + L", " + std::to_wstring(p.height) + L"}";
		return stream;
	};
	bool operator==(const size& sz)
	{
		return sz.height == height && sz.width == width;
	};
};

// Defines a rectangle in 2D space
struct rect
{
public:
	point position;
	size sizef;
	rect() : position(point(0,0)), sizef(size(0,0)) {};
	rect(const point& p, const size& sz) : position(p), sizef(sz) {};
	rect(float x, float y, float width, float height) : position(x, y), sizef(width, height) {};
	void inflate(float _width, float _height) {position.x -= _width; position.y -= _height; sizef.height += _height*2; sizef.width += _width*2;};
	void inflate(const size& sz) {inflate(sz.width, sz.height);};
	bool contains(const point& p) const 
	{
		return ((position.x <= p.x) && (p.x < (position.x + sizef.width)) && (position.y <= p.y) && (p.y < (position.y + sizef.height)));
	};
	rect& operator= (const rect& sz) {position = sz.position; sizef = sz.sizef; return (*this);};
	wstring to_wstring() {return wstring(L"{") + position.to_wstring() + L", " + sizef.to_wstring() + L"}";};
	friend wostream& operator<<(wostream& stream, const rect& rc)
	{
		stream << L"{" << rc.position << L", " << rc.sizef << L"}";
		return stream;
	};
};

// Defines an ellipse in 2D space
struct ellipse // contains machen
{
public:
	point position;
	float radius_x;
	float radius_y;
	rect get_rect() const {return rect(position.x - radius_x, position.y - radius_y, 2 * radius_x, 2 * radius_y);};
	ellipse(const point& p, float radiusx, float radiusy) : position(p), radius_x(radiusx), radius_y(radiusy) {};
	ellipse(float x, float y, float radiusx, float radiusy) : position(x, y), radius_x(radiusx), radius_y(radiusy) {};
	ellipse(const rect& rc) : position(static_cast<float>(rc.position.x + rc.sizef.width / 2.0f), static_cast<float>(rc.position.y + rc.sizef.height / 2.0f)), radius_x(static_cast<float>(rc.sizef.width / 2.0f)), radius_y(static_cast<float>(rc.sizef.height / 2.0)) {};
	ellipse& operator= (const ellipse& sz) {position = sz.position; radius_x = sz.radius_x; radius_y = sz.radius_y; return (*this);};
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
	uchar a, r, g, b;
	uint get_value() const {return ((a << 24) | (r << 16) | (g << 8) | (b));};
	colour() : a(255), r(0), g(0), b(0) {};
	colour(uchar r, uchar g, uchar b, uchar a = 255) : a(a), r(r), g(g), b(b) {};
	colour(uchar a, const colour& c) : a(a), r(c.r), g(c.g), b(c.b) {};
	colour(uint value) : a((value & 0xff000000) >> 24), r((value & 0x00ff0000) >> 16), g((value & 0x0000ff00) >> 8), b(value & 0x000000ff) {};
	colour(uchar a, uint value) : a(a), r((value & 0xff0000) >> 16), g((value & 0x00ff00) >> 8), b(value & 0x0000ff) {};
	colour(known_colour c) : a(255), r(((uint)c & 0xff0000) >> 16), g(((uint)c & 0x00ff00) >> 8), b(uint(c) & 0x0000ff) {};
	colour(uchar a_, known_colour c) : a(a_), r(((uint)c & 0xff0000) >> 16), g(((uint)c & 0x00ff00) >> 8), b(uint(c) & 0x0000ff) {};
	colour& operator= (const colour& c) {a = c.a; r = c.r; g = c.g; b = c.b; return (*this);};
	colour opacity(float fac) const
	{
		return colour(r, g, b, static_cast<uchar>(a*fac));
	};
	colour invert() {return colour(~r, ~g, ~b, ~a);};
	bool operator==(const colour& c) {return c.a == a && c.r == r && c.g == g && c.b == b;};
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

const float pi = 3.141592654f;
// Defines a 3x2matrix for a 2D-transformation
struct matrix
{
public:
	float m11, m12, m21, m22, m31, m32;

	matrix() : m11(1), m12(0), m21(0), m22(1), m31(0), m32(0) {};
	matrix(float _m11, float _m12, float _m21, float _m22, float _m31, float _m32) : m11(_m11), m12(_m12), m21(_m21), m22(_m22), m31(_m31), m32(_m32) {};

	// Returns an Identitymatrix, which causes no transformation
	static matrix identity() {return matrix(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);};
	// Returns wether a matrix is an Identitymatrix or not
	static bool is_identity(const matrix& m) {return (m == identity());};
	// Returns wether this matrix is an Identitymatrix or not
	bool is_identity() {return (*this == identity());};
	// Resets the current matrix
	void reset() {*this = matrix::identity();};

	// Compares two matrices
	bool operator==(const matrix& m) const {return (m11 == m.m11 && m12 == m.m12 && m21 == m.m21 && m22 == m.m22 && m31 == m.m31 && m32 == m.m32);};
	// Multiplies two matrices
	matrix operator*(const matrix& m)
	{
		matrix m_;
		float m11_ = m11*m.m11 + m12*m.m21; // + 0*m.m31;
		float m12_ = m11*m.m12 + m12*m.m22; // + 0*m.m32;
		float m21_ = m21*m.m11 + m22*m.m21; // + 0*m.m31;
		float m22_ = m21*m.m12 + m22*m.m22; // + 0*m.m32 
		float m31_ = m31*m.m11 + m32*m.m21 + m.m31;			//  1 0 /0
		float m32_ = m31*m.m12 + m32*m.m22 + m.m32;			//  0 1 /0
		m_.m11 = m11_;											//  0 0 /1
		m_.m12 = m12_;
		m_.m21 = m21_;
		m_.m22 = m22_;
		m_.m31 = m31_;
		m_.m32 = m32_;
		return m_;
	};
	// Returns a matrix with the given rotation
	static matrix rotation(float angle)
	{
		float sinalpha = std::sinf(angle / 180*pi);
		float cosalpha = std::cosf(angle / 180*pi);
		return matrix(cosalpha, sinalpha, -sinalpha, cosalpha, 0, 0);
	};
	// Rotates the matrix
	void rotate(float angle)
	{
		float sinalpha = std::sinf(angle / 180*pi);
		float cosalpha = std::cosf(angle / 180*pi);
		*this = matrix(cosalpha, sinalpha, -sinalpha, cosalpha, 0, 0) * (*this);
	};
	
	static matrix rotation_at(float angle, const point& p)
	{
		float sinalpha = std::sinf(angle / 180*pi);
		float cosalpha = std::cosf(angle / 180*pi);
		return matrix(1, 0, 0, 1, -p.x, -p.y) * matrix(cosalpha, sinalpha, -sinalpha, cosalpha, p.x, p.y);
	};

	void rotate_at(float angle, const point& p)
	{
		float sinalpha = std::sinf(angle / 180*pi);
		float cosalpha = std::cosf(angle / 180*pi);
		*this = matrix(1, 0, 0, 1, -p.x, -p.y) * matrix(cosalpha, sinalpha, -sinalpha, cosalpha, p.x, p.y);
	};// gucke nach skew-transform 

	void rotate_x_y(float angle) // später hinzufügen
	{
		// y: cos,0,sin,0,1,0,-sin,0,cos
		// x: 1,0,0,0,cos,-sin,0,sin,cos
	};

	static matrix translation(float x, float y)
	{
		return matrix(1, 0, 0, 1, x, y);
	};

	void translate(float x, float y)
	{
		*this = matrix(1, 0, 0, 1, x, y) * (*this);
	};

	static matrix scalation(float fac_x, float fac_y)
	{
		return matrix(fac_x, 0, 0, fac_y, 0, 0);
	};

	void scale(float fac_x, float fac_y)
	{
		*this = matrix(fac_x, 0, 0, fac_y, 0, 0) * (*this);
	};

	static matrix shearing(float fac_x, float fac_y)
	{
		return matrix(1, fac_x, fac_y, 1, 0, 0);
	};

	void shear(float fac_x, float fac_y)
	{
		*this = matrix(1, fac_x, fac_y, 1, 0, 0) * (*this);
	};

	static matrix skewing(float x, float y, float angle_x, float angle_y)
	{
		float tan_theta = std::tanf(angle_x / 180*pi);
		float tan_phi = std::tanf(angle_y / 180*pi);
		return matrix(1, tan_phi, tan_theta, 1, -y*tan_theta, -x*tan_phi);
	};

	void skew(float x, float y, float angle_x, float angle_y)
	{
		float tan_theta = std::tanf(angle_x / 180*pi);
		float tan_phi = std::tanf(angle_y / 180*pi);
		(*this) = matrix(1, tan_phi, tan_theta, 1, -y*tan_theta, -x*tan_phi) * (*this);
	};
};

namespace render_objects {

enum class brush_types {solid_brush, texture_brush, linear_gradient_brush};
enum class fill_modes {alternate, winding};
enum class wrap_modes {wrap, mirror_x, mirror_y, mirror_xy, clamp};

// Pen dependencies
enum class pen_align {center, inset};
enum class dash_cap {flat = 0, round = 2, triangle = 3};
enum class dash_style {dash = 1, dash_dot = 3, dash_dot_dot = 4, dot = 2, solid = 0}; 
//
// Font
namespace font_style {const int regular = 0, bold = 1, italic = 2;};
enum class string_format {direction_left_to_right, direction_right_to_left};
enum class horizontal_string_align {left, middle, right};
enum class vertical_string_align {top, middle, bottom};
//

class font
{
public:
	virtual ~font() {};
	// Returns the current Style (see namespace font_style)
	virtual int get_style() const = 0;
	// Returns the current size in pixels
	virtual float get_size() const = 0;
	// Returns the current familyname
	virtual wstring get_family() const = 0;
	// Releases all allocated resources
	virtual void release_resources() = 0;
	virtual bool operator==(font* f) = 0;
};

class texture
{
public:
	virtual ~texture() {};
	// Returns the horizontal resolution of the texture
	virtual int get_width() const = 0;
	// Returns the vertical resolution of the texture
	virtual int get_height() const = 0;
	// Releases all allocated resources
	virtual void release_resources() = 0;
	// Clones the whole texture
	virtual texture* clone() = 0;

	// Locks the Imagedata in Memory (Format: PBGRA - 32 Bits per Pixel)
	virtual unsigned char* alloc() = 0;
	// Frees the Imagedata
	virtual void free() = 0;
	// Returns the stride (Width*BitsPerPixel) after the texture has been locked
	virtual int get_stride() = 0;
};

class brush 
{
public:
	brush() {};
	virtual ~brush() {};
	// Yields the current brushtype
	virtual brush_types get_type() = 0;
	// Releases all allocated resources
	virtual void release_resources() = 0;
	// Transforms the brush
	virtual void set_transform(const matrix& m) = 0;
	// Yields the current transformation
	virtual matrix get_transform() const = 0;
	// Returns the current opacity
	virtual float get_opacity() const = 0;
	// Sets the opacity in percent
	virtual void set_opacity(float val) = 0;
};

class solid_brush : 
	public brush
{
public:
	virtual ~solid_brush() {};
	// Returns the colour
	virtual colour get_colour() = 0;
	// Sets the colour
	virtual void set_colour(const colour& c) = 0;
	virtual void set_transform(const matrix& m) {};
	virtual matrix get_transform() const {return matrix::identity();};
};

class texture_brush : 
	public brush
{
public:
	virtual ~texture_brush() {};
	// Yields the texture of the brush
	virtual texture* get_texture() const = 0;
	// Sets the edge/border wrapmode
	virtual void set_wrap_mode(const wrap_modes& mode) = 0;
	// Yields the wrapmode
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
	// Sets the start- and destinationpoint of the gradientfield
	virtual void set_rect(const point& p1, const point& p2) = 0;
	// Sets the rectangle of the gradientfield
	virtual void set_rect(const rect& rc) {set_rect(rc.position, rc.sizef.to_point());};
	// Yields the rectangle of the gradientfield
	virtual rect get_rect() const = 0;
	virtual gradient_stop get_gradients() const = 0;
	virtual void set_gradients(gradient_stop& gradients) = 0;
};

class radial_gradient_brush : public brush
{

}; //IIIIIIIIIIIIIIIIIIIIIIIIIIIII missing 

class pen
{
public:
	virtual ~pen() {};
	virtual brush* get_brush() const = 0;
	virtual dash_cap get_start_cap() const = 0;
	virtual void set_start_cap(const dash_cap& p) = 0;
	virtual dash_cap get_end_cap() const = 0;
	virtual void set_end_cap(const dash_cap& p) = 0;
	virtual pen_align get_align() const = 0;
	virtual void set_align(const pen_align& a) = 0;
	virtual dash_cap get_dash_cap() const = 0;
	virtual void set_dash_cap(const dash_cap& c) = 0;
	virtual dash_style get_dash_style() const = 0;
	virtual void set_dash_style(const dash_style& d) = 0;
	virtual float get_dash_offs() const = 0;
	virtual void set_dash_offs(float f) = 0;
	virtual float get_width() const = 0;
	virtual void set_width(float f) = 0;
	virtual void release_resources() = 0;
};

class geometry
{
public:
	virtual ~geometry() {};
	virtual void begin_geometry(const point& p) = 0;
	virtual void end_geometry() = 0;
	virtual bool contains(const point& p) = 0; 
	virtual bool outline_contains(const point& p, pen* pe) = 0;
	virtual void release_resources() = 0;

	//virtual void add_arc(const rect& rc, float start_angle, float sweep_angle) = 0;
	virtual void add_bezier(const point& p1, const point& p2, const point& p3) = 0;
	virtual void add_beziers(const point* ps, int count) = 0;
	//virtual void add_curve(const point* ps, int count, float tension) = 0;
	virtual void add_line(const point& p1) = 0;
	virtual void add_lines(const point* ps, int count) = 0;
	virtual void add_rect(const rect& rc) = 0;
	virtual void add_ellipse(const ellipse& e) = 0;
	virtual void add_rounded_rect(const rect& rc, float radiusX, float radiusY) = 0;

	virtual void transform(const matrix& m) = 0;
};

enum class text_rendering_modes {system_default, antialias, cleartype};

class graphics
{
public:
	virtual ~graphics(void) {};
	virtual HDC get_dc() const = 0;

	virtual void begin() = 0;
	virtual void clear(const colour& c)= 0;
	virtual void end() = 0;

	virtual void fill_rect(const rect& rc, brush* b) = 0;
	virtual void fill_rects(const rect* rcs, int count, brush* b) = 0;
	virtual void fill_ellipse(const ellipse& e, brush* b) = 0;
	virtual void fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b) = 0;
	virtual void fill_geometry(geometry* geo, brush* b) = 0;

	virtual void draw_ellipse(const ellipse& e, pen* p) = 0;
	virtual void draw_line(const point& p1, const point& p2, pen* p) = 0;
	virtual void draw_lines(const point* ps, int count, pen* p) = 0;
	//virtual void draw_curve(point* points, int count, float tension);
	virtual void draw_rect(const rect rc, pen* p) = 0;
	virtual void draw_rects(const rect* rcs, int count, pen* p) = 0;
	virtual void draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p) = 0;
	virtual void draw_geometry(geometry* geo, pen* p) = 0;
	virtual void draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format = string_format::direction_left_to_right) = 0;
	virtual void draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format = string_format::direction_left_to_right, const horizontal_string_align& clipping_h = horizontal_string_align::left, const vertical_string_align& clipping_v = vertical_string_align::top) = 0;
	virtual void draw_texture(texture* image, const point& p) = 0;
	virtual void draw_texture(texture* image, const point& p, const size& sz) = 0;
	virtual void draw_texture(texture* image, const rect& rc) = 0;
	virtual void draw_texture(texture* image, const rect& src, const point& dst) = 0;
	virtual void draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest) = 0;
	virtual void draw_texture(texture* image, const rect& src, const rect& dest) = 0;

	virtual solid_brush* create_solid_brush(const colour& c) = 0;
	virtual texture_brush* create_texture_brush(texture* image) = 0;
	virtual linear_gradient_brush* create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma = false, const wrap_modes& wrapmode = wrap_modes::wrap) = 0;
	virtual pen* create_pen(brush* b, float width = 1.0f, pen_align align = pen_align::center, dash_cap startcap = dash_cap::flat, dash_cap endcap = dash_cap::flat, dash_cap dash_cap = dash_cap::flat, dash_style dash_style = dash_style::solid, float offset = 0.0f) = 0;
	virtual texture* create_texture(const wstring& filename) = 0;
	virtual texture* create_texture(int id, LPWSTR type, HINSTANCE inst) = 0;
	virtual font* create_font(const wstring& family_name, float size, int fstyle = font_style::regular) = 0;
	virtual geometry* create_geometry() = 0;
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

	virtual void release_resources() = 0;
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

class drawsurface
{
public:
	drawsurface() : visible(true) {};
	virtual ~drawsurface() {};
	callback<void(const wstring&)> title_changed;
	callback<void(const dynamic_drawsurface*)> surface_added;
	callback<void(const dynamic_drawsurface*)> surface_removed;
	callback<void(const size&)> size_changed;
	callback<void(bool)> visible_changed;
	callback<void(const point&)> position_changed;
	callback<void(const size&)> min_size_changed, max_size_changed;
	callback<void()> resources_created;
	callback<void()> resources_released;
	callback<void()> layouted;
	callback<void(const padding&)> padding_changed;
	callback<void(const int, const point&)> mouse_move;
	callback<void(const mouse_buttons&, const int, const point&)> mouse_dbl_click, mouse_click;
	callback<void(const mouse_buttons&, const int, const point&)> mouse_up, mouse_down;
	callback<void(const int, const point&)> mouse_enter, mouse_leave;
	callback<void(bool)> enabled_changed;

	virtual void render(render_objects::graphics* g)  = 0;
	virtual bool contains(const point& p) const = 0;
	
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
	virtual void add_surface(dynamic_drawsurface*) = 0;
	virtual void remove_surface(dynamic_drawsurface*) = 0;
	virtual bool get_enabled() const {return enabled;};
	virtual void set_enabled(bool b) = 0;
	virtual void redraw() = 0;
	virtual void redraw(const rect& bounds) = 0;
	virtual void layout() = 0;

	virtual void release_resources() = 0;
protected:
	bool visible;
	bool enabled;
	wstring title;
	point position;
	size sizef;
	size minsize;
	size maxsize;
	virtual void create_resources(render_objects::graphics*) = 0;
	padding pddng;
	list<dynamic_drawsurface*> surfaces;
};

// Layout enums and references
enum class horizontal_align {left, center, right, stretch};
enum class vertical_align {top, center, bottom, stretch};
enum class drawing_state {normal, pressed, hover};
//

class _cl_hlp
{
public:
	typedef callback<void(const mouse_buttons&, const int, const point&)> pr_event;
	_cl_hlp(pr_event& pr_cl, pr_event& pr_dwn, pr_event& pr_up, bool& condition) : cb_cl(pr_cl), cond(condition)
	{
		is_prsd = false;
		pr_dwn += make_func_ptr(this, &_cl_hlp::on_dwn);
		pr_up += make_func_ptr(this, &_cl_hlp::on_up);
	};
private:
	pr_event& cb_cl;
	mouse_buttons btn;
	bool is_prsd;
	bool& cond;
	void on_dwn(const mouse_buttons& b, const int, const point&)
	{
		btn = b;
		is_prsd = true;
	};
	void on_up(const mouse_buttons& b, const int modifier, const point& pos)
	{
		if(!is_prsd || b != btn || !cond) return;
	    is_prsd = false;
		cb_cl(b, modifier, pos);
	};
};

class dynamic_drawsurface : 
	public drawsurface, 
		private _cl_hlp
{
public:
	dynamic_drawsurface() : _cl_hlp(mouse_click, mouse_down, mouse_up, is_mouse_over)
	{
		owner = 0;
		//mfont = 0;
		hs_resources = false;
		is_mouse_over = is_mouse_down = false;
		bottom_s = 0;
		top_s = 0;
		topmost_s = 0;
		draw_state = drawing_state::normal;
		parent = 0;
		auto_pos = true;
		auto_size = true;
		z_position = z_layer::bottom;
		set_min_size(size(5, 5));
		set_max_size(size::max_size());
		m_font = 0;
		enabled = true;
	};

	callback<void(const dynamic_drawsurface*)> parent_changed;
	callback<void(const horizontal_align&)> horizontal_align_changed;
	callback<void(const vertical_align&)> vertical_align_changed;
	callback<void(const margin&)> margin_changed;
	callback<void(const render_objects::font*)> font_changed;
	callback<void(const colour&)> font_colour_changed, back_colour_changed, effect_colour_changed;
	callback<void(const matrix&)> transform_changed;

	virtual void render(render_objects::graphics* g)
	{
		if(!hs_resources)
		{
			create_resources(g);
			hs_resources = true;
		}
	};
	virtual bool contains(const point& p) const {return false;};
	virtual inline point calc_position(const point& p)
	{
		return point(p.x - position.x, p.y - position.y);
	};
	virtual void set_size(const size& sz, bool redraw = true) 
	{
		if(sizef == sz) return;
		sizef.width =  max(min(maxsize.width, sz.width), minsize.width);
		sizef.height =  max(min(maxsize.height, sz.height), minsize.height);
		size_changed(sz);
		auto par = get_absolute_owner();
		if(par && redraw) par->layout();
	};
	virtual void set_position(const point& p, bool redraw = true)
	{
		if(!change_if_diff(position, p)) return;
		position_changed(p);
		auto par = get_absolute_owner();
		if(par && redraw) par->layout();
	};
	virtual void set_visible(bool b)
	{
		if(!change_if_diff(visible, b)) return;
		visible_changed(b);
		if(owner) owner->redraw(get_redraw_rc());
	};
	virtual void set_min_size(const size& s)
	{
		if(minsize == s) return;
		minsize = s;
		min_size_changed(s);
		if(sizef.height < s.height)
			set_size(size(sizef.width, s.height), false);
		if(sizef.width < s.width)
			set_size(size(s.width, sizef.height));
		auto par = get_absolute_owner();
		if(par) par->layout();
	};
	virtual void set_max_size(const size& s)
	{
		if(maxsize == s) return;
		maxsize = s;
		max_size_changed(s);
		if(sizef.height > s.height)
			set_size(size(sizef.width, s.height), false);
		if(sizef.width > s.width)
			set_size(size(s.width, sizef.height));
		auto par = get_absolute_owner();
		if(par) par->layout();
	};
	virtual void set_padding(const padding& p)
	{
		if(change_if_diff(pddng, p)) return;
		padding_changed(p);
		layout();
	};
	virtual void set_title(const wstring& s)
	{
		if(!change_if_diff(title, s)) return;
		title_changed(s);
		if(owner) owner->redraw(get_redraw_rc());
	};

	virtual render_objects::font* get_font() const {return m_font;};
	virtual void set_font(render_objects::font* f) 
	{
		if(!change_if_diff(m_font, f)) return;
		font_changed(f);
		owner->redraw(get_redraw_rc());
	};
	virtual colour get_back_colour() const {return back_colour;};
	virtual void set_back_colour(const colour& c) 
	{
		if(!change_if_diff(back_colour, c)) return;
		back_colour_changed(c);
		if(owner) owner->redraw(get_redraw_rc());
	};
	virtual colour get_font_colour() const {return font_colour;};
	virtual void set_font_colour(const colour& c) 
	{
		if(!change_if_diff(font_colour, c)) return;
		font_colour_changed(c);
		if(owner) owner->redraw(get_redraw_rc());
	};
	virtual colour get_effect_colour() const {return effect_colour;};
	virtual void set_effect_colour(const colour& c)
	{
		if(!change_if_diff(effect_colour, c)) return;
		effect_colour_changed(c);
		if(owner) owner->redraw(get_redraw_rc());
	};

	virtual void add_surface(dynamic_drawsurface* surf)
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
	};
	virtual void remove_surface(dynamic_drawsurface* surf) 
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
		surface_removed(surf);
	};

	virtual void redraw() {if(owner) owner->redraw();};
	virtual void redraw(const rect& bounds) {if(owner) owner->redraw(bounds);};
	virtual void layout()
	{
		for(auto& surf : surfaces)
		{
			if(surf->get_auto_position())
			{
				float x = 0, y= 0;
				float w = 0, h = 0;
				margin m = surf->get_margin();
				switch(surf->get_horinzontal_align())
				{
				case horizontal_align::left:
					x = pddng.left + get_position().x;
					w = 0;
					break;
				case horizontal_align::right:
					x = get_size().width - get_position().x - surf->get_size().width + pddng.left;
					w = 0;
					break;
				case horizontal_align::center:
					x = (get_size().width - get_position().x) / 2.f - surf->get_size().width / 2.f + pddng.left;
					w = 0;
					break;
				case horizontal_align::stretch:
					w = get_size().width;
					x = pddng.left + get_position().x;
				}
				switch(surf->get_vertical_align())
				{
				case vertical_align::top:
					y = pddng.top + get_position().y;
					h = 0;
					break;
				case vertical_align::bottom:
					y = get_size().height - surf->get_size().height;
					h = 0;
					break;
				case vertical_align::center:
					y = (get_size().height - get_position().y)/ 2.f - surf->get_size().height / 2.f;
					h = 0;
					break;
				case vertical_align::stretch:
					y = pddng.top + get_position().y;
					h = get_size().height;
				}
				surf->set_position(point(x+pddng.left+m.left, y+pddng.top+m.top), false);
				if(surf->get_auto_size())surf->set_size(size(w-pddng.right-pddng.left-m.right-m.left, h-pddng.top-pddng.bottom-m.bottom-m.top), false);
			}
			surf->layout();
		}
		redraw(get_redraw_rc());
		layouted();
	};

	virtual void release_resources()
	{
		auto par = get_absolute_owner();
		if(par)
			par->remove_surface(this);
		release(m_font);
		hs_resources = false;
		resources_released();
	};
	virtual void create_resources(render_objects::graphics* g) 
	{
		HFONT f = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONT l = {};
		GetObject(f, sizeof(LOGFONT), &l);
		m_font = g->create_font(l.lfFaceName, 20);
		DeleteObject(f);
		resources_created();
	};

	dynamic_drawsurface* get_parent() const {return parent;};
	void set_parent(dynamic_drawsurface* new_parent) 
	{
		if(new_parent == parent) return;
		auto par = get_absolute_owner();
		if(par)
			par->remove_surface(this);
		parent = new_parent; 
		parent_changed(new_parent);
		if(parent)
			parent->add_surface(this);
		else
			owner->add_surface(this);
		redraw();
	};
	drawsurface* get_owner() const {return owner;};
	void set_owner(drawsurface* ownr)
	{
		if(ownr == owner) return;
		auto par = get_absolute_owner();
		if(par)
			par->remove_surface(this);
		owner = ownr;
		if(owner)
			owner->add_surface(this);
		redraw();
	};
	inline drawsurface* get_absolute_owner() const {return parent ? parent : owner;};
	z_layer get_z_position() const {return z_position;};
	virtual	rect get_redraw_rc()
	{
		return rect(get_position(), get_size());
	};

	virtual bool get_enabled() const 
	{
		auto par = get_absolute_owner();
		if(par) return enabled && par->get_enabled();
		return enabled;
	};
	virtual void set_enabled(bool b)
	{
		if(!change_if_diff(enabled, b)) return;
		enabled_changed(b);
		if(owner)owner->redraw(get_redraw_rc());
	};
	bool get_auto_position() const {return auto_pos;};
	void set_auto_position(bool b) 
	{
		if(b == auto_pos) return;
		auto_pos = b;
		auto par = get_absolute_owner();
		if(par) par->layout();
	};
	bool get_auto_size() const {return auto_size;};
	void set_auto_size(bool b)
	{
		if(b == auto_size) return;
		auto_size = b;
		auto par = get_absolute_owner();
		if(par) par->layout();
	};
	horizontal_align get_horinzontal_align() const {return hor_align;};
	void set_horizontal_align(const horizontal_align& h)
	{
		if(hor_align == h) return;
		hor_align = h;
		auto par = get_absolute_owner();
		if(par) par->layout();
		horizontal_align_changed(h);
	};
	vertical_align get_vertical_align() const {return vert_align;};
	void set_vertical_align(const vertical_align& h)
	{
		if(vert_align == h) return;
		vert_align = h;
		auto par = get_absolute_owner();
		if(par) par->layout();
		vertical_align_changed(h);
	};
	margin get_margin() const {return mrgn;};
	void set_margin(const margin& m) 
	{
		if(mrgn == m) return;
		mrgn = m;
		margin_changed(m);
		auto par = get_absolute_owner();
		if(par) par->layout();
	};
	virtual bool is_available() const {return visible&&get_enabled();};
	// Comming soon
	//virtual matrix get_transform() const {return transform;};
	//virtual void set_transform(const matrix& m) 
	//{
	//	if(!change_if_diff(transform, m)) return;
	//	transform_changed(m);
	//	if(owner) owner->redraw();
	//};

	inline bool get_mouse_down() const {return is_mouse_down;};
	inline bool get_mouse_over() const {return is_mouse_over;};

	virtual void on_mouse_move(const int m, const point& p)
	{
		bool risen = false;
		for(auto& surf : surfaces)
		{
			if(surf->contains(p) && surf->is_available())
			{
				if(!surf->get_mouse_over())
					surf->on_mouse_enter(m, p);
				surf->on_mouse_move(m, p);
				risen = true;
			}
			else if(surf->get_mouse_over() && surf->is_available())
				surf->on_mouse_leave(m, p);
		}
		if(!risen) mouse_move(m, p);
	};
	virtual void on_mouse_dbl_click(const mouse_buttons& b, const int m, const point& p)
	{
		bool risen = false;
		for(auto& surf : surfaces)
		{
			if(surf->contains(p) && surf->is_available())
			{
				surf->on_mouse_dbl_click(b, m, p);
				risen = true;
			}
		}
		if(!risen) mouse_dbl_click(b, m, p);
	};
	virtual void on_mouse_down(const mouse_buttons& b, const int m, const point& p)
	{
		bool risen = false;
		is_mouse_down = true;
		if(b == mouse_buttons::left) draw_state = drawing_state::pressed;
		for(auto& surf : surfaces)
		{
			if(surf->contains(p) &&  surf->is_available())
			{
				surf->on_mouse_down(b, m, p);
				risen = true;
			}
		}
		if(!risen) mouse_down(b, m, p);
	};
	virtual void on_mouse_up(const mouse_buttons& b, const int m, const point& p)
	{
		bool risen = false;
		is_mouse_down = false;
	    is_mouse_over ? draw_state = drawing_state::hover : draw_state = drawing_state::normal;
		for(auto& surf : surfaces)
		{
			if(surf->contains(p) && surf->is_available())
			{
				surf->on_mouse_up(b, m, p);
				risen = true;
			}
		}
		if(!risen) mouse_up(b, m, p);
	};
	virtual void on_mouse_enter(const int m, const point& p)
	{
		is_mouse_over = true;
		is_mouse_down ? draw_state = drawing_state::pressed : draw_state = drawing_state::hover;
		mouse_enter(m, p);
		if(owner) owner->redraw(get_redraw_rc());
	};
	virtual void on_mouse_leave(const int& m, const point& p)
	{
		is_mouse_over = false;
		draw_state = drawing_state::normal;
		mouse_leave(m, p);
		if(owner) owner->redraw(get_redraw_rc());
	};

protected:
	dynamic_drawsurface* parent; // may be null
	drawsurface* owner;
	bool is_mouse_over;
	drawing_state draw_state;
	bool hs_resources;
	colour font_colour;
	colour back_colour;
	colour effect_colour;
	matrix transform;
private:
	z_layer z_position;
	int bottom_s, top_s, topmost_s;
	horizontal_align hor_align;
	vertical_align vert_align;
	bool auto_pos, auto_size;
	margin mrgn;
	bool is_mouse_down;
	render_objects::font* m_font;
};

};

#endif