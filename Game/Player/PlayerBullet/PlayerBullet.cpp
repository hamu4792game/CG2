#include "Game/Player/PlayerBullet/PlayerBullet.h"


void PlayerBullet::Initialize(const Vector3& vector, const WorldTransform& player)
{
	//transform.parent_ = &player;
	//	world座標を渡してあげる
	transform.translation_ = player.GetTranslate();
	transform.translation_.y += 0.0f;

	//	移動量の正規化 * speed
	velocity = Normalize(FindVector(player.GetTranslate(), vector)) * 0.5f;
	isAlive = true;
	isHit = false;
	time = 0;
}

void PlayerBullet::Update(const Vector3& vector)
{
	isHit = false;

	if (isAlive) {
		//	移動量の追加
		transform.translation_ += velocity;
		//	回転
		transform.rotation_.y += AngleToRadian(10.0f);
		time++;

		//	対象(enemy)までの距離を取得し、フラグを立てる || 4秒以上で自動的に消える
		if (OuterProduct(transform.translation_ - vector) <= 5.0f) {
			time = 0;
			isAlive = false;
			isHit = true;
		}
		else if (time % (60 * 4) == 0) {
			time = 0;
			isAlive = false;
		}
	}
	
	//	更新
	transform.UpdateMatrix();
}

void PlayerBullet::Draw(const Matrix4x4& viewProjection, Model* model)
{
	if (isAlive) {
		model->Draw(transform, viewProjection, 0xffffffff);
	}
}
