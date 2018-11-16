#pragma once

#include <array>
#include "def.h"

using namespace std;

// �G�[�W�F���g�N���X
class Agent {

private:
	Pos2D Position;

public:
	int MyColor;	//�����̐F
	int NextAction;	//���̍s��

	Agent(Pos2D pos, int color);
	~Agent() {};

	void Update();			//�s��NextAction��K�p����
	void Update(int act);
	void Action();			//�s��NextAction�����肷��
	void ForcedStay();		//�s��NextAction�������I��Stay�ɂ���

	int GetAction() { return NextAction; };
	void SetAction(int act) { NextAction = act; };
	Pos2D GetPos() const	{ return Position; }
	
	Pos2D PosAfterAction() const;			//NextAction�ł̍s����̈ʒu��Ԃ�
	Pos2D PosAfterAction(int act) const;	//act�ł̍s����̈ʒu��Ԃ�
	Pos2D PosWithAction() const;			//NextAction�ł̍s���̔���ʒu��Ԃ�
	Pos2D PosWithAction(int act) const;		//act�ł̍s���̔���ʒu��Ԃ�

	void ChangePos(const Pos2D &p);

};

