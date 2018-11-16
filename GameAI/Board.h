#pragma once

#include <vector>
#include <array>
#include "Task.h"
#include "def.h"

using namespace std;
class Cell;
class Agent;

// �ՖʃN���X
class Board : public Task {

private:
	vector< vector<Cell> > *BoardCells;		//�}�X��2�����z��
	int height;		//�^�e�̃}�X�̐�
	int width;		//���R�̃}�X�̐�
	int *font;
	double *Average;

public:
	Board(const vector< vector<Cell> > &cells);
	Board(const Board &board);
	~Board();

	void Initialize()	override;
	void Finalize()		override;
	void Update()		override;
	void Draw()			override;

	//void Update(const array<Agent, AGENTNUM> &red, const array<Agent, AGENTNUM> &blue);
	void Update(vector<Agent*> &agents);
	bool Put(int color, Pos2D pos);
	void CalcAverage();
	double GetAverage();

	Pos2D GetSize() const { return Pos2D(width, height); }
	const Cell& CellInfo(const Pos2D &p) const;

	void ToggleCell(const Pos2D &p);

};

