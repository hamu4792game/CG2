#pragma once
#include "math/Matrix4x4.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Texture/Texture2D.h"
#include "Engine/Texture/Model.h"

class Title
{
public:
	Title(std::shared_ptr<Camera> camera = nullptr);
	~Title() = default;

	void ModelLoad();

	void Initialize();

	void Update();

	void Draw(const Matrix4x4& viewProjection);
	void Draw2d(const Matrix4x4& viewProjection);

private:

	std::shared_ptr<Camera> camera_;

	Model sphere;
	Texture2D title_tai;
	Texture2D title_men;
	Texture2D push;
	Vector4 pushColor;
	bool flg;
	bool pushFlg;

	Vector2 pos;
	Vector2 scale;
	WorldTransform sphereTransform;

};

