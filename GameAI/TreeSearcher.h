#pragma once

#include <array>
#include <vector>
#include <list>
#include <memory>
#include "def.h"
#include "Board.h"
#include "Team.h"

#define MAX_WITH_COUNT 0
#define MAX_WITH_RESULT 1

using namespace std;

class Agent;
class Board;
class Team;


class GameLog {

public:
	vector< array< array<int, AGENTNUM>, TEAMNUM> > Hand;	//�s���̋L�^[�L�^�ԍ�][�`�[���ԍ�][�G�[�W�F���g�ԍ�]
	int Winner;												//�_��(�Ԃ�+�A��-)

	GameLog() { Winner = 0, Hand.clear(); }
	~GameLog() { Hand.clear(); }

};


class State {

public:
	/* ---- �������֌W ---- */
	int Depth;								//�m�[�h�̐[��
	const int Color;						//�m�[�h�̐F�i�ǂ���̐F�̍s�����j
	const array<int, AGENTNUM> VAction;		//���̃m�[�h�ł̍s��
	Board VBoard;						//���̃m�[�h�ł̍s���O�̔Ֆ�
	array<Team, TEAMNUM> VTeam;		//���̃m�[�h�ł̍s���O�̃v���C���[

									/* ---- �c���[�\���֌W ---- */
	State* Parent;					//�e�m�[�h�ւ̃|�C���^
	vector<State*> Child;			//�q�m�[�h�̔z��

									/* ---- �]���֌W ---- */
	int Count;						//Rollout�̉�
	double Qvalue;					//���σX�R�A					
	double ResultSum;				//Rollout�ł̓_���̍��v
	double UCB;						//�T�����Ғl
	vector<GameLog*> RolloutLog;	//Rollout�̌���

	State(
		State* parent,
		const Board &board,
		const array<int, AGENTNUM> &act,
		const array<Team, TEAMNUM> &team,
		const int &deep,
		const int &color,
		const vector<GameLog*> &log
	);

	~State();

	State* Selection();
	void Expansion(const int &leaveturn);
	void GenerateChild();
	void Evaluation(const int &leaveturn);
	int Rollout(const int &leaveturn);
	void Backup();
	void CalcUCB();
	void UpdateCount();
	array<int, AGENTNUM> MaxCountHand();
	array<int, AGENTNUM> MaxResultHand();
	array< array< vector<GameLog*>, ACTIONNUM>, ACTIONNUM > DevideRolloutLog();

	void WinFromLog();
	void DeleteRolloutLog();
	void DeleteChildNode();

	void UpdateDepth();
	void UpdateInfo();

};

class TreeSearcher {

private:
	State* Tree;
	double Advantage;

	State* FindActNode(const array< array<int, AGENTNUM>, TEAMNUM> &move);
	void CleanNode(State *root);

public:
	TreeSearcher();
	~TreeSearcher() {};

	void MoveUpdate(const array< array<int, AGENTNUM>, TEAMNUM> &move);
	array<int, AGENTNUM> SearchNextAction(const Board &board, const Team &us, const Team &enemy, const int &leaveturn);
	void ResetTree();
	double GetAdvantage() { return Advantage; }

};

