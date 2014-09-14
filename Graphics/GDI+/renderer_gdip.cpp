#include "stdafx.h"
#include "renderer_gdip.h"

namespace gcl { namespace std_renderer {

// Font
gdiplus_font::gdiplus_font(const wstring& family_name, float size, int fstyle) : f(0), family(family_name), sz(size), style(fstyle), frm(new StringFormat())
{
	f = new Font(&FontFamily(family_name.c_str()), size, fstyle, UnitPixel);
	if(!f->IsAvailable()) throw invalid_argument("Font is not available");
}

gdiplus_font::~gdiplus_font()
{
	delete f;
	delete frm;
}

bool gdiplus_font::operator==(font* f) 
{
	return (family == f->get_family() && sz == f->get_size() && style == f->get_style());
}

rect gdiplus_font::get_metrics(const wstring& str, const size& clip, graphics* g) const
{
	if(!g)
		throw invalid_argument("Invalid graphics");
	Graphics* g2 = dynamic_cast<gdiplus_renderer*>(g)->get_graphics();
	if(!g2)
		throw invalid_argument("Invalid graphics");
	Gdiplus::RectF rout;
	g2->MeasureString(str.c_str(), str.length(), f, Gdiplus::RectF(0.f, 0.f, clip.width, clip.height), frm, &rout);
	return rect(rout.X, rout.Y, rout.Width, rout.Height);
}
// Font

// Texture
gdiplus_texture::gdiplus_texture(const wstring& filename) : data(0) 
{
	img = new Bitmap(filename.c_str()); 
	if(!img) 
		throw invalid_argument("Unable to initialize Image");
}

gdiplus_texture::gdiplus_texture(int id, LPWSTR type, HINSTANCE inst) : data(0) 
{
	if(FAILED(load_texture_resource(id, type, inst))) 
		throw invalid_argument("Unable to initialize Image");
}

gdiplus_texture::gdiplus_texture(Bitmap* bmp) : data(0)
{
	img = bmp->Clone(0.f, 0.f, (float)bmp->GetWidth(), (float)bmp->GetHeight(), PixelFormat32bppPARGB);
	if(!img)
		throw runtime_error("Unable to copy Bitmap");
}

gdiplus_texture::gdiplus_texture(const size& sz)
{
	if(sz.width <= 0 || sz.height <= 0)
		throw invalid_argument("Invalid size");
	img = new Bitmap(static_cast<INT>(sz.width), static_cast<INT>(sz.height));
}

gdiplus_texture::gdiplus_texture(HBITMAP bmp, HPALETTE hP)
{
	img = new Bitmap(bmp, hP);
}

gdiplus_texture::gdiplus_texture(HICON ico)
{
	img = new Bitmap(ico);
}

gdiplus_texture::gdiplus_texture(const oem_bitmap& id)
{
	HBITMAP hbmp = static_cast<HBITMAP>(LoadImage(NULL, MAKEINTRESOURCE(id), IMAGE_BITMAP, 0, 0, LR_SHARED | LR_DEFAULTSIZE));
	if(!hbmp)
		throw runtime_error("Unable to load Bitmap");
	img = new Bitmap(hbmp, 0);
}

unsigned char* gdiplus_texture::alloc()
{
	if(data)
		throw invalid_argument("Data already allocated");
	data = new BitmapData();
	if(GDI_FAIL(img->LockBits(&Gdiplus::Rect(0, 0, img->GetWidth(), img->GetHeight()), ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppPARGB, data)))
		throw runtime_error("Unable to lock Bits");
	return static_cast<unsigned char*>(data->Scan0);
}

int gdiplus_texture::get_stride()
{
	if(!data)
		throw invalid_argument("Data isn't allocated yet");
	return data->Stride;
}

void gdiplus_texture::free()
{
	if(!data)
		throw invalid_argument("Data isn't allocated yet");
	img->UnlockBits(data);
	delete data;
	data = 0;
}

HRESULT gdiplus_texture::load_texture_resource(int id, LPWSTR type, HINSTANCE inst)
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
}
// Texture

// Solidbrush
gdiplus_solid_brush::gdiplus_solid_brush(Color& c) : opacity(1.0f) 
{
	br = new SolidBrush(c); c_ = colour(c.GetR(), c.GetG(), c.GetB(), c.GetA());
}

void gdiplus_solid_brush::set_colour(const colour& c) 
{
	c_ = c; 
	if(GDI_FAIL(br->SetColor(Color(c.opacity(opacity).get_value())))) 
		throw runtime_error("Unable to set colour of GDI+ SolidBrush");
}
// Solidbrush

// Texturebrush
gdiplus_texture_brush::gdiplus_texture_brush(texture* image) : img(dynamic_cast<gdiplus_texture*>(image)), mode(wrap_modes::wrap) 
{
	br = new TextureBrush(img->get_native_member(), WrapModeTile);
}

matrix gdiplus_texture_brush::get_transform() const 
{
	Matrix m; 
	if(GDI_FAIL(br->GetTransform(&m))) 
		throw runtime_error("Unable to receive matrix");
	float f[5] = {};
	if(GDI_FAIL(m.GetElements(f))) 
		throw runtime_error("Unable to get elements");
	return matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
}
// Texturebrush

// Lineargradientbrush
gdiplus_linear_gradient_brush::gdiplus_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode) : opacity(1.f), stops(gradients), start_p(start), end_p(end), gamma_(gamma), mode(wrapmode), cnt(gradients.get_count()), m(matrix::identity())
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
}

