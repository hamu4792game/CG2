#pragma once

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "math/Vector4.h"
#include "ConstantBuffer.h"

// ブレンドモード
enum BlendMode {
	None,   //!< ブレンドなし
	Normal, //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	Add,    //!< 加算。Src * SrcA + Dest * 1
	Subtract, //!< 減算。Dest * 1 - Src * SrcA
	Multily,  //!< 乗算。Src * 0 + Dest * Src
	Screen,   //!< スクリーン。Src * (1 - Dest) + Dest * 1

};



class Texture2D
{
public:
	Texture2D() = default;
	~Texture2D();

	void Finalize();

private:
	ID3D12Resource* resource = nullptr;
	ID3D12DescriptorHeap* SRVHeap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	ID3D12Resource* vertexResource = nullptr;
	IDxcBlob* vertexShader = nullptr;
	IDxcBlob* pixelShader = nullptr;

	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;

	struct Circle
	{
		Vector2 center;
		float radius;
	};
	ConstantBuffer<Circle> cBuffer;
	ConstantBuffer<Vector4> cColor;


public:

	void Texture(const std::string& filePath, const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateDescriptor(const std::string& filePath);

	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void Draw(uint32_t color);

	Vector4 ChangeColor(uint32_t color);

	BlendMode blend;
	void SetBlend(BlendMode blend_) {
		blend = blend_;
	};
	

private:
	//	DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	//	TextureResourceにデータを転送する
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

};

