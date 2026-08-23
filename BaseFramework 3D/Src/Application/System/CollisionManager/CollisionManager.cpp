#include "CollisionManager.h"

#include"../../../Framework/GameObject/KdGameObject.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollision.h"


#include"../../GameObject/Character/CharacterBase.h"

namespace
{
	size_t ToIndex(CollisionLayer layer)
	{
		return static_cast<size_t>(layer);
	}
}


void CollisionManager::Init()
{
	m_pDebagWire = std::make_unique<KdDebugWireFrame>();
}

bool CollisionManager::SphereVsAABB(
	const DirectX::BoundingSphere& sphere,
	const DirectX::BoundingBox& box,
	Math::Vector3& outPush,
	Math::Vector3&outNormal)
{
	outPush = Math::Vector3::Zero;

	Math::Vector3 sphereCenter = sphere.Center;

	Math::Vector3 boxCenter = box.Center;
	Math::Vector3 boxExtents = box.Extents;

	// ==============================
    // BoxのMin / Max座標を求める
    // ==============================

	Math::Vector3 boxMin = boxCenter - boxExtents;

	Math::Vector3 boxMax = boxCenter + boxExtents;

	// ==============================
	// Sphere中心から一番近い
	// Box上の点を求める
	// ==============================

	Math::Vector3 closestPoint;

	closestPoint.x = std::clamp(sphereCenter.x, boxMin.x, boxMax.x);
	closestPoint.y = std::clamp(sphereCenter.y, boxMin.y, boxMax.y);
	closestPoint.z = std::clamp(sphereCenter.z, boxMin.z, boxMax.z);

	// ==============================
	// 最近接点 → Sphere中心
	// ==============================

	Math::Vector3 diff = sphereCenter - closestPoint;

	float distanceSq = diff.LengthSquared();

	float radiusSq = sphere.Radius * sphere.Radius;

	// SphereとBoxが離れている
	if (distanceSq > radiusSq)
	{
		return false;
	}

	// ==============================
	// Sphere中心がBox内部にない場合
	// ==============================

	if (distanceSq > 0.000001f)
	{
		float distance = std::sqrt(distanceSq);

		Math::Vector3 normal = diff / distance;

		float overlap = sphere.Radius - distance;

		outPush = normal * overlap;

		outNormal = normal;

		return true;
	}

	// ==============================
	// Sphere中心がBox内部にある場合
	//
	// closestPoint == sphereCenter
	// になるので、通常の方法では
	// 押し戻し方向を求められない。
	// ==============================

	float distanceLeft = sphereCenter.x - boxMin.x;

	float distanceRight = boxMax.x - sphereCenter.x;

	float distanceBottom = sphereCenter.y - boxMin.y;

	float distanceTop = boxMax.y - sphereCenter.y;

	float distanceBack= sphereCenter.z - boxMin.z;

	float distanceFront = boxMax.z - sphereCenter.z;

	// 一番近い面までの距離
	float minDistance = distanceLeft;

	outPush = Math::Vector3(-(distanceLeft + sphere.Radius), 0.0f, 0.0f);

	if (distanceRight < minDistance)
	{
		minDistance = distanceRight;

		outPush = Math::Vector3(distanceRight + sphere.Radius, 0.0f, 0.0f);
	}

	if (distanceBottom < minDistance)
	{
		minDistance = distanceBottom;

		outPush = Math::Vector3(-(distanceBottom + sphere.Radius), 0.0f, 0.0f);
	}

	if (distanceTop < minDistance)
	{
		minDistance = distanceTop;

		outPush = Math::Vector3(distanceTop + sphere.Radius, 0.0f, 0.0f);
	}

	if (distanceBack < minDistance)
	{
		minDistance = distanceBack;

		outPush = Math::Vector3(-(distanceBack + sphere.Radius), 0.0f, 0.0f);
	}

	if (distanceFront < minDistance)
	{
		minDistance = distanceFront;

		outPush = Math::Vector3(distanceFront + sphere.Radius, 0.0f, 0.0f);
	}

	return true;
}

void CollisionManager::RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
{
	if (!object)
	{
		return;
	}

	objectList& objectList = m_objectLists[ToIndex(layer)];

	// 同じオブジェクトの二重登録を防ぐ
	for (const auto& weakObject : objectList)
	{
		const auto registerObject = weakObject.lock();

		if (registerObject == object)
		{
			return;
		}
	}

	objectList.emplace_back(object);
}

void CollisionManager::UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
{
	objectList& objectList = m_objectLists[ToIndex(layer)];

	std::erase_if(objectList,
		[&object](const std::weak_ptr<KdGameObject>& weakObject)
		{
			const auto registerObject = weakObject.lock();

			return !registerObject || registerObject == object;
		});
}

