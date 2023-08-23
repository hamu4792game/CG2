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

	//	追従対象からカメラまでの初期位置
	//Vector3 camerapos = { 0.0f,5.0f,-70.0f };

public:
	WorldTransform transform;
	//	Y軸の角度
	float degree;
	//	viewProjectionMatrixの取得
	Matrix4x4 GetViewProMat();

	Vector3 position;	//カメラの座標
	Vector3 target;		//カメラの注視点
	Vector3 playerPosition;	//playerの座標
	Vector3 enemyPosition;	//enemyの座標

private:
	//	追従対象 (player)
	const WorldTransform* target_ = nullptr;
	//	注視点 (enemy)
	const WorldTransform* lockon_ = nullptr;
	
public:
	//	追従対象の設定
	void SetTarget(const WorldTransform* target) { target_ = target; };
	//	追従対象の設定
	void SetLockon(const WorldTransform* lockon) { lockon_ = lockon; };

};

