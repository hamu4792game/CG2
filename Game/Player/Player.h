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
	Player();
	~Player() = default;

	//	初期化処理
	void Initialize();
	//	モデルのロード
	void ModelLoad();
	//	更新処理
	void Update();
	//	描画処理
	void Draw(const Matrix4x4& viewProjection);

private:
	enum PARTS
	{
		center,
		Body,
		Head,
		L_arm,
		R_arm,
		L_leg,
		R_leg,
		Weapon,

		Num
	};

	//	移動制限
	void MoveLimit();

	//	移動処理
	void Move();
	//	攻撃処理
	void Attack();
	//	ジャンプ処理
	void Jamp();

public:
	void CameraMove();

private:
	
	WorldTransform transform;

	//	前座標の移動分ベクトル
	Vector3 oldMove;
	//	前座標
	Vector3 oldPos[10] = {};

	uint32_t color = 0xffffffff;

	float easeNum = 0.0f;

	//	敵からの距離
	float distance = 0.0f;

	//	モデルデータ配列
	std::vector<std::unique_ptr < Model>> models_;
	//	パーツ用データ
	std::vector<WorldTransform> parts_;

	Camera* camera = nullptr;
	Enemy* enemy_ = nullptr;

	//	弾の最大数
	const uint16_t num;
	//	弾のモデル
	std::array <std::unique_ptr<Model>, 20> bulletModel_;
	//	弾のデータ
	std::array<std::unique_ptr<PlayerBullet>, 20> bullets_;

	bool jampflg = false;
	float velocity = 0.0f;

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