const CollisionManager::objectList& CollisionManager::GetObjects(CollisionLayer layer) const
{
	return m_objectLists[ToIndex(layer)];
}

void CollisionManager::RemoveExpiredObjects()
{
	for (auto& objectList : m_objectLists)
	{
		std::erase_if(objectList,
			[](const std::weak_ptr<KdGameObject>& weakObject)
			{
				return weakObject.expired();
			});
	}
}

void CollisionManager::Clear()
{
	for (auto& objectList : m_objectLists)
	{
		objectList.clear();
	}
	
}

void CollisionManager::Resolve()
{

	// Character同士からPushを計算
	ResolveCharacterCollision();

	// ノックバックをSubStepで適用
	for (auto& character : GetCharacters())
	{
		ApplyKnockBack(character);
	}

	// 最後に全員まとめて壁とのめり込みを再確認
	ResolveWallCollision();

	// 地面・坂との着地判定
	ResolveGroundCollision();

	// Groundによる位置変更後の保険
	ResolveWallCollision();
}


Math::Vector3 CollisionManager::ResolveWallCollisionForCharacter(const std::shared_ptr<CharacterBase>& character)
{
	if (!character)
	{
		return Math::Vector3::Zero;
	}

	Math::Vector3 totalWallPush = Math::Vector3::Zero;

	const auto& walls =WallCollisionManager::Instance().GetWallCollisionList();

	for (const auto& wall : walls)
	{
		if (!wall)
		{
			continue;
		}

		Math::Vector3 push;
		Math::Vector3 normal;

		if (SphereVsAABB(character->GetBumpSphere(),wall->GetBox(),push,normal))
		{
			character->SetPos(character->GetPos() + push);

			totalWallPush += push;
		}
	}

	return totalWallPush;
}

void CollisionManager::ResolveGroundCollisionForCharacter(const std::shared_ptr<CharacterBase>& character)
{

	if (!character)
	{
		return;
	}

	const auto& grounds = GetObjects(CollisionLayer::Ground);

	KdCollider::RayInfo rayInfo;
	rayInfo.m_pos = character->GetPos();
	

	constexpr float enableStepHigh = 0.2f;
	constexpr float groundSnapDistance = 0.2;

	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	rayInfo.m_range =
		character->GetGravity() + enableStepHigh + groundSnapDistance;

	rayInfo.m_type = KdCollider::TypeGround;

	for (const auto& weakGround : grounds)
	{
		auto ground = weakGround.lock();
		if (!ground)
		{
			continue;
		}
		std::list<KdCollider::CollisionResult> retRayList;
		ground->Intersects(rayInfo, &retRayList);

		// ③ 結果を使って座標を補完する
		// レイに当たったリストから一番近いオブジェクトを検出
		float maxOverLap = 0;
		Math::Vector3 hitPos = {};
		bool hit = false;
		for (auto& ret : retRayList)
		{
			// レイを遮断しオーバーした長さが
			// 一番長いものを探す
			if (maxOverLap < ret.m_overlapDistance)
			{
				maxOverLap = ret.m_overlapDistance;
				hitPos = ret.m_hitPos;
				hit = true;
			}
		}
		if (hit)
		{
			// 地面に当たっている
			character->SetPos(hitPos);
		}
	}

}

void CollisionManager::ApplyCharacterPush(const std::shared_ptr<CharacterBase>& character)
{

	if (!character)
	{
		return;
	}

	Math::Vector3 push =character->GetTotalPush();

	constexpr float MaxStep = 0.05f;

	float length = push.Length();

	if (length <= 0.000001f)
	{
		return;
	}

	int stepCount =static_cast<int>(std::ceil(length / MaxStep));

	Math::Vector3 delta =push /static_cast<float>(stepCount);

	for (int i = 0; i < stepCount; ++i)
	{
		// すでに進める方向が残っていない
		if (delta.LengthSquared() <= 0.000001f)
		{
			break;
		}

		character->SetPos(character->GetPos() + delta);

		Math::Vector3 wallPush= 
			ResolveWallCollisionForCharacter(character);

		if (wallPush.LengthSquared() > 0.000001f)
		{
			Math::Vector3 normal = wallPush;
			normal.Normalize();

			float dot = delta.Dot(normal);

			if (dot < 0.0f)
			{
				delta -= normal * dot;
			}
		}

		ResolveGroundCollisionForCharacter(character);
	}

	character->ClearPush();

}

