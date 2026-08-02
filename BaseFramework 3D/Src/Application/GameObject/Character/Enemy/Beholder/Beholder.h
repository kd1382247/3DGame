#pragma once

#include"../EnemyBase.h"

class Beholder :public EnemyBase
{
public:

	Beholder(){}
	~Beholder()override{}

	void Init()override;
	void Update()override;
	void PostUpdate()override;


private:




};