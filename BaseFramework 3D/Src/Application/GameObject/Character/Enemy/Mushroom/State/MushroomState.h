#pragma once

enum class MushroomAnimationType
{
	None,
	Attack,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class MushroomActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class MushroomMoveState
{
	Idle,
	Walk
};
