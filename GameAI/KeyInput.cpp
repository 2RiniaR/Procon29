#include "KeyInput.h"
#include "DxLib.h"

int KeyInput::Key[256];

// �L�[�̓��͂��X�V
void KeyInput::Update() {

	char tmpKey[256];

	GetHitKeyStateAll(tmpKey);

	for (int i = 0; i < 256; i++) {
		if (tmpKey[i] != 0) Key[i]++;
		else				Key[i] = 0;
	}

}