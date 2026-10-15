#pragma once

enum class CollisionLayer
{
	Player,
	Enemy,
	PlayerAttack,
	EnemyAttack,
	CharacterBump,
	Gimmick,
	AABB,
	OBB,
	Gate,		// 部屋の柵(RoomGate)
	Area,
	Max
};