#pragma once

#include"../Character/CharacterBase.h"

class EnemyBase :public CharacterBase
{
public:

	EnemyBase(){}
	~EnemyBase()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

private:



private:


};