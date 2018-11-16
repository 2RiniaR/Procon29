#pragma once

#include <array>
#include <vector>
#include "def.h"

using namespace std;

class Board;
class Agent;
class Team;


// ゲームシステムの管理クラス
class SystemKeeper {

private:
	int *font1;
	int Turn;
	int EndTurn;
	array<int, POINTS_SIZE> Score;

	int DecideEndTurn();

public:
	SystemKeeper();
	SystemKeeper(int turn);
	~SystemKeeper() {};

	static void CheckConflict(const Board &board, vector<Agent*> &agents);
	bool isGameFin() { return (Turn >= EndTurn); };
	int JudgeWinner();

	void CalcScore(const Board &board);
	static int CountAreaPoint(const Board &board, int color);
	static int CountTilePoint(const Board &board, int color);

	static Board GetBoardAfterAction(const Board &board, const Team &us, const array<int, AGENTNUM> &act);
	static array<array<bool, ACTIONNUM>, ACTIONNUM> GetCanActionList(const Board &board, const Team &us);
	int GetScore(int n) { return Score[n]; }
	int GetNowTurn() { return Turn; }
	int GetEndTurn() { return EndTurn; }
	int GetLeaveTurn() { return EndTurn - Turn + 1; }
	void AddTurn(int a);

	static void Eataway(vector< vector<int> > *mask, const Board &board, int x, int y, int color);

	void Initialize();
	void Initialize(int turn);
	void Finalize();
	void Update();
	void Draw();

};

