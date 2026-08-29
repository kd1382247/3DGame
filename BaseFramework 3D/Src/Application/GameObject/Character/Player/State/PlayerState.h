#pragma once

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
	SpecialMove,
	Damage,
	Die,
	Guard,
	JumpStart,
	JumpAir,
	JumpLand

};

enum class PlayerMoveState
{
	Idle,
	Run
};
