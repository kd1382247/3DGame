#pragma once

#include"../../../StageBase.h"

class GroundCollision:public StageBase
{

public:
	GroundCollision() {}
	~GroundCollision()override{}

	void Init()override;
	void DrawLit()override{}

	void SetMatrix(Math::Matrix mat) { m_mWorld = mat; }
};
