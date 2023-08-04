#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{
private:
	//	コンストラクタ等、未定義
	WinApp() = default;
	~WinApp() = default;

public: // 静的メンバ変数
	//	ウィンドウサイズ
	static const int kWindowWidth = 1280;
	static const int kWindowHeight = 720;

public:	// 静的メンバ関数
	
	/// <summary>
	/// シングルトンインスタンスの取得
	/// これにより1つしか生成されなくなる
	/// </summary>
	/// <returns></returns>
	static WinApp* GetInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>可否</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ウィンドウメッセージの処理
	/// </summary>
	/// <returns>true:終了 false:継続</returns>
	static bool ProcessMessage();
public: // メンバ関数

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="clientWidth">横幅</param>
	/// <param name="clientHeight">縦幅</param>
	void CreateGameWindow(const wchar_t* title, int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);


	/// <summary>
	/// ウィンドウの破棄
	/// </summary>
	void DeleteGameWindow();


private:
	// Window関連
	//	ウィンドウクラス
	WNDCLASS wc{};
	// ウィンドウハンドル
	HWND hwnd = nullptr;

public:	// ゲッター

	static const HWND GetHwnd() { return GetInstance()->hwnd; };
	static const WNDCLASS GetWc() { return GetInstance()->wc; };

};
