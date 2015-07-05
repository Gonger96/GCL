#include "stdafx.h"
#include "graphics.h"

namespace gcl {

wstring gcl_create_classname(const wstring& name)
{
	wstringstream ss;
	time_t t;
	time(&t);
	ss << name << to_wstring(t) << rand() << L'\0';
	return ss.str();
}

// Point
point& point::operator= (const point& p) 
{
	x = p.x; 
	y = p.y; 
	return (*this);
}

wstring point::to_wstring() const 
{
	return L"{" + std::to_wstring(x) + L", " + std::to_wstring(y) + L"}";
}

bool point::operator==(const point& p) const
{
	return p.x == x && p.y == y;
}

bool point::operator!=(const point& p) const
{
	return p.x != x || p.y != y;
}
// Point

// Size
const size size::max_size() 
{
	return size(numeric_limits<float>::max(), numeric_limits<float>::max());
}

point size::to_point() const 
{
	return point(width, height);
}

size& size::operator= (const size& sz) 
{
	width = sz.width; 
	height = sz.height; 
	return (*this);
}

wstring size::to_wstring() const
{
	return wstring(L"{") + std::to_wstring(width) + L", " + std::to_wstring(height) + L"}";
}

bool size::operator==(const size& sz) const
{
	return sz.height == height && sz.width == width;
}

bool size::operator!=(const size& sz) const
{
	return sz.height != height || sz.width != width;
}

size::size(const ui_metrics& m)
{
	width = static_cast<float>(GetSystemMetrics(static_cast<int>(m)));
	height = static_cast<float>(GetSystemMetrics(static_cast<int>(m)+1));
}
// Size

// Rectangle
void rect::inflate(float _width, float _height) 
{
	position.x -= _width; 
	position.y -= _height; 
	sizef.height += _height*2; 
	sizef.width += _width*2;
}

void rect::inflate(const size& sz) 
{
	inflate(sz.width, sz.height);
}

bool rect::contains(const point& p) const 
{
	return ((position.x <= p.x) && (p.x < (position.x + sizef.width)) && (position.y <= p.y) && (p.y < (position.y + sizef.height)));
}

bool rect::contains(const point& p, const matrix& m) const
{
	matrix mc = m;
	point pc = p;
	mc.invert();
	mc.transform_points(&pc);
	return contains(pc);
}

rect rect::get_bounds(const matrix& m) const
{
	if(m.is_identity())
		return *this;
	point ps[] =  {point(position), point(get_x() + get_width(), position.y), point(get_x(), get_y()+get_height()), point(get_x()+get_width(), get_y()+get_height())};
	m.transform_points(ps, 4);
	rect r;
	r.position.x = gcl_min(ps[0].x, ps[1].x, ps[2].x, ps[3].x);
	r.position.y = gcl_min(ps[0].y, ps[1].y, ps[2].y, ps[3].y);
	r.sizef.width = gcl_max(ps[0].x, ps[1].x, ps[2].x, ps[3].x) - r.get_x();
	r.sizef.height = gcl_max(ps[0].y, ps[1].y, ps[2].y, ps[3].y) - r.get_y();
	return r;
}

rect& rect::operator=(const rect& sz) 
{
	position = sz.position; 
	sizef = sz.sizef; 
	return (*this);
}

bool rect::operator==(const rect& rc) const 
{
	return position == rc.position && sizef == rc.sizef;
}

bool rect::operator!=(const rect& rc) const 
{
	return position != rc.position || sizef != rc.sizef;
}

wstring rect::to_wstring() const
{
	return wstring(L"{") + position.to_wstring() + L", " + sizef.to_wstring() + L"}";
}
// Rectangle

// Ellipse
rect ellipse::get_rect() const 
{
	return rect(position.x - radius_x, position.y - radius_y, 2 * radius_x, 2 * radius_y);
}

ellipse& ellipse::operator=(const ellipse& sz) 
{
	position = sz.position; 
	radius_x = sz.radius_x; 
	radius_y = sz.radius_y; 
	return (*this);
}

bool ellipse::operator==(const ellipse& e) const 
{
	return (position == e.position && radius_x == e.radius_x && radius_y == e.radius_y);
}
// Ellipse

// Colour
colour::uint colour::get_value() const 
{
	return ((a << 24) | (r << 16) | (g << 8) | (b));
}

colour::colour(system_colour c) 
{
	DWORD rgb = GetSysColor(static_cast<int>(c));
	a = 255;
	r = GetRValue(rgb);
	g = GetGValue(rgb);
	b = GetBValue(rgb);
}

colour::colour(uchar _a, system_colour c)
{
	DWORD rgb = GetSysColor(static_cast<int>(c));
	a = _a;
	r = GetRValue(rgb);
	g = GetGValue(rgb);
	b = GetBValue(rgb);
}

colour& colour::operator= (const colour& c) 
{
	a = c.a;
	r = c.r; 
	g = c.g; 
	b = c.b; 
	return (*this);
}

colour colour::opacity(float fac) const
{
	return colour(r, g, b, static_cast<uchar>(a*fac));
}

colour colour::invert(bool alpha) 
{
	return colour(~r, ~g, ~b, alpha ? ~a : a);
}

bool colour::operator==(const colour& c) 
{
	return c.a == a && c.r == r && c.g == g && c.b == b;
}

void colour::differ(uchar value)
{
	r += value;
	g += value;
	b += value;
}

bool colour::operator==(const colour& c) const
{
	return (c.a == a || c.r == r || c.g == g || c.b == b);
}

bool colour::operator!=(const colour& c) const
{
	return (c.a != a || c.r != r || c.g != g || c.b != b);
}
// Colour

// Matrix
matrix matrix::identity() 
{
	return matrix(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
}

bool matrix::is_identity(const matrix& m) 
{
	return (m == identity());
}

bool matrix::is_identity() const 
{
	return (*this == identity());
}

void matrix::reset() 
{
	*this = matrix::identity();
}

bool matrix::operator==(const matrix& m) const 
{
	return (m11 == m.m11 && m12 == m.m12 && m21 == m.m21 && m22 == m.m22 && m31 == m.m31 && m32 == m.m32);
}

bool matrix::operator!=(const matrix& m) const
{
	return (m11 != m.m11 || m12 != m.m12 || m21 != m.m21 || m22 != m.m22 || m31 != m.m31 || m32 != m.m32);
}

matrix matrix::operator*(const matrix& m)
{
	matrix m_;
	float m11_ = m11*m.m11 + m12*m.m21; // + 0*m.m31;
	float m12_ = m11*m.m12 + m12*m.m22; // + 0*m.m32;
	float m21_ = m21*m.m11 + m22*m.m21; // + 0*m.m31;
	float m22_ = m21*m.m12 + m22*m.m22; // + 0*m.m32 
	float m31_ = m31*m.m11 + m32*m.m21 + m.m31;			//  1 0 /0
	float m32_ = m31*m.m12 + m32*m.m22 + m.m32;			//  0 1 /0
	m_.m11 = m11_;										//  0 0 /1
	m_.m12 = m12_;
	m_.m21 = m21_;
	m_.m22 = m22_;
	m_.m31 = m31_;
	m_.m32 = m32_;
	return m_;
}

matrix matrix::operator*(float scalar)
{
	return matrix(m11*scalar, m12*scalar, m21*scalar, m22*scalar, m31*scalar, m32*scalar);
}

matrix matrix::operator/(const matrix& m)
{
	if(m.get_determinant() == 0)
		throw logic_error("Matrix not invertible");
	matrix ms = ~m;
	return *(this)*ms;
}

matrix matrix::operator/(float scalar)
{
	return matrix(m11/scalar, m12/scalar, m21/scalar, m22/scalar, m31/scalar, m32/scalar);
}

matrix matrix::operator~() const
{
	matrix m1 = *this;
	m1.invert();
	return m1;
}

void matrix::transform_points(point* pts, unsigned int count) const
{
	for(unsigned int i = 0; i < count; i++, pts++)
	{
		float x = pts->x*m11 + pts->y*m21 + m31;
		float y = pts->x*m12 + pts->y*m22 + m32;
		pts->x = x;
		pts->y = y;
	}
}

void matrix::transform_vectors(point* pts, unsigned int count) const
{
	for(unsigned int i = 0; i < count; i++, pts++)
	{
		float x = pts->x*m11 + pts->y*m21;
		float y = pts->x*m12 + pts->y*m22;
		pts->x = x;
		pts->y = y;
	}
}

void matrix::invert()
{
	if(get_determinant() == 0)
		throw logic_error("Matrix not invertible");
	float det = 1/get_determinant();
	*this = matrix(determinant(m22, 0, m32, 1)*det, determinant(0, m12, 1, m32)*det, determinant(0, m21, 1, m31)*det, determinant(m11, 0, m31, 1)*det, determinant(m21, m22, m31, m32)*det, determinant(m12, m11, m32, m31)*det);
}

float matrix::get_determinant() const
{
	return m11*m22 - m12*m21;
}

matrix matrix::rotation(float angle)
{
	float sinalpha = std::sinf(angle / 180*static_cast<float>(static_cast<float>(M_PI)));
	float cosalpha = std::cosf(angle / 180*static_cast<float>(M_PI));
	return matrix(cosalpha, sinalpha, -sinalpha, cosalpha, 0, 0);
}

void matrix::rotate(float angle)
{
	float sinalpha = std::sinf(angle / 180*static_cast<float>(M_PI));
	float cosalpha = std::cosf(angle / 180*static_cast<float>(M_PI));
	*this = matrix(cosalpha, sinalpha, -sinalpha, cosalpha, 0, 0) * (*this);
}
	
matrix matrix::rotation(float angle, const point& p)
{
	float sinalpha = std::sinf(angle / 180*static_cast<float>(M_PI));
	float cosalpha = std::cosf(angle / 180*static_cast<float>(M_PI));
	return matrix(1, 0, 0, 1, -p.x, -p.y) * matrix(cosalpha, sinalpha, -sinalpha, cosalpha, p.x, p.y);
}

void matrix::rotate(float angle, const point& p)
{
	float sinalpha = std::sinf(angle / 180*static_cast<float>(M_PI));
	float cosalpha = std::cosf(angle / 180*static_cast<float>(M_PI));
	*this = (matrix(1, 0, 0, 1, -p.x, -p.y) * matrix(cosalpha, sinalpha, -sinalpha, cosalpha, p.x, p.y)) * *this;
}

matrix matrix::translation(float x, float y)
{
	return matrix(1, 0, 0, 1, x, y);
}

void matrix::translate(float x, float y)
{
	m31 += x;
	m32 += y;
}

matrix matrix::scalation(float fac_x, float fac_y)
{
	return matrix(fac_x, 0, 0, fac_y, 0, 0);
}

void matrix::scale(float fac_x, float fac_y)
{
	*this = matrix(fac_x, 0, 0, fac_y, 0, 0) * (*this);
}

matrix matrix::shearing(float fac_x, float fac_y)
{
	return matrix(1, fac_x, fac_y, 1, 0, 0);
}

void matrix::shear(float fac_x, float fac_y)
{
	*this = matrix(1, fac_x, fac_y, 1, 0, 0) * (*this);
}

matrix matrix::skewing(float x, float y, float angle_x, float angle_y)
{
	float tan_theta = std::tanf(angle_x / 180*static_cast<float>(M_PI));
	float tan_phi = std::tanf(angle_y / 180*static_cast<float>(M_PI));
	return matrix(1, tan_phi, tan_theta, 1, -y*tan_theta, -x*tan_phi);
}

void matrix::skew(float x, float y, float angle_x, float angle_y)
{
	float tan_theta = std::tanf(angle_x / 180*static_cast<float>(M_PI));
	float tan_phi = std::tanf(angle_y / 180*static_cast<float>(M_PI));
	(*this) = matrix(1, tan_phi, tan_theta, 1, -y*tan_theta, -x*tan_phi) * (*this);
}

matrix matrix::mirroring_x()
{
	return matrix(-1, 0, 0, 1, 0, 0);
}

matrix matrix::mirroring_x(float axis)
{
	return matrix(-1, 0, 0, 1, axis*2.f, 0);
}

void matrix::mirror_x()
{
	*this = matrix(-1, 0, 0, 1, 0, 0)*(*this);
}

void matrix::mirror_x(float axis)
{
	*this = matrix(-1, 0, 0, 1, axis*2.f, 0)*(*this);
}

matrix matrix::mirroring_y()
{
	return matrix(1, 0, 0, -1, 0, 0);
}

matrix matrix::mirroring_y(float axis)
{
	return matrix(1, 0, 0, -1, 0, axis*2.f);
}

void matrix::mirror_y()
{
	*this = matrix(1, 0, 0, -1, 0, 0)*(*this);
}

void matrix::mirror_y(float axis)
{
	*this = matrix(1, 0, 0, -1, 0, axis*2.f)*(*this);
}
// Matrix

// Clipboard
void clipboard::clear()
{
	if(!EmptyClipboard())
		throw runtime_error("unable to clear clipboard");
}

void clipboard::open(HWND wind)
{
	if(!OpenClipboard(wind))
		throw runtime_error("Unable to open clipboard");
}

void clipboard::close()
{
	if(!CloseClipboard())
		throw runtime_error("Unable to close clipboard");
}

HWND clipboard::get_owner()
{
	return GetClipboardOwner();
}

bool clipboard::is_format_available(UINT format)
{
	return !!IsClipboardFormatAvailable(format);
}

UINT clipboard::register_format(const wstring& str)
{
	UINT u = RegisterClipboardFormat(str.c_str());
	if(u == 0)
		throw runtime_error("Unable to register format");
	return u;
}

clipboard::generic_data clipboard::get_data(UINT format)
{
	auto data = GetClipboardData(format);
	if(!data)
		throw runtime_error("Invalid data");
	return data;
}

vector<UINT> clipboard::get_available_formats()
{
	unsigned current = 0;
	vector<UINT> coll;
	while(true)
	{
		current = EnumClipboardFormats(current);
		if(current == 0)
			break;
		coll.push_back(current);
	}
	return move(coll);
}

void clipboard::set_data(UINT format, generic_data data)
{
	if(!SetClipboardData(format, data))
		throw runtime_error("Unable to set clipboard data");
}

IDataObject* clipboard::ole_get_data()
{
	IDataObject* obj;
	if(FAILED(OleGetClipboard(&obj)))
		throw runtime_error("Unable to get clipboard data");
	return obj;
}

void clipboard::ole_flush()
{
	OleFlushClipboard();
}

void clipboard::ole_set_data(IDataObject* data_object)
{
	if(FAILED(OleSetClipboard(data_object)))
		throw runtime_error("Unable to set clipboard data");
}
// Clipboard

// DragDrop
namespace dragdrop {

data_object::data_object(FORMATETC* fmt, STGMEDIUM* med, int cnt)
{
	ref_count = 1;
	format_cnt = cnt;
	formats = new FORMATETC[cnt];
	mediums = new STGMEDIUM[cnt];
	for(int i = 0; i < cnt; i++)
	{
		formats[i] = fmt[i];
		mediums[i] = med[i];
	}
}

data_object::~data_object()
{
	delete[] formats;
	delete[] mediums;
}

HRESULT WINAPI data_object::GetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
	int idx = get_format_index(pFormatEtc);
	if(idx == -1)
		return DV_E_FORMATETC;
	pMedium->tymed = formats[idx].tymed;
	pMedium->pUnkForRelease = 0;
	switch(formats[idx].tymed)
	{
	case TYMED_HGLOBAL:
		{
			void* hMem = mediums[idx].hGlobal;
			DWORD len = GlobalSize(hMem);
			void* source = GlobalLock(hMem);
			void* dest = GlobalAlloc(GMEM_FIXED, len);
			memcpy(dest, source, len);
			GlobalUnlock(hMem);
			pMedium->hGlobal = dest;
			break;
		}
	case TYMED_ENHMF:
		pMedium->hEnhMetaFile = CopyEnhMetaFile(mediums[idx].hEnhMetaFile, NULL);
		break;
	case TYMED_MFPICT:
		pMedium->hMetaFilePict = mediums[idx].hMetaFilePict;
		break;
	case TYMED_GDI:
		pMedium->hBitmap = mediums[idx].hBitmap;
		break;
	case TYMED_FILE:
		{
			void* hMem = mediums[idx].lpszFileName;
			DWORD len = GlobalSize(hMem);
			void* source = GlobalLock(hMem);
			void* dest = GlobalAlloc(GMEM_FIXED, len);
			memcpy(dest, source, len);
			GlobalUnlock(hMem);
			pMedium->lpszFileName = reinterpret_cast<wchar_t*>(dest);
			break;
		}
	case TYMED_ISTORAGE:
		pMedium->pstg = mediums[idx].pstg;
		break;
	case TYMED_ISTREAM:
		pMedium->pstm = mediums[idx].pstm;
		break;
	default:
		return DV_E_FORMATETC;
	}
	return S_OK;
}

HRESULT WINAPI data_object::GetDataHere(FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
{
	return DATA_E_FORMATETC;
}

HRESULT WINAPI data_object::QueryGetData(FORMATETC* pFormatEtc)
{
	return (get_format_index(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}

HRESULT WINAPI data_object::GetCanonicalFormatEtc(FORMATETC* pFormatEct, FORMATETC* pFormatEtcOut)
{
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
}

HRESULT WINAPI data_object::SetData(FORMATETC* pFormatEtc, STGMEDIUM* pMedium, BOOL fRelease)
{
	return E_NOTIMPL;
}

HRESULT WINAPI data_object::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppEnumFormatEtc)
{
	if(dwDirection == DATADIR_GET)
		return SHCreateStdEnumFmtEtc(format_cnt, formats, ppEnumFormatEtc);
	else
		return E_NOTIMPL;
}

HRESULT WINAPI data_object::DAdvise(FORMATETC* pFormatEtc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT WINAPI data_object::DUnadvise(DWORD dwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT WINAPI data_object::EnumDAdvise(IEnumSTATDATA** ppEnumAdvise)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT WINAPI data_object::QueryInterface(REFIID iid, void** ppvObject)
{
	if(iid == IID_IDataObject || iid == IID_IUnknown)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}

ULONG WINAPI data_object::AddRef()
{
	return InterlockedIncrement(&ref_count);
}

ULONG WINAPI data_object::Release()
{
	LONG count = InterlockedDecrement(&ref_count);
	if(count == 0)
	{
		delete this;
		return 0;
	}
	else
		return count;
}

int data_object::get_format_index(FORMATETC* frmt)
{
	for(int i = 0; i < format_cnt; ++i)
	{
		if(formats[i].cfFormat == frmt->cfFormat && formats[i].dwAspect == frmt->dwAspect && (formats[i].tymed & frmt->tymed))
			return i;
	}
	return -1;
}

drop_target::drop_target(drawsurface* target)
{
	if(!target)
		throw invalid_argument("Invalid window");
	wnd = target;
	ref_count = 1;
	drop_helper = 0;
}

drop_target::~drop_target()
{
	if(drop_helper)
		drop_helper->Release();
}

void drop_target::register_dragdrop()
{
	if(!wnd || !wnd->get_handle())
		throw runtime_error("Invalid window");
	if(FAILED(RegisterDragDrop(wnd->get_handle(), this)))
		throw runtime_error("Registering DragDrop failed");
}

void drop_target::deregister_dragdrop()
{
	if(!wnd || !wnd->get_handle())
		throw runtime_error("Invalid window");
	if(FAILED(RevokeDragDrop(wnd->get_handle())))
		throw runtime_error("Unregistering DragDrop failed");
}

void drop_target::register_helper()
{
	if(FAILED(CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (void**)&drop_helper)))
		throw runtime_error("Unable to create DropTargetHelper");
}

void drop_target::deregister_helper()
{
	if(!drop_helper)
		return;
	drop_helper->Release();
	drop_helper = 0;
}

HRESULT WINAPI drop_target::QueryInterface(REFIID iid, void** ppvObject)
{
	if(iid == IID_IDropTarget || iid == IID_IUnknown)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}

ULONG WINAPI drop_target::AddRef()
{
	return InterlockedIncrement(&ref_count);
}

ULONG WINAPI drop_target::Release()
{
	return InterlockedDecrement(&ref_count);
}

HRESULT WINAPI drop_target::DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	drop_effects effect = drop_effects::none;
	wnd->on_drag_enter(pDataObject, grfKeyState, point(static_cast<float>(pt.x), static_cast<float>(pt.y)), &effect);
	*pdwEffect = static_cast<DWORD>(effect);
	if(drop_helper)
	{
		POINT p = {pt.x, pt.y};
		drop_helper->DragEnter(wnd->get_handle(), pDataObject, &p, *pdwEffect);
	}
	return S_OK;
}

HRESULT WINAPI drop_target::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	drop_effects effect = drop_effects::none;
	wnd->on_drag_over(grfKeyState, point(static_cast<float>(pt.x), static_cast<float>(pt.y)), &effect);
	*pdwEffect = static_cast<DWORD>(effect);
	if(drop_helper)
	{
		POINT p = {pt.x, pt.y};
		drop_helper->DragOver(&p, *pdwEffect);
	}
	return S_OK;
}

HRESULT WINAPI drop_target::DragLeave()
{
	wnd->on_drag_leave();
	if(drop_helper)
		drop_helper->DragLeave();
	return S_OK;
}

HRESULT WINAPI drop_target::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	drop_effects effect = drop_effects::none;
	wnd->on_drop(pDataObject, grfKeyState, point(static_cast<float>(pt.x), static_cast<float>(pt.y)), &effect);
	*pdwEffect = static_cast<DWORD>(effect);
	if(drop_helper)
	{
		POINT p = {pt.x, pt.y};
		drop_helper->Drop(pDataObject, &p, *pdwEffect);
	}
	return S_OK;
}

drop_source::drop_source()
{
	ref_count = 1;
}

HRESULT WINAPI drop_source::QueryInterface(REFIID iid, void** ppvObject)
{
	if(iid == IID_IDropSource || iid == IID_IUnknown)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
}

ULONG WINAPI drop_source::AddRef()
{
	return InterlockedIncrement(&ref_count);
}

ULONG WINAPI drop_source::Release()
{
	return InterlockedDecrement(&ref_count);
}

HRESULT WINAPI drop_source::QueryContinueDrag (BOOL fEscapePressed, DWORD grfKeyState)
{
	if(fEscapePressed == TRUE)
		return DRAGDROP_S_CANCEL;
	if((grfKeyState & MK_LBUTTON) == 0)
		return DRAGDROP_S_DROP;
	return S_OK;
}

HRESULT WINAPI drop_source::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

drop_effects get_effect_from_keys(DWORD keystate)
{
	if(((keystate & MK_ALT) == MK_ALT) || (((keystate & MK_SHIFT) == MK_SHIFT) && ((keystate & MK_CONTROL) == MK_CONTROL)))
		return drop_effects::link;
	else if((keystate & MK_CONTROL) == MK_CONTROL)
		return drop_effects::copy;
	else
		return drop_effects::move;
}

HGLOBAL data_to_global(const void* src, size_t len)
{
	void* dest = GlobalAlloc(GMEM_FIXED, len);
	memcpy(dest, src, len);
	return dest;
}

};
// DragDrop

namespace render_objects {

// Icon
size icon::get_small_icon_size()
{
	return size(static_cast<float>(GetSystemMetrics(SM_CXSMICON)), static_cast<float>(GetSystemMetrics(SM_CYSMICON)));
}

size icon::get_icon_size()
{
	return size(static_cast<float>(GetSystemMetrics(SM_CXICON)), static_cast<float>(GetSystemMetrics(SM_CYICON)));
};
// Icon

// Clip
clip::clip(const rect& area)
{
	if(area.sizef.width <= 0 || area.sizef.height <= 0)
		throw invalid_argument("Rectanglebounds too small");
	rc = area;
	is_rectangular = true;
}

clip::clip(const clip&& clm) : geo(clm.geo), rc(clm.rc), is_rectangular(clm.is_rectangular)
{}

clip::clip(geometry* area)
{
	if(!area)
		throw invalid_argument("Geometry is null");
	geo = area;
	is_rectangular = false;
}

rect clip::get_rect() const
{
	if(!is_rectangle_clip())
		throw logic_error("No rectangle set");
	return rc;
}

geometry* clip::get_geometry() const
{
	if(is_rectangle_clip())
		throw logic_error("No geometry set");
	return geo;
}
// Clip

// Cursor
cursor_surface::cursor_surface() {cur = 0;}

cursor_surface::cursor_surface(HCURSOR cur_)
{
	cur = CopyCursor(cur_);
	if(!cur && cur_)
		throw invalid_argument("Unable to load cursor");
}

cursor_surface::cursor_surface(const system_cursor& sys_cur)
{
	cur = LoadCursor(NULL, MAKEINTRESOURCE(sys_cur));
	if(!cur)
		throw invalid_argument("Unable to load cursor");
}

cursor_surface::cursor_surface(HINSTANCE inst, int id)
{
	cur = LoadCursor(inst, MAKEINTRESOURCE(id));
	if(!cur)
		throw invalid_argument("Unable to load cursor");
}

cursor_surface::cursor_surface(const cursor_surface& surf)
{
	cur = CopyCursor(surf.cur);
	if(!cur && surf.cur)
		throw invalid_argument("Unable to load cursor");
}

cursor_surface::cursor_surface(const wstring& filename)
{
	cur = LoadCursorFromFile(filename.c_str());
	if(!cur)
		throw invalid_argument("Unable to load cursor");
}

cursor_surface::~cursor_surface()
{
	if(cur)
		DestroyCursor(cur);
}

void cursor_surface::operator=(const cursor_surface& surf)
{
	cur = CopyCursor(surf.cur);
	if(!cur && surf.cur)
		throw invalid_argument("Unable to load cursor");
}

HCURSOR cursor_surface::get_cursor() const {return cur;}
// Cursor
};

// Impl
_cl_hlp::_cl_hlp(pr_event& pr_cl, pr_event& pr_dwn, pr_event& pr_up, bool& condition) : cb_cl(pr_cl), cond(condition)
{
	is_prsd = false;
	pr_dwn += make_func_ptr(this, &_cl_hlp::on_dwn);
	pr_up += make_func_ptr(this, &_cl_hlp::on_up);
}

void _cl_hlp::on_dwn(const mouse_buttons& b, const int, const point&)
{
	btn = b;
	is_prsd = true;
}

void _cl_hlp::on_up(const mouse_buttons& b, const int modifier, const point& pos)
{
	if(!is_prsd || b != btn || !cond) return;
	is_prsd = false;
	cb_cl(b, modifier, pos);
}
// Impl

// Dyn. Drawsurface
dynamic_drawsurface::dynamic_drawsurface() : _cl_hlp(mouse_click, mouse_down, mouse_up, is_mover)
{
	owner = 0;
	key_capture = mouse_capture = false;
	vert_align = vertical_align::top;
	hor_align = horizontal_align::left;
	hs_resources = false;
	is_mover = is_mdown = false;
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
	focus = false;
	is_transf = false;
	opacity = 1.f;
	cur = 0;
	m_font = 0;
	tabidx = 0;
	tabstop = true;
	tab_capture = false;
	clip_childs = true;
	is_drop_entered = false;
}

dynamic_drawsurface::~dynamic_drawsurface() 
{
	for(auto& surf : surfaces)
	{
		surface_removed(surf);
	}
	surfaces.clear();
}

void dynamic_drawsurface::render(render_objects::graphics* g)
{
	init_resources(g);
	if(surfaces.size() == 0)
		return;
	if(clip_childs)
		is_rectangular() ? g->push_clip(render_objects::clip(get_rect())) : g->push_clip(render_objects::clip(geo.get()));
	for(auto& surf : surfaces)
	{
		if(surf->get_visible())
		{
			if(surf->is_transf)
				g->set_transform(surf->get_absolute_transform());
			else
				g->set_transform(get_absolute_transform());
			surf->render(g);
		}
	}
	if(clip_childs)
		g->pop_clip();
}

void dynamic_drawsurface::on_syscolour_changed()
{
	syscolour_changed();
	for(auto& surf : surfaces)
		surf->on_syscolour_changed();
}

bool dynamic_drawsurface::contains(const point& p) const 
{
	if(is_rectangular())
		return rect(get_position(), get_size()).contains(p, get_absolute_transform());
	if(!geo) return false;
	return geo->contains(p, get_absolute_transform());
}

void dynamic_drawsurface::set_size(const size& sz, bool redraw) 
{
	if(sizef == sz) return;
	sizef.width =  max(min(maxsize.width, sz.width), minsize.width);
	sizef.height =  max(min(maxsize.height, sz.height), minsize.height);
	size_changed(sizef, resizing_types::restored);
	auto par = get_absolute_owner();
	if(par && redraw) par->layout();
}

void dynamic_drawsurface::set_position(const point& p, bool redraw)
{
	if(!change_if_diff(position, p)) return;
	position_changed(p);
	auto par = get_absolute_owner();
	if(par && redraw) par->layout();
}

void dynamic_drawsurface::set_visible(bool b)
{
	if(!change_if_diff(visible, b)) return;
	visible_changed(b);
	if(owner) owner->redraw(get_bounds());
}

void dynamic_drawsurface::set_min_size(const size& s)
{
	if(minsize == s) return;
	minsize = s;
	minsize.width = max(minsize.width, 1.f);
	minsize.height = max(minsize.height, 1.f);
	min_size_changed(s);
	if(sizef.height < s.height)
		set_size(size(sizef.width, s.height), false);
	if(sizef.width < s.width)
		set_size(size(s.width, sizef.height));
	auto par = get_absolute_owner();
	if(par) par->layout();
}

void dynamic_drawsurface::set_max_size(const size& s)
{
	if(maxsize == s) return;
	maxsize = s;
	maxsize.width = max(maxsize.width, 1.f);
	maxsize.height = max(maxsize.height, 1.f);
	max_size_changed(s);
	if(sizef.height > s.height)
		set_size(size(sizef.width, s.height), false);
	if(sizef.width > s.width)
		set_size(size(s.width, sizef.height));
	auto par = get_absolute_owner();
	if(par) par->layout();
}

void dynamic_drawsurface::set_padding(const padding& p)
{
	if(change_if_diff(pddng, p)) return;
	padding_changed(p);
	layout();
}

void dynamic_drawsurface::set_title(const wstring& s)
{
	if(!change_if_diff(title, s)) return;
	title_changed(s);
	if(owner) owner->redraw(get_bounds());
}

void dynamic_drawsurface::set_font(render_objects::font* f) 
{
	if(f == m_font.get()) return;
	m_font.reset(f);
	font_changed(f);
	owner->redraw(get_bounds());
}

void dynamic_drawsurface::set_tab_index(unsigned i)
{
	tabidx = i;
}

void dynamic_drawsurface::on_menu_opening()
{
	auto surf = get_focused_surface();
	if(surf)
		surf->on_menu_opening();
	else if(mmenu)
	{
		point p(get_center(this));
		if(owner)
		{
			POINT pa = {static_cast<int>(p.x), static_cast<int>(p.y)};
			MapWindowPoints(owner->get_handle(), GetDesktopWindow(), &pa, 1);
			p.x = static_cast<float>(pa.x); 
			p.y = static_cast<float>(pa.y);
		}
		mmenu->show(p);
	}
}

bool dynamic_drawsurface::on_tab_pressed() // Returns wether the parent has to select the child or not
{
	if(tab_capture)
		return false;
	bool risen = false;
	auto surf = get_focused_surface();
	if(surf) // Already 1 selected
	{
		if(surf->on_tab_pressed()) // Change to new child
		{
			unsigned last_tab = surf->get_tab_index();
			
			auto nexts = surfaces.end();
			for(auto itr = surfaces.begin(); itr != surfaces.end(); ++itr) // Get next child to focus
			{
				if((*itr)->get_tab_stop() && (*itr)->is_available())
				{
					if(nexts == surfaces.end())
					{
						if((*itr)->get_tab_index() > last_tab)
						{nexts = itr;}
					}
					else
					{
						if((*itr)->get_tab_index() > last_tab && (*itr)->get_tab_index() < (*nexts)->get_tab_index())
						{nexts == itr;}
					}
				}
			}

			if(nexts == surfaces.end()) // No more childs, set focus false and parent selects next
			{
				surf->set_focus(false);
				return true;
			}
			else // Select next child
			{
				(*nexts)->set_focus(true);
				return false;
			}
		}
		else // Do not change child
		{
			return false;
		}
	}
	else // No selected yet
	{
		auto nexts = surfaces.end();
		for(auto itr = surfaces.begin(); itr!=surfaces.end(); ++itr) // Get first child to focus
		{
			if((*itr)->get_tab_stop() && (*itr)->is_available())
			{
				if(nexts == surfaces.end())
					nexts = itr;
				else
				{
					if((*itr)->get_tab_index() < (*nexts)->get_tab_index())
						nexts = itr;
				}
			}
		}
		if(nexts == surfaces.end()) // Got no childs, parent selects next
			return true;
		else // Select first child
		{
			(*nexts)->set_focus(true);
			return false;
		}
	}
}

void dynamic_drawsurface::on_drag_enter(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect)
{
	bool risen = false;
	POINT pn;
	GetCursorPos(&pn);
	MapWindowPoints(GetDesktopWindow(), owner->get_handle(), &pn, 1);
	last_data_object = data_object;
	point p(static_cast<float>(pn.x), static_cast<float>(pn.y));
	for(auto surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available())
		{
			surf->on_drag_enter(data_object, keystate, pt, effect);
			surf->is_drop_entered = true;
			risen = true;
		}
	}
	if(!risen && get_enable_dragdrop())
		drag_enter(data_object, keystate, pt, effect);
}

