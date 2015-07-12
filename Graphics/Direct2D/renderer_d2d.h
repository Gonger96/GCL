/*****************************************************************************
*                         GCL - renderer_d2d.h                               *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"
#include "graphics.h"
#include "callback.h"
#include "system.h"
#include <wincodec.h>
#include <dwrite.h>

#ifndef RENDERER_D2D_H
#define RENDERER_D2D_H
#ifdef _MSC_VER
#	pragma once
#endif

using namespace gcl::render_objects;
using namespace D2D1;
namespace gcl { namespace std_renderer {

class d2d_font : 
	public font
{
public:
	d2d_font(const wstring& family_name, float size, int fstyle, ID2D1Factory* tg, IDWriteFactory* fctry);
	d2d_font(const d2d_font&)=delete;
	virtual ~d2d_font() {format->Release();}
	int get_style() const {return style;}
	float get_size() const {return sz;}
	wstring get_family() const {return family;}
	bool operator==(font* f) {return (family == f->get_family() && sz == f->get_size() && style == f->get_style());}
	IDWriteTextFormat* get_native_member() {return format;}
	rect get_metrics(const wstring& str, const size& clip, graphics* g) const;
	vector<wstring> get_available_font_families();
private:
	wstring family;
	float sz;
	int style;

	ID2D1Factory* target;
	IDWriteFactory* factory;
	IDWriteTextFormat* format;
};

class d2d_texture :
	public texture
{
public:
	d2d_texture(const wstring& filename, IWICImagingFactory* _factory, ID2D1RenderTarget* _target);
	d2d_texture(int id, HINSTANCE inst, LPWSTR type, IWICImagingFactory* _factory, ID2D1RenderTarget* _target);
	d2d_texture(IWICBitmap* wic_bmp, IWICImagingFactory* w_factory, ID2D1Bitmap* d2d_bmp, ID2D1RenderTarget* _target);
	d2d_texture(const size& sz, IWICImagingFactory* _factory, ID2D1RenderTarget* _target);
	d2d_texture(HBITMAP hbmp, HPALETTE hp, IWICImagingFactory* _factory, ID2D1RenderTarget* _target);
	d2d_texture(HICON ico, IWICImagingFactory* _factory, ID2D1RenderTarget* _target);
	d2d_texture(const oem_bitmap& id, IWICImagingFactory* _factory, ID2D1RenderTarget* _target);
	virtual ~d2d_texture();
	d2d_texture(const d2d_texture&)=delete;
	int get_width() const {return static_cast<int>(bmp->GetSize().width);}
	int get_height() const {return static_cast<int>(bmp->GetSize().height);}
	ID2D1Bitmap* get_native_member() {return bmp;}
	texture* clone() {return new d2d_texture(w_bmp, factory, bmp, target);}
	unsigned char* alloc();
	void free();
	int get_stride();
private:
	ID2D1Bitmap* bmp;
	IWICBitmap* w_bmp;
	IWICBitmapLock* lock;
	IWICImagingFactory* factory;
	ID2D1RenderTarget* target;

	HRESULT load_texture_file(const wstring& path, ID2D1Bitmap** bmp);
	HRESULT load_texture_resource(int id, LPWSTR type, HINSTANCE inst, ID2D1Bitmap** bmp);
	HRESULT load_texture_from_texture(IWICBitmap* wic_bmp, ID2D1Bitmap* d2d_bmp);
};

class d2d_solid_brush : 
	public solid_brush
{
public:
	d2d_solid_brush(ID2D1RenderTarget* target, D2D1::ColorF& c);
	d2d_solid_brush(const d2d_solid_brush&)=delete;
	virtual ~d2d_solid_brush() {br->Release();}
	brush_types get_type() {return brush_types::solid_brush;}
	colour get_colour();
	void set_colour(const colour& c) {br->SetColor(ColorF(c.r/255.f, c.g/255.f, c.b/255.f, c.a / 255.0f));}
	ID2D1SolidColorBrush* get_native_member() {return br;}
	void set_opacity(float val) {opacity = val; br->SetOpacity(val);}
	float get_opacity() const {return opacity;}
private:
	ID2D1SolidColorBrush* br;
	ID2D1RenderTarget* t;
	float opacity;
};

class d2d_texture_brush : 
	public texture_brush
{
public:
	d2d_texture_brush(texture* img, ID2D1RenderTarget* _target);
	d2d_texture_brush(const d2d_texture_brush&)=delete;
	virtual ~d2d_texture_brush() {bmp_brush->Release();}
	brush_types get_type() {return brush_types::texture_brush;};
	void set_transform(const matrix& m) {bmp_brush->SetTransform(Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32));}
	matrix get_transform() const;
	
	void set_wrap_mode(const wrap_modes& mode);
	wrap_modes get_wrap_mode() const {return mde;}

	texture* get_texture() const {return image;}
	ID2D1BitmapBrush* get_native_member() {return bmp_brush;}

	void set_opacity(float val) {opacity = val; bmp_brush->SetOpacity(val);}
	float get_opacity() const {return opacity;}
private:
	wrap_modes mde;
	d2d_texture* image;
	float opacity;
	ID2D1BitmapBrush* bmp_brush;
	ID2D1RenderTarget* target;
};

class d2d_linear_gradient_brush : 
	public linear_gradient_brush
{
public:
	d2d_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, ID2D1RenderTarget* target_, bool gamma , const wrap_modes& wrapmode);
	d2d_linear_gradient_brush(const d2d_linear_gradient_brush&)=delete;
	virtual ~d2d_linear_gradient_brush() {br->Release();}
	brush_types get_type() {return brush_types::linear_gradient_brush;}
	void set_transform(const matrix& m) {br->SetTransform(Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32));}
	matrix get_transform() const;
	float get_opacity() const {return br->GetOpacity();}
	void set_opacity(float val) {br->SetOpacity(val);}
	gradient_stop get_gradients() const {return stops;}
	void set_gradients(gradient_stop& gradients);
	void set_rect(const point& p1, const point& p2);
	rect get_rect() const;
	ID2D1LinearGradientBrush* get_native_member() {return br;}
private:
	ID2D1RenderTarget* target;
	ID2D1LinearGradientBrush* br;
	gradient_stop stops;
};

class d2d_radial_gradient_brush :
	public radial_gradient_brush
{
public:
	d2d_radial_gradient_brush(ID2D1RenderTarget* trgt, const ellipse& e, const gradient_stop& gradients, bool gamma);
	~d2d_radial_gradient_brush() {br->Release();}
	brush_types get_type() {return brush_types::radial_gradient_brush;}
	void set_transform(const matrix& m_);
	matrix get_transform() const {return m;}
	float get_opacity() const {return opacity;}
	void set_opacity(float val);
	void set_ellipse(const ellipse& e);
	ellipse get_ellipse() const {return elp;}
	void set_gradient_origin_offset(const point& p);
	point get_gradient_origin_offset() const {return offs;}
	gradient_stop get_gradients() const {return stops;}
	void set_gradients(gradient_stop& gradients);
	ID2D1RadialGradientBrush* get_native_member() {return br;}
private:
	void recreate_brush();
	matrix m;
	float opacity;
	ellipse elp;
	point offs;
	bool gamma_;
	gradient_stop stops;
	ID2D1RenderTarget* target;
	ID2D1RadialGradientBrush* br;
};

class d2d_geometry : 
	public geometry
{
public:
	d2d_geometry(ID2D1Factory* fct);
	d2d_geometry(const d2d_geometry&)=delete;
	virtual ~d2d_geometry();
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
	void add_polygon(const point* ps, int count);
	void add_geometry(geometry* geo);

	rect get_bounds(const matrix& transform) const;
	ID2D1Geometry* get_native_member();
private:
	ID2D1PathGeometry* geo;
	ID2D1GeometrySink* geo_sink;
	ID2D1Factory* factory;
	ID2D1RenderTarget* target;
	point lastPoint;
};

class d2d_pen : 
	public pen
{
public:
	d2d_pen(brush* org, ID2D1Brush* br, ID2D1Factory* f, float w, dash_cap startcap, dash_cap endcap, dash_cap caps, dash_style dstyle, float offset);
	virtual ~d2d_pen() {stroke_style->Release();}
	ID2D1Brush* get_native_member() {return brush_;}
	ID2D1StrokeStyle* get_stroke_style() {return stroke_style;}

	brush* get_brush() const {return org_br;}

	dash_cap get_start_cap() const {return start_cap;}
	void update() {}
	void set_start_cap(const dash_cap& p) {start_cap = p; recreate();}
	dash_cap get_end_cap() const {return end_cap;}
	void set_end_cap(const dash_cap& p) {end_cap = p; recreate();}
	dash_cap get_dash_cap() const {return cap;}
	void set_dash_cap(const dash_cap& c) {cap = c; recreate();}
	dash_style get_dash_style() const {return style;}
	void set_dash_style(const dash_style& d) {style = d; recreate();}
	float get_dash_offs() const {return dash_offset;}
	void set_dash_offs(float f)  {dash_offset = f; recreate();}
	float get_width() const {return width;}
	void set_width(float f) {width = f; recreate();}
private:
	void recreate();
	dash_cap start_cap, end_cap, cap;
	dash_style style;
	float width, dash_offset;
	ID2D1Brush* brush_;
	ID2D1StrokeStyle* stroke_style;
	ID2D1Factory* factory;
	brush* org_br;
};

class d2d_icon :
	public icon
{
public:
	d2d_icon(const wstring& filename, IWICImagingFactory* fact, ID2D1RenderTarget* _target);
	d2d_icon(HICON  ico_, IWICImagingFactory* fact, ID2D1RenderTarget* _target);
	d2d_icon(const system_icon& ico_, IWICImagingFactory* fact, ID2D1RenderTarget* _target);
	d2d_icon(int id, HINSTANCE inst, IWICImagingFactory* fact, ID2D1RenderTarget* _target);
	d2d_icon(const wstring& filename, const size& sz, IWICImagingFactory* fact, ID2D1RenderTarget* _target);
	d2d_icon(int id, HINSTANCE inst, const size& sz, IWICImagingFactory* fact, ID2D1RenderTarget* _target);
	d2d_icon(const d2d_icon&)=delete;
	virtual ~d2d_icon();
	HICON get_icon() const {return ico;};
	ID2D1Bitmap* get_native_member() {return bmp;};
	size get_size() const;
private:
	void init_from_ico();
	ID2D1RenderTarget* target;
	IWICImagingFactory* factory;
	ID2D1Bitmap* bmp;
	HICON ico;
};

class direct2d_renderer : 
	public render_objects::graphics
{
public:
	direct2d_renderer(HWND hWnd, callback<void(const size&, const resizing_types&)>& target_resize);
	direct2d_renderer(direct2d_renderer* old_renderer, d2d_texture* text);
	direct2d_renderer(HDC dc);
	direct2d_renderer(const direct2d_renderer&) = delete;
	ID2D1RenderTarget* get_current_target() const;
	virtual ~direct2d_renderer();
	void begin();
	void clear(const colour& c);
	void end();
	void bind_dc(HDC dc);

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
	void draw_geometry(geometry* geo, pen* p);

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
	icon* create_icon(const wstring& filename);
	icon* create_icon(HICON ico);
	icon* create_icon(const system_icon& ico);
	icon* create_icon(int id, HINSTANCE inst);
	icon* create_icon(const wstring& filename, const size& sz);
	icon* create_icon(int id, HINSTANCE inst, const size& sz);
	font* create_font(const wstring& family_name, float size, int fstyle);
	geometry* create_geometry();
	radial_gradient_brush* create_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma);
	graphics* create_graphics(HWND handle, callback<void(const size&, const resizing_types&)>& cb);
	graphics* create_graphics(texture* txt);
	graphics* create_graphics(HDC dc);
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
	text_rendering_modes get_text_rendering_mode() const;
	void set_antialias(bool val);
	bool get_antialias() const {return antialias;}
	void push_clip(const clip& cl);
	void pop_clip();
private:
	void target_resizes(const size&, const resizing_types&);
	ID2D1Brush* get_brush(brush* b);
	ID2D1Brush* get_pen(const pen& p);
	ID2D1StrokeStyle* get_stroke_style(const pen& p);
	D2D1_RECT_F get_rect(const rect& rc);
	D2D1_POINT_2F get_point(const point& p);
	Matrix3x2F get_matrix(const matrix& m);
	graphics_type type;

	static ID2D1Factory* factory;
	static int inst_count;
	ID2D1HwndRenderTarget* hwnd_render_target;
	ID2D1BitmapRenderTarget* bmp_render_target;
	ID2D1DCRenderTarget* dc_render_target;
	d2d_texture* bmp;
	static IWICImagingFactory* imgfactory;
	static IDWriteFactory* write_factory;
	HWND handle;
	PAINTSTRUCT ps;
	HDC dc;
	text_rendering_modes textMode;
	bool antialias;
protected:
	void create_resources();
};


};
};
#endif