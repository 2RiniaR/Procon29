#include <array>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <random>
#include "DxLib.h"
#include "def.h"
#include "KeyInput.h"
#include "Agent.h"
#include "TreeSearcher.h"
#include "Team.h"
#include "SystemKeeper.h"
#include "Board.h"
#include "Cell.h"
#include "FileReader.h"
#include "GameManager.h"

SystemKeeper* GameManager::System;
double GameManager::BoardAve;

//0.0～1.0の乱数を返す
double Drand() {

	return (double)rand_f(mt) / RANDSEEDMAX;

}

//コンストラクタ
GameManager::GameManager() {
	
	

}

//初期処理
void GameManager::Initialize() {

	//状態のリセット
	NowGameEnd = false;
	AIState = 0;
	MyAdvantage = 0;
	Winner = WHITE;
	isCustomMode = 0;

	//乱数の種をまく
	srand((unsigned int)time(NULL));

	//ファイル読み込み
	FileReader::ReadBoardInfo("QR/board.txt");
	FileReader::ReadTurnInfo("Data/Input/turn.txt");
	FileReader::ParamFileRead("Data/Input/param.txt");

	//システムの初期化
	System = new SystemKeeper(FileReader::GetEndTurn());

	//プレイヤーの初期化
	array< array<Pos2D, AGENTNUM>, TEAMNUM > initpos = FileReader::GetInitPos();
	GameTeams = new array<Team, TEAMNUM>{
		Team(RED, initpos[RED]),
		Team(BLUE, initpos[BLUE])
	};

	AgentsArray = {
		&(GameTeams->at(RED).Agents[AGENT1]),
		&(GameTeams->at(RED).Agents[AGENT2]),
		&(GameTeams->at(BLUE).Agents[AGENT1]),
		&(GameTeams->at(BLUE).Agents[AGENT2])
	};

	//盤面の初期化
	GameBoard = new Board(FileReader::GetReadBoard());
	GameBoard->Update(AgentsArray);
	GameBoard->CalcAverage();
	BoardAve = GameBoard->GetAverage();

	//システムの初期化2
	System->CalcScore(*GameBoard);
	FileReader::Cparam_Average(BoardAve);

	//フォントの初期化
	font1 = CreateFontToHandle(NULL, 12, 3);
	font2 = CreateFontToHandle(NULL, 24, 4);
	font3 = CreateFontToHandle(NULL, 32, 5);
	font4 = CreateFontToHandle(NULL, 28, 5);
	font5 = CreateFontToHandle(NULL, 18, 3);

	//描画ステータスの初期化
	SelectingAgent = 0;
	Rotation = 0;
	isReverse = false;

}

//終了処理
void GameManager::Finalize() {

}

