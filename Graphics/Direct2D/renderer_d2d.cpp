#include "stdafx.h"
#include "renderer_d2d.h"

namespace gcl { namespace std_renderer {
	
// Font
d2d_font::d2d_font(const wstring& family_name, float size, int fstyle, ID2D1Factory* tg, IDWriteFactory* fctry) : target(tg), factory(fctry), family(family_name), sz(size), style(fstyle)
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
	wchar_t loc[LOCALE_NAME_MAX_LENGTH];
	GetUserDefaultLocaleName(loc, LOCALE_NAME_MAX_LENGTH);
	if(FAILED(factory->CreateTextFormat(family_name.c_str(), NULL, fontWeight, fontStyle, DWRITE_FONT_STRETCH_NORMAL, sz, loc, &format))) throw runtime_error("Unable to create TextFormat");
}

rect d2d_font::get_metrics(const wstring& s, const size& clip, graphics*) const
{
	IDWriteTextLayout* layout;
	if(FAILED(factory->CreateTextLayout(s.c_str(), s.length(), format, clip.width, clip.height, &layout)))
		throw runtime_error("Unable to retrieve metrics");
	DWRITE_TEXT_RANGE rg;
	rg.length = s.length();
	rg.startPosition = 0;
	layout->SetFontStyle(static_cast<DWRITE_FONT_STYLE>(style), rg);
	DWRITE_TEXT_METRICS metrics;
	layout->GetMetrics(&metrics);
	layout->Release();
	return rect(0, 0, max(metrics.widthIncludingTrailingWhitespace, metrics.width), metrics.height);
}

vector<wstring> d2d_font::get_available_font_families()
{
	vector<wstring> coll;
	IDWriteFontCollection* fc = 0;
	if(FAILED(factory->GetSystemFontCollection(&fc, TRUE)))
		throw runtime_error("Unable to create FontCollection");
	int cnt = fc->GetFontFamilyCount();
	wchar_t loc[LOCALE_NAME_MAX_LENGTH];
	GetUserDefaultLocaleName(loc, LOCALE_NAME_MAX_LENGTH);
	for(int i = 0; i < cnt; i++)
	{
		IDWriteFontFamily* fam = 0;
		fc->GetFontFamily(i, &fam);
		IDWriteLocalizedStrings* str = 0;
		fam->GetFamilyNames(&str);
		BOOL exists = 0;
		unsigned int idx = 0;
		str->FindLocaleName(loc, &idx, &exists);
		if(!exists)
		{
			str->FindLocaleName(L"en-us", &idx, &exists);
		}
		if(!exists)
			idx = 0;
		unsigned int len = 0;
		str->GetStringLength(idx, &len);
		wchar_t* name = new wchar_t[len+1];
		str->GetString(idx, name, len+1);
		coll.push_back(name);
		delete[] name;
		str->Release();
		fam->Release();
	}
	fc->Release();
	return move(coll);
}
// Font

// Texture
d2d_texture::d2d_texture(const wstring& filename, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target), bmp(0), w_bmp(0), lock(0)
{
	if(FAILED(load_texture_file(filename, &bmp))) 
		throw invalid_argument("Unable to load Bitmap");
}

d2d_texture::d2d_texture(int id, HINSTANCE inst, LPWSTR type, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target), bmp(0), w_bmp(0), lock(0)
{
	if(FAILED(load_texture_resource(id, type, inst, &bmp))) 
		throw invalid_argument("Unable to load Bitmap");
}

d2d_texture::d2d_texture(IWICBitmap* wic_bmp, IWICImagingFactory* w_factory, ID2D1Bitmap* d2d_bmp, ID2D1RenderTarget* _target) : factory(w_factory), target(_target), lock(0)
{
	if(FAILED(load_texture_from_texture(wic_bmp, d2d_bmp))) throw runtime_error("Unable to copy Bitmap");
}

d2d_texture::d2d_texture(const size& sz, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target)
{
	if(sz.width <= 0 || sz.height <= 0)
		throw invalid_argument("Invalid size");
	auto hr = factory->CreateBitmap(static_cast<UINT>(sz.width), static_cast<UINT>(sz.height), GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &w_bmp);
	hr = target->CreateBitmapFromWicBitmap(w_bmp, &bmp);
	if(FAILED(hr))
		throw runtime_error("Unable to initialize Bitmap");

}

d2d_texture::d2d_texture(HBITMAP hbmp, HPALETTE hp, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target)
{
	HRESULT hr = factory->CreateBitmapFromHBITMAP(hbmp, hp, WICBitmapAlphaChannelOption::WICBitmapIgnoreAlpha, &w_bmp);
	hr = target->CreateBitmapFromWicBitmap(w_bmp, &bmp);
	if(FAILED(hr))
		throw runtime_error("Unable to initialize Bitmap");
}

d2d_texture::d2d_texture(HICON ico, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target)
{
	IWICBitmap* w_bmp2 = 0;
	HRESULT hr = factory->CreateBitmapFromHICON(ico, &w_bmp2);
	IWICFormatConverter* converter = 0;
	hr = factory->CreateFormatConverter(&converter);
	hr = converter->Initialize(w_bmp2, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
	hr = factory->CreateBitmapFromSource(converter, WICBitmapCacheOnLoad, &w_bmp); 
	hr = target->CreateBitmapFromWicBitmap(converter, &bmp);
	w_bmp2->Release();
	converter->Release();
	if(FAILED(hr))
		throw runtime_error("Unable to initialize Bitmap");
}

d2d_texture::d2d_texture(const oem_bitmap& id, IWICImagingFactory* _factory, ID2D1RenderTarget* _target) : factory(_factory), target(_target)
{
	HBITMAP hbmp = static_cast<HBITMAP>(LoadImage(NULL, MAKEINTRESOURCE(id), IMAGE_BITMAP, 0, 0, LR_SHARED | LR_DEFAULTSIZE));
	if(!hbmp)
		throw runtime_error("Unable to load Bitmap");
	HRESULT hr = factory->CreateBitmapFromHBITMAP(hbmp, 0, WICBitmapAlphaChannelOption::WICBitmapIgnoreAlpha, &w_bmp);
	hr = target->CreateBitmapFromWicBitmap(w_bmp, &bmp);
	if(FAILED(hr))
		throw runtime_error("Unable to initialize Bitmap");
}

d2d_texture::~d2d_texture()
{
	bmp->Release(); 
	w_bmp->Release();
}

unsigned char* d2d_texture::alloc() 
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
}

void d2d_texture::free() 
{
	if(!lock)
		throw invalid_argument("Data isn't allocated yet");
	lock->Release();
	lock = 0;
	bmp->Release();
	bmp = 0;
	if(FAILED(target->CreateBitmapFromWicBitmap(w_bmp, &bmp)))
		throw runtime_error("Unable to update Bitmap");
}

int d2d_texture::get_stride() 
{
	if(!lock)
		throw invalid_argument("Data isn't allocated yet");
	UINT num = 0;
	if(FAILED(lock->GetStride(&num)))
		throw runtime_error("Unable to receive Stride");
	return static_cast<int>(num);
}

HRESULT d2d_texture::load_texture_file(const wstring& path, ID2D1Bitmap** bmp)
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
}

