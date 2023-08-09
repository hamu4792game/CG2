#pragma once
#include "Engine/Texture/Model.h"
#include "Engine/WorldTransform/WorldTransform.h"

class Skydome
{
public:
	Skydome(const std::string& filePath);
	~Skydome() = default;

private:
	Model skydome;
	WorldTransform transform;

public:
	void Draw(const Matrix4x4& viewProjection);

};