void dynamic_drawsurface::on_drag_over(DWORD keystate, const point& pt, dragdrop::drop_effects* effect)
{
	bool risen = false;
	POINT pn;
	GetCursorPos(&pn);
	MapWindowPoints(GetDesktopWindow(), owner->get_handle(), &pn, 1);
	point p(static_cast<float>(pn.x), static_cast<float>(pn.y));
	for(auto surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available() && surf->is_drop_entered)
		{
			surf->on_drag_over(keystate, pt, effect);
			risen = true;
		}
		else if(surf->contains(p) && surf->is_available() && !surf->is_drop_entered)
		{
			surf->on_drag_enter(last_data_object, keystate, pt, effect);
			surf->is_drop_entered = true;
		}
		else if(surf->is_drop_entered && !surf->contains(p))
		{
			surf->on_drag_leave();
			surf->is_drop_entered = false;
		}
	}
	if(!risen && get_enable_dragdrop())
		drag_over(keystate, pt, effect);
}

void dynamic_drawsurface::on_drag_leave()
{
	for(auto surf : surfaces)
	{
		if(surf->is_drop_entered)
		{
			surf->on_drag_leave();
			surf->is_drop_entered = false;
		}
	}
	if(get_enable_dragdrop())
		drag_leave();
}

void dynamic_drawsurface::on_drop(IDataObject* data_object, DWORD keystate, const point& pt, dragdrop::drop_effects* effect)
{
	bool risen = false;
	POINT pn;
	GetCursorPos(&pn);
	MapWindowPoints(GetDesktopWindow(), owner->get_handle(), &pn, 1);
	point p(static_cast<float>(pn.x), static_cast<float>(pn.y));
	for(auto surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available() && surf->is_drop_entered/* && surf->get_enable_dragdrop()*/)
		{
			surf->on_drop(data_object, keystate, pt, effect);
			surf->is_drop_entered = false;
			risen = true;
		}
	}
	if(!risen && get_enable_dragdrop())
		drop(data_object, keystate, pt, effect);
}