HRESULT d2d_texture::load_texture_resource(int id, LPWSTR type, HINSTANCE inst, ID2D1Bitmap** bmp)
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
}

HRESULT d2d_texture::load_texture_from_texture(IWICBitmap* wic_bmp, ID2D1Bitmap* d2d_bmp)
{
	HRESULT	hr = target->CreateBitmapFromWicBitmap(wic_bmp, &bmp);
	if(SUCCEEDED(hr))
		hr = factory->CreateBitmapFromSource(wic_bmp, WICBitmapCacheOnLoad, &w_bmp);
	return hr;
}
// Texture

// Texturebrush
d2d_texture_brush::d2d_texture_brush(texture* img, ID2D1RenderTarget* _target) : image(dynamic_cast<d2d_texture*>(img)), target(_target) 
{
	if((_target->CreateBitmapBrush(image->get_native_member(),BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP), &bmp_brush))) 
		throw invalid_argument("Unable to create BitmapBush");
}

matrix d2d_texture_brush::get_transform() const 
{
	Matrix3x2F m = Matrix3x2F(); 
	bmp_brush->GetTransform(&m); 
	return matrix(m._11, m._12, m._21, m._22, m._31, m._32);
}
	
void d2d_texture_brush::set_wrap_mode(const wrap_modes& mode) 
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
}
// Texturebrush

// Solidbrush
colour d2d_solid_brush::get_colour() 
{
	auto c = br->GetColor(); 
	return colour(static_cast<int>(c.r*255), static_cast<int>(c.g*255), static_cast<int>(c.b*255), static_cast<int>(c.a*255.0f));
}

d2d_solid_brush::d2d_solid_brush(ID2D1RenderTarget* target, D2D1::ColorF& c) : t(target), opacity(1.0f) 
{
	if(FAILED(target->CreateSolidColorBrush(c, &br))) 
		throw runtime_error("Unable to create D2D_SolidBrush");
}
// Solidbrush

// Lineargradientbrush
d2d_linear_gradient_brush::d2d_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, ID2D1RenderTarget* target_, bool gamma , const wrap_modes& wrapmode) : target(target_), stops(gradients)
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
}

matrix d2d_linear_gradient_brush::get_transform() const 
{
	Matrix3x2F m = Matrix3x2F(); 
	br->GetTransform(&m); 
	return matrix(m._11, m._12, m._21, m._22, m._31, m._32);
}

void d2d_linear_gradient_brush::set_gradients(gradient_stop& gradients)
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
}

void d2d_linear_gradient_brush::set_rect(const point& p1, const point& p2) 
{
	br->SetStartPoint(Point2F(p1.x, p1.y)); 
	br->SetEndPoint(Point2F(p2.x, p2.y));
}

rect d2d_linear_gradient_brush::get_rect() const 
{
	D2D1_POINT_2F p1 = br->GetStartPoint(); 
	D2D1_POINT_2F p2 = br->GetEndPoint(); 
	return rect(point(p1.x, p1.y), size(p2.x, p2.y));
}
// Lineargradientbrush

// Radialgradientbrush
d2d_radial_gradient_brush::d2d_radial_gradient_brush(ID2D1RenderTarget* trgt, const ellipse& e, const gradient_stop& gradients, bool gamma) : elp(e), stops(gradients), gamma_(gamma)
{
	target = trgt;
	br = 0;
	opacity = 1.f;
	recreate_brush();
}

void d2d_radial_gradient_brush::set_transform(const matrix& m_)
{
	if(m == m_) return;
	m = m_;
	br->SetTransform(Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32));
}

void d2d_radial_gradient_brush::set_opacity(float val)
{
	if(opacity == val) return;
	opacity = val;
	br->SetOpacity(val);
}

void d2d_radial_gradient_brush::set_ellipse(const ellipse& e)
{
	if(elp == e) return;
	elp = e;
	br->SetCenter(Point2F(e.position.x, e.position.y));
	br->SetRadiusX(e.radius_x);
	br->SetRadiusY(e.radius_y);
}

void d2d_radial_gradient_brush::set_gradient_origin_offset(const point& p)
{
	if(offs == p) return;
	offs = p;
	br->SetGradientOriginOffset(Point2F(p.x, p.y));
}

void d2d_radial_gradient_brush::set_gradients(gradient_stop& gradients)
{
	stops = gradients;
	recreate_brush();
}

