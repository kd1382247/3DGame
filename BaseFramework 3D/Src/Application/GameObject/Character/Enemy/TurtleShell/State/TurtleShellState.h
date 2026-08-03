#pragma once

enum class TurtleShellAnimationType
{
	None,
	RotationAttackST,
	RotationAttackRPT,
	GetHit,
	Idle,
	Walk,
	Die
};


enum class TurtleShellActionState
{
	Normal,
	Damage,
	Attack,
	Death
};

enum class TurtleShellMoveState
{
	Idle,
	Walk
};
