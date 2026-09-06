#pragma once

#include"CollisionLayer/CollisionLayer.h"

class KdGameObject;
class CharacterBase;


class CollisionManager
{
public:

	void DrawDebug();

	
	using objectList = std::vector<std::weak_ptr<KdGameObject>>;

	void RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object);

	void UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object);


	const objectList& GetObjects(CollisionLayer layer)const;

	void RemoveExpiredObjects();

	void Clear();

	void Resolve();

	bool SphereVsAABB(
		const DirectX::BoundingSphere& sphere,
		const DirectX::BoundingBox& box,
		Math::Vector3& outPush,
		Math::Vector3& outNormal);

private:


	void Init();


	struct SweepHitResult
	{
		bool  m_hit = false;
		float m_toi = 1.0f;
		Math::Vector3 m_normal = Math::Vector3::Zero;
	};

	void UpdateClosestHit(SweepHitResult& closestHit, float toi, const Math::Vector3& normal);

	// Character取得
	std::vector<std::shared_ptr<CharacterBase>>GetCharacters();


	

	// Character Movement
	void ResolveCharacterMovement();

	// ボックスのめり込みを解決する
	void ResolveAABBStartOverlap(const std::shared_ptr<CharacterBase>& character, Math::Vector3& currentPos, Math::Vector3& remainingMove);
	void ResolveOBBStartOverlap(const std::shared_ptr<CharacterBase>& character, Math::Vector3& currentPos, Math::Vector3& remainingMove);


	float GetUpDot(const SweepHitResult& closestHit)const { return closestHit.m_normal.Dot(Math::Vector3::Up); }

	bool IsWalkableSurface(const SweepHitResult& closestHit)const;

	// 近いボックスとスウィープ判定
	void ResolveSweepHit(const std::shared_ptr<CharacterBase>& character, const SweepHitResult& hit, Math::Vector3& currentPos, Math::Vector3& remainingMove);




	// Collision Test
	
	SweepHitResult FindClosestOBBHit(const std::shared_ptr<CharacterBase>& character,const Math::Vector3& currentPos,const Math::Vector3& remainingMove);

	SweepHitResult FindClosestAABBHit(const std::shared_ptr<CharacterBase>& character,const Math::Vector3& currentPos,const Math::Vector3& remainingMove);

	bool SphereVsOBB(const DirectX::BoundingSphere& sphere,const DirectX::BoundingOrientedBox& obb,Math::Vector3& outPush,Math::Vector3& outNormal);

	bool SphereSweepVsOBB(const Math::Vector3& start,const Math::Vector3& move,float radius,const DirectX::BoundingOrientedBox& obb,float& outTOI,Math::Vector3& outNormal);


	bool SphereSweepVsAABB(const Math::Vector3& sphereCenter,float radius,const Math::Vector3& move,const Math::Vector3& boxMin,const Math::Vector3& boxMax,float& outTOI,Math::Vector3& outNormal);

	bool SegmentVsAABB(const Math::Vector3& start,const Math::Vector3& move,const Math::Vector3& boxMin,const Math::Vector3& boxMax,float& outTOI,Math::Vector3 &outNormal);


	// Legacy / 保留中

	void ResolveGroundSnap();
	void ResolveBoxGroundSnap();


	// 押し戻し量を細かく分けて壁との当たり判定を行う
	Math::Vector3 ResolveWallCollisionForCharacter(const std::shared_ptr<CharacterBase>& character);

	// 押し戻し量を細かく分けて地面との当たり判定を行う
	void ResolveGroundCollisionForCharacter(const std::shared_ptr<CharacterBase>& character);


	void ResolveCharacterCollision();
	void ResolveWallCollision();
	void ResolveGroundCollision();

	void ApplyCharacterPush(const std::shared_ptr<CharacterBase>& character);

	void ApplyKnockBack(const std::shared_ptr<CharacterBase>& character);


	// レイヤーのサイズを取得
	static constexpr size_t LayerCount =
		static_cast<size_t>(CollisionLayer::Max);

	std::array<objectList, LayerCount>m_objectLists;


	std::unique_ptr<KdDebugWireFrame>m_pDebugWire=nullptr;

private: // シングルトンパターン

	CollisionManager() { Init(); }
	~CollisionManager(){}

public:

	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}
};