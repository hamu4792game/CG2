#include "Engine/Manager/ShaderManager.h"
#include <cassert>
#include <combaseapi.h>

#include <dxcapi.h>
#include "math/Vector4.h"
#pragma comment(lib,"dxcompiler.lib")

ShaderManager* ShaderManager::GetInstance()
{
	static ShaderManager instance;
	return &instance;
}

void ShaderManager::DXcInitialize()
{
	HRESULT hr = S_FALSE;
	//	dxcCompilerを初期化
	dxcUtils = nullptr;
	dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//	現時点でincludeはしないが、includeに対応するための設定を行っておく
	includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
}

void ShaderManager::Finalize()
{
	includeHandler->Release();
	dxcCompiler->Release();
	dxcUtils->Release();
}

//	shaderをcompileする関数
IDxcBlob* ShaderManager::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	//	1.hlslファイルを読む
	//	これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//	hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//	読めなかったら止める
	assert(SUCCEEDED(hr));
	//	読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;	//	utf8の文字コードであることを通知

	//	Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),	//コンパイル対象のhlslファイル名
		L"-E",L"main",		//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,		//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",	//デバッグ用の城尾法を埋め込む
		L"-Od",				//最適化を外しておく
		L"-Zpr",			//メモリレイアウトは行優先
	};

	//	p24
	//	実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	//	読み込んだファイル
		arguments,				//	コンパイルオプション
		_countof(arguments),	//	コンパイルオプションの数
		includeHandler,			//	includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	//	コンパイル結果
	);
	//	コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	//	警告、エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//	警告、エラーダメ絶対
		assert(false);
	}
	shaderError->Release();

	//	コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//	成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//	もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//	実行用のバイナリを返却
	return shaderBlob;

}