void dynamic_drawsurface::set_focus(bool b) 
{
	if(!change_if_diff(focus, b)) return;
	focus_changed(b);
	if(b && parent)
		parent->set_focus(true);
	if(!b)
	{
		for(auto& surf : surfaces)
			surf->set_focus(false);
	}
	if(parent)
	{
		parent->set_focused_surface(b ? this : 0);
	}
	else if(owner)
		owner->set_focused_surface(b ? this : 0);
	if(owner)owner->redraw();
}

void dynamic_drawsurface::set_focused_surface(dynamic_drawsurface* surf)
{
	if(focused_surf == surf) return;
	if(focused_surf)
	{
		focused_surf->set_focus(false);
	}
	if(surf)
	{
		surf->set_focus(true);
	}
	focused_surf = surf;
}

void dynamic_drawsurface::add_surface(dynamic_drawsurface* surf)
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
	if(surf->get_tab_index() == 0)
		surf->set_tab_index(surfaces.size()-1);
	layout();
	surface_added(surf);
}

void dynamic_drawsurface::remove_surface(dynamic_drawsurface* surf) 
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
}

void dynamic_drawsurface::layout()
{
	if(hs_resources)
		is_rectangular() ? update_shape(get_rect()) : update_shape(geo.get());
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
				x = get_position().x;
				w = 0;
				break;
			case horizontal_align::right:
				x = get_size().width + get_position().x - surf->get_size().width;
				w = 0;
				break;
			case horizontal_align::center:
				x = (get_size().width/2.f + get_position().x) - surf->get_size().width / 2.f;
				w = 0;
				break;
			case horizontal_align::stretch:
				w = get_size().width;
				x = get_position().x;
			}
			switch(surf->get_vertical_align())
			{
			case vertical_align::top:
				y = get_position().y;
				h = 0;
				break;
			case vertical_align::bottom:
				y = get_size().height + get_position().y - surf->get_size().height;
				h = 0;
				break;
			case vertical_align::center:
				y = (get_size().height / 2.f+ get_position().y) - surf->get_size().height / 2.f;
				h = 0;
				break;
			case vertical_align::stretch:
				y = get_position().y;
				h = get_size().height;
			}
			surf->set_position(point(x+pddng.left+m.left, y+pddng.top+m.top), false);
			if(surf->get_auto_size())surf->set_size(size(w-pddng.right-pddng.left-m.right-m.left, h-pddng.top-pddng.bottom-m.bottom-m.top), false);
		}
		surf->layout();
	}
	redraw(get_bounds());
	layouted();
}

