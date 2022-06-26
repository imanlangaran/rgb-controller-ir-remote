
#include "ColorConverterLib.h"

void RGBConverter::RgbToHsv(uint8_t red, uint8_t green, uint8_t blue, double& hue)
{
	auto rd = static_cast<double>(red) / 255;
	auto gd = static_cast<double>(green) / 255;
	auto bd = static_cast<double>(blue) / 255;
	auto max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
	 

	auto d = max - min;

	hue = 0;
	if (max != min)
	{
		if (max == rd)
		{
			hue = (gd - bd) / d + (gd < bd ? 6 : 0);
		}
		else if (max == gd)
		{
			hue = (bd - rd) / d + 2;
		}
		else if (max == bd)
		{
			hue = (rd - gd) / d + 4;
		}
		hue /= 6;
	}
}



void RGBConverter::HsvToRgb(double hue, double saturation, double value, float& red, float& green, float& blue)
{
	double r, g, b;

	auto i = static_cast<int>(hue * 6);
	auto f = hue * 6 - i;
	auto p = value * (1 - saturation);
	auto q = value * (1 - f * saturation);
	auto t = value * (1 - (1 - f) * saturation);

	switch (i % 6)
	{
	case 0: r = value , g = t , b = p;
		break;
	case 1: r = q , g = value , b = p;
		break;
	case 2: r = p , g = value , b = t;
		break;
	case 3: r = p , g = q , b = value;
		break;
	case 4: r = t , g = p , b = value;
		break;
	case 5: r = value , g = p , b = q;
		break;
	}

	red = static_cast<uint8_t>(r * 255);
	green = static_cast<uint8_t>(g * 255);
	blue = static_cast<uint8_t>(b * 255);
}



double inline RGBConverter::threeway_max(double a, double b, double c)
{
	return max(a, max(b, c));
}

double inline RGBConverter::threeway_min(double a, double b, double c)
{
	return min(a, min(b, c));
}