gdiplus_linear_gradient_brush::~gdiplus_linear_gradient_brush()
{
	if(br)delete br; br = 0;
	delete[] pos_f;
	delete[] colr_f;
}

void gdiplus_linear_gradient_brush::set_transform(const matrix& m_) 
{
	m = m_;
	Matrix mold;
	br->GetTransform(&mold);
	float f[6] = {};
	if(GDI_FAIL(mold.GetElements(f))) throw runtime_error("Unable to get Matrixelements");
	matrix mnew = m * matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
	if(GDI_FAIL(br->SetTransform(&Matrix(mnew.m11, mnew.m12, mnew.m21, mnew.m22, mnew.m31, mnew.m32)))) throw runtime_error("Unable to set matrix");
}

matrix gdiplus_linear_gradient_brush::get_transform() const 
{
	Matrix m; 
	if(GDI_FAIL(br->GetTransform(&m))) 
		throw runtime_error("Unable to receive Matrix");
	float f[6] = {};
	if(GDI_FAIL(m.GetElements(f))) 
		throw runtime_error("Unable to get Elements");
	return matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
}

void gdiplus_linear_gradient_brush::set_opacity(float val) 
{
	opacity = val; 
	create_colour(colr_f, cnt); 
	create_br(start_p, end_p, pos_f, colr_f, cnt, gamma_, mode);
}

void gdiplus_linear_gradient_brush::set_rect(const point& p1, const point& p2) 
{
	start_p = p1; 
	end_p = p2; 
	create_br(p1, p2, pos_f, colr_f, cnt, gamma_, mode);
}

void gdiplus_linear_gradient_brush::set_gradients(gradient_stop& gradients)
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
}

void gdiplus_linear_gradient_brush::create_br(const point& start, const point& end, float* pos, Color* colours, int count, bool gamma, const wrap_modes& wrapmode)
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
}

void gdiplus_linear_gradient_brush::create_colour(Color* cl, int count)
{
	for(int i = 0; i < count; i++)
	{
		colr_f[i] = Color(static_cast<BYTE>(cl[i].GetAlpha()*opacity), cl[i].GetRed(), cl[i].GetGreen(), cl[i].GetBlue());
	}
}
// Lineargradientbrush

// Radialgradientbrush
gdiplus_radial_gradient_brush::gdiplus_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma) : elp(e), stops(gradients), gamma_(gamma)
{
	br = 0;
	opacity = 1.f;
	recreate_brush();
}

gdiplus_radial_gradient_brush::~gdiplus_radial_gradient_brush()
{
	delete br;
	delete[] colr_f;
	delete[] pos_f;
}

void gdiplus_radial_gradient_brush::set_transform(const matrix& m_)
{
	if(m == m_) return;
	m = m_;
	if(GDI_FAIL(br->SetTransform(&Matrix(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32))))
		throw invalid_argument("Unable to set transform");
}

void gdiplus_radial_gradient_brush::set_opacity(float val)
{
	if(val == opacity) return;
	if(val < 0 || val > 1)
		throw invalid_argument("The value must be between 0 and 1");
	opacity = val;
	recreate_brush();
}

void gdiplus_radial_gradient_brush::set_ellipse(const ellipse& e)
{
	if(e == elp) return;
	elp = e;
	GraphicsPath p;
	p.StartFigure();
	p.AddEllipse(elp.get_rect().position.x, elp.get_rect().position.y, elp.get_rect().sizef.width, elp.get_rect().sizef.height);
	p.CloseAllFigures();
	br->SetGraphicsPath(&p);
	br->SetCenterPoint(PointF(elp.position.x+offs.x, elp.position.y+offs.y));
}

void gdiplus_radial_gradient_brush::set_gradient_origin_offset(const point& p)
{
	if(offs == p) return;
	offs = p;
	br->SetCenterPoint(PointF(elp.position.x+offs.x, elp.position.y+offs.y));
}

void gdiplus_radial_gradient_brush::set_gradients(gradient_stop& gradients)
{
	stops = gradients;
	recreate_brush();
}

void gdiplus_radial_gradient_brush::recreate_brush()
{
	if(br)
	{
		delete br;
		delete[] colr_f;
		delete[] pos_f;
	}
	colr_f = new Color[stops.get_count()];
	pos_f = new float[stops.get_count()];
	for(unsigned int i = 0; i < stops.get_count(); i++)
	{
		colr_f[stops.get_count()-i-1] = Color(static_cast<BYTE>(stops.get_colours()[i].a*opacity), stops.get_colours()[i].r, stops.get_colours()[i].g, stops.get_colours()[i].b);
		pos_f[i] = stops.get_positions()[i];
	}
	GraphicsPath p;
	p.StartFigure();
	p.AddEllipse(elp.get_rect().position.x, elp.get_rect().position.y, elp.get_rect().sizef.width, elp.get_rect().sizef.height);
	p.CloseAllFigures();
	br = new PathGradientBrush(&p);
	br->SetCenterPoint(PointF(elp.position.x+offs.x, elp.position.y+offs.y));
	br->SetInterpolationColors(colr_f, pos_f, stops.get_count());
	br->SetGammaCorrection(gamma_);
	br->SetTransform(&Matrix(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32));
	br->SetWrapMode(WrapMode::WrapModeClamp);
}
// Radialgradientbrush