//更新処理
void GameManager::Update() {

	//ゲーム終了のチェック
	//if (NowGameEnd == true) return;

	int x, y;
	Pos2D tmppos;


	//Rが押されたら盤面を時計回りに回転
	if (KeyInput::GetKeyStatus(KEY_INPUT_R) == 1) Rotation = (Rotation + 1) % 4;
	//Lが押されたら盤面を反時計回りに回転
	if (KeyInput::GetKeyStatus(KEY_INPUT_E) == 1) Rotation = (Rotation + 3) % 4;
	//Wが押されたら自分と相手の色を入れ替える
	if (KeyInput::GetKeyStatus(KEY_INPUT_W) == 1) isReverse = !isReverse;


	//1が押されたら赤-1に注目
	if (KeyInput::GetKeyStatus(KEY_INPUT_1) == 1) SelectingAgent = 0;
	//2が押されたら赤-2に注目
	if (KeyInput::GetKeyStatus(KEY_INPUT_2) == 1) SelectingAgent = 1;
	//3が押されたら青-1に注目
	if (KeyInput::GetKeyStatus(KEY_INPUT_3) == 1) SelectingAgent = 2;
	//4が押されたら青-2に注目
	if (KeyInput::GetKeyStatus(KEY_INPUT_4) == 1) SelectingAgent = 3;


	//マウスの入力を受け付ける
	int button, cx, cy, log;
	if (GetMouseInputLog2(&button, &cx, &cy, &log, true) == 0) {

		//左クリック
		if ((button & MOUSE_INPUT_LEFT) != 0 && log == MOUSE_INPUT_LOG_DOWN) {

			if (400 < cx && cx < 450 && 250 < cy && cy < 300) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_UPLEFT));
			if (450 < cx && cx < 500 && 250 < cy && cy < 300) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_UP));
			if (500 < cx && cx < 550 && 250 < cy && cy < 300) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_UPRIGHT));
			if (400 < cx && cx < 450 && 300 < cy && cy < 350) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_LEFT));
			if (450 < cx && cx < 500 && 300 < cy && cy < 350) AgentsArray[SelectingAgent]->SetAction(RotateActRev(STAY));
			if (500 < cx && cx < 550 && 300 < cy && cy < 350) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_RIGHT));
			if (400 < cx && cx < 450 && 350 < cy && cy < 400) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_DOWNLEFT));
			if (450 < cx && cx < 500 && 350 < cy && cy < 400) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_DOWN));
			if (500 < cx && cx < 550 && 350 < cy && cy < 400) AgentsArray[SelectingAgent]->SetAction(RotateActRev(MOVE_DOWNRIGHT));

		}

		//右クリック
		if ((button & MOUSE_INPUT_RIGHT) != 0 && log == MOUSE_INPUT_LOG_DOWN) {

			if (400 < cx && cx < 450 && 250 < cy && cy < 300) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_UPLEFT));
			if (450 < cx && cx < 500 && 250 < cy && cy < 300) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_UP));
			if (500 < cx && cx < 550 && 250 < cy && cy < 300) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_UPRIGHT));
			if (400 < cx && cx < 450 && 300 < cy && cy < 350) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_LEFT));
			if (450 < cx && cx < 500 && 300 < cy && cy < 350) AgentsArray[SelectingAgent]->SetAction(RotateActRev(STAY));
			if (500 < cx && cx < 550 && 300 < cy && cy < 350) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_RIGHT));
			if (400 < cx && cx < 450 && 350 < cy && cy < 400) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_DOWNLEFT));
			if (450 < cx && cx < 500 && 350 < cy && cy < 400) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_DOWN));
			if (500 < cx && cx < 550 && 350 < cy && cy < 400) AgentsArray[SelectingAgent]->SetAction(RotateActRev(DESTROY_DOWNRIGHT));

		}

	}

	//GとLが押されている間、カスタムモード
	if (KeyInput::GetKeyStatus(KEY_INPUT_G) == 1) {
		isCustomMode = (isCustomMode + 1) % 2;
	}

	if (isCustomMode == 1) {

		//上キーでターンを増やす
		if (KeyInput::GetKeyStatus(KEY_INPUT_UP) == 1) {
			System->AddTurn(1);
			GameTeams->at(RED).ResetTree();
		}

		//下キーでターンを減らす
		if (KeyInput::GetKeyStatus(KEY_INPUT_DOWN) == 1) {
			System->AddTurn(-1);
			GameTeams->at(RED).ResetTree();
		}

		//if (GetMouseInputLog2(&button, &cx, &cy, &log, true) == 0) {

		x = cx / 32;
		y = cy / 32;
		tmppos = RotatePosRev(Pos2D(x, y));

		if (0 <= tmppos.x && tmppos.x < GameBoard->GetSize().x && 0 <= tmppos.y && tmppos.y < GameBoard->GetSize().y) {


			//マスを左クリックで赤→青→白→赤と変化させる
			if ((button & MOUSE_INPUT_LEFT) != 0 && log == MOUSE_INPUT_LOG_DOWN) {
				GameBoard->ToggleCell(tmppos);
				GameTeams->at(RED).ResetTree();
			}

			//右クリックで選択中のエージェントを移動させる
			if ((button & MOUSE_INPUT_RIGHT) != 0 && log == MOUSE_INPUT_LOG_DOWN) {
				AgentsArray[SelectingAgent]->ChangePos(tmppos);
				GameTeams->at(RED).ResetTree();
			}

		}

		//		}
	}
	

	//Cが押されたらパラメータを再読み込み
	if (KeyInput::GetKeyStatus(KEY_INPUT_C) == 1) {

		FileReader::ParamFileRead("Data/Input/param.txt");

	}


	//Kが押されたら行動を決定
	if (KeyInput::GetKeyStatus(KEY_INPUT_K) == 1) {

		//プレイヤー行動の制限・衝突確認
		System->CheckConflict(*GameBoard, AgentsArray);

		/* この時点で行動が確定する */

		//ボードの更新
		GameBoard->Update(AgentsArray);

		//ツリー更新
		GameTeams->at(RED).UpdateTree(array< array<int, AGENTNUM>, TEAMNUM>{
			GameTeams->at(RED).GetAgentAction(),
			GameTeams->at(BLUE).GetAgentAction()
		});

		//エージェントの更新
		GameTeams->at(RED).Update();
		GameTeams->at(BLUE).Update();

		//スコア計算
		System->CalcScore(*GameBoard);

		//ゲーム終了判定
		if (System->isGameFin() == true) {
			NowGameEnd = true;
			Winner = System->JudgeWinner();
		}

		//ターン更新
		System->Update();

	}


	//AIによる思考
	if (AIState == 1) {
		//プレイヤーが行動を思考する
		GameTeams->at(RED).Action(*GameBoard, GameTeams->at(NEXTCOLOR(RED)));
		MyAdvantage = GameTeams->at(RED).GetAdvantage();
		AIState = 0;
	}

	//AIツリーリセット
	if (AIState == 2) {
		GameTeams->at(RED).ResetTree();
		AIState = 0;
	}

	//Aが押されたらAI探索
	if (KeyInput::GetKeyStatus(KEY_INPUT_A) == 1) {
		//AI探索のフラグを立てる
		AIState = 1;
	}

	//Xが押されたらAIツリーリセット
	if (KeyInput::GetKeyStatus(KEY_INPUT_X) == 1) {
		//AIツリーリセットのフラグを立てる
		AIState = 2;
	}
	
}

