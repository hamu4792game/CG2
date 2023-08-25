#include "Ground.h"

Ground::Ground(const std::string& filePath)
{
	transform.translation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(70.0f, 1.0f, 70.0f);
	//skydome->Texture(filePath, "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	ground.Texture(filePath, "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Ground::Draw(const Matrix4x4& viewProjection)
{
	transform.UpdateMatrix();
	ground.Draw(transform, viewProjection, 0xffffffff);
}
