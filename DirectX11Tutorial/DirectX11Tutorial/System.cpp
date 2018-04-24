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

//アプリケーションのすべての設定を行います。
bool System::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//関数に送る前に初期化 Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	//Windows APIを初期化
	InitializeWindows(screenWidth, screenHeight);

	//インプットオブジェクトの生成、こいつはユーザーからキーボード入力を得るために使う Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;

	if (!m_Input)
	{
		return false;
	}

	//インプットオブジェクトを初期化
	m_Input->Initialize();

	//グラフィックスオブジェクトの生成、こいつはこのアプリケーションのためのすべてのレンダリングを操作する Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new Graphics;

	if (!m_Graphics)
	{
		return false;
	}

	//グラフィックスオブジェクトを初期化
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	//ここまでの間ですべて問題なくいけば進める。
	return true;
}

void System::Shutdown()
{
	//グラフィックスオブジェクトを開放
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//インプットオブジェクトを開放
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//ウィンドウを閉じる
	ShutdownWindows();
}

void System::Run()
{
	MSG msg;
	bool done, result;

	//メッセージ構造体を初期化
	ZeroMemory(&msg, sizeof(MSG));

	//ウィンドウかユーザーから終了メッセージが出るまでループする
	done = false;
	while (!done)
	{
		//ウィンドウメッセージを操作
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//ウィンドウがアプリケーションを終了するように指示したら終了
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		//それ以外の場合はフレーム処理
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

	//ユーザーがエスケープキーを押したか、アプリケーションを終了したがっているかを確認
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//フレーム処理をグラフィックスオブジェクトに行う
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
		//キーボード上のキーが押されたかどうかを確認
	case WM_KEYDOWN:
	{
		//それがインプットオブジェクトに送られ、状態を記録できる
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	//キーボード上の押されていたキーが離れたとき
	case WM_KEYUP:
	{
		//それを入力オブジェクトに送り、状態を解除できる
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	//アプリケーションはその他のメッセージを使用しないため、デフォルトのメッセージハンドラに送る
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

	//このオブジェクトの外部ポインタを取得
	ApplicationHandle = this;

	//このアプリケーションのインスタンスを取得
	m_hinstance = GetModuleHandle(NULL);

	//アプリケーションに名前を与える
	m_applicationName = L"Engine";

	//ウィンドウクラスと初期設定を立ち上げる
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

	//ウィンドウクラスを登録
	RegisterClassEx(&wc);

	//クライアントのデスクトップ画面の解像度を決定します。
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//フルスクリーンモードまたはウィンドウモードで実行しているかどうかによって、画面の設定を行う
	if (FULL_SCREEN)
	{
		//全画面の場合、画面をユーザのデスクトップと32ビットの最大サイズに設定
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//ディスプレイの設定をフルスクリーンに変える
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//ウィンドウの左斜め上の角を設定する
		posX = posY = 0;
	}
	else
	{
		//もしウィンドウモードなら解像度を800x600に設定
		screenWidth = 800;
		screenHeight = 600;

		//スクリーンの中心にウィンドウを置く
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//画面設定でウィンドウを作成し、ハンドルを取得
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//ウィンドウを画面上に持ち上げ、メインフォーカスとして設定
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//マウスカーソルを隠す
	ShowCursor(false);

	return;
}

void System::ShutdownWindows()
{
	//マウスカーソルを表示
	ShowCursor(true);

	//フルスクリーンモードを解除したら設定しなおす
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//ウィンドウを削除
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//アプリケーションのインスタンスを削除
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//このクラスへのポインタを削除
	ApplicationHandle = NULL;

	return;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	//ウィンドウが破棄されているかどうかを確認
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	//ウィンドウが閉じられているかどうか確認
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	//すべてのほかのメッセージはシステムクラスのメッセージハンドラを通る
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
