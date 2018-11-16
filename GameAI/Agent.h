#pragma once

#include <array>
#include "def.h"

using namespace std;

// エージェントクラス
class Agent {

private:
	Pos2D Position;

public:
	int MyColor;	//自分の色
	int NextAction;	//次の行動

	Agent(Pos2D pos, int color);
	~Agent() {};

	void Update();			//行動NextActionを適用する
	void Update(int act);
	void Action();			//行動NextActionを決定する
	void ForcedStay();		//行動NextActionを強制的にStayにする

	int GetAction() { return NextAction; };
	void SetAction(int act) { NextAction = act; };
	Pos2D GetPos() const	{ return Position; }
	
	Pos2D PosAfterAction() const;			//NextActionでの行動後の位置を返す
	Pos2D PosAfterAction(int act) const;	//actでの行動後の位置を返す
	Pos2D PosWithAction() const;			//NextActionでの行動の判定位置を返す
	Pos2D PosWithAction(int act) const;		//actでの行動の判定位置を返す

	void ChangePos(const Pos2D &p);

};

