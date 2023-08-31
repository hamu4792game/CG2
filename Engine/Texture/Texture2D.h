#pragma once

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "math/Vector4.h"
#include "Engine/Base/ConstantBuffer.h"
#include "math/Matrix4x4.h"


// ブレンドモード
enum BlendMode {
	None,   //!< ブレンドなし
	Normal, //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	Add,    //!< 加算。Src * SrcA + Dest * 1
	Subtract, //!< 減算。Dest * 1 - Src * SrcA
	Multily,  //!< 乗算。Src * 0 + Dest * Src
	Screen,   //!< スクリーン。Src * (1 - Dest) + Dest * 1
	Dark,	//	比較暗
	Light,	//	比較明
};

class Texture2D
{
public:
	Texture2D() = default;
	~Texture2D() = default;

	void Finalize();

private:
	ID3D12Resource* resource[1] = {};
	ID3D12DescriptorHeap* SRVHeap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	ID3D12Resource* vertexResource = nullptr;
	// ここにindexBufferViewとindex用のリソースを作る
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	ID3D12Resource* indexResource = nullptr;

	IDxcBlob* vertexShader = nullptr;
	IDxcBlob* pixelShader = nullptr;

	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;

	struct Mono {
		Vector2 pibot;
		float rate;
	};
	ConstantBuffer<Matrix4x4> cMat;
	ConstantBuffer<Vector4> cColor;
	ConstantBuffer<Mono> cBuffer;

	uint32_t textureWidth = 0;
	uint32_t textureHeight = 0;

public:

	void Texture(const std::string& filePath, const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateDescriptor(const std::string& filePath);

	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateVertexResource();

	void CreateGraphicsPipeline();

public:
	//void Draw(Matrix4x4 worldMat, Matrix4x4 viewProjectionMat, uint32_t color);
	void Draw(Vector2 pos, Vector2 scale, float rotate, Matrix4x4 viewProjectionMat, uint32_t color);

	BlendMode blend = BlendMode::Normal;
	void SetBlend(BlendMode blend_);
	

private:
	//	DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

};

