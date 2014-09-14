#include "stdafx.h"
#include "win32_dlgs.h"

namespace gcl { namespace ui { namespace msg_box {

// MessageBox
msg_results show(const wstring& text, const wstring& caption, const buttons& btn, const icons& ico, const def_buttons& def, const display_options& options)
{
	int ret = MessageBoxW(0, text.c_str(), caption.c_str(), btn | ico | def | options);
	if(ret == 0)
		throw invalid_argument("MessageBox failed (" + to_string(GetLastError()) + ")");
	return static_cast<msg_results>(ret);
}

msg_results show(HWND handle, const wstring& text, const wstring& caption, const buttons& btn, const icons& ico, const def_buttons& def, const display_options& options)
{
	int ret = MessageBoxW(0, text.c_str(), caption.c_str(), btn | ico | def | options);
	if(ret == 0)
		throw invalid_argument("MessageBox failed (" + to_string(GetLastError()) + ")");
	return static_cast<msg_results>(ret);
}

msg_results show(window* parent, const wstring& text, const wstring& caption, const buttons& btn, const icons& ico, bool use_handle, const def_buttons& def, const display_options& options)
{
	parent->set_enabled(false);
	int ret = MessageBoxW(use_handle ? parent->get_handle() : 0, text.c_str(), caption.c_str(), btn | ico | def | options);
	if(ret == 0)
		throw invalid_argument("MessageBox failed (" + to_string(GetLastError()) + ")");
	parent->set_enabled(true);
	parent->set_focus(true);
	return static_cast<msg_results>(ret);
}

int show(const wstring& text, const wstring& caption, unsigned int styles, HWND handle)
{
	int ret = MessageBoxW(handle, text.c_str(), caption.c_str(), styles);
	if(ret == 0)
		throw invalid_argument("MessageBox failed (" + to_string(GetLastError()) + ")");
	return ret;
}
};
// Messagebox

colour_picker::colour_picker()
{
	clr.lStructSize = sizeof(CHOOSECOLOR);
	clr.rgbResult = 0;
	clr.Flags = CC_RGBINIT;
	clr.lpCustColors = new COLORREF[16];
	clr.lpTemplateName = 0;
	clr.hInstance = 0;
	clr.lCustData = 0;
	clr.lpfnHook = 0;
}

colour_picker::~colour_picker()
{
	delete[] clr.lpCustColors;
}

msg_results colour_picker::show(window* owner, bool use_handle)
{
	clr.hwndOwner = 0;
	if(owner)
	{
		if(use_handle)
			clr.hwndOwner = owner->get_handle();
		owner->set_enabled(false);
	}
	msg_results res = ChooseColor(&clr) ? msg_results::_ok : msg_results::cancel;
	if(owner)
	{
		owner->set_enabled(true);
		owner->set_focus(true);
	}
	return res;
}

colour* colour_picker::get_custom_colours() const
{
	colour* cl = new colour[16];
	for(int i = 0; i < 16; i++)
		cl[i] = colour(GetRValue(clr.lpCustColors[i]), GetGValue(clr.lpCustColors[i]), GetBValue(clr.lpCustColors[i]));
	return cl;
}

};
};