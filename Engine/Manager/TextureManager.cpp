#include "Engine/Manager/TextureManager.h"
#include "Engine/Log.h"

#include <fstream>
#include <sstream>

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
	//	テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//	ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	//	ミニマップ付きのデータを返す
	return mipImages;
}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//	meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//	全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}
ModelData TextureManager::LoadObfFile(const std::string& directoryPath, const std::string& filename)
{
	//	必要な変数の宣言
	ModelData modelData;	// 構築するModelData
	std::vector<Vector4> position;	// 位置
	std::vector<Vector3> normals;	// 法線
	std::vector<Vector2> texcoords;	// テクスチャ座標
	std::string line;	// ファイルから読んだ1行を格納するもの
	/*p5_2 12*/

	return ModelData();
}


//
//void TextureManager::CreateDescriptor(const std::string& filePath)
//{
//	// 読み込み済みテクスチャを検索
//	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
//		return texture.name == fileName;
//		});
//	if (it != textures_.end()) {
//		// 読み込み済みテクスチャの要素番号を取得
//		return static_cast<uint32_t>(std::distance(textures_.begin(), it));
//	}
//}
