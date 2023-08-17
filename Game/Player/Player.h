#pragma once
#include "Engine/Texture/Model.h"
#include <vector>
#include "Engine/WorldTransform/WorldTransform.h"
#include "Engine/Camera/Camera.h"

class Player
{
public:
	Player() : num(4) {};
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
	
	WorldTransform transform;

	uint32_t color = 0xffffffff;

	const uint16_t num;

	//	モデルデータ配列
	std::vector<std::unique_ptr < Model>> models_;
	//	パーツ用データ
	std::vector<WorldTransform> parts_;

	const Camera* camera = nullptr;
public:
	//	カメラのセット
	void SetViewProjection(const Camera* view) { camera = view; };

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { return transform; };

	

};
