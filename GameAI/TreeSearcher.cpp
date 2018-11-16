#include <cmath>
#include <array>
#include <vector>
#include "def.h"
#include "Board.h"
#include "FileReader.h"
#include "SystemKeeper.h"
#include "TreeSearcher.h"
#include "GameManager.h"
#include "Team.h"

//-----------------�n�C�p�[�p�����[�^-----------------------

//�m�[�h�W�J��臒l
//������ƃm�[�h�̐����ʂ������邪�T�������U����
//�グ��Ɛ��x�͍����Ȃ邪�T���悪�����Ȃ�
#define THRESHOLD 10

//Rollout�̌J��Ԃ���
//������Ə������Ԃ͒Z���Ȃ邪���x��������
//�グ��Ɛ��x���オ�邪�������Ԃ������Ȃ�
#define SEARCHNUM 10000

//�T���]���o�����X
//�������Rollout���ʗD��i�T���̕��U�j
//�グ��ƒT�����Ғl�D��i�T���̏W���j
#define COSTPARAM 22.0

//��ǂ݃^�[����
//������Ɖ������ǂ߂Ȃ��Ȃ邪�������₷���Ȃ�
//�グ��Ɛ�ǂ݂��₷���Ȃ邪�������x���Ȃ�
#define MAXDEPTH 15

//�ŏI�I�Ȏ�̌�����@
// MAX_WITH_RESULT :�ŏI�I�ȕ]���l�Ŏ������
// MAX_WITH_COUNT :�ŏI�I�ȒT���񐔂Ŏ������(UCB�l�Ɉˑ�)
#define MAXHANDMETHOD MAX_WITH_RESULT

//�P����AI���g���^�[����
#define EASYTURN 0

//-----------------------------------------------------------

//�R���X�g���N�^
TreeSearcher::TreeSearcher() {

	Advantage = 0;

}

//�������̂��|��
void TreeSearcher::CleanNode(State *root) {

	root->UpdateInfo();

	//�u�e�v�Ɓu�e�̐e (=Tree)�v��RollOutLog�̂ݎc��
	for (auto itr = Tree->Child.begin(); itr != Tree->Child.end(); itr++) {

		if ((*itr) == root->Parent) {

			//�e�Ȃ��
			for (auto itr2 = (*itr)->Child.begin(); itr2 != (*itr)->Child.end(); itr2++) {
				if ((*itr2) != root) {

					//root�łȂ����
					if (*itr2 != nullptr) {
						(*itr2)->DeleteRolloutLog();
						delete *itr2;
						*itr2 = nullptr;
					}

				}
			}

			if (*itr != nullptr) {
				(*itr)->RolloutLog.clear();
				(*itr)->Child.clear();
				delete *itr;
				*itr = nullptr;
			}

		}
		else {

			//�e�łȂ����
			//�q�����ׂď�������
			(*itr)->DeleteChildNode();
			if (*itr != nullptr) {
				(*itr)->DeleteRolloutLog();
				delete *itr;
				*itr = nullptr;
			}

		}
	}

	if (Tree != nullptr) {
		Tree->RolloutLog.clear();
		Tree->Child.clear();
		delete Tree;
		Tree = nullptr;
	}

	root->Parent = nullptr;

	//�߂����^�[���̃��O��Pop
	for (auto itr = root->RolloutLog.begin(); itr != root->RolloutLog.end(); itr++) {
		if (!(*itr)->Hand.empty()) (*itr)->Hand.erase((*itr)->Hand.begin());
	}

	root->UpdateDepth();

}

//���[�g�m�[�h�̍X�V�A���������[�N�h�~
void TreeSearcher::MoveUpdate(const array< array<int, AGENTNUM>, TEAMNUM> &move) {

	State* newroot = FindActNode(move);

	if (newroot == nullptr) {
		//�c���[�v�f�̃��������
		if (Tree != nullptr) {
			delete Tree;
			Tree = nullptr;
		}
		return;
	}

	//�s�v�ɂȂ����m�[�h�̍폜
	CleanNode(newroot);

	//���m�[�h�̍X�V
	Tree = newroot;

}

//�s��move�ɊY������m�[�h��T��
State* TreeSearcher::FindActNode(const array< array<int, AGENTNUM>, TEAMNUM> &move) {

	State* tmp = NULL;

	if (Tree == nullptr) return NULL;

	//�s���ɉ������m�[�h��T��
	for (auto itr = Tree->Child.begin(); itr != Tree->Child.end(); itr++) {
		if ((*itr)->VAction == move[RED]) {

			tmp = (*itr);
			for (auto itr2 = tmp->Child.begin(); itr2 != tmp->Child.end(); itr2++) {
				if ((*itr2)->VAction == move[BLUE]) return (*itr2);
			}
			return NULL;

		}
	}

	return NULL;

}

