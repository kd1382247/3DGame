#pragma once

enum class SwarmAnimationType
{
	None,
	Attack,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class SwarmActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class SwarmMoveState
{
	Idle,
	Walk
};

