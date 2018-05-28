#ifndef VG_PAINT_H
#define VG_PAINT_H

#include "nanovg.h"

class VGPaint
{
public:
	VGPaint();

	void setFill(NVGcontext*);
	void setStroke(NVGcontext*);
};

#endif // VG_PAINT_H
