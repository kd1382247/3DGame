#pragma once

#include"../EffectBase.h"

class StunEffect :public EffectBase
{

public:

	StunEffect(){}
	~StunEffect()override{}

	void Init()override;
	void Update()override;
	void DrawEffect()override;

	void SetUpReference()  override;


private:
	
	float m_effectDuration = 60 * 3;

	const float maxAnimCount = 7.0f;


};