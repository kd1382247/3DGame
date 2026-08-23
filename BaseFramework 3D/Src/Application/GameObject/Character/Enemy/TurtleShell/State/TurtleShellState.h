#pragma once

enum class TurtleShellAnimationType
{
	None,
	SpinAttackST,
	SpinAttackRPT,
	GetHit,
	Dizzy,
	Idle,
	Walk,
	Die
};


enum class TurtleShellActionState
{
	Normal,
	Damage,
	Dizzy,
	SpinAttackST,
	SpinAttackRPT,
	Death
};

enum class TurtleShellMoveState
{
	Idle,
	Walk
};
