#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <string>
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"

//	頂点データ構造体
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

//	mtl
struct MaterialData {
	std::string textureFilePath;
};

//	モデルデータ構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

class TextureManager
{
public:
	TextureManager() = default;
	~TextureManager() = default;


	//	シングルトンインスタンスの取得
	static TextureManager* GetInstance();

	//	DirectX12のTextureResourceを作る
	//	DirectX12のTextureResourceを作る
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	static void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	//	
	//void CreateDescriptor(const std::string& filePath);

private:
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	//	デスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	// テクスチャコンテナ
	//std::array<Texture2D> textures_;

public:
	//	オブジェクトファイルを読み込む関数
	static ModelData LoadObjFile(const std::string&  filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& filename);

	
};

