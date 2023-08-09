#include "WorldTransform.h"

Matrix4x4 WorldTransform::UpdateMatrix()
{
	//	スケール、回転、平行移動を合成して行列を計算する
	this->worldMatrix = MakeAffineMatrix(scale_, rotation_, translation_);

	//	親があれば親のワールド行列を掛ける
	if (parent_)
	{
		this->worldMatrix = this->worldMatrix * parent_->worldMatrix;
	}

	return this->worldMatrix;
}
