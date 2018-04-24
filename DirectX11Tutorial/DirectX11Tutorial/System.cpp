#include "System.h"

System::System()
{
	m_Input = 0;
	m_Graphics = 0;
}


System::System(const System& other)
{
}

System::~System()
{
}

//�A�v���P�[�V�����̂��ׂĂ̐ݒ���s���܂��B
bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//�֐��ɑ���O�ɏ����� Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	//Windows API��������
	InitializeWindows(screenWidth, screenHeight);

	//�C���v�b�g�I�u�W�F�N�g�̐����A�����̓��[�U�[����L�[�{�[�h���͂𓾂邽�߂Ɏg�� Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;

	if (!m_Input)
	{
		return false;
	}

	//�C���v�b�g�I�u�W�F�N�g��������
	m_Input->Initialize();

	//�O���t�B�b�N�X�I�u�W�F�N�g�̐����A�����͂��̃A�v���P�[�V�����̂��߂̂��ׂẴ����_�����O�𑀍삷�� Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new Graphics;

	if (!m_Graphics)
	{
		return false;
	}

	//�O���t�B�b�N�X�I�u�W�F�N�g��������
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	//�����܂ł̊Ԃł��ׂĖ��Ȃ������ΐi�߂�B
	return true;
}

void System::Shutdown()
{
	//�O���t�B�b�N�X�I�u�W�F�N�g���J��
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//�C���v�b�g�I�u�W�F�N�g���J��
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//�E�B���h�E�����
	ShutdownWindows();
}

void System::Run()
{
	MSG msg;
	bool done, result;

	//���b�Z�[�W�\���̂�������
	ZeroMemory(&msg, sizeof(MSG));

	//�E�B���h�E�����[�U�[����I�����b�Z�[�W���o��܂Ń��[�v����
	done = false;
	while (!done)
	{
		//�E�B���h�E���b�Z�[�W�𑀍�
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�E�B���h�E���A�v���P�[�V�������I������悤�Ɏw��������I��
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		//����ȊO�̏ꍇ�̓t���[������
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool System::Frame()
{
	bool result;

	//���[�U�[���G�X�P�[�v�L�[�����������A�A�v���P�[�V�������I�����������Ă��邩���m�F
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//�t���[���������O���t�B�b�N�X�I�u�W�F�N�g�ɍs��
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//�L�[�{�[�h��̃L�[�������ꂽ���ǂ������m�F
	case WM_KEYDOWN:
	{
		//���ꂪ�C���v�b�g�I�u�W�F�N�g�ɑ����A��Ԃ��L�^�ł���
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	//�L�[�{�[�h��̉�����Ă����L�[�����ꂽ�Ƃ�
	case WM_KEYUP:
	{
		//�������̓I�u�W�F�N�g�ɑ���A��Ԃ������ł���
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	//�A�v���P�[�V�����͂��̑��̃��b�Z�[�W���g�p���Ȃ����߁A�f�t�H���g�̃��b�Z�[�W�n���h���ɑ���
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void System::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//���̃I�u�W�F�N�g�̊O���|�C���^���擾
	ApplicationHandle = this;

	//���̃A�v���P�[�V�����̃C���X�^���X���擾
	m_hinstance = GetModuleHandle(NULL);

	//�A�v���P�[�V�����ɖ��O��^����
	m_applicationName = L"Engine";

	//�E�B���h�E�N���X�Ə����ݒ�𗧂��グ��
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//�E�B���h�E�N���X��o�^
	RegisterClassEx(&wc);

	//�N���C�A���g�̃f�X�N�g�b�v��ʂ̉𑜓x�����肵�܂��B
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//�t���X�N���[�����[�h�܂��̓E�B���h�E���[�h�Ŏ��s���Ă��邩�ǂ����ɂ���āA��ʂ̐ݒ���s��
	if (FULL_SCREEN)
	{
		//�S��ʂ̏ꍇ�A��ʂ����[�U�̃f�X�N�g�b�v��32�r�b�g�̍ő�T�C�Y�ɐݒ�
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//�f�B�X�v���C�̐ݒ���t���X�N���[���ɕς���
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//�E�B���h�E�̍��΂ߏ�̊p��ݒ肷��
		posX = posY = 0;
	}
	else
	{
		//�����E�B���h�E���[�h�Ȃ�𑜓x��800x600�ɐݒ�
		screenWidth = 800;
		screenHeight = 600;

		//�X�N���[���̒��S�ɃE�B���h�E��u��
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//��ʐݒ�ŃE�B���h�E���쐬���A�n���h�����擾
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//�E�B���h�E����ʏ�Ɏ����グ�A���C���t�H�[�J�X�Ƃ��Đݒ�
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//�}�E�X�J�[�\�����B��
	ShowCursor(false);

	return;
}

void System::ShutdownWindows()
{
	//�}�E�X�J�[�\����\��
	ShowCursor(true);

	//�t���X�N���[�����[�h������������ݒ肵�Ȃ���
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//�E�B���h�E���폜
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//�A�v���P�[�V�����̃C���X�^���X���폜
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//���̃N���X�ւ̃|�C���^���폜
	ApplicationHandle = NULL;

	return;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	//�E�B���h�E���j������Ă��邩�ǂ������m�F
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	//�E�B���h�E�������Ă��邩�ǂ����m�F
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	//���ׂĂ̂ق��̃��b�Z�[�W�̓V�X�e���N���X�̃��b�Z�[�W�n���h����ʂ�
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
