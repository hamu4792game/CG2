#include "Player.h"

Player::Player()
{

}

void Player::ModelLoad()
{
	for (uint16_t i = 0; i < 5; i++)
	{
		models_.push_back(std::make_unique<Model>());
	}

	models_[0]->Texture("Resources/player.obj", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}
