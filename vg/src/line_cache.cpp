#include "line_cache.h"

#include "context.h"

LineCache::LineCache(Context* ctx, const std::vector<vec2f>& verts)
	: ctx(ctx), polygon(0), line(0), verts(verts)
{
}

LineCache::~LineCache()
{
	if (polygon != 0)
		delete polygon;
	if (line != 0)
		delete line;
}

void LineCache::makePolygon()
{
}

void LineCache::makeLine(float thickness, LINE_CAP_TYPE cap)
{
	makeNormals();

	// todo handle caps

	float half_thickness = thickness/2.f;

	// each quad uses 6 indices, 3 for each triangle
	int quad_count = verts.size() - 1;
	int indices_arr_size = quad_count*6; // 3 indices per 2 tri for 1 quad
	int verts_arr_size = quad_count*8; // 4 verts per quad, 2 coords x,y

	GLfloat verts_arr[verts_arr_size];
	GLfloat tex_coords_arr[verts_arr_size];
	GLuint indices_arr[indices_arr_size];

	for (unsigned int i=1; i<verts.size(); i++)
	{
		vec2f prev = verts[i-1];
		vec2f cur = verts[i];

		vec2f prev_top = prev - (normals[i-1] * half_thickness);
		vec2f prev_bot = prev + (normals[i-1] * half_thickness);
		vec2f cur_top = cur - (normals[i-1] * half_thickness);
		vec2f cur_bot = cur + (normals[i-1] * half_thickness);

		verts_arr[(i-1)*8 + 0] = prev_top.x;
		verts_arr[(i-1)*8 + 1] = prev_top.y;
		verts_arr[(i-1)*8 + 2] = cur_top.x;
		verts_arr[(i-1)*8 + 3] = cur_top.y;
		verts_arr[(i-1)*8 + 4] = cur_bot.x;
		verts_arr[(i-1)*8 + 5] = cur_bot.y;
		verts_arr[(i-1)*8 + 6] = prev_bot.x;
		verts_arr[(i-1)*8 + 7] = prev_bot.y;

		indices_arr[(i-1)*6 + 0] = (i-1)*4 + 0; // top triangle
		indices_arr[(i-1)*6 + 1] = (i-1)*4 + 1;
		indices_arr[(i-1)*6 + 2] = (i-1)*4 + 2;
		indices_arr[(i-1)*6 + 3] = (i-1)*4 + 0; // bot triangle
		indices_arr[(i-1)*6 + 4] = (i-1)*4 + 2;
		indices_arr[(i-1)*6 + 5] = (i-1)*4 + 3;

		//
		tex_coords_arr[(i-1)*8 + 0] = 0;
		tex_coords_arr[(i-1)*8 + 1] = 0;
		tex_coords_arr[(i-1)*8 + 2] = 0;
		tex_coords_arr[(i-1)*8 + 3] = 0;
		tex_coords_arr[(i-1)*8 + 4] = 0;
		tex_coords_arr[(i-1)*8 + 5] = 0;
		tex_coords_arr[(i-1)*8 + 6] = 0;
		tex_coords_arr[(i-1)*8 + 7] = 0;
	}

	line = new TriCache(ctx, verts_arr, verts_arr_size, indices_arr, indices_arr_size, tex_coords_arr);
}

void LineCache::makeNormals()
{
	// calculate relative verts
	relative_verts.reserve(verts.size()-1);
	std::vector<vec2f>::iterator it = verts.begin();

	vec2f last_vert = *it;
	it++;
	for (; it != verts.end(); it++)
	{
		relative_verts.push_back(*it - last_vert);
		last_vert = *it;
	}

	// calculate normals
	normals.reserve(relative_verts.size());
	it = relative_verts.begin();

	for (; it != relative_verts.end();)
	{
		normals.push_back((++it)->normalize().rotate90CW());
	}
}
