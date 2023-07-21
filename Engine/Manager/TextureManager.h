#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <string>

class TextureManager
{
public:
	TextureManager() = default;
	~TextureManager() = default;


	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	//	DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	//	
	//void CreateDescriptor(const std::string& filePath);

private:
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	//	デスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	// テクスチャコンテナ
	//std::array<Texture2D> textures_;

};

