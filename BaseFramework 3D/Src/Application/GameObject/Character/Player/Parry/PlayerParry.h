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
	void UpdateIncincibility(const float deltaTime);

	// パリィが成功したかどうか
	void SetIsParrySuccess(const bool flg) { m_isParrySuccess = flg; }
	bool GetIsParrySuccess()const          { return m_isParrySuccess; }

	bool IsParryActive()const { return m_isParryActive; }

	void EndParry() { m_isParryActive = false; }

	void SetInvincibility(const bool flg) { m_isParryInvincible = flg; }
	bool IsInvincibility()const { return m_isParryInvincible; }



private:


	const PlayerParameter::ParryParam * m_pParam = nullptr;

	float m_parryTimer = 0.0f;
	bool  m_isParryActive = false;
	bool  m_isParrySuccess = false;



	bool  m_isParryInvincible = false;

	float  m_parryInvincibleTime = 0.0f;
	float  m_parryIFrameDuration = 0.3f;
};