void d2d_radial_gradient_brush::recreate_brush()
{
	if(br)
		br->Release();
	ID2D1GradientStopCollection* coll = 0;
	D2D1_GRADIENT_STOP* g_stops = new D2D1_GRADIENT_STOP[stops.get_count()];
	for(unsigned int i = 0; i < stops.get_count(); i++)
	{
		g_stops[i] = GradientStop(stops.get_positions()[i], ColorF(stops.get_colours()[i].r, stops.get_colours()[i].g, stops.get_colours()[i].b, stops.get_colours()[i].a/255.f*opacity));
	}
	if(FAILED(target->CreateGradientStopCollection(g_stops, stops.get_count(), gamma_ ? D2D1_GAMMA_1_0 : D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &coll)))
		throw runtime_error("Cannot create GradientStopCollection");
	if(FAILED(target->CreateRadialGradientBrush(RadialGradientBrushProperties(Point2F(elp.position.x, elp.position.y), Point2F(offs.x, offs.y), elp.radius_x, elp.radius_y), 
		BrushProperties(opacity, Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32)), coll, &br)))
		throw runtime_error("Cannot create RadialGradientBrush");
	coll->Release();
	delete[] g_stops;
}
// Radialgradientbrush

// Graphics 
ID2D1Factory* direct2d_renderer::factory = 0;
IWICImagingFactory* direct2d_renderer::imgfactory = 0;
IDWriteFactory* direct2d_renderer::write_factory = 0;
int direct2d_renderer::inst_count = 0;

void direct2d_renderer::target_resizes(const size& sz, const resizing_types&)
{
	if(!hwnd_render_target) return;
	if(type == graphics_type::handle) hwnd_render_target->Resize(SizeU(static_cast<int>(sz.width), static_cast<int>(sz.height)));
}

direct2d_renderer::direct2d_renderer(HWND handle_, callback<void(const size&, const resizing_types&)>& cb) : hwnd_render_target(0), bmp_render_target(0), dc(0)
{
	handle = handle_;
	bmp = 0;
	inst_count++;
	type = graphics_type::handle;
	cb += make_func_ptr(this, &direct2d_renderer::target_resizes);
	create_resources();
	RECT rc;
	GetClientRect(handle, &rc);
	if(FAILED(factory->CreateHwndRenderTarget(RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE), HwndRenderTargetProperties(handle, SizeU(rc.right-rc.left, rc.bottom-rc.top)), &hwnd_render_target))) throw runtime_error("Couldn't initialize Direct2D");
}

direct2d_renderer::direct2d_renderer(direct2d_renderer* old_renderer, d2d_texture* text)
{
	handle = 0;
	bmp = text;
	inst_count++;
	type = graphics_type::texture;
	create_resources();
	if(FAILED(old_renderer->get_current_target()->CreateCompatibleRenderTarget(SizeF(static_cast<float>(text->get_width()), static_cast<float>(text->get_height())), SizeU(text->get_width(), text->get_height()),
		PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, &bmp_render_target)))
		throw runtime_error("Unable to create BitmapRenderTarget");
	get_current_target()->BeginDraw();
	get_current_target()->DrawBitmap(text->get_native_member(), RectF(0, 0, static_cast<float>(text->get_width()), static_cast<float>(text->get_height())));
	get_current_target()->EndDraw();
}

direct2d_renderer::direct2d_renderer(HDC _dc)
{
	if(!_dc)
		throw invalid_argument("Empty HDC");
	dc = _dc;
	type = graphics_type::dc;
	create_resources();
	D2D1_PIXEL_FORMAT frmt;
	frmt.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	frmt.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	if(FAILED(factory->CreateDCRenderTarget(&RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, frmt), &dc_render_target)))
		throw runtime_error("Unable to create DCRenderTarget");
	RECT rc;
	GetWindowRect(WindowFromDC(dc), &rc);
	if(FAILED(dc_render_target->BindDC(dc, &rc)))
		throw runtime_error("Unable to bind DC");
}

void direct2d_renderer::bind_dc(HDC _dc)
{
	if(!dc_render_target)
		throw logic_error("No DcRenderer");
	dc = _dc;
	RECT rc;
	GetWindowRect(WindowFromDC(dc), &rc);
	if(FAILED(dc_render_target->BindDC(dc, &rc)))
		throw runtime_error("Unable to bind DC");
}

void direct2d_renderer::create_resources()
{
	if(factory) return;
	if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory))) throw runtime_error("Couldn't initialise Direct2D");
	if(FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)(IID_PPV_ARGS(&imgfactory))))) throw runtime_error("Unable to create ImagingFactory");
	if(FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&write_factory)))) throw runtime_error("Unable to create DirectWriteFactory");
}

direct2d_renderer::~direct2d_renderer()
{
	inst_count--;
	get_current_target()->Release();
	if(inst_count) return;
	write_factory->Release();
	imgfactory->Release();
	factory->Release();
}

void direct2d_renderer::begin()
{
	if(!get_current_target()) return;
	if(type == graphics_type::handle)
		dc = BeginPaint(handle, &ps);
	get_current_target()->BeginDraw();
	get_current_target()->SetTransform(Matrix3x2F::Identity());
}

