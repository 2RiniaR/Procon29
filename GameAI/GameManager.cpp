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

//0.0�`1.0�̗�����Ԃ�
double Drand() {

	return (double)rand_f(mt) / RANDSEEDMAX;

}

//�R���X�g���N�^
GameManager::GameManager() {
	
	

}

//��������
void GameManager::Initialize() {

	//��Ԃ̃��Z�b�g
	NowGameEnd = false;
	AIState = 0;
	MyAdvantage = 0;
	Winner = WHITE;
	isCustomMode = 0;

	//�����̎���܂�
	srand((unsigned int)time(NULL));

	//�t�@�C���ǂݍ���
	FileReader::ReadBoardInfo("QR/board.txt");
	FileReader::ReadTurnInfo("Data/Input/turn.txt");
	FileReader::ParamFileRead("Data/Input/param.txt");

	//�V�X�e���̏�����
	System = new SystemKeeper(FileReader::GetEndTurn());

	//�v���C���[�̏�����
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

	//�Ֆʂ̏�����
	GameBoard = new Board(FileReader::GetReadBoard());
	GameBoard->Update(AgentsArray);
	GameBoard->CalcAverage();
	BoardAve = GameBoard->GetAverage();

	//�V�X�e���̏�����2
	System->CalcScore(*GameBoard);
	FileReader::Cparam_Average(BoardAve);

	//�t�H���g�̏�����
	font1 = CreateFontToHandle(NULL, 12, 3);
	font2 = CreateFontToHandle(NULL, 24, 4);
	font3 = CreateFontToHandle(NULL, 32, 5);
	font4 = CreateFontToHandle(NULL, 28, 5);
	font5 = CreateFontToHandle(NULL, 18, 3);

	//�`��X�e�[�^�X�̏�����
	SelectingAgent = 0;
	Rotation = 0;
	isReverse = false;

}

//�I������
void GameManager::Finalize() {

}

