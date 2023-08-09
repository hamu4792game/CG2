#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
///
class Vector3 final {
public:
	Vector3();
	Vector3(const float& X, const float& Y, const float& Z);

public:
	float x;
	float y;
	float z;

public:
	Vector3 operator+(const Vector3& num) const;
	Vector3 operator-(const Vector3& num) const;
	Vector3& operator+=(const Vector3& num);
	Vector3& operator-=(const Vector3& num);
	Vector3& operator=(const Vector3& num);

	Vector3 operator*(const float& num) const;
	Vector3 operator*(const Vector3& num) const;
};

Vector3 Normalize(Vector3 vec3);