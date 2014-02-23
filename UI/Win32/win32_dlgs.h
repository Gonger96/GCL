/*****************************************************************************
*                        gcl - win32_dlgs.h                                  *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*****************************************************************************/
#ifndef WIN32DLGS_H
#define WIN32DLGS_H
#ifdef _MSC_VER 
#pragma once
#endif
#include "stdafx.h"

namespace gcl { namespace ui {

namespace msg_box
{
	enum icons : unsigned long {none, error = MB_ICONERROR, hand = MB_ICONHAND, stop = MB_ICONSTOP, question = MB_ICONQUESTION, exclamation = MB_ICONEXCLAMATION, warning = MB_ICONWARNING, info = MB_ICONINFORMATION, asterisk = MB_ICONASTERISK};
	enum display_options : unsigned long {def_options = 0, def_desktop_only = MB_DEFAULT_DESKTOP_ONLY, align_right = MB_RIGHT, rtl_text = MB_RTLREADING, serice_notify = MB_SERVICE_NOTIFICATION, top_most = MB_TOPMOST, set_foreground = MB_SETFOREGROUND};
	enum def_buttons : unsigned long {button_1 = MB_DEFBUTTON1, button_2 = MB_DEFBUTTON2, button_3 = MB_DEFBUTTON3, button_4 = MB_DEFBUTTON4};
	enum buttons : unsigned long {ok = MB_OK, ok_cancel = MB_OKCANCEL, abort_retry_ignore = MB_ABORTRETRYIGNORE, yes_no_cancel = MB_YESNOCANCEL, yes_no = MB_YESNO, retry_cancel = MB_RETRYCANCEL};
	enum class msg_results {_ok = IDOK, cancel = IDCANCEL, abort = IDABORT, retry = IDRETRY, ignore = IDIGNORE, yes = IDYES, no = IDNO, try_again = IDTRYAGAIN, _continue = IDCONTINUE};
	msg_results show(const wstring& text, const wstring& caption = L"", const buttons& btn = ok, const icons& ico = none, const def_buttons& def = button_1, const display_options& options = def_options);
	msg_results show(HWND handle, const wstring& text, const wstring& caption = L"", const buttons& btn = ok, const icons& ico = none, const def_buttons& def = button_1, const display_options& options = def_options);
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


};
};

#endif
