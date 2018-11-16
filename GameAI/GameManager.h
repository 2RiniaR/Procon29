#pragma once

#include <array>
#include <vector>
#include <random>
#include "Task.h"
#include "def.h"
#include "Team.h"
#include "Board.h"
#include "SystemKeeper.h"

using namespace std;

// �O���Q��
class Team;
class Agent;
class Board;
class SystemKeeper;

static random_device rnd;	// �񌈒�I�ȗ���������
static mt19937 mt(rnd());	// �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h
static uniform_int_distribution<> rand_f(0, RANDSEEDMAX);	// [0, RANDSEEDMAX] �͈͂̈�l����
static uniform_int_distribution<> rand_act(0, 16);	// [0, 16] �͈͂̈�l����

double Drand();

// �}�l�[�W�����g�N���X
class GameManager : public Task{

private:

	/* �V�X�e���֌W */
	Board *GameBoard;					//�Ֆ�
	array<Team, TEAMNUM> *GameTeams;	//�`�[��
	vector<Agent*> AgentsArray;
	bool NowGameEnd;
	int AIState;
	int Winner;
	double MyAdvantage;
	int isCustomMode;

	/* �`��֌W */
	int font1, font2, font3, font4, font5;
	int SelectingAgent;
	int Rotation;
	bool isReverse;
	const char* ActionStr(int act);
	void Trump(const array<int, AGENTNUM> &act);
	Pos2D RotatePos(const Pos2D &pos);
	Pos2D RotatePosRev(const Pos2D &pos);
	int RotateAct(const int &act);
	int RotateActRev(const int &act);
	void DrawActMark(const Agent &ag);

public:
	static SystemKeeper *System;

	GameManager();
	~GameManager() {};

	static double BoardAve;
	void Initialize()	override;
	void Finalize()		override;
	void Update()		override;
	void Draw()			override;

	Board& GetBoard() { return *GameBoard; }
	Team& GetTeam(int color) { return GameTeams->at(color); }

};

