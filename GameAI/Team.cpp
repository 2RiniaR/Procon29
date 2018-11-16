#include <array>
#include <ctime>
#include <cstdlib>
#include "TreeSearcher.h"
#include "Team.h"
#include "GameManager.h"
#include "Agent.h"
#include "def.h"
#include "Board.h"
#include "Cell.h"
#include "SystemKeeper.h"

// コンストラクタ
Team::Team(int color, const array<Pos2D, AGENTNUM> &pos) :
	Agents(array<Agent, AGENTNUM>{Agent(pos[AGENT1], color), Agent(pos[AGENT2], color)}),
	ActionAI(new TreeSearcher())
{

	MyColor = color;

}

//エージェントの情報更新
void Team::Update() {

	Agents[AGENT1].Update();
	Agents[AGENT2].Update();

}

void Team::Update(const array<int, AGENTNUM> &act) {

	Agents[AGENT1].Update(act[AGENT1]);
	Agents[AGENT2].Update(act[AGENT2]);

}

//ツリーを更新
void Team::UpdateTree(const array< array<int, AGENTNUM>, TEAMNUM> &move) {

	ActionAI->MoveUpdate(move);

}

//エージェントの行動を決める
void Team::Action(const Board &board, const Team &enemy) {

	//中核のAI部分
	array<int, AGENTNUM> action = ActionAI->SearchNextAction(board, *this, enemy, GameManager::System->GetLeaveTurn());

	//次の行動の意向を更新
	SetAgentAction(action);

}

//エージェントの行動を配列の形で返す
array<int, AGENTNUM> Team::GetAgentAction() {

	return array<int, AGENTNUM>{Agents[AGENT1].NextAction, Agents[AGENT2].NextAction};

}

void Team::ResetTree() {
	ActionAI->ResetTree();
}

double Team::GetAdvantage() {
	return ActionAI->GetAdvantage();
}


void Team::EasyAI(const Board &board, const Team &enemy) {

	array<array<bool, ACTIONNUM>, ACTIONNUM> Actionable = SystemKeeper::GetCanActionList(board, *this);
	array<int, ACTIONNUM*ACTIONNUM> Gain;
	int GainMin = INT_MAX;
	double GainTotal = 0;
	double RandomValue;
	double cRand = 0;
	Pos2D tmppos;
	array<int, AGENTNUM> ans{ NONE, NONE };
	int i, j;

	//各行動の獲得値の算出
	//各行動は2体のエージェントが互いに干渉しあわないので分割処理が可能
	//もっと効率化できる！

	//0で初期化
	Gain.fill(0);

	//AGENT1の処理
	for (i = 0; i < ACTIONNUM; i++) {

		//不正な行動は最低評価にする
		if (Actionable[i][STAY - 1] == false) continue;

		//単純な得点計算
		if (MOVE_UP <= i + 1 && i + 1 <= MOVE_UPLEFT) {

			tmppos = Agents[AGENT1].PosAfterAction(i + 1);
			if (board.CellInfo(tmppos).Color == WHITE) for (j = 0; j < ACTIONNUM; j++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;

		}
		else if (DESTROY_UP <= i + 1 && i + 1 <= DESTROY_UPLEFT) {

			tmppos = Agents[AGENT1].PosWithAction(i + 1);
			if (board.CellInfo(tmppos).Color == MyColor)			for (j = 0; j < ACTIONNUM; j++) Gain[i*ACTIONNUM + j] -= board.CellInfo(tmppos).Point;
			if (board.CellInfo(tmppos).Color == NEXTCOLOR(MyColor)) for (j = 0; j < ACTIONNUM; j++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;

		}

	}

	//AGENT2の処理
	for (j = 0; j < ACTIONNUM; j++) {

		//不正な行動は最低評価にする
		if (Actionable[STAY - 1][j] == false) continue;

		//AGENT2の処理
		if (MOVE_UP <= j + 1 && j + 1 <= MOVE_UPLEFT) {

			tmppos = Agents[AGENT2].PosAfterAction(j + 1);
			if (board.CellInfo(tmppos).Color == WHITE) for (i = 0; i < ACTIONNUM; i++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;

		}
		else if (DESTROY_UP <= j + 1 && j + 1 <= DESTROY_UPLEFT) {

			tmppos = Agents[AGENT2].PosWithAction(j + 1);
			if (board.CellInfo(tmppos).Color == MyColor)			for (i = 0; i < ACTIONNUM; i++) Gain[i*ACTIONNUM + j] -= board.CellInfo(tmppos).Point;
			if (board.CellInfo(tmppos).Color == NEXTCOLOR(MyColor)) for (i = 0; i < ACTIONNUM; i++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;

		}

	}

	//得点に応じて行動の選択率を変える

	//最小値を計算
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;
			GainMin = (Gain[i*ACTIONNUM + j] < GainMin) ? Gain[i*ACTIONNUM + j] : GainMin;

		}
	}

	//合計値を計算
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;
			Gain[i*ACTIONNUM + j] -= GainMin;
			GainTotal += (double)Gain[i*ACTIONNUM + j];

		}
	}

	//0除算対策
	if (GainTotal == 0.0) GainTotal = 0.01;

	//0から1までの乱数生成
	RandomValue = Drand();

	//それぞれの要素の確率を計算
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;

			//(手の評価値 - 最小値) / 合計値
			cRand += Gain[i*ACTIONNUM + j] / GainTotal;

			//確率的中
			if (cRand >= RandomValue) {
				ans = { i + 1, j + 1 };
				SetAgentAction(ans);
				return;
			}
		}
	}

	ans = { NONE, NONE };
	SetAgentAction(ans);

}