// Pen
gdiplus_pen::gdiplus_pen(brush* org, Brush* br, float w, dash_cap startcap, dash_cap endcap, dash_cap caps, dash_style dstyle, float offset) : org_br(org), width(w), start_cap(startcap), end_cap(endcap), cap(caps), style(dstyle), dash_offset(offset), gdi_br(br)
{
	p = new Pen(br, width);
	p->SetAlignment(PenAlignmentCenter);
	p->SetDashCap(static_cast<DashCap>(caps));
	p->SetDashOffset(offset);
	p->SetDashStyle(static_cast<DashStyle>(style));
	p->SetEndCap(static_cast<LineCap>(endcap)); 
	p->SetStartCap(static_cast<LineCap>(start_cap));
}

void gdiplus_pen::update()
{
	if(p)
		p->SetBrush(gdi_br);
}
// Pen

// Icon
gdiplus_icon::gdiplus_icon(const wstring& filename)
{
	ico = static_cast<HICON>(LoadImage(NULL, filename.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_size();
}

gdiplus_icon::gdiplus_icon(HICON ico_)
{
	ico = DuplicateIcon(NULL, ico_);
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_size();
}

gdiplus_icon::gdiplus_icon(const system_icon& ico_)
{
	ico = LoadIcon(NULL, MAKEINTRESOURCE(ico_));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_size();
}

gdiplus_icon::gdiplus_icon(int id, HINSTANCE inst)
{
	ico = LoadIcon(inst, MAKEINTRESOURCE(id));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_size();
}

gdiplus_icon::gdiplus_icon(const wstring& filename, const size& sz)
{
	ico = static_cast<HICON>(LoadImage(NULL, filename.c_str(), IMAGE_ICON, static_cast<int>(sz.width), static_cast<int>(sz.height), LR_LOADFROMFILE));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_size();
}

gdiplus_icon::gdiplus_icon(int id, HINSTANCE inst, const size& sz)
{
	ico = static_cast<HICON>(LoadImage(inst, MAKEINTRESOURCE(id), IMAGE_ICON, static_cast<int>(sz.width), static_cast<int>(sz.height), 0));
	if(!ico)
		throw invalid_argument("Unable to load icon");
	init_size();
}

void gdiplus_icon::init_size()
{		
	ICONINFOEX info = {};
	GetIconInfoEx(ico, &info);
	BITMAP bmp = {};
	GetObject(&info, sizeof(bmp), &bmp);
	sz = size(static_cast<float>(bmp.bmWidth), static_cast<float>(bmp.bmHeight));
}
// Icon

// Graphics
bool gdiplus_renderer::is_start_up = false;
size_t gdiplus_renderer::inst_count = 0;

gdiplus_renderer::gdiplus_renderer(HWND handle_, callback<void(const size&, const resizing_types&)>& cb) : bmp(0), tokens(0), context(0), g(0), buffered_context(0), buffered_surface(0), raster_operations(ternary_raster_operations::source_copy), textMode(text_rendering_modes::system_default)
{
	inst_count++;
	handle = handle_;
	type = graphics_type::handle;
	create_resources();
}

gdiplus_renderer::gdiplus_renderer(HDC dc) : bmp(0), tokens(0), context(dc), g(0), buffered_context(0), buffered_surface(0), raster_operations(ternary_raster_operations::source_copy), textMode(text_rendering_modes::system_default)
{
	inst_count++;
	handle = 0;
	type = graphics_type::dc;
	create_resources();
}

gdiplus_renderer::gdiplus_renderer(texture* text) : tokens(0), g(0), buffered_context(0), buffered_surface(0), raster_operations(ternary_raster_operations::source_copy), textMode(text_rendering_modes::system_default)
{
	inst_count++;
	handle = 0;
	type = graphics_type::texture;
	bmp = dynamic_cast<gdiplus_texture*>(text)->get_native_member();
	create_resources();
}

void gdiplus_renderer::create_resources()
{
	if(is_start_up) return;
	if(GdiplusStartup(&tokens, &startinput, NULL) != Ok)
		throw invalid_argument("Could not initialize GDI+ renderer");
	is_start_up = true;
}

gdiplus_renderer::~gdiplus_renderer()
{
	inst_count--;
	if(type == graphics_type::dc)
	{
		DeleteDC(context);
		context = 0;
	}
	if(!inst_count)
		GdiplusShutdown(tokens);
}

void gdiplus_renderer::begin()
{
	if(type == graphics_type::handle)
	{
		context = BeginPaint(handle, &ps);
		buffered_context = CreateCompatibleDC(context);
		size sz = get_surface_size();
		buffered_surface = CreateCompatibleBitmap(context, static_cast<int>(sz.width), static_cast<int>(sz.height));
		SelectObject(buffered_context, buffered_surface);
		g = new Graphics(buffered_context);
		if(!g) throw runtime_error("Unable to create Graphics");
	}
	else if(type == graphics_type::dc)
	{
		buffered_context = CreateCompatibleDC(context);
		RECT rc = {};
		GetWindowRect(handle, &rc);
		size sz(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom-rc.top));
		buffered_surface = CreateCompatibleBitmap(context, static_cast<int>(sz.width), static_cast<int>(sz.height));
		SelectObject(buffered_context, buffered_surface);
		g = new Graphics(context);
		if(!g) throw runtime_error("Unable to create Graphics");
	}
	else if(type == graphics_type::texture)
	{
		g = Graphics::FromImage(bmp);
		if(!g) throw runtime_error("Unable to create Graphics");
	}
	if(antialias)
		g->SetSmoothingMode(SmoothingModeAntiAlias);
	else
		g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetTransform(&Matrix());
	set_text_rendering_mode(textMode);
}

void gdiplus_renderer::clear(const colour& c)
{
	g->Clear(Color(c.get_value()));
}

void gdiplus_renderer::end()
{
	if(type == graphics_type::handle)
	{
		size sz = get_surface_size();
		BitBlt(context, 0, 0, static_cast<int>(sz.width), static_cast<int>(sz.height), buffered_context, 0, 0, static_cast<DWORD>(raster_operations));
		delete g;
		g = 0;
		DeleteObject(buffered_surface);
		buffered_surface = 0;
		DeleteDC(buffered_context);
		buffered_context = 0;
		EndPaint(handle, &ps);
		context = 0;
	}
	else if(type == graphics_type::dc)
	{
		RECT rc = {};
		GetWindowRect(handle, &rc);
		size sz(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom-rc.top));
		/*BitBlt(context, 0, 0, static_cast<int>(sz.width), static_cast<int>(sz.height), buffered_context, 0, 0, static_cast<DWORD>(raster_operations));*/
		delete g;
		g = 0;
		DeleteObject(buffered_surface);
		buffered_surface = 0;
		DeleteDC(buffered_context);
		buffered_context = 0;
	}
	else if(type == graphics_type::texture)
	{
		context = 0;
		delete g; 
		g = 0;
	}
}

