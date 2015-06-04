#include "stdafx.h"
#include "io.h"

// Path
namespace gcl { namespace io {

wstring path::get_current_folder()
{
	wchar_t pth[MAX_PATH + 1];
	GetCurrentDirectoryW(MAX_PATH, pth);
	return wstring(pth);
}
wstring path::get_known_folder(sh_folder_id folder_id)
{
	wchar_t* out_path = 0;
	if(SHGetKnownFolderPath(folder_id, 0, 0, &out_path) != S_OK)
		throw runtime_error("Folder doesn't exist");
	wstring path(out_path);
	CoTaskMemFree(out_path);
	return path;
}
wstring path::find_filename(const wstring& path)
{
	auto pth = PathFindFileName(path.c_str());
	if(pth)
		return wstring(pth);
	else
		throw invalid_argument("No filename available");
}
wstring path::find_extension(const wstring& path)
{
	auto pth = PathFindExtension(path.c_str());
	if (pth)
		return wstring(pth);
	else
		throw invalid_argument("No filename available");
}
void path::remove_blanks(wstring& path) { PathRemoveBlanks(const_cast<wchar_t*>(path.c_str())); }
void path::remove_extension(wstring& path) { PathRemoveExtension(const_cast<wchar_t*>(path.c_str())); }
bool path::is_network(const wstring& path) { return !!PathIsNetworkPath(path.c_str()); }
bool path::is_directory(const wstring& path) { return !!PathIsDirectory(path.c_str()); }
bool path::exists(const wstring& path) { return !!PathFileExists(path.c_str()); }
wstring path::combine(const wstring& p1, const wstring& p2)
{
	wchar_t path_out[MAX_PATH+1];
	if(PathCombine(path_out, p1.c_str(), p2.c_str()) == 0)
		throw invalid_argument("");
	return wstring(path_out);
}
// Path
};
};