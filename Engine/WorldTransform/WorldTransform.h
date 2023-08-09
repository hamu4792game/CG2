#pragma once
#include "math/Matrix4x4.h"
#include "math/Vector3.h"

class WorldTransform
{
public:
	WorldTransform() = default;
	~WorldTransform() = default;

public:
	// ローカルスケール
	Vector3 scale_ = { 1, 1, 1 };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0, 0, 0 };
	// ローカル座標
	Vector3 translation_ = { 0, 0, 0 };
	// ローカル → ワールド変換行列
	Matrix4x4 worldMatrix;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	Matrix4x4 UpdateMatrix();
};