void gdiplus_renderer::set_antialias(bool val)
{
	antialias = val;
	if(!g) return;
	if(antialias)
		g->SetSmoothingMode(SmoothingModeAntiAlias);
	else
		g->SetSmoothingMode(SmoothingModeHighQuality);
}

Brush* gdiplus_renderer::get_brush(brush* br)
{
	switch(br->get_type())
	{
	case brush_types::solid_brush:
			return dynamic_cast<gdiplus_solid_brush*>(br)->get_native_member();
	case brush_types::texture_brush:
		return dynamic_cast<gdiplus_texture_brush*>(br)->get_native_member();
	case brush_types::linear_gradient_brush:
		return dynamic_cast<gdiplus_linear_gradient_brush*>(br)->get_native_member();
	case brush_types::radial_gradient_brush:
		return dynamic_cast<gdiplus_radial_gradient_brush*>(br)->get_native_member();
	default:
		throw invalid_argument("Unkown Brushtype");
	}
}

solid_brush* gdiplus_renderer::create_solid_brush(const colour& c)
{
	return new gdiplus_solid_brush(Color(c.get_value()));
}

texture_brush* gdiplus_renderer::create_texture_brush(texture* image)
{
	return new gdiplus_texture_brush(image);
}

linear_gradient_brush* gdiplus_renderer::create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode)
{
	return new  gdiplus_linear_gradient_brush(start, end, gradients, gamma, wrapmode);
}

pen* gdiplus_renderer::create_pen(brush* b, float width, dash_cap startcap, dash_cap endcap, dash_cap dash_cap_, dash_style dash_style_, float offset)
{
	return new gdiplus_pen(b, get_brush(b), width, startcap, endcap, dash_cap_, dash_style_, offset);
}

texture* gdiplus_renderer::create_texture(const wstring& filename)
{
	return new gdiplus_texture(filename);
}

texture* gdiplus_renderer::create_texture(int id, LPWSTR type, HINSTANCE inst)
{
	return new gdiplus_texture(id, type, inst);
}

texture* gdiplus_renderer::create_texture(const size& sz)
{
	return new gdiplus_texture(sz);
}

texture* gdiplus_renderer::create_texture(HBITMAP hbmp, HPALETTE hP)
{
	return new gdiplus_texture(hbmp, hP);
}

texture* gdiplus_renderer::create_texture(HICON ico)
{
	return new gdiplus_texture(ico);
}

texture* gdiplus_renderer::create_texture(const oem_bitmap& id)
{
	return new gdiplus_texture(id);
}

icon* gdiplus_renderer::create_icon(const wstring& filename)
{
	return new gdiplus_icon(filename);
}

icon* gdiplus_renderer::create_icon(HICON ico)
{
	return new gdiplus_icon(ico);
}

icon* gdiplus_renderer::create_icon(const system_icon& ico)
{
	return new gdiplus_icon(ico);
}

icon* gdiplus_renderer::create_icon(int id, HINSTANCE inst)
{
	return new gdiplus_icon(id, inst);
}

icon* gdiplus_renderer::create_icon(const wstring& filename, const size& sz)
{
	return new gdiplus_icon(filename, sz);
}

icon* gdiplus_renderer::create_icon(int id, HINSTANCE inst, const size& sz)
{
	return new gdiplus_icon(id, inst, sz);
}