void direct2d_renderer::clear(const colour& c)
{
	if(!get_current_target()) return;
	get_current_target()->Clear(ColorF(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
}

void direct2d_renderer::end()
{
	if(FAILED(get_current_target()->EndDraw())) 
		throw runtime_error("Fatal error while calling \"EndDraw()\"");
	if(type == graphics_type::handle)
	{
		EndPaint(handle, &ps);
		dc = 0;
	}
	else if(type == graphics_type::texture)
	{
		ID2D1Bitmap* bmp_pr = 0;
		bmp_render_target->GetBitmap(&bmp_pr);
		bmp->get_native_member()->CopyFromBitmap(&Point2U(0, 0), bmp_pr, &RectU(0, 0, bmp->get_width(), bmp->get_height()));
		bmp_pr->Release();
	}
}

ID2D1Brush* direct2d_renderer::get_brush(brush* b)
{
	switch(b->get_type())
	{
	case brush_types::solid_brush:
		return dynamic_cast<d2d_solid_brush*>(b)->get_native_member();
	case brush_types::texture_brush:
		return dynamic_cast<d2d_texture_brush*>(b)->get_native_member();
	case brush_types::linear_gradient_brush:
		return dynamic_cast<d2d_linear_gradient_brush*>(b)->get_native_member();
	case brush_types::radial_gradient_brush:
		return dynamic_cast<d2d_radial_gradient_brush*>(b)->get_native_member();
	default:
		throw invalid_argument("Unknown Brushtype");
	}
}

ID2D1RenderTarget* direct2d_renderer::get_current_target() const
{
	switch(type)
	{
	case graphics_type::handle:
		return hwnd_render_target;
	case graphics_type::texture:
		return bmp_render_target;
	case graphics_type::dc:
		return dc_render_target;
	default:
		throw invalid_argument("No target selected");
	}
}

D2D1_RECT_F direct2d_renderer::get_rect(const rect& rc)
{
	return RectF(rc.position.x, rc.position.y, rc.sizef.width + rc.position.x, rc.sizef.height + rc.position.y);
}

D2D1_POINT_2F direct2d_renderer::get_point(const point& p)
{
	return Point2F(p.x, p.y);
}

void direct2d_renderer::push_clip(const clip& cl)
{
	if(cl.is_rectangle_clip())
	{
		rect rc = cl.get_rect();
		get_current_target()->PushLayer(LayerParameters(get_rect(rc)), 0);
	}
	else
	{
		ID2D1Geometry* gp = dynamic_cast<d2d_geometry*>(cl.get_geometry())->get_native_member();
		get_current_target()->PushLayer(LayerParameters(InfiniteRect(), gp), 0);
	}
}

void direct2d_renderer::pop_clip()
{
	get_current_target()->PopLayer();
}
//******************************************************************************************************************************************************************************************************************************************************

void direct2d_renderer::fill_rect(const rect& rc, brush* b)
{
	get_current_target()->FillRectangle(get_rect(rc), get_brush(b));
}

void direct2d_renderer::fill_rects(const rect* rcs, int count, brush* b)
{
	ID2D1Brush* br = get_brush(b);
	for(int i = 0; i < count; i++)
		get_current_target()->FillRectangle(get_rect(rcs[i]), br);
}

void direct2d_renderer::fill_ellipse(const ellipse& e, brush* b)
{
	get_current_target()->FillEllipse(Ellipse(Point2F(e.position.x, e.position.y), e.radius_x, e.radius_y), get_brush(b));
}

void direct2d_renderer::fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b)
{
	get_current_target()->FillRoundedRectangle(RoundedRect(RectF(rc.position.x, rc.position.y, rc.sizef.width + rc.position.x, rc.sizef.height + rc.position.y), radiusx, radiusy), get_brush(b)); 
}

void direct2d_renderer::fill_geometry(geometry* geo, brush* b)
{
	ID2D1Geometry* geo_ = dynamic_cast<d2d_geometry*>(geo)->get_native_member();
	get_current_target()->FillGeometry(geo_, get_brush(b));
}

void direct2d_renderer::fill_polygon(const point* ps, int count, brush* b)
{
	if(count < 3)
		throw logic_error("Count must be at least 3");
	ID2D1PathGeometry* geo = 0;
	ID2D1GeometrySink* sink = 0;
	if(FAILED(factory->CreatePathGeometry(&geo)))
		throw runtime_error("Unable to create PathGeometry");
	if(FAILED(geo->Open(&sink)))
		throw runtime_error("Unable to open Geometry");
	sink->BeginFigure(get_point(ps[0]), D2D1_FIGURE_BEGIN_FILLED);
	for(int i = 1; i < count; i++)
		sink->AddLine(get_point(ps[i]));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	if(FAILED(sink->Close()))
		throw runtime_error("Unable to close Geometry");
	get_current_target()->FillGeometry(geo, get_brush(b));
	sink->Release();
	geo->Release();
}
//*************************************************************************************************************************************************************************************************************************************************

void direct2d_renderer::draw_ellipse(const ellipse& e, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	get_current_target()->DrawEllipse(Ellipse(get_point(e.position), e.radius_x, e.radius_y), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_line(const point& p1, const point& p2, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	get_current_target()->DrawLine(get_point(p1), get_point(p2), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_lines(const point* ps, int count, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	for (int i = 0; i < count-1; i++)
		get_current_target()->DrawLine(get_point(ps[i]), get_point(ps[i++]), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_rect(const rect rc, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	get_current_target()->DrawRectangle(get_rect(rc), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_rects(const rect* rcs, int count, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	for(int i = 0; i < count; i++)
		get_current_target()->DrawRectangle(get_rect(rcs[i]), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	get_current_target()->DrawRoundedRectangle(RoundedRect(get_rect(rc), radiusx, radiusy), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_polygon(const point* ps, int count, pen* p)
{
	if(count < 3)
		throw logic_error("Count must be at least 3");
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	ID2D1PathGeometry* geo = 0;
	ID2D1GeometrySink* sink = 0;
	if(FAILED(factory->CreatePathGeometry(&geo)))
		throw runtime_error("Unable to create PathGeometry");
	if(FAILED(geo->Open(&sink)))
		throw runtime_error("Unable to open Geometry");
	sink->BeginFigure(get_point(ps[0]), D2D1_FIGURE_BEGIN_HOLLOW);
	for(int i = 1; i < count; i++)
		sink->AddLine(get_point(ps[i]));
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	if(FAILED(sink->Close()))
		throw runtime_error("Unable to close Geometry");
	get_current_target()->DrawGeometry(geo, p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
	sink->Release();
	geo->Release();
}

void direct2d_renderer::draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format)
{
	float fmax = (numeric_limits<float>::max)();
	IDWriteTextFormat* format_ = dynamic_cast<d2d_font*>(fn)->get_native_member();
	if(format == string_format::direction_left_to_right)
	{
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT))) throw runtime_error("Unable to set ReadingDirection");
	}
	else
	{
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT))) throw runtime_error("Unable to set ReadingDirection");
	}
	if(FAILED(format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)) ||
	FAILED(format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING))) throw runtime_error("Unable to set Alignments");
	get_current_target()->DrawTextW(str.c_str(), static_cast<UINT32>(str.length()), format_, &RectF(origin.x, origin.y, fmax, fmax), get_brush(b), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void direct2d_renderer::draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format, const horizontal_string_align& clipping_h, const vertical_string_align& clipping_v)
{
	IDWriteTextFormat* format_ = dynamic_cast<d2d_font*>(fn)->get_native_member();
	if(format == string_format::direction_left_to_right)
	{
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT))) throw runtime_error("Unable to set ReadingDirection");
	}
	else
	{
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT))) throw runtime_error("Unable to set ReadingDirection");
	}
	DWRITE_TEXT_ALIGNMENT alignh;
	DWRITE_PARAGRAPH_ALIGNMENT alignv;
	switch(clipping_h)
	{
	case horizontal_string_align::left:
		alignh = DWRITE_TEXT_ALIGNMENT_LEADING;
		break;
	case horizontal_string_align::middle:
		alignh = DWRITE_TEXT_ALIGNMENT_CENTER;
		break;
	default:
		alignh = DWRITE_TEXT_ALIGNMENT_TRAILING;
	}
	
	switch(clipping_v)
	{
	case vertical_string_align::top:
		alignv = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		break;
	case vertical_string_align::middle:
		alignv = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		break;
	default:
		alignv = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	}
	if(FAILED(format_->SetParagraphAlignment(alignv)) ||
	FAILED(format_->SetTextAlignment(alignh))) throw runtime_error("Unable to set Alignments");
	get_current_target()->DrawTextW(str.c_str(), static_cast<UINT32>(str.length()), format_, &get_rect(cliprc), get_brush(b), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void direct2d_renderer::draw_texture(texture* image, const point& p, unsigned char opacity)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	get_current_target()->DrawBitmap(bmp->get_native_member(), get_rect(rect(p.x, p.y, static_cast<float>(bmp->get_width()), static_cast<float>(bmp->get_height()))), opacity / 255.f);
}