//�X�V����
void GameManager::Update() {

	//�Q�[���I���̃`�F�b�N
	//if (NowGameEnd == true) return;

	int x, y;
	Pos2D tmppos;


	//R�������ꂽ��Ֆʂ����v���ɉ�]
	if (KeyInput::GetKeyStatus(KEY_INPUT_R) == 1) Rotation = (Rotation + 1) % 4;
	//L�������ꂽ��Ֆʂ𔽎��v���ɉ�]
	if (KeyInput::GetKeyStatus(KEY_INPUT_E) == 1) Rotation = (Rotation + 3) % 4;
	//W�������ꂽ�玩���Ƒ���̐F�����ւ���
	if (KeyInput::GetKeyStatus(KEY_INPUT_W) == 1) isReverse = !isReverse;


	//1�������ꂽ���-1�ɒ���
	if (KeyInput::GetKeyStatus(KEY_INPUT_1) == 1) SelectingAgent = 0;
	//2�������ꂽ���-2�ɒ���
	if (KeyInput::GetKeyStatus(KEY_INPUT_2) == 1) SelectingAgent = 1;
	//3�������ꂽ���-1�ɒ���
	if (KeyInput::GetKeyStatus(KEY_INPUT_3) == 1) SelectingAgent = 2;
	//4�������ꂽ���-2�ɒ���
	if (KeyInput::GetKeyStatus(KEY_INPUT_4) == 1) SelectingAgent = 3;


	//�}�E�X�̓��͂��󂯕t����
	int button, cx, cy, log;
	if (GetMouseInputLog2(&button, &cx, &cy, &log, true) == 0) {

		//���N���b�N
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

		//�E�N���b�N
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

	//G��L��������Ă���ԁA�J�X�^�����[�h
	if (KeyInput::GetKeyStatus(KEY_INPUT_G) == 1) {
		isCustomMode = (isCustomMode + 1) % 2;
	}

	if (isCustomMode == 1) {

		//��L�[�Ń^�[���𑝂₷
		if (KeyInput::GetKeyStatus(KEY_INPUT_UP) == 1) {
			System->AddTurn(1);
			GameTeams->at(RED).ResetTree();
		}

		//���L�[�Ń^�[�������炷
		if (KeyInput::GetKeyStatus(KEY_INPUT_DOWN) == 1) {
			System->AddTurn(-1);
			GameTeams->at(RED).ResetTree();
		}

		//if (GetMouseInputLog2(&button, &cx, &cy, &log, true) == 0) {

		x = cx / 32;
		y = cy / 32;
		tmppos = RotatePosRev(Pos2D(x, y));

		if (0 <= tmppos.x && tmppos.x < GameBoard->GetSize().x && 0 <= tmppos.y && tmppos.y < GameBoard->GetSize().y) {


			//�}�X�����N���b�N�Őԁ��������Ԃƕω�������
			if ((button & MOUSE_INPUT_LEFT) != 0 && log == MOUSE_INPUT_LOG_DOWN) {
				GameBoard->ToggleCell(tmppos);
				GameTeams->at(RED).ResetTree();
			}

			//�E�N���b�N�őI�𒆂̃G�[�W�F���g���ړ�������
			if ((button & MOUSE_INPUT_RIGHT) != 0 && log == MOUSE_INPUT_LOG_DOWN) {
				AgentsArray[SelectingAgent]->ChangePos(tmppos);
				GameTeams->at(RED).ResetTree();
			}

		}

		//		}
	}
	

	//C�������ꂽ��p�����[�^���ēǂݍ���
	if (KeyInput::GetKeyStatus(KEY_INPUT_C) == 1) {

		FileReader::ParamFileRead("Data/Input/param.txt");

	}


	//K�������ꂽ��s��������
	if (KeyInput::GetKeyStatus(KEY_INPUT_K) == 1) {

		//�v���C���[�s���̐����E�Փˊm�F
		System->CheckConflict(*GameBoard, AgentsArray);

		/* ���̎��_�ōs�����m�肷�� */

		//�{�[�h�̍X�V
		GameBoard->Update(AgentsArray);

		//�c���[�X�V
		GameTeams->at(RED).UpdateTree(array< array<int, AGENTNUM>, TEAMNUM>{
			GameTeams->at(RED).GetAgentAction(),
			GameTeams->at(BLUE).GetAgentAction()
		});

		//�G�[�W�F���g�̍X�V
		GameTeams->at(RED).Update();
		GameTeams->at(BLUE).Update();

		//�X�R�A�v�Z
		System->CalcScore(*GameBoard);

		//�Q�[���I������
		if (System->isGameFin() == true) {
			NowGameEnd = true;
			Winner = System->JudgeWinner();
		}

		//�^�[���X�V
		System->Update();

	}


	//AI�ɂ��v�l
	if (AIState == 1) {
		//�v���C���[���s�����v�l����
		GameTeams->at(RED).Action(*GameBoard, GameTeams->at(NEXTCOLOR(RED)));
		MyAdvantage = GameTeams->at(RED).GetAdvantage();
		AIState = 0;
	}

	//AI�c���[���Z�b�g
	if (AIState == 2) {
		GameTeams->at(RED).ResetTree();
		AIState = 0;
	}

	//A�������ꂽ��AI�T��
	if (KeyInput::GetKeyStatus(KEY_INPUT_A) == 1) {
		//AI�T���̃t���O�𗧂Ă�
		AIState = 1;
	}

	//X�������ꂽ��AI�c���[���Z�b�g
	if (KeyInput::GetKeyStatus(KEY_INPUT_X) == 1) {
		//AI�c���[���Z�b�g�̃t���O�𗧂Ă�
		AIState = 2;
	}
	
}

// �`�揈��
void GameManager::Draw() {

	//�w�i�`��
	DrawBox(0, 0, 1280, 960, GetColor(255, 255, 255), true);

	int x, y, i, j, w, h;
	Pos2D tmppos;

	w = GameBoard->GetSize().x;
	h = GameBoard->GetSize().y;

	//-------------------�����ɔՖʂ�`��----------------------
	DrawBox(0, 0, 384, 384, GetColor(0, 0, 0), false);
	for (i = 0; i < GameBoard->GetSize().x; i++) {
		for (j = 0; j < GameBoard->GetSize().y; j++) {

			tmppos = RotatePos(Pos2D(i, j));
			x = 32 * tmppos.x;
			y = 32 * tmppos.y;

			//�}�X�ɐF��h��
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

			//�}�X�̓_����\������
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


	//----------------�ՖʂɃG�[�W�F���g��`��-------------------
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


	//-----------------�����ɓ��_��\��------------------
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



	//------------�E��ɃG�[�W�F���g����\��--------------
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

		//�ʒu
		tmppos = RotatePos(AgentsArray[i]->GetPos());
		DrawFormatStringToHandle(440 + 130 * i, 60, GetColor(0, 0, 0), font2, "(%d, %d)", tmppos.x, tmppos.y);

		//�s���\��
		DrawFormatStringToHandle(440 + 130 * i, 100, GetColor(0, 0, 0), font2, "%s", ActionStr(RotateAct(AgentsArray[i]->GetAction())));

	}


	//---------------------�E�ɑ���p�l����\��-----------------------
	DrawBox(400, 250, 550, 400, GetColor(100, 255, 100), true);
	DrawBox(450, 300, 500, 350, GetColor(180, 180, 180), true);
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			DrawBox(400 + 50 * i, 250 + 50 * j, 400 + 50 * i + 50, 250 + 50 * j + 50, GetColor(0, 0, 0), false);
		}
	}


	//---------------------�E�Ƀg�����v��\��--------------------
	DrawBox(400, 150, 960, 240, GetColor(0, 0, 0), false);
	DrawFormatStringToHandle(410, 190, GetColor(0, 0, 0), font5, "����]���l�F%lf", MyAdvantage);
	Trump(GameTeams->at(RED).GetAgentAction());


	//---------------------�E�ɑ��������\��-------------------
	DrawBox(560, 250, 960, 400, GetColor(0, 0, 0), false);
	DrawFormatStringToHandle(570, 260, GetColor(0, 0, 0), font5, "A :AI�T��");
	DrawFormatStringToHandle(570, 280, GetColor(0, 0, 0), font5, "K :�^�[���X�V");
	DrawFormatStringToHandle(570, 300, GetColor(0, 0, 0), font5, "R :�ՖʉE��]");
	DrawFormatStringToHandle(570, 320, GetColor(0, 0, 0), font5, "E :�Ֆʍ���]");
	DrawFormatStringToHandle(570, 340, GetColor(0, 0, 0), font5, "W :�F���]");
	DrawFormatStringToHandle(570, 360, GetColor(0, 0, 0), font5, "X :�c���[������");
	DrawFormatStringToHandle(570, 380, GetColor(0, 0, 0), font5, "C :�ϐ������[�h");

	
	//-------------------�E���ɏ��g��\��--------------------
	DrawBox(400, 410, 960, 530, GetColor(0, 0, 0), false);
	//��]�󋵂�\��
	DrawFormatStringToHandle(420, 420, GetColor(0, 0, 0), font2, "��]�F%d�x", Rotation*90);
	//�F���]�󋵂�\��
	if(isReverse == false)	DrawFormatStringToHandle(600, 420, GetColor(255, 0, 0), font2, "�F���]�F�Ȃ��i�����͐ԁj");
	else					DrawFormatStringToHandle(600, 420, GetColor(0, 0, 255), font2, "�F���]�F����i�����͐j");

	DrawFormatStringToHandle(420, 460, GetColor(0, 0, 0), font5, "Rollout�񐔁F%d", FileReader::GetRolloutNum());
	DrawFormatStringToHandle(420, 480, GetColor(0, 0, 0), font5, "�T���[�x   �F%d", FileReader::GetSearchDeep());
	DrawFormatStringToHandle(420, 500, GetColor(0, 0, 0), font5, "�T���␳�l �F%lf", FileReader::GetCostParam());

	DrawFormatStringToHandle(660, 500, GetColor(0, 0, 0), font5, "�Ֆʕ��ϒl �F%lf", BoardAve);


	//-------------------------���s��\��-------------------------
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


	//--------------------AI�T����-----------------------
	if (AIState == 1) {
		DrawFormatStringToHandle(410, 215, GetColor(0, 0, 0), font5, "AI�T����...");
	}
	else if (AIState == 2) {
		DrawFormatStringToHandle(410, 215, GetColor(0, 0, 0), font5, "�T����������");
	}

	if (isCustomMode == 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		DrawBox(0, 0, 960, 540, GetColor(255, 120, 255), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

}

//�s���𕶎���ŕ\������
const char* GameManager::ActionStr(int act) {

	switch (act) {

	case MOVE_UP:
		return "���F��";

	case MOVE_UPRIGHT:
		return "���F�E��";

	case MOVE_RIGHT:
		return "���F�E";

	case MOVE_DOWNRIGHT:
		return "���F�E��";

	case MOVE_DOWN:
		return "���F��";

	case MOVE_DOWNLEFT:
		return "���F����";

	case MOVE_LEFT:
		return "���F��";

	case MOVE_UPLEFT:
		return "���F����";

	case DESTROY_UP:
		return "��F��";

	case DESTROY_UPRIGHT:
		return "��F�E��";

	case DESTROY_RIGHT:
		return "��F�E";

	case DESTROY_DOWNRIGHT:
		return "��F�E��";

	case DESTROY_DOWN:
		return "��F��";

	case DESTROY_DOWNLEFT:
		return "��F����";

	case DESTROY_LEFT:
		return "��F��";

	case DESTROY_UPLEFT:
		return "��F����";

	default:
		return "��~";

	};

}

//�g�����v�̒l��\������
void GameManager::Trump(const array<int, AGENTNUM> &act) {

	//AGENT1�͍��AAGENT2�͐�
	const char* str1 = NULL;
	const char* str2 = NULL;

	switch (RotateAct(act[AGENT1])) {

	case MOVE_UP:
		str1 = "��1";
		break;

	case MOVE_UPRIGHT:
		str1 = "��2";
		break;

	case MOVE_RIGHT:
		str1 = "��3";
		break;

	case MOVE_DOWNRIGHT:
		str1 = "��4";
		break;

	case MOVE_DOWN:
		str1 = "��5";
		break;

	case MOVE_DOWNLEFT:
		str1 = "��6";
		break;

	case MOVE_LEFT:
		str1 = "��7";
		break;

	case MOVE_UPLEFT:
		str1 = "��8";
		break;

	case DESTROY_UP:
		str1 = "��1(���R)";
		break;

	case DESTROY_UPRIGHT:
		str1 = "��2(���R)";
		break;

	case DESTROY_RIGHT:
		str1 = "��3(���R)";
		break;

	case DESTROY_DOWNRIGHT:
		str1 = "��4(���R)";
		break;

	case DESTROY_DOWN:
		str1 = "��5(���R)";
		break;

	case DESTROY_DOWNLEFT:
		str1 = "��6(���R)";
		break;

	case DESTROY_LEFT:
		str1 = "��7(���R)";
		break;

	case DESTROY_UPLEFT:
		str1 = "��8(���R)";
		break;

	default:
		str1 = "�E��";
		break;

	};

	switch (RotateAct(act[AGENT2])) {

	case MOVE_UP:
		str2 = "��1";
		break;

	case MOVE_UPRIGHT:
		str2 = "��2";
		break;

	case MOVE_RIGHT:
		str2 = "��3";
		break;

	case MOVE_DOWNRIGHT:
		str2 = "��4";
		break;

	case MOVE_DOWN:
		str2 = "��5";
		break;

	case MOVE_DOWNLEFT:
		str2 = "��6";
		break;

	case MOVE_LEFT:
		str2 = "��7";
		break;

	case MOVE_UPLEFT:
		str2 = "��8";
		break;

	case DESTROY_UP:
		str2 = "��1(���R)";
		break;

	case DESTROY_UPRIGHT:
		str2 = "��2(���R)";
		break;

	case DESTROY_RIGHT:
		str2 = "��3(���R)";
		break;

	case DESTROY_DOWNRIGHT:
		str2 = "��4(���R)";
		break;

	case DESTROY_DOWN:
		str2 = "��5(���R)";
		break;

	case DESTROY_DOWNLEFT:
		str2 = "��6(���R)";
		break;

	case DESTROY_LEFT:
		str2 = "��7(���R)";
		break;

	case DESTROY_UPLEFT:
		str2 = "��8(���R)";
		break;

	default:
		str2 = "�E��";
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

	//��~�܂��͂��̑�
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

	//��~�܂��͂��̑�
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

	//��~�܂��͂��̑�
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
