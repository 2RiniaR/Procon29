#include "def.h"
#include "Agent.h"

// �R���X�g���N�^
Agent::Agent(Pos2D pos, int color) {

	Position = pos;
	MyColor = color;
	NextAction = STAY;

}

//�s��NextAction�ɂ���čs��������̏ꏊ
Pos2D Agent::PosAfterAction() const {

	switch (NextAction) {

	case MOVE_UP:
		return Pos2D(Position + Pos2D(0, -1));
		break;

	case MOVE_UPRIGHT:
		return Pos2D(Position + Pos2D(1, -1));
		break;

	case MOVE_RIGHT:
		return Pos2D(Position + Pos2D(1, 0));
		break;

	case MOVE_DOWNRIGHT:
		return Pos2D(Position + Pos2D(1, 1));
		break;

	case MOVE_DOWN:
		return Pos2D(Position + Pos2D(0, 1));
		break;

	case MOVE_DOWNLEFT:
		return Pos2D(Position + Pos2D(-1, 1));
		break;

	case MOVE_LEFT:
		return Pos2D(Position + Pos2D(-1, 0));
		break;

	case MOVE_UPLEFT:
		return Pos2D(Position + Pos2D(-1, -1));
		break;

	default:
		return Position;
		break;

	}

}

//�s���ɂ���ċ�������ɂȂ�ꏊ
Pos2D Agent::PosWithAction() const {

	switch (NextAction) {

	case MOVE_UP:
		return Pos2D(Position + Pos2D(0, -1));
		break;

	case MOVE_UPRIGHT:
		return Pos2D(Position + Pos2D(1, -1));
		break;

	case MOVE_RIGHT:
		return Pos2D(Position + Pos2D(1, 0));
		break;

	case MOVE_DOWNRIGHT:
		return Pos2D(Position + Pos2D(1, 1));
		break;

	case MOVE_DOWN:
		return Pos2D(Position + Pos2D(0, 1));
		break;

	case MOVE_DOWNLEFT:
		return Pos2D(Position + Pos2D(-1, 1));
		break;

	case MOVE_LEFT:
		return Pos2D(Position + Pos2D(-1, 0));
		break;

	case MOVE_UPLEFT:
		return Pos2D(Position + Pos2D(-1, -1));
		break;

	case DESTROY_UP:
		return Pos2D(Position + Pos2D(0, -1));
		break;

	case DESTROY_UPRIGHT:
		return Pos2D(Position + Pos2D(1, -1));
		break;

	case DESTROY_RIGHT:
		return Pos2D(Position + Pos2D(1, 0));
		break;

	case DESTROY_DOWNRIGHT:
		return Pos2D(Position + Pos2D(1, 1));
		break;

	case DESTROY_DOWN:
		return Pos2D(Position + Pos2D(0, 1));
		break;

	case DESTROY_DOWNLEFT:
		return Pos2D(Position + Pos2D(-1, 1));
		break;

	case DESTROY_LEFT:
		return Pos2D(Position + Pos2D(-1, 0));
		break;

	case DESTROY_UPLEFT:
		return Pos2D(Position + Pos2D(-1, -1));
		break;

	default:
		return Position;
		break;

	}

}

//�����I�ɍs���𖳌��Ƃ���
void Agent::ForcedStay(){

	NextAction = STAY; 

}

//�s�����v���p�e�B�ɓK�p����
void Agent::Update() {

	Position = PosAfterAction();
	NextAction = STAY;

}

void Agent::Update(int act) {

	Position = PosAfterAction(act);
	NextAction = STAY;

}

//�s�������肷��
void Agent::Action() {

	NextAction = rand() % 17 + 1;

}

Pos2D Agent::PosAfterAction(int act) const {

	switch (act) {

	case MOVE_UP:
		return Pos2D(Position + Pos2D(0, -1));
		break;

	case MOVE_UPRIGHT:
		return Pos2D(Position + Pos2D(1, -1));
		break;

	case MOVE_RIGHT:
		return Pos2D(Position + Pos2D(1, 0));
		break;

	case MOVE_DOWNRIGHT:
		return Pos2D(Position + Pos2D(1, 1));
		break;

	case MOVE_DOWN:
		return Pos2D(Position + Pos2D(0, 1));
		break;

	case MOVE_DOWNLEFT:
		return Pos2D(Position + Pos2D(-1, 1));
		break;

	case MOVE_LEFT:
		return Pos2D(Position + Pos2D(-1, 0));
		break;

	case MOVE_UPLEFT:
		return Pos2D(Position + Pos2D(-1, -1));
		break;

	default:
		return Position;
		break;

	}

}

Pos2D Agent::PosWithAction(int act) const {

	switch (act) {

	case MOVE_UP:
		return Pos2D(Position + Pos2D(0, -1));
		break;

	case MOVE_UPRIGHT:
		return Pos2D(Position + Pos2D(1, -1));
		break;

	case MOVE_RIGHT:
		return Pos2D(Position + Pos2D(1, 0));
		break;

	case MOVE_DOWNRIGHT:
		return Pos2D(Position + Pos2D(1, 1));
		break;

	case MOVE_DOWN:
		return Pos2D(Position + Pos2D(0, 1));
		break;

	case MOVE_DOWNLEFT:
		return Pos2D(Position + Pos2D(-1, 1));
		break;

	case MOVE_LEFT:
		return Pos2D(Position + Pos2D(-1, 0));
		break;

	case MOVE_UPLEFT:
		return Pos2D(Position + Pos2D(-1, -1));
		break;

	case DESTROY_UP:
		return Pos2D(Position + Pos2D(0, -1));
		break;

	case DESTROY_UPRIGHT:
		return Pos2D(Position + Pos2D(1, -1));
		break;

	case DESTROY_RIGHT:
		return Pos2D(Position + Pos2D(1, 0));
		break;

	case DESTROY_DOWNRIGHT:
		return Pos2D(Position + Pos2D(1, 1));
		break;

	case DESTROY_DOWN:
		return Pos2D(Position + Pos2D(0, 1));
		break;

	case DESTROY_DOWNLEFT:
		return Pos2D(Position + Pos2D(-1, 1));
		break;

	case DESTROY_LEFT:
		return Pos2D(Position + Pos2D(-1, 0));
		break;

	case DESTROY_UPLEFT:
		return Pos2D(Position + Pos2D(-1, -1));
		break;

	default:
		return Position;
		break;

	}

}

void Agent::ChangePos(const Pos2D &p) {

	Position = p;

}