font* gdiplus_renderer::create_font(const wstring& family_name, float size, int fstyle)
{
	return new gdiplus_font(family_name, size, fstyle);
}

geometry* gdiplus_renderer::create_geometry()
{
	return new gdiplus_geometry();
}

graphics* gdiplus_renderer::create_graphics(HWND handle, callback<void(const size&, const resizing_types&)>& cb)
{
	return new gdiplus_renderer(handle, cb);
}

graphics* gdiplus_renderer::create_graphics(HDC dc)
{
	return new gdiplus_renderer(dc);
}

graphics* gdiplus_renderer::create_graphics(texture* text)
{
	return new gdiplus_renderer(text);
};

radial_gradient_brush* gdiplus_renderer::create_radial_gradient_brush(const ellipse& e, const gradient_stop& gradients, bool gamma)
{
	return new gdiplus_radial_gradient_brush(e, gradients, gamma);
}

font* gdiplus_renderer::get_system_font(float sz, int fstyle) const
{
	HFONT f = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	LOGFONT l = {};
	GetObject(f, sizeof(LOGFONT), &l);
	font* pf = new gdiplus_font(l.lfFaceName, sz, fstyle);
	DeleteObject(f);
	return pf;
}

void gdiplus_renderer::push_clip(const clip& cl)
{
	if(!g)
		throw invalid_argument("Invalid drawingstate");
	if(clips.size() != 0)
	{
		Region* rgc = clips.top()->Clone();
		Region* rg = 0;
		if(cl.is_rectangle_clip())
		{
			rect rc = cl.get_rect();
			rg = new Region(Gdiplus::RectF(rc.get_x(), rc.get_y(), rc.get_width(), rc.get_height()));
		}
		else
		{
			rg = new Region(dynamic_cast<gdiplus_geometry*>(cl.get_geometry())->get_native_member());
		}
		rg->Intersect(rgc);
		if(rgc->IsEmpty(g)) // Does not intersect with new clip
		{
			clips.push(shared_ptr<Region>(rgc));
			delete rg;
		}
		else
		{
			clips.push(shared_ptr<Region>(rg));
			delete rgc;
		}
	}
	else
	{
		if(cl.is_rectangle_clip())
		{
			rect rc = cl.get_rect();
			clips.push(shared_ptr<Region>(new Region(Gdiplus::RectF(rc.get_x(), rc.get_y(), rc.get_width(), rc.get_height()))));
		}
		else
		{
			clips.push(shared_ptr<Region>(new Region(dynamic_cast<gdiplus_geometry*>(cl.get_geometry())->get_native_member())));
		}
	}
	g->SetClip(clips.top().get());
}

void gdiplus_renderer::pop_clip()
{
	if(clips.empty())
		throw invalid_argument("Empty Clipstack");
	if(!g)
		throw invalid_argument("Invalid drawingstate");
	auto rgp = clips.top();
	rgp.reset();
	clips.pop();
	if(clips.size() == 0)
		g->ResetClip();
	else
		g->SetClip(clips.top().get());
}

//********************************************************************************************************************************************************************************************
void gdiplus_renderer::fill_rect(const rect& rc, brush* b)
{
	g->FillRectangle(get_brush(b), rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height);
}

void gdiplus_renderer::fill_rects(const rect* rcs, int count, brush* b)
{
	Gdiplus::RectF* p = new Gdiplus::RectF[count];
	for(int i = 0; i < count; i++)
		p[i] = Gdiplus::RectF(rcs[i].position.x, rcs[i].position.y, rcs[i].sizef.width, rcs[i].sizef.height);
	g->FillRectangles(get_brush(b), p, count);
	delete[] p;
}

void gdiplus_renderer::fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b)
{
	if(radiusx > 45)  radiusx = 45;
	if(radiusy > 45) radiusy = 45;
	radiusx *= 2;
	radiusy *= 2;
	GraphicsPath path;
	path.AddArc(rc.position.x, rc.position.y, radiusx, radiusy, 180, 90);
	path.AddArc(rc.position.x + rc.sizef.width - radiusx, rc.position.y, radiusx, radiusy, 270, 90);
	path.AddArc(rc.position.x + rc.sizef.width - radiusx, rc.position.y + rc.sizef.height - radiusy, radiusx, radiusy, 0, 90);
	path.AddArc(rc.position.x, rc.position.y + rc.sizef.height - radiusy, radiusx, radiusy, 90, 90);
	path.CloseAllFigures();
	g->FillPath(get_brush(b), &path);
}

void gdiplus_renderer::fill_geometry(geometry* geo, brush* b)
{
	g->FillPath(get_brush(b), dynamic_cast<gdiplus_geometry*>(geo)->get_native_member());
}

void gdiplus_renderer::fill_polygon(const point* ps, int count, brush* b)
{
	if(count < 3)
		throw logic_error("Count must be at least 3");
	PointF* pfs = new PointF[count];
	for(int i = 0; i < count; i++)
		pfs[i] = PointF(ps[i].x, ps[i].y);
	g->FillPolygon(get_brush(b), pfs, count);
	delete[] pfs;
}

void gdiplus_renderer::fill_ellipse(const ellipse& e, brush* b)
{
	rect dimensions = e.get_rect();
	g->FillEllipse(get_brush(b), dimensions.position.x, dimensions.position.y, dimensions.sizef.width, dimensions.sizef.height);
}

