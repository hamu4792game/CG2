#pragma once
#include "math/Matrix4x4.h"

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
public:
	//	viewProjectionMatrixの取得
	Matrix4x4 GetViewProMat(Vector3 translate = { 0.0f,0.0f,-5.0f }, Vector3 scale = { 1.0f,1.0f,1.0f }, Vector3 rotate = { 0.0f,0.0f,0.0f });
};

