#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

class Color
{
public:
	Color(const Color& copy) : r(copy.r), g(copy.g), b(copy.b), a(copy.a) {}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) : r(r), g(g), b(b), a(a) {}
	Color(int r, int g, int b, int a=255) : r(r), g(g), b(b), a(a) {}
	Color(float r, float g, float b, float a=1.0f) : r(r*255), g(g*255), b(b*255), a(a*255) {}

	uint32_t getRGBA() { return ((uint32_t)a << 24) + (b << 16) + ((uint32_t)g << 8) + ((uint32_t)r); }

	static Color RED() { return Color(255, 0, 0, 255); }
	static Color GREEN() { return Color(0, 255, 0, 255); }
	static Color BLUE() { return Color(0, 0, 255, 255); }

	static Color PINK() { return Color(255, 0, 255, 255); }
	static Color YELLOW() { return Color(255, 255, 0, 255); }
	static Color CYAN() { return Color(0, 255, 255, 255); }

	static Color PURPLE() { return Color(128, 0, 128, 255); }

	static Color BLACK() { return Color(0, 0, 0, 255); }
	static Color WHITE() { return Color(255, 255, 255, 255); }
	static Color TRANSPARENT() { return Color(0, 0, 0, 0); }

	uint8_t r, g, b, a;
};

#endif // COLOR_H