void direct2d_renderer::draw_texture(texture* image, const point& p, const size& sz, unsigned char opacity)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	get_current_target()->DrawBitmap(bmp->get_native_member(), get_rect(rect(p.x, p.y, sz.width, sz.height)), opacity / 255.f);
}

void direct2d_renderer::draw_texture(texture* image, const rect& rc, unsigned char opacity)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	get_current_target()->DrawBitmap(bmp->get_native_member(), get_rect(rect(rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height)), opacity / 255.f);
}

void direct2d_renderer::draw_texture(texture* image, const rect& src, const point& dst, unsigned char opacity)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	get_current_target()->DrawBitmap(bmp->get_native_member(), get_rect(rect(dst.x, dst.y, src.sizef.width, src.sizef.height)), opacity / 255.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		get_rect(rect(src.position.x, src.position.y, src.sizef.width, src.sizef.height)));
}

void direct2d_renderer::draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest, unsigned char opacity)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	get_current_target()->DrawBitmap(bmp->get_native_member(), get_rect(rect(pdest.x, pdest.y, szdest.width, szdest.height)), opacity/255.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		get_rect(rect(src.position.x, src.position.y, src.sizef.width, src.sizef.height)));
}

void direct2d_renderer::draw_texture(texture* image, const rect& src, const rect& dest, unsigned char opacity)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	get_current_target()->DrawBitmap(bmp->get_native_member(), get_rect(rect(dest.position.x, dest.position.y, dest.sizef.width, dest.sizef.height)), opacity/255.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		get_rect(rect(src.position.x, src.position.y, src.sizef.width, src.sizef.height)));
}

void direct2d_renderer::draw_icon(icon* ico, const point& pos)
{
	size sz = ico->get_size();
	get_current_target()->DrawBitmap(dynamic_cast<d2d_icon*>(ico)->get_native_member(), get_rect(rect(pos, sz)));
}

void direct2d_renderer::draw_icon(icon* ico, const point& pos, const size& sz)
{
	get_current_target()->DrawBitmap(dynamic_cast<d2d_icon*>(ico)->get_native_member(), get_rect(rect(pos, sz)));
}

void direct2d_renderer::draw_geometry(geometry* geo, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	get_current_target()->DrawGeometry(dynamic_cast<d2d_geometry*>(geo)->get_native_member(), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}


solid_brush* direct2d_renderer::create_solid_brush(const colour& c)
{
	return new d2d_solid_brush(get_current_target(), D2D1::ColorF(c.r/255.f, c.g/255.f, c.b/255.f, c.a / 255.0f));
}

texture_brush* direct2d_renderer::create_texture_brush(texture* image)
{
	return new d2d_texture_brush(image, get_current_target());
}

linear_gradient_brush* direct2d_renderer::create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode)
{
	return new d2d_linear_gradient_brush(start, end, gradients, get_current_target(), gamma, wrapmode);
}

pen* direct2d_renderer::create_pen(brush* b, float width, dash_cap startcap, dash_cap endcap, dash_cap dash_cap, dash_style dashstyle, float offset)
{
	return new d2d_pen(b, get_brush(b), factory, width, startcap, endcap, dash_cap, dashstyle, offset);
}

texture* direct2d_renderer::create_texture(const wstring& filename)
{
	return new d2d_texture(filename, imgfactory, get_current_target());
}

texture* direct2d_renderer::create_texture(int id, LPWSTR type, HINSTANCE inst)
{
	return new d2d_texture(id, inst, type, imgfactory, get_current_target());
}

