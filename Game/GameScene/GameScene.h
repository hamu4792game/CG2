#pragma once
#include "Engine/Engine.h"
#include "Engine/Texture/Model.h"
#include "Engine/Camera/Camera.h"
#include "Engine/Input/AudioInput/AudioInput.h"
#include "Engine/Texture/Texture2D.h"

class GameScene
{
public:
	GameScene() = default;
	~GameScene() = default;

	enum class Scene {
		TITLE,
		BATTLE,
		GAMEOVER,
		GAMECLEAR
	};
	Scene scene = Scene::TITLE;
	Scene oldscene = Scene::TITLE;

	//	シングルトンインスタンス
	static GameScene* GetInstance();

	
private:
	
	//	カメラの生成
	std::shared_ptr<Camera> camera;
	std::unique_ptr<Camera> camera2d;
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};
	Matrix4x4 viewProjectionMatrix2d{};

	//	シーン用インスタンス


public:

	void Initialize();

	void Update();
	
	void Draw();

};