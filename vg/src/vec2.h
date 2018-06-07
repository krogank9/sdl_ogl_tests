#ifndef VEC2_H
#define VEC2_H

#include <cmath>

template <class T>
class vec2 {
public:
	T x, y;

	vec2() :x(0), y(0) {}
	vec2(T x, T y) : x(x), y(y) {}
	vec2(const vec2& v) : x(v.x), y(v.y) {}

	bool operator==(const vec2& v) {
		return x == v.x && y == v.y;
	}

	vec2& operator=(const vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	vec2 operator+(vec2 v) {
		return vec2(x + v.x, y + v.y);
	}
	vec2 operator-(vec2 v) {
		return vec2(x - v.x, y - v.y);
	}

	vec2& operator+=(vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	vec2& operator-=(vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2 operator+(double s) {
		return vec2(x + s, y + s);
	}
	vec2 operator-(double s) {
		return vec2(x - s, y - s);
	}
	vec2 operator*(double s) {
		return vec2(x * s, y * s);
	}
	vec2 operator/(double s) {
		return vec2(x / s, y / s);
	}


	vec2& operator+=(double s) {
		x += s;
		y += s;
		return *this;
	}
	vec2& operator-=(double s) {
		x -= s;
		y -= s;
		return *this;
	}
	vec2& operator*=(double s) {
		x *= s;
		y *= s;
		return *this;
	}
	vec2& operator/=(double s) {
		x /= s;
		y /= s;
		return *this;
	}

	void set(T x, T y) {
		this->x = x;
		this->y = y;
	}

	vec2 rotate(double rads) const {
		double c = cos(rads);
		double s = sin(rads);
		double tx = x * c - y * s;
		double ty = x * s + y * c;
		return vec2(tx,ty);
	}

	vec2 rotate90CW() const { return vec2(-y, x); }

	vec2 rotate90CCW() const { return vec2(y, -x); }

	vec2 rotate180() const { return vec2(-x, -y); }

	vec2 normalize() const {
		if (length() == 0)
			return vec2(0,0);
		return vec2(x/length(), y/length());
	}

	vec2 avg(vec2& other) const
	{
		return vec2((x+other.x)/2, (y+other.y)/2);
	}

	vec2 avgByAngleToCW(vec2& other) const
	{
		double myAngle = atan2(y, x);
		double otherAngle = atan2(other.y, other.x);
		if (otherAngle < myAngle)
			otherAngle += M_PI*2;
		double halfwayAngle = (otherAngle - myAngle)/2.f;
		double addAngle = halfwayAngle - myAngle;
		return rotate(-addAngle);
	}

	vec2 avgByAngleToCCW(vec2& other) const
	{
		double myAngle = atan2(y, x);
		double otherAngle = atan2(other.y, other.x);
		if (myAngle < otherAngle)
			myAngle += M_PI*2;
		double halfwayAngle = (myAngle - otherAngle)/2.f;
		double addAngle = halfwayAngle - myAngle;
		return rotate(-addAngle);
	}

	float dist(vec2 v) const {
		vec2 d(v.x - x, v.y - y);
		return d.length();
	}
	float length() const {
		return std::sqrt(x * x + y * y);
	}
	vec2 truncate(double length) {
		double angle = atan2f(y, x);
		return vec2(length * cos(angle), length * sin(angle));
	}

	vec2 ortho() const {
		return vec2(y, -x);
	}

	static float dot(vec2 v1, vec2 v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
	static float cross(vec2 v1, vec2 v2) {
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

};

typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

#endif // VEC2_H
