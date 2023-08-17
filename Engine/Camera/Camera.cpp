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

	camerapos = { 0.0f,5.0f,-70.0f };
}

void Camera::Update()
{
	//	ターゲットがあれば追従
	if (true)
	{	
		/*if (KeyInput::GetKey(DIK_W))
		{
			camerapos.z += 0.2f;
		}
		if (KeyInput::GetKey(DIK_S))
		{
			camerapos.z -= 0.2f;
		}*/

		Vector3 pos = lockon_->GetTranslate() - target_->GetTranslate();
		pos.y = 0.0f;
		pos = Normalize(pos);

		degree = atan2f(pos.x, pos.z);
		transform.rotation_.y = degree;
		Matrix4x4 rom = MakeRotateYMatrix(degree);

		Vector3 offset = TransformNormal(camerapos, rom);

		//	座標をコピーしてオフセット分ずらす
		//	カメラはエネミーを中心に回転する
		transform.translation_ = lockon_->GetTranslate() + offset;

	}
}

Matrix4x4 Camera::GetViewProMat() {
	Update();
	//	行列の計算
	cameraMatrix = MakeAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
	viewMatrix = Inverse(cameraMatrix);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	return viewProjectionMatrix;
}
