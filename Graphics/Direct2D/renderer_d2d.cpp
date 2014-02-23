#include "stdafx.h"
#include "renderer_d2d.h"

namespace gcl { namespace std_renderer {

void direct2d_renderer::target_resizes(const size& sz)
{
	if(!render_target) return;
		render_target->Resize(SizeU(static_cast<int>(sz.width), static_cast<int>(sz.height)));
}

direct2d_renderer::direct2d_renderer(HWND handle_, callback<void(const size&)>& cb) : render_target(0), factory(0), imgfactory(0), write_factory(0), dc(0)
{
	handle = handle_;
	cb += make_func_ptr(this, &direct2d_renderer::target_resizes);
	create_resources();
}

void direct2d_renderer::create_resources()
{
	if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory))) throw runtime_error("Couldn't initialise Direct2D");
	RECT rc;
	GetClientRect(handle, &rc);
	if(FAILED(factory->CreateHwndRenderTarget(RenderTargetProperties(), HwndRenderTargetProperties(handle, SizeU(rc.right-rc.left, rc.bottom-rc.top)), &render_target))) throw runtime_error("Couldn't initialise Direct2D");
	if(FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)(IID_PPV_ARGS(&imgfactory))))) throw runtime_error("Unable to create ImagingFactory");
	if(FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&write_factory)))) throw runtime_error("Unable to create DirectWriteFactory");
}

void direct2d_renderer::release_resources() 
{
	write_factory->Release();
	write_factory = 0;
	imgfactory->Release();
	imgfactory = 0;
	render_target->Release();
	render_target = 0;
	factory->Release();
	factory = 0;
}

void direct2d_renderer::begin()
{
	if(!render_target) return;
	dc = BeginPaint(handle, &ps);
	render_target->BeginDraw();
	render_target->SetTransform(Matrix3x2F::Identity());
}

void direct2d_renderer::clear(const colour& c)
{
	if(!render_target) return;
	render_target->Clear(ColorF(c.r/255.f, c.b/255.f, c.b/255.f, c.a/255.f));
}

