#include "stdafx.h"
#include "renderer_gdip.h"

namespace gcl { namespace std_renderer {

gdiplus_renderer::gdiplus_renderer(HWND handle_, callback<void(const size&)>& cb) : tokens(0), context(0), g(0), buffered_context(0), buffered_surface(0), raster_operations(ternary_raster_operations::source_copy), textMode(text_rendering_modes::system_default)
{
	handle = handle_;
	type = graphics_type::handle;
	create_resources();
}

gdiplus_renderer::gdiplus_renderer(HDC dc) : tokens(0), context(dc), g(0), buffered_context(0), buffered_surface(0), raster_operations(ternary_raster_operations::source_copy), textMode(text_rendering_modes::system_default)
{
	handle = 0;
	type = graphics_type::dc;
	create_resources();
}

void gdiplus_renderer::create_resources()
{
	if(GdiplusStartup(&tokens, &startinput, NULL) != Ok)
		throw invalid_argument("Could not initialize GDI+ renderer");
}

gdiplus_renderer::~gdiplus_renderer()
{
	if(type == graphics_type::dc)
	{
		DeleteDC(context);
		context = 0;
	}
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
	if(antialias)
		g->SetSmoothingMode(SmoothingModeAntiAlias);
	else
		g->SetSmoothingMode(SmoothingModeNone);
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
}

void gdiplus_renderer::set_antialias(bool val)
{
	antialias = val;
	if(!g) return;
	if(antialias)
		g->SetSmoothingMode(SmoothingModeAntiAlias);
	else
		g->SetSmoothingMode(SmoothingModeNone);
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

font* gdiplus_renderer::create_font(const wstring& family_name, float size, int fstyle)
{
	return new gdiplus_font(family_name, size, fstyle);
}

geometry* gdiplus_renderer::create_geometry()
{
	return new gdiplus_geometry();
}

graphics* gdiplus_renderer::create_graphics(HWND handle, callback<void(const size&)>& cb)
{
	return new gdiplus_renderer(handle, cb);
}

graphics* gdiplus_renderer::create_graphics(HDC dc)
{
	return new gdiplus_renderer(dc);
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

void gdiplus_renderer::draw_texture(texture* image, const point& p)
{
	gdiplus_texture* text = dynamic_cast<gdiplus_texture*>(image);
	g->DrawImage(text->get_native_member(), p.x, p.y);
}

void gdiplus_renderer::draw_texture(texture* image, const point& p, const size& sz)
{
	g->DrawImage(dynamic_cast<gdiplus_texture*>(image)->get_native_member(), p.x, p.y, sz.width, sz.height);
}

void gdiplus_renderer::draw_texture(texture* image, const rect& rc)
{
	draw_texture(image, rc.position, rc.sizef);
}

void gdiplus_renderer::draw_texture(texture* image, const rect& src, const point& dst)
{
	draw_texture(image, src, rect(dst, src.sizef));
}

void gdiplus_renderer::draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest)
{
	draw_texture(image, src, rect(pdest, szdest));
}

void gdiplus_renderer::draw_texture(texture* image, const rect& src, const rect& dest)
{
	g->DrawImage(dynamic_cast<gdiplus_texture*>(image)->get_native_member(), Gdiplus::RectF(dest.position.x, dest.position.y, dest.sizef.width, dest.sizef.height), src.position.x, src.position.y, src.sizef.width, src.sizef.height, UnitPixel); 
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
	Matrix* m = get_matrix(matrix::rotation_at(angle, p));
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
	float f[5] = {};
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





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

bool gdiplus_geometry::contains(const point& p)
{
	return path->IsVisible(p.x, p.y) != 0;
}

bool gdiplus_geometry::outline_contains(const point& p, pen* pe)
{
	return path->IsOutlineVisible(p.x, p.y, dynamic_cast<gdiplus_pen*>(pe)->get_native_member()) != 0;
}

void gdiplus_geometry::transform(const matrix& m)
{
	Matrix m1(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32);
	path->Transform(&m1);
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
	path->AddRectangle(Gdiplus::RectF(rc.position.x, rc.position.y, rc.sizef.height, rc.sizef.width));
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


};
};