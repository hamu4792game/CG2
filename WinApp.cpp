#include "WinApp.h"

//	imguiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
//	関数の外部宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include <string>

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//	メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//	ウィンドウが破棄された
	case WM_DESTROY:
		//	OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//	標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight)
{
	//	COMの初期化を行う
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//	ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//	ウィンドウクラス名（なんでも良い）
	wc.lpszClassName = L"CG2WindowClass";
	//	インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//	カーソル指定
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//	ウィンドウクラスを登録する
	RegisterClass(&wc);

	//	ウィンドウサイズを表す構造体にクライアント領域を入れる
	//	{x座標,y座標}
	RECT wrc = { 0,0,clientWidth,clientHeight };

	//	クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,		//	利用するクラス名
		title,					//	タイトルバーの文字（何でもいい）
		WS_OVERLAPPEDWINDOW,	//	よく見るウィンドウスタイル
		CW_USEDEFAULT,			//	表示X座標（Windowsに任せる）
		CW_USEDEFAULT,			//	表示Y座標（WindowsOSに任せる）
		wrc.right - wrc.left,	//	ウィンドウ横幅	
		wrc.bottom - wrc.top,	//	ウィンドウ縦幅
		nullptr,				//	親ウィンドウハンドル
		nullptr,				//	メニューハンドル
		wc.hInstance,			//	インスタンスハンドル
		nullptr					//	オプション
	);
}
