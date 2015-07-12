/*****************************************************************************
*                        GCL - renderer_gdip.h                               *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"
#include "graphics.h"
#include "system.h"
#include <Shlwapi.h>

#ifndef RENDERER_GDIP_H
#define RENDERER_GDIP_H
#ifdef _MSC_VER
#	pragma once
#endif

using namespace Gdiplus;
using namespace gcl::render_objects;

namespace gcl { namespace std_renderer {

enum class ternary_raster_operations {source_copy = SRCCOPY, source_paint = SRCPAINT, source_and = SRCAND, source_invert = SRCINVERT, source_erase = SRCERASE, not_source_copy = NOTSRCCOPY, not_source_erase = NOTSRCERASE, merge_copy = MERGECOPY, merge_paint = MERGEPAINT, pattern_copy = PATCOPY, pattern_paint = PATPAINT, pattern_invert = PATINVERT, destination_invert = DSTINVERT, blackness = BLACKNESS, whiteness = WHITENESS};

#define GDI_FAIL(x) (x)!=Status::Ok

class gdiplus_font : 
	public font
{
public:
	gdiplus_font(const wstring& family_name, float size, int fstyle);
	gdiplus_font(const gdiplus_font&)=delete;
	virtual ~gdiplus_font();
	bool operator==(font* f);
	int get_style() const {return style;}
	float get_size() const {return sz;}
	wstring get_family() const {return family;}
	Font* get_native_font() {return f;}
	StringFormat* get_native_format() {return frm;}
	rect get_metrics(const wstring& str, const size& clip, graphics* g) const;
	vector<wstring> get_available_font_families();
private:
	int style;
	float sz;
	wstring family;
	Font* f;
	StringFormat* frm;
};

class gdiplus_texture :
	public texture
{
public:	gdiplus_texture(const wstring& filename);
	gdiplus_texture(int id, LPWSTR type, HINSTANCE inst);
	gdiplus_texture(Bitmap* bmp);
	gdiplus_texture(const size& sz);
	gdiplus_texture(HBITMAP bmp, HPALETTE hP);
	gdiplus_texture(HICON ico);
	gdiplus_texture(const oem_bitmap& id);
	gdiplus_texture(const gdiplus_texture&)=delete;
	virtual ~gdiplus_texture() {delete img;}
	int get_width() const {return img->GetWidth();}
	int get_height() const {return img->GetHeight();};
	Image* get_native_member() {return img;};
	texture* clone() {return new gdiplus_texture(img);}
    unsigned char* alloc();
	int get_stride();
	void free();
private:
	HRESULT load_texture_resource(int id, LPWSTR type, HINSTANCE inst);
	BitmapData* data;
	Gdiplus::Bitmap* img;
};

class gdiplus_solid_brush : 
	public solid_brush
{
public:
	explicit gdiplus_solid_brush(Color& c);
	gdiplus_solid_brush(const gdiplus_solid_brush&)=delete;
	virtual ~gdiplus_solid_brush() {if(br)delete br;}
	brush_types get_type() {return brush_types::solid_brush;}
	colour get_colour() {return c_.opacity(opacity);}
	void set_colour(const colour& c);
	SolidBrush* get_native_member() {return br;}
	void set_opacity(float val) {opacity = val; set_colour(c_);}
	float get_opacity() const {return opacity;}
private:
	SolidBrush* br;
	float opacity;
	colour c_;
};

class gdiplus_texture_brush : 
	public texture_brush
{
public:
	gdiplus_texture_brush(texture* image);
	gdiplus_texture_brush(const gdiplus_texture_brush&)=delete;
	virtual ~gdiplus_texture_brush() {if(br) delete br;}
	brush_types get_type() {return brush_types::texture_brush;}
	void set_transform(const matrix& m) {if(GDI_FAIL(br->SetTransform(&Matrix(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32)))) throw runtime_error("Unable to set matrix");}
	matrix get_transform() const;
	texture* get_texture() const {return img;}
	TextureBrush* get_native_member() {return br;}
	 
	void set_wrap_mode(const wrap_modes& _mode) {mode = _mode; br->SetWrapMode(static_cast<WrapMode>(_mode));}
	wrap_modes get_wrap_mode() const {return mode;}
	void set_opacity(float) {throw logic_error("Not implemented");}
	float get_opacity() const {return 1.f;}
private:
	TextureBrush* br;
	gdiplus_texture* img;
	wrap_modes mode;
};

class gdiplus_linear_gradient_brush : 
	public linear_gradient_brush
{
public:
	gdiplus_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode);
	gdiplus_linear_gradient_brush(const gdiplus_linear_gradient_brush&)=delete;
	virtual ~gdiplus_linear_gradient_brush();
	brush_types get_type() {return brush_types::linear_gradient_brush;}
	void set_transform(const matrix& m_);
	matrix get_transform() const;
	float get_opacity() const {return opacity;}
	void set_opacity(float val);

	void set_rect(const point& p1, const point& p2);
	rect get_rect() const {return rect(start_p, size(end_p.x, end_p.y));}
	gradient_stop get_gradients() const {return stops;}
	void set_gradients(gradient_stop& gradients);
	LinearGradientBrush* get_native_member() {return br;}
private:
	void create_br(const point& start, const point& end, float* pos, Color* colours, int count, bool gamma, const wrap_modes& wrapmode);
	void create_colour(Color* cl, int count);
	matrix m;
	LinearGradientBrush* br;
	float opacity;
	float* pos_f;
	int cnt;
	Color* colr_f;
	point start_p, end_p;
	bool gamma_;
	wrap_modes mode;
	gradient_stop stops;
};

class gdiplus_radial_gradient_brush :
	public radial_gradient_brush
{
public:
	gdiplus_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma);
	~gdiplus_radial_gradient_brush();
	brush_types get_type() {return brush_types::radial_gradient_brush;}
	void set_transform(const matrix& m_);
	matrix get_transform() const { return m;}
	float get_opacity() const {return opacity;}
	void set_opacity(float val);
	void set_ellipse(const ellipse& e);
	ellipse get_ellipse() const {return elp;}
	void set_gradient_origin_offset(const point& p);
	point get_gradient_origin_offset() const {return offs;}
	gradient_stop get_gradients() const {return stops;}
	void set_gradients(gradient_stop& gradients);
	PathGradientBrush* get_native_member() {return br;}
private:
	void recreate_brush();
	PathGradientBrush* br;
	matrix m;
	float opacity;
	float* pos_f;
	Color* colr_f;
	ellipse elp;
	point offs;
	bool gamma_;
	gradient_stop stops;
};

class gdiplus_pen : 
	public pen
{
public:
	gdiplus_pen(brush* org, Brush* br, float w, dash_cap startcap, dash_cap endcap, dash_cap caps, dash_style dstyle, float offset);
	gdiplus_pen(const gdiplus_pen&)=delete;
	virtual ~gdiplus_pen() {delete p;}
	void update();
	brush* get_brush() const {return org_br;}
	Pen* get_native_member() {return p;}
	dash_cap get_start_cap() const {return start_cap;}
	void set_start_cap(const dash_cap& c) {p->SetStartCap(static_cast<LineCap>(c));}
	dash_cap get_end_cap() const {return end_cap;}
	void set_end_cap(const dash_cap& c) {p->SetEndCap(static_cast<LineCap>(c));}
	dash_cap get_dash_cap() const {return cap;}
	void set_dash_cap(const dash_cap& c) {p->SetDashCap(static_cast<DashCap>(c));}
	dash_style get_dash_style() const {return style;}
	void set_dash_style(const dash_style& d) {p->SetDashStyle(static_cast<DashStyle>(d));}
	float get_dash_offs() const {return dash_offset;}
	void set_dash_offs(float f) {p->SetDashOffset(f);}
	float get_width() const {return width;}
	void set_width(float f) {p->SetWidth(f);}
private:
	Pen* p;
	dash_cap start_cap, end_cap, cap;
	dash_style style;
	float width, dash_offset;
	brush* org_br;
	Brush* gdi_br;
};

class gdiplus_geometry : 
	public geometry
{
public:
	gdiplus_geometry();
	~gdiplus_geometry();
	gdiplus_geometry(const gdiplus_geometry&)=delete;
	void begin_geometry(const point& p);
	void end_geometry();
	bool contains(const point& p, const matrix& m); 
	bool outline_contains(const point& p, pen* pe, const matrix& m);

	void add_bezier(const point& p1, const point& p2, const point& p3);
	void add_beziers(const point* ps, int count);
	void add_line(const point& p1);
	void add_lines(const point* ps, int count);
	void add_rect(const rect& rc);
	void add_ellipse(const ellipse& e);
	void add_rounded_rect(const rect& rc, float radiusX, float radiusY);
	void add_geometry(geometry* geo);
	void add_polygon(const point* ps, int count);

	rect get_bounds(const matrix& transform) const;
	GraphicsPath* get_native_member();
private:
	GraphicsPath* path;
};

class gdiplus_icon :
	public icon
{
public:
	gdiplus_icon(const wstring& filename);
	gdiplus_icon(HICON ico_);
	gdiplus_icon(const system_icon& ico_);
	gdiplus_icon(int id, HINSTANCE inst);
	gdiplus_icon(const wstring& filename, const size& sz);
	gdiplus_icon(int id, HINSTANCE inst, const size& sz);
	gdiplus_icon(const gdiplus_icon&)=delete;
	virtual ~gdiplus_icon() {DestroyIcon(ico);}
	HICON get_icon() const {return ico;};
	size get_size() const {return sz;};
private:
	HICON ico;
	size sz;
	void init_size();
};

// Unsafe
class gdiplus_renderer : 
	public render_objects::graphics 
{
public:
	gdiplus_renderer(HWND handle, callback<void(const size&, const resizing_types&)>& cb);
	gdiplus_renderer(HDC dc);
	gdiplus_renderer(texture* text);
	gdiplus_renderer(const gdiplus_renderer&)=delete;
	virtual ~gdiplus_renderer();
	HDC get_dc() const {return g->GetHDC();}
	void release_dc(HDC dc_) const {g->ReleaseHDC(dc_);}
	void begin();
	void clear(const colour& c);
	void end();
	void bind_dc(HDC dc);

	static colour gdicolor_to_colour(const Color& c);

	void fill_rect(const rect& rc, brush* b);
	void fill_rects(const rect* rcs, int count, brush* b);
	void fill_ellipse(const ellipse& e, brush* b);
	void fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b);
	void fill_geometry(geometry* geo, brush* b);
	void fill_polygon(const point* ps, int count, brush* b);

	void draw_ellipse(const ellipse& e, pen* p);
    void draw_line(const point& p1, const point& p2, pen* p);
	void draw_lines(const point* ps, int count, pen* p);
	void draw_rect(const rect rc, pen* p);
	void draw_rects(const rect* rcs, int count, pen* p);
	void draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p);
	void draw_polygon(const point* ps, int count, pen* p);
	void draw_geometry(geometry* geo, pen* p);
	void draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format);
	void draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format, const horizontal_string_align& clipping_h, const vertical_string_align& clipping_v);
	void draw_texture(texture* image, const point& p, unsigned char opacity);
	void draw_texture(texture* image, const point& p, const size& sz, unsigned char opacity);
	void draw_texture(texture* image, const rect& rc, unsigned char opacity);
	void draw_texture(texture* image, const rect& src, const point& dst, unsigned char opacity);
	void draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest, unsigned char opacity);
	void draw_texture(texture* image, const rect& src, const rect& dest, unsigned char opacity);
	void draw_icon(icon* ico, const point& pos);
	void draw_icon(icon* ico, const point& pos, const size& sz);

	solid_brush* create_solid_brush(const colour& c);
	texture_brush* create_texture_brush(texture* image);
	linear_gradient_brush* create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode);
	pen* create_pen(brush* b, float width, dash_cap startcap, dash_cap endcap, dash_cap dash_cap, dash_style dash_style, float offset);
	texture* create_texture(const wstring& filename);
	texture* create_texture(int id, LPWSTR type, HINSTANCE inst);
	texture* create_texture(const size& sz);
	texture* create_texture(HBITMAP hbmp, HPALETTE hP);
	texture* create_texture(HICON ico);
	texture* create_texture(const oem_bitmap& id);
	graphics* create_graphics(HWND handle, callback<void(const size&, const resizing_types&)>& cb);
	graphics* create_graphics(HDC dc);
	graphics* create_graphics(texture* txt);
	icon* create_icon(const wstring& filename);
	icon* create_icon(HICON ico);
	icon* create_icon(const system_icon& ico);
	icon* create_icon(int id, HINSTANCE inst);
	icon* create_icon(const wstring& filename, const size& sz);
	icon* create_icon(int id, HINSTANCE inst, const size& sz);
	font* create_font(const wstring& family_name, float size, int fstyle);
	geometry* create_geometry();
	radial_gradient_brush* create_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma);
	font* get_system_font(float sz, int fstyle) const;

    void rotate(float angle);
	void rotate_at(float angle, const point& p);
	void translate(float x, float y);
	void scale(float fac_x, float fac_y);
	void shear(float fac_x, float fac_y);
	void skew(float x, float y, float anglex, float angley);
	matrix get_transform() const;
	void set_transform(const matrix& m);

	void set_text_rendering_mode(const text_rendering_modes& mode);
	text_rendering_modes get_text_rendering_mode() const {return textMode;}
	void set_antialias(bool val);
	bool get_antialias() const {return antialias;}
	void push_clip(const clip& cl);
	void pop_clip();
	ternary_raster_operations get_raster_operations() const {return raster_operations;}
	void set_raster_operations(ternary_raster_operations rop) {raster_operations = rop;}
	Graphics* get_graphics() const {return g;}
private:
	size get_surface_size() const;
	Brush* get_brush(brush* br);
	Matrix* get_matrix(const matrix& m);

	HWND handle;
	Image* bmp;
	HDC context;
	HDC buffered_context;
	HBITMAP buffered_surface;
	PAINTSTRUCT ps; 
	Graphics* g;
	ternary_raster_operations raster_operations;
	bool antialias;
	ULONG_PTR tokens;
	GdiplusStartupInput startinput;
	text_rendering_modes textMode;
	graphics_type type;
	static bool is_start_up;
	static size_t inst_count;
	stack<shared_ptr<Region>> clips;
protected:
	void create_resources();
};

};
};

#endif