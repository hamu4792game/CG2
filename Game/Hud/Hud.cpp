#include "Hud.h"
#include "externals/imgui/imgui.h"

void Hud::Initialize()
{
	pos = Vector2(0.0f, 0.0f);
	scale = Vector2(1.0f, 1.0f);
	rotate = 0.0f;
}

void Hud::TextureLoad()
{
	enemyHp.Texture("Resources/hpber.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Hud::Draw(const Matrix4x4& viewProjection)
{
	ImGui::DragFloat2("pos", &pos.x, 1.0f);
	ImGui::DragFloat2("scale", &scale.x, 0.001f);
	ImGui::DragFloat("rotate", &rotate, 0.1f);
	enemyHp.Draw(pos, scale, rotate, viewProjection, 0xffffffff);
}