//�L���Ǝv������Ԃ��i�����e�J�����ؒT���j
/*
board		:���݂̔Ֆ�
us			:�����̃`�[��
enemy		:����̃`�[��
leaveturn	:�Q�[���I���܂ł̎c��^�[�����i�ŏI�^�[���� - ���݂̃^�[���� + 1�j
*/
array<int, AGENTNUM> TreeSearcher::SearchNextAction(const Board &board, const Team &us, const Team &enemy, const int &leaveturn) {

	//�ŏ�5�^�[����EasyAISoft
	if (GameManager::System->GetNowTurn() <= EASYTURN) return us.GetEasyAISoft(board, enemy);

	//���m�[�h�i�[��0�j�𐶐�
	if (Tree == nullptr) {
		Tree = new State(
			NULL,								//�e�m�[�h�͖���
			board,								//���݂̏�Ԃ̔Ֆʂ�n��
			array<int, AGENTNUM>{NONE, NONE},	//�s���͖����i�Ȃɂ�board��team�ɓK�p���Ȃ��j
			array<Team, TEAMNUM>{Team(us), Team(enemy)},	//�����Ƒ�����`�[���Ƃ��ēn��
			0,									//���m�[�h�Ȃ̂Ő[����0
			NEXTCOLOR(us.MyColor),				//�[��1�������̐F�ɂ���̂ŁA�����̐F�ł͂Ȃ�����I��
			vector<GameLog*>(0)					//���[���A�E�g�Ȃ�
		);
	}

	//selection�ɂ��I�����ꂽ�m�[�h
	State* select = NULL;

	//SEARCHNUM��J��Ԃ�
	for (int s = 0; s < FileReader::GetRolloutNum(); s++) {

		//����֐��ɏ]���ėt�m�[�h��I������
		select = Tree->Selection();

		//Rollout�ɂ��m�[�h�̕]��
		select->Evaluation(leaveturn);

		//�񐔂̍X�V�A臒l�𒴂��Ă���Ίg��
		select->Expansion(leaveturn);

		//���ʂ���w�ɔ��f����
		select->Backup();

	}

	Advantage = -1 * Tree->Qvalue;

	//�ŏI�I�Ȏ�̑I��
	if(MAXHANDMETHOD == MAX_WITH_RESULT)	return Tree->MaxResultHand();
	else									return Tree->MaxCountHand();

}

//�c���[�̃��Z�b�g�i�Ǐ������΍�j
void TreeSearcher::ResetTree() {

	if (Tree == nullptr) return;

	Tree->DeleteChildNode();
	Tree = nullptr;

}

/*

	�y�����F�Q�[���c���[�\���ɂ��āz
	�E�[��0�͍��m�[�h�A���݂̏󋵂Ƃ���
	�E�[������̃m�[�h(1, 3, 5,...)�͎������s����������̏󋵂Ƃ���
		VBoard	: �s���O�̔ՖʁB1�O�̃m�[�h��VBoard��act��K�p������B
		VTeam	: �s���O�̃G�[�W�F���g�B�Y����[RED], [BLUE]�Ŋi�[����B���̃m�[�h�ɓn���ۂɏ��Ԃ�ς�����͂��Ȃ��B
		VAction	: �����̃`�[���̍s��
		Color	: �����̃`�[���̐F�iRED, BLUE�j
		Depth	: �[���B���ꂪ����������ɂ���āA�m�[�h�����������肩�킩��B

	�E�[���������̃m�[�h(2, 4, 6,...)�͑��肪�s����������̏󋵂Ƃ���
		VBoard	: �s���O�̔ՖʁB2�O�̃m�[�h��VBoard��act��K�p������B
		VTeam	: �s���O�̃G�[�W�F���g�B�Y����[RED], [BLUE]�Ŋi�[����B���̃m�[�h�ɓn���ۂɏ��Ԃ�ς�����͂��Ȃ��B
		VAction	: ����̃`�[���̍s��
		Color	: ����̃`�[���̐F�iRED, BLUE�j
		Depth	: �[���B���ꂪ����������ɂ���āA�m�[�h�����������肩�킩��B

*/

