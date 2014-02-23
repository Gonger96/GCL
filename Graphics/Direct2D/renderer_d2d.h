/*****************************************************************************
*                         gcl - renderer_d2d.h                               *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*****************************************************************************/
#ifndef RENDERER_D2D_H
#define RENDERER_D2D_H
#ifdef _MSC_VER
#pragma once
#endif
#include "stdafx.h"
#include "graphics.h"
#include "callback.h"
#include "system.h"
#include <wincodec.h>
#include <dwrite.h>
using namespace gcl::render_objects;
using namespace D2D1;
namespace gcl { namespace std_renderer {

class d2d_font : 
	public font
{
public:
	d2d_font(const wstring& family_name, float size, int fstyle, ID2D1Factory* tg, IDWriteFactory* fctry) : target(tg), factory(fctry), family(family_name), sz(size), style(fstyle)
	{
		DWRITE_FONT_STYLE fontStyle;
		DWRITE_FONT_WEIGHT fontWeight;
		if((fstyle & font_style::italic) == font_style::italic)
			fontStyle = DWRITE_FONT_STYLE_ITALIC;
		else if((fstyle & font_style::regular) == font_style::regular)
			fontStyle = DWRITE_FONT_STYLE_NORMAL;
		if((fstyle & font_style::bold) == font_style::bold)
			fontWeight = DWRITE_FONT_WEIGHT_BOLD;
		else
			fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
		if(FAILED(factory->CreateTextFormat(family_name.c_str(), NULL, fontWeight, fontStyle, DWRITE_FONT_STRETCH_NORMAL, sz, string_to_unicode_string(locale().name()).c_str(), &format))) throw runtime_error("Unable to create TextFormat");
	};
	d2d_font(const d2d_font&)=delete;
	int get_style() const {return style;};
	float get_size() const {return sz;};
	wstring get_family() const {return family;};
	void release_resources() {format->Release(); format = 0;};
	bool operator==(font* f) {return (family == f->get_family() && sz == f->get_size() && style == f->get_style());};
	IDWriteTextFormat* get_native_member() {return format;};
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
	d2d_texture(const wstring& filename, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target), bmp(0), w_bmp(0), lock(0) {if(FAILED(load_texture_file(filename, &bmp))) throw invalid_argument("Unable to load Bitmap");};
	d2d_texture(int id, HINSTANCE inst, LPWSTR type, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target), bmp(0), w_bmp(0), lock(0) {if(FAILED(load_texture_resource(id, type, inst, &bmp))) throw invalid_argument("Unable to load Bitmap");};
	d2d_texture(IWICBitmap* wic_bmp, IWICImagingFactory* w_factory, ID2D1Bitmap* d2d_bmp, ID2D1RenderTarget* _target) : factory(w_factory), target(_target), lock(0)
	{
		if(FAILED(load_texture_from_texture(wic_bmp, d2d_bmp))) throw runtime_error("Unable to copy Bitmap");
	};
	d2d_texture(const d2d_texture&)=delete;
	int get_width() const {return static_cast<int>(bmp->GetSize().width);};
	int get_height() const {return static_cast<int>(bmp->GetSize().height);};
	ID2D1Bitmap* get_native_member() {return bmp;};
	void release_resources()
	{
		bmp->Release(); 
		w_bmp->Release(); 
		bmp = 0; 
		w_bmp = 0;
	};
	texture* clone()
	{
		return new d2d_texture(w_bmp, factory, bmp, target);
	};

	unsigned char* alloc() 
	{
		if(lock)
			throw invalid_argument("Data already allocated");
		WICRect rc = {0, 0, get_width(), get_height()};
		if(FAILED(w_bmp->Lock(&rc, WICBitmapLockRead | WICBitmapLockWrite, &lock)))
			throw runtime_error("Unable to lock Data");
		WICInProcPointer ptr = 0;
		UINT outBuffer = 0;
		if(FAILED(lock->GetDataPointer(&outBuffer, &ptr))) 
			throw runtime_error("Unable to receive DataPointer");
		return ptr;
	};
	void free() 
	{
		if(!lock)
			throw invalid_argument("Data isn't allocated yet");
		lock->Release();
		lock = 0;
		bmp->Release();
		bmp = 0;
		if(FAILED(target->CreateBitmapFromWicBitmap(w_bmp, &bmp)))
			throw runtime_error("Unable to update Bitmap");
	};
	int get_stride() 
	{
		if(!lock)
			throw invalid_argument("Data isn't allocated yet");
		UINT num = 0;
		if(FAILED(lock->GetStride(&num)))
			throw runtime_error("Unable to receive Stride");
		return static_cast<int>(num);
	};
private:
	ID2D1Bitmap* bmp;
	IWICBitmap* w_bmp;
	IWICBitmapLock* lock;
	IWICImagingFactory* factory;
	ID2D1RenderTarget* target;

	HRESULT load_texture_file(const wstring& path, ID2D1Bitmap** bmp)
	{
		IWICBitmapDecoder* decoder = 0;
		IWICBitmapFrameDecode* source = 0;
		IWICFormatConverter* converter = 0;

		HRESULT hr = factory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
		if(SUCCEEDED(hr))
			hr = decoder->GetFrame(0, &source);
		if(SUCCEEDED(hr))
			hr = factory->CreateFormatConverter(&converter);
		if(SUCCEEDED(hr))
			hr = converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
		if(SUCCEEDED(hr))
			hr = target->CreateBitmapFromWicBitmap(converter, NULL, bmp);
		if(SUCCEEDED(hr))
			hr = factory->CreateBitmapFromSource(converter, WICBitmapCacheOnLoad, &w_bmp);
		if(decoder)decoder->Release();
		if(source)source->Release();
		if(converter)converter->Release();
		return hr;
	};
	HRESULT load_texture_resource(int id, LPWSTR type, HINSTANCE inst, ID2D1Bitmap** bmp)
	{
		void* data = 0;
		IWICFormatConverter* converter = 0;
		IWICStream* stream = 0;
		IWICBitmapDecoder* decoder = 0;
		IWICBitmapFrameDecode* frameDecoder = 0;
		HRSRC res = resource::alloc(inst, id, type, &data);
		
		HRESULT hr = factory->CreateStream(&stream);
		DWORD imgSize = SizeofResource(inst, res);
		if(SUCCEEDED(hr))
			hr = stream->InitializeFromMemory(static_cast<unsigned char*>(data), imgSize);
		if(SUCCEEDED(hr))
			hr = factory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
		if(SUCCEEDED(hr))
			hr = decoder->GetFrame(0, &frameDecoder);
		if(SUCCEEDED(hr))
			hr = factory->CreateFormatConverter(&converter);
		if(SUCCEEDED(hr))
			hr = converter->Initialize(frameDecoder, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
		if(SUCCEEDED(hr)) 
			hr = target->CreateBitmapFromWicBitmap(converter, NULL,  bmp);
		if(SUCCEEDED(hr))
			hr = factory->CreateBitmapFromSource(converter, WICBitmapCacheOnLoad, &w_bmp);
		if(converter)converter->Release();
		if(stream)stream->Release();
		if(decoder)decoder->Release();
		if(frameDecoder)frameDecoder->Release();
		resource::free(res);
		return hr;
	};
	HRESULT load_texture_from_texture(IWICBitmap* wic_bmp, ID2D1Bitmap* d2d_bmp)
	{
		HRESULT	hr = target->CreateBitmapFromWicBitmap(wic_bmp, &bmp);
		if(SUCCEEDED(hr))
			hr = factory->CreateBitmapFromSource(wic_bmp, WICBitmapCacheOnLoad, &w_bmp);
		return hr;
	};
};

class d2d_solid_brush : 
	public solid_brush
{
public:
	d2d_solid_brush(ID2D1RenderTarget* target, D2D1::ColorF& c) : t(target), opacity(1.0f) {if(FAILED(target->CreateSolidColorBrush(c, &br))) throw runtime_error("Unable to create D2D_SolidBrush");};
	d2d_solid_brush(const d2d_solid_brush&)=delete;
	void release_resources() {br->Release(); br = 0;};
	brush_types get_type() {return brush_types::solid_brush;};
	colour get_colour() {auto c = br->GetColor(); return colour(static_cast<int>(c.a*255.0f), static_cast<int>(c.r*255), static_cast<int>(c.g*255), static_cast<int>(c.b*255));};
	void set_colour(const colour& c) {br->SetColor(ColorF(c.r, c.g, c.b, c.a / 255.0f));};
	ID2D1SolidColorBrush* get_native_member() {return br;};
	void set_opacity(float val) {opacity = val; br->SetOpacity(val);};
	float get_opacity() const {return opacity;};
private:
	ID2D1SolidColorBrush* br;
	ID2D1RenderTarget* t;
	float opacity;
};

class d2d_texture_brush : 
	public texture_brush
{
public:
	d2d_texture_brush(texture* img, ID2D1RenderTarget* _target) : image(dynamic_cast<d2d_texture*>(img)), target(_target) {if((_target->CreateBitmapBrush(image->get_native_member(),BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP), &bmp_brush))) throw invalid_argument("Unable to create BitmapBush");};
	d2d_texture_brush(const d2d_texture_brush&)=delete;
	brush_types get_type() {return brush_types::texture_brush;};
	void release_resources() {bmp_brush->Release(); bmp_brush = 0; };
	void set_transform(const matrix& m) {bmp_brush->SetTransform(Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32));};
	matrix get_transform() const {Matrix3x2F m = Matrix3x2F(); bmp_brush->GetTransform(&m); return matrix(m._11, m._12, m._21, m._22, m._31, m._32);};
	
