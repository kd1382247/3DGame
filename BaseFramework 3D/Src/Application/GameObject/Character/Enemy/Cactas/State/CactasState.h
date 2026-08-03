#pragma once

enum class CactasAnimationType
{
	None,
	Attack,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class CactasActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class CactasMoveState
{
	Idle,
	Walk
};
