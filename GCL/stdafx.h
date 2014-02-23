/*****************************************************************************
*                           gcl - stdAfx.h                                   *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                                                                            *
*                       Precompiled Header                                   *
*****************************************************************************/
#ifndef RENDERER_STDAFX_H
#define RENDERER_STDAFX_H
#ifdef _MSC_VER
#pragma once
#endif

// Windows includes
#include <Windows.h>
#include <windowsx.h>

// Direct2D includes
#include <D2D1.h>
#include <d2d1helper.h>
// Add "D2D1.lib", "WindowsCodecs.lib" and "DWrite.lib" to the Linker
#ifdef _MSC_VER
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "WindowsCodecs.lib")
#pragma comment (lib, "DWrite.lib")
#endif
using namespace D2D1;

// GDI+ includes
#include <gdiplus.h>
// Add "GdiPlus.lib" and "ShlwAPI.lib"  to the Linker
#ifdef _MSC_VER
#pragma comment (lib, "GdiPlus.lib")
#pragma comment (lib, "ShlwAPI.lib")
#endif

// Std includes
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <string>
#include <ctime>
#include <future>
#include <thread>
#include <stdexcept>
#include <random>
#include <exception>
#include <utility>
#include <mutex>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <regex>
#include <list>
#include <chrono>
#include <locale>
using namespace std;

// Enables Visualstyles for Win32-dialogs
// If you are using another compiler, you need to add a manifest
#ifdef _MSC_VER
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#endif
