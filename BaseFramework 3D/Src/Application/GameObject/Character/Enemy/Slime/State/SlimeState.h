#pragma once

enum class SlimeAnimationType
{
	None,
	Attack,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class SlimeActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class SlimeMoveState
{
	Idle,
	Walk
};