//**************************************************************************************************************************************************************************************************************

void gdiplus_renderer::draw_line(const point& p1, const point& p2, pen* p)
{
	gdiplus_pen* p_ = dynamic_cast<gdiplus_pen*>(p);
	g->DrawLine(p_->get_native_member(), PointF(p1.x, p1.y), PointF(p2.x, p2.y));
}

void gdiplus_renderer::draw_lines(const point* ps, int count, pen* p)
{
	gdiplus_pen* p_ = dynamic_cast<gdiplus_pen*>(p);
	PointF* pf = new PointF[count];
	for(int i = 0; i < count; i++)
		pf[i] = PointF(ps[i].x, ps[i].y);
	g->DrawLines(p_->get_native_member(), pf, count); 
}

void gdiplus_renderer::draw_rect(const rect rc, pen* p)
{
	gdiplus_pen* p_ = dynamic_cast<gdiplus_pen*>(p);
	g->DrawRectangle(p_->get_native_member(), rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height);
}

void gdiplus_renderer::draw_ellipse(const ellipse& e, pen* p)
{
	gdiplus_pen* p_ = dynamic_cast<gdiplus_pen*>(p);
	rect rc = e.get_rect();
	g->DrawEllipse(p_->get_native_member(), rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height);
}

void gdiplus_renderer::draw_rects(const rect* rcs, int count, pen* p)
{
	gdiplus_pen* p_ = dynamic_cast<gdiplus_pen*>(p);
	Gdiplus::RectF* pf = new Gdiplus::RectF[count];
	for(int i = 0; i < count; i++)
		pf[i] = Gdiplus::RectF(rcs[i].position.x, rcs[i].position.y, rcs[i].sizef.width, rcs[i].sizef.height);
	g->DrawRectangles(p_->get_native_member(), pf, count);
}

void gdiplus_renderer::draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p)
{
	if(radiusx > 45)  radiusx = 45;
	if(radiusy > 45) radiusy = 45;
	radiusx *= 2;
	radiusy *= 2;
	gdiplus_pen* p_ = dynamic_cast<gdiplus_pen*>(p);
	GraphicsPath path;
	path.AddArc(rc.position.x, rc.position.y, radiusx, radiusy, 180, 90);
	path.AddArc(rc.position.x + rc.sizef.width - radiusx, rc.position.y, radiusx, radiusy, 270, 90);
	path.AddArc(rc.position.x + rc.sizef.width - radiusx, rc.position.y + rc.sizef.height - radiusy, radiusx, radiusy, 0, 90);
	path.AddArc(rc.position.x, rc.position.y + rc.sizef.height - radiusy, radiusx, radiusy, 90, 90);
	path.CloseAllFigures();
	g->DrawPath(p_->get_native_member(), &path);
}

void gdiplus_renderer::draw_polygon(const point* ps, int count, pen* p)
{
	if(count < 3)
		throw logic_error("Count must be at least 3");
	PointF* pfs = new PointF[count];
	for(int i = 0; i < count; i++)
		pfs[i] = PointF(ps[i].x, ps[i].y);
	g->DrawPolygon(dynamic_cast<gdiplus_pen*>(p)->get_native_member(), pfs, count);
	delete[] pfs;
}

void gdiplus_renderer::draw_geometry(geometry* geo, pen* p)
{
	g->DrawPath(dynamic_cast<gdiplus_pen*>(p)->get_native_member(), dynamic_cast<gdiplus_geometry*>(geo)->get_native_member());
}

void gdiplus_renderer::draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format)
{
	gdiplus_font* gdipFont = dynamic_cast<gdiplus_font*>(fn);
	auto style = gdipFont->get_native_format();
	int strFormat = 0;
	switch(format)
	{
	case string_format::direction_right_to_left:
		strFormat = StringFormatFlagsDirectionRightToLeft;
	}
	style->SetFormatFlags(strFormat);
	style->SetAlignment(StringAlignmentNear);
	g->DrawString(str.c_str(), static_cast<int>(str.length()), gdipFont->get_native_font(), PointF(origin.x, origin.y), style, get_brush(b));
}

void gdiplus_renderer::draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format, const horizontal_string_align& clipping_h, const vertical_string_align& clipping_v)
{
	gdiplus_font* gdipFont = dynamic_cast<gdiplus_font*>(fn);
	auto style = gdipFont->get_native_format();
	style->SetAlignment(static_cast<StringAlignment>(clipping_h));
	style->SetLineAlignment(static_cast<StringAlignment>(clipping_v));
	int strFormat = 0;
	switch(format)
	{
	case string_format::direction_right_to_left:
		strFormat = StringFormatFlagsDirectionRightToLeft;
	}
	style->SetFormatFlags(strFormat);
	g->DrawString(str.c_str(), static_cast<int>(str.length()), gdipFont->get_native_font(), Gdiplus::RectF(cliprc.position.x, cliprc.position.y, cliprc.sizef.width, cliprc.sizef.height), style, get_brush(b));
}

