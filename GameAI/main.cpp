#include <cmath>
#include "DxLib.h"
#include "KeyInput.h"
#include "FlameCounter.h"
#include "GameManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	/*--------------------------------------*/
	/*              Initialize              */
	/*--------------------------------------*/

	//�E�B���h�E�̃^�C�g����ݒ肷��
	SetMainWindowText("Procon29_ibaraki");

	//�T�C�Y�ύX���\�ɂ���
	SetWindowSizeChangeEnableFlag(TRUE, TRUE);

	//�E�B���h�E���[�h��L���ɂ���
	ChangeWindowMode(TRUE);
	
	// �c�w���C�u��������������
	if (DxLib_Init() == -1)	return -1;											

	//�E�B���h�E�̗���ʐݒ�
	SetDrawScreen(DX_SCREEN_BACK);

	//�𑜓x�ύX
	SetGraphMode(960, 540, 16);

	//�N���X�̃C���X�^���X��
	FlameCounter flamecounter;
	GameManager gamemanager;

	gamemanager.Initialize();

	/*--------------------------------------*/
	/*               Mainloop               */
	/*--------------------------------------*/

	while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {

		flamecounter.FlameCount();

		KeyInput::Update();
		gamemanager.Update();
		gamemanager.Draw();

		flamecounter.Wait();

	}

	/*--------------------------------------*/
	/*               Finalize               */
	/*--------------------------------------*/

	//�e���W���[���̏I������
	gamemanager.Finalize();

	// �c�w���C�u�����g�p�̏I������
	DxLib_End();

	// �\�t�g�̏I��
	return 0; 

}
