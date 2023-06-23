#pragma once
#include <Windows.h>
#include <cstdint>

class WinApp
{
public: // 静的メンバ変数
	//	ウィンドウサイズ
	static const int kWindowWidth = 1280;
	static const int kWindowHeight = 720;

public:	// 静的メンバ関数
	
	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>可否</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: // メンバ関数

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="clientWidth">横幅</param>
	/// <param name="clientHeight">縦幅</param>
	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

private:
	//	ウィンドウクラス
	WNDCLASS wc{};

};
