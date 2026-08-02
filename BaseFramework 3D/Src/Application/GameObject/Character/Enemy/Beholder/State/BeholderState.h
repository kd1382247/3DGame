#pragma once

enum class PlayerAnimationType
{
	None,
	Attack1,
	Attack2,
	Attack3,
	AttackSpin,
	Defend,
	DefendHit,
	Die,
	DieStay,
	Dizzy,
	GetHit,
	GetUp,
	Idle,
	IdleNormal,
	JumpStart,
	JumpAir,
	JumpLand,
	JumpSpin,
	LevelUp,
	MoveBWD,
	MoveFWD,
	MoveLFT,
	MoveRGT,
	SprintFWD,
	Vectory
};
// 方向種類
enum DirType
{
	Up = 1 << 0,	// 上　0000 0001
	Down = 1 << 1,	// 下　0000 0010
	Left = 1 << 2,	// 左　0000 0100
	Right = 1 << 3,	// 右　0000 1000
};

// 攻撃しているか
enum class PlayerActionState
{
	Normal,
	Attack,
	Damage,
	Dead,
	JumpStart,
	JumpAir,
	JumpLand

};

enum class PlayerMoveState
{
	Idle,
	Run
};
