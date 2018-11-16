#include <cmath>
#include "DxLib.h"
#include "KeyInput.h"
#include "FlameCounter.h"
#include "GameManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	/*--------------------------------------*/
	/*              Initialize              */
	/*--------------------------------------*/

	//ウィンドウのタイトルを設定する
	SetMainWindowText("Procon29_ibaraki");

	//サイズ変更を可能にする
	SetWindowSizeChangeEnableFlag(TRUE, TRUE);

	//ウィンドウモードを有効にする
	ChangeWindowMode(TRUE);
	
	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)	return -1;											

	//ウィンドウの裏画面設定
	SetDrawScreen(DX_SCREEN_BACK);

	//解像度変更
	SetGraphMode(960, 540, 16);

	//クラスのインスタンス化
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

	//各モジュールの終了処理
	gamemanager.Finalize();

	// ＤＸライブラリ使用の終了処理
	DxLib_End();

	// ソフトの終了
	return 0; 

}
