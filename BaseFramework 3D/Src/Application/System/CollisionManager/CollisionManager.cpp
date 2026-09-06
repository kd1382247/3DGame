#include "CollisionManager.h"

#include"../TimeManager/TimeManager.h"

#include"../../../Framework/GameObject/KdGameObject.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollision.h"

#include"../../GameObject/Stage/Stage01/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/OBBCollision/OBBCollision.h"


#include"../../GameObject/Character/CharacterBase.h"

namespace
{
	size_t ToIndex(CollisionLayer layer)
	{
		return static_cast<size_t>(layer);
	}
}

void CollisionManager::DrawDebug()
{
	m_pDebugWire->Draw();
}

void CollisionManager::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
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

	outNormal = Math::Vector3::Left;

	if (distanceRight < minDistance)
	{
		minDistance = distanceRight;

		outPush = Math::Vector3(distanceRight + sphere.Radius, 0.0f, 0.0f);


		outNormal = Math::Vector3::Right;
	}

	if (distanceBottom < minDistance)
	{
		minDistance = distanceBottom;

		outPush = Math::Vector3(0.0f, -(distanceBottom + sphere.Radius), 0.0f);


		outNormal = Math::Vector3::Down;
	}

	if (distanceTop < minDistance)
	{
		minDistance = distanceTop;

		outPush = Math::Vector3(0.0f, distanceTop + sphere.Radius, 0.0f);


		outNormal = Math::Vector3::Up;
	}

	if (distanceBack < minDistance)
	{
		minDistance = distanceBack;

		outPush = Math::Vector3(0.0f, 0.0f, -(distanceBack + sphere.Radius));


		outNormal = Math::Vector3::Forward;
	}

	if (distanceFront < minDistance)
	{
		minDistance = distanceFront;

		outPush = Math::Vector3(0.0f, 0.0f, distanceFront + sphere.Radius);


		outNormal = Math::Vector3::Backward;
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

	// まず全キャラクターを非接地状態にする
	for (const auto& character : GetCharacters())
	{
		if (!character)
		{
			continue;
		}

		character->SetIsGrounded(false);
	}


	
	ResolveCharacterMovement();
	//ResolveGroundSnap();
	
	//ResolveBoxGroundSnap();

	

	// Character同士からPushを計算
	//ResolveCharacterCollision();


	// 最後に全員まとめて壁とのめり込みを再確認
	//ResolveWallCollision();

	// 地面・坂との着地判定
	//ResolveGroundCollision();

	// Groundによる位置変更後の保険
	//ResolveWallCollision();
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
	constexpr float groundSnapDistance = 0.2f;

	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	float deltaTime = TimeManager::Instance().GetDeltaTime();
	float fallDistance = 0.0f;

	if (character->GetGravity() > 0)
	{
		fallDistance = character->GetGravity() * deltaTime;
	}

	rayInfo.m_range =fallDistance+ enableStepHigh + groundSnapDistance;

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

	constexpr float maxStap = 0.05f;

	int stepCount = static_cast<int>(std::ceil(length / maxStap));

	Math::Vector3 delta = knockBack / static_cast<float>(stepCount);


	for (int i = 0; i < stepCount; ++i)
	{
		character->SetPos(character->GetPos() + delta);

		ResolveGroundCollisionForCharacter(character);

		ResolveWallCollisionForCharacter(character);
	}

	// 徐々に減衰
	knockBack *= 0.88f;

	if (knockBack.LengthSquared() <= 0.000001f)
	{
		knockBack = Math::Vector3::Zero;
	}

	character->SetKnockBack(knockBack);

}

