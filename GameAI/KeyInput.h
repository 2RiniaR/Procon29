#pragma once

// キーの入力情報を管理するクラス
class KeyInput{

private:
	static int Key[256];

public:
	static void Update();
	static int GetKeyStatus(int KeyCode) { return Key[KeyCode]; };

};

