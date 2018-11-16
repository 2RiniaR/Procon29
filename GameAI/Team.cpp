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

// �R���X�g���N�^
Team::Team(int color, const array<Pos2D, AGENTNUM> &pos) :
	Agents(array<Agent, AGENTNUM>{Agent(pos[AGENT1], color), Agent(pos[AGENT2], color)}),
	ActionAI(new TreeSearcher())
{

	MyColor = color;

}

//�G�[�W�F���g�̏��X�V
void Team::Update() {

	Agents[AGENT1].Update();
	Agents[AGENT2].Update();

}

void Team::Update(const array<int, AGENTNUM> &act) {

	Agents[AGENT1].Update(act[AGENT1]);
	Agents[AGENT2].Update(act[AGENT2]);

}

//�c���[���X�V
void Team::UpdateTree(const array< array<int, AGENTNUM>, TEAMNUM> &move) {

	ActionAI->MoveUpdate(move);

}

//�G�[�W�F���g�̍s�������߂�
void Team::Action(const Board &board, const Team &enemy) {

	//���j��AI����
	array<int, AGENTNUM> action = ActionAI->SearchNextAction(board, *this, enemy, GameManager::System->GetLeaveTurn());

	//���̍s���̈ӌ����X�V
	SetAgentAction(action);

}

//�G�[�W�F���g�̍s����z��̌`�ŕԂ�
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

	//�e�s���̊l���l�̎Z�o
	//�e�s����2�̂̃G�[�W�F���g���݂��Ɋ�������Ȃ��̂ŕ����������\
	//�����ƌ������ł���I

	//0�ŏ�����
	Gain.fill(0);

	//AGENT1�̏���
	for (i = 0; i < ACTIONNUM; i++) {

		//�s���ȍs���͍Œ�]���ɂ���
		if (Actionable[i][STAY - 1] == false) continue;

		//�P���ȓ��_�v�Z
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

	//AGENT2�̏���
	for (j = 0; j < ACTIONNUM; j++) {

		//�s���ȍs���͍Œ�]���ɂ���
		if (Actionable[STAY - 1][j] == false) continue;

		//AGENT2�̏���
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

	//���_�ɉ����čs���̑I�𗦂�ς���

	//�ŏ��l���v�Z
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;
			GainMin = (Gain[i*ACTIONNUM + j] < GainMin) ? Gain[i*ACTIONNUM + j] : GainMin;

		}
	}

	//���v�l���v�Z
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;
			Gain[i*ACTIONNUM + j] -= GainMin;
			GainTotal += (double)Gain[i*ACTIONNUM + j];

		}
	}

	//0���Z�΍�
	if (GainTotal == 0.0) GainTotal = 0.01;

	//0����1�܂ł̗�������
	RandomValue = Drand();

	//���ꂼ��̗v�f�̊m�����v�Z
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;

			//(��̕]���l - �ŏ��l) / ���v�l
			cRand += Gain[i*ACTIONNUM + j] / GainTotal;

			//�m���I��
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

	//�e�s���̊l���l�̎Z�o
	//�e�s����2�̂̃G�[�W�F���g���݂��Ɋ�������Ȃ��̂ŕ����������\
	//�����ƌ������ł���I

	//0�ŏ�����
	Gain.fill(0);

	//AGENT1�̏���
	for (i = 0; i < ACTIONNUM; i++) {

		//�s���ȍs���͍Œ�]���ɂ���
		if (Actionable[i][STAY - 1] == false) continue;

		//�P���ȓ��_�v�Z
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

	//AGENT2�̏���
	for (j = 0; j < ACTIONNUM; j++) {

		//�s���ȍs���͍Œ�]���ɂ���
		if (Actionable[STAY - 1][j] == false) continue;

		//AGENT2�̏���
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

	//���_�ɉ����čs���̑I�𗦂�ς���

	//���v�l���v�Z
	for (i = 0; i < ACTIONNUM-1; i++) {
		for (j = 0; j < ACTIONNUM-1; j++) {

			if (Actionable[i][j] == false) continue;

			GainTotal += exp(Gain[i*ACTIONNUM + j]);

		}
	}

	//0���Z�΍�
	if (GainTotal == 0.0) GainTotal = 0.01;

	//0����1�܂ł̗�������
	RandomValue = Drand();

	//���ꂼ��̗v�f�̊m�����v�Z
	for (i = 0; i < ACTIONNUM-1; i++) {
		for (j = 0; j < ACTIONNUM-1; j++) {

			if (Actionable[i][j] == false) continue;

			//(��̕]���l - �ŏ��l) / ���v�l
			cRand += exp(Gain[i*ACTIONNUM + j]) / GainTotal;

			//�m���I��
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

	//���ꂼ��̗v�f�̊m�����v�Z
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (Actionable[i][j] == false) continue;

			//(��̕]���l - �ŏ��l) / ���v�l
			cRand += 1 / Total;

			//�m���I��
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