// 描画処理
void GameManager::Draw() {

	//背景描画
	DrawBox(0, 0, 1280, 960, GetColor(255, 255, 255), true);

	int x, y, i, j, w, h;
	Pos2D tmppos;

	w = GameBoard->GetSize().x;
	h = GameBoard->GetSize().y;

	//-------------------左側に盤面を描画----------------------
	DrawBox(0, 0, 384, 384, GetColor(0, 0, 0), false);
	for (i = 0; i < GameBoard->GetSize().x; i++) {
		for (j = 0; j < GameBoard->GetSize().y; j++) {

			tmppos = RotatePos(Pos2D(i, j));
			x = 32 * tmppos.x;
			y = 32 * tmppos.y;

			//マスに色を塗る
			switch (GameBoard->CellInfo(Pos2D(i, j)).Color) {

			case RED:
				if(isReverse == false)	DrawBox(x, y, x + 32, y + 32, GetColor(255, 150, 150), true);
				else					DrawBox(x, y, x + 32, y + 32, GetColor(150, 150, 255), true);
				break;

			case BLUE:
				if (isReverse == false)	DrawBox(x, y, x + 32, y + 32, GetColor(150, 150, 255), true);
				else					DrawBox(x, y, x + 32, y + 32, GetColor(255, 150, 150), true);
				break;

			default:
				DrawBox(x, y, x + 32, y + 32, GetColor(200, 200, 200), true);
				break;

			}

			//マスの点数を表示する
			if (GameBoard->CellInfo(Pos2D(i, j)).Point > BoardAve) {
				DrawBox(x, y, x + 32, y + 32, GetColor(0, 120, 0), false);
				DrawBox(x+1, y+1, x + 31, y + 31, GetColor(0, 120, 0), false);
			}
			else {
				DrawBox(x, y, x + 32, y + 32, GetColor(0, 0, 0), false);
			}
			DrawFormatStringToHandle(x + 4, y + 4, GetColor(0, 0, 0), font1, "%d", GameBoard->CellInfo(Pos2D(i, j)).Point);
		}
	}


	//----------------盤面にエージェントを描画-------------------
	if (isReverse == false) {
		tmppos = RotatePos(AgentsArray[0]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(255, 0, 0), true);

		tmppos = RotatePos(AgentsArray[1]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(255, 0, 255), true);

		tmppos = RotatePos(AgentsArray[2]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(0, 0, 255), true);

		tmppos = RotatePos(AgentsArray[3]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(0, 255, 255), true);
	}
	else {
		tmppos = RotatePos(AgentsArray[2]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(255, 0, 0), true);

		tmppos = RotatePos(AgentsArray[3]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(255, 0, 255), true);

		tmppos = RotatePos(AgentsArray[0]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(0, 0, 255), true);

		tmppos = RotatePos(AgentsArray[1]->GetPos());
		DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(0, 255, 255), true);
	}

	tmppos = RotatePos(AgentsArray[SelectingAgent]->GetPos());
	DrawCircle(tmppos.x * 32 + 16, tmppos.y * 32 + 18, 9, GetColor(255, 255, 0), false, 2);

	for (i = 0; i < 4; i++) {
		DrawActMark(*AgentsArray[i]);
	}


	//-----------------左下に得点を表示------------------
	DrawBox(0, 400, 384, 530, GetColor(0, 0, 0), false);
	DrawFormatStringToHandle(10, 420, GetColor(0, 0, 0), font4, "Turn : %2d/%2d", System->GetNowTurn(), System->GetEndTurn());

	if (isReverse == false) {
		DrawFormatStringToHandle(10, 460, GetColor(128, 0, 0), font4, "Red  : %3d(T:%3d, A:%3d)", System->GetScore(RED_TOTAL), System->GetScore(RED_TILE), System->GetScore(RED_AREA));
		DrawFormatStringToHandle(10, 495, GetColor(0, 0, 128), font4, "Blue : %3d(T:%3d, A:%3d)", System->GetScore(BLUE_TOTAL), System->GetScore(BLUE_TILE), System->GetScore(BLUE_AREA));
	}
	else {
		DrawFormatStringToHandle(10, 460, GetColor(128, 0, 0), font4, "Red  : %3d(T:%3d, A:%3d)", System->GetScore(BLUE_TOTAL), System->GetScore(BLUE_TILE), System->GetScore(BLUE_AREA));
		DrawFormatStringToHandle(10, 495, GetColor(0, 0, 128), font4, "Blue : %3d(T:%3d, A:%3d)", System->GetScore(RED_TOTAL), System->GetScore(RED_TILE), System->GetScore(RED_AREA));
	}



	//------------右上にエージェント情報を表示--------------
	DrawBox(400, 0, 960, 140, GetColor(0, 0, 0), false);

	if (isReverse == false) {
		DrawFormatStringToHandle(440 + 0, 20, GetColor(255, 0, 0), font3, "RED-1");
		DrawFormatStringToHandle(440 + 130, 20, GetColor(255, 0, 255), font3, "RED-2");
		DrawFormatStringToHandle(440 + 260, 20, GetColor(0, 0, 255), font3, "BLUE-1");
		DrawFormatStringToHandle(440 + 390, 20, GetColor(0, 255, 255), font3, "BLUE-2");
	}
	else {
		DrawFormatStringToHandle(440 + 260, 20, GetColor(255, 0, 0), font3, "RED-1");
		DrawFormatStringToHandle(440 + 390, 20, GetColor(255, 0, 255), font3, "RED-2");
		DrawFormatStringToHandle(440 + 0, 20, GetColor(0, 0, 255), font3, "BLUE-1");
		DrawFormatStringToHandle(440 + 130, 20, GetColor(0, 255, 255), font3, "BLUE-2");
	}

	DrawFormatStringToHandle(420 + 130 * SelectingAgent, 20, GetColor(0, 128, 0), font3, ">");

	for (i = 0; i < 4; i++) {

		//位置
		tmppos = RotatePos(AgentsArray[i]->GetPos());
		DrawFormatStringToHandle(440 + 130 * i, 60, GetColor(0, 0, 0), font2, "(%d, %d)", tmppos.x, tmppos.y);

		//行動予定
		DrawFormatStringToHandle(440 + 130 * i, 100, GetColor(0, 0, 0), font2, "%s", ActionStr(RotateAct(AgentsArray[i]->GetAction())));

	}


	//---------------------右に操作パネルを表示-----------------------
	DrawBox(400, 250, 550, 400, GetColor(100, 255, 100), true);
	DrawBox(450, 300, 500, 350, GetColor(180, 180, 180), true);
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			DrawBox(400 + 50 * i, 250 + 50 * j, 400 + 50 * i + 50, 250 + 50 * j + 50, GetColor(0, 0, 0), false);
		}
	}


	//---------------------右にトランプを表示--------------------
	DrawBox(400, 150, 960, 240, GetColor(0, 0, 0), false);
	DrawFormatStringToHandle(410, 190, GetColor(0, 0, 0), font5, "現状評価値：%lf", MyAdvantage);
	Trump(GameTeams->at(RED).GetAgentAction());


	//---------------------右に操作説明を表示-------------------
	DrawBox(560, 250, 960, 400, GetColor(0, 0, 0), false);
	DrawFormatStringToHandle(570, 260, GetColor(0, 0, 0), font5, "A :AI探索");
	DrawFormatStringToHandle(570, 280, GetColor(0, 0, 0), font5, "K :ターン更新");
	DrawFormatStringToHandle(570, 300, GetColor(0, 0, 0), font5, "R :盤面右回転");
	DrawFormatStringToHandle(570, 320, GetColor(0, 0, 0), font5, "E :盤面左回転");
	DrawFormatStringToHandle(570, 340, GetColor(0, 0, 0), font5, "W :色反転");
	DrawFormatStringToHandle(570, 360, GetColor(0, 0, 0), font5, "X :ツリー初期化");
	DrawFormatStringToHandle(570, 380, GetColor(0, 0, 0), font5, "C :変数リロード");

	
	//-------------------右下に情報枠を表示--------------------
	DrawBox(400, 410, 960, 530, GetColor(0, 0, 0), false);
	//回転状況を表示
	DrawFormatStringToHandle(420, 420, GetColor(0, 0, 0), font2, "回転：%d度", Rotation*90);
	//色反転状況を表示
	if(isReverse == false)	DrawFormatStringToHandle(600, 420, GetColor(255, 0, 0), font2, "色反転：なし（自分は赤）");
	else					DrawFormatStringToHandle(600, 420, GetColor(0, 0, 255), font2, "色反転：あり（自分は青）");

	DrawFormatStringToHandle(420, 460, GetColor(0, 0, 0), font5, "Rollout回数：%d", FileReader::GetRolloutNum());
	DrawFormatStringToHandle(420, 480, GetColor(0, 0, 0), font5, "探索深度   ：%d", FileReader::GetSearchDeep());
	DrawFormatStringToHandle(420, 500, GetColor(0, 0, 0), font5, "探索補正値 ：%lf", FileReader::GetCostParam());

	DrawFormatStringToHandle(660, 500, GetColor(0, 0, 0), font5, "盤面平均値 ：%lf", BoardAve);


	//-------------------------勝敗を表示-------------------------
	if (NowGameEnd == 1) {
		if (isReverse == false) {
			if (Winner == RED)  DrawFormatStringToHandle(250, 420, GetColor(255, 0, 0), font3, "Red Won!");
			if (Winner == BLUE) DrawFormatStringToHandle(250, 420, GetColor(0, 0, 255), font3, "Blue Won!");
		}
		else {
			if (Winner == BLUE)  DrawFormatStringToHandle(250, 420, GetColor(255, 0, 0), font3, "Red Won!");
			if (Winner == RED) DrawFormatStringToHandle(250, 420, GetColor(0, 0, 255), font3, "Blue Won!");
		}
	}


	//--------------------AI探索中-----------------------
	if (AIState == 1) {
		DrawFormatStringToHandle(410, 215, GetColor(0, 0, 0), font5, "AI探索中...");
	}
	else if (AIState == 2) {
		DrawFormatStringToHandle(410, 215, GetColor(0, 0, 0), font5, "探索初期化中");
	}

	if (isCustomMode == 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		DrawBox(0, 0, 960, 540, GetColor(255, 120, 255), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

}

//行動を文字列で表現する
const char* GameManager::ActionStr(int act) {

	switch (act) {

	case MOVE_UP:
		return "動：上";

	case MOVE_UPRIGHT:
		return "動：右上";

	case MOVE_RIGHT:
		return "動：右";

	case MOVE_DOWNRIGHT:
		return "動：右下";

	case MOVE_DOWN:
		return "動：下";

	case MOVE_DOWNLEFT:
		return "動：左下";

	case MOVE_LEFT:
		return "動：左";

	case MOVE_UPLEFT:
		return "動：左上";

	case DESTROY_UP:
		return "壊：上";

	case DESTROY_UPRIGHT:
		return "壊：右上";

	case DESTROY_RIGHT:
		return "壊：右";

	case DESTROY_DOWNRIGHT:
		return "壊：右下";

	case DESTROY_DOWN:
		return "壊：下";

	case DESTROY_DOWNLEFT:
		return "壊：左下";

	case DESTROY_LEFT:
		return "壊：左";

	case DESTROY_UPLEFT:
		return "壊：左上";

	default:
		return "停止";

	};

}

//トランプの値を表示する
void GameManager::Trump(const array<int, AGENTNUM> &act) {

	//AGENT1は黒、AGENT2は赤
	const char* str1 = NULL;
	const char* str2 = NULL;

	switch (RotateAct(act[AGENT1])) {

	case MOVE_UP:
		str1 = "黒1";
		break;

	case MOVE_UPRIGHT:
		str1 = "黒2";
		break;

	case MOVE_RIGHT:
		str1 = "黒3";
		break;

	case MOVE_DOWNRIGHT:
		str1 = "黒4";
		break;

	case MOVE_DOWN:
		str1 = "黒5";
		break;

	case MOVE_DOWNLEFT:
		str1 = "黒6";
		break;

	case MOVE_LEFT:
		str1 = "黒7";
		break;

	case MOVE_UPLEFT:
		str1 = "黒8";
		break;

	case DESTROY_UP:
		str1 = "黒1(ヨコ)";
		break;

	case DESTROY_UPRIGHT:
		str1 = "黒2(ヨコ)";
		break;

	case DESTROY_RIGHT:
		str1 = "黒3(ヨコ)";
		break;

	case DESTROY_DOWNRIGHT:
		str1 = "黒4(ヨコ)";
		break;

	case DESTROY_DOWN:
		str1 = "黒5(ヨコ)";
		break;

	case DESTROY_DOWNLEFT:
		str1 = "黒6(ヨコ)";
		break;

	case DESTROY_LEFT:
		str1 = "黒7(ヨコ)";
		break;

	case DESTROY_UPLEFT:
		str1 = "黒8(ヨコ)";
		break;

	default:
		str1 = "ウラ";
		break;

	};

	switch (RotateAct(act[AGENT2])) {

	case MOVE_UP:
		str2 = "赤1";
		break;

	case MOVE_UPRIGHT:
		str2 = "赤2";
		break;

	case MOVE_RIGHT:
		str2 = "赤3";
		break;

	case MOVE_DOWNRIGHT:
		str2 = "赤4";
		break;

	case MOVE_DOWN:
		str2 = "赤5";
		break;

	case MOVE_DOWNLEFT:
		str2 = "赤6";
		break;

	case MOVE_LEFT:
		str2 = "赤7";
		break;

	case MOVE_UPLEFT:
		str2 = "赤8";
		break;

	case DESTROY_UP:
		str2 = "赤1(ヨコ)";
		break;

	case DESTROY_UPRIGHT:
		str2 = "赤2(ヨコ)";
		break;

	case DESTROY_RIGHT:
		str2 = "赤3(ヨコ)";
		break;

	case DESTROY_DOWNRIGHT:
		str2 = "赤4(ヨコ)";
		break;

	case DESTROY_DOWN:
		str2 = "赤5(ヨコ)";
		break;

	case DESTROY_DOWNLEFT:
		str2 = "赤6(ヨコ)";
		break;

	case DESTROY_LEFT:
		str2 = "赤7(ヨコ)";
		break;

	case DESTROY_UPLEFT:
		str2 = "赤8(ヨコ)";
		break;

	default:
		str2 = "ウラ";
		break;

	};

	DrawFormatStringToHandle(410, 160, GetColor(0, 0, 0), font2, "%s + ", str1);
	DrawFormatStringToHandle(560, 160, GetColor(255, 0, 0), font2, "%s", str2);

}


Pos2D GameManager::RotatePosRev(const Pos2D &pos) {

	Pos2D ans = {0 ,0};
	Pos2D size = GameBoard->GetSize();

	switch (Rotation) {
	case 0:
		ans.x = pos.x;
		ans.y = pos.y;
		break;

	case 1:
		ans.x = pos.y; 
		ans.y = (size.y - 1) - pos.x;

		break;

	case 2:
		ans.x = (size.x - 1) - pos.x;
		ans.y = (size.y - 1) - pos.y;
		break;

	case 3:
		ans.x = (size.x - 1) - pos.y;
		ans.y = pos.x;
		break;
	}

	return ans;

}


Pos2D GameManager::RotatePos(const Pos2D &pos) {

	Pos2D ans = { 0 ,0 };
	Pos2D size = GameBoard->GetSize();

	switch (Rotation) {
	case 0:
		ans.x = pos.x;
		ans.y = pos.y;
		break;

	case 1:
		ans.x = (size.y - 1) - pos.y;
		ans.y = pos.x;
		break;

	case 2:
		ans.x = (size.x - 1) - pos.x;
		ans.y = (size.y - 1) - pos.y;
		break;

	case 3:
		ans.x = pos.y;
		ans.y = (size.x - 1) - pos.x;
		break;
	}

	return ans;

}


int GameManager::RotateAct(const int &act) {

	//停止またはその他
	if (act < MOVE_UP || DESTROY_UPLEFT < act) return act;

	switch (Rotation) {
	case 0:
		return act;

	case 1:
		if (MOVE_UP <= act && act <= MOVE_UPLEFT) return (((act - 1) + 2) % 8) + 1;
		else return(((act - 9) + 2) % 8) + 9;

	case 2:
		if (MOVE_UP <= act && act <= MOVE_UPLEFT) return (((act - 1) + 4) % 8) + 1;
		else return(((act - 9) + 4) % 8) + 9;

	case 3:
		if (MOVE_UP <= act && act <= MOVE_UPLEFT) return (((act - 1) + 6) % 8) + 1;
		else return(((act - 9) + 6) % 8) + 9;
		
	}

	return act;

}

int GameManager::RotateActRev(const int &act) {

	//停止またはその他
	if (act < MOVE_UP || DESTROY_UPLEFT < act) return act;

	switch (Rotation) {
	case 0:
		return act;

	case 1:
		if (MOVE_UP <= act && act <= MOVE_UPLEFT) return (((act - 1) + 6) % 8) + 1;
		else return(((act - 9) + 6) % 8) + 9;

	case 2:
		if (MOVE_UP <= act && act <= MOVE_UPLEFT) return (((act - 1) + 4) % 8) + 1;
		else return(((act - 9) + 4) % 8) + 9;

	case 3:
		if (MOVE_UP <= act && act <= MOVE_UPLEFT) return (((act - 1) + 2) % 8) + 1;
		else return(((act - 9) + 2) % 8) + 9;

	}

	return act;

}


void GameManager::DrawActMark(const Agent &ag) {

	//停止またはその他
	if (ag.NextAction < MOVE_UP || DESTROY_UPLEFT < ag.NextAction) return;

	Pos2D Now, After;
	Now = RotatePos(ag.GetPos());
	After = RotatePos(ag.PosWithAction());

	DrawLine(32 * Now.x + 16, 32 * Now.y + 16+2, 32 * After.x + 16, 32 * After.y + 16+2, GetColor(90, 90, 90), 2);

	if (MOVE_UP <= ag.NextAction && ag.NextAction <= MOVE_UPLEFT) {
		DrawCircle(32 * After.x + 16, 32 * After.y + 16+2, 4, GetColor(60, 60, 60), false, 1);
	}
	else {
		DrawLine(32 * After.x + 12, 32 * After.y + 12+2, 32 * After.x + 20, 32 * After.y + 20+2, GetColor(60, 60, 60), 2);
		DrawLine(32 * After.x + 20, 32 * After.y + 12+2, 32 * After.x + 12, 32 * After.y + 20+2, GetColor(60, 60, 60), 2);
	}

}
