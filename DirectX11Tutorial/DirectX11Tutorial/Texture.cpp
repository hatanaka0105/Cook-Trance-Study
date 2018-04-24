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

	//��������targa�̃C���[�W�f�[�^�����[�h
	result = LoadTarga(filename, height, width);
	if (!result)
	{
		return false;
	}

	//�e�N�X�`���̃f�B�X�N���v�V������ݒ�
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

	//��e�N�X�`���̐���
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	//targa�̃C���[�W�f�[�^�̍s�s�b�`(the row pitch)��ݒ�
	rowPitch = (width * 4) * sizeof(unsigned char);

	//targa�̃C���[�W�f�[�^���e�N�X�`���ɃR�s�[
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//�V�F�[�_�[���\�[�X�r���[�̃f�B�X�N���v�V������ݒ�
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	//�e�N�X�`���̃V�F�[�_�[���\�[�X�r���[���쐬
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	//���̃e�N�X�`���̃~�b�v�}�b�v�𐶐�
	deviceContext->GenerateMips(m_textureView);

	//�C���[�W�f�[�^���e�N�X�`���Ƀ��[�h���ꂽ�̂ŁAtarga�̃C���[�W�f�[�^�����
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

	//�o�C�i���œǂނ��߂�targa�t�@�C�����I�[�v��
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	//�t�@�C���w�b�_�[�ɓǂݍ���
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//�w�b�_�[����K�v�ȏ��(important information)���擾
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	//bpp��32bit�Ŋ���24bit�łȂ����Ƃ��m�F
	if (bpp != 32)
	{
		return false;
	}

	//32bit�C���[�W�f�[�^�̃T�C�Y���v�Z
	imageSize = width * height * 4;

	//targa�C���[�W�f�[�^���������Ɋ��蓖��
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targa�C���[�W�f�[�^��ǂݍ���
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//�t�@�C�������
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//targa�̈���f�[�^���������Ɋ��蓖��
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	//targa����f�[�^�z��̃C���f�b�N�X��������
	index = 0;

	//targa�C���[�W�f�[�^�̃C���f�b�N�X��������
	k = (width * height * 4) - (width * 4);

	//targa�t�H�[�}�b�g���t���܂Ɋi�[����Ă��邽�߁A
	//targa�C���[�W�f�[�^��targa�o�͐�z��ɐ����������ŃR�s�[���܂��B
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; //��
			m_targaData[index + 1] = targaImage[k + 1]; //��
			m_targaData[index + 2] = targaImage[k + 0]; //��
			m_targaData[index + 3] = targaImage[k + 3]; //�A���t�@

			//targa�f�[�^�̃C���f�b�N�X���C���N�������g
			k += 4;
			index += 4;
		}

		//targa�C���[�W�f�[�^�C���f�b�N�X���A
		//��̐擪�ɂ���s�̑O�̍s�ɋt�ɓǂݍ��񂾌�ɖ߂�
		k -= (width * 8);
	}

	//�R�s�[��̔z��ɃR�s�[���ꂽ���_��targa�C���[�W�f�[�^�����
	delete[] targaImage;
	targaImage = 0;

	return true;
}