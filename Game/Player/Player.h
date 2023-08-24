#pragma once
#include "Engine/Texture/Model.h"
#include <vector>
#include "Engine/WorldTransform/WorldTransform.h"
#include "Engine/Camera/Camera.h"
#include "Game/Player/PlayerBullet/PlayerBullet.h"
#include "Game/Enemy/Enemy.h"

class Player
{
public:
	Player() : num(4) {};
	~Player() = default;

	//	初期化処理
	void Initialize();
	//	更新処理
	void Update();
	//	描画処理
	void Draw(const Matrix4x4& viewProjection);

private:
	enum PARTS
	{
		Body,
		Head,
		L_arm,
		R_arm,

		Num
	};

	//	モデルのロード
	void ModelLoad();
	//	移動制限
	void MoveLimit();

public:
	//	移動処理
	void Move();
	//	攻撃処理
	void Attack();

	void CameraMove();

private:
	
	WorldTransform transform;

	//	前座標の移動分ベクトル
	Vector3 oldMove;
	//	前座標
	Vector3 oldPos;

	uint32_t color = 0xffffffff;

	const uint16_t num;

	//	敵からの距離
	float distance = 0.0f;

	//	モデルデータ配列
	std::vector<std::unique_ptr < Model>> models_;
	//	パーツ用データ
	std::vector<WorldTransform> parts_;

	Camera* camera = nullptr;

	Enemy* enemy_ = nullptr;

	//	弾のデータ
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

public:
	//	カメラのセット
	void SetViewProjection(Camera* view) { camera = view; };
	void SetEnemy_ptr(Enemy* enemy) { enemy_ = enemy; };

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { return transform; };

	

};
