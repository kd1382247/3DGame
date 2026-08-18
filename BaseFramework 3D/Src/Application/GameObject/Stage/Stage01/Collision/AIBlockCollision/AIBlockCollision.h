#pragma once

#include"../../../StageBase.h"

class AIBlockCollision :public StageBase
{

public:
	AIBlockCollision() {}
	~AIBlockCollision()override {}

	void Init()override;
	void DrawLit()override {}
};
