#include "System.h"
#include <fstream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* system;
	bool result;

	//システムオブジェクトを生成
	system = new System;

	if (!system)
	{
		return 0;
	}

	//システムオブジェクトを初期化して実行
	result = system->Initialize();

	if (result)
	{
		system->Run();
	}

	//システムのオブジェクトを終了し解放
	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}