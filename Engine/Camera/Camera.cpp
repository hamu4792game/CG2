#include "Engine/Camera/Camera.h"

Camera::Camera(float farClip_, bool proType) {
	farZ = farClip_;
	type = proType;
	if (type) {
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, farZ);
	}
	else {
		projectionMatrix = MakeOrthographicMatrix(-640.0f, 360.0f, 640.0f, -360.0f, 0.1f, farZ);
	}
}

Matrix4x4 Camera::GetViewProMat(Vector3 translate, Vector3 rotate, Vector3 scale) {
	//	行列の計算
	cameraMatrix = MakeAffineMatrix(scale, rotate, translate);
	viewMatrix = Inverse(cameraMatrix);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	return viewProjectionMatrix;
}
