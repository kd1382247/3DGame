#pragma once

#include"../Animation/PlayerAnimationType.h"
#include"../Parameter/PlayerParameter.h"

class PlayerParry
{
public:

	PlayerParry(){}
	~PlayerParry(){}

	void Init(const PlayerParameter::ParryParam& param) { m_pParam = &param; }

	void StartParry();

	void UpdateParryWindow(const float deltaTime);
	

	// パリィが成功したかどうか
	void SetIsParrySuccess(const bool flg) { m_isParrySuccess = flg; }
	bool GetIsParrySuccess()const          { return m_isParrySuccess; }

	bool IsParryActive()const { return m_isParryActive; }


private:


	const PlayerParameter::ParryParam * m_pParam = nullptr;

	float m_parryTimer = 0.0f;
	bool  m_isParryActive = false;
	bool  m_isParrySuccess = false;
};