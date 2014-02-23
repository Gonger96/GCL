/*****************************************************************************
*                           gcl - renderer_gdip.h                            *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#ifndef RENDERER_GDIP_H
#define RENDERER_GDIP_H
#ifdef _MSC_VER
#pragma once
#endif
#include "stdafx.h"
#include "graphics.h"
#include "system.h"
#include <Shlwapi.h>
using namespace Gdiplus;
using namespace gcl::render_objects;

namespace gcl { namespace std_renderer {

enum class ternary_raster_operations {source_copy = SRCCOPY, source_paint = SRCPAINT, source_and = SRCAND, source_invert = SRCINVERT, source_erase = SRCERASE, not_source_copy = NOTSRCCOPY, not_source_erase = NOTSRCERASE, merge_copy = MERGECOPY, merge_paint = MERGEPAINT, pattern_copy = PATCOPY, pattern_paint = PATPAINT, pattern_invert = PATINVERT, destination_invert = DSTINVERT, blackness = BLACKNESS, whiteness = WHITENESS};

#define GDI_FAIL(x) (x)!=Status::Ok

class gdiplus_font : 
	public font
{
public:
	gdiplus_font(const wstring& family_name, float size, int fstyle) : f(0), family(family_name), sz(size), style(fstyle), frm(new StringFormat())
	{
		f = new Font(&FontFamily(family_name.c_str()), size, fstyle, UnitPixel);
		if(!f->IsAvailable()) throw invalid_argument("Font is not available");
	}
	gdiplus_font(const gdiplus_font&)=delete;
	bool operator==(font* f) {return (family == f->get_family() && sz == f->get_size() && style == f->get_style());};
	int get_style() const {return style;};
	float get_size() const {return sz;};
	wstring get_family() const {return family;};
	void release_resources() {delete f; f= 0; delete frm; frm = 0;};
	Font* get_native_font() {return f;};
	StringFormat* get_native_format() {return frm;};
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
public:
	gdiplus_texture(const wstring& filename) : data(0) {img = new Bitmap(filename.c_str()); if(!img) throw invalid_argument("Unable to initialize Image");};
	gdiplus_texture(int id, LPWSTR type, HINSTANCE inst) : data(0) {if(FAILED(load_texture_resource(id, type, inst))) throw invalid_argument("Unable to initialize Image");};
	gdiplus_texture(Bitmap* bmp) : data(0)
	{
		img = bmp->Clone(0.f, 0.f, (float)bmp->GetWidth(), (float)bmp->GetHeight(), PixelFormat32bppPARGB);
		if(!img)
			throw runtime_error("Unable to copy Bitmap");
	};
	gdiplus_texture(const gdiplus_texture&)=delete;
	int get_width() const {return img->GetWidth();};
	int get_height() const {return img->GetHeight();};
	Image* get_native_member() {return img;};
	void release_resources() {delete img; img = 0;};
	texture* clone()
	{
		return new gdiplus_texture(img);
	};
    unsigned char* alloc()
	{
		if(data)
			throw invalid_argument("Data already allocated");
		data = new BitmapData();
		if(GDI_FAIL(img->LockBits(&Gdiplus::Rect(0, 0, img->GetWidth(), img->GetHeight()), ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppPARGB, data)))
			throw runtime_error("Unable to lock Bits");
		return static_cast<unsigned char*>(data->Scan0);
	};
	int get_stride()
	{
		if(!data)
			throw invalid_argument("Data isn't allocated yet");
		return data->Stride;
	};
	void free()
	{
		if(!data)
			throw invalid_argument("Data isn't allocated yet");
		img->UnlockBits(data);
		delete data;
		data = 0;
	};
private:
	HRESULT load_texture_resource(int id, LPWSTR type, HINSTANCE inst)
	{
		void* data = 0;
		HRSRC res = resource::alloc(inst, id, type, &data);
		IStream* stream = 0;
		DWORD len = SizeofResource(inst, res);
		stream = SHCreateMemStream(static_cast<unsigned char*>(data), len);
		if(stream)
		{
			img = new Bitmap(stream);
			if(!img)
				throw runtime_error("Unable to create Bitmap");
			stream->Release();
		}
		resource::free(res);
		return (stream ? S_OK : E_FAIL);
	};
	BitmapData* data;
	Bitmap* img;
};

class gdiplus_solid_brush : 
	public solid_brush
{
public:
	explicit gdiplus_solid_brush(Color& c) : opacity(1.0f) {br = new SolidBrush(c); c_ = colour(c.GetR(), c.GetG(), c.GetB(), c.GetA());};
	gdiplus_solid_brush(const gdiplus_solid_brush&)=delete;
	void release_resources() {delete br; br = 0;};
	brush_types get_type() {return brush_types::solid_brush;};
	colour get_colour() {return c_.opacity(opacity);};
	void set_colour(const colour& c) {c_ = c; if(GDI_FAIL(br->SetColor(Color(c.opacity(opacity).get_value())))) throw runtime_error("Unable to set colour of GDI+ SolidBrush");};
	SolidBrush* get_native_member() {return br;};
	void set_opacity(float val) {opacity = val; set_colour(c_);};
	float get_opacity() const {return opacity;};
private:
	SolidBrush* br;
	float opacity;
	colour c_;
};

class gdiplus_texture_brush : 
	public texture_brush
{
public:
	gdiplus_texture_brush(texture* image) : img(dynamic_cast<gdiplus_texture*>(image)), mode(wrap_modes::wrap) {br = new TextureBrush(img->get_native_member(), WrapModeTile);};
	gdiplus_texture_brush(const gdiplus_texture_brush&)=delete;
	brush_types get_type() {return brush_types::texture_brush;};
	void release_resources() {delete br; br = 0;};
	void set_transform(const matrix& m) {if(GDI_FAIL(br->SetTransform(&Matrix(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32)))) throw runtime_error("Unable to set matrix");};
	matrix get_transform() const 
	{
		Matrix m; 
		if(GDI_FAIL(br->GetTransform(&m))) 
			throw runtime_error("Unable to receive Matrix");
		float f[5] = {};
		if(GDI_FAIL(m.GetElements(f))) 
			throw runtime_error("Unable to get Elements");
		return matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
	};
	texture* get_texture() const {return img;};
	TextureBrush* get_native_member() {return br;};
	 
	void set_wrap_mode(const wrap_modes& _mode) {mode = _mode; br->SetWrapMode(static_cast<WrapMode>(_mode));};
	wrap_modes get_wrap_mode() const {return mode;};
	void set_opacity(float) {throw not_implemented();};
	float get_opacity() const {return 1.f;};
private:
	TextureBrush* br;
	gdiplus_texture* img;
	wrap_modes mode;
};

class gdiplus_linear_gradient_brush : 
	public linear_gradient_brush
{
public:
	gdiplus_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode) : opacity(1.f), stops(gradients), start_p(start), end_p(end), gamma_(gamma), mode(wrapmode), cnt(gradients.get_count()), m(matrix::identity())
	{
		colr_f = new Color[gradients.get_count()];
		pos_f = new float[gradients.get_count()];
		for(unsigned int i = 0; i < gradients.get_count(); i++)
		{
			colr_f[i] = Color(gradients.get_colours()[i].get_value());
			pos_f[i] = gradients.get_positions()[i];
		}
		create_colour(colr_f, gradients.get_count());
		create_br(start, end, pos_f, colr_f, gradients.get_count(), gamma, wrapmode);
	};
	gdiplus_linear_gradient_brush(const gdiplus_linear_gradient_brush&)=delete;
	brush_types get_type() {return brush_types::linear_gradient_brush;};
	void release_resources() {delete br; delete[] pos_f; delete[] colr_f; br = 0; pos_f = 0; colr_f = 0;};
	void set_transform(const matrix& m_) 
	{
		m = m_;
		Matrix mold;
		br->GetTransform(&mold);
		float f[6] = {};
		if(GDI_FAIL(mold.GetElements(f))) throw runtime_error("Unable to get Matrixelements");
		matrix mnew = m * matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
		if(GDI_FAIL(br->SetTransform(&Matrix(mnew.m11, mnew.m12, mnew.m21, mnew.m22, mnew.m31, mnew.m32)))) throw runtime_error("Unable to set matrix");
	};
	matrix get_transform() const 
	{
		Matrix m; 
		if(GDI_FAIL(br->GetTransform(&m))) 
			throw runtime_error("Unable to receive Matrix");
		float f[6] = {};
		if(GDI_FAIL(m.GetElements(f))) 
			throw runtime_error("Unable to get Elements");
		return matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
	};
	float get_opacity() const {return opacity;};
	void set_opacity(float val) {opacity = val; create_colour(colr_f, cnt); create_br(start_p, end_p, pos_f, colr_f, cnt, gamma_, mode);};

	void set_rect(const point& p1, const point& p2) {start_p = p1; end_p = p2; create_br(p1, p2, pos_f, colr_f, cnt, gamma_, mode);};
	rect get_rect() const {return rect(start_p, size(end_p.x, end_p.y));};
	gradient_stop get_gradients() const {return stops;};
	void set_gradients(gradient_stop& gradients)
	{
		delete[] colr_f;
		colr_f = new Color[gradients.get_count()];
		delete[] pos_f;
		pos_f = new float[gradients.get_count()];
		for(unsigned int i = 0; i < gradients.get_count(); i++)
		{
			colr_f[i] = Color(gradients.get_colours()[i].get_value());
			pos_f[i] = gradients.get_positions()[i];
		}
		create_colour(colr_f, gradients.get_count());
		br->SetInterpolationColors(colr_f, pos_f, gradients.get_count());
	};
	LinearGradientBrush* get_native_member() {return br;};
private:
	void create_br(const point& start, const point& end, float* pos, Color* colours, int count, bool gamma, const wrap_modes& wrapmode)
	{
		br = new LinearGradientBrush(PointF(start.x, start.y), PointF(end.x, end.y+1), Color(Color::White), Color(Color::White));
		if(GDI_FAIL(br->SetGammaCorrection(gamma))) throw invalid_argument("Unable to set Gammacorrection");
		if(GDI_FAIL(br->SetWrapMode(static_cast<WrapMode>(wrapmode)))) throw invalid_argument("Unable to set WrapMode");
		if((br->SetInterpolationColors(colours, pos, count))) throw invalid_argument("Unable to set Interpolationcolors");
		Matrix mold;
		br->GetTransform(&mold);
		float f[6] = {};
		if(GDI_FAIL(mold.GetElements(f))) throw runtime_error("Unable to get Matrixelements");
		matrix mnew = m * matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
		if(GDI_FAIL(br->SetTransform(&Matrix(mnew.m11, mnew.m12, mnew.m21, mnew.m22, mnew.m31, mnew.m32)))) throw runtime_error("Unable to set matrix");
	};
	void create_colour(Color* cl, int count)
	{
		for(int i = 0; i < count; i++)
		{
			colr_f[i] = Color(static_cast<BYTE>(cl[i].GetAlpha()*opacity), cl[i].GetRed(), cl[i].GetGreen(), cl[i].GetBlue());
		}
	};
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

class gdiplus_pen : 
	public pen
{
public:
	gdiplus_pen(brush* org, Brush* br, float w, pen_align algn = pen_align::center, dash_cap startcap = dash_cap::flat, dash_cap endcap = dash_cap::flat, dash_cap caps = dash_cap::flat, dash_style dstyle = dash_style::solid, float offset = 0) : org_br(org), width(w), align(algn), start_cap(startcap), end_cap(endcap), cap(caps), style(dstyle), dash_offset(offset) 
	{
		p = new Pen(br, width);
		p->SetAlignment(static_cast<PenAlignment>(align));
		p->SetDashCap(static_cast<DashCap>(caps));
		p->SetDashOffset(offset);
		p->SetDashStyle(static_cast<DashStyle>(style));
		p->SetEndCap(static_cast<LineCap>(endcap)); 
		p->SetStartCap(static_cast<LineCap>(start_cap));
	};
	gdiplus_pen(const gdiplus_pen&)=delete;
	void release_resources() {delete p; p = 0;};
	brush* get_brush() const {return org_br;};
	Pen* get_native_member() {return p;};
	dash_cap get_start_cap() const {return start_cap;};
	void set_start_cap(const dash_cap& c) {p->SetStartCap(static_cast<LineCap>(c));};
	dash_cap get_end_cap() const {return end_cap;};
	void set_end_cap(const dash_cap& c) {p->SetEndCap(static_cast<LineCap>(c));};
	pen_align get_align() const {return align;};
	void set_align(const pen_align& a) {p->SetAlignment(static_cast<PenAlignment>(a));};
	dash_cap get_dash_cap() const {return cap;};
	void set_dash_cap(const dash_cap& c) {p->SetDashCap(static_cast<DashCap>(c));};
	dash_style get_dash_style() const {return style;};
	void set_dash_style(const dash_style& d) {p->SetDashStyle(static_cast<DashStyle>(d));};
	float get_dash_offs() const {return dash_offset;};
	void set_dash_offs(float f) {p->SetDashOffset(f);};
	float get_width() const {return width;};
	void set_width(float f) {p->SetWidth(f);};
private:
	Pen* p;
	pen_align align;
	dash_cap start_cap, end_cap, cap;
	dash_style style;
	float width, dash_offset;
	brush* org_br;
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
	bool contains(const point& p); 
	bool outline_contains(const point& p, pen* pe);
	void release_resources();

	void add_bezier(const point& p1, const point& p2, const point& p3);
	void add_beziers(const point* ps, int count);
	void add_line(const point& p1);
	void add_lines(const point* ps, int count);
	void add_rect(const rect& rc);
	void add_ellipse(const ellipse& e);
	void add_rounded_rect(const rect& rc, float radiusX, float radiusY);

	void transform(const matrix& m);

	GraphicsPath* get_native_member();
private:
	GraphicsPath* path;
};

// Unsafe
class gdiplus_renderer : 
	public render_objects::graphics 
{
public:
	gdiplus_renderer(HWND handle, callback<void(const size&)>& cb);
	gdiplus_renderer(const gdiplus_renderer&)=delete;
	HDC get_dc() const {return g->GetHDC();};
	void begin();
	void clear(const colour& c);
	void end();

	static colour gdicolor_to_colour(const Color& c);

	void fill_rect(const rect& rc, brush* b);
	void fill_rects(const rect* rcs, int count, brush* b);
	void fill_ellipse(const ellipse& e, brush* b);
	void fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b);
	void fill_geometry(geometry* geo, brush* b);

	void draw_ellipse(const ellipse& e, pen* p);
    void draw_line(const point& p1, const point& p2, pen* p);
	void draw_lines(const point* ps, int count, pen* p);
	void draw_rect(const rect rc, pen* p);
	void draw_rects(const rect* rcs, int count, pen* p);
	void draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p);
	void draw_geometry(geometry* geo, pen* p);
	void draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format);
	void draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format, const horizontal_string_align& clipping_h, const vertical_string_align& clipping_v);
	void draw_texture(texture* image, const point& p);
	void draw_texture(texture* image, const point& p, const size& sz);
    void draw_texture(texture* image, const rect& rc);
    void draw_texture(texture* image, const rect& src, const point& dst);
	void draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest);
	void draw_texture(texture* image, const rect& src, const rect& dest);

	solid_brush* create_solid_brush(const colour& c);
	texture_brush* create_texture_brush(texture* image);
	linear_gradient_brush* create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode);
	pen* create_pen(brush* b, float width, pen_align align, dash_cap startcap, dash_cap endcap, dash_cap dash_cap, dash_style dash_style, float offset);
	texture* create_texture(const wstring& filename);
	texture* create_texture(int id, LPWSTR type, HINSTANCE inst);
	font* create_font(const wstring& family_name, float size, int fstyle);
	geometry* create_geometry();

    void rotate(float angle);
	void rotate_at(float angle, const point& p);
	void translate(float x, float y);
	void scale(float fac_x, float fac_y);
	void shear(float fac_x, float fac_y);
	void skew(float x, float y, float anglex, float angley);
	matrix get_transform() const;
	void set_transform(const matrix& m);

	void set_text_rendering_mode(const text_rendering_modes& mode);
	text_rendering_modes get_text_rendering_mode() const {return textMode;};
	void set_antialias(bool val);
	bool get_antialias() const {return antialias;};

	ternary_raster_operations get_raster_operations() const {return raster_operations;};
	void set_raster_operations(ternary_raster_operations rop) {raster_operations = rop;};
private:
	size get_surface_size() const;
	Brush* get_brush(brush* br);
	Matrix* get_matrix(const matrix& m);
	void release_resources();

	HWND handle;
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
protected:
	void create_resources();
};

};
};

#endif