texture* direct2d_renderer::create_texture(const size& sz)
{
	return new d2d_texture(sz, imgfactory, get_current_target());
}

texture* direct2d_renderer::create_texture(HBITMAP hbmp, HPALETTE hP)
{
	return new d2d_texture(hbmp, hP, imgfactory, get_current_target());
}

texture* direct2d_renderer::create_texture(HICON ico)
{
	return new d2d_texture(ico, imgfactory, get_current_target());
}

texture* direct2d_renderer::create_texture(const oem_bitmap& id)
{
	return new d2d_texture(id, imgfactory, get_current_target());
}

icon* direct2d_renderer::create_icon(const wstring& filename)
{
	return new d2d_icon(filename, imgfactory, get_current_target());
}

icon* direct2d_renderer::create_icon(HICON ico)
{
	return new d2d_icon(ico, imgfactory, get_current_target());
}

icon* direct2d_renderer::create_icon(const system_icon& ico)
{
	return new d2d_icon(ico, imgfactory, get_current_target());
}

icon* direct2d_renderer::create_icon(int id, HINSTANCE inst)
{
	return new d2d_icon(id, inst, imgfactory, get_current_target());
}

icon* direct2d_renderer::create_icon(const wstring& filename, const size& sz)
{
	return new d2d_icon(filename, sz, imgfactory, get_current_target());
}

icon* direct2d_renderer::create_icon(int id, HINSTANCE inst, const size& sz)
{
	return new d2d_icon(id, inst, sz, imgfactory, get_current_target());
}

font* direct2d_renderer::create_font(const wstring& family_name, float size, int fstyle)
{
	return new d2d_font(family_name, size, fstyle, factory, write_factory);

}

geometry* direct2d_renderer::create_geometry()
{
	return new d2d_geometry(factory);
}

radial_gradient_brush* direct2d_renderer::create_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma)
{
	return new d2d_radial_gradient_brush(get_current_target(), e, gradients, gamma);
}

graphics* direct2d_renderer::create_graphics(HWND handle, callback<void(const size&, const resizing_types&)>& cb)
{
	return new direct2d_renderer(handle, cb);
};

graphics* direct2d_renderer::create_graphics(texture* txt)
{
	return new direct2d_renderer(this, dynamic_cast<d2d_texture*>(txt));
}

graphics* direct2d_renderer::create_graphics(HDC _dc)
{
	return new direct2d_renderer(_dc);
}

font* direct2d_renderer::get_system_font(float sz, int fstyle) const
{
	HFONT f = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	LOGFONT l = {};
	GetObject(f, sizeof(LOGFONT), &l);
	font* pf = new d2d_font(l.lfFaceName, sz, fstyle, factory, write_factory);
	DeleteObject(f);
	return pf;
}
//********************************************************************************************************************************************************************************************************************************

Matrix3x2F direct2d_renderer::get_matrix(const matrix& m)
{
	return Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32);
}

void direct2d_renderer::rotate(float angle)
{
	get_current_target()->SetTransform(get_matrix(matrix::rotation(angle)));
}

void direct2d_renderer::rotate_at(float angle, const point& p)
{
	get_current_target()->SetTransform(get_matrix(matrix::rotation(angle, p)));
}

void direct2d_renderer::translate(float x, float y)
{
	get_current_target()->SetTransform(get_matrix(matrix::translation(x, y)));
}

void direct2d_renderer::scale(float fac_x, float fac_y)
{
	get_current_target()->SetTransform(get_matrix(matrix::scalation(fac_x, fac_y)));
}

void direct2d_renderer::shear(float fac_x, float fac_y)
{
	get_current_target()->SetTransform(get_matrix(matrix::shearing(fac_x, fac_y)));
}

void direct2d_renderer::skew(float x, float y, float anglex, float angley)
{
	get_current_target()->SetTransform(get_matrix(matrix::skewing(x, y, anglex, angley)));
}

matrix direct2d_renderer::get_transform() const
{
	Matrix3x2F m;
	get_current_target()->GetTransform(&m);
	return matrix(m._11, m._12, m._21, m._22, m._31, m._32);
};

void direct2d_renderer::set_transform(const matrix& m)
{
	get_current_target()->SetTransform(get_matrix(m));
}

void direct2d_renderer::set_text_rendering_mode(const text_rendering_modes& mode)
{
	textMode = mode;
	switch(mode)
	{
	case text_rendering_modes::antialias:
		get_current_target()->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
		break;
	case text_rendering_modes::cleartype:
		get_current_target()->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
		break;
	case text_rendering_modes::system_default:
		get_current_target()->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
	}
}

text_rendering_modes direct2d_renderer::get_text_rendering_mode() const {return textMode;}

void direct2d_renderer::set_antialias(bool val)
{
	antialias = val;
	if(!get_current_target()) return;
	if(val)
		get_current_target()->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	else
		get_current_target()->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

// Graphics

// Geo
d2d_geometry::d2d_geometry(ID2D1Factory* trgt)
{
	factory = trgt;
	geo_sink = 0;
}

void d2d_geometry::begin_geometry(const point& p)
{
	if(geo)geo->Release();
	geo = 0;
	if(geo_sink)geo_sink->Release();
	geo_sink = 0;
	if(FAILED(factory->CreatePathGeometry(&geo))) throw runtime_error("Unable to create PathGeometry");
	if(FAILED(geo->Open(&geo_sink))) throw runtime_error("Unable to open Sink");
	lastPoint = p;
	geo_sink->BeginFigure(Point2F(p.x, p.y), D2D1_FIGURE_BEGIN_FILLED);
}

void d2d_geometry::end_geometry()
{
	geo_sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	if(FAILED(geo_sink->Close())) throw runtime_error("Unable to close Sink");
}

bool d2d_geometry::contains(const point& p, const matrix& m)
{
	BOOL b  = 0;
	if(FAILED(geo->FillContainsPoint(Point2F(p.x, p.y), &Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), &b))) throw invalid_argument("Invalid comparison");
	return (b != 0);
}