//�m�[�h�F�R���X�g���N�^
/*
	parent	:�e�v�f�̃|�C���^
	board	:act�K�p�O�̔Ֆ�
	act		:���̗v�f�ōs����s��
	team	:act�K�p�O�̃`�[��
	deep	:���̗v�f�̐[���i����0�Ƃ���j
	color	:�m�[�h�̃`�[��

	������ :������board, agent�͍s��act��K�p����O�̃I�u�W�F�N�g
*/
State::State(
	State* parent,
	const Board &board,
	const array<int, AGENTNUM> &act,
	const array<Team, TEAMNUM> &team,
	const int &deep,
	const int &color,
	const vector<GameLog*> &log
) :
	Depth(deep),
	Color(color),
	VBoard(board),
	VAction(act),
	VTeam(team),
	Parent(parent),
	RolloutLog(log)
{
	Count = (int)RolloutLog.size();
	WinFromLog();
	CalcUCB();
}

//�m�[�h�F�f�X�g���N�^
State::~State() {

	RolloutLog.clear();
	Child.clear();

}

//�m�[�h�F�s���I��
State* State::Selection() {

	State* max = NULL;		//�ړ���̎q�m�[�h
	double vm = -DBL_MAX;	//�T�����Ғl�̍ő�_

							//�c���[�̖��[�v�f�ł��邩�𔻒�
	if (Child.empty() == true) return this;

	//���ҕ�VQ(s,a) + �R�X�gC(s,a) ���ő�̎q�v�f�̃A�h���X��max�Ɋi�[
	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if ((*itr)->UCB > vm) {
			vm = (*itr)->UCB;
			max = (*itr);
		}
	}

	//�q�v�f�ōċN����
	return max->Selection();

}

//�m�[�h�F�����ɉ����Ċg��
void State::Expansion(const int &leaveturn) {

	//臒l�ɒB���Ă��邩�A���Ɋg������Ă��邩�̃`�F�b�N
	if (!Child.empty() || !(Count > THRESHOLD*(Depth + 1))) return;
	if (leaveturn * 2 - 1 - Depth <= 0) return;

	//臒l�ɒB���Ă��Ă��A�g������Ă��Ȃ��ꍇ�g������
	GenerateChild();

}

//Rollout���s�񐔂��J�E���g
void State::UpdateCount() {

	Count++;

	if (Parent != nullptr) Parent->UpdateCount();

}

//�m�[�h�F�q�m�[�h�̊g��
void State::GenerateChild() {

	int i, j;
	array<Team, TEAMNUM> copyteam = VTeam;
	Board copyboard = VBoard;
	vector<Agent*> VAgentsArray{
		&(copyteam[RED].Agents[AGENT1]),
		&(copyteam[RED].Agents[AGENT2]),
		&(copyteam[BLUE].Agents[AGENT1]),
		&(copyteam[BLUE].Agents[AGENT2])
	};

	//���������̃m�[�h�̂Ƃ�
	if (Parent != nullptr && Color == BLUE) {

		//�ՖʍX�V
		copyteam[RED].SetAgentAction(Parent->VAction);
		copyteam[BLUE].SetAgentAction(VAction);
		SystemKeeper::CheckConflict(copyboard, VAgentsArray);
		copyboard.Update(VAgentsArray);

		//�G�[�W�F���g�X�V
		copyteam[RED].Update();
		copyteam[BLUE].Update();

	}

	auto iscan = SystemKeeper::GetCanActionList(copyboard, copyteam[NEXTCOLOR(Color)]);
	auto LogList = DevideRolloutLog();

	//���݂̏�Ԃ���s���\�Ȃ��ׂĂ̗v�f�𐶐�����
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (iscan[i][j] == false) continue;

			if (Color == RED) {
				//��������̃m�[�h�̂Ƃ�
				Child.push_back(new State(
					this,
					VBoard,
					array<int, 2>{i + 1, j + 1},
					VTeam,
					Depth + 1,
					BLUE,
					LogList[i][j]
				));
			}
			else if (Color == BLUE) {
				//���������̃m�[�h�̂Ƃ�
				Child.push_back(new State(
					this,
					copyboard,
					array<int, 2>{i + 1, j + 1},
					copyteam,
					Depth + 1,
					RED,
					LogList[i][j]
				));
			}

		}
	}

	//Rollout�̃��O�̍폜
	RolloutLog.clear();

}

