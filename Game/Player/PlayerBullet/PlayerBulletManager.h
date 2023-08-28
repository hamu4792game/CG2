#pragma once
#include "Engine/Texture/Model.h"
#include <vector>

class PlayerBulletManager
{
public:
	PlayerBulletManager() = default;
	~PlayerBulletManager() = default;

private:

	std::vector<std::unique_ptr<Model>> bulletModels_;

};
