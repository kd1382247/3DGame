#pragma once

#include"../EffectBase.h"

class HitEffect :public EffectBase
{

public:

	HitEffect() {}
	~HitEffect()override {}

	void Init()override;
	void Update()override;
	void DrawEffect()override;
	void DrawBright()override;

	void SetUpReference()  override;


private:

	const float maxAnimCount = 16.0f;

};