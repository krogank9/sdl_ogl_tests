#ifndef VEC4_H
#define VEC4_H

template <class T>
class vec4
{
public:
	vec4() {}
	vec4(vec4& copy)
	{
		coords[0] = copy.x;
		coords[1] = copy.y;
		coords[2] = copy.z;
		coords[3] = copy.w;
	}
	vec4(T x, T y, T z, T w)
	{
		coords[0] = x;
		coords[1] = y;
		coords[2] = z;
		coords[3] = w;
	}

	T* getPtr() { return coords; }
private:
	T coords[4];
};

typedef vec4<float> vec4f;

#endif // VEC4_H