void dynamic_drawsurface::create_resources(render_objects::graphics* g) 
{
	if(hs_resources) return;
	if(!is_rectangular())
	{
		geo = shared_ptr<render_objects::geometry>(g->create_geometry());
		update_shape(geo.get());
	}
	else
		update_shape(get_rect());
	hs_resources = true;
}

void dynamic_drawsurface::set_parent(dynamic_drawsurface* new_parent) 
{
	if(new_parent == parent) return;
	if(parent)
		parent->remove_surface(this);
	parent = new_parent; 
	parent_changed(new_parent);
	if(parent)
	{
		parent->add_surface(this);
		if(owner)owner->remove_surface(this);
	}
	else
	{
		if(owner)owner->add_surface(this);
	}
	redraw();
}

void dynamic_drawsurface::set_owner(drawsurface* ownr)
{
	if(ownr == owner) return;
	if(owner)
		owner->remove_surface(this);
	owner = ownr;
	owner_changed(ownr);
	if(owner)
	{
		if(!parent)owner->add_surface(this);
	}
	redraw();
}

rect dynamic_drawsurface::get_bounds() const
{
	if(!hs_resources)
		return rect(0.f, 0.f, 0.f, 0.f);
	if(is_rectangular())
		return rect(get_position(), get_size()).get_bounds(get_absolute_transform());
	else
		return  geo->get_bounds(get_absolute_transform());
}

bool dynamic_drawsurface::get_enabled() const 
{
	auto par = get_absolute_owner();
	if(par) return enabled && par->get_enabled();
	return enabled;
}

void dynamic_drawsurface::set_enabled(bool b)
{
	if(!change_if_diff(enabled, b)) return;
	enabled_changed(b);
	if(!b)
		set_focus(false);
	if(owner)owner->redraw(get_bounds());
}

void dynamic_drawsurface::set_auto_position(bool b) 
{
	if(b == auto_pos) return;
	auto_pos = b;
	auto par = get_absolute_owner();
	if(par) par->layout();
}

void dynamic_drawsurface::set_auto_size(bool b)
{
	if(b == auto_size) return;
	auto_size = b;
	auto par = get_absolute_owner();
	if(par) par->layout();
}

void dynamic_drawsurface::set_horizontal_align(const horizontal_align& h)
{
	if(hor_align == h) return;
	hor_align = h;
	auto par = get_absolute_owner();
	if(par) par->layout();
	horizontal_align_changed(h);
}

void dynamic_drawsurface::set_vertical_align(const vertical_align& h)
{
	if(vert_align == h) return;
	vert_align = h;
	auto par = get_absolute_owner();
	if(par) par->layout();
	vertical_align_changed(h);
}

void dynamic_drawsurface::set_margin(const margin& m) 
{
	if(mrgn == m) return;
	mrgn = m;
	margin_changed(m);
	auto par = get_absolute_owner();
	if(par) par->layout();
}

void dynamic_drawsurface::set_transform(const matrix& m) 
{
	if(!change_if_diff(trnsfrm, m)) return;
	is_transf = !m.is_identity();
	for(auto& surf : surfaces)
		surf->transform_changed(m);
	if(owner) owner->redraw();
}

void dynamic_drawsurface::set_opacity(float f)
{
	if(!change_if_diff(opacity, f)) return;
	opacity_changed(f);
	if(owner) owner->redraw(get_bounds());	
}

void dynamic_drawsurface::on_mouse_move(const int m, const point& p)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->contains(p) && surf->get_visible())
		{
			if(!surf->is_mouse_over())
			{
				if(surf->get_enabled())
					surf->on_mouse_enter(p);
				if(owner)
					owner->redraw(get_bounds());
			}
			if(surf->get_enabled())
				surf->on_mouse_move(m, p);
			risen = true;
		}
		else if(surf->is_mouse_over() && surf->is_available())
		{
			surf->on_mouse_leave(p);
			if(owner)
				owner->redraw(get_bounds());
		}
	}
	if(!risen) mouse_move(m, point_to_surface(p));
}

void dynamic_drawsurface::on_mouse_dbl_click(const mouse_buttons& b, const int m, const point& p)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->contains(p) && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_mouse_dbl_click(b, m, p);
			risen = true;
		}
	}
	if(!risen)
	{
		auto surf = get_focused_surface();
		if(surf)
			surf->set_focus(false);
		set_focus(true);
		mouse_dbl_click(b, m, point_to_surface(p));
	}
}

void dynamic_drawsurface::on_mouse_down(const mouse_buttons& b, const int m, const point& p)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->contains(p) &&  surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_mouse_down(b, m, p);
			risen = true;
		}
	}
	if(!risen)
	{
		is_mdown = true;
		if(b == mouse_buttons::left) draw_state = drawing_state::pressed;
		mouse_down(b, m, point_to_surface(p));
		set_focus(true);
	}
}

