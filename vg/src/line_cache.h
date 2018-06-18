#ifndef LINE_CACHE_H
#define LINE_CACHE_H

#include <vector>

#include "vec2.h"
#include "tri_cache.h"

class Context;

class LineCache
{
public:
	enum LINE_CAP_TYPE { NONE, BLOCK, ROUND };
	enum LINE_JOIN_TYPE { MITER, BEVEL, ROUNDED };

	LineCache(Context* ctx, const std::vector<vec2f>& verts);
	~LineCache();

	TriCache& getLine(float thickness, LINE_CAP_TYPE cap=NONE)
	{
		if (line == 0)
			makeLine(thickness, cap);
		return *line;
	}

private:
	void makePolygon();
	void makeLine(float thickness, LINE_CAP_TYPE cap=NONE);

	void makeNormals();

	Context* ctx;

	TriCache* polygon;
	TriCache* line;

	std::vector<vec2f> verts;
	std::vector<vec2f> relative_verts;
	std::vector<vec2f> normals;
};

#endif // LINE_CACHE_H
