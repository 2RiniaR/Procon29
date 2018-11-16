#include "DxLib.h"
#include "FlameCounter.h"

//--------------------------------------
// コンストラクタ
//--------------------------------------
FlameCounter::FlameCounter(){
}

//--------------------------------------
// デストラクタ
//--------------------------------------
FlameCounter::~FlameCounter(){
}

//--------------------------------------
// 処理待ち時間の計算
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
// 処理を待つ
//--------------------------------------
void FlameCounter::Wait() {

	int tookTime = GetNowCount() - mStartTime;
	int waitTime = mCount * 1000 / FPS - tookTime;
	if (waitTime > 0) {
		Sleep(waitTime);
	}

}