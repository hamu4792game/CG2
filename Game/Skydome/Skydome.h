#pragma once
#include "Engine/Texture/Model.h"

class Skydome
{
public:
	Skydome(const std::string& filePath);
	~Skydome();

private:
	Model skydome;
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

public:
	void Draw(const Matrix4x4& viewProjection);

};