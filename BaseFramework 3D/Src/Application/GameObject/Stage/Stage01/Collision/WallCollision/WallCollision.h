#pragma once

#include"../../../StageBase.h"

class WallCollision :public StageBase
{

public:
	WallCollision() {}
	~WallCollision()override {}

	void Init()override;
	void DrawLit()override {}


};
