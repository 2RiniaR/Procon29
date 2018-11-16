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

// 前方参照
class Team;
class Agent;
class Board;
class SystemKeeper;

static random_device rnd;	// 非決定的な乱数生成器
static mt19937 mt(rnd());	// メルセンヌ・ツイスタの32ビット版、引数は初期シード
static uniform_int_distribution<> rand_f(0, RANDSEEDMAX);	// [0, RANDSEEDMAX] 範囲の一様乱数
static uniform_int_distribution<> rand_act(0, 16);	// [0, 16] 範囲の一様乱数

double Drand();

// マネージメントクラス
class GameManager : public Task{

private:

	/* システム関係 */
	Board *GameBoard;					//盤面
	array<Team, TEAMNUM> *GameTeams;	//チーム
	vector<Agent*> AgentsArray;
	bool NowGameEnd;
	int AIState;
	int Winner;
	double MyAdvantage;
	int isCustomMode;

	/* 描画関係 */
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

