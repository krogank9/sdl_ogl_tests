#ifndef VEC4_H
#define VEC4_H

template <class T>
class vec4
{
public:
	vec4() {}
	vec4(const vec4& copy)
	{
		coords[0] = copy.x();
		coords[1] = copy.y();
		coords[2] = copy.z();
		coords[3] = copy.w();
	}
	vec4(T x, T y, T z, T w)
	{
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
		coords[3] = w;
	}

	T coord(int i) const { return coords[i]; }
	T x() const { return coords[0]; }
	T y() const { return coords[1]; }
	T z() const { return coords[2]; }
	T w() const { return coords[3]; }

	bool operator==(const vec4& rhs) const
	{
		return coord(0) == rhs.coord(0)
				&& coord(1) == rhs.coord(1)
				&& coord(2) == rhs.coord(2)
				&& coord(3) == rhs.coord(3);
	}

	bool operator!=(const vec4& rhs) const
	{
		return coord(0) != rhs.coord(0)
				|| coord(1) != rhs.coord(1)
				|| coord(2) != rhs.coord(2)
				|| coord(3) != rhs.coord(3);
	}

	T* getPtr() const { return coords; }

private:
	T coords[4];
};

typedef vec4<float> vec4f;
typedef vec4<double> vec4d;

#endif // VEC4_H
