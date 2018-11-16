#pragma once

#include <array>
#include "def.h"
#include "Agent.h"

using namespace std;
class Agent;
class Board;
class TreeSearcher;

// �`�[���N���X
class Team {

private:
	TreeSearcher *ActionAI;

public:
	int MyColor;					//���g�̃`�[���̐F�iRED, BLUE�j
	array<Agent, AGENTNUM> Agents;	//�G�[�W�F���g�̔z��

	Team(int color, const array<Pos2D, AGENTNUM> &pos);
	~Team() {};

	void Action(const Board &board, const Team &enemy);
	void Update();
	void Update(const array<int, AGENTNUM> &act);
	void UpdateTree(const array< array<int, AGENTNUM>, TEAMNUM> &move);
	void SetAgentAction(const array<int, AGENTNUM> &act);
	void ResetTree();
	double GetAdvantage();

	void EasyAI(const Board &board, const Team &enemy);
	void EasyAISoft(const Board &board, const Team &enemy);
	void EasyAIFlat(const Board &board, const Team &enemy);
	array<int, AGENTNUM> GetEasyAISoft(const Board &board, const Team &enemy) const;
	array<int, AGENTNUM> GetAgentAction();

};

