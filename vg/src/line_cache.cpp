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

	// *2 for each (x,y) and *2 again for normal in both directions
	int verts_arr_size = verts.size()*2*2;
	// each quad uses 6 indices, 3 for each triangle
	int quad_count = verts.size() - 1;
	int indices_arr_size = quad_count*6;

	GLfloat verts_arr[verts_arr_size];
	GLfloat tex_coords_arr[verts_arr_size];
	GLuint indices_arr[(verts.size()*2-1)*6];

	float length_so_far = 0.f;
	int prev_bot_index_x = 0;
	int prev_bot_index_y = 0;
	int prev_top_index_x = 0;
	int prev_top_index_y = 0;
	for (int i=0; i<verts.size(); i++)
	{
		// each iteration sets 4 values top(x,y) bot(x,y)
		int cur_top_index_x = i*4+0;
		int cur_top_index_y = i*4+1;
		int cur_bot_index_x = i*4+2;
		int cur_bot_index_y = i*4+3;

		// verts
		vec2f top_vec = (verts[i] + normals[i]) * half_thickness;
		vec2f bot_vec = (verts[i] + normals[i]) * -half_thickness;
		verts_arr[cur_top_index_x] = top_vec.x;
		verts_arr[cur_top_index_y] = top_vec.y;
		verts_arr[cur_bot_index_x] = bot_vec.x;
		verts_arr[cur_bot_index_y] = bot_vec.y;

		// texture
		tex_coords_arr[cur_top_index_y] = 0.f;
		tex_coords_arr[cur_bot_index_y] = thickness;
		tex_coords_arr[cur_top_index_x] = length_so_far;
		tex_coords_arr[cur_bot_index_x] = length_so_far;

		// indices
		if (i > 0)
		{
			// top triangle
			indices_arr[(i-1)*6 + 0] = prev_top_index_x/2; // top left
			indices_arr[(i-1)*6 + 1] = cur_top_index_x/2; // top right
			indices_arr[(i-1)*6 + 2] = cur_bot_index_x/2; // bot right
			// bot triangle
			indices_arr[(i-1)*6 + 3] = prev_top_index_x/2; // top left
			indices_arr[(i-1)*6 + 4] = cur_bot_index_x/2; // bot right
			indices_arr[(i-1)*6 + 5] = prev_bot_index_x/2; // bot left
		}

		prev_top_index_x = cur_top_index_x;
		prev_top_index_y = cur_top_index_y;
		prev_bot_index_x = cur_bot_index_x;
		prev_bot_index_y = cur_bot_index_y;
		length_so_far += relative_verts[i].length();
	}

	line = new TriCache(ctx, verts_arr, verts_arr_size, indices_arr, indices_arr_size, tex_coords_arr);
}

void LineCache::makeNormals()
{
	// calculate relative verts
	relative_verts.reserve(verts.size());
	std::vector<vec2f>::iterator it = verts.begin();

	vec2f last_vert = *it;
	it++;
	for (; it != verts.end(); it++)
	{
		relative_verts.push_back(*it - last_vert);
		last_vert = *it;
	}

	// calculate normals
	normals.reserve(verts.size());
	it = relative_verts.begin();

	// line join normal = angle avg of two neighboring normals
	vec2f prev = it->normalize();
	vec2f cur;

	normals.push_back(prev.rotate90CCW()); // first has only one neighbor
	for (; it != relative_verts.end()-1;)
	{
		cur = (++it)->normalize();
		normals.push_back(prev.avgByAngleTo(cur));
		prev = cur;
	}
	normals.push_back(cur.rotate90CCW()); // last has only one neighbor
}