void CollisionManager::ApplyKnockBack(const std::shared_ptr<CharacterBase>& character)
{

	if (!character)
	{
		return;
	}

	Math::Vector3 knockBack = character->GetKnockBack();

	float length = knockBack.Length();

	if (length <= 0.000001f)
	{
		return;
	}

	constexpr float maxStap = 0.05;

	int stepCount = static_cast<int>(std::ceil(length / maxStap));

	Math::Vector3 delta = knockBack / static_cast<float>(stepCount);


	for (int i = 0; i < stepCount; ++i)
	{
		character->SetPos(character->GetPos() + delta);

		ResolveGroundCollisionForCharacter(character);

		ResolveGroundCollisionForCharacter(character);
	}

	// 徐々に減衰
	knockBack *= 0.88;

	if (knockBack.LengthSquared() <= 0.000001f)
	{
		knockBack = Math::Vector3::Zero;
	}

	character->SetKnockBack(knockBack);

}

std::vector<std::shared_ptr<CharacterBase>> CollisionManager::GetCharacters()
{
	std::vector<std::shared_ptr<CharacterBase>> characters;

	for (const auto& weakObj :GetObjects(CollisionLayer::CharacterBump))
	{
		auto character =std::dynamic_pointer_cast<CharacterBase>(weakObj.lock());

		if (!character)
		{
			continue;
		}

		characters.push_back(character);
	}

	return characters;
}

void CollisionManager::ResolveCharacterCollision()
{

	std::vector<std::shared_ptr<CharacterBase>>characters = GetCharacters();

	constexpr int Iteration = 4;

	for(int iter=0;iter<Iteration;iter++)
	{

		// 全キャラの押し戻し量をクリア
		for (auto& character : characters)
		{
			character->ClearPush();
		}

		// 全ペアと判定
		for (size_t i = 0; i < characters.size(); i++)
		{
			for (size_t j = i + 1; j < characters.size(); j++)
			{
				auto& charaA = characters[i];
				auto& charaB = characters[j];


				KdCollider::SphereInfo sphereInfo(
					KdCollider::TypeBump,
					charaA->GetBumpSphere());

				std::list<KdCollider::CollisionResult>results;

				charaB->Intersects(sphereInfo, &results);

				for (auto& ret : results)
				{

					Math::Vector3 dir = ret.m_hitDir;
					dir.y = 0;


					if (dir.LengthSquared() < 0.000001f)
					{
						continue;
					}


					dir.Normalize();

					constexpr float PenetrationSlop = 0.02f;
					constexpr float CorrectionRate = 0.8f;

					float overlap =
						ret.m_overlapDistance - PenetrationSlop;

					if (overlap <= 0.0f)
					{
						continue;
					}


					Math::Vector3 push = dir * overlap * CorrectionRate;


					float rateA = charaA->GetBumpPushRate();
					float rateB = charaB->GetBumpPushRate();

					float total = rateA + rateB;

					if (total <= 0)
					{
						total = 2.0f;
						rateA = total / 2;
						rateB = total / 2;
					}

					Math::Vector3 pushA = push * (rateA / total);
					charaA->AddPush(pushA);

					Math::Vector3 pushB = -push * (rateB / total);
					charaB->AddPush(pushB);

				}
			}
		}

		// 補正済みPushをSubStepで適用
		for (auto& character : characters)
		{
			ApplyCharacterPush(character);
		}
	}

}

void CollisionManager::ResolveWallCollision()
{
	auto characters = GetCharacters();

	for (auto& character : characters)
	{
		ResolveWallCollisionForCharacter(character);
	}

}

void CollisionManager::ResolveGroundCollision()
{
	std::vector<std::shared_ptr<CharacterBase>>characters = GetCharacters();

	const auto& grounds = GetObjects(CollisionLayer::Ground);


	for(auto&character:characters)
	{
		KdCollider::RayInfo rayInfo;

		rayInfo.m_pos =character->GetPos();

		constexpr float enableStepHigh = 0.2f;
		constexpr float groundSnapDistance = 0.2;

		rayInfo.m_pos.y += enableStepHigh;

		// レイの発射方向を設定
		rayInfo.m_dir = Math::Vector3::Down;

		rayInfo.m_range =
			character->GetGravity() + enableStepHigh + groundSnapDistance;

		rayInfo.m_type = KdCollider::TypeGround;

		for (const auto& weakGround : grounds)
		{
			auto ground = weakGround.lock();
			if (!ground)
			{
				continue;
			}
			std::list<KdCollider::CollisionResult> retRayList;
			ground->Intersects(rayInfo, &retRayList);

			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			if (hit)
			{
				// 地面に当たっている
				character->SetPos(hitPos);
				character->SetGravity(0.0f);
				character->SetJumpFlg(false);
			}
		}
	}

}
