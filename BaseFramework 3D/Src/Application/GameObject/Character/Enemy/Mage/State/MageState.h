#pragma once

enum class MageAnimationType
{
	None,
	Attack1,
	Attack2,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class MageActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class MageMoveState
{
	Idle,
	Walk
};
