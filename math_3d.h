#pragma once
#ifndef MATH_3D_H
#define MATH_3D_H

struct Vector3f {
	float x, y, z;

	Vector3f() = default;
	Vector3f(float x, float y, float z) : x(x), y(y), z(z) { }
};

struct Matrix4f {
	float m[4][4];
};

#endif /* MATH_3D_H */