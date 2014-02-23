#include "stdafx.h"
#include "win32_dlgs.h"

namespace gcl { namespace ui { namespace msg_box {

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


int show(const wstring& text, const wstring& caption, unsigned int styles, HWND handle)
{
	int ret = MessageBoxW(handle, text.c_str(), caption.c_str(), styles);
	if(ret == 0)
		throw invalid_argument("MessageBox failed (" + to_string(GetLastError()) + ")");
	return ret;
}

};
};
};