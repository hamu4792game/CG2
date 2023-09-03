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
			float(Engine::GetInstance()->WindowWidth / 2), -float(Engine::GetInstance()->WindowHeight / 2), 0.01f, farZ);
	}
	

	if (false)
	{
		transform.translation_ = { 0.0f,0.0f,-5.0f };
		transform.rotation_ = { 0.0f,0.0f,0.0f };
		transform.scale_ = { 1.0f,1.0f,1.0f };

		transform.translation_.x = 0.0f;
		transform.translation_.y = 250.0f;
		transform.translation_.z = 0.0f;
		transform.rotation_.x = AngleToRadian(90.0f);
		transform.rotation_.y = AngleToRadian(0.0f);
		transform.rotation_.z = AngleToRadian(0.0f);
	}
	//cameraMatrix = MakeAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);

}

Matrix4x4 Camera::GetViewProMat() {
	//	行列の計算
	cameraMatrix = MakeAffineMatrix(transform.scale_, transform.rotation_, transform.translation_);
	viewMatrix = Inverse(cameraMatrix);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	return viewProjectionMatrix;
}
