#pragma once

enum class BombAnimationType
{
	None,
	Attack,
	GetHit,
	Idle,
	Walk,
	Die
};

// 攻撃しているか
enum class BombActionState
{
	Normal,
	Attack,
	Damage,
	Death
};

enum class BombMoveState
{
	Idle,
	Walk
};
