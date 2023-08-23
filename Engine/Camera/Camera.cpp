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
	transform.rotation_ = { 0.0f,0.0f,0.0f };
	transform.scale_ = { 1.0f,1.0f,1.0f };

	//camerapos = { 0.0f,5.0f,-70.0f };

}

void Camera::Update()
{
	playerPosition = target_->GetTranslate();
	enemyPosition = lockon_->GetTranslate();
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
		pos *= 5.0f;
		//カメラがどれだけプレイヤーの座標より高いかを設定します。
		pos.y = 50.0f;
		//プレイヤーの座標に求めたベクトルを足して、カメラの座標とします。
		position = playerPosition + pos;

		//	座標をコピーしてオフセット分ずらす
		//	カメラはエネミーを中心に回転する
		transform.translation_ = position;

	}
}

Matrix4x4 Camera::GetViewProMat() {
	Update();
	//	行列の計算
	cameraMatrix = MakeAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	return viewProjectionMatrix;
}
