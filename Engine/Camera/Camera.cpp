#include "Engine/Camera/Camera.h"
#include "Engine/Engine.h"

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
	translate = { 0.0f,0.0f,-5.0f };
	rotate = { 0.0f,0.0f,0.0f };
	scale = { 1.0f,1.0f,1.0f };
}

void Camera::Update()
{
	//	ターゲットがあれば追従
	if (target_)
	{
		//	追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f,2.0f,-20.0f };

		//	カメラの角度から回転行列を計算する
		Matrix4x4 rotatoMat = MakeRotateMatrix(this->rotate);

		offset = TransformNormal(offset, rotatoMat);

		//	座標をコピーしてオフセット分ずらす
		translate = target_->translation_ + offset;
	}
}

Matrix4x4 Camera::GetViewProMat() {
	Update();
	//	行列の計算
	cameraMatrix = MakeAffineMatrix(scale, rotate, translate);
	viewMatrix = Inverse(cameraMatrix);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	return viewProjectionMatrix;
}
