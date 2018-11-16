#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "Board.h"
#include "Cell.h"
#include "FileReader.h"

//�S��NULL�ŏ�����
int FileReader::EndTurn(0);
Board* FileReader::ReadBoard(NULL);
array< array<Pos2D, AGENTNUM>, TEAMNUM >* FileReader::InitPos(NULL);

int FileReader::RolloutNum(10000);
int FileReader::SearchDeep(15);
double FileReader::CostParam(22.0);

//�R���X�g���N�^
FileReader::FileReader() {

}

//�f�X�g���N�^
FileReader::~FileReader(){

}

//�t�@�C�����J���ēǂݍ���
void FileReader::ReadBoardInfo(const char *filename) {

	FILE *file;

	//�t�@�C�����J��
	if (fopen_s(&file, filename, "r") != 0) {
		exit(1);
	}

	//�c�Ɖ��̐��𐔂���
	int width, height;
	fscanf_s(file, "%d %d:", &height, &width);

	//�}�X�̒l��ǂݍ���
	vector< vector<Cell> > Rcells(width, vector<Cell>(height));
	int tmp;
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {

			fscanf_s(file, "%d", &tmp);
			Rcells[j][i].Point = tmp;
			fscanf_s(file, " ");

		}
		fscanf_s(file, ":");
	}

	ReadBoard = new Board(Rcells);

	//�G�[�W�F���g�̏����ʒu��ǂݍ���
	int initX1, initY1, initX2, initY2;
	fscanf_s(file, "%d %d:%d %d", &initY1, &initX1, &initY2, &initX2);
	InitPos = new array< array<Pos2D, AGENTNUM>, TEAMNUM >;
	initX1--, initX2--, initY1--, initY2--;
	InitPos->at(RED)[AGENT1] = Pos2D(initX1, initY1);
	InitPos->at(RED)[AGENT2] = Pos2D(initX2, initY2);

	//����̏������W���Z�o
	int EnemyX1, EnemyX2, EnemyY1, EnemyY2;

	if ((((width + 1) / 2.0) - initX1)*(((width + 1) / 2.0) - initX2) >= 0) {

		//y���ɑ΂��ē������ɃG�[�W�F���g2�̂�����
		EnemyX1 = width - initX1 - 1;
		EnemyX2 = width - initX2 - 1;

		if ((((height + 1) / 2.0) - initY1)*(((height + 1) / 2.0) - initY2) > 0) {
			//x���ɑ΂��ē������ɃG�[�W�F���g2�̂�����
			EnemyY1 = height - initY1 - 1;
			EnemyY2 = height - initY2 - 1;
		}
		else {
			//x���ɑ΂��Ĕ��Α��ɃG�[�W�F���g��2�̂���
			EnemyY1 = initY1;
			EnemyY2 = initY2;
		}

	}
	else {

		//y���ɑ΂��Ĕ��Α��ɃG�[�W�F���g��2�̂���
		EnemyX1 = initX1;
		EnemyX2 = initX2;

		if ((((height + 1) / 2.0) - initY1)*(((height + 1) / 2.0) - initY2) > 0) {
			//x���ɑ΂��ē������ɃG�[�W�F���g2�̂�����
			EnemyY1 = height - initY1 - 1;
			EnemyY2 = height - initY2 - 1;
		}
		else {
			//x���ɑ΂��Ĕ��Α��ɃG�[�W�F���g��2�̂���
			EnemyY1 = initY2;
			EnemyY2 = initY1;
		}

	}


	InitPos->at(BLUE)[AGENT1] = Pos2D(EnemyX1, EnemyY1);
	InitPos->at(BLUE)[AGENT2] = Pos2D(EnemyX2, EnemyY2);
	
	//�t�@�C�������
	fclose(file);

}


void FileReader::ReadTurnInfo(const char *filename) {

	FILE *file;

	//�t�@�C�����J��
	if (fopen_s(&file, filename, "r") != 0) {
		exit(1);
	}

	fscanf_s(file, "%d", &EndTurn);

	fclose(file);

}

void FileReader::ParamFileRead(const char* filename) {

	FILE *file;
	if (fopen_s(&file, filename, "r") != 0) exit(1);

	fscanf_s(file, "Rollout:%d\n", &RolloutNum);
	fscanf_s(file, "Maxdeep:%d\n", &SearchDeep);
	fscanf_s(file, "Costparam:%lf\n", &CostParam);

	fclose(file);

}


void FileReader::Cparam_Average(double average) {

	CostParam = average * SearchDeep * sqrt(2);

}