void dynamic_drawsurface::on_mouse_up(const mouse_buttons& b, const int m, const point& p)
{
	bool risen = false;
	is_mdown = false;
	is_mover ? draw_state = drawing_state::hot : draw_state = drawing_state::normal;
	for(auto& surf : surfaces)
	{
		if(surf->contains(p) && surf->is_available())
		{
			surf->on_mouse_up(b, m, p);
			risen = true;
		}
	}
	if(!risen) 
	{
		if(mmenu && b == mouse_buttons::right && get_focus())
		{
			POINT p;
			GetCursorPos(&p);
			mmenu->show(point(static_cast<float>(p.x), static_cast<float>(p.y)));
		}
		mouse_up(b, m, point_to_surface(p));
	}
}

void dynamic_drawsurface::on_mouse_wheel(const int m, const point& p, int delta)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_mouse_wheel() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_mouse_wheel(m, p, delta);
			risen = true;
		}
	}
	if(!risen)
		mouse_wheel(m, point_to_surface(p), delta);
}

void dynamic_drawsurface::on_mouse_h_wheel(const int m, const point& p, int delta)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_mouse_wheel() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_mouse_h_wheel(m, p, delta);
			risen = true;
		}
	}
	if(!risen)
		mouse_h_wheel(m, point_to_surface(p), delta);
}

void dynamic_drawsurface::crt_up()
{
	bool risen = false;
	is_mdown = false;
	is_mover ? draw_state = drawing_state::hot : draw_state = drawing_state::normal;
}

void dynamic_drawsurface::on_mouse_enter(const point& p)
{
	is_mover = true;
	is_mdown ? draw_state = drawing_state::pressed : draw_state = drawing_state::hot;
	mouse_enter(p);
	if(owner)
	{
		SendMessage(owner->get_handle(), WM_GCL_CURSORCHANGED, reinterpret_cast<WPARAM>(cur.get_cursor()), 0);
		owner->redraw(get_bounds());
	}
}

void dynamic_drawsurface::on_mouse_leave(const point& p)
{
	draw_state = drawing_state::normal;
	is_mover = is_mdown = false;
	for(auto& surf : surfaces)
	{
		if(surf->is_available())
		{
			surf->on_mouse_leave(p);
		}
	}
	mouse_leave(p);
	if(parent)
	{
		if(parent->contains(p))
			parent->on_mouse_enter(p);
	}
	else if(owner)
	{
		SendMessage(owner->get_handle(), WM_GCL_CURSORCHANGED, 0, 0);
	}
	if(owner)
		owner->redraw(get_bounds());
}

void dynamic_drawsurface::on_key_down(const virtual_keys& key, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_key_down(key, params);
			risen = true;
		}
	}
	if(!risen)
		key_down(key, params);
}

void dynamic_drawsurface::on_key_up(const virtual_keys& key, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_key_up(key, params);
			risen = true;
		}
	}
	if(!risen)
	{
		key_up(key, params);
	}
}

void dynamic_drawsurface::on_syskey_down(const virtual_keys& key, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_syskey_down(key, params);
			risen = true;
		}
	}
	if(!risen)
		syskey_down(key, params);
}

void dynamic_drawsurface::on_syskey_up(const virtual_keys& key, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_syskey_up(key, params);
			risen = true;
		}
	}
	if(!risen)
		syskey_up(key, params);
}

void dynamic_drawsurface::on_char_sent(wchar_t c, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_char_sent(c, params);
			risen = true;
		}
	}
	if(!risen)
		char_sent(c, params);
}

void dynamic_drawsurface::on_deadchar_sent(wchar_t c, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_deadchar_sent(c, params);
			risen = true;
		}
	}
	if(!risen)
		deadchar_sent(c, params);
}

void dynamic_drawsurface::on_syschar_sent(wchar_t c, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_syschar_sent(c, params);
			risen = true;
		}
	}
	if(!risen)
		syschar_sent(c, params);
}

void dynamic_drawsurface::on_sysdeadchar_sent(wchar_t c, const key_extended_params& params)
{
	bool risen = false;
	for(auto& surf : surfaces)
	{
		if(surf->get_captures_keyboard() && surf->get_focus() && surf->get_visible())
		{
			if(surf->get_enabled())
				surf->on_sysdeadchar_sent(c, params);
			risen = true;
		}
	}
	if(!risen)
		sysdeadchar_sent(c, params);
}

void dynamic_drawsurface::set_cursor(const render_objects::cursor_surface& cur_s)
{
	cur = cur_s;
	if(owner)
		SendMessage(owner->get_handle(), WM_GCL_CURSORCHANGED, reinterpret_cast<WPARAM>(cur.get_cursor()), 0);
}

void dynamic_drawsurface::erase_cursor() 
{
	cur = render_objects::cursor_surface();
	if(owner)
		SendMessage(owner->get_handle(), WM_GCL_CURSORCHANGED, 0, 0);
}

void dynamic_drawsurface::init_resources(gcl::render_objects::graphics* g)
{
	if(!hs_resources)
	{
		create_resources(g);
		hs_resources = true;
	}
}

void dynamic_drawsurface::set_menu(ui::context_menu* m)
{
	if(m == mmenu.get())
		return;
	mmenu = shared_ptr<ui::context_menu>(m);
	menu_changed(m);
}

matrix dynamic_drawsurface::get_absolute_transform() const
{
	matrix m = trnsfrm;
	dynamic_drawsurface* p = parent;
	while(p)
	{
		m = p->get_transform()*m;
		p = p->get_parent();
	}
	return m;
}

void dynamic_drawsurface::set_clip_childs(bool b)
{
	if(!change_if_diff(clip_childs, b)) return;
	if(owner)
		b ? owner->redraw(get_bounds()) : owner->redraw();
}
// Dyn. Drawsurface*/
namespace ui {

// Menugraphics
bool menu_graphics::is_high_contrast()
{
	HIGHCONTRAST hcntr = {};
	hcntr.cbSize = sizeof(HIGHCONTRAST);
	if(!SystemParametersInfo(SPI_GETHIGHCONTRAST, 0, &hcntr, 0))
		throw runtime_error("SystemParametersInfo failed");
	return (hcntr.dwFlags & HCF_HIGHCONTRASTON) == HCF_HIGHCONTRASTON;
}

void menu_graphics::set_front_colour(const colour& cl)
{
	if(!change_if_diff(cl_hi, cl))
		return;
	if(highlight_brush)
		highlight_brush->set_colour(cl);
	front_colour_changed(cl);
	int_redraw();
}

void menu_graphics::set_hot_colour(const colour& cl)
{
	if(!change_if_diff(cl_hot, cl))
		return;
	if(hot_brush)
		hot_brush->set_colour(cl);
	hot_colour_changed(cl);
	int_redraw();
}

void menu_graphics::set_grayed_colour(const colour& cl)
{
	if(!change_if_diff(cl_gray, cl))
		return;
	if(grayed_brush)
		grayed_brush->set_colour(cl);
	grayed_colour_changed(cl);
	int_redraw();
}

void menu_graphics::set_back_colour(const colour& cl)
{
	if(!change_if_diff(cl_back, cl))
		return;
	back_colour_changed(cl);
	int_redraw();
}
// Menugraphics

// Menustrip
menu_strip::menu_strip()
{
	height = 0;
	checked = checkable = false;
	enabled = true;
	seperator_top = seperator_bottom = false;
	hidx = -1;
	hs_childs = false;
	arrow_size = 5.f;
	cl_back = colour::black;
	child_shown = false;
}

menu_strip::~menu_strip()
{
	if(handle)
	{
		DestroyWindow(handle);
		UnregisterClass(classname.c_str(), GetModuleHandle(NULL));
	}
}

void menu_strip::add_strip(menu_strip* strip)
{
	if(!hs_childs)
		throw logic_error("Strip wasn't initialized");
	strip->p = this;
	childs.push_back(strip);
	int_redraw();
}

void menu_strip::update_resources()
{
	if(hs_childs)
	{
		classname = gcl_create_classname(L"GCL_Menustrip&");
		WNDCLASS wc = {};
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = WndProc;
		wc.lpszClassName = classname.c_str();
		wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
		wc.style = CS_DROPSHADOW | CS_ENABLE;
		if(!RegisterClass(&wc))
			throw runtime_error("Unable to register menuclass");
		handle = CreateWindowEx(WS_EX_TOPMOST, classname.c_str(), L"", WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, p->get_howner(), 0, GetModuleHandle(NULL), this);
		if(!handle)
			throw runtime_error("Unable to create menu");
		SetFocus(p->get_howner());
		window_graphics.reset(p->get_graphics()->create_graphics(handle, size_changed));
		window_graphics->set_antialias(true);
		title_font = shared_ptr<font>(window_graphics->get_system_font());
		if(is_high_contrast())
		{
			cl_hi = colour::menu_text;
			cl_gray = colour::gray_text;
			cl_hot = colour::menu_highlight;
		}
		else
		{
			cl_hi = colour::white;
			cl_gray = colour::gray;
			cl_hot = colour::gcl_gray;
		}
		highlight_brush = shared_ptr<render_objects::solid_brush>(window_graphics->create_solid_brush(cl_hi));
		grayed_brush = shared_ptr<solid_brush>(window_graphics->create_solid_brush(cl_gray));
		hot_brush = shared_ptr<solid_brush>(window_graphics->create_solid_brush(cl_hot));
		seperator_pen = shared_ptr<pen>(window_graphics->create_pen(grayed_brush.get(), 1.f));
	}
	else
	{
		if(handle)
		{
			DestroyWindow(handle);
			UnregisterClass(classname.c_str(), GetModuleHandle(NULL));
		}
		window_graphics.reset();
		title_font.reset();
		highlight_brush.reset();
		grayed_brush.reset();
		hot_brush.reset();
		seperator_pen.reset();
	}
}

void menu_strip::show(const point& p, bool key)
{
	hidx = key ? 0 : -1;
	float img_size = render_objects::icon::get_small_icon_size().width;
	float currheight = 0, currwidth = img_size+16.f;
	window_graphics->begin();
	for(auto& ch : childs)
	{
		currwidth = max(currwidth, title_font->get_metrics(ch->get_title(), size(static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)), static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK))), window_graphics.get()).get_width() + img_size+16.f);
		currheight += ch->get_height()+4.f;
	}
	window_graphics->end();
	RECT rc;
	GetClientRect(GetDesktopWindow(), &rc);
	if(rc.bottom <= p.y+currheight+4.f)
		SetWindowPos(handle, 0, static_cast<int>(p.x), static_cast<int>(p.y-currheight-4.f), static_cast<int>(currwidth+arrow_size*3), static_cast<int>(currheight+4.f), SWP_NOZORDER);
	else
		SetWindowPos(handle, 0, static_cast<int>(p.x), static_cast<int>(p.y), static_cast<int>(currwidth+arrow_size*3), static_cast<int>(currheight+4.f), SWP_NOZORDER);
	shown(p);
	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);
}

