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
	Battle battle;
	std::unique_ptr<Player> player;
	

	std::unique_ptr<Skydome> skydome;


	//	カメラの生成
	std::unique_ptr<Camera> camera;
	//	カメラ座標
	Vector3 cameraTranslate = { 0.0f,0.0f,-10.0f };
	Vector3 cameraRotate = { 0.0f,0.0f,0.0f };
	//	カメラ行列の生成
	Matrix4x4 viewProjectionMatrix{};

public:

	void Initialize();

	void Update();
	
	void Draw();

};