void Team::EasyAISoft(const Board &board, const Team &enemy) {

	SetAgentAction(GetEasyAISoft(board, enemy));

}


array<int, AGENTNUM> Team::GetEasyAISoft(const Board &board, const Team &enemy) const {

	array<array<bool, ACTIONNUM>, ACTIONNUM> Actionable = SystemKeeper::GetCanActionList(board, *this);
	array<int, ACTIONNUM*ACTIONNUM> Gain;
	int GainMin = INT_MAX;
	double GainTotal = 0;
	double RandomValue;
	double cRand = 0;
	Pos2D tmppos;
	array<int, AGENTNUM> ans{ NONE, NONE };
	int i, j;

	//各行動の獲得値の算出
	//各行動は2体のエージェントが互いに干渉しあわないので分割処理が可能
	//もっと効率化できる！

	//0で初期化
	Gain.fill(0);

	//AGENT1の処理
	for (i = 0; i < ACTIONNUM; i++) {

		//不正な行動は最低評価にする
		if (Actionable[i][STAY - 1] == false) continue;

		//単純な得点計算
		if (MOVE_UP <= i + 1 && i + 1 <= MOVE_UPLEFT) {

			tmppos = Agents[AGENT1].PosAfterAction(i + 1);
			if(board.CellInfo(tmppos).Color == WHITE) for (j = 0; j < ACTIONNUM; j++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;
		
		}
		else if (DESTROY_UP <= i + 1 && i + 1 <= DESTROY_UPLEFT) {

			tmppos = Agents[AGENT1].PosWithAction(i + 1);
			if (board.CellInfo(tmppos).Color == MyColor)			for (j = 0; j < ACTIONNUM; j++) Gain[i*ACTIONNUM + j] -= board.CellInfo(tmppos).Point;
			if (board.CellInfo(tmppos).Color == NEXTCOLOR(MyColor)) for (j = 0; j < ACTIONNUM; j++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;

		}

	}

	//AGENT2の処理
	for (j = 0; j < ACTIONNUM; j++) {

		//不正な行動は最低評価にする
		if (Actionable[STAY - 1][j] == false) continue;

		//AGENT2の処理
		if (MOVE_UP <= j + 1 && j + 1 <= MOVE_UPLEFT) {

			tmppos = Agents[AGENT2].PosAfterAction(j + 1);
			if (board.CellInfo(tmppos).Color == WHITE) for (i = 0; i < ACTIONNUM; i++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;
		
		}
		else if (DESTROY_UP <= j + 1 && j + 1 <= DESTROY_UPLEFT) {

			tmppos = Agents[AGENT2].PosWithAction(j + 1);
			if (board.CellInfo(tmppos).Color == MyColor)			for (i = 0; i < ACTIONNUM; i++) Gain[i*ACTIONNUM + j] -= board.CellInfo(tmppos).Point;
			if (board.CellInfo(tmppos).Color == NEXTCOLOR(MyColor)) for (i = 0; i < ACTIONNUM; i++) Gain[i*ACTIONNUM + j] += board.CellInfo(tmppos).Point;

		}

	}

	//得点に応じて行動の選択率を変える

	//合計値を計算
	for (i = 0; i < ACTIONNUM-1; i++) {
		for (j = 0; j < ACTIONNUM-1; j++) {

			if (Actionable[i][j] == false) continue;

			GainTotal += exp(Gain[i*ACTIONNUM + j]);

		}
	}

	//0除算対策
	if (GainTotal == 0.0) GainTotal = 0.01;

	//0から1までの乱数生成
	RandomValue = Drand();

	//それぞれの要素の確率を計算
	for (i = 0; i < ACTIONNUM-1; i++) {
		for (j = 0; j < ACTIONNUM-1; j++) {

			if (Actionable[i][j] == false) continue;

			//(手の評価値 - 最小値) / 合計値
			cRand += exp(Gain[i*ACTIONNUM + j]) / GainTotal;

			//確率的中
			if (cRand >= RandomValue) {
				ans = { i + 1, j + 1 };
				return ans;
			}
		}
	}

	ans = { NONE, NONE };
	return ans;

}


void Team::EasyAIFlat(const Board &board, const Team &enemy) {

	array<array<bool, ACTIONNUM>, ACTIONNUM> Actionable = SystemKeeper::GetCanActionList(board, *this);
	double Total = 0;
	double RandomValue;
	double cRand = 0;
	array<int, AGENTNUM> ans{ NONE, NONE };
	int i, j;

	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {
			if(Actionable[i][j] == true) Total++;
		}
	}

	RandomValue = Drand();

	//それぞれの要素の確率を計算
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;

			//(手の評価値 - 最小値) / 合計値
			cRand += 1 / Total;

			//確率的中
			if (cRand >= RandomValue) {
				ans = { i + 1, j + 1 };
				SetAgentAction(ans);
				return;
			}
		}
	}

	ans = { NONE, NONE };
	SetAgentAction(ans);

}


void Team::SetAgentAction(const array<int, AGENTNUM> &act) {

	Agents[AGENT1].NextAction = act[AGENT1];
	Agents[AGENT2].NextAction = act[AGENT2];

}