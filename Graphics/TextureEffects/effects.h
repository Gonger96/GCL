/*****************************************************************************
*                           gcl - effects.h                                 *
*                      Copyright (C) F. Gausling                             *
*    Version 0.0.0.1 Alpha for more information and the full license visit   *
*****************************************************************************/
#ifndef EFFECTS_H
#define EFFECTS_H
#ifdef _MSC_VER
#pragma once
#endif
#include "stdafx.h"
#include "graphics.h"
using namespace gcl::render_objects;

namespace gcl { namespace effects {

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

enum class direction_types {north, north_east, east, south_east, south, south_west, west, north_west};

// Standard colorfilter
namespace standard
{
	// Inverts a specified colorchannel
	void invert(texture* image, bool red, bool green, bool blue);
	// Grayscales the image
	void gray_scale(texture* image);
	// Multiplies the actual brightness with a given value
	void brightness(texture* image, float fac);
	// Multiplies the actual contrast with a given value
	void contrast(texture* image, float fac);
	// Sets a new Gammavalue for each colorchannel
	void gamma(texture* image, float red, float green, float blue);
	// Multiplies the colorchannels with the given values
	void color(texture* image, float red, float green, float blue);
	// Multiplies the Alphachannel with a given value
	void alpha(texture* image, float fac);
	// Sets the Alphachannel to a fixed value
	void alpha_fix(texture* image, unsigned char value);
};
// Convolutionfilter
namespace convolution
{
	template <int n>
	using unfixed_kernel = collection<float, n>;
	// A 3x3Matrix for convolution
	struct kernel
	{
	public:
		// Matrixmember
		float m[9];
		// Factor
		float factor;
		// Offset
		float offset;
		kernel(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33, float fac, float offs)
		{
			m[0] = m11;
			m[1] = m12;
			m[2] = m13;
			m[3] = m21;
			m[4] = m22;
			m[5] = m23;
			m[6] = m31;
			m[7] = m32;
			m[8] = m33;
			factor = fac;
			offset = offs;
		};
		kernel(float* m_, float fac, float offs)
		{
			for(int i = 0; i < 9; i++)
				m[i] = m_[i];
			factor = fac;
			offset = offs;
		};
		kernel()
		{
			for(int i = 0; i < 9; i++)
				m[i] = 0;
			m[4] = 1;
			factor = 1;
			offset = 0;
		};
	};
	// Calculates the imageconvolution
	void calc_convolution(texture* image, kernel& conv, bool normalise = false, bool cred = true, bool cgreen = true, bool cblue = true, bool calpha = true);

	// Returns a value which is between "min" and "max"
	template <typename t>
	inline const t& extend(const t& value, const t& min, const t& max)
	{
		if(value < min) return min;
		if(value > max) return max;
		return value;
	};
	// Divides two numbers and returns the rest in parameter "result"
	inline int div_rem(int a, int b, int& result)
	{
		result = a%b;
		return a/b;
	};
	// Makes the texture look sharpened
	void sharpen(texture* image, bool alpha);
	// Makes the texture look more sharpened
	void sharpen_more(texture* image, bool alpha);
	// Makes the texture look embossed
	void emboss(texture* image, bool alpha);
	// Makes the texture look embossed
	void emboss_laplascian(texture* image);
	// Makes the texture look embossed to a specified direction
	void emboss_direction(texture* image, bool vertical, bool horizontal);
	// Makes the texture look blurred
	void blur(texture* image, bool alpha);
	// Makes the texture look blurred
	void blur_more(texture* image, bool alpha);
	void edge_enhance(texture* image, const direction_types& direction);// und edgedetect IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
};

namespace displacement
{
	void calc_displacement(texture* image, point** offset);
};

};
};

#endif