void direct2d_renderer::end()
{
	if(FAILED(render_target->EndDraw())) throw runtime_error("Fatal error while calling \"EndDraw()\"");
	EndPaint(handle, &ps);
	dc = 0;
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
	default:
		throw invalid_argument("Unknown Brushtype");
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

//******************************************************************************************************************************************************************************************************************************************************

void direct2d_renderer::fill_rect(const rect& rc, brush* b)
{
	render_target->FillRectangle(get_rect(rc), get_brush(b));
}

void direct2d_renderer::fill_rects(const rect* rcs, int count, brush* b)
{
	ID2D1Brush* br = get_brush(b);
	for(int i = 0; i < count; i++)
		render_target->FillRectangle(get_rect(rcs[i]), br);
}

void direct2d_renderer::fill_ellipse(const ellipse& e, brush* b)
{
	render_target->FillEllipse(Ellipse(Point2F(e.position.x, e.position.y), e.radius_x, e.radius_y), get_brush(b));
}

void direct2d_renderer::fill_rounded_rect(const rect& rc, float radiusx, float radiusy, brush* b)
{
	render_target->FillRoundedRectangle(RoundedRect(RectF(rc.position.x, rc.position.y, rc.sizef.width + rc.position.x, rc.sizef.height + rc.position.y), radiusx, radiusy), get_brush(b)); 
}

void direct2d_renderer::fill_geometry(geometry* geo, brush* b)
{
	ID2D1Geometry* geo_ = dynamic_cast<d2d_geometry*>(geo)->get_geometry();
	render_target->FillGeometry(geo_, get_brush(b));
}

//*************************************************************************************************************************************************************************************************************************************************

void direct2d_renderer::draw_ellipse(const ellipse& e, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	render_target->DrawEllipse(Ellipse(get_point(e.position), e.radius_x, e.radius_y), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_line(const point& p1, const point& p2, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	render_target->DrawLine(get_point(p1), get_point(p2), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_lines(const point* ps, int count, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	for (int i = 0; i < count-1; i++)
		render_target->DrawLine(get_point(ps[i]), get_point(ps[i++]), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_rect(const rect rc, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	render_target->DrawRectangle(get_rect(rc), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_rects(const rect* rcs, int count, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	for(int i = 0; i < count; i++)
		render_target->DrawRectangle(get_rect(rcs[i]), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_rounded_rect(const rect& rc, float radiusx, float radiusy, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	render_target->DrawRoundedRectangle(RoundedRect(get_rect(rc), radiusx, radiusy), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}

void direct2d_renderer::draw_string(const wstring& str, const point& origin, font* fn, brush* b, const string_format& format)
{
	float fmax = (numeric_limits<float>::max)();
	IDWriteTextFormat* format_ = dynamic_cast<d2d_font*>(fn)->get_native_member();
	if((int)format == (int)string_format::direction_left_to_right)
	{
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT))) throw runtime_error("Unable to set ReadingDirection");
	}
	else
	{
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT))) throw runtime_error("Unable to set ReadingDirection");
	}
	if(FAILED(format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)) ||
	FAILED(format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING))) throw runtime_error("Unable to set Alignments");
	render_target->DrawTextW(str.c_str(), static_cast<UINT32>(str.length()), format_, &RectF(origin.x, origin.y, fmax, fmax), get_brush(b), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void direct2d_renderer::draw_string(const wstring& str, const rect& cliprc, font* fn, brush* b, const string_format& format, const horizontal_string_align& clipping_h, const vertical_string_align& clipping_v)
{
	IDWriteTextFormat* format_ = dynamic_cast<d2d_font*>(fn)->get_native_member();
	if(format == string_format::direction_left_to_right)
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT))) throw runtime_error("Unable to set ReadingDirection");
	else
		if(FAILED(format_->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT))) throw runtime_error("Unable to set ReadingDirection");
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
	render_target->DrawTextW(str.c_str(), static_cast<UINT32>(str.length()), format_, &get_rect(cliprc), get_brush(b), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void direct2d_renderer::draw_texture(texture* image, const point& p)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	render_target->DrawBitmap(bmp->get_native_member(), get_rect(rect(p.x, p.y, static_cast<float>(160), static_cast<float>(bmp->get_height()))));
}

void direct2d_renderer::draw_texture(texture* image, const point& p, const size& sz)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	render_target->DrawBitmap(bmp->get_native_member(), get_rect(rect(p.x, p.y, sz.width, sz.height)));
}

void direct2d_renderer::draw_texture(texture* image, const rect& rc)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	render_target->DrawBitmap(bmp->get_native_member(), get_rect(rect(rc.position.x, rc.position.y, rc.sizef.width, rc.sizef.height)));
}

void direct2d_renderer::draw_texture(texture* image, const rect& src, const point& dst)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	render_target->DrawBitmap(bmp->get_native_member(), get_rect(rect(dst.x, dst.y, src.sizef.width, src.sizef.height)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		get_rect(rect(src.position.x, src.position.y, src.sizef.width, src.sizef.height)));
}

void direct2d_renderer::draw_texture(texture* image, const rect& src, const point& pdest, const size& szdest)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	render_target->DrawBitmap(bmp->get_native_member(), get_rect(rect(pdest.x, pdest.y, szdest.width, szdest.height)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		get_rect(rect(src.position.x, src.position.y, src.sizef.width, src.sizef.height)));
}

void direct2d_renderer::draw_texture(texture* image, const rect& src, const rect& dest)
{
	d2d_texture* bmp = dynamic_cast<d2d_texture*>(image);
	render_target->DrawBitmap(bmp->get_native_member(), get_rect(rect(dest.position.x, dest.position.y, dest.sizef.width, dest.sizef.height)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		get_rect(rect(src.position.x, src.position.y, src.sizef.width, src.sizef.height)));
}

void direct2d_renderer::draw_geometry(geometry* geo, pen* p)
{
	d2d_pen* p_ = dynamic_cast<d2d_pen*>(p);
	render_target->DrawGeometry(dynamic_cast<d2d_geometry*>(geo)->get_geometry(), p_->get_native_member(), p_->get_width(), p_->get_stroke_style());
}


solid_brush* direct2d_renderer::create_solid_brush(const colour& c)
{
	return new d2d_solid_brush(render_target, D2D1::ColorF(c.r/255.f, c.g/255.f, c.b/255.f, c.a / 255.0f));
}

texture_brush* direct2d_renderer::create_texture_brush(texture* image)
{
	return new d2d_texture_brush(image, render_target);
}

linear_gradient_brush* direct2d_renderer::create_linear_gradient_brush(const point& start, const point& end, gradient_stop& gradients, bool gamma, const wrap_modes& wrapmode)
{
	return new d2d_linear_gradient_brush(start, end, gradients, render_target, gamma, wrapmode);
}

pen* direct2d_renderer::create_pen(brush* b, float width, pen_align align, dash_cap startcap, dash_cap endcap, dash_cap dash_cap, dash_style dashstyle, float offset)
{
	return new d2d_pen(b, get_brush(b), factory, width, align, startcap, endcap, dash_cap, dashstyle, offset);
}

texture* direct2d_renderer::create_texture(const wstring& filename)
{
	return new d2d_texture(filename, imgfactory, render_target);
}

texture* direct2d_renderer::create_texture(int id, LPWSTR type, HINSTANCE inst)
{
	return new d2d_texture(id, inst, type, imgfactory, render_target);
}

font* direct2d_renderer::create_font(const wstring& family_name, float size, int fstyle)
{
	return new d2d_font(family_name, size, fstyle, factory, write_factory);
}

geometry* direct2d_renderer::create_geometry()
{
	return new d2d_geometry(factory);
}
//********************************************************************************************************************************************************************************************************************************

Matrix3x2F direct2d_renderer::get_matrix(const matrix& m)
{
	return Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32);
}

void direct2d_renderer::rotate(float angle)
{
	render_target->SetTransform(get_matrix(matrix::rotation(angle)));
}

void direct2d_renderer::rotate_at(float angle, const point& p)
{
	render_target->SetTransform(get_matrix(matrix::rotation_at(angle, p)));
}

void direct2d_renderer::translate(float x, float y)
{
	render_target->SetTransform(get_matrix(matrix::translation(x, y)));
}

void direct2d_renderer::scale(float fac_x, float fac_y)
{
	render_target->SetTransform(get_matrix(matrix::scalation(fac_x, fac_y)));
}

void direct2d_renderer::shear(float fac_x, float fac_y)
{
	render_target->SetTransform(get_matrix(matrix::shearing(fac_x, fac_y)));
}

void direct2d_renderer::skew(float x, float y, float anglex, float angley)
{
	render_target->SetTransform(get_matrix(matrix::skewing(x, y, anglex, angley)));
}

matrix direct2d_renderer::get_transform() const
{
	Matrix3x2F m;
	render_target->GetTransform(&m);
	return matrix(m._11, m._12, m._21, m._22, m._31, m._32);
};

void direct2d_renderer::set_transform(const matrix& m)
{
	render_target->SetTransform(get_matrix(m));
}

void direct2d_renderer::set_text_rendering_mode(const text_rendering_modes& mode)
{
	textMode = mode;
	switch(mode)
	{
	case text_rendering_modes::antialias:
		render_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
		break;
	case text_rendering_modes::cleartype:
		render_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
		break;
	case text_rendering_modes::system_default:
		render_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
	}
}

text_rendering_modes direct2d_renderer::get_text_rendering_mode() const {return textMode;}

void direct2d_renderer::set_antialias(bool val)
{
	antialias = val;
	if(!render_target) return;
	if(!val)
		render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	else
		render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

d2d_geometry::d2d_geometry(ID2D1Factory* trgt)
{
	factory = trgt;
	geo_trans = 0;
	geo_sink = 0;
}

void d2d_geometry::begin_geometry(const point& p)
{
	if(geo)geo->Release();
	geo = 0;
	if(geo_sink)geo_sink->Release();
	geo_sink = 0;
	if(geo_trans)geo_trans->Release();
	geo_trans = 0;
	if(FAILED(factory->CreatePathGeometry(&geo))) throw runtime_error("Unable to create PathGeometry");
	if(FAILED(geo->Open(&geo_sink))) throw runtime_error("Unable to open Sink");
	lastPoint = p;
	geo_sink->BeginFigure(Point2F(p.x, p.y), D2D1_FIGURE_BEGIN_FILLED);
}

void d2d_geometry::end_geometry()
{
	geo_sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	if(FAILED(geo_sink->Close())) throw runtime_error("Unable to close Sink");
	if(FAILED(factory->CreateTransformedGeometry(geo, Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), &geo_trans))) throw runtime_error("Unable to create TransformedGeometry");
}

bool d2d_geometry::contains(const point& p)
{
	BOOL b  = 0;
	if(FAILED(geo->FillContainsPoint(Point2F(p.x, p.y), &Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), &b))) throw invalid_argument("Invalid comparison");
	return (b != 0);
}

