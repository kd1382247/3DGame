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

	// Character同士からPushを計算
	ResolveCharacterCollision();

	// ノックバック
	for (const auto& character : GetCharacters())
	{
		if (!character)
		{
			continue;
		}

		ApplyKnockBack(character);
	}

}

void CollisionManager::ApplyCharacterPush(const std::shared_ptr<CharacterBase>& character)
{

	if (!character)
	{
		return;
	}

	Math::Vector3 push =character->GetTotalPush();

	if (push.LengthSquared() <= 0.000001f)
	{
		character->ClearPush();
		return;
	}

	Math::Vector3 finalePos =
		ResolveCharacterDisplacement(character, character->GetPos(), push, false);

	character->SetPos(finalePos);

	character->ClearPush();

}

void CollisionManager::ApplyKnockBack(const std::shared_ptr<CharacterBase>& character)
{

	Math::Vector3 knockBack = character->GetKnockBack();

	if (knockBack.LengthSquared() <= 0.000001f)
	{
		return;
	}

	Math::Vector3 finalPos =
		ResolveCharacterDisplacement(character, character->GetPos(), knockBack, false);


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

		constexpr float penetrationThreshold = 0.001f;

		float pushLength = push.Length();

		if (pushLength <= penetrationThreshold)
		{
			continue;
		}

		constexpr float overlapSkin = 0.0001f;

		currentPos += normal * (pushLength + overlapSkin);
		

		if (normal.LengthSquared() > 0.000001f)
		{
			normal.Normalize();

			float into =remainingMove.Dot(normal);

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



	Math::Vector3 sphereOffset = character->GetBumpSphere().Center - character->GetPos();

	Math::Vector3 sphereCenter = currentPos + sphereOffset;

	DirectX::BoundingSphere sphere;
	sphere.Center = sphereCenter;
	sphere.Radius = character->GetBumpSphere().Radius;

	for (const auto& obbCollision : obbs)
	{
		if (!obbCollision)
		{
			continue;
		}

		Math::Vector3 push = Math::Vector3::Zero;
		Math::Vector3 normal = Math::Vector3::Zero;


		if (!SphereVsOBB(sphere, obbCollision->GetBox(), push, normal))
		{
			continue;
		}

		float pushLength = push.Length();

		constexpr float overlapSkin = 0.0001f;

		if (pushLength <= overlapSkin)
		{
			continue;
		}

		currentPos += normal * (pushLength + overlapSkin);

		// OBB内部へ進もうとしている移動成分も除去
		float into = remainingMove.Dot(normal);


		if (into < 0.0f)
		{
			remainingMove -= normal * into;
		}

	}

}

bool CollisionManager::IsWalkableSurface(const SweepHitResult& closestHit, const std::shared_ptr<CharacterBase>& character) const
{
	if(!character)
	{
		return false;
	}

	const float maxSlopeAngle =character->GetMaxWalkableSlopeAngle();

	const float maxSlopeRadian =DirectX::XMConvertToRadians(maxSlopeAngle);

	const float walkableGroundDot =std::cos(maxSlopeRadian);

	return GetUpDot(closestHit) >= walkableGroundDot;
}

void CollisionManager::ResolveSweepHit(const std::shared_ptr<CharacterBase>& character, const SweepHitResult& closestHit, Math::Vector3& currentPos, Math::Vector3& remainingMove, const Math::Vector3& sourceMove,bool updateGroundState)
{

	const float upDot = GetUpDot(closestHit);

	const bool isWalkable = IsWalkableSurface(closestHit,character);

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
	if (updateGroundState&&isWalkable&&sourceMove.y<0.0f)
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
	if (updateGroundState&&isWalkable&&sourceMove.y<=0.0f)
	{
		character->SetGravity(0.0f);
		character->SetIsGrounded(true);
	}

	// 天井側の面
	if (updateGroundState&&upDot < 0.0f&&sourceMove.y>0.0f)
	{
		// 上昇を止める
		character->SetGravity(0.0f);
	}

}

void CollisionManager::ResolveCharacterMovement()
{
	std::vector<std::shared_ptr<CharacterBase>>characters = GetCharacters();

	
	for (const auto& character : characters)
	{
		if (!character)
		{
			continue;
		}

		Math::Vector3 finalPos =
			ResolveCharacterDisplacement(character, character->GetPrevPos(), character->GetPendingMove(),true);
		

		character->SetPos(finalPos);
	}
}

Math::Vector3 CollisionManager::ResolveCharacterDisplacement(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& startPos, const Math::Vector3& move, bool updateGroundState)
{

	constexpr int MaxSweepInteration = 4;
	constexpr float moveEpsilon = 0.000001f;


	Math::Vector3 currentPos = startPos;
	Math::Vector3 remainingMove = move;

	for (int i = 0; i < MaxSweepInteration; i++)
	{

		if (move.LengthSquared() <= moveEpsilon)
		{
			break;
		}

		ResolveAABBStartOverlap(character, currentPos, remainingMove);

		ResolveOBBStartOverlap(character, currentPos, remainingMove);


		//	始めにAABBを調べる
		SweepHitResult closestHit = FindClosestAABBHit(character, currentPos, remainingMove);

		// OBBを調べる
		SweepHitResult obbHit = FindClosestOBBHit(character, currentPos, remainingMove);

		if (obbHit.m_hit && obbHit.m_toi < closestHit.m_toi)
		{
			closestHit = obbHit;
		}

		if (!closestHit.m_hit)
		{
			currentPos += remainingMove;
			break;
		}


		ResolveSweepHit(character, closestHit, currentPos, remainingMove,move,updateGroundState);

	}

	return currentPos;
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

bool CollisionManager::SphereVsOBB(const DirectX::BoundingSphere& sphere, const DirectX::BoundingOrientedBox& obb, Math::Vector3& outPush, Math::Vector3& outNormal)
{
	Math::Vector4 orientation = obb.Orientation;

	// Sphere中心をOBBローカル空間へ
	Math::Vector3 localCenter =
		XMVector3InverseRotate(Math::Vector3(sphere.Center) - Math::Vector3(obb.Center),orientation);

	DirectX::BoundingSphere localSphere;
	localSphere.Center = localCenter;
	localSphere.Radius = sphere.Radius;

	// OBBはローカル空間ではAABB
	DirectX::BoundingBox localBox;
	localBox.Center = { 0.0f, 0.0f, 0.0f };
	localBox.Extents = obb.Extents;

	Math::Vector3 localPush = Math::Vector3::Zero;
	Math::Vector3 localNormal = Math::Vector3::Zero;

	if (!SphereVsAABB(localSphere,localBox,localPush,localNormal))
	{
		return false;
	}

	// ローカル → ワールド
	outPush =XMVector3Rotate(localPush,orientation);

	outNormal =XMVector3Rotate(localNormal,orientation);

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
		XMVector3InverseRotate(start - Math::Vector3(obb.Center),orientation);

	Math::Vector3 localMove =
		XMVector3InverseRotate(move,orientation);

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

	outNormal =XMVector3Rotate(localNormal,orientation);

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
					// 相手から離れる方向
					Math::Vector3 dir = ret.m_hitDir;

					// character同士は水平方向だけ押し合う
					dir.y = 0;

					// 押し出す方向を求められない
					if (dir.LengthSquared() < 0.000001f)
					{
						continue;
					}

	
					dir.Normalize();

			
					float overlap = ret.m_overlapDistance;

					if (overlap <= 0.0f)
					{
						continue;
					}

					Math::Vector3 push = dir * overlap;

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

		for (auto& character : characters)
		{
			ApplyCharacterPush(character);
		}
	}
}