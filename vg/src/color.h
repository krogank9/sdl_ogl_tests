#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <cmath>

class Color
{
public:
	Color(const Color& copy) : r(copy.r), g(copy.g), b(copy.b), a(copy.a) {}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) : r(r/255.f), g(g/255.f), b(b/255.f), a(a/255.f) {}
	Color(int r, int g, int b, int a=255) : r(r/255.f), g(g/255.f), b(b/255.f), a(a/255.f) {}
	Color(float r, float g, float b, float a=1.0f) : r(r), g(g), b(b), a(a) {}

	uint32_t getRGBA()
	{
		return ((uint32_t)a_u8() << 24) + (b_u8() << 16) + ((uint32_t)g_u8() << 8) + ((uint32_t)r_u8());
	}

	uint8_t r_u8(uint8_t v) { r = v/255.f; return v; }
	uint8_t g_u8(uint8_t v) { g = v/255.f; return v; }
	uint8_t b_u8(uint8_t v) { b = v/255.f; return v; }
	uint8_t a_u8(uint8_t v) { a = v/255.f; return v; }
	uint8_t r_u8() { return r*255; }
	uint8_t g_u8() { return g*255; }
	uint8_t b_u8() { return b*255; }
	uint8_t a_u8() { return a*255; }

	bool has_negative() { return r < 0 || g < 0 ||b < 0 || a < 0; }
	void make_positive() { r = abs(a); g = abs(b); b = abs(b); a = abs(a); }

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

	float r, g, b, a;
};

#endif // COLOR_H
