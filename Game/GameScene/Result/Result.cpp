#include "Result.h"
#include <algorithm>

Result::Result(std::shared_ptr<Camera> camera)
{
	camera_ = camera;
}

void Result::ModelLoad()
{
	clear.Texture("Resources/hud/result.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	over.Texture("Resources/hud/over.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	push.Texture("Resources/hud/push.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Result::Initialize()
{
	flag = false;
	pushColor = Vector4(255.0f, 255.0f, 0.0f, 255.0f);
}

void Result::ClearUpdate()
{
	flag = true;
	flg ? pushColor.w += 5.0f : pushColor.w -= 5.0f;
	pushColor.w = std::clamp(pushColor.w, 0.0f, 255.0f);
	if (pushColor.w == 0.0f || pushColor.w == 255.0f) {
		flg = !flg;
	}
}

void Result::OverUpdate()
{
	flag = false;
	flg ? pushColor.w += 5.0f : pushColor.w -= 5.0f;
	pushColor.w = std::clamp(pushColor.w, 0.0f, 255.0f);
	if (pushColor.w == 0.0f || pushColor.w == 255.0f) {
		flg = !flg;
	}
}

void Result::Draw(const Matrix4x4& viewProjection)
{
}

void Result::Draw2d(const Matrix4x4& viewProjection)
{
	if (flag) {
		clear.Draw(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), 0.0f, viewProjection, 0xffffffff);
	}
	else {
		over.Draw(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), 0.0f, viewProjection, 0xff0000ff);
	}
		push.Draw(Vector2(0.0f, -280.0f), Vector2(1.5f, 1.5f), 0.0f, viewProjection, ChangeColor(pushColor));
	
}
