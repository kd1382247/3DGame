#pragma once

enum class GolemAnimationType
{
	None,
	Attack1,
	Attack2,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class GolemActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class GolemMoveState
{
	Idle,
	Walk
};
