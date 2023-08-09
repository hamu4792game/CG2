#include "Skydome.h"

Skydome::Skydome(const std::string& filePath)
{
	transform.translation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(50.0f, 50.0f, 50.0f);
	//skydome->Texture(filePath, "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	skydome.Texture(filePath, "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Skydome::Draw(const Matrix4x4& viewProjection)
{
	transform.UpdateMatrix();
	skydome.Draw(transform, viewProjection, 0xffffffff);
}
