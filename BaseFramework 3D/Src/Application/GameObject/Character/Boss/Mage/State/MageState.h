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
	Summon,       // 1. 敵を召喚
	MagicCircle,  // 2. プレイヤー位置に魔法円
	ForwardArea,  // 3. 前方範囲魔法
	Bolt,         // 4. プレイヤーへ単発魔法弾
	AllDirection, // 5. 全方位魔法(HP50%以下)
	Clone,        // 6. 分身(HP50%以下)

	Num
};
