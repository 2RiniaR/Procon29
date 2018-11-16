#pragma once
#include <array>
#include "def.h"

using namespace std;

class Board;

class FileReader {

private:
	static Board* ReadBoard;
	static array< array<Pos2D, AGENTNUM>, TEAMNUM >* InitPos;
	static int EndTurn;

	static int RolloutNum;
	static int SearchDeep;
	static double CostParam;

public:
	FileReader();
	~FileReader();

	static void ReadBoardInfo(const char* filename);
	static Board GetReadBoard() { return *ReadBoard; };
	static array< array<Pos2D, AGENTNUM>, TEAMNUM > GetInitPos() { return *InitPos; };

	static void ReadTurnInfo(const char *filename);
	static int GetEndTurn() { return EndTurn; }

	static void ParamFileRead(const char *filename);
	static int GetRolloutNum() { return RolloutNum; }
	static int GetSearchDeep() { return SearchDeep; }
	static double GetCostParam() { return CostParam; }

	static void Cparam_Average(double average);

};

