#include "Input.h"

Input::Input()
{
}


Input::Input(const Input& other)
{
}


Input::~Input()
{
}

void Input::Initialize()
{
	int i;

	//�������A������Ă��Ȃ����ׂẴL�[��������
	for (i = 0; i < 256;i++)
	{
		m_keys[i] = false;
	}
	
	return;
}

void Input::KeyDown(unsigned int input)
{
	//�����L�[��������Ă�����L�[�z��ɂ��̏�Ԃ�ۑ�

	m_keys[input] = true;
	return;
}

void Input::KeyUp(unsigned int input)
{
	//�����L�[��������Ă�����L�[�z��̒��̂��̏�Ԃ��폜

	m_keys[input] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	//�L�[�̏�Ԃ�Ԃ�

	return m_keys[key];
}