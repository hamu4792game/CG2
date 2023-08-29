#pragma once
#include "Math/Vector3.h"
#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"
#include "Engine/Camera/Camera.h"
#include "Game/Hud/Hud.h"

class Battle
{
public:
	Battle(std::shared_ptr<Camera> camera = nullptr);
	~Battle() = default;

public:
	//	初期化処理
	void Initialize();

	//	モデルのロード
	void ModelLoad();

	//	更新処理
	void Update();

	//	描画処理
	void Draw(const Matrix4x4& viewProjection);
	void Draw2d(const Matrix4x4& viewProjection2d);

private:	// 必要なメンバ変数のインスタンス
	std::shared_ptr<Camera> camera_;

	std::unique_ptr<Player> player;

	std::unique_ptr<Enemy> enemy;

	std::unique_ptr<Hud> hud;

private:
	//	エネミーからプレイヤーに伸びるベクトル
	Vector3 vec;

};
