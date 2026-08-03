#pragma once

enum class StarFishAnimationType
{
	None,
	Attack,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class StarFishActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class StarFishMoveState
{
	Idle,
	Walk
};