	void set_wrap_mode(const wrap_modes& mode) 
	{
		mde = mode;
		switch(mode)
		{
		case wrap_modes::clamp:
			bmp_brush->SetExtendModeX(D2D1_EXTEND_MODE_CLAMP);
			bmp_brush->SetExtendModeY(D2D1_EXTEND_MODE_CLAMP);
			break;
		case wrap_modes::mirror_x:
			bmp_brush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
			bmp_brush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
			break;
		case wrap_modes::mirror_xy:
			bmp_brush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
			bmp_brush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);
			break;
		case wrap_modes::mirror_y:
			bmp_brush->SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
			bmp_brush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);
			break;
		case wrap_modes::wrap:
			bmp_brush->SetExtendModeX(D2D1_EXTEND_MODE_WRAP);
			bmp_brush->SetExtendModeY(D2D1_EXTEND_MODE_WRAP);
		}
	};
	wrap_modes get_wrap_mode() const {return mde;};

	texture* get_texture() const {return image;};
	ID2D1BitmapBrush* get_native_member() {return bmp_brush;};

	void set_opacity(float val) {opacity = val; bmp_brush->SetOpacity(val);};
	float get_opacity() const {return opacity;};
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
	d2d_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, ID2D1RenderTarget* target_, bool gamma , const wrap_modes& wrapmode) : target(target_), stops(gradients)
	{
		D2D1_GRADIENT_STOP* stps = new D2D1_GRADIENT_STOP[gradients.get_count()];
		float* fp = gradients.get_positions();
		colour* fc = gradients.get_colours();
		for(unsigned int i = 0; i < gradients.get_count(); i++)
		{
			colour c = *(fc+i);
			stps[i] = GradientStop(*(fp+i), ColorF(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
		}
		ID2D1GradientStopCollection* coll = 0;
		D2D1_EXTEND_MODE mode;
		switch(wrapmode)
		{
		case wrap_modes::clamp:
			mode = D2D1_EXTEND_MODE_CLAMP;
			break;
		case wrap_modes::mirror_x:
		case wrap_modes::mirror_xy:
		case wrap_modes::mirror_y:
			mode = D2D1_EXTEND_MODE_MIRROR;
			break;
		case wrap_modes::wrap:
			mode = D2D1_EXTEND_MODE_WRAP;
		}
		if(FAILED(target_->CreateGradientStopCollection(stps, gradients.get_count(), gamma ? D2D1_GAMMA_1_0 : D2D1_GAMMA_2_2, mode, &coll))) throw runtime_error("Unable to create GradientStopCollection");
		if(FAILED(target_->CreateLinearGradientBrush(LinearGradientBrushProperties(Point2F(start.x, start.y), Point2F(end.x, end.y)), BrushProperties(), coll, &br))) throw runtime_error("Unable to create LinearGradientBrush");
		coll->Release();
		delete[] stps;
	};
	d2d_linear_gradient_brush(const d2d_linear_gradient_brush&)=delete;
	brush_types get_type() {return brush_types::linear_gradient_brush;};
	void release_resources() {br->Release(); br = 0;};
	void set_transform(const matrix& m) {br->SetTransform(Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32));};
	matrix get_transform() const {Matrix3x2F m = Matrix3x2F(); br->GetTransform(&m); return matrix(m._11, m._12, m._21, m._22, m._31, m._32);};
	float get_opacity() const {return br->GetOpacity();};
	void set_opacity(float val) {br->SetOpacity(val);};
	gradient_stop get_gradients() const {return stops;};
	void set_gradients(gradient_stop& gradients)
	{
		auto start = br->GetStartPoint();
		auto end = br->GetEndPoint();
		D2D1_GRADIENT_STOP* stps = new D2D1_GRADIENT_STOP[gradients.get_count()];
		float* fp = gradients.get_positions();
		colour* fc = gradients.get_colours();
		for(unsigned int i = 0; i < gradients.get_count(); i++)
		{
			colour c = *(fc+i);
			stps[i] = GradientStop(*(fp+i), ColorF(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
		}
		ID2D1GradientStopCollection* gst = 0;
		br->GetGradientStopCollection(&gst);
		if(br) br->Release();
		br = 0;
		auto exmode = gst->GetExtendMode();
		auto gamma = gst->GetColorInterpolationGamma();
		ID2D1GradientStopCollection* gst1 = 0;
		if(FAILED(target->CreateGradientStopCollection(stps, gradients.get_count(), &gst1))) throw runtime_error("Unable to create GradientStopCollection");
		if(FAILED(target->CreateLinearGradientBrush(LinearGradientBrushProperties(start, end), BrushProperties(), gst1, &br))) throw runtime_error("Unable to create LinearGradientBrush");
		gst1->Release();
		delete[] stps;
	};
	void set_rect(const point& p1, const point& p2) {br->SetStartPoint(Point2F(p1.x, p1.y)); br->SetEndPoint(Point2F(p2.x, p2.y));};
	rect get_rect() const {D2D1_POINT_2F p1 = br->GetStartPoint(); D2D1_POINT_2F p2 = br->GetEndPoint(); return rect(point(p1.x, p1.y), size(p2.x, p2.y));};
	ID2D1LinearGradientBrush* get_native_member() {return br;};
private:
	ID2D1RenderTarget* target;
	ID2D1LinearGradientBrush* br;
	gradient_stop stops;
};

class d2d_geometry : 
	public geometry
{
public:
	d2d_geometry(ID2D1Factory* fct);
	d2d_geometry(const d2d_geometry&)=delete;
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

	ID2D1Geometry* get_geometry();
private:
	ID2D1PathGeometry* geo;
	ID2D1GeometrySink* geo_sink;
	ID2D1TransformedGeometry* geo_trans;
	ID2D1Factory* factory;
	matrix m;
	point lastPoint;
};

class d2d_pen : 
	public pen
{
public:
	d2d_pen(brush* org, ID2D1Brush* br, ID2D1Factory* f, float w, pen_align algn = pen_align::center, dash_cap startcap = dash_cap::flat, dash_cap endcap = dash_cap::flat, dash_cap caps = dash_cap::flat, dash_style dstyle = dash_style::solid, float offset = 0) : org_br(org), width(w), align(algn), start_cap(startcap), end_cap(endcap), cap(caps), style(dstyle), dash_offset(offset), brush_(br), factory(f) {recreate();};
	~d2d_pen() {};
	ID2D1Brush* get_native_member() {return brush_;};
	ID2D1StrokeStyle* get_stroke_style() {return stroke_style;};
	void release_resources() {stroke_style->Release(); stroke_style = 0;};

	brush* get_brush() const {return org_br;};

	dash_cap get_start_cap() const {return start_cap;};
	void set_start_cap(const dash_cap& p) {start_cap = p; recreate();};
	dash_cap get_end_cap() const {return end_cap;};
	void set_end_cap(const dash_cap& p) {end_cap = p; recreate();};
	pen_align get_align() const {return align;};
	void set_align(const pen_align& a) {align = a; recreate();};
	dash_cap get_dash_cap() const {return cap;};
	void set_dash_cap(const dash_cap& c) {cap = c; recreate();};
	dash_style get_dash_style() const {return style;};
	void set_dash_style(const dash_style& d) {style = d; recreate();};
	float get_dash_offs() const {return dash_offset;};
	void set_dash_offs(float f)  {dash_offset = f; recreate();};
	float get_width() const {return width;};
	void set_width(float f) {width = f; recreate();};
private:
	void recreate() 
	{if(FAILED(factory->CreateStrokeStyle(StrokeStyleProperties(static_cast<D2D1_CAP_STYLE>(start_cap), static_cast<D2D1_CAP_STYLE>(end_cap), static_cast<D2D1_CAP_STYLE>(cap), align == pen_align::center ? D2D1_LINE_JOIN_MITER : D2D1_LINE_JOIN_BEVEL, 10.0f,
	static_cast<D2D1_DASH_STYLE>(style), dash_offset), 0, 0, &stroke_style))) throw runtime_error("Unable to create Strokestyle");};
	pen_align align;
	dash_cap start_cap, end_cap, cap;
	dash_style style;
	float width, dash_offset;
	ID2D1Brush* brush_;
	ID2D1StrokeStyle* stroke_style;
	ID2D1Factory* factory;
	brush* org_br;
};


class direct2d_renderer : 
	public render_objects::graphics
{
public:
	direct2d_renderer(HWND hWnd, callback<void(const size&)>& target_resize);
	direct2d_renderer(const direct2d_renderer&)=delete;
	HDC get_dc() const {return dc;};
	void begin();
	void clear(const colour& c);
	void end();

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
	void draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format);
	void draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format, const horizontal_string_align& clipping_h, const vertical_string_align& clipping_v);
	void draw_texture(texture* image, const point& p);
	void draw_texture(texture* image, const point& p, const size& sz);
	void draw_texture(texture* image, const rect& rc);
	void draw_texture(texture* image, const rect& src, const point& dst);
	void draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest);
	void draw_texture(texture* image, const rect& src, const rect& dest);
	void draw_geometry(geometry* geo, pen* p);

	solid_brush* create_solid_brush(const colour& c);
	texture_brush* create_texture_brush(texture* image);
	linear_gradient_brush* create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma = false, const wrap_modes& wrapmode = wrap_modes::wrap);
	pen* create_pen(brush* b, float width = 1.0f, pen_align align = pen_align::center, dash_cap startcap = dash_cap::flat, dash_cap endcap = dash_cap::flat, dash_cap dash_cap = dash_cap::flat, dash_style dash_style = dash_style::solid, float offset = 0.0f);
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
	text_rendering_modes get_text_rendering_mode() const;
	void set_antialias(bool val);
	bool get_antialias() const {return antialias;};
private:
	void target_resizes(const size&);
	ID2D1Brush* get_brush(brush* b);
	ID2D1Brush* get_pen(const pen& p);
	ID2D1StrokeStyle* get_stroke_style(const pen& p);
	D2D1_RECT_F get_rect(const rect& rc);
	D2D1_POINT_2F get_point(const point& p);
	Matrix3x2F get_matrix(const matrix& m);
	void release_resources();

	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* render_target;
	IWICImagingFactory* imgfactory;
	IDWriteFactory* write_factory;
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