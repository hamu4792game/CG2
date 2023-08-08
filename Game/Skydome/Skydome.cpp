#include "Skydome.h"

Skydome::Skydome(const std::string& filePath)
{
	pos = Vector3(0.0f, 0.0f, 0.0f);
	rotate = Vector3(0.0f, 0.0f, 0.0f);
	scale = Vector3(50.0f, 50.0f, 50.0f);
	//skydome->Texture(filePath, "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	skydome.Texture(filePath, "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

Skydome::~Skydome()
{

}

void Skydome::Draw(const Matrix4x4& viewProjection)
{
	skydome.Draw(pos, scale, rotate, viewProjection, 0xffffffff);
}
