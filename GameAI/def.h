#pragma once

#define NONE 0

//座標クラス
class Pos2D {

public:
	int x;
	int y;

	Pos2D() {};		//コンストラクタ
	Pos2D(int x, int y) { this->x = x, this->y = y; };	//イニシャライザ
	~Pos2D() {};	//デストラクタ

	Pos2D operator +(const Pos2D &vec) const { return Pos2D(this->x + vec.x, this->y + vec.y); };	//加算演算子
	Pos2D operator -(const Pos2D &vec) const { return Pos2D(this->x - vec.x, this->y - vec.y); };	//減算演算子
	Pos2D operator -() const { return Pos2D(-1 * this->x, -1 * this->y); };	//負の数
	bool operator ==(const Pos2D &r) const { return (this->x == r.x && this->y == r.y); }
	bool operator !=(const Pos2D &r) const { return  !(*this == r); }

};

//基礎情報
#define TEAMNUM		2
#define AGENTNUM	2

// マスの色
#define NEXTCOLOR(c) (c+1)%2
#define WHITE	-1
#define RED		0
#define BLUE	1

// 行動
#define MOVE_UP				1
#define MOVE_UPRIGHT		2
#define MOVE_RIGHT			3
#define MOVE_DOWNRIGHT		4
#define MOVE_DOWN			5
#define MOVE_DOWNLEFT		6
#define MOVE_LEFT			7
#define MOVE_UPLEFT			8
#define DESTROY_UP			9
#define DESTROY_UPRIGHT		10
#define DESTROY_RIGHT		11
#define DESTROY_DOWNRIGHT	12
#define DESTROY_DOWN		13
#define DESTROY_DOWNLEFT	14
#define DESTROY_LEFT		15
#define DESTROY_UPLEFT		16
#define STAY				17
#define ACTIONNUM			17

// スコア配列
#define RED_TILE	0
#define RED_AREA	1
#define RED_TOTAL	2
#define BLUE_TILE	3
#define BLUE_AREA	4
#define BLUE_TOTAL	5
#define POINTS_SIZE 6

// エージェント
#define NEXTAGENT(a) (a+1)%2
#define AGENT1 0
#define AGENT2 1

#define RANDSEEDMAX 1000000