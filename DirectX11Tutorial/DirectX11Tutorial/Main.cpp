#include "System.h"
#include <fstream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* system;
	bool result;

	//�V�X�e���I�u�W�F�N�g�𐶐�
	system = new System;

	if (!system)
	{
		return 0;
	}

	//�V�X�e���I�u�W�F�N�g�����������Ď��s
	result = system->Initialize();

	if (result)
	{
		system->Run();
	}

	//�V�X�e���̃I�u�W�F�N�g���I�������
	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}