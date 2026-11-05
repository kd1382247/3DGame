#pragma once

#include"../../../../System/CollisionManager/IObstacle/IObstacle.h"
#include"../../StageOwnedObject/StageOwnedObject.h"

class StageObject;

class AABBCollision :public StageOwnedObject,public IObstacle
{

public:
	AABBCollision() {}
	~AABBCollision()override {}

	void Init()override;

	void DrawDebug()override;

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }



	//=====================================================
	// 当たり判定
	//=====================================================

	// ワールドのBoxのMinとMaxを求める
	void GetWorldBounds(
		Math::Vector3& outMin,
		Math::Vector3& outMax)const override;

	// 球とのめり込み判定
	bool OverlapSphere(
		const DirectX::BoundingSphere& sphere,
		Math::Vector3& outPush,
		Math::Vector3& outNormal)const override;

	// 動く球が最初に当たる時刻と法線
	 bool SweepSphere(
		const Math::Vector3& start,
		const Math::Vector3& move,
		float radius,
		float& outToi,
		Math::Vector3& outNormal)const override;



	DirectX::BoundingBox GetBox()const;

	void Destroy()override;

};