bool d2d_geometry::outline_contains(const point& p, pen* pe)
{
	BOOL b  = 0;
	if((geo->StrokeContainsPoint(Point2F(p.x, p.y), pe->get_width(), dynamic_cast<d2d_pen*>(pe)->get_stroke_style(), &Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), &b))) throw invalid_argument("Invalid comparison");
	return (b != 0);
}

void d2d_geometry::release_resources()
{
	if(geo)geo->Release();
	geo = 0;
	if(geo_sink)geo_sink->Release();
	geo_sink = 0;
	if(geo_trans)geo_trans->Release();
	geo_trans = 0;
}

void d2d_geometry::transform(const matrix& m1)
{
	m = m1;
	if(geo_trans)
	{
		if(FAILED(factory->CreateTransformedGeometry(geo, Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), &geo_trans))) throw runtime_error("Unable to create TransformedGeometry");
	}
}

ID2D1Geometry* d2d_geometry::get_geometry()
{
	return geo_trans;
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
	if(FAILED(geo->CombineWithGeometry(rectgeo, D2D1_COMBINE_MODE_UNION, Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), newSink))) throw runtime_error("Unable to add Geometry");
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
	if(FAILED(geo->CombineWithGeometry(rectgeo, D2D1_COMBINE_MODE_UNION, Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), newSink))) throw runtime_error("Unable to add Geometry");
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
	if(FAILED(geo->CombineWithGeometry(rectgeo, D2D1_COMBINE_MODE_UNION, Matrix3x2F(m.m11, m.m12, m.m21, m.m22, m.m31, m.m32), newSink))) throw runtime_error("Unable to add Geometry");
	rectgeo->Release();
	geo_sink->Release();
	geo->Release();
	geo = newGeo;
	geo_sink = newSink;
	geo_sink->BeginFigure(Point2F(lastPoint.x, lastPoint.y), D2D1_FIGURE_BEGIN_FILLED);
}

};
};