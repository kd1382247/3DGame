#pragma once

enum class PlayerAnimationType
{
	None,

	// ３段攻撃
	Attack1,
	Attack2,
	Attack3,

	// 回転切
	SpinAttack,

	// チャージ中
	SpinChargeIDLE,
	SpinChargeBWD,
	SpinChargeFWD,
	SpinChargeLFT,
	SpinChargeRGT,
	SpinChargeStart,

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

	Idle,
	IdleNormal,

	// ジャンプ
	JumpStart,
	JumpRise,
	JumpFall,
	JumpLand,
	JumpSpin,
	
	// 移動
	MoveBWD,
	MoveFWD,
	MoveLFT,
	MoveRGT,
	
	SprintFWD,
	LevelUp,
	Vectory
};