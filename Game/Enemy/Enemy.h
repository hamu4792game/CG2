#pragma once
#include "Engine/Texture/Model.h"
#include <vector>
#include "Engine/WorldTransform/WorldTransform.h"
#include "Engine/Camera/Camera.h"

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

private:

	enum PARTS {
		Egg,
		Body,

		Num
	};

	float easeNum = 0.0f;
	bool isFlag = false;

private:
	void FirstUpdate();

public:

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { return transform; };


};
