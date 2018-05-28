#ifndef VG_PATH_H
#define VG_PATH_H

#include "vg_node.h"
#include "vg_paint.h"

class VGPath : public VGNode
{
public:
	VGPath();

	virtual void render(NVGcontext*);

	VGPaint fillPaint;
	VGPaint strokePaint;
	bool closed;
};

#endif // VG_PATH_H
