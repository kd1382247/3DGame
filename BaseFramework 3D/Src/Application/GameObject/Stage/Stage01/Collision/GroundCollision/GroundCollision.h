#pragma once

#include"../../../StageBase.h"

class GroundCollision:public StageBase
{

public:
	GroundCollision() {}
	~GroundCollision()override{}

	void Init()override;
	void DrawLit()override{}

	
};
