#pragma once

enum class ChestMonsterAnimationType
{
	None,
	Attack,
	GetHit,
	GenerateMinion,
	Idle,
	Walk,
	Die
};


enum class ChestMonsterActionState
{
	Normal,
	Damage,
	Attack,
	GenerateMinion,
	Death
};

enum class ChestMonsterMoveState
{
	Idle,
	Walk
};
