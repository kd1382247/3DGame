#pragma once

#include"CollisionLayer/CollisionLayer.h"

class KdGameObject;
class CharacterBase;


class CollisionManager
{
public:

	void DrawDebug();

	void Init();

	bool SphereVsAABB(const DirectX::BoundingSphere& sphere, const DirectX::BoundingBox& box, Math::Vector3& outPush, Math::Vector3& outNormal);

	using objectList = std::vector<std::weak_ptr<KdGameObject>>;

	void RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object);

	void UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object);


	const objectList& GetObjects(CollisionLayer layer)const;

	void RemoveExpiredObjects();

	void Clear();

	void Resolve();

	// 押し戻し量を細かく分けて壁との当たり判定を行う
	Math::Vector3 ResolveWallCollisionForCharacter(const std::shared_ptr<CharacterBase>& character);

	// 押し戻し量を細かく分けて地面との当たり判定を行う
	void ResolveGroundCollisionForCharacter(const std::shared_ptr<CharacterBase>& character);


	void ResolveCharacterCollision();
	void ResolveWallCollision();
	void ResolveGroundCollision();

private:


	void ApplyCharacterPush(const std::shared_ptr<CharacterBase>& character);

	void ApplyKnockBack(const std::shared_ptr<CharacterBase>& character);

	std::vector<std::shared_ptr<CharacterBase>>GetCharacters();


	void TestGroundSweep();

	void ResolveGroundSnap();

	void ResolveCharacterMovement();

	bool SphereSweepVsAABB(
		const Math::Vector3& sphereCenter,
		float radius,
		const Math::Vector3& move,
		const Math::Vector3& boxMin,
		const Math::Vector3& boxMax,
		float& outTOI,
		Math::Vector3& outNormal);

	bool SegmentVsAABB(
		const Math::Vector3& start,
		const Math::Vector3& move,
		const Math::Vector3& boxMin,
		const Math::Vector3& boxMax,
		float& outTOI,
		Math::Vector3 &outNormal
	);



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