void menu_strip::render(graphics* g, drawing_state draw_state, const point& origin)
{
	float imgh = icon::get_small_icon_size().height;
	if(draw_state == drawing_state::hot && enabled)
		g->fill_rect(rect(origin.x+2, origin.y-2, p->get_hsize().width - 4, get_height()+4), p->get_hot_brush());
	if(checkable && checked && image)
	{
		g->fill_rect(rect(origin.x+5, origin.y+get_height()/2.f-imgh/2.f-2, imgh+4, imgh+4), p->get_hot_brush());
	}
	else if(checkable && checked && !image)
	{
		auto hook = get_hook(origin.x+2+imgh/2.f, (origin.y+get_height()/2.f-7.f));
		g->fill_polygon(hook, 6, enabled ? p->get_brush() : p->get_grayed_brush()); // 6 -> Hookheight
		delete[] hook;
	}
	if(image)
		g->draw_texture(image.get(), rect(origin.x+7, origin.y+get_height()/2.f-imgh/2.f, imgh, imgh));
	g->draw_string(title, rect(origin.x+imgh+14.f, origin.y, p->get_hsize().width, get_height()), p->get_title_font(), enabled ? p->get_brush() : p->get_grayed_brush(), render_objects::string_format::direction_left_to_right, render_objects::horizontal_string_align::left, render_objects::vertical_string_align::middle);
	if(seperator_top)
		g->draw_line(point(origin.x+imgh+12.f, origin.y-2.f), point(p->get_hsize().width-2, origin.y-2.f), p->get_pen());
	if(seperator_bottom)
		g->draw_line(point(origin.x+imgh+12.f, origin.y+get_height()+2.f), point(p->get_hsize().width-2, origin.y+get_height()+2.f), p->get_pen());
	g->set_antialias(false);
	if(hs_childs && !p->get_child_shown())
	{
		point arr[] = {point(p->get_hsize().width - 5, origin.y+get_height()/2.f), point(p->get_hsize().width - arrow_size - 5, origin.y+get_height()/2.f-arrow_size), point(p->get_hsize().width-arrow_size-5, origin.y+get_height()/2.f+arrow_size)};
		g->fill_polygon(arr, 3, p->get_grayed_brush());
	}
	else if(hs_childs)
	{
		point arr[] = {point(p->get_hsize().width-5-arrow_size, origin.y+get_height()/2.f), point(p->get_hsize().width - 5, origin.y+get_height()/2.f-arrow_size), point(p->get_hsize().width - 5, origin.y+get_height()/2.f+arrow_size)};
		g->fill_polygon(arr, 3, p->get_grayed_brush());
	}
	g->set_antialias(true);
}

void menu_strip::render_childs(graphics* g)
{
	g->begin();
	g->clear(is_high_contrast() ? colour::menu : cl_back);
	g->set_transform(matrix());
	float y = 4.f;
	for(unsigned i = 0; i < childs.size(); ++i)
	{
		menu_strip* strip = childs[i];
		strip->render(g, hidx == i ? drawing_state::hot : drawing_state::normal, point(0, y));
		y += strip->get_height()+4.f;
	}
	g->end();
}

void menu_strip::int_redraw(rect rc)
{
	if(!handle)
		return;
	RECT rec;
	rec.bottom = static_cast<int>(rc.get_bottom());
	rec.top = static_cast<int>(rc.get_top());
	rec.left = static_cast<int>(rc.get_left());
	rec.right = static_cast<int>(rc.get_right());
	InvalidateRect(handle, &rec, TRUE);
}

void menu_strip::int_redraw()
{
	if(handle)
		InvalidateRect(handle, 0, TRUE);
	p->int_redraw();
}

point* menu_strip::get_hook(float x, float y)
{
	float wcheck = 1.f;
	point* hook = new point[6];
	hook[0] = point(3+x, 6+y);
	hook[1] = point(3+x, 6+y+wcheck);
	hook[2] = point(6+x, 10+y);
	hook[3] = point(12+x, 4+y);
	hook[4] = point(12+x, 4+y-wcheck);
	hook[5] = point(6+x, 10+y-wcheck);
	return hook;
}

void menu_strip::set_height(float v)
{
	if(!change_if_diff(height, v))
		return;
	if(p)
		p->int_redraw();
}

void menu_strip::set_image(texture* img)
{
	image.reset(img);
	if(p)
		p->int_redraw();
}

void menu_strip::set_checked(bool b)
{
	if(!change_if_diff(checked, b))
		return;
	if(p)
		p->int_redraw();
}

void menu_strip::set_checkable(bool b)
{
	if(!change_if_diff(checkable, b))
		return;
	if(p)
		p->int_redraw();
}

void menu_strip::set_enabled(bool b)
{
	if(!change_if_diff(enabled, b))
		return;
	if(p)
		p->int_redraw();
}

void menu_strip::set_title(const wstring& text)
{
	if(!change_if_diff(title, text))
		return;
	if(p)
		p->int_redraw();
}

void menu_strip::set_top_seperator(bool b)
{
	if(!change_if_diff(seperator_top, b))
		return;
	if(p)
		p->int_redraw();
}

void menu_strip::set_bottom_seperator(bool b)
{
	if(!change_if_diff(seperator_bottom, b))
		return;
	if(p)
		p->int_redraw();
}

size menu_strip::get_hsize() const
{
	RECT rc = {};
	GetClientRect(handle, &rc);
	return size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
}

void menu_strip::on_syscolour_changed()
{
	if(!highlight_brush)
		return;
	if(is_high_contrast())
	{
		highlight_brush->set_colour(colour::menu_text);
		grayed_brush->set_colour(colour::gray_text);
		hot_brush->set_colour(colour::menu_highlight);
	}
	else
	{
		highlight_brush->set_colour(cl_hi);
		grayed_brush->set_colour(cl_gray);
		hot_brush->set_colour(cl_hot);
	}
}

LRESULT menu_strip::message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			if(window_graphics)
				render_childs(window_graphics.get());
			break;
		}
	case WM_SIZE:
		{
			RECT rc = {}; GetClientRect(hWnd, &rc);
			size_changed(size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top)), static_cast<resizing_types>(wParam));
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
	case WM_SETCURSOR:
		{
			SetCursor(LoadCursor(0, IDC_ARROW));
			break;
		}
	case WM_SYSCOLORCHANGE:
		{
			on_syscolour_changed();
			break;
		}
	case WM_MOUSEMOVE:
		{
			hidx = -1;
			float y = 4.f;
			for(unsigned i = 0; i < childs.size(); ++i)
			{
				menu_strip* strip = childs[i];
				if(rect(0, y, 200, strip->get_height()).contains(point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)))))
					hidx = i;
				y += strip->get_height()+4.f;
			}
			if(hidx != -1)
				InvalidateRect(hWnd, 0, TRUE);
			if(!mouse_over)
			{
				TRACKMOUSEEVENT tre = {};
				tre.cbSize = sizeof(tre);
				tre.dwFlags = TME_LEAVE | TME_HOVER;
				tre.hwndTrack = hWnd;
				TrackMouseEvent(&tre);
			}
			break;
		}
	case WM_MOUSEHOVER:
		mouse_over = true;
		break;
	case WM_MOUSELEAVE:
		mouse_over = false;
		hidx = -1;
		InvalidateRect(hWnd, 0, TRUE);
		mouse_down = false;
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_LBUTTONUP:
		if(mouse_down)
		{
			bool risen = false;
			float y = 4.f;
			for(unsigned i = 0; i < childs.size(); ++i)
			{
				menu_strip* strip = childs[i];
				if(rect(0, y, 200, strip->get_height()).contains(point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)))) && strip->enabled)
				{
					if(strip->get_checkable())
						strip->set_checked(!strip->get_checked());
					strip->click(i);
					risen = true;
					if(strip->hs_childs)
					{
						risen = false;
						if(i == showed_idx && child_shown)
						{
							strip->close(true);
							child_shown = false;
						}
						else if(child_shown)
						{
							if(childs.size() > showed_idx)
								childs[showed_idx]->close(true);
							child_shown = false;
						}
						else
						{
							RECT rc;
							GetWindowRect(handle, &rc);
							child_shown = true;
							InvalidateRect(hWnd, 0, TRUE);
							strip->show(point(static_cast<float>(rc.right), static_cast<float>(rc.top + y-4)), false);
							showed_idx = i;
						}
					}
					break;
				}
				y += strip->get_height()+4.f;
			}
			if(!risen)
				break;
		}
		else
		{
			mouse_down = false;
			break;
		}
		case WM_KILLFOCUS:
		{
			wchar_t buff[256]; 
			GetClassName(reinterpret_cast<HWND>(wParam), buff, 255);
			wstring clname(buff);
			if(clname.find(L"GCL_Menustrip") == wstring::npos && clname.find(L"GCL_Contextmenu") == wstring::npos)
			{
				if(childs.size() > showed_idx && child_shown)
				{
					childs[showed_idx]->close(true);
					child_shown = false;
				}
				p->close(false);
				ShowWindow(hWnd, SW_HIDE);
			}
			break;
		}
		case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_DOWN:
				if(++hidx >= static_cast<int>(childs.size()))
					hidx = min(childs.size()-1, 0u);
				break;
			case VK_UP:
				if(--hidx < 0)
					hidx = childs.size()-1;
				break;
			case VK_LEFT:
				{
					SetFocus(p->get_hhandle());
					p->_set_child_shown(false);
					InvalidateRect(p->get_hhandle(), 0, TRUE);
					close(true);
					break;
				}
			case VK_RETURN:
				{
					if(hidx >= static_cast<int>(childs.size()) ||hidx < 0)
						break;
					auto strip = childs[hidx];
					if(strip->get_checkable())
						strip->set_checked(!strip->get_checked());
					strip->click(hidx);
					if(!strip->hs_childs)
					{
						close(true);
						break;
					}
				}
			case VK_RIGHT:
				{
					if(hidx >= static_cast<int>(childs.size()) || hidx < 0)
						break;
					auto strip = childs[hidx];
					if(strip->hs_childs && strip->enabled)
					{
						RECT rc;
						GetWindowRect(handle, &rc);
						child_shown = true;
						InvalidateRect(hWnd, 0, TRUE);
						float y = 4.f;
						for(int i = 0; i < hidx; ++i)
							y += childs[i]->get_height()+4.f;
						strip->show(point(static_cast<float>(rc.right), static_cast<float>(rc.top + y-4)), true);
						showed_idx = hidx;
					}
				}
				break;
			case VK_APPS:
				close(true);
				p->close(false);
			}
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
	case WM_SETFOCUS:
		focus = true;
		break;
	case WM_LBUTTONDOWN:
		mouse_down = true;
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		UnregisterClass(classname.c_str(), GetModuleHandle(0));
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void menu_strip::close(bool parent)
{
	ShowWindow(handle, SW_HIDE);
	if(parent)
	{
		if(childs.size() > showed_idx && child_shown)
		{
			childs[showed_idx]->close(true);
			child_shown = false;
		}
	}
	else
	{
		p->close(false);
	}
}

