#include <cmath>
#include "DxLib.h"
#include "Board.h"
#include "Cell.h"
#include "Team.h"
#include "def.h"
#include "Agent.h"
#include "SystemKeeper.h"

//コンストラクタ
SystemKeeper::SystemKeeper() {

	Initialize();

}

//終了ターンを指定したコンストラクタ
SystemKeeper::SystemKeeper(int turn) {

	Initialize(turn);

}

//初期処理
void SystemKeeper::Initialize() {

	Turn = 1;
	EndTurn = DecideEndTurn();
	Score.fill(0);

	//font1 = new int(CreateFontToHandle(NULL, 32, 5));

}

//終了ターンを指定して初期化
void SystemKeeper::Initialize(int turn) {

	Turn = 1;
	EndTurn = turn;
	Score.fill(0);

	//font1 = new int(CreateFontToHandle(NULL, 32, 5));

}

//終了処理
void SystemKeeper::Finalize() {

}

//更新処理
void SystemKeeper::Update() {

	//ターンを更新
	Turn++;

}

//描画処理
void SystemKeeper::Draw() {

	DrawFormatStringToHandle(10, 420, GetColor(0, 0, 0), *font1, "Turn : %3d/%3d", Turn, EndTurn);
	DrawFormatStringToHandle(10, 460, GetColor(128, 0, 0), *font1, "Red  : %3d(T:%3d, A:%3d)", Score[RED_TOTAL], Score[RED_TILE], Score[RED_AREA]);
	DrawFormatStringToHandle(10, 495, GetColor(0, 0, 128), *font1, "Blue : %3d(T:%3d, A:%3d)", Score[BLUE_TOTAL], Score[BLUE_TILE], Score[BLUE_AREA]);

}

//ターン数を決定する
int SystemKeeper::DecideEndTurn() {

	return GetRand(60) + 60;	//ターン数 : 60～120

}

//不正な動きをしているエージェントを強制的にStayさせる
void SystemKeeper::CheckConflict(const Board &board, vector<Agent*> &agents) {

	int size = (int)agents.size();
	Pos2D tmppos;
	int i, j;

	for (i = 0; i < size; i++) {

		//盤面の外には進めない
		tmppos = agents[i]->PosAfterAction();
		if (tmppos.x < 0 ||
			tmppos.x >= board.GetSize().x ||
			tmppos.y < 0 ||
			tmppos.y >= board.GetSize().y)
		{
			agents[i]->ForcedStay();
			continue;
		}

		//相手チームのマスには進めない
		if (board.CellInfo(agents[i]->PosAfterAction()).Color == NEXTCOLOR(agents[i]->MyColor)) {
			agents[i]->ForcedStay();
			continue;
		}


	}

	vector<bool> isSamePos(size, false);

	//他エージェントと判定座標が被ると行動が無効となる
	for (i = 0; i < size; i++) {
		for (j = i+1; j < size; j++) {

			if (agents[i]->PosWithAction() == agents[j]->PosWithAction()) {
				isSamePos[i] = true;
				isSamePos[j] = true;
			}

		}
		if (isSamePos[i] == true) agents[i]->ForcedStay();

	}

}

//領域ポイントを返す
int SystemKeeper::CountAreaPoint(const Board &board, int color) {

	int i, j, x, y;
	int result;

	//盤面作成 field[y + 2][x + 2]
	x = board.GetSize().x;
	y = board.GetSize().y;
	std::vector<std::vector<int>> field(x + 2, (std::vector<int>(y + 2, 0)));

	//初期値代入
	
	for (i = 0; i < y + 2; i++) {
		field[0][i] = 1;
		field[x+1][i] = 1;
	}
	for (i = 0; i < x + 2; i++) {
		field[i][0] = 1;
		field[i][y + 1] = 1;
	}
	

	//囲み処理
	//外側の領域に対してループ
	for (int i = 0; i < x + 2; i++) Eataway(&field, board, i, 0, color);
	for (int i = 0; i < x + 2; i++) Eataway(&field, board, i, y+1, color);
	for (int i = 0; i < y + 2; i++) Eataway(&field, board, 0, i, color);
	for (int i = 0; i < y + 2; i++) Eataway(&field, board, x+1, i, color);

	//得点計算
	result = 0;

	//内側の領域に対してループ
	for (i = 0; i < x; i++) {
		for (j = 0; j < y; j++) {

			if (field[i + 1][j + 1] == 0) {
				if (board.CellInfo(Pos2D(i, j)).Color != color) 
					result += abs( board.CellInfo(Pos2D(i, j)).Point );
			}

		}
	}

	return result;

}


void SystemKeeper::Eataway(vector< vector<int> > *mask, const Board &board, int x, int y, int color) {

	int xmax = board.GetSize().x;
	int ymax = board.GetSize().y;

	mask->at(x)[y] = 1;

	//上のマスを1にするために実行
	if (y > 0) {
		if (mask->at(x)[y - 1] == 0 && board.CellInfo(Pos2D(x-1, y-2)).Color != color) Eataway(mask, board, x, y-1, color);
	}
	//下のマスを1にするために実行
	if (y < ymax - 1) {
		if (mask->at(x)[y + 1] == 0 && board.CellInfo(Pos2D(x-1, y)).Color != color) Eataway(mask, board, x, y+1, color);
	}
	//右のマスを1にするために実行
	if (x < xmax - 1) {
		if (mask->at(x + 1)[y] == 0 && board.CellInfo(Pos2D(x, y-1)).Color != color) Eataway(mask, board, x+1, y, color);
	}
	//左のマスを1にするために実行
	if (x > 0) {
		if (mask->at(x - 1)[y] == 0 && board.CellInfo(Pos2D(x-2, y-1)).Color != color) Eataway(mask, board, x-1, y, color);
	}

}

