#pragma once

#include"../../StageOwnedObject/StageOwnedObject.h"

class StageObject;

// 敵の視線を遮るブロック(AABB)
// エディタで配置し、ステージモデルごとに保存する
// EnemyBaseが「プレイヤーとの間にブロックがあるか」をレイで調べるために使う
// キャラクターとの当たり判定には使わない(CollisionManagerには登録しない)
class AIBlock :public StageOwnedObject
{

public:
	AIBlock() {}
	~AIBlock()override {}

	void Init()override;

	void DrawDebug()override;

	// ワールド座標でのAABB(位置と大きさから作る)
	DirectX::BoundingBox GetBox()const;

	void Destroy()override;


};