LRESULT CALLBACK menu_strip::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	menu_strip* inst = 0;
	if(msg == WM_CREATE)
	{
		TRACKMOUSEEVENT tre = {};
		tre.cbSize = sizeof(tre);
		tre.dwFlags = TME_LEAVE | TME_HOVER;
		tre.hwndTrack = hWnd;
		TrackMouseEvent(&tre);
		inst = static_cast<menu_strip*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(inst));
	}
	else
	{
		inst = reinterpret_cast<menu_strip*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if(!inst) return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return inst->message_received(hWnd, msg, wParam, lParam);
}

bool menu_strip::test_handle(HWND hWnd) const
{
	if(handle == hWnd)
		return true;
	for(auto itr = childs.begin(); itr != childs.end(); ++itr)
	{
		if((*itr)->test_handle(hWnd))
			return true;
	}
	return false;
}

void menu_strip::init(menu_graphics* parent)
{
	p = parent;
	hs_childs = true;
	update_resources();
}
// Menustrip

// Contextmenu
context_menu::context_menu(HWND owner_, render_objects::graphics* owner_graphics)
{
	if(!owner_graphics)
		throw invalid_argument("Invalid graphics");
	hidx = -1;
	child_arrow = 5.f;
	owner = owner_;
	opened = false;
	classname =  gcl_create_classname(L"GCL_Contextmenu&");
	WNDCLASS wc = {};
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = classname.c_str();
	wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
	wc.style = CS_DROPSHADOW | CS_ENABLE;
	if(!RegisterClass(&wc))
		throw runtime_error("Unable to register menuclass");
	handle = CreateWindowEx(WS_EX_TOPMOST, classname.c_str(), L"", WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, owner, 0, GetModuleHandle(NULL), this);
	if(!handle)
		throw runtime_error("Unable to create menu");
	SetFocus(owner);
	window_graphics.reset(owner_graphics->create_graphics(handle, size_changed));
	window_graphics->set_antialias(true);
	title_font = shared_ptr<render_objects::font>(window_graphics->get_system_font());
	cl_back = colour::black;
	if(is_high_contrast())
	{
		cl_hi = colour::menu_text;
		cl_gray = colour::gray_text;
		cl_hot = colour::menu_highlight;
	}
	else
	{
		cl_hi = colour::white;
		cl_gray = colour::gray;
		cl_hot = colour::gcl_gray;
	}
	highlight_brush = shared_ptr<render_objects::solid_brush>(window_graphics->create_solid_brush(cl_hi));
	grayed_brush = shared_ptr<solid_brush>(window_graphics->create_solid_brush(cl_gray));
	hot_brush = shared_ptr<solid_brush>(window_graphics->create_solid_brush(cl_hot));
	seperator_pen = shared_ptr<pen>(window_graphics->create_pen(grayed_brush.get(), 1.f));
	child_shown = false;
}

context_menu::~context_menu()
{
	if(handle)
	{
		DestroyWindow(handle);
		UnregisterClass(classname.c_str(), GetModuleHandle(NULL));
	}
}

void context_menu::add_strip(menu_strip* strip)
{
	if(!strip->p)
		strip->p = this;
	childs.push_back(strip);
	if(handle)
		int_redraw();
	if(!strip->hs_resources)
		strip->create_resources(window_graphics.get());
}

void context_menu::remove_strip(menu_strip* strip)
{
	childs.erase(remove(childs.begin(), childs.end(), strip));
	if(handle)
		int_redraw();
}

void context_menu::clear_strips()
{
	childs.clear();
	if(handle)
		int_redraw();
}

bool context_menu::test_handle(HWND hWnd)
{
	if(handle == hWnd)
		return true;
	for(auto itr = childs.begin(); itr != childs.end(); ++itr)
	{
		if((*itr)->test_handle(hWnd))
			return true;
	}
	return false;
}

void context_menu::show(const point& p, int custom_width)
{
	float img_size = render_objects::icon::get_small_icon_size().width;
	float currheight = 0, currwidth = img_size+16.f;
	window_graphics->begin();
	for(auto& ch : childs)
	{
		currwidth = max(currwidth, title_font->get_metrics(ch->get_title(), size(static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)), static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK))), window_graphics.get()).get_width() + img_size+16.f);
		currheight += ch->get_height()+4.f;
	}
	if(custom_width > 0 && (custom_width-child_arrow*3) > currwidth)
		currwidth = custom_width-child_arrow*3;
	window_graphics->end();
	RECT rc;
	GetClientRect(GetDesktopWindow(), &rc);
	if(rc.bottom <= p.y+currheight+4.f)
		SetWindowPos(handle, 0, static_cast<int>(p.x), static_cast<int>(p.y-currheight-4.f), static_cast<int>(currwidth+child_arrow*3), static_cast<int>(currheight+4.f), SWP_NOZORDER);
	else
		SetWindowPos(handle, 0, static_cast<int>(p.x), static_cast<int>(p.y), static_cast<int>(currwidth+child_arrow*3), static_cast<int>(currheight+4.f), SWP_NOZORDER);
	shown(p);
	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);
	opened = true;
}

void context_menu::int_redraw(rect rc)
{
	if(!handle)
		return;
	RECT rec;
	rec.bottom = static_cast<int>(rc.get_bottom());
	rec.top = static_cast<int>(rc.get_top());
	rec.left = static_cast<int>(rc.get_left());
	rec.right = static_cast<int>(rc.get_right());
	InvalidateRect(handle, &rec, TRUE);
}

void context_menu::int_redraw()
{
	if(handle)
		InvalidateRect(handle, 0, TRUE);
}

void context_menu::render(render_objects::graphics* gr)
{
	gr->begin();
	gr->clear(is_high_contrast() ? colour::menu : cl_back);
	gr->set_transform(matrix());
	float y = 4.f;
	for(unsigned i = 0; i < childs.size(); ++i)
	{
		menu_strip* strip = childs[i];
		strip->render(gr, hidx == i ? drawing_state::hot : drawing_state::normal, point(0, y));
		y += strip->get_height()+4.f;
	}
	gr->end();
}

size context_menu::get_hsize() const
{
	RECT rc = {};
	GetClientRect(handle, &rc);
	return size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
}

void context_menu::on_syscolour_changed()
{
	if(!highlight_brush)
		return;
	if(is_high_contrast())
	{
		highlight_brush->set_colour(colour::menu_text);
		grayed_brush->set_colour(colour::gray_text);
		hot_brush->set_colour(colour::menu_highlight);
	}
	else
	{
		highlight_brush->set_colour(cl_hi);
		grayed_brush->set_colour(cl_gray);
		hot_brush->set_colour(cl_hot);
	}
}


LRESULT context_menu::message_received(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			if(window_graphics)
				render(window_graphics.get());
			break;
		}
	case WM_SYSCOLORCHANGE:
		{
			on_syscolour_changed();
			break;
		}
	case WM_SIZE:
		{
			RECT rc = {}; GetClientRect(hWnd, &rc);
			size_changed(size(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top)), static_cast<resizing_types>(wParam));
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
	case WM_SETCURSOR:
		{
			SetCursor(LoadCursor(0, IDC_ARROW));
			break;
		}
	case WM_MOUSEMOVE:
		{
			float y = 4.f;
			for(unsigned i = 0; i < childs.size(); ++i)
			{
				menu_strip* strip = childs[i];
				if(rect(0, y, get_hsize().width, strip->get_height()).contains(point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)))))
					hidx = i;
				y += strip->get_height()+4.f;
			}
			if(hidx != -1)
				InvalidateRect(hWnd, 0, TRUE);
			if(!mouse_over)
			{
				TRACKMOUSEEVENT tre = {};
				tre.cbSize = sizeof(tre);
				tre.dwFlags = TME_LEAVE | TME_HOVER;
				tre.hwndTrack = hWnd;
				TrackMouseEvent(&tre);
			}
			break;
		}
	case WM_MOUSEHOVER:
		mouse_over = true;
		break;
	case WM_MOUSELEAVE:
		mouse_over = false;
		hidx = -1;
		InvalidateRect(hWnd, 0, TRUE);
		mouse_down = false;
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_LBUTTONUP:
		if(mouse_down)
		{
			bool risen = false;
			float y = 4.f;
			for(unsigned i = 0; i < childs.size(); ++i)
			{
				menu_strip* strip = childs[i];
				if(rect(0, y, get_hsize().width, strip->get_height()).contains(point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)))) && strip->get_enabled())
				{
					if(strip->get_checkable())
						strip->set_checked(!strip->get_checked());
					strip->click(i);
					risen = true;
					if(strip->hs_childs)
					{
						risen = false;
						if(i == showed_idx && child_shown)
						{
							strip->close(true);
							opened = false;
							child_shown = false;
						}
						else if(child_shown)
						{
							if(childs.size() > showed_idx)
							{
								childs[showed_idx]->close(true);
								opened = false;
								child_shown = false;
							}
						}
						else
						{
							RECT rc;
							GetWindowRect(handle, &rc);
							child_shown = true;
							InvalidateRect(hWnd, 0, TRUE);
							strip->show(point(static_cast<float>(rc.right), static_cast<float>(rc.top + y-4)), false);
							showed_idx = i;
						}
					}
					break;
				}
				y += strip->get_height()+4.f;
			}
			if(!risen)
				break;
		}
		else
		{
			mouse_down = false;
			break;
		}
	case WM_KILLFOCUS:
		{
			wchar_t buff[256]; 
			GetClassName(reinterpret_cast<HWND>(wParam), buff, 255);
			wstring clname(buff);
			if(clname.find(L"GCL_Menustrip") == wstring::npos && clname.find(L"GCL_Contextmenu") == wstring::npos)
			{
				close(true);
				opened = false;
			}
			break;
		}
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_DOWN:
				if(++hidx >= static_cast<int>(childs.size()))
					hidx = min(childs.size()-1, 0u);
				break;
			case VK_UP:
				if(--hidx < 0)
					hidx = childs.size()-1;
				break;
			case VK_RETURN:
				{
					if(hidx >= static_cast<int>(childs.size()) || hidx < 0)
						break;
					auto strip = childs[hidx];
					if(strip->get_checkable())
						strip->set_checked(!strip->get_checked());
					strip->click(hidx);
					if(!strip->hs_childs)
					{
						close(true);
						opened = false;
						break;
					}
				}
			case VK_RIGHT:
				{
					if(hidx >= static_cast<int>(childs.size()) || hidx < 0)
						break;
					auto strip = childs[hidx];
					if(strip->hs_childs && strip->enabled)
					{
						RECT rc;
						GetWindowRect(handle, &rc);
						child_shown = true;
						InvalidateRect(hWnd, 0, TRUE);
						float y = 4.f;
						for(int i = 0; i < hidx; ++i)
							y += childs[i]->get_height()+4.f;
						strip->show(point(static_cast<float>(rc.right), static_cast<float>(rc.top + y-4)), true);
						showed_idx = hidx;
					}
					break;
				}
			case VK_APPS:
				close(true);
				opened = false;
			}
			InvalidateRect(hWnd, 0, TRUE);
			break;
		}
	case WM_SETFOCUS:
		focus = true;
		break;
	case WM_LBUTTONDOWN:
		mouse_down = true;
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		closed();
		opened = true;
		UnregisterClass(classname.c_str(), GetModuleHandle(0));
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void context_menu::close(bool parent)
{
	ShowWindow(handle, SW_HIDE);
	opened = false;
	if(child_shown && childs.size() > showed_idx)
	{
		childs[showed_idx]->close(true);
		child_shown = false;
	}
	closed();
}