void gdiplus_renderer::draw_texture(texture* image, const point& p, unsigned char opacity)
{
	gdiplus_texture* text = dynamic_cast<gdiplus_texture*>(image);
	ColorMatrix mt = {1, 0, 0, 0, 0,
                      0, 1, 0, 0, 0,
                      0, 0, 1, 0, 0,
                      0, 0, 0, opacity/255.f, 0,
                      0, 0, 0, 0, 1};
	ImageAttributes ia;
	ia.SetColorMatrix(&mt);
	g->DrawImage(dynamic_cast<gdiplus_texture*>(image)->get_native_member(), Gdiplus::RectF(p.x, p.y, static_cast<float>(text->get_width()), static_cast<float>(text->get_height())), 0.f, 0.f, static_cast<float>(text->get_width()), static_cast<float>(text->get_height()), 
		UnitPixel, &ia); 
}

void gdiplus_renderer::draw_texture(texture* image, const point& p, const size& sz, unsigned char opacity)
{
	gdiplus_texture* text = dynamic_cast<gdiplus_texture*>(image);
	ColorMatrix mt = {1, 0, 0, 0, 0,
                      0, 1, 0, 0, 0,
                      0, 0, 1, 0, 0,
                      0, 0, 0, opacity/255.f, 0,
                      0, 0, 0, 0, 1};
	ImageAttributes ia;
	ia.SetColorMatrix(&mt);
	g->DrawImage(dynamic_cast<gdiplus_texture*>(image)->get_native_member(), Gdiplus::RectF(p.x, p.y, sz.width, sz.height), 0.f, 0.f, static_cast<float>(text->get_width()), static_cast<float>(text->get_height()), 
		UnitPixel, &ia); 
}

void gdiplus_renderer::draw_texture(texture* image, const rect& rc, unsigned char opacity)
{
	draw_texture(image, rc.position, rc.sizef, opacity);
}

void gdiplus_renderer::draw_texture(texture* image, const rect& src, const point& dst, unsigned char opacity)
{
	draw_texture(image, src, rect(dst, src.sizef), opacity);
}

void gdiplus_renderer::draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest, unsigned char opacity)
{
	draw_texture(image, src, rect(pdest, szdest), opacity);
}

void gdiplus_renderer::draw_texture(texture* image, const rect& src, const rect& dest, unsigned char opacity)
{
	ColorMatrix mt = {1, 0, 0, 0, 0,
                      0, 1, 0, 0, 0,
                      0, 0, 1, 0, 0,
                      0, 0, 0, opacity/255.f, 0,
                      0, 0, 0, 0, 1};
	ImageAttributes ia;
	ia.SetColorMatrix(&mt);
	g->DrawImage(dynamic_cast<gdiplus_texture*>(image)->get_native_member(), Gdiplus::RectF(dest.position.x, dest.position.y, dest.sizef.width, dest.sizef.height), src.position.x, src.position.y, src.sizef.width, src.sizef.height, UnitPixel, &ia); 
}

void gdiplus_renderer::draw_icon(icon* ico, const point& pos)
{
	HDC dc = g->GetHDC();
	size sz = ico->get_size();
	DrawIconEx(dc, static_cast<int>(pos.x), static_cast<int>(pos.y), ico->get_icon(), static_cast<int>(sz.width), static_cast<int>(sz.height), 0, 0, DI_NORMAL);
	g->ReleaseHDC(dc);
}

void gdiplus_renderer::draw_icon(icon* ico, const point& pos, const size& sz)
{
	HDC dc = g->GetHDC();
	DrawIconEx(dc, static_cast<int>(pos.x), static_cast<int>(pos.y), ico->get_icon(), static_cast<int>(sz.width), static_cast<int>(sz.height), 0, 0, DI_NORMAL);
	g->ReleaseHDC(dc);
}

//*****************************************************************************************************************************************************************************************************************************************

Matrix* gdiplus_renderer::get_matrix(const matrix& m)
{
	return Matrix(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32).Clone();
}

void gdiplus_renderer::rotate(float angle)
{
	Matrix* m = get_matrix(matrix::rotation(angle));
	g->SetTransform(m);
	delete m;
}

void gdiplus_renderer::rotate_at(float angle, const point& p)
{
	Matrix* m = get_matrix(matrix::rotation(angle, p));
	g->SetTransform(m);
	delete m;
}

void gdiplus_renderer::translate(float x, float y)
{
	Matrix* m = get_matrix(matrix::translation(x, y));
	g->SetTransform(m);
	delete m;
}

void gdiplus_renderer::scale(float fac_x, float fac_y)
{
	Matrix* m = get_matrix(matrix::scalation(fac_x, fac_y));
	g->SetTransform(m);
	delete m;
}

void gdiplus_renderer::shear(float fac_x, float fac_y)
{
	Matrix* m = get_matrix(matrix::shearing(fac_x, fac_y));
	g->SetTransform(m);
	delete m;
}

void gdiplus_renderer::skew(float x, float y, float anglex, float angley)
{
	Matrix* m = get_matrix(matrix::skewing(x, y, anglex, angley));
	g->SetTransform(m);
	delete m;
}

matrix gdiplus_renderer::get_transform() const
{
	Matrix m;
	if(GDI_FAIL(g->GetTransform(&m))) throw runtime_error("Unable to get Matrix");
	float f[6] = {};
	if(GDI_FAIL(m.GetElements(f))) throw runtime_error("Unable to get Elements");
	return matrix(f[0], f[1], f[2], f[3], f[4], f[5]);
}

