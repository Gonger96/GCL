/*****************************************************************************
*                           gcl - input.h                                    *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*                   http://github.com/Gonger96/GCL                           *
*****************************************************************************/
#ifndef INPUT_H
#define INPUT_H
#ifdef _MSC_VER
#pragma once
#endif
#include "stdafx.h"
#include "graphics.h"
using namespace gcl;

namespace gcl { namespace input {

	namespace mouse {
	
	point get_position();
	point get_client_position(HWND client);
	void set_position(const point& p);
	void set_client_position(HWND client, const point& p);
	void hide();
	void show();

	};

}; 
};

#endif