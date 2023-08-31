#include "GameScene.h"
#include "Battle/Battle.h"
#include "Engine/Input/KeyInput/KeyInput.h"
#include "Engine/Easing/Easing.h"

GameScene* GameScene::GetInstance()
{
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize()
{
	//	モデルの読み込みと生成
	skydome = std::make_unique<Skydome>();
	skydome->ModelLoad();
	ground = std::make_unique<Ground>("Resources/ground/ground.obj");
	camera = std::make_shared<Camera>(2000.0f, true);
	camera2d = std::make_unique<Camera>();
	//	カメラ行列の更新
	viewProjectionMatrix = camera->GetViewProMat();
	viewProjectionMatrix2d = camera2d->GetViewProMat();

	battleBGM.SoundLoadWave("./Resources/sound/w006.wav");
	battleBGM.SoundPlayWave(true);
	battleBGM.SetVolume(0.3f);

	scene = Scene::BATTLE;
	oldscene = Scene::GAMEOVER;

	//	初期化
	title = std::make_unique<Title>(camera);
	title->Initialize();
	
	battle = std::make_unique<Battle>(camera);
	battle->Initialize();

	result = std::make_unique<Result>(camera);
	result->Initialize();

	//	変数
	boxScale = 0.0f, boxRotate = 0.0f;
	easeNum = 0.0f;
	sceneChangeFlag = false;
	flag = false;


	title->ModelLoad();
	battle->ModelLoad();
	result->ModelLoad();


	box.Texture("Resources/hud/block.png", "./Shader/Texture2D.VS.hlsl", "./Shader/Texture2D.PS.hlsl");
}

void GameScene::Update()
{
	//	シーン切替わり時の初期化
	if (oldscene != scene) {
		switch (scene)
		{
		case GameScene::Scene::TITLE:
			title->Initialize();
			break;
		case GameScene::Scene::BATTLE:
			battle->Initialize();
			break;
		case GameScene::Scene::GAMEOVER:
			result->Initialize();
			break;
		case GameScene::Scene::GAMECLEAR:
			result->Initialize();
			break;
		}
	}
	oldscene = scene;

	//	更新
	switch (scene)
	{
	case GameScene::Scene::TITLE:
		title->Update();
		if ((KeyInput::PushKey(DIK_SPACE) || KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) && !sceneChangeFlag) {
			sceneChangeFlag = true;
		}
		break;
	case GameScene::Scene::BATTLE:
		battle->Update();
		if (KeyInput::PushKey(DIK_L) && !sceneChangeFlag) {
			sceneChangeFlag = true;
		}
		
		break;
	case GameScene::Scene::GAMEOVER:
		result->OverUpdate();
		if ((KeyInput::PushKey(DIK_SPACE) || KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) && !sceneChangeFlag) {
			sceneChangeFlag = true;
		}
		break;
	case GameScene::Scene::GAMECLEAR:
		result->ClearUpdate();
		if ((KeyInput::PushKey(DIK_SPACE) || KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) && !sceneChangeFlag) {
			sceneChangeFlag = true;
		}
		break;
	}

	//	シーンチェンジの処理
	if (sceneChangeFlag) {
		SceneChange();
	}
	

	//	カメラ行列の更新
	viewProjectionMatrix = camera->GetViewProMat();
	viewProjectionMatrix2d = camera2d->GetViewProMat();
}

void GameScene::Draw()
{
	skydome->Draw(viewProjectionMatrix);
	ground->Draw(viewProjectionMatrix);
	
	switch (scene)
	{
	case GameScene::Scene::TITLE:
		title->Draw(viewProjectionMatrix);
		title->Draw2d(viewProjectionMatrix2d);
		break;
	case GameScene::Scene::BATTLE:
		battle->Draw(viewProjectionMatrix);
		battle->Draw2d(viewProjectionMatrix2d);
		break;
	case GameScene::Scene::GAMEOVER:
		result->Draw(viewProjectionMatrix);
		result->Draw2d(viewProjectionMatrix2d);
		break;
	case GameScene::Scene::GAMECLEAR:
		result->Draw(viewProjectionMatrix);
		result->Draw2d(viewProjectionMatrix2d);
		break;
	}
	box.Draw(Vector2(0.0f, 0.0f), Vector2(boxScale, boxScale), boxRotate, viewProjectionMatrix2d, 0x000000ff);
}

void GameScene::SceneChange()
{
	//80
	boxRotate += AngleToRadian(15.0f);
	easeNum += 0.02f;
	easeNum = std::clamp(easeNum, 0.0f, 1.0f);
	if (!flag) {
		float T = Easing::EaseOutQuart(easeNum);
		boxScale = (1.0f - T) * 0.0f + T * 100.0f;

		if (easeNum == 1.0f) {
			flag = true;
			easeNum = 0.0f;
			boxScale = 100.0f;
			switch (scene)
			{
			case GameScene::Scene::TITLE:
				scene = GameScene::Scene::BATTLE;
				break;
			case GameScene::Scene::BATTLE:
				scene = GameScene::Scene::GAMECLEAR;
				break;
			case GameScene::Scene::GAMEOVER:
				scene = GameScene::Scene::TITLE;
				break;
			case GameScene::Scene::GAMECLEAR:
				scene = GameScene::Scene::TITLE;
				break;
			}
		}
	}
	else if (flag) {
		float T = Easing::EaseOutQuart(easeNum);
		boxScale = (1.0f - T) * 100.0f + T * 0.0f;
		if (easeNum == 1.0f) {
			flag = false;
			sceneChangeFlag = false;
			easeNum = 0.0f;
			boxScale = 0.0f;
			boxRotate = 0.0f;
		}
	}

	
}
