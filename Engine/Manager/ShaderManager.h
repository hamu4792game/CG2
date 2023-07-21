#pragma once
#include "Engine/Log.h"

class ShaderManager
{
public:
	ShaderManager() = default;
	~ShaderManager() = default;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// これにより1つしか生成されなくなる
	/// </summary>
	/// <returns></returns>
	static ShaderManager* GetInstance();

	/// <summary>
	/// dxcCompilerを初期化
	/// </summary>
	void DXcInitialize();

	void Finalize();

public:
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath">compilerするshaderファイルへのパス</param>
	/// <param name="profile">compilerに使用するProfile</param>
	/// <returns></returns>
	IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:
	IDxcUtils* dxcUtils;
	IDxcCompiler3* dxcCompiler;
	IDxcIncludeHandler* includeHandler;

};

