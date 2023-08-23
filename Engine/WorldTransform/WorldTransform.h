#pragma once
#include "math/Matrix4x4.h"
#include "math/Vector3.h"

class WorldTransform
{
public:
	WorldTransform() = default;
	~WorldTransform() = default;

	WorldTransform& operator=(const WorldTransform &trans);

public:
	// ローカルスケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル → ワールド変換行列
	Matrix4x4 worldMatrix;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	Matrix4x4 UpdateMatrix();

	//	平行移動行列の平行移動の取得
	Vector3 GetTranslate() const { return Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]); };

};
