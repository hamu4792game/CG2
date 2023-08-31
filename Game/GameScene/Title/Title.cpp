
#include "Game/GameScene/GameScene.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include <algorithm>

Title::Title(std::shared_ptr<Camera> camera)
{
	camera_ = camera;
}

void Title::ModelLoad()
{
	sphere.Texture("Resources/enemy/enemyType1.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	title_tai.Texture("Resources/hud/tai.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	title_men.Texture("Resources/hud/men.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
	push.Texture("Resources/hud/push.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void Title::Initialize()
{
	camera_->transform.translation_ = Vector3(0.0f, 330.0f, 0.0f);
	camera_->transform.rotation_ = Vector3(AngleToRadian(90.0f), 0.0f, 0.0f);

	sphereTransform.translation_.y = 325.0f;
	pushColor = Vector4(255.0f, 20.0f, 9.0f, 255.0f);
	flg = false;
}

void Title::Update()
{
	sphereTransform.rotation_.y += AngleToRadian(1.0f);

	flg ? pushColor.w += 5.0f : pushColor.w -= 5.0f;
	pushColor.w = std::clamp(pushColor.w, 0.0f, 255.0f);
	if (pushColor.w == 0.0f || pushColor.w == 255.0f) {
		flg = !flg;
	}

	sphereTransform.UpdateMatrix();
}

void Title::Draw(const Matrix4x4& viewProjection)
{
	sphere.Draw(sphereTransform, viewProjection, 0xffffffff);
}

void Title::Draw2d(const Matrix4x4& viewProjection)
{
	title_tai.Draw(Vector2(-150.0f, 100.0f), Vector2(1.2f, 1.2f), 0.0f, viewProjection, 0xf0c810dd);
	title_men.Draw(Vector2(150.0f, -100.0f), Vector2(1.2f, 1.2f), 0.0f, viewProjection, 0xf0c810dd);
	push.Draw(Vector2(0.0f, -280.0f), Vector2(1.5f, 1.5f), 0.0f, viewProjection, ChangeColor(pushColor));
}
