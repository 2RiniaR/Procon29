#pragma once

#include <array>
#include <vector>
#include <list>
#include <memory>
#include "def.h"
#include "Board.h"
#include "Team.h"

#define MAX_WITH_COUNT 0
#define MAX_WITH_RESULT 1

using namespace std;

class Agent;
class Board;
class Team;


class GameLog {

public:
	vector< array< array<int, AGENTNUM>, TEAMNUM> > Hand;	//行動の記録[記録番号][チーム番号][エージェント番号]
	int Winner;												//点差(赤が+、青が-)

	GameLog() { Winner = 0, Hand.clear(); }
	~GameLog() { Hand.clear(); }

};


class State {

public:
	/* ---- 所持情報関係 ---- */
	int Depth;								//ノードの深さ
	const int Color;						//ノードの色（どちらの色の行動か）
	const array<int, AGENTNUM> VAction;		//このノードでの行動
	Board VBoard;						//このノードでの行動前の盤面
	array<Team, TEAMNUM> VTeam;		//このノードでの行動前のプレイヤー

									/* ---- ツリー構造関係 ---- */
	State* Parent;					//親ノードへのポインタ
	vector<State*> Child;			//子ノードの配列

									/* ---- 評価関係 ---- */
	int Count;						//Rolloutの回数
	double Qvalue;					//平均スコア					
	double ResultSum;				//Rolloutでの点差の合計
	double UCB;						//探索期待値
	vector<GameLog*> RolloutLog;	//Rolloutの結果

	State(
		State* parent,
		const Board &board,
		const array<int, AGENTNUM> &act,
		const array<Team, TEAMNUM> &team,
		const int &deep,
		const int &color,
		const vector<GameLog*> &log
	);

	~State();

	State* Selection();
	void Expansion(const int &leaveturn);
	void GenerateChild();
	void Evaluation(const int &leaveturn);
	int Rollout(const int &leaveturn);
	void Backup();
	void CalcUCB();
	void UpdateCount();
	array<int, AGENTNUM> MaxCountHand();
	array<int, AGENTNUM> MaxResultHand();
	array< array< vector<GameLog*>, ACTIONNUM>, ACTIONNUM > DevideRolloutLog();

	void WinFromLog();
	void DeleteRolloutLog();
	void DeleteChildNode();

	void UpdateDepth();
	void UpdateInfo();

};

class TreeSearcher {

private:
	State* Tree;
	double Advantage;

	State* FindActNode(const array< array<int, AGENTNUM>, TEAMNUM> &move);
	void CleanNode(State *root);

public:
	TreeSearcher();
	~TreeSearcher() {};

	void MoveUpdate(const array< array<int, AGENTNUM>, TEAMNUM> &move);
	array<int, AGENTNUM> SearchNextAction(const Board &board, const Team &us, const Team &enemy, const int &leaveturn);
	void ResetTree();
	double GetAdvantage() { return Advantage; }

};

