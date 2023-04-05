#pragma once
#ifndef MATH_3D_H
#define MATH_3D_H

#define _USE_MATH_DEFINES
#include <math.h>

struct Vector3f { // ������ � 3-������ ������������
	float x, y, z;

	Vector3f() = default;
	Vector3f(float x, float y, float z) : x(x), y(y), z(z) { }
};

#define ToRadian(x) ((x) * M_PI / 180.0f) // �� �������� � �������
#define ToDegree(x) ((x) * 180.0f / M_PI) // �� �������� � �������

struct Matrix4f { // ������� ������� 4 �� 4
	float m[4][4];

	Matrix4f() = default; // ����������� ��� ����������
	inline void InitIdentity() { // ��������� ���������� ��������� �������
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}
	inline Matrix4f operator*(const Matrix4f& other) const { // ���������� ��������� ��������� �/� ���������
		Matrix4f result;

		for (unsigned i = 0; i < 4; i++) {
			for (unsigned j = 0; j < 4; j++) {
				result.m[i][j] = m[i][0] * other.m[0][j] +
								 m[i][1] * other.m[1][j] +
								 m[i][2] * other.m[2][j] +
								 m[i][3] * other.m[3][j];
			}
		}
		return result;
	}
};

#endif /* MATH_3D_H */