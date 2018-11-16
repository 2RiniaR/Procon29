#include "DxLib.h"
#include "FlameCounter.h"

//--------------------------------------
// �R���X�g���N�^
//--------------------------------------
FlameCounter::FlameCounter(){
}

//--------------------------------------
// �f�X�g���N�^
//--------------------------------------
FlameCounter::~FlameCounter(){
}

//--------------------------------------
// �����҂����Ԃ̌v�Z
//--------------------------------------
bool FlameCounter::FlameCount() {

	if (mCount == 0) {
		mStartTime = GetNowCount();
	}

	if (mCount == N) {
		int t = GetNowCount();
		mFps = 1000.f / ((t - mStartTime) / (float)N);
		mCount = 0;
		mStartTime = t;
	}

	mCount++;
	return true;

}

//--------------------------------------
// ������҂�
//--------------------------------------
void FlameCounter::Wait() {

	int tookTime = GetNowCount() - mStartTime;
	int waitTime = mCount * 1000 / FPS - tookTime;
	if (waitTime > 0) {
		Sleep(waitTime);
	}

}