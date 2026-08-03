#pragma once

enum class BeholderAnimationType
{
	None,
	NormalAttack,
	BeamAttackST,
	BeamAttackRPT,
	BeamAttackEND,
	EnergyAttack,
	Die,
	Dizzy,
	GetHit,
	Idle,
	Walk,
	Vectory
};

// 攻撃しているか
enum class BeholderActionState
{
	Normal,
	Damage,
	NormalAttack,
	BeamAttackST,
	BeamAttackRPT,
	BeamAttackEND,
	EnergyAttack,
	Death,
};

enum class BeholderMoveState
{
	Idle,
	Walk
};
