#include "Texture.h"

Texture::Texture()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

Texture::Texture(const Texture& other)
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	//メモリにtargaのイメージデータをロード
	result = LoadTarga(filename, height, width);
	if (!result)
	{
		return false;
	}

	//テクスチャのディスクリプションを設定
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//空テクスチャの生成
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	//targaのイメージデータの行ピッチ(the row pitch)を設定
	rowPitch = (width * 4) * sizeof(unsigned char);

	//targaのイメージデータをテクスチャにコピー
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//シェーダーリソースビューのディスクリプションを設定
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	//テクスチャのシェーダーリソースビューを作成
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	//このテクスチャのミップマップを生成
	deviceContext->GenerateMips(m_textureView);

	//イメージデータがテクスチャにロードされたので、targaのイメージデータを解放
	delete[] m_targaData;
	m_targaData = 0;
	
	return true;
}

void Texture::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
}

bool Texture::LoadTarga(char* filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	//バイナリで読むためにtargaファイルをオープン
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	//ファイルヘッダーに読み込み
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//ヘッダーから必要な情報(important information)を取得
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	//bppが32bitで且つ24bitでないことを確認
	if (bpp != 32)
	{
		return false;
	}

	//32bitイメージデータのサイズを計算
	imageSize = width * height * 4;

	//targaイメージデータをメモリに割り当て
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targaイメージデータを読み込み
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//ファイルを閉じる
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//targaの宛先データをメモリに割り当て
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	//targa宛先データ配列のインデックスを初期化
	index = 0;

	//targaイメージデータのインデックスを初期化
	k = (width * height * 4) - (width * 4);

	//targaフォーマットが逆さまに格納されているため、
	//targaイメージデータをtarga出力先配列に正しい順序でコピーします。
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; //赤
			m_targaData[index + 1] = targaImage[k + 1]; //緑
			m_targaData[index + 2] = targaImage[k + 0]; //青
			m_targaData[index + 3] = targaImage[k + 3]; //アルファ

			//targaデータのインデックスをインクリメント
			k += 4;
			index += 4;
		}

		//targaイメージデータインデックスを、
		//列の先頭にある行の前の行に逆に読み込んだ後に戻す
		k -= (width * 8);
	}

	//コピー先の配列にコピーされた時点でtargaイメージデータを解放
	delete[] targaImage;
	targaImage = 0;

	return true;
}