//�m�[�h�F�]���l�i�����j�̎Z�o
void State::Evaluation(const int &leaveturn) {

	//�K�ꂽ�񐔂̍X�V
	UpdateCount();

	//1����K��Ă��Ȃ��Ƃ��́A������0�ɂ���
	if (Count == 0) {
		ResultSum = 0;
		Qvalue = 0;
		return;
	}

	//Rollout�̌��ʂ𔽉f
	double ro = Rollout(leaveturn);

	//if (Color == RED)	ResultSum = ((Result * (Count - 1)) + ro) / Count;
	if (Color == RED)	ResultSum += ro;
	else				ResultSum -= ro;
	Qvalue = ResultSum / Count;
	

}

//�m�[�h�FRollout�ɂ�錋�ʂ̗\�������A��������true�A�����������͕�������false��Ԃ�
int State::Rollout(const int &leaveturn) {

	array<Team, TEAMNUM> VirtualTeam = VTeam;
	Board VirtualBoard = VBoard;
	SystemKeeper VirtualSystem((leaveturn > FileReader::GetSearchDeep()) ? FileReader::GetSearchDeep() : leaveturn);
	int Winner = -1;
	GameLog* Log = new GameLog;
	vector<Agent*> agents = { &(VirtualTeam[RED].Agents[AGENT1]), &(VirtualTeam[RED].Agents[AGENT2]), &(VirtualTeam[BLUE].Agents[AGENT1]), &(VirtualTeam[BLUE].Agents[AGENT2]) };

	VirtualBoard.Update(agents);
	VirtualSystem.CalcScore(VirtualBoard);

	//���^�[���̏���
	if (Parent != nullptr) {

		//�G�[�W�F���g�̎v�l
		//MYTEAM��VAction
		if (Color == RED) {
			VirtualTeam[RED].SetAgentAction(VAction);
			VirtualTeam[BLUE].EasyAI(VirtualBoard, VirtualTeam[RED]);
		}
		else {
			VirtualTeam[RED].SetAgentAction(Parent->VAction);
			VirtualTeam[BLUE].SetAgentAction(VAction);
		}

		//�Փˊm�F
		SystemKeeper::CheckConflict(VirtualBoard, agents);

		//���O���L�^
		Log->Hand.push_back(array< array<int, AGENTNUM>, TEAMNUM>{VirtualTeam[RED].GetAgentAction(), VirtualTeam[BLUE].GetAgentAction()});

		//�{�[�h�X�V
		VirtualBoard.Update(agents);

		//�G�[�W�F���g�X�V
		VirtualTeam[RED].Update();
		VirtualTeam[BLUE].Update();

		//�X�R�A�v�Z
		VirtualSystem.CalcScore(VirtualBoard);

		//�^�[���X�V
		VirtualSystem.Update();

	}

	//�Q�[���ʂ�ɐi�߂�
	while (true) {

		//�G�[�W�F���g�̎v�l
		VirtualTeam[RED].EasyAI(VirtualBoard, VirtualTeam[BLUE]);
		VirtualTeam[BLUE].EasyAI(VirtualBoard, VirtualTeam[RED]);

		//�Փˊm�F
		SystemKeeper::CheckConflict(VirtualBoard, agents);

		//���O���L�^
		Log->Hand.push_back(array< array<int, AGENTNUM>, TEAMNUM>{VirtualTeam[RED].GetAgentAction(), VirtualTeam[BLUE].GetAgentAction()});

		//�{�[�h�X�V
		VirtualBoard.Update(agents);

		//�G�[�W�F���g�X�V
		VirtualTeam[RED].Update();
		VirtualTeam[BLUE].Update();

		//�X�R�A�v�Z
		VirtualSystem.CalcScore(VirtualBoard);

		if (VirtualSystem.isGameFin() == true) break;

		//�^�[���X�V
		VirtualSystem.Update();

	}



	//���s����
	Winner = VirtualSystem.JudgeWinner();

	//���O���L�^
	//Log->Winner = Winner;
	Log->Winner = VirtualSystem.GetScore(RED_TOTAL) - VirtualSystem.GetScore(BLUE_TOTAL);
	RolloutLog.push_back(Log);

	//���ʂ�Ԃ�
	return Log->Winner;

}

//�m�[�h�F�o�b�N�v���p�Q�[�V����
void State::Backup() {

	//�t�m�[�h�ȊO�͎q�m�[�h�̑S�Ă̕]���l�𑫂������̂����g�̕]���l�i�����j�Ƃ���
	if (!Child.empty()) {

		ResultSum = 0;
		Qvalue = 0;
		for (auto itr = Child.begin(); itr != Child.end(); itr++) {
			ResultSum -= (*itr)->ResultSum;
		}
		Qvalue = ResultSum / Count;

	}

	//�q�m�[�h��UCB�l���Čv�Z
	for (auto itr = Child.begin(); itr != Child.end(); itr++) (*itr)->CalcUCB();

	//�e�m�[�h�ōċN
	if (Parent != nullptr) Parent->Backup();

}

