#pragma once

#define NONE 0

//���W�N���X
class Pos2D {

public:
	int x;
	int y;

	Pos2D() {};		//�R���X�g���N�^
	Pos2D(int x, int y) { this->x = x, this->y = y; };	//�C�j�V�����C�U
	~Pos2D() {};	//�f�X�g���N�^

	Pos2D operator +(const Pos2D &vec) const { return Pos2D(this->x + vec.x, this->y + vec.y); };	//���Z���Z�q
	Pos2D operator -(const Pos2D &vec) const { return Pos2D(this->x - vec.x, this->y - vec.y); };	//���Z���Z�q
	Pos2D operator -() const { return Pos2D(-1 * this->x, -1 * this->y); };	//���̐�
	bool operator ==(const Pos2D &r) const { return (this->x == r.x && this->y == r.y); }
	bool operator !=(const Pos2D &r) const { return  !(*this == r); }

};

//��b���
#define TEAMNUM		2
#define AGENTNUM	2

// �}�X�̐F
#define NEXTCOLOR(c) (c+1)%2
#define WHITE	-1
#define RED		0
#define BLUE	1

// �s��
#define MOVE_UP				1
#define MOVE_UPRIGHT		2
#define MOVE_RIGHT			3
#define MOVE_DOWNRIGHT		4
#define MOVE_DOWN			5
#define MOVE_DOWNLEFT		6
#define MOVE_LEFT			7
#define MOVE_UPLEFT			8
#define DESTROY_UP			9
#define DESTROY_UPRIGHT		10
#define DESTROY_RIGHT		11
#define DESTROY_DOWNRIGHT	12
#define DESTROY_DOWN		13
#define DESTROY_DOWNLEFT	14
#define DESTROY_LEFT		15
#define DESTROY_UPLEFT		16
#define STAY				17
#define ACTIONNUM			17

// �X�R�A�z��
#define RED_TILE	0
#define RED_AREA	1
#define RED_TOTAL	2
#define BLUE_TILE	3
#define BLUE_AREA	4
#define BLUE_TOTAL	5
#define POINTS_SIZE 6

// �G�[�W�F���g
#define NEXTAGENT(a) (a+1)%2
#define AGENT1 0
#define AGENT2 1

#define RANDSEEDMAX 1000000