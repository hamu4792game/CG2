#include "Skydome.h"

Skydome::Skydome()
{
	transform.translation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.rotation_ = Vector3(0.0f, 0.0f, 0.0f);
	transform.scale_ = Vector3(100.0f, 100.0f, 100.0f);
}

void Skydome::ModelLoad()
{
	skydome.Texture("Resources/boxSky/boxSky.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Skydome::Draw(const Matrix4x4& viewProjection)
{
	transform.UpdateMatrix();
	skydome.Draw(transform, viewProjection, 0xffffffff);
}
