#pragma once

enum class CollisionLayer
{
	Ground,
	Player,
	Enemy,
	PlayerAttack,
	EnemyAttack,
	CharacterBump,
	Gimmick,
	AABB,
	OBB,
	AIBlock,
	Gate,		// 部屋の柵(RoomGate)
	Area,
	Max
};