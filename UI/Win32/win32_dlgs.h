/*****************************************************************************
*                           gcl - win32_dlgs.h                               *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#include "stdafx.h"
#include "window.h"

#ifndef WIN32DLGS_H
#define WIN32DLGS_H
#ifdef _MSC_VER 
#	pragma once
#endif

namespace gcl { namespace ui {

enum class msg_results {_ok = IDOK, cancel = IDCANCEL, abort = IDABORT, retry = IDRETRY, ignore = IDIGNORE, yes = IDYES, no = IDNO, try_again = IDTRYAGAIN, _continue = IDCONTINUE};
namespace msg_box
{
	enum icons : unsigned long {none, error = MB_ICONERROR, hand = MB_ICONHAND, stop = MB_ICONSTOP, question = MB_ICONQUESTION, exclamation = MB_ICONEXCLAMATION, warning = MB_ICONWARNING, info = MB_ICONINFORMATION, asterisk = MB_ICONASTERISK};
	enum display_options : unsigned long {def_options = 0, def_desktop_only = MB_DEFAULT_DESKTOP_ONLY, align_right = MB_RIGHT, rtl_text = MB_RTLREADING, serice_notify = MB_SERVICE_NOTIFICATION, top_most = MB_TOPMOST, set_foreground = MB_SETFOREGROUND};
	enum def_buttons : unsigned long {button_1 = MB_DEFBUTTON1, button_2 = MB_DEFBUTTON2, button_3 = MB_DEFBUTTON3, button_4 = MB_DEFBUTTON4};
	enum buttons : unsigned long {ok = MB_OK, ok_cancel = MB_OKCANCEL, abort_retry_ignore = MB_ABORTRETRYIGNORE, yes_no_cancel = MB_YESNOCANCEL, yes_no = MB_YESNO, retry_cancel = MB_RETRYCANCEL};
	msg_results show(const wstring& text, const wstring& caption = L"", const buttons& btn = ok, const icons& ico = none, const def_buttons& def = button_1, const display_options& options = def_options);
	msg_results show(HWND handle, const wstring& text, const wstring& caption = L"", const buttons& btn = ok, const icons& ico = none, const def_buttons& def = button_1, const display_options& options = def_options);
	msg_results show(window* parent, const wstring& text, const wstring& caption = L"", const buttons& btn = ok, const icons& ico = none, bool use_handle = true, const def_buttons& def = button_1, const display_options& options = def_options);
	int show(const wstring& text, const wstring& caption, unsigned int styles = 0, HWND handle = 0);
};

inline msg_box::display_options operator|(const msg_box::display_options& act, const msg_box::display_options& n_f)
{
	return static_cast<msg_box::display_options>(act | n_f);
}

inline msg_box::display_options operator&(const msg_box::display_options& act, const msg_box::display_options& n_f)
{
	return static_cast<msg_box::display_options>(act & n_f);
}

inline msg_box::display_options operator^(const msg_box::display_options& act, const msg_box::display_options& n_f)
{
	return static_cast<msg_box::display_options>(act ^ n_f);
}

class colour_picker
{
public:
	colour_picker();
	~colour_picker();
	msg_results show(window* owner = 0, bool use_handle = false);

	HWND get_owner_handle() const {return clr.hwndOwner;}
	colour get_colour() const {return colour(GetRValue(clr.rgbResult), GetGValue(clr.rgbResult), GetBValue(clr.rgbResult));}
	void set_colour(const colour& cl) {clr.rgbResult = RGB(cl.r, cl.g, cl.b);}
	// Returns an array of 16 userdefined colours (Memory needs to be freed)
	colour* get_custom_colours() const;
	bool get_use_any_colour() const {return ((clr.Flags & CC_ANYCOLOR) == CC_ANYCOLOR);}
	void set_use_any_colour(bool b) {b ? clr.Flags |= CC_ANYCOLOR : clr.Flags &= ~CC_ANYCOLOR;}
	bool get_full_open() const {return ((clr.Flags & CC_FULLOPEN) == CC_FULLOPEN);}
	void set_full_open(bool b) {b ? clr.Flags |= CC_FULLOPEN : clr.Flags &= ~CC_FULLOPEN;}
	bool get_prevent_full_open() const {return ((clr.Flags & CC_PREVENTFULLOPEN) == CC_PREVENTFULLOPEN);}
	void set_prevent_full_open(bool b) {b ? clr.Flags |= CC_PREVENTFULLOPEN : clr.Flags &= ~CC_PREVENTFULLOPEN;}
	bool get_only_solid_colours() const {return ((clr.Flags & CC_SOLIDCOLOR) == CC_SOLIDCOLOR);}
	void set_only_solid_colours(bool b) {b ? clr.Flags |= CC_SOLIDCOLOR : clr.Flags &= ~CC_SOLIDCOLOR;}
protected:
	CHOOSECOLOR clr;
};

struct file_type
{
	wstring descriptor, filter;
	file_type(const wstring& desc, const wstring& filtr) : descriptor(desc), filter(filtr) {}
	COMDLG_FILTERSPEC get_filter_spec() const
	{
		COMDLG_FILTERSPEC spec;
		spec.pszName = descriptor.c_str();
		spec.pszSpec = filter.c_str();
		return spec;
	}
};

class file_dlg
{
public:
	virtual ~file_dlg() {opf->Release();}
	bool show(HWND hWnd = 0);
	void close();
	wstring get_result();
	IShellItem* get_result_item();
	int get_file_type_index();
	void set_default_extension(const wstring& ext);
	void set_default_folder(const wstring& fn);
	void set_default_folder(IShellItem* itm);
	void set_file_types(file_type* bgn, file_type* end);
	void set_filename(const wstring& fn);
	void set_filename_label(const wstring& txt);
	void set_ok_button_text(const wstring& txt);
	void set_title(const wstring& txt);
	void set_options(FILEOPENDIALOGOPTIONS optns);
	void add_place(const wstring& filename, bool top);
	void add_place(IShellItem* itm, bool top);
	FILEOPENDIALOGOPTIONS get_options();
protected:
	file_dlg() {}
	void set(IFileDialog* dlg) {opf = dlg;}
	IFileDialog* opf;
};

class file_picker :
	public file_dlg
{
public:
	file_picker();
	virtual ~file_picker() {}
	IShellItemArray* get_results();
	IFileOpenDialog* get_dialog() const {return dlg;}
protected:
	IFileOpenDialog* dlg;
};

class file_saver :
	public file_dlg
{
public:
	file_saver();
	virtual ~file_saver() {}
protected:
	IFileSaveDialog* dlg;
};

};
};

#endif
