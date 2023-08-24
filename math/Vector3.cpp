#include "Vector3.h"
#include <cmath>
#include <cassert>
#include <numbers>

Vector3::Vector3() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

Vector3::Vector3(const float& X, const float& Y, const float& Z) {
	this->x = X;
	this->y = Y;
	this->z = Z;
}

Vector3 Vector3::operator+(const Vector3& num) const {
	Vector3 result;
	result.x = this->x + num.x;
	result.y = this->y + num.y;
	result.z = this->z + num.z;
	return result;
}

Vector3 Vector3::operator-(const Vector3& num) const {
	Vector3 result;
	result.x = this->x - num.x;
	result.y = this->y - num.y;
	result.z = this->z - num.z;
	return result;
}

Vector3& Vector3::operator+=(const Vector3& num) {
	*this = *this + num;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& num) {
	*this = *this - num;

	return *this;
}

Vector3& Vector3::operator=(const Vector3& num)
{
	this->x = num.x;
	this->y = num.y;
	this->z = num.z;
	return *this;
}

Vector3 Vector3::operator*(const float& num) const
{
	Vector3 result;
	result.x = this->x * num;
	result.y = this->y * num;
	result.z = this->z * num;
	return result;
}

Vector3& Vector3::operator*=(const float& num)
{
	*this = *this * num;

	return *this;
}

float OuterProduct(const Vector3& vec)
{
	return sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

Vector3 Normalize(Vector3 vec3)
{
	Vector3 result;
	float w = OuterProduct(vec3);
	if (w == 0)	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	w = 1.0f / w;
	result.x = vec3.x * w;
	result.y = vec3.y * w;
	result.z = vec3.z * w;
	return result;
}

Vector3 AbsoluteValue(const Vector3& num)
{
	Vector3 result{};
	result.x = fabsf(num.x);
	result.y = fabsf(num.y);
	result.z = fabsf(num.z);

	return result;
}

Vector3 NormalizeRad(const Vector3& vec)
{
	Vector3 result;
	result.x = std::atan2f(vec.y, vec.z);
	result.y = std::atan2f(vec.z, vec.x);
	result.z = std::atan2f(vec.y, vec.x);
	return result;
}

float AngleToRadian(float angle)
{
	return angle * (std::numbers::pi_v<float> / 180.0f);
}

float RadianToAngle(float radian)
{
	return radian * (180.0f / std::numbers::pi_v<float>);
}

Vector3 FindVector(const Vector3& vec1, const Vector3& vec2)
{
	return vec2 - vec1;
}


