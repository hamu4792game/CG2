#include "KeyInput.h"
#include <cassert>
#include "Engine/Base/WinApp.h"

KeyInput* KeyInput::GetInstance()
{
	static KeyInput instance;
	return &instance;
}

void KeyInput::Initialize()
{
	HRESULT result;
	//	DirectInputの初期化
	directInput = nullptr;
	result = DirectInput8Create(WinApp::GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//	キーボードの生成
	keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//	入力データの形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);	// 標準形式
	assert(SUCCEEDED(result));
	//	排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(WinApp::GetHwnd(), 
		DISCL_FOREGROUND |		/*画面が手前にある場合のみ入力を受け付ける*/
		DISCL_NONEXCLUSIVE |	/*デバイスをこのアプリだけで専有しない*/
		DISCL_NOWINKEY);		/*Windowsキーを無効にする*/
	assert(SUCCEEDED(result));
}

void KeyInput::InputInitialize()
{
	KeyInput::GetInstance()->Initialize();
}

void KeyInput::Updata()
{
	KeyInput* instance = KeyInput::GetInstance();
	//	キーボード情報の取得開始
	instance->keyboard->Acquire();
	//	全キーの入力状態を取得する
	//	前フレームのキー状態の取得
	for (uint16_t i = 0; i < 256; i++) {
		instance->oldKey[i] = instance->key[i];
	}
	//	今のキー状態の取得
	instance->keyboard->GetDeviceState(sizeof(instance->key), instance->key);


}

bool KeyInput::GetKey(uint8_t keynumber)
{
	return KeyInput::GetInstance()->key[keynumber];
}

bool KeyInput::PushKey(uint8_t keynumber)
{
	return KeyInput::GetInstance()->key[keynumber] && !KeyInput::GetInstance()->oldKey[keynumber];
}

bool KeyInput::ReleaseKey(uint8_t keynumber)
{
	return !KeyInput::GetInstance()->key[keynumber] && KeyInput::GetInstance()->oldKey[keynumber];
}
