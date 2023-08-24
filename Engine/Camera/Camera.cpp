#include "Engine/Camera/Camera.h"
#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"
#include "Engine/Input/KeyInput/KeyInput.h"

Camera::Camera(float farClip_, bool proType) {
	farZ = farClip_;
	type = proType;
	if (type) {
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(Engine::GetInstance()->WindowWidth) / float(Engine::GetInstance()->WindowHeight), 0.1f, farZ);
	}
	else {
		projectionMatrix = MakeOrthographicMatrix(-float(Engine::GetInstance()->WindowWidth / 2), float(Engine::GetInstance()->WindowHeight / 2),
			float(Engine::GetInstance()->WindowWidth / 2), -float(Engine::GetInstance()->WindowHeight / 2), 0.1f, farZ);
	}
	transform.translation_ = { 0.0f,0.0f,-5.0f };
	transform.rotation_ = { 0.09f,0.0f,0.0f };
	transform.scale_ = { 1.0f,1.0f,1.0f };

	camerapos = { 0.0f,5.0f,-20.0f };

	if (false)
	{
		transform.translation_.x = 0.0f;
		transform.translation_.y = 250.0f;
		transform.translation_.z = 0.0f;
		transform.rotation_.x = AngleToRadian(90.0f);
		transform.rotation_.y = AngleToRadian(0.0f);
		transform.rotation_.z = AngleToRadian(0.0f);
	}

}

void Camera::Update()
{
	playerPosition = target_->translation_;
	enemyPosition = lockon_->translation_;
	//	ターゲットがあれば追従
	if (true)
	{	
		//注視点はエネミーの座標にします。
		target = enemyPosition;
		//エネミーからプレイヤーに伸びるベクトルを求めます。
		Vector3 pos = playerPosition - enemyPosition;
		//カメラの高さは一定にしたいので、y成分を0にします。
		pos.y = 0.0f;
		//ベクトルを正規化します。
		pos = Normalize(pos);
		//スカラーを掛けます
		pos *= 40.0f;
		//カメラがどれだけプレイヤーの座標より高いかを設定します。
		pos.y = 10.0f;
		//プレイヤーの座標に求めたベクトルを足して、カメラの座標とします。
		position = playerPosition + pos;
		//	カメラを回転してあげる 逆ベクトルなので-
		transform.rotation_.y = atan2f(-position.x, -position.z);
		//transform.rotation_.x = atan2f(-position.y, -position.z);

		//	座標の反映
		transform.translation_ = position;
	}
}

Matrix4x4 Camera::GetViewProMat() {
	//Update();
	//	行列の計算
	cameraMatrix = MakeAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
	viewMatrix = Inverse(cameraMatrix);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	return viewProjectionMatrix;
}