//�m�[�h�FUCB�i�T�����Ғl�j���v�Z
void State::CalcUCB() {

	//1����K��Ă��Ȃ���΁A�T�����Ғl���ő�ɂ���
	if (Count == 0) {
		UCB = DBL_MAX;
		return;
	}

	//UCB�̎��ɏ]���ċ��߂�
	UCB = (Qvalue/FileReader::GetCostParam()) + (sqrt(2 * log(Parent->Count) / (double)Count));

}

//�m�[�h�F�q�m�[�h�̒��ōł��T���񐔂��������Ԃ�
array<int, AGENTNUM> State::MaxCountHand() {

	int c = INT_MIN;	//�T���񐔂̍ő吔
	State* s = NULL;	//�T���񐔂��ő�̗v�f

	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if (c <= (*itr)->Count) {
			c = (*itr)->Count;
			s = (*itr);
		}
	}

	return s->VAction;

}

//�m�[�h�F�q�m�[�h�̒��ōł����σX�R�A���������Ԃ�
array<int, AGENTNUM> State::MaxResultHand() {

	double c = INT_MIN;	//���σX�R�A�̍ő吔
	State* s = NULL;	//���σX�R�A���ő�̗v�f

	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if (c <= (*itr)->Qvalue) {
			c = (*itr)->Qvalue;
			s = (*itr);
		}
	}

	return s->VAction;

}

//�m�[�h�F���O���s�����Ƃɕ������ĕԂ�
array< array< vector<GameLog*>, ACTIONNUM>, ACTIONNUM > State::DevideRolloutLog() {

	array< array< vector<GameLog*>, ACTIONNUM>, ACTIONNUM > ans;
	int Aturn = (Depth / 2);
	array<int, AGENTNUM> tmp;

	for (auto itr = RolloutLog.begin(); itr != RolloutLog.end(); itr++) {
		tmp = (*itr)->Hand[Aturn][NEXTCOLOR(Color)];	//!!!�^�[�������l�ʂ��Ȃ���΂Ȃ�Ȃ� depth/2
		ans[tmp[AGENT1] - 1][tmp[AGENT2] - 1].push_back(*itr);
	}

	return ans;

}

//�n���ꂽ���O�̔z�񂩂�X�R�A���Z�o
void State::WinFromLog() {

	Qvalue = 0;
	ResultSum = 0;
	for (auto itr = RolloutLog.begin(); itr != RolloutLog.end(); itr++) {
		if (Color == RED)	ResultSum += (*itr)->Winner;
		else				ResultSum -= (*itr)->Winner;
	}

	if (Count == 0) return;
	Qvalue /= (double)Count;

}

//�����Ă���S�Ẵ��O���폜
void State::DeleteRolloutLog() {

	for (auto itr = RolloutLog.begin(); itr != RolloutLog.end(); itr++) {
		if (*itr != nullptr) {
			delete *itr;
			*itr = nullptr;
		}
	}

	RolloutLog.clear();

}

//�S�Ă̎q�m�[�h���폜
void State::DeleteChildNode() {

	if (Child.empty()) return;

	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if (*itr != nullptr) {

			//�ċN
			(*itr)->DeleteChildNode();

			//�S�Ă�Rollout���O����������A�m�[�h���폜
			(*itr)->DeleteRolloutLog();
			delete *itr;
			*itr = nullptr;

		}
	}

	Child.clear();

}

//�m�[�h�̐[����2�グ��
void State::UpdateDepth() {

	if (!Child.empty()) {
		for (auto itr = Child.begin(); itr != Child.end(); itr++) (*itr)->UpdateDepth();
	}

	Depth -= 2;

	if (Depth < 0) exit(-1);

}

void State::UpdateInfo() {

	vector<Agent*> Agents = {
		&(VTeam[RED].Agents[AGENT1]),
		&(VTeam[RED].Agents[AGENT2]),
		&(VTeam[BLUE].Agents[AGENT1]),
		&(VTeam[BLUE].Agents[AGENT2])
	};

	//�ՖʍX�V
	VTeam[RED].SetAgentAction(Parent->VAction);
	VTeam[BLUE].SetAgentAction(VAction);
	SystemKeeper::CheckConflict(VBoard, Agents);
	VBoard.Update(Agents);

	//�G�[�W�F���g�X�V
	VTeam[RED].Update();
	VTeam[BLUE].Update();

}
