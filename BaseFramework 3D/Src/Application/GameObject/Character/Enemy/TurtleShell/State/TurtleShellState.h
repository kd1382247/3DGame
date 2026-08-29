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
	Launch,
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
