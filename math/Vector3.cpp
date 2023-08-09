#include "Vector3.h"
#include <cmath>
#include <cassert>

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

Vector3 Vector3::operator*(const Vector3& num) const
{
	return Vector3(this->x * num.x, this->y * num.y, this->z * num.z);
}

Vector3 Normalize(Vector3 vec3)
{
	Vector3 result;
	float w = sqrtf((vec3.x * vec3.x) + (vec3.y * vec3.y) + (vec3.z * vec3.z));
	assert(w != 0);
	result.x = vec3.x / w;
	result.y = vec3.y / w;
	result.z = vec3.z / w;
	return result;
}