LRESULT CALLBACK context_menu::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	context_menu* inst = 0;
	if(msg == WM_CREATE)
	{
		TRACKMOUSEEVENT tre = {};
		tre.cbSize = sizeof(tre);
		tre.dwFlags = TME_LEAVE | TME_HOVER;
		tre.hwndTrack = hWnd;
		TrackMouseEvent(&tre);
		inst = static_cast<context_menu*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(inst));
	}
	else
	{
		inst = reinterpret_cast<context_menu*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if(!inst) return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return inst->message_received(hWnd, msg, wParam, lParam);
}
// Contextmenu

// MenuStripMain
void menu_strip_m::set_title(const wstring& s)
{
	if(!change_if_diff(title, s))
		return;
	if(owner)
		owner->redraw();
}

void menu_strip_m::set_menu(context_menu* menu)
{
	if(mmenu.get() == menu)
		return;
	mmenu.reset(menu);
	if(owner)
		owner->redraw();
}

void menu_strip_m::set_enabled(bool b)
{
	if(!change_if_diff(enabled, b))
		return;
	if(owner)
		owner->redraw();
}

void menu_strip_m::measure(graphics* g, const point& p)
{
	rect rc = owner->get_font()->get_metrics(get_title(), size::max_size(), g);
	rc.position = p;
	bounds = rc;
}
// MenuStripMain

// Menu
menu::menu()
{
	set_captures_keyboard(false);
	set_min_size(size(25, 25));
	cl_back = colour::gcl_dark_gray;
	cl_font = colour::white;
	cl_hot = colour::gcl_gray;
	cl_down = colour::black;
	cl_gray = colour::gray;
	space = 15.f;
	hidex = -1;
	mouse_move += make_func_ptr(this, &menu::this_mouse_move);
	mouse_leave += make_func_ptr(this, &menu::this_mouse_leave);
	mouse_up += make_func_ptr(this, &menu::this_mouse_up);
	mouse_down += make_func_ptr(this, &menu::this_mouse_down);
}

menu::~menu()
{
	mouse_move -= make_func_ptr(this, &menu::this_mouse_move);
	mouse_leave -= make_func_ptr(this, &menu::this_mouse_leave);
	mouse_up += make_func_ptr(this, &menu::this_mouse_up);
	mouse_down += make_func_ptr(this, &menu::this_mouse_down);
}

void menu::render(graphics* g)
{
	if(!has_resources())
		init_resources(g);
	g->push_clip(get_rect());
	float x = space;
	g->fill_rect(get_rect(), br_back.get());
	for(unsigned i = 0; i < strips.size(); ++i)
	{
		auto strip = strips[i];
		strip->measure(g, point(x+get_position().x, get_position().y));
		rect measure = strip->get_bounds();
		if(i == hidex && get_enabled() && strip->get_enabled())
		{
			g->fill_rect(strip->get_intersect_bounds(get_position().y, get_size().height, space), br_hot.get());
		}
		auto mnu = strip->get_menu();
		if(mnu && get_enabled() && strip->get_enabled())
		{
			if(mnu->is_shown())
				g->fill_rect(strip->get_intersect_bounds(get_position().y, get_size().height, space), br_down.get());
		}
		g->draw_string(strip->get_title(), point(x+get_position().x, get_position().y + get_size().height / 2.f - measure.get_height() / 2.f), get_font(), (get_enabled() && strip->get_enabled()) ? br_font.get() : br_gray.get());
		x += measure.get_width()+space;
	}
	g->pop_clip();
	dynamic_drawsurface::render(g);
}

void menu::set_opacity(float f)
{
	if(f == get_opacity())
		return;
	if(has_resources())
	{
		br_back->set_opacity(f);
		br_hot->set_opacity(f);
		br_down->set_opacity(f);
		br_font->set_opacity(f);
		br_gray->set_opacity(f);
	}
	dynamic_drawsurface::set_opacity(f);
}


void menu::create_resources(graphics* g)
{
	dynamic_drawsurface::create_resources(g);
	if(menu_graphics::is_high_contrast())
	{
		br_back = shared_ptr<solid_brush>(g->create_solid_brush(colour::menu));
		br_down = shared_ptr<solid_brush>(g->create_solid_brush(colour::menu));
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(colour::menu_text));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(colour::menu_highlight));
		br_gray = shared_ptr<solid_brush>(g->create_solid_brush(colour::gray_text));
	}
	else
	{
		br_back = shared_ptr<solid_brush>(g->create_solid_brush(cl_back));
		br_down = shared_ptr<solid_brush>(g->create_solid_brush(cl_down));
		br_font = shared_ptr<solid_brush>(g->create_solid_brush(cl_font));
		br_hot = shared_ptr<solid_brush>(g->create_solid_brush(cl_hot));
		br_gray = shared_ptr<solid_brush>(g->create_solid_brush(cl_gray));
	}
	if(get_opacity() != 1.f)
	{
		float f = get_opacity();
		br_back->set_opacity(f);
		br_hot->set_opacity(f);
		br_down->set_opacity(f);
		br_font->set_opacity(f);
		br_gray->set_opacity(f);
	}
}

void menu::this_mouse_move(const int mod, const point& p1)
{
	int idx = hidex;
	int opened_idx = get_opened();
	point p = p1;
	p.x += get_position().x;
	p.y += get_position().y;
	for(unsigned i = 0; i < strips.size(); ++i)
	{
		if(strips[i]->get_intersect_bounds(get_position().y, get_size().height, space).contains(p, get_absolute_transform()))
		{
			hidex = i;
			if(hidex != idx)
			{
				redraw(get_bounds());
				if(opened_idx != -1 && hidex != -1 && hidex != opened_idx && is_between_equ(idx, 0, static_cast<int>(strips.size()-1)))
				{
					auto strip = strips[hidex];
					if(!strip->get_enabled())
						break;
					strips[idx]->get_menu()->close(true);
					POINT pp = {static_cast<int>(strip->get_intersect_bounds(get_position().y, get_size().height, space).get_x()), static_cast<int>(get_position().y + get_size().height)};
					MapWindowPoints(get_owner()->get_handle(), GetDesktopWindow(), &pp, 1);
					strips[opened_idx]->get_menu()->close(true);
					if(strip->get_menu())
						strip->get_menu()->show(point(static_cast<float>(pp.x), static_cast<float>(pp.y)));
				}
			}
			return;
		}
	}
	hidex = -1;
	if(hidex != idx)
		redraw(get_bounds());
}

void menu::this_mouse_leave(const point&)
{
	if(hidex != -1)
	{
		hidex = -1;
		redraw(get_bounds());
	}
}

void menu::this_mouse_up(const mouse_buttons& mb, int modd, const point& p)
{
	if(hidex == -1 || mb != mouse_buttons::left || !get_enabled())
		return;
	auto strip = strips[hidex];
	if(!strip->get_enabled())
		return;
	POINT pp = {static_cast<int>(strip->get_intersect_bounds(get_position().y, get_size().height, space).get_x()), static_cast<int>(get_position().y + get_size().height)};
	MapWindowPoints(get_owner()->get_handle(), GetDesktopWindow(), &pp, 1);
	if(strip->get_menu())
		strip->get_menu()->show(point(static_cast<float>(pp.x), static_cast<float>(pp.y)));
}

int menu::get_opened()
{
	for(unsigned i = 0; i < strips.size(); ++i)
	{
		auto mnu = strips[i]->get_menu();
		if(mnu)
		{
			if(mnu->is_shown())
				return i;
		}
	}
	return -1;
}

void menu::add_strip(menu_strip_m* strip)
{
	if(!strip->owner)
		strip->owner = this;
	strips.push_back(strip);
	redraw();
}

void menu::remove_strip(menu_strip_m* strip)
{
	strip->owner = 0;
	strips.erase(remove(strips.begin(), strips.end(), strip));
	redraw();
}

void menu::clear_strips()
{
	strips.clear();
	redraw();
}

void menu::on_syscolour_changed()
{
	if(menu_graphics::is_high_contrast())
	{
		br_back->set_colour(colour::menu);
		br_down->set_colour(colour::menu);
		br_font->set_colour(colour::menu_text);
		br_hot ->set_colour(colour::menu_highlight);
		br_gray->set_colour(colour::gray_text);
	}		   
	else	   
	{		   
		br_back->set_colour(cl_back);
		br_down->set_colour(cl_down);
		br_font->set_colour(cl_font);
		br_hot ->set_colour(cl_hot);
		br_gray->set_colour(cl_gray);
	}
	dynamic_drawsurface::on_syscolour_changed();
}

void menu::set_back_colour(const colour& c)
{
	if(!change_if_diff(cl_back, c))
		return;
	if(has_resources())
		br_back->set_colour(cl_back);
	back_colour_changed(cl_back);
}

void menu::set_hot_colour(const colour& c)
{
	if(!change_if_diff(cl_hot, c))
		return;
	if(has_resources())
		br_hot->set_colour(cl_hot);
	hot_colour_changed(cl_hot);
}

void menu::set_pressed_colour(const colour& c)
{
	if(!change_if_diff(cl_down, c))
		return;
	if(has_resources())
		br_down->set_colour(cl_down);
	pressed_colour_changed(cl_down);
}

void menu::set_font_colour(const colour& c)
{
	if(!change_if_diff(cl_font, c))
		return;
	if(has_resources())
		br_font->set_colour(cl_font);
	font_colour_changed(cl_font);
}
// Menu

};
};