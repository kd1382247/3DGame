#pragma once

enum class MageAnimationType
{
	None,
	Attack1,
	Attack2,
	GetHit,
	Idle,
	Walk,
	Die
};

// Mageの攻撃パターン
enum class MageAttackPattern
{
	Summon,        // 1. 敵を召喚
	TargetCircle,  // 2. プレイヤー位置に魔法攻撃(魔法円)
	ForwardSector, // 3. 前方に扇形の範囲攻撃
	Bolt,          // 4. プレイヤーへ単発魔法弾
	NovaCircle,    // 5. ボス中心から半径数メートルの範囲攻撃(HP50%以下限定)

	Num
};
