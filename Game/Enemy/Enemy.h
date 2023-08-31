#pragma once
#include "Engine/Texture/Model.h"
#include <vector>
#include "Engine/WorldTransform/WorldTransform.h"
#include "Engine/Camera/Camera.h"

class Player;

class Enemy
{
public:
	Enemy();
	~Enemy() = default;

	//	初期化
	void Initialize();
	//	モデルのロード
	void ModelLoad();
	//	更新処理
	void Update();
	//	描画処理
	void Draw(const Matrix4x4& viewProjection);

	//	ダメージを受けたら
	void Damage();

public:
	WorldTransform transform;

	const uint16_t num;

	Vector4 color;

	//	モデルデータ配列
	std::vector<std::unique_ptr < Model>> models_;
	//	パーツ用データ
	std::vector<WorldTransform> parts_;


	enum STYLE {
		FIRST,
		FIRSTChange,
		SECOND,
		SECONDChange,
		LAST
	};
	STYLE style = STYLE::FIRST;

	int hp = 0;
	int maxHp = 0;
	int power = 0;
	bool clearFlag = false;

private:

	enum PARTS {
		Egg,
		Body,

		Num
	};

	enum PATTERN {
		Normal,
		Charge,
	};
	PATTERN pattern = PATTERN::Normal;

	float easeNum = 0.0f;
	bool isFlag = false;
	Vector3 easeStartPos;
	Vector3 easeEndPos;

	Player* player_ = nullptr;

	//	ベクトルのしゅとく
	Vector3 vec;

	void Change();

private:
	void FirstUpdate();

	void SecondUpdate();

public:

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { return transform; };

	void SetPlayer_ptr(Player* player);

};
