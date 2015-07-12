/*****************************************************************************
*							  GCL - io.h 	                                 *
*                      Copyright (C) F. Gausling                             *
*		Version 0.9.0 for more information and the full license visit	     *
*						 http://www.gcl-ui.com		                         *
*****************************************************************************/
#include "stdafx.h"

#ifndef IO_H
#define IO_H
#ifdef _MSC_VER 
#	pragma once
#endif
namespace gcl {
namespace io {

typedef KNOWNFOLDERID sh_folder_id;

namespace path
{
	wstring get_current_folder();
	wstring get_known_folder(sh_folder_id folder_id);
	wstring find_filename(const wstring& path);
	wstring find_extension(const wstring& path);
	void remove_blanks(wstring& path);
	void remove_extension(wstring& path);
	bool is_network(const wstring& path);
	bool is_directory(const wstring& path);
	bool exists(const wstring& path);
	wstring combine(const wstring& p1, const wstring& p2);
};
};
};
#endif