void CollisionManager::UpdateClosestHit(SweepHitResult& closestHit, float toi, const Math::Vector3& normal)
{
	// 今まで見つけた衝突より遠いなら更新しない
	if (toi >= closestHit.m_toi)
	{
		return;
	}

	constexpr float walkableGroundDot = 0.707f;

	float upDot =
		normal.Dot(Math::Vector3::Up);

	bool isWalkable =
		upDot >= walkableGroundDot;

	closestHit.m_hit = true;
	closestHit.m_toi = toi;
	closestHit.m_normal = normal;
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

void CollisionManager::ResolveAABBStartOverlap(const std::shared_ptr<CharacterBase>& character, Math::Vector3& currentPos, Math::Vector3& remainingMove)
{
	const auto& walls =
		WallCollisionManager::Instance().GetWallCollisionList();

	for (const auto& wall : walls)
	{
		if (!wall)
		{
			continue;
		}

		Math::Vector3 sphereOffset =character->GetBumpSphere().Center -character->GetPos();

		Math::Vector3 sphereCenter =currentPos + sphereOffset;

		DirectX::BoundingSphere sphere;
		sphere.Center = sphereCenter;
		sphere.Radius = character->GetBumpSphere().Radius;

		Math::Vector3 push = Math::Vector3::Zero;
		Math::Vector3 normal = Math::Vector3::Zero;

		if (!SphereVsAABB(sphere,wall->GetBox(),push,normal))
		{
			continue;
		}

		constexpr float penetrationSlop = 0.001f;

		float pushLength = push.Length();

		if (pushLength > penetrationSlop)
		{
			currentPos +=
				normal * (pushLength - penetrationSlop);
		}

		if (normal.LengthSquared() > 0.000001f)
		{
			normal.Normalize();

			float into =
				remainingMove.Dot(normal);

			if (into < 0.0f)
			{
				remainingMove -=normal * into;
			}
		}
	}
}

void CollisionManager::ResolveOBBStartOverlap(const std::shared_ptr<CharacterBase>& character, Math::Vector3& currentPos, Math::Vector3& remainingMove)
{

	const auto& obbs = OBBCollisionManager::Instance().GetOBBCollisionList();

	for (const auto& obbCollision : obbs)
	{
		if (!obbCollision)
		{
			continue;
		}

		Math::Vector3 sphereOffset = character->GetBumpSphere().Center -character->GetPos();

		Math::Vector3 sphereCenter = currentPos + sphereOffset;

		DirectX::BoundingSphere sphere;
		sphere.Center = sphereCenter;
		sphere.Radius = character->GetBumpSphere().Radius;

		Math::Vector3 push = Math::Vector3::Zero;
		Math::Vector3 normal = Math::Vector3::Zero;

		if (!SphereVsOBB(sphere,obbCollision->GetBox(),push,normal))
		{
			continue;
		}

		constexpr float penetrationSlop = 0.001f;

		float pushLength = push.Length();

		if (pushLength <= penetrationSlop)
		{
			continue;
		}

		// めり込んでいる分だけ外へ戻す
		currentPos +=normal * (pushLength - penetrationSlop);

		// OBB内部へ進もうとしている移動成分も除去
		float into =remainingMove.Dot(normal);

		if (into < 0.0f)
		{
			remainingMove -=normal * into;
		}
	}

}

bool CollisionManager::IsWalkableSurface(const SweepHitResult& closestHit) const
{
	// 歩ける面の角度(45°)
	constexpr float walkableGroundDot = 0.707f;

	return GetUpDot(closestHit) >= walkableGroundDot;
}

void CollisionManager::ResolveSweepHit(const std::shared_ptr<CharacterBase>& character, const SweepHitResult& closestHit, Math::Vector3& currentPos, Math::Vector3& remainingMove)
{

	const float upDot = GetUpDot(closestHit);

	const bool isWalkable = IsWalkableSurface(closestHit);

    // 衝突位置まで移動
	Math::Vector3 move = remainingMove * closestHit.m_toi;

	currentPos += move;

    // 面からわずかに離す
	constexpr float skin = 0.001f;

	if (isWalkable)
	{
		// 歩行可能面では横方向に押さない
		currentPos.y += closestHit.m_normal.y * skin;
	}
	else
	{
		// 壁・天井では今まで通り法線方向へ離す
		currentPos += closestHit.m_normal * skin;
	}


    // 衝突後に残っている移動量
	Math::Vector3 leftover = remainingMove * (1.0f - closestHit.m_toi);


    // 接地中の下方向移動を除去
	if (isWalkable && character->GetPendingMove().y < 0.0f)
	{
		leftover.y = 0.0f;
	}
	

    // 面の内部へ進む成分を除去
	float into =leftover.Dot(closestHit.m_normal);

	if (into < 0.0f)
	{
		leftover -= closestHit.m_normal * into;
	}

	remainingMove = leftover;

	
	// 歩行可能な床
	if (isWalkable &&character->GetPendingMove().y <= 0.0f)
	{
		character->SetGravity(0.0f);
		character->SetIsGrounded(true);
	}


	// 天井側の面
	if (upDot < 0.0f &&character->GetPendingMove().y > 0.0f)
	{
		// 上昇を止める
		character->SetGravity(0.0f);
	}

}

void CollisionManager::ResolveGroundSnap()
{
	auto characters = GetCharacters();

	const auto& grounds = GetObjects(CollisionLayer::Ground);

	for(const auto&character:characters)
	{

		if (!character)
		{
			continue;
		}

		if (character->GetPendingMove().y > 0.0f)
		{
			continue;
		}

		constexpr float enableStepHigh = 0.2f;
		constexpr float groundSnapDistance = 0.2f;

		KdCollider::RayInfo rayInfo;

		rayInfo.m_pos = character->GetPos();
		rayInfo.m_pos.y += enableStepHigh;

		rayInfo.m_dir = Math::Vector3::Down;

		rayInfo.m_range =
			enableStepHigh + groundSnapDistance;

		rayInfo.m_type =
			KdCollider::TypeGround;

		bool hit = false;
		float maxOverlap = 0.0f;
		Math::Vector3 hitPos = {};
		Math::Vector3 hitNormal = {};

		for (const auto& weakObj : grounds)
		{
			auto ground = weakObj.lock();

			if (!ground)
			{
				continue;
			}

			std::list < KdCollider::CollisionResult >result;

			ground->Intersects(rayInfo, &result);

			for (const auto& ret : result)
			{
				if (maxOverlap < ret.m_overlapDistance)
				{
					maxOverlap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hitNormal = ret.m_hitNDir;
					hit = true;
				}
			}
		}

		if (hit)
		{
			Math::Vector3 groundNormal = hitNormal;
			groundNormal.Normalize();

			constexpr float walkableGroundDot = 0.707f;

			float groundDot =
				groundNormal.Dot(Math::Vector3::Up);

			bool isWalkable =
				groundDot >= walkableGroundDot;

			if (isWalkable)
			{
				// 地面に当たっている
				character->SetPos(hitPos);
				character->SetGravity(0.0f);
				character->SetIsGrounded(true);
			}
		}
	}

}

void CollisionManager::ResolveCharacterMovement()
{
	std::vector<std::shared_ptr<CharacterBase>>characters = GetCharacters();

	for (const auto& character : characters)
	{

		Math::Vector3 currentPos = character->GetPrevPos();
		Math::Vector3 remainingMove = character->GetPendingMove();


		KdDebugGUI::Instance().ClearLog();

		for (int i = 0; i < 3; i++)
		{

			ResolveAABBStartOverlap(character, currentPos, remainingMove);

			ResolveOBBStartOverlap(character, currentPos, remainingMove);
			

			//	始めにAABBを調べる
			SweepHitResult closestHit= FindClosestAABBHit(character, currentPos, remainingMove);

			
			// OBBを調べる
			SweepHitResult obbHit = FindClosestOBBHit(character, currentPos, remainingMove);

			if (obbHit.m_hit &&obbHit.m_toi < closestHit.m_toi)
			{
				closestHit = obbHit;
			}

			if (!closestHit.m_hit)
			{
				currentPos += remainingMove;
				break;
			}

			ResolveSweepHit(character,closestHit,currentPos,remainingMove);

		}

		character->SetPos(currentPos);
	}
}

CollisionManager::SweepHitResult CollisionManager::FindClosestOBBHit(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& currentPos, const Math::Vector3& remainingMove)
{
	
	 SweepHitResult closestHit;

	 const auto& obbs = OBBCollisionManager::Instance().GetOBBCollisionList();

	 for (const auto& obbCollision : obbs)
	 {
		 if (!obbCollision)
		 {
			 continue;
		 }

		 float toi = 0.0f;
		 Math::Vector3 normal = Math::Vector3::Zero;
		 Math::Vector3 sphereOffset = character->GetBumpSphere().Center - character->GetPos();

		 Math::Vector3 sweepStart = currentPos + sphereOffset;

		 bool hit = SphereSweepVsOBB(
			 sweepStart,
			 remainingMove,
			 character->GetBumpSphere().Radius,
			 obbCollision->GetBox(),
			 toi,
			 normal);

		 if (!hit)
		 {
			 continue;
		 }
		 
		 UpdateClosestHit(closestHit, toi, normal);

	 }

	 return closestHit;

}

 CollisionManager::SweepHitResult CollisionManager::FindClosestAABBHit(const std::shared_ptr<CharacterBase>& character,const Math::Vector3& currentPos,const Math::Vector3& remainingMove)
 {

	 SweepHitResult closestHit;

	 const auto& walls =
		 WallCollisionManager::Instance().GetWallCollisionList();

	 for (const auto& wall : walls)
	 {
		 if (!wall)
		 {
			 continue;
		 }

		 float toi = 0.0f;
		 Math::Vector3 normal = Math::Vector3::Zero;

		 //	ボックスの範囲を算出
		 Math::Vector3 center = wall->GetBox().Center;
		 Math::Vector3 extents = wall->GetBox().Extents;

		 Math::Vector3 boxMax = center + extents;
		 Math::Vector3 boxMin = center - extents;

		 Math::Vector3 sphereOffset =
			 character->GetBumpSphere().Center - character->GetPos();

		 Math::Vector3 sweepStart =currentPos + sphereOffset;


		 bool hit = SphereSweepVsAABB(sweepStart,character->GetBumpSphere().Radius,remainingMove,boxMin,boxMax,toi,normal);

		 // =========================
		 // Sweep Hit
		 // =========================

		 if (!hit)
		 {
			 continue;
		 }

		 UpdateClosestHit(closestHit, toi, normal);
	
	 }

	 return closestHit;

 }

void CollisionManager::ResolveBoxGroundSnap()
{
	auto characters = GetCharacters();

	const auto& obbs =
		OBBCollisionManager::Instance().GetOBBCollisionList();

	constexpr float snapDistance = 0.2f;
	constexpr float walkableGroundDot = 0.707f;

	for (const auto& character : characters)
	{
		if (!character)
		{
			continue;
		}

		// 上昇中はGroundSnapしない
		if (character->GetPendingMove().y > 0.0f)
		{
			continue;
		}

		// ---------------------------------
		// キャラクターの足元からRayを飛ばす
		// ---------------------------------
		Math::Vector3 rayStart = character->GetPos();

		bool foundGround = false;
		float closestDistance = snapDistance;

		Math::Vector3 groundPos = {};
		Math::Vector3 groundNormal = {};

		for (const auto& obbCollision : obbs)
		{
			if (!obbCollision)
			{
				continue;
			}

			const auto& obb =
				obbCollision->GetBox();

			float distance = 0.0f;

			// 足元から真下
			if (!obb.Intersects(
				rayStart,
				Math::Vector3::Down,
				distance))
			{
				continue;
			}

			// Snap可能距離より遠い
			if (distance > snapDistance)
			{
				continue;
			}

			// ---------------------------------
			// OBB上面の法線
			// ---------------------------------
			Math::Vector3 normal =
				DirectX::XMVector3Rotate(
					Math::Vector3::Up,
					Math::Vector4(obb.Orientation));

			normal.Normalize();

			float groundDot =
				normal.Dot(Math::Vector3::Up);

			if (groundDot < walkableGroundDot)
			{
				continue;
			}

			// 一番近いものだけ採用
			if (!foundGround ||
				distance < closestDistance)
			{
				foundGround = true;
				closestDistance = distance;

				groundPos =
					rayStart +
					Math::Vector3::Down * distance;

				groundNormal = normal;
			}
		}

		if (!foundGround)
		{
			continue;
		}

		character->SetPos(groundPos);
		character->SetGravity(0.0f);
		character->SetIsGrounded(true);
	}
}

bool CollisionManager::SphereVsOBB(const DirectX::BoundingSphere& sphere, const DirectX::BoundingOrientedBox& obb, Math::Vector3& outPush, Math::Vector3& outNormal)
{
	Math::Vector4 orientation = obb.Orientation;

	// Sphere中心をOBBローカル空間へ
	Math::Vector3 localCenter =
		XMVector3InverseRotate(
			Math::Vector3(sphere.Center) - Math::Vector3(obb.Center),
			orientation);

	DirectX::BoundingSphere localSphere;
	localSphere.Center = localCenter;
	localSphere.Radius = sphere.Radius;

	// OBBはローカル空間ではAABB
	DirectX::BoundingBox localBox;
	localBox.Center = { 0.0f, 0.0f, 0.0f };
	localBox.Extents = obb.Extents;

	Math::Vector3 localPush = Math::Vector3::Zero;
	Math::Vector3 localNormal = Math::Vector3::Zero;

	if (!SphereVsAABB(
		localSphere,
		localBox,
		localPush,
		localNormal))
	{
		return false;
	}

	// ローカル → ワールド
	outPush =
		XMVector3Rotate(
			localPush,
			orientation);

	outNormal =
		XMVector3Rotate(
			localNormal,
			orientation);

	if (outNormal.LengthSquared() > 0.000001f)
	{
		outNormal.Normalize();
	}

	return true;


}

bool CollisionManager::SphereSweepVsOBB(const Math::Vector3& start, const Math::Vector3& move, float radius, const DirectX::BoundingOrientedBox& obb, float& outTOI, Math::Vector3& outNormal)
{
	
	Math::Vector4 orientation = obb.Orientation;

	// -------------------------------
    // World → OBBローカル
    // -------------------------------

	Math::Vector3 localStart =
		XMVector3InverseRotate(
			start - Math::Vector3(obb.Center),
			orientation);

	Math::Vector3 localMove =
		XMVector3InverseRotate(
			move,
			orientation);

	// ============================
	// OBBローカルではAABBになる
	// ============================

	Math::Vector3 extents = obb.Extents;

	Math::Vector3 boxMin = -extents;
	Math::Vector3 boxMax = extents;

	Math::Vector3 localNormal = Math::Vector3::Zero;

	// ============================
	// 既存のAABB Sweepを再利用
	// ============================

	if (!SphereSweepVsAABB(
		localStart,
		radius,
		localMove,
		boxMin,
		boxMax,
		outTOI,
		localNormal))
	{
		return false;
	}

	// ============================
	// Local Normal → World Normal
	// ============================

	outNormal =
		XMVector3Rotate(
			localNormal,
			orientation);

	outNormal.Normalize();

	return true;

}

bool CollisionManager::SphereSweepVsAABB(const Math::Vector3& start, float radius, const Math::Vector3& move, const Math::Vector3& boxMin, const Math::Vector3& boxMax, float& outTOI, Math::Vector3& outNormal)
{

	Math::Vector3 radiusVec(radius, radius, radius);

	// スフィアの半径分、ボックスの範囲を広げる
	Math::Vector3 expandedMin = boxMin - radiusVec;
	Math::Vector3 expandedMax = boxMax + radiusVec;

	return SegmentVsAABB(start, move, expandedMin, expandedMax, outTOI, outNormal);
}

bool CollisionManager::SegmentVsAABB(const Math::Vector3& start, const Math::Vector3& move, const Math::Vector3& boxMin, const Math::Vector3& boxMax, float& outTOI, Math::Vector3& outNormal)
{
	
	constexpr float epsilon = 0.000001f;

	// 衝突位置
	float tEnter = 0.0f;
	float tExit = 1.0f;

	// 当たった面の法線ベクトル
	Math::Vector3 hitNormal = Math::Vector3::Zero;

	// X軸
	if (std::abs(move.x) < epsilon)
	{
		// 動いていない場合、ボックスの範囲内にいるか
		if (start.x<boxMin.x || start.x>boxMax.x)
		{
			return false;
		}
	}
	else
	{
		float t1 = (boxMin.x - start.x) / move.x;
		float t2 = (boxMax.x - start.x) / move.x;

		float axisEnter = std::min(t1, t2);
		float axisExit = std::max(t1, t2);

		if (axisEnter > tEnter)
		{
			tEnter = axisEnter;

			// 法線ベクトルを更新
			hitNormal = move.x > 0 ?
				Math::Vector3::Left :
				Math::Vector3::Right;
		}

		tExit = std::min(tExit, axisExit);

		if (tEnter > tExit)
		{
			return false;
		}
	}

	// Y軸
	if (std::abs(move.y) < epsilon)
	{
		// 動いていない場合、ボックスの範囲内にいるか
		if (start.y<boxMin.y || start.y>boxMax.y)
		{
			return false;
		}
	}
	else
	{
		float t1 = (boxMin.y - start.y) / move.y;
		float t2 = (boxMax.y - start.y) / move.y;

		float axisEnter = std::min(t1, t2);
		float axisExit = std::max(t1, t2);

		if (axisEnter > tEnter)
		{
			tEnter = axisEnter;

			// 法線ベクトルを更新
			hitNormal = move.y > 0 ?
				Math::Vector3::Down :
				Math::Vector3::Up;
		}

		tExit = std::min(tExit, axisExit);

		if (tEnter > tExit)
		{
			return false;
		}
	}

	// Z軸
	if (std::abs(move.z) < epsilon)
	{
		// 動いていない場合、ボックスの範囲内にいるか
		if (start.z<boxMin.z || start.z>boxMax.z)
		{
			return false;
		}
	}
	else
	{
		float t1 = (boxMin.z - start.z) / move.z;
		float t2 = (boxMax.z - start.z) / move.z;

		float axisEnter = std::min(t1, t2);
		float axisExit = std::max(t1, t2);

		if (axisEnter > tEnter)
		{
			tEnter = axisEnter;

			// 法線ベクトルを更新
			hitNormal = move.z > 0 ?
				Math::Vector3::Forward :
				Math::Vector3::Backward;
		}

		tExit = std::min(tExit, axisExit);

		if (tEnter > tExit)
		{
			return false;
		}
	}

	
	// 移動によって新しく面に衝突していない
	if (hitNormal.LengthSquared() <= 0.000001f)
	{

		return false;
	}

	outTOI = tEnter;
	outNormal = hitNormal;

	return true;

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

				if (charaA->IsInOutro() ||
					charaB->IsInOutro())
				{
					continue;
				}


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
		constexpr float groundSnapDistance = 0.2f;

		rayInfo.m_pos.y += enableStepHigh;

		// レイの発射方向を設定
		rayInfo.m_dir = Math::Vector3::Down;

		float deltaTime = TimeManager::Instance().GetDeltaTime();
		float fallDistance = 0.0f;

		if(character->GetGravity()>0)
		{
			fallDistance = character->GetGravity() * deltaTime;
		}

		rayInfo.m_range = fallDistance + enableStepHigh + groundSnapDistance;

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
				character->SetIsGrounded(true);
			}
		}
	}

}
