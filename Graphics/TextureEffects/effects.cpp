#include "stdafx.h"
#include "effects.h"

namespace gcl { namespace effects {

// Std Filter

void standard::invert(texture* image, bool red, bool green, bool blue)
{
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			if(blue) data[0] = ~data[0];
			if(green) data[1] = ~data[1];
			if(red) data[2] = ~data[2];
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::gray_scale(texture* image)
{
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			data[0] = data[1] = data[2] = static_cast<unsigned char>(0.299f * data[2] + 0.587f * data[1] + 0.114f * data[0]);
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::brightness(texture* image, float fac)
{
	if(fac < 0) throw invalid_argument("Argument \"fac\" must be greater 0");
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			int b = static_cast<int>(data[0] * fac), g = static_cast<int>(data[1] * fac), r = static_cast<int>(data[2] * fac);
			b = std::min(255, b);
			g = std::min(255, g);
			r = std::min(255, r);
			data[0] = b;
			data[1] = g;
			data[2] = r;
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::contrast(texture* image, float fac)
{
	if(fac < 0 || fac > 2) throw invalid_argument("Argument \"fac\" must be greater 0 and less than 2");
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	fac *= fac;
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			int b = data[0], g = data[1], r = data[2];
			double pixel = 0;
            pixel = r / 255.0;
            pixel -= 0.5;
            pixel *= fac;
            pixel += 0.5;
            pixel *= 255;
            if (pixel < 0) pixel = 0;
            if (pixel > 255) pixel = 255;
            data[2] = static_cast<unsigned char>(pixel);

            pixel = g / 255.0;
            pixel -= 0.5;
            pixel *= fac;
            pixel += 0.5;
            pixel *= 255;
            if (pixel < 0) pixel = 0;
            if (pixel > 255) pixel = 255;
            data[1] = static_cast<unsigned char>(pixel);

            pixel = b / 255.0;
            pixel -= 0.5;
            pixel *= fac;
            pixel += 0.5;
            pixel *= 255;
            if (pixel < 0) pixel = 0;
            if (pixel > 255) pixel = 255;
            data[0] = static_cast<char>(pixel);
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::gamma(texture* image, float red, float green, float blue)
{
	if(red   < 0.2f || red   > 5 ||
	   green < 0.2f || green > 5 ||
	   blue  < 0.2f || blue  > 5)
	   throw invalid_argument("Parameter RGB must be less than 5 and greater than 0.2");
	unsigned char gRed[256] = {};
	unsigned char gGreen[256] = {};
	unsigned char gBlue[256] = {};
	for(int i = 0; i < 256; i++)
	{
		gRed[i] = std::min(255, static_cast<int>(255.f*std::powf(i / 255.f, 1.0f / red) + 0.5f));
		gGreen[i] = std::min(255, static_cast<int>(255.f*std::powf(i / 255.f, 1.0f / green) + 0.5f));
		gBlue[i] = std::min(255, static_cast<int>(255.f*std::powf(i / 255.f, 1.0f / blue) + 0.5f));
	}
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			data[2] = gRed[data[2]];
			data[1] = gGreen[data[1]];
			data[0] = gBlue[data[0]];
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::color(texture* image, float red, float green, float blue)
{
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	float pixel = 0;
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			pixel = data[0]*blue;
			pixel = std::max(0.f, pixel);
			data[0] = static_cast<unsigned char>(std::min(255.f, pixel));

			pixel = data[1]*green;
			pixel = std::max(0.f, pixel);
			data[1] = static_cast<unsigned char>(std::min(255.f, pixel));

			pixel = data[2]*red;
			pixel = std::max(0.f, pixel);
			data[2] = static_cast<unsigned char>(std::min(255.f, pixel));
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::alpha(texture* image, float fac)
{
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			float val = data[3] * fac;
			data[3] = static_cast<unsigned char>(std::min(255.f, val));
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

void standard::alpha_fix(texture* image, unsigned char value)
{
	unsigned char* data = image->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			data[3] = value;
			data += 4;
		}
		data += stride - width*4;
	}
	image->free();
}

// Convolution
void convolution::calc_convolution(texture* image, kernel& conv, bool normalise, bool cred, bool cgreen, bool cblue, bool calpha)
{
	if(conv.factor == 0 || !image)
		throw invalid_argument("");
	texture* bmp = image->clone();
	unsigned char* data = image->alloc();
	unsigned char* ndata = bmp->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	float alpha = 0;
    float red = 0;
    float green = 0;
    float blue = 0;

	if(normalise)
	{
		conv.factor = conv.m[0] +
			      conv.m[1] +
				  conv.m[2] +
				  conv.m[3] +
				  conv.m[4] +
				  conv.m[5] +
				  conv.m[6] +
				  conv.m[7] +
				  conv.m[8];
	}

	for(int i = 0; i < width*height; i++)
	{
		int x; int y = div_rem(i, width, x);
		alpha = 0;
        red = 0;
        green = 0;
        blue = 0;
        for (int kernelX = 0, sourceX = x - 1; kernelX < 3; kernelX++, sourceX++)
        {
            for (int kernelY = 0, sourceY = y - 1; kernelY < 3; kernelY++, sourceY++)
            {
                unsigned char* color = ndata + extend(sourceY, 0, height - 1) * stride + extend(sourceX, 0, width - 1) * 4;
                float factor = conv.m[kernelX+(kernelY*3)];
                alpha += color[3] * factor;
                red += color[2] * factor;
                green += color[1] * factor;
                blue += color[0] * factor;
            }
        }
		blue = blue / conv.factor + conv.offset;
		if(normalise && blue <= 0) blue == 0 ? blue += 128 : blue += 255;
        green = green / conv.factor + conv.offset;
		if(normalise && green <= 0) green == 0 ? green += 128 : green += 255;
        red = red / conv.factor + conv.offset;
		if(normalise && red <= 0) red == 0 ? red += 128 : red += 255;
        alpha = alpha / conv.factor + conv.offset;
		if(normalise && alpha <= 0) alpha == 0 ? alpha += 128 : alpha += 255;

        unsigned char* destinationPointer = data + i * 4;
		if(cblue)destinationPointer[0] = std::max(std::min(255, static_cast<int>(blue)), 0);
		if(cgreen)destinationPointer[1] = std::max(std::min(255, static_cast<int>(green)), 0);
		if(cred)destinationPointer[2] = std::max(std::min(255, static_cast<int>(red)), 0);
		if(calpha)destinationPointer[3] = std::max(std::min(255, static_cast<int>(alpha)), 0);
	}

	bmp->free();
	bmp->release_resources();
	image->free();
}

void convolution::sharpen(texture* image, bool alpha)
{
	calc_convolution(image, kernel(0, -1, 0, -1, 5, -1, 0, -1, 0, 1, 0), false, true, true, true, alpha);
}

void convolution::sharpen_more(texture* image, bool alpha)
{
	calc_convolution(image, kernel(-1, -1, -1, -1, 9, -1, -1, -1, -1, 1, 0), false, true, true, true, alpha);
}

void convolution::emboss(texture* image, bool alpha)
{
	calc_convolution(image, kernel(-2, -1, 0, -1, 1, 1, 0, 1, 2, 1, 0), false, true, true, true, alpha);
}

void convolution::emboss_laplascian(texture* image)
{
	calc_convolution(image, kernel(-1, 0, -1, 0, 4, 0, -1, 0, -1, 1, 127), false, true, true, true, false);
}

void convolution::emboss_direction(texture* image, bool vertical, bool horizontal)
{
	if(horizontal && vertical)
		calc_convolution(image, kernel(0, -1, 0, -1, 4, -1, 0, -1, 0, 1, 127), false, true, true, true, false);
	else if(vertical)
		calc_convolution(image, kernel(0, -1, 0, 0, 0, 0, 0, 1, 0, 1, 127), false, true, true, true, false);
	else if(horizontal)
		calc_convolution(image, kernel(0, 0, 0, -1, 2, -1, 0, 0, 0, 1, 127), false, true, true, true, false);
}

void convolution::blur(texture* image, bool alpha)
{
	calc_convolution(image, kernel(1, 2, 1, 2, 4, 2, 1, 2, 1, 1, 0), true, true, true, true, alpha);
}

void convolution::blur_more(texture* image, bool alpha)
{
	calc_convolution(image, kernel(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0), true, true, true, true, alpha);
}

void convolution::edge_enhance(texture* image, const direction_types& direction)
{
	switch(direction)
	{
	case direction_types::north:
		calc_convolution(image, kernel(0, -1, 0, 0, 1, 0, 0, 0, 0, 1, 0), false, true, true, true, false);
		break;
	case direction_types::north_east:
		calc_convolution(image, kernel(0, 0, -1, 0, 1, 0, 0, 0, 0, 1, 0), false, true, true, true, false);
		break;
	case direction_types::east:
		calc_convolution(image, kernel(0, 0, 0, 0, 1, -1, 0, 0, 0, 1, 0), false, true, true, true, false);
		break;
	case direction_types::south_east:
		calc_convolution(image, kernel(0, 0, 0, 0, 1, 0, 0, 0, -1, 1, 0), false, true, true, true, false);
		break;
	case direction_types::south:
		calc_convolution(image, kernel(0, 0, 0, 0, 1, 0, 0, -1, 0, 1, 0), false, true, true, true, false);
		break;
	case direction_types::south_west:
		calc_convolution(image, kernel(0, 0, 0, 0, 1, 0, -1, 0, 0, 1, 0), false, true, true, true, false);
		break;
	case direction_types::west:
		calc_convolution(image, kernel(0, 0, 0, -1, 1, 0, 0, 0, 0, 1, 0), false, true, true, true, false);
		break;
	case direction_types::north_west:
		calc_convolution(image, kernel(-1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0), false, true, true, true, false);
		break;
	}
}

// Displacement
void displacement::calc_displacement(texture* image, point** offset)
{
	if(!image || !offset)
		throw invalid_argument("");
	texture* bmp = image->clone();
	unsigned char* data = image->alloc();
	unsigned char* ndata = bmp->alloc();
	int stride = image->get_stride();
	int width = image->get_width();
	int height = image->get_height();
	int currOffs = stride - width*4;
	int xO, yO;
	for(int i = 0; i < width*height; i++)
	{
		int x; int y = convolution::div_rem(i, width, x);
		xO = static_cast<int>(offset[x][y].x);
		yO = static_cast<int>(offset[x][y].y);
        image[i] = bmp[(y+yO)*stride + (x+xO)*3];
        image[i+1] = bmp[(y+yO)*stride + (x+xO)*3 +1];
        image[i+2] = bmp[(y+yO)*stride + (x+xO)*3 +2];
	}
	bmp->free();
	bmp->release_resources();
	image->free();
}

};
};