void gdiplus_renderer::set_transform(const matrix& m_)
{
	Matrix* m = get_matrix(m_);
	g->SetTransform(m);
	delete m;
}

colour gdiplus_renderer::gdicolor_to_colour(const Color& c)
{
	return colour(c.GetValue());
}

size gdiplus_renderer::get_surface_size() const
{
	RECT rc = {};
	GetWindowRect(handle, &rc);
	return size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
}

void gdiplus_renderer::set_text_rendering_mode(const text_rendering_modes& mode)
{
	textMode = mode;
	if(!g) return;
	switch(mode)
	{
	case text_rendering_modes::antialias:
		g->SetTextRenderingHint(TextRenderingHintAntiAlias);
		break;
	case text_rendering_modes::cleartype:
		g->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
		break;
	case text_rendering_modes::system_default:
		g->SetTextRenderingHint(TextRenderingHintSystemDefault);
	}
}
// Graphics

//Geo
gdiplus_geometry::gdiplus_geometry()
{
	path = 0;
}

gdiplus_geometry::~gdiplus_geometry() 
{
	if(path)
		delete path;
}

void gdiplus_geometry::begin_geometry(const point& p)
{
	path = new GraphicsPath();
	path->StartFigure();
	path->AddLine(p.x, p.y, p.x, p.y);
}

void gdiplus_geometry::end_geometry()
{
	path->CloseAllFigures();
}

bool gdiplus_geometry::contains(const point& p, const matrix& m)
{
	matrix m1 = m;
	point p1 = p;
	m1.invert();
	m1.transform_points(&p1);
	return path->IsVisible(PointF(p1.x, p1.y)) != 0;
}

bool gdiplus_geometry::outline_contains(const point& p, pen* pe, const matrix& m)
{
	matrix m1 = m;
	point p1 = p;
	m1.invert();
	m1.transform_points(&p1);
	return path->IsOutlineVisible(PointF(p1.x, p1.y), dynamic_cast<gdiplus_pen*>(pe)->get_native_member()) != 0;
}

void gdiplus_geometry::add_bezier(const point& p1, const point& p2, const point& p3)
{
	PointF f;
	path->GetLastPoint(&f);
	path->AddBezier(f, PointF(p1.x, p1.y), PointF(p2.x, p2.y), PointF(p3.x, p3.y));
}

void gdiplus_geometry::add_beziers(const point* ps, int count)
{
	PointF* points = new PointF[count];
	for(int i = 0; i < count; i++)
		points[i] = PointF(ps[i].x, ps[i].y);
	path->AddBeziers(points, count);
}

void gdiplus_geometry::add_line(const point& p1)
{
	PointF f;
	path->GetLastPoint(&f);
	path->AddLine(f, PointF(p1.x, p1.y));
}

void gdiplus_geometry::add_lines(const point* ps, int count)
{
	PointF* points = new PointF[count];
	for(int i = 0; i < count; i++)
		points[i] = PointF(ps[i].x, ps[i].y);
	path->AddLines(points, count);
}

void gdiplus_geometry::add_rect(const rect& rc)
{
	PointF p;
	path->GetLastPoint(&p);
	path->AddRectangle(Gdiplus::RectF(rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height));
	path->AddLine(p.X, p.Y, p.X, p.Y);
}

void gdiplus_geometry::add_ellipse(const ellipse& e)
{
	rect rc = e.get_rect();
	path->AddEllipse(rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height);
}

void gdiplus_geometry::add_rounded_rect(const rect& rc, float radiusX, float radiusY)
{
	if(radiusX > 45)  radiusX = 45;
	if(radiusY > 45) radiusY = 45;
	radiusX *= 2;
	radiusY *= 2;
	GraphicsPath p;
	p.StartFigure();
	p.AddArc(rc.position.x, rc.position.y, radiusX, radiusY, 180, 90);
	p.AddArc(rc.position.x + rc.sizef.width - radiusX, rc.position.y, radiusX, radiusY, 270, 90);
	p.AddArc(rc.position.x + rc.sizef.width - radiusX, rc.position.y + rc.sizef.height - radiusY, radiusX, radiusY, 0, 90);
	p.AddArc(rc.position.x, rc.position.y + rc.sizef.height - radiusY, radiusX, radiusY, 90, 90);
	p.CloseAllFigures();
	path->AddPath(&p, FALSE);

}

GraphicsPath* gdiplus_geometry::get_native_member()
{
	return path;
}

void gdiplus_geometry::add_geometry(geometry* geo)
{
	path->AddPath(dynamic_cast<gdiplus_geometry*>(geo)->get_native_member(), true);
}

void gdiplus_geometry::add_polygon(const point* ps, int count)
{
	if(count < 3)
		throw logic_error("Count must be at least 3");
	PointF* pfs = new PointF[count];
	for(int i = 0; i < count; i++)
		pfs[i] = PointF(ps[i].x, ps[i].y);
	path->AddPolygon(pfs, count);
	delete[] pfs;
}

rect gdiplus_geometry::get_bounds(const matrix& tr) const
{
	Gdiplus::RectF rc;
	path->GetBounds(&rc, &Matrix(tr.m11, tr.m12, tr.m21, tr.m22, tr.m31, tr.m32));
	return rect(rc.X, rc.Y, rc.Width, rc.Height);
}
// Geo

};
};