#include "PlayerSpinAttack.h"

Math::Vector3 PlayerSpinAttack::CalcMoveVector(const float deltaTime) const
{
	return m_specialMoveDir * (m_pParam->m_moveSpeed * 60.0f) * deltaTime;
}

const PlayerActionTiming& PlayerSpinAttack::GetTiming() const
{
	// ここの数値を変えると、攻撃判定やトレイルが出るフレームを調整できる
	static const PlayerActionTiming timing = { 10.0f, 30.0f, 0.0f, 40.0f };

	return timing;
}

