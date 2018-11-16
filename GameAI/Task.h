#pragma once

// 全てのタスククラスの基底クラス
class Task{

public:
	Task() {};
	virtual ~Task() {};

	virtual void Initialize()	 = 0;
	virtual void Finalize()		 = 0;
	virtual void Update()		 = 0;
	virtual void Draw()			 = 0;

};
