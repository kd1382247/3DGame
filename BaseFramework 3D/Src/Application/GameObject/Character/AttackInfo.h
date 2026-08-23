#pragma once

struct AttackInfo
{
	int damage = 0;

	// ノックバック方向
	Math::Vector3 knockBackDir = Math::Vector3::Zero;

	// ノックバックの強さ
	float knockBackPower = 0.0f;

};