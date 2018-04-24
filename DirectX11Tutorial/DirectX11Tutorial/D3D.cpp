#include "D3D.h"

D3D::D3D()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_depthDisabledStencilState = 0;
}

D3D::D3D(const D3D& other)
{
}


D3D::~D3D()
{
}

bool D3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	//Vsync�̐ݒ���i�[
	m_vsync_enabled = vsync;

	//DirectX��GPI�t�@�N�g���[���쐬
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//�t�@�N�g���[���g�����߂̎�v��GPI(�O���{)�̃A�_�v�^�[���쐬
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//��v�ȃA�_�v�^�̃��j�^�o�͂��
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//�A�_�v�^�o�́i���j�^�j��DXGI_FORMAT_R8G8B8A8_UNORM�\���`���ɓK�����郂�[�h�̐����擾
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(false))
	{
		return false;
	}

	//���̃��j�^�[/�r�f�I�J�[�h�̑g�ݍ��킹�ŉ\�Ȃ��ׂĂ̕\�����[�h��ێ����郊�X�g���쐬
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//�\�����[�h�̃��X�g�\�����L��
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//���ׂĂ̕\�����[�h���s���āA��ʂ̕��ƍ����Ɉ�v����\�����[�h��������
	//��v������̂�������ƁA���̃��j�^�[�̃��t���b�V�����[�g�̕��q�ƕ�����i�[
	for (i = 0; i < numModes;i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//�A�_�v�^�[(�O���{)�̐���(Description)���Ƃ�
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//��p�̃r�f�I�J�[�h�̃����������K�o�C�g�P�ʂŕۑ�
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//�f�B�X�v���C���[�h�̃��X�g���J��
	delete[] displayModeList;
	displayModeList = 0;

	//�A�_�v�^�[�o�͂��J��
	adapterOutput->Release();
	adapterOutput = 0;

	//�A�_�v�^�[���J��
	adapter->Release();
	adapter = 0;

	//�t�@�N�g���[���J��
	factory->Release();
	factory = 0;

	//swapChainDesc��������
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//�P��̃o�b�N�o�b�t�@�ɐݒ�
	swapChainDesc.BufferCount = 1;

	//�o�b�N�o�b�t�@�̕��ƍ�����ݒ�
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//�o�b�N�o�b�t�@�̕\�ʂ�32bit��W���Ɛݒ�
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//�o�b�N�o�b�t�@�̃��t���b�V�����[�g��ݒ�
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//�o�b�N�o�b�t�@�̎g�p���@��ݒ�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//�����_�����O����E�B���h�E�̃n���h����ݒ�
	swapChainDesc.OutputWindow = hwnd;

	//�}���`�T���v�����O���I�t�ɂ���
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//�E�B���h�E���[�h�܂��̓t���X�N���[���ɐݒ�
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//�X�L�������C���̏����ƃX�P�[�����O���w�肵�Ȃ��悤�ɐݒ�
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//�v���[���e�[�V������Ƀo�b�N�o�b�t�@�̓��e��j�����܂��B
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//���x�ȃt���O��ݒ肵�Ȃ��ł��������B
	swapChainDesc.Flags = 0;

	//�@�\���x����DirectX 11�ɐݒ肵�܂��B
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//�X���b�v�`�F�[���ADirect3D�f�o�C�X�ADirect3D�f�o�C�X�R���e�L�X�g���쐬���܂��B
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	//�o�b�N�o�b�t�@�ւ̃|�C�^���[���擾
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//�o�b�N�o�b�t�@�̃|�C���^�ƃ����_�[�^�[�Q�b�g�̃r���[���쐬
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//�o�b�N�o�b�t�@�ւɕK�v�ȃ|�C���^���J��
	backBufferPtr->Release();
	backBufferPtr = 0;

	//�[�x�o�b�t�@�̊T�v��������
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//�[�x�o�b�t�@�̊T�v������
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//�L�����ꂽ�T�v���g�p���Đ[�x�o�b�t�@�̃e�N�X�`���쐬
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//�X�e���V���X�e�[�g�̊T�v��������
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//�X�e���V���X�e�[�g�̊T�v������
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//�s�N�Z�����O�ʂ������Ă���ꍇ�̃X�e���V������
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�s�N�Z������ʂ������Ă���ꍇ�̃X�e���V������
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�[�x�X�e���V���X�e�[�g���쐬

	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//�[�x�X�e���V���X�e�[�g������
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//�[�x�X�e���V���r���[��������
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//�[�x�X�e���V���r���[�̊T�v������
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//�[�x�X�e���V���r���[���쐬
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//�����_�[�^�[�Q�b�g�r���[�Ɛ[�x�X�e���V���o�b�t�@���o�̓����_�����O�p�C�v���C���Ƀo�C���h
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//�ǂ̂悤�Ƀ|���S����`�悷�邩�����肷�郉�X�^�̊T�v��ݒ�
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//�L�������T�v���烉�X�^���C�U�̏�Ԃ��쐬
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//���X�^���C�U�[�X�e�[�g��ݒ�

	m_deviceContext->RSSetState(m_rasterState);

	//�����_�����O�̃r���[�|�[�g��ݒ�
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//�r���[�|�[�g���쐬
	m_deviceContext->RSSetViewports(1, &viewport);

	//�ˉe�s���ݒ�
	fieldOfView = XM_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//3D�����_�����O�̂��߂̎ˉe�s����쐬
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//���[���h�s����P���s��ŏ�����
	m_worldMatrix = XMMatrixIdentity();

	//2D�����_�����O�̂��߂̐����e�s����쐬
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);


	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void D3D::Shutdown()
{
	//�E�B���h�E���[�h�ɂ���O�ɃV���b�g�_�E�����邩�A�X���b�v�`�F�[�����������Ɨ�O���X���[�����

	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//�o�b�t�@���N���A����F���w��
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//�o�b�N�o�b�t�@���N���A
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//�[�x�o�b�t�@���N���A
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	return;
}

void D3D::EndScene()
{
	//�����_�����O�����������̂ŁA�o�b�N�o�b�t�@����ʂɕ\��
	if (m_vsync_enabled)
	{
		//�Œ胊�t���b�V�����[�g
		m_swapChain->Present(1, 0);
	}
	else
	{
		//�ł��邾������
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3D::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3D::GetDeviceContext()
{
	return m_deviceContext;
}

void D3D::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3D::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3D::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3D::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3D::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void D3D::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}