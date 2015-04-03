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

// ColourPicker
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
// ColourPicker

// Filedialog
bool file_dlg::show(HWND hWnd)
{
	if(FAILED(opf->Show(hWnd)))
		return false;
	return true;
}

void file_dlg::close()
{
	if(FAILED(opf->Close(S_OK)))
		throw runtime_error("Unable to close IFileDialog");
}

wstring file_dlg::get_result()
{
	IShellItem* itm;
	if(FAILED(opf->GetResult(&itm)))
		throw runtime_error("Unable to get result");
	wchar_t* fn;
	if(FAILED(itm->GetDisplayName(SIGDN_FILESYSPATH, &fn)))
		throw runtime_error("Unable to get result");
	itm->Release();
	wstring fname(fn);
	CoTaskMemFree(fn);
	return fname;
}

IShellItem* file_dlg::get_result_item()
{
	IShellItem* itm = 0;
	if(FAILED(opf->GetResult(&itm)))
		throw runtime_error("Unable to get result");
	return itm;
}

int file_dlg::get_file_type_index()
{
	UINT idx = 0;
	if(FAILED(opf->GetFileTypeIndex(&idx)))
		throw runtime_error("Unable to get filetype");
	return idx;
}

void file_dlg::set_default_extension(const wstring& ext)
{
	if(FAILED(opf->SetDefaultExtension(ext.c_str())))
		throw invalid_argument("Parameter \"ext\" is invalid");
}

void file_dlg::set_default_folder(const wstring& fn)
{
	IShellItem* itm;
	if(FAILED(SHCreateShellItem(0, 0, SHSimpleIDListFromPath(fn.c_str()), &itm)))
		throw invalid_argument("Parameter \"fn\" is invalid");
	if(FAILED(opf->SetDefaultFolder(itm)))
		throw runtime_error("Unable to set default folder");
	itm->Release();
}

void file_dlg::set_default_folder(IShellItem* itm)
{
	if(FAILED(opf->SetDefaultFolder(itm)))
		throw runtime_error("Unable to set default folder");
}

void file_dlg::set_file_types(file_type* bgn, file_type* end)
{
	int cnt = end-bgn;
	if(cnt<1)
		throw invalid_argument("Invalid pointer");
	COMDLG_FILTERSPEC* specs = new COMDLG_FILTERSPEC[cnt];
	for(int i = 0; i < cnt; i++)
		specs[i] = (bgn+i)->get_filter_spec();
	if(FAILED(opf->SetFileTypes(cnt, specs)))
		throw runtime_error("Unable to set filetypes");
	delete[] specs;
}

void file_dlg::set_filename(const wstring& fn)
{
	if(FAILED(opf->SetFileName(fn.c_str())))
		throw runtime_error("Unable to set filename");
}

void file_dlg::set_filename_label(const wstring& txt)
{
	if(FAILED(opf->SetFileNameLabel(txt.c_str())))
		throw runtime_error("Unable to set filename");
}

void file_dlg::set_ok_button_text(const wstring& txt)
{
	if(FAILED(opf->SetOkButtonLabel(txt.c_str())))
		throw runtime_error("Unable to set filename");
}

void file_dlg::set_title(const wstring& txt)
{
	if(FAILED(opf->SetTitle(txt.c_str())))
		throw runtime_error("Unable to set filename");
}

void file_dlg::set_options(FILEOPENDIALOGOPTIONS optns)
{
	if(FAILED(opf->SetOptions(optns)))
		throw runtime_error("Unable to set options");
}

FILEOPENDIALOGOPTIONS file_dlg::get_options()
{
	FILEOPENDIALOGOPTIONS ops;
	if(FAILED(opf->GetOptions(&ops)))
		throw runtime_error("Unable to get options");
	return ops;
}

void file_dlg::add_place(const wstring& filename, bool top)
{
	IShellItem* itm;
	if(FAILED(SHCreateShellItem(0, 0, SHSimpleIDListFromPath(filename.c_str()), &itm)))
		throw invalid_argument("Parameter \"filename\" is invalid");
	if(FAILED(opf->AddPlace(itm, top ? FDAP_TOP : FDAP_BOTTOM)))
		throw runtime_error("Unable to add place");
	itm->Release();
}

void file_dlg::add_place(IShellItem* itm, bool top)
{
	if(FAILED(opf->AddPlace(itm, top ? FDAP_TOP : FDAP_BOTTOM)))
		throw runtime_error("Unable to add place");
}
// Filedialog

// FilePicker
file_picker::file_picker() : file_dlg()
{
	if(FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg))))
		throw runtime_error("Unable to create FileOpenDialog");
	set(dlg);
}

IShellItemArray* file_picker::get_results()
{
	IShellItemArray* arr;
	if(FAILED(dlg->GetResults(&arr)))
		throw runtime_error("Unable to get results");
	return arr;
}
// FilePicker

// FileSaver
file_saver::file_saver() : file_dlg()
{
	if(FAILED(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg))))
		throw runtime_error("Unable to create FileOpenDialog");
	set(dlg);
}
// FileSaver

};
};