//タイルポイントを返す
int SystemKeeper::CountTilePoint(const Board &board, int color) {

	int result = 0;
	int i, j;

	for (i = 0; i < board.GetSize().x; i++) {
		for (j = 0; j < board.GetSize().y; j++) {
			if (board.CellInfo(Pos2D(i, j)).Color == color) result += board.CellInfo(Pos2D(i, j)).Point;
		}
	}

	return result;

}

//行動後の盤面を返す
Board SystemKeeper::GetBoardAfterAction(const Board &board, const Team &us, const array<int, AGENTNUM> &act) {

	Team VirtualTeam = us;
	Board VirtualBoard = board;

	//行動を思考する
	VirtualTeam.Agents[AGENT1].NextAction = act[AGENT1];
	VirtualTeam.Agents[AGENT2].NextAction = act[AGENT2];

	//衝突を判定
	vector<Agent*> agents = { &VirtualTeam.Agents[AGENT1], &VirtualTeam.Agents[AGENT2] };
	CheckConflict(VirtualBoard, agents);

	//仮想盤面の更新
	VirtualBoard.Update(agents);

	//仮想プレイヤーの更新
	VirtualTeam.Agents[AGENT1].Update();
	VirtualTeam.Agents[AGENT2].Update();

	return VirtualBoard;

}

//盤面からスコアを計算する
void SystemKeeper::CalcScore(const Board &board) {

	//赤チームの点数計算
	Score[RED_TILE] = CountTilePoint(board, RED);
	Score[RED_AREA] = CountAreaPoint(board, RED);
	Score[RED_TOTAL] = Score[RED_TILE] + Score[RED_AREA];

	//青チームの点数計算
	Score[BLUE_TILE] = CountTilePoint(board, BLUE);
	Score[BLUE_AREA] = CountAreaPoint(board, BLUE);
	Score[BLUE_TOTAL] = Score[BLUE_TILE] + Score[BLUE_AREA];

}

//どちらが勝っているかを返す
int SystemKeeper::JudgeWinner() {

	if (Score[RED_TOTAL] > Score[BLUE_TOTAL]) return RED;
	else if (Score[BLUE_TOTAL] > Score[RED_TOTAL]) return BLUE;
	else return WHITE;

}

//boardの盤面の時にusがしてよい行動としてはいけない行動を判別する
array<array<bool, ACTIONNUM>, ACTIONNUM> SystemKeeper::GetCanActionList(const Board &board, const Team &us) {

	array<array<bool, ACTIONNUM>, ACTIONNUM> ans;
	Pos2D p;
	int i;
	int act1, act2;

	//全ての要素をtrueで初期化
	for (i = 0; i < 17; i++) ans[i].fill(true);

	//Agent1
	for (int act = 0; act < 17; act++) {
		
		//盤面外では行動できない
		p = us.Agents[AGENT1].PosWithAction(act + 1);
		if (p.x < 0 ||
			p.x >= board.GetSize().x ||
			p.y < 0 ||
			p.y >= board.GetSize().y)
		{
			for (int i = 0; i < 17; i++) { ans[act][i] = false; };
			continue;
		}

		//相手のマスには行けない
		p = us.Agents[AGENT1].PosAfterAction(act + 1);
		if (board.CellInfo(p).Color == NEXTCOLOR(us.Agents[AGENT1].MyColor)) {
			for (i = 0; i < 17; i++) { ans[act][i] = false; };
			continue;
		}

		//白色のマスは破壊できない
		p = us.Agents[AGENT1].PosWithAction(act + 1);
		if (DESTROY_UP <= act+1 && act+1 <= DESTROY_UPLEFT) {
			if (board.CellInfo(p).Color == WHITE) {
				for (i = 0; i < 17; i++) { ans[act][i] = false; };
				continue;
			}
		}

	}

	//Agent2
	for (int act = 0; act < 17; act++) {

		//盤面外では行動できない
		p = us.Agents[AGENT2].PosWithAction(act + 1);
		if (p.x < 0 ||
			p.x >= board.GetSize().x ||
			p.y < 0 ||
			p.y >= board.GetSize().y)
		{
			for (i = 0; i < 17; i++) ans[i][act] = false;
			continue;
		}

		//相手のマスには行けない
		p = us.Agents[AGENT2].PosAfterAction(act + 1);
		if (board.CellInfo(p).Color == NEXTCOLOR(us.Agents[AGENT2].MyColor)) {
			for (i = 0; i < 17; i++) ans[i][act] = false;
			continue;
		}

		//白色のマスは破壊できない
		p = us.Agents[AGENT2].PosWithAction(act + 1);
		if (DESTROY_UP <= act+1 && act+1 <= DESTROY_UPLEFT) {
			if (board.CellInfo(p).Color == WHITE) {
				for (i = 0; i < 17; i++) ans[i][act] = false;
				continue;
			}
		}

	}

	//行動判定のマスが重複することはできない
	for (act1 = 0; act1 < 17; act1++) {
		for (act2 = 0; act2 < 17; act2++) {
			if (us.Agents[AGENT1].PosWithAction(act1 + 1) == us.Agents[AGENT2].PosWithAction(act2 + 1)) ans[act1][act2] = false;
		}
	}

	return ans;

}

void SystemKeeper::AddTurn(int a) {

	Turn += a;

	if (Turn <= 0) Turn = 1;
	if (Turn > EndTurn) Turn = EndTurn;

}
