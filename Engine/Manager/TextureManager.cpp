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

ModelData TextureManager::LoadObjFile(const std::string& filename)
{
	//	必要な変数の宣言
	ModelData modelData;	// 構築するModelData
	std::vector<Vector4> positions;	// 位置
	std::vector<Vector3> normals;	// 法線
	std::vector<Vector2> texcoords;	// テクスチャ座標
	std::string line;	// ファイルから読んだ1行を格納するもの
	/*p5_2 12*/
	//	ファイルを開く
	std::ifstream file(filename);
	assert(file.is_open());

	//	ファイルを読み込み、メタデータを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	// 先頭の識別子を読む

		//	identifierに応じた処理 v:頂点位置,vt:頂点テクスチャ座標,vn:頂点法線,f:面
		if (identifier == "v") {
			Vector4 position{};
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord{};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal{};
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3]{};
			//	面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; faceVertex++)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//	頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; element++)
				{
					std::string index;
					//	区切りでインデックスを読んでいく
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				//	要素へのIndexから、実際の要素の値を取得して、頂点を構築していく
				Vector4 position = positions[static_cast<std::vector<Vector4, std::allocator<Vector4>>::size_type>(elementIndices[0]) - 1];
				position.x *= -1.0f;
				Vector2 texcoord = texcoords[static_cast<std::vector<Vector2, std::allocator<Vector2>>::size_type>(elementIndices[1]) - 1];
				Vector3 normal = normals[static_cast<std::vector<Vector3, std::allocator<Vector3>>::size_type>(elementIndices[2]) - 1];
				normal.x *= -1.0f;
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//	頂点を逆で登録することで、周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			//	基本的に
			modelData.material = LoadMaterialTemplateFile(materialFilename);
		}
	}
	return modelData;
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


MaterialData TextureManager::LoadMaterialTemplateFile(const std::string& filename)
{
	//	変数の宣言
	
	MaterialData materialData; // 構築するデータ
	std::string line;	// ファイルから読んだ1行を格納するもの
	std::ifstream file("./Resources/" + filename);	//ファイルを開く
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//	連結してファイルパスにする
			materialData.textureFilePath = textureFilename;
		}
	}

	return materialData;
}
