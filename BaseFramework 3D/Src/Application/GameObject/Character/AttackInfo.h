#pragma once

struct AttackInfo
{
	int m_damage = 0;

	// ノックバック方向
	Math::Vector3 m_knockBackDir = Math::Vector3::Zero;

	// ノックバックの強さ
	float m_knockBackPower = 0.0f;

	// ヒットストップの時間
	float m_hitStop = 0.0f;

	// 撃破時のヒットストップ
	float m_killHitStop = 0.0f;

	// 撃破時のスロー演出
	float m_SlowScale = 0.0f;
	float m_SlowDuration = 0.0f;

	// 反射できるか
	bool  m_canReflect = false;
	
};