#pragma once

struct AttackInfo
{
	int m_damage = 0;

	// ノックバック方向
	Math::Vector3 m_knockBackDir = Math::Vector3::Zero;

	// ノックバックの距離
	float m_knockBackDist = 0.0f;

	// 上方向に浮き上がる量
	float m_knockBackHeight = 0.0f;

	// ヒットストップの時間
	float m_hitStop = 0.0f;

	// 撃破時のヒットストップ
	float m_killHitStop = 0.0f;

	// 撃破時のスロー演出
	float m_SlowScale = 0.0f;
	float m_SlowDuration = 0.0f;

	// 反射できるか
	bool  m_canReflect = false;

	// ガードできる攻撃なのか
	bool  m_canGuard = true;
	
};