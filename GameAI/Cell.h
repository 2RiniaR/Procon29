#pragma once

#include "def.h"
#include "Task.h"

// マスクラス
class Cell : public Task {

private:

public:
	Cell() { Color = WHITE; };
	Cell(int p);
	Cell(int p, int color) { Point = p, Color = color; }
	~Cell() {};

	int Point;		//マスの得点
	int Color;		//現在の色

	void Initialize()	override;
	void Finalize()		override;
	void Update()		override;
	void Draw()			override;

};

