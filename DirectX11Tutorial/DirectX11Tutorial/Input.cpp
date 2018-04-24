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

	//解放され、押されていないすべてのキーを初期化
	for (i = 0; i < 256;i++)
	{
		m_keys[i] = false;
	}
	
	return;
}

void Input::KeyDown(unsigned int input)
{
	//もしキーが押されていたらキー配列にその状態を保存

	m_keys[input] = true;
	return;
}

void Input::KeyUp(unsigned int input)
{
	//もしキーが離されていたらキー配列の中のその状態を削除

	m_keys[input] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	//キーの状態を返す

	return m_keys[key];
}