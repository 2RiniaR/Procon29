#include "DxLib.h"
#include "Cell.h"
#include "Agent.h"
#include "Board.h"

// コンストラクタ
Board::Board(const vector< vector<Cell> > &cells) :
	BoardCells( new vector< vector<Cell> >(cells) )
{
	width = (int)BoardCells->size();
	height = (int)BoardCells->at(0).size();

	Initialize();
}

// コンストラクタ
Board::Board(const Board &board) :
	BoardCells( new vector< vector<Cell> >(*board.BoardCells) )
{
	width = (int)BoardCells->size();
	height = (int)BoardCells->at(0).size();

	Initialize();
}

// 初期処理
void Board::Initialize() {

}

// 終了処理
void Board::Finalize() {

}

// 更新処理
void Board::Update() {

}

// 描画処理
void Board::Draw() {

	int x, y, i ,j;

	DrawBox(0, 0, 384, 384, GetColor(0, 0, 0), false);

	//左側に盤面を描画
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			x = 32 * i, y = 32 * j;
			DrawBox(x, y, x + 32, y + 32, GetColor(0, 0, 0), false);
			DrawFormatStringToHandle(x + 4, y + 4, GetColor(0, 0, 0), *font, "%d", BoardCells->at(i)[j].Point);
		}
	}

}

void Board::Update(vector<Agent*> &agents) {

	for (int i = 0; i < (AGENTNUM*TEAMNUM); i++) {
		//指定したタイルを破壊する
		if (DESTROY_UP <= agents[i]->GetAction() && agents[i]->GetAction() <= DESTROY_UPLEFT) Put(WHITE, agents[i]->PosWithAction());
	}

	for (int i = 0; i < (AGENTNUM*TEAMNUM); i++) {
		//エージェント自身の座標にタイルを置く
		Put(agents[i]->MyColor, agents[i]->PosAfterAction());
	}

}

bool Board::Put(int color, Pos2D pos) {

	if (
		pos.x < 0 ||
		pos.x >= width ||
		pos.y < 0 ||
		pos.y >= height
		)	return false;

	BoardCells->at(pos.x)[pos.y].Color = color;
	return true;

}

const Cell& Board::CellInfo(const Pos2D &p) const {
	return BoardCells->at(p.x)[p.y]; 
}


void Board::CalcAverage() {

	double ans = 0;
	int i, j;

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			ans += abs(CellInfo(Pos2D(i, j)).Point);
		}
	}

	ans /= width * height;

	Average = new double(ans);

}

double Board::GetAverage() {

	if (Average == nullptr) return 0.0;
	else return *Average;

}

Board::~Board() {

	delete BoardCells;

}

void Board::ToggleCell(const Pos2D &p) {

	switch (BoardCells->at(p.x)[p.y].Color) {

	case RED:
		Put(BLUE, p);
		break;

	case BLUE:
		Put(WHITE, p);
		break;

	case WHITE:
		Put(RED, p);
		break;

	}

}