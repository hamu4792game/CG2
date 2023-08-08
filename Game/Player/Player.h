#pragma once
#include "Engine/Texture/Model.h"
#include <vector>
#include "Engine/WorldTransform/WorldTransform.h"
#include <list>

class Player
{
public:
	Player();
	~Player() = default;

	void ModelLoad();

private:
	
	//	モデルデータ配列
	std::list<std::unique_ptr < Model>> models_;
	//	パーツ用データ
	//std::vector<WorldTransform> parts_;

};
