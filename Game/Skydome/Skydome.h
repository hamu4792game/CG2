#pragma once
#include "Engine/Texture/Model.h"
#include "Engine/WorldTransform/WorldTransform.h"

class Skydome
{
public:
	Skydome();
	~Skydome() = default;

	void ModelLoad();

private:
	Model skydome;
	WorldTransform transform;

public:
	void Draw(const Matrix4x4& viewProjection);

};