bool d2d_geometry::outline_contains(const point& p, pen* pe, const matrix& m)
{
	BOOL b  = 0;
	if((geo->StrokeContainsPoint(Point2F(p.x, p.y), pe->get_width(), dynamic_cast<d2d_pen*>(pe)->get_stroke_style(), &Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), &b))) throw invalid_argument("Invalid comparison");
	return (b != 0);
}

d2d_geometry::~d2d_geometry()
{
	if(geo)geo->Release();
	if(geo_sink)geo_sink->Release();
}

ID2D1Geometry* d2d_geometry::get_native_member()
{
	return geo;
}

void d2d_geometry::add_bezier(const point& p1, const point& p2, const point& p3)
{
	geo_sink->AddBezier(BezierSegment(Point2F(p1.x, p1.y), Point2F(p2.x, p2.y), Point2F(p3.x, p3.y)));
	lastPoint = p3;
}

void d2d_geometry::add_beziers(const point* ps, int count)
{
	D2D1_BEZIER_SEGMENT* segs = new D2D1_BEZIER_SEGMENT[count/3];
	for(int i = 0; i < count/3; i+=3)
		segs[i] = BezierSegment(Point2F(ps[i].x, ps[i].y), Point2F(ps[i+1].x, ps[i+1].y), Point2F(ps[i+2].x, ps[i+2].y));
	geo_sink->AddBeziers(segs, count/3);
	delete[] segs;
	lastPoint = ps[count-1];
}

void d2d_geometry::add_line(const point& p1)
{
	geo_sink->AddLine(Point2F(p1.x, p1.y));
	lastPoint = p1;
}

void d2d_geometry::add_lines(const point* ps, int count)
{
	D2D1_POINT_2F* points = new D2D1_POINT_2F[count];
	for(int i = 0; i < count; i++)
		points[i] = Point2F(ps[i].x, ps[i].y);
	geo_sink->AddLines(points, count);
	delete[] points;
	lastPoint = ps[count-1];
}

void d2d_geometry::add_rect(const rect& rc)
{
	geo_sink->EndFigure(D2D1_FIGURE_END_OPEN);
	if(FAILED(geo_sink->Close())) throw runtime_error("Unable to add Geometry");
	ID2D1RectangleGeometry* rectgeo = 0;
	if(FAILED(factory->CreateRectangleGeometry(RectF(rc.position.x, rc.position.y, rc.sizef.width + rc.position.x, rc.sizef.height + rc.position.y), &rectgeo))) throw runtime_error("Unable to add Geometry");
	ID2D1PathGeometry* newGeo = 0;
	if(FAILED(factory->CreatePathGeometry(&newGeo))) throw runtime_error("Unable to add Geometry");
	ID2D1GeometrySink* newSink = 0;
	if(FAILED(newGeo->Open(&newSink))) throw runtime_error("Unable to add Geometry");
	if(FAILED(geo->CombineWithGeometry(rectgeo, D2D1_COMBINE_MODE_UNION, Matrix3x2F::Identity(), newSink))) throw runtime_error("Unable to add Geometry");
	rectgeo->Release();
	geo_sink->Release();
	geo->Release();
	geo = newGeo;
	geo_sink = newSink;
	geo_sink->BeginFigure(Point2F(lastPoint.x, lastPoint.y), D2D1_FIGURE_BEGIN_FILLED);
}

void d2d_geometry::add_ellipse(const ellipse& e)
{
	geo_sink->EndFigure(D2D1_FIGURE_END_OPEN);
	if(FAILED(geo_sink->Close())) throw runtime_error("Unable to add Geometry");
	ID2D1EllipseGeometry* rectgeo = 0;
	if(FAILED(factory->CreateEllipseGeometry(Ellipse(Point2F(e.position.x, e.position.y), e.radius_x, e.radius_y), &rectgeo))) throw runtime_error("Unable to add Geometry");
	ID2D1PathGeometry* newGeo = 0;
	if(FAILED(factory->CreatePathGeometry(&newGeo))) throw runtime_error("Unable to add Geometry");
	ID2D1GeometrySink* newSink = 0;
	if(FAILED(newGeo->Open(&newSink))) throw runtime_error("Unable to add Geometry");
	if(FAILED(geo->CombineWithGeometry(rectgeo, D2D1_COMBINE_MODE_UNION, Matrix3x2F::Identity(), newSink))) throw runtime_error("Unable to add Geometry");
	rectgeo->Release();
	geo_sink->Release();
	geo->Release();
	geo = newGeo;
	geo_sink = newSink;
	geo_sink->BeginFigure(Point2F(lastPoint.x, lastPoint.y), D2D1_FIGURE_BEGIN_FILLED);
}

void d2d_geometry::add_rounded_rect(const rect& rc, float radiusX, float radiusY)
{
	geo_sink->EndFigure(D2D1_FIGURE_END_OPEN);
	if(FAILED(geo_sink->Close())) throw runtime_error("Unable to add Geometry");
	ID2D1RoundedRectangleGeometry* rectgeo = 0;
	if(FAILED(factory->CreateRoundedRectangleGeometry(RoundedRect(RectF(rc.position.x, rc.position.y, rc.sizef.width + rc.position.x, rc.position.y + rc.sizef.height), radiusX, radiusY), &rectgeo))) throw runtime_error("Unable to add Geometry");
	ID2D1PathGeometry* newGeo = 0;
	if(FAILED(factory->CreatePathGeometry(&newGeo))) throw runtime_error("Unable to add Geometry");
	ID2D1GeometrySink* newSink = 0;
	if(FAILED(newGeo->Open(&newSink))) throw runtime_error("Unable to add Geometry");
	if(FAILED(geo->CombineWithGeometry(rectgeo, D2D1_COMBINE_MODE_UNION, Matrix3x2F::Identity(), newSink))) throw runtime_error("Unable to add Geometry");
	rectgeo->Release();
	geo_sink->Release();
	geo->Release();
	geo = newGeo;
	geo_sink = newSink;
	geo_sink->BeginFigure(Point2F(lastPoint.x, lastPoint.y), D2D1_FIGURE_BEGIN_FILLED);
}

