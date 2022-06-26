
#ifndef _RGBCONVERTER_h
#define _RGBCONVERTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class RGBConverter 
{

public:
	static void RgbToHsv(uint8_t r, uint8_t g, uint8_t b, double &hue);
	
	static void HsvToRgb(double hue, double saturation, double value, float & red, float & green, float & blue);
	
private:
	static double threeway_max(double a, double b, double c);
	static double threeway_min(double a, double b, double c);
};
#endif
