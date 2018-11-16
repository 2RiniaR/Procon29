#pragma once

#include "def.h"
#include "Task.h"

// �}�X�N���X
class Cell : public Task {

private:

public:
	Cell() { Color = WHITE; };
	Cell(int p);
	Cell(int p, int color) { Point = p, Color = color; }
	~Cell() {};

	int Point;		//�}�X�̓��_
	int Color;		//���݂̐F

	void Initialize()	override;
	void Finalize()		override;
	void Update()		override;
	void Draw()			override;

};

