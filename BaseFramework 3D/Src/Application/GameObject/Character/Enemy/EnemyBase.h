#pragma once

#include"../CharacterBase.h"

class EnemyBase :public CharacterBase
{
public:

	EnemyBase(){}
	~EnemyBase()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspecter()override;

private:



private:


};