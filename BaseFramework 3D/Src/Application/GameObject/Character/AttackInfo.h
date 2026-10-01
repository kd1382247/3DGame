#pragma once

struct AttackInfo
{
	int m_damage = 0;

	// ノックバック方向
	Math::Vector3 m_knockBackDir = Math::Vector3::Zero;

	// ノックバックの強さ
	float m_knockBackPower = 0.0f;

	// ヒットストップの時間
	float m_hitStopDuration = 0.0f;

	// 撃破時のヒットストップ
	float m_killHitStopDuration = 0.0f;

	// 撃破時のスロー演出
	float m_killSlowScale = 0.0f;
	float m_killSlowDuration = 0.0f;
	
};