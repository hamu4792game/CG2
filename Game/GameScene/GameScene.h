#pragma once
#include "Engine/Engine.h"
#include "Game/GameScene/Battle/Battle.h"
#include "Game/Skydome/Skydome.h"
#include "Game/Player/Player.h"
#include "Engine/Texture/Model.h"
#include "Engine/Camera/Camera.h"

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
	//	カメラの生成
	std::shared_ptr<Camera> camera;
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};

	//	シーン用インスタンス
	Battle battle;

public:

	void Initialize();

	void Update();
	
	void Draw();

};