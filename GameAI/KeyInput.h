#pragma once

// �L�[�̓��͏����Ǘ�����N���X
class KeyInput{

private:
	static int Key[256];

public:
	static void Update();
	static int GetKeyStatus(int KeyCode) { return Key[KeyCode]; };

};

