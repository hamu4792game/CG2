#pragma once
#include "math/Matrix4x4.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Texture/Texture2D.h"

class Result
{
public:
	Result(std::shared_ptr<Camera> camera = nullptr);
	~Result() = default;

	void ModelLoad();

	void Initialize();

	void ClearUpdate();
	void OverUpdate();

	void Draw(const Matrix4x4& viewProjection);
	void Draw2d(const Matrix4x4& viewProjection);

private:

	std::shared_ptr<Camera> camera_;
	Texture2D clear;
	Texture2D over;
	Texture2D push;
	bool flg;
	Vector4 pushColor;

	bool flag;

};
