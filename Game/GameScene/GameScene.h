#pragma once
#include "Engine/Engine.h"
#include "Game/GameScene/Battle/Battle.h"
#include "Game/Skydome/Skydome.h"
#include "Game/Ground/Ground.h"
#include "Game/Player/Player.h"
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
		RESULT
	};
	Scene scene = Scene::BATTLE;

	//	シングルトンインスタンス
	static GameScene* GetInstance();

	
private:
	//	天球の生成
	std::unique_ptr<Skydome> skydome;
	//	地面の生成
	std::unique_ptr<Ground> ground;
	//	カメラの生成
	std::shared_ptr<Camera> camera;
	std::unique_ptr<Camera> camera2d;
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};
	Matrix4x4 viewProjectionMatrix2d{};

	//	シーン用インスタンス
	std::unique_ptr<Battle> battle;

	AudioInput battleBGM;


public:

	void Initialize();

	void Update();
	
	void Draw();

};