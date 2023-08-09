#pragma once
#include "math/Matrix4x4.h"
#include "Engine/WorldTransform/WorldTransform.h"

class Camera
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="farClip_">深度値</param>
	/// <param name="proType">false = 正射影 true = 透視投影</param>
	Camera(float farClip_ = 1000.0f, bool proType = false);
	~Camera() = default;

private:
	//	深度値
	float farZ;
	//	正射影か透視投影か false = 正射影 true = 透視投影
	bool type;
	//	行列の生成
	Matrix4x4 cameraMatrix{};
	Matrix4x4 viewMatrix{};
	Matrix4x4 projectionMatrix{};
	Matrix4x4 viewProjectionMatrix{};

	void Update();
public:
	Vector3 translate;
	Vector3 rotate;
	Vector3 scale;

	//	viewProjectionMatrixの取得
	Matrix4x4 GetViewProMat();

private:
	//	追従対象
	const WorldTransform* target_ = nullptr;
public:
	//	追従対象の設定
	void SetTarget(const WorldTransform* target) { target_ = target; };

};

