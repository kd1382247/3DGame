#pragma once

#include"../../../../System/CollisionManager/IObstacle/IObstacle.h"
#include"../../StageOwnedObject/StageOwnedObject.h"

class StageObject;

class OBBCollision :public StageOwnedObject,public IObstacle
{

public:

	enum class OBBCollisionType
	{
		Solid,
		Walkable
	};

	OBBCollision() {}
	~OBBCollision()override {}

	void Init()override;
	void DrawLit()override {}

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

	// 歩ける地面かを返す
	bool IsWalkableOnly()const override { return m_collisionType == OBBCollisionType::Walkable; }


	void SetCollisionType(const OBBCollisionType type) { m_collisionType = type; }
	OBBCollisionType GetCollisionType()const     { return m_collisionType; }

	void SetCollisionType(const std::string& typeName);
	std::string GetCollisionTypeName()const { return m_collisionTypeName[static_cast<int>(GetCollisionType())]; }


	DirectX::BoundingOrientedBox GetBox()const;

	// 位置・大きさ・回転は固定表示(スクロールしない)側に出す
	void DrawInspectorHeader()override;

	void Destroy()override;

private:

	OBBCollisionType m_collisionType = OBBCollisionType::Walkable;

	static inline const std::string m_collisionTypeName[2] = { {"Solid"},{"Walkable"} };


};
