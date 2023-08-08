#pragma once

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <wrl.h>
#include <cstdint>

class KeyInput
{
public:
	KeyInput() = default;
	~KeyInput() = default;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// これにより1つしか生成されなくなる
	/// </summary>
	/// <returns>KeyInputのシングルトンインスタンス</returns>
	static KeyInput* GetInstance();

private:
	//	通常初期化処理
	void Initialize();
public:
	//	静的初期化処理
	static void InputInitialize();

	// 更新処理
	static void Update();

private:	//変数
	//	DirectInputの初期化
	Microsoft::WRL::ComPtr<IDirectInput8> directInput;
	//	キーボードの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard;
	//	キーの用意
	BYTE key[256] = {};
	BYTE oldKey[256] = {};

public:	// ゲッター
	
	// キーが押されているか
	static bool GetKey(uint8_t keynumber);

	//	キーを押した瞬間
	static bool PushKey(uint8_t keynumber);
	
	//	キーを離した瞬間
	static bool ReleaseKey(uint8_t keynumber);


};