void d2d_geometry::add_polygon(const point* ps, int count)
{
	if(count < 3)
		throw logic_error("Count must be at least 3");
	for(int i = 0; i < count; i++)
		geo_sink->AddLine(Point2F(ps[i].x, ps[i].y));
	geo_sink->AddLine(Point2F(ps[0].x, ps[0].y));
	lastPoint = ps[0];
}

void d2d_geometry::add_geometry(geometry* geo_)
{
	ID2D1PathGeometry* newgeo = 0;
	factory->CreatePathGeometry(&newgeo);
	ID2D1GeometrySink* newsink = 0;
	newgeo->Open(&newsink);
	geo_sink->EndFigure(D2D1_FIGURE_END_OPEN);
	if(FAILED(geo_sink->Close()))
		throw runtime_error("Unable to close sink");
	geo->CombineWithGeometry(dynamic_cast<d2d_geometry*>(geo_)->get_native_member(), D2D1_COMBINE_MODE_XOR, Matrix3x2F::Identity(), newsink);
	geo_sink->Release();
	geo->Release();
	geo = newgeo;
	geo_sink = newsink;
	newsink->BeginFigure(Point2F(lastPoint.x, lastPoint.y), D2D1_FIGURE_BEGIN_FILLED);
}

rect d2d_geometry::get_bounds(const matrix& tr) const
{
	D2D1_RECT_F rc;
	if(FAILED(geo->GetBounds(Matrix3x2F(tr.m11, tr.m12, tr.m21, tr.m22, tr.m31, tr.m32), &rc)))
		throw invalid_argument("Could not receive Geometrybounds");;
	return rect(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
}
// Geo

// Pen
d2d_pen::d2d_pen(brush* org, ID2D1Brush* br, ID2D1Factory* f, float w, dash_cap startcap, dash_cap endcap, dash_cap caps, dash_style dstyle, float offset) : org_br(org), width(w), start_cap(startcap), end_cap(endcap), cap(caps), style(dstyle), dash_offset(offset), brush_(br), factory(f) 
{
	recreate();
}

void d2d_pen::recreate() 
{
	if(FAILED(factory->CreateStrokeStyle(StrokeStyleProperties(static_cast<D2D1_CAP_STYLE>(start_cap), static_cast<D2D1_CAP_STYLE>(end_cap), static_cast<D2D1_CAP_STYLE>(cap), D2D1_LINE_JOIN_MITER, 10.f, static_cast<D2D1_DASH_STYLE>(style), dash_offset), 0, 0, &stroke_style))) 
		throw runtime_error("Unable to create Strokestyle");
}
// Pen

// Icon
d2d_icon::d2d_icon(const wstring& filename, IWICImagingFactory* fact, ID2D1RenderTarget* _target) : target(_target), factory(fact)
{
	ico = reinterpret_cast<HICON>(LoadImage(NULL, filename.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_from_ico();
}

d2d_icon::d2d_icon(HICON  ico_, IWICImagingFactory* fact, ID2D1RenderTarget* _target) : target(_target), factory(fact)
{
	ico = DuplicateIcon(NULL, ico_);
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_from_ico();
}

d2d_icon::d2d_icon(const system_icon& ico_, IWICImagingFactory* fact, ID2D1RenderTarget* _target) : target(_target), factory(fact)
{
	ico = LoadIcon(NULL, MAKEINTRESOURCE(ico_));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_from_ico();
}

d2d_icon::d2d_icon(int id, HINSTANCE inst, IWICImagingFactory* fact, ID2D1RenderTarget* _target) : target(_target), factory(fact)
{
	ico = LoadIcon(inst, MAKEINTRESOURCE(id));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_from_ico();
}

d2d_icon::d2d_icon(const wstring& filename, const size& sz, IWICImagingFactory* fact, ID2D1RenderTarget* _target) : target(_target), factory(fact)
{
	ico = reinterpret_cast<HICON>(LoadImage(NULL, filename.c_str(), IMAGE_ICON, static_cast<int>(sz.width), static_cast<int>(sz.height), LR_LOADFROMFILE));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_from_ico();
}	

d2d_icon::d2d_icon(int id, HINSTANCE inst, const size& sz, IWICImagingFactory* fact, ID2D1RenderTarget* _target) : target(_target), factory(fact)
{
	ico = reinterpret_cast<HICON>(LoadImage(inst, MAKEINTRESOURCE(id), IMAGE_ICON, static_cast<int>(sz.width), static_cast<int>(sz.height), 0));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_from_ico();
}

d2d_icon::~d2d_icon()
{
	bmp->Release();
	DestroyIcon(ico);
}

size d2d_icon::get_size() const
{
	auto rc = bmp->GetSize();
	return size(rc.width, rc.height);
}

void d2d_icon::init_from_ico()
{
	IWICBitmap* w_bmp = 0;
	HRESULT hr = factory->CreateBitmapFromHICON(ico, &w_bmp);
	IWICFormatConverter* converter = 0;
	hr = factory->CreateFormatConverter(&converter);
	hr = converter->Initialize(w_bmp, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
	hr = factory->CreateBitmapFromSource(converter, WICBitmapCacheOnLoad, &w_bmp); 
	hr = target->CreateBitmapFromWicBitmap(converter, &bmp);
	w_bmp->Release();
	converter->Release();
	if(FAILED(hr))
		throw runtime_error("Unable to initialize Bitmap");
}
// Icon

};
};