#pragma once

// �S�Ẵ^�X�N�N���X�̊��N���X
class Task{

public:
	Task() {};
	virtual ~Task() {};

	virtual void Initialize()	 = 0;
	virtual void Finalize()		 = 0;
	virtual void Update()		 = 0;
	virtual void Draw()			 = 0;

};
