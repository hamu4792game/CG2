#pragma once
#include "Engine/Texture/Model.h"
#include "Engine/WorldTransform/WorldTransform.h"

class Ground
{
public:
	Ground(const std::string& filePath);
	~Ground() = default;

private:
	Model ground;
	WorldTransform transform;

public:
	void Draw(const Matrix4x4& viewProjection);

};