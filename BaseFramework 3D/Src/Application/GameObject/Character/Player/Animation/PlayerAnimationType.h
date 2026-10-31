#pragma once

enum class PlayerAnimationType
{
	None,

	// ３段攻撃
	Attack1,
	Attack2,
	Attack3,
	AttackSpin,

	// チャージ中
	ChargeAttackIDLE,
	ChargeAttackBWD,
	ChargeAttackFWD,
	ChargeAttackLFT,
	ChargeAttackRGT,

	//ガード
	GuardStart,
	GuardEnd,
	GuardIDLE,
	GuardWalkBWD,
	GuardWalkFWD,
	GuardWalkLFT,
	GuardWalkRGT,
	GuardHit,
	GuardBreak,
	Parry,

	Die,
	DieStay,

	Dizzy,

	GetHit,
	GetUp,

	// 
	Idle,
	IdleNormal,
	JumpStart,
	JumpAir,
	JumpLand,
	JumpSpin,
	
	MoveBWD,
	MoveFWD,
	MoveLFT,
	MoveRGT,
	
	SprintFWD,
	LevelUp,
	Vectory
};