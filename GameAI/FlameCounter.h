#pragma once

// FPS�v���N���X
class FlameCounter{

private:
	int mStartTime;
	int mCount;
	float mFps;
	const int N = 60;
	const int FPS = 60;

public:
	FlameCounter();
	~FlameCounter();

	bool FlameCount();
	void Wait();

};

