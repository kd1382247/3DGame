#include "CollisionManager.h"

#include"../TimeManager/TimeManager.h"
#include"CollisionMath/CollisionMath.h"

#include"../../../Framework/GameObject/KdGameObject.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollision.h"

#include"../../GameObject/Stage/Stage01/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/OBBCollision/OBBCollision.h"

#include"../../GameObject/Character/CharacterBase.h"


void CollisionManager::DrawDebug()
{
	m_pDebugWire->Draw();
}

void CollisionManager::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void CollisionManager::Resolve()
{

	if (TimeManager::Instance().IsHitStop())
	{
		return;
	}

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

	// ノックバック
	for (const auto& character : GetCharacters())
	{
		if (!character)
		{
			continue;
		}

		ApplyKnockBack(character);
	}

	// 最後にキャラ同士の判定をする
	ResolveCharacterCollision();

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

	Math::Vector3 finalPos =
		ResolveCharacterDisplacement(character, character->GetPos(), push, false);

	character->SetPos(finalPos);

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

	character->SetPos(finalPos);

	character->SetKnockBack(knockBack);

}

void CollisionManager::ResolveSweepContacts(
	const std::shared_ptr<CharacterBase>& character,
	const SweepResult& sweepResult,
	Math::Vector3& currentPos,
	Math::Vector3& remainingMove,
	const Math::Vector3& sourceMove,
	bool updateGroundState)
{
	if (!character || !sweepResult.m_hit || sweepResult.m_normals.empty())
	{
		return;
	}

	// ==============================
	// 1面接触
	// ==============================

	if (sweepResult.m_normals.size() == 1)
	{
		ResolveSweepHit(
			character,
			sweepResult.m_toi,
			sweepResult.m_normals[0],
			currentPos,
			remainingMove,
			sourceMove,
			updateGroundState);

		return;
	}

	// ==============================
	// 複数面接触
	// ==============================

	ResolveMultipleSurfaceHit(
		character,
		sweepResult,
		currentPos,
		remainingMove,
		sourceMove,
		updateGroundState);
}

CollisionManager::SweepResult CollisionManager::FindSweepContacts(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& currentPos, const Math::Vector3& remainingMove)
{
	SweepResult result;

	if (!character)
	{
		return result;
	}

	constexpr float contactDistanceEpsilon = 0.002f;

	const float moveLength = remainingMove.Length();

	Math::Vector3 sphereOffset =
		character->GetBumpSphere().Center - character->GetPos();

	Math::Vector3 sweepStart = currentPos + sphereOffset;

	const auto& walls =WallCollisionManager::Instance().GetWallCollisionList();

	for (const auto& wall : walls)
	{
		if (!wall)
		{
			continue;
		}

		const auto& box = wall->GetBox();

		Math::Vector3 center = box.Center;
		Math::Vector3 extents = box.Extents;

		Math::Vector3 boxMin = center - extents;

		Math::Vector3 boxMax = center + extents;

		float toi = 0.0f;
		Math::Vector3 normal = Math::Vector3::Zero;

		bool hit =
			CollisionMath::SphereSweepVsAABB(
				sweepStart,
				character->GetBumpSphere().Radius,
				remainingMove,
				boxMin,
				boxMax,
				toi,
				normal);

		if (!hit)
		{
			continue;
		}

		AddSweepContact(result, toi, normal, moveLength);
	}

	const auto& obbs = OBBCollisionManager::Instance().GetOBBCollisionList();

	for (const auto& obbCollision : obbs)
	{
		if (!obbCollision)
		{
			continue;
		}

		float toi = 0.0f;

		Math::Vector3 normal = Math::Vector3::Zero;

		bool hit =
			CollisionMath::SphereSweepVsOBB(
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

		// 坂用OBBは歩行可能面だけ
		if (obbCollision->GetCollisionType() == OBBCollision::OBBCollisionType::Walkable)
		{

			if (!IsWalkableSurface(normal,character))
			{
				continue;
			}
		}

		AddSweepContact(result, toi, normal, moveLength);
	}

	return result;

}

void CollisionManager::AddSweepContact(SweepResult& result, float toi, const Math::Vector3& normal, float moveLength)
{
	constexpr float contactDistanceEpsilon = 0.002f;

	const float hitDistance = toi * moveLength;

	// =====================================
	// 最初のHit
	// =====================================

	if (!result.m_hit)
	{
		result.m_hit = true;
		result.m_toi = toi;

		result.m_normals.clear();
		result.m_normals.push_back(normal);

		return;
	}

	const float closestDistance =
		result.m_toi * moveLength;

	const float distanceDiff =
		hitDistance - closestDistance;

	// =====================================
	// 今までより明らかに手前
	// =====================================

	if (distanceDiff < -contactDistanceEpsilon)
	{
		result.m_toi = toi;

		result.m_normals.clear();
		result.m_normals.push_back(normal);

		return;
	}

	// =====================================
	// ほぼ同時接触
	// =====================================

	if (std::abs(distanceDiff) <=
		contactDistanceEpsilon)
	{
		// 重複Normalも意図的に保持する
		result.m_normals.push_back(normal);
	}

}

void CollisionManager::ResolveStartOverlapContact(const Math::Vector3& push, const Math::Vector3& normal, Math::Vector3& currentPos, Math::Vector3& remainingMove)
{
	constexpr float overlapSkin = 0.0001f;

	const float pushLength = push.Length();

	if (pushLength <= overlapSkin)
	{
		return;
	}

	Math::Vector3 contactNormal = normal;

	if (contactNormal.LengthSquared() <= 0.000001f)
	{
		return;
	}

	contactNormal.Normalize();

	currentPos += contactNormal * (pushLength + overlapSkin);

	const float into = remainingMove.Dot(contactNormal);

	if (into < 0.0f)
	{
		remainingMove -= contactNormal * into;
	}
}

void CollisionManager::ResolveMultipleSurfaceHit(
	const std::shared_ptr<CharacterBase>& character,
	const SweepResult& sweepResult,
	Math::Vector3& currentPos,
	Math::Vector3& remainingMove,
	const Math::Vector3& sourceMove,
	bool updateGroundState)
{
	if (!character ||!sweepResult.m_hit ||sweepResult.m_normals.empty())
	{
		return;
	}

	// =====================================
	// 1. 最初の衝突時刻まで進む
	// =====================================

	currentPos += remainingMove * sweepResult.m_toi;

	// 衝突後に残った移動量
	remainingMove *= (1.0f - sweepResult.m_toi);

	// =====================================
	// 2. 全接触面から少し離す
	// =====================================

	constexpr float skin = 0.001f;

	for (const auto& normal : sweepResult.m_normals)
	{
		currentPos += normal * skin;
	}

	// =====================================
	// 3. 全接触面へ入り込む成分を除去
	// =====================================

	constexpr int ConstraintIterations = 4;

	for (int iteration = 0; iteration < ConstraintIterations; iteration++)
	{
		bool changed = false;

		for (const auto& normal : sweepResult.m_normals)
		{
			float into = remainingMove.Dot(normal);

			if (into < 0.0f)
			{
				remainingMove -= normal * into;

				changed = true;
			}
		}

		if (!changed)
		{
			break;
		}
	}

	// =====================================
	// 4. Ground / Ceiling判定
	// =====================================
	if (updateGroundState)
	{
		for (const auto& normal : sweepResult.m_normals)
		{
			UpdateCharacterSurfaceState(
				character,
				normal,
				sourceMove,
				updateGroundState);
		}
	}
}

void CollisionManager::UpdateCharacterSurfaceState(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& normal, const Math::Vector3& sourceMove, bool updateGroundState)
{
	if (!character || !updateGroundState)
	{
		return;
	}

	const bool isWalkable = IsWalkableSurface(normal, character);

	const float upDot = normal.Dot(Math::Vector3::Up);

	// 歩行可能な床
	if (isWalkable && sourceMove.y <= 0.0f)
	{
		character->SetGravity(0.0f);
		character->SetIsGrounded(true);
	}

	// 天井
	if (upDot < 0.0f && sourceMove.y > 0.0f)
	{
		character->SetGravity(0.0f);
	}

}

std::vector<std::shared_ptr<CharacterBase>> CollisionManager::GetCharacters()
{

	std::vector<std::shared_ptr<CharacterBase>> characters;

	for (const auto& weakObj : GetObjects(CollisionLayer::CharacterBump))
	{
		auto character = std::dynamic_pointer_cast<CharacterBase>(weakObj.lock());

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

		if(!CollisionMath::SphereVsAABB(sphere,wall->GetBox(),push,normal))
		{
			continue;
		}

		ResolveStartOverlapContact(push,normal,currentPos,remainingMove);

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

		Math::Vector3 sphereOffset = character->GetBumpSphere().Center - character->GetPos();

		Math::Vector3 sphereCenter = currentPos + sphereOffset;

		DirectX::BoundingSphere sphere;
		sphere.Center = sphereCenter;
		sphere.Radius = character->GetBumpSphere().Radius;

		Math::Vector3 push = Math::Vector3::Zero;
		Math::Vector3 normal = Math::Vector3::Zero;


		if (!CollisionMath::SphereVsOBB(sphere, obbCollision->GetBox(), push, normal))
		{
			continue;
		}

		// =====================================
		// 坂用OBBは歩行可能面だけを処理
		// =====================================
		if (obbCollision->GetCollisionType() ==OBBCollision::OBBCollisionType::Walkable)
		{
			if (!IsWalkableSurface(normal, character))
			{
				continue;
			}
		}

		ResolveStartOverlapContact(push, normal, currentPos, remainingMove);

	}
}

bool CollisionManager::IsWalkableSurface(const Math::Vector3& normal, const std::shared_ptr<CharacterBase>& character) const
{
	if(!character)
	{
		return false;
	}

	const float maxSlopeAngle =character->GetMaxWalkableSlopeAngle();

	const float maxSlopeRadian =DirectX::XMConvertToRadians(maxSlopeAngle);

	const float walkableGroundDot =std::cos(maxSlopeRadian);

	return normal.Dot(Math::Vector3::Up) >= walkableGroundDot;
}

void CollisionManager::ResolveSweepHit(
	const std::shared_ptr<CharacterBase>& character,
	float toi,
	const Math::Vector3 &normal,
	Math::Vector3& currentPos,
	Math::Vector3& remainingMove,
	const Math::Vector3& sourceMove,
	bool updateGroundState)
{

	const float upDot = normal.Dot(Math::Vector3::Up);

	const bool isWalkable = IsWalkableSurface(normal,character);

    // 衝突位置まで移動
	Math::Vector3 move = remainingMove * toi;

	currentPos += move;

    // 面からわずかに離す
	constexpr float skin = 0.001f;

	if (isWalkable)
	{
		// 歩行可能面では横方向に押さない
		currentPos.y += normal.y * skin;
	}
	else
	{
		// 壁・天井では今まで通り法線方向へ離す
		currentPos += normal * skin;
	}

    // 衝突後に残っている移動量
	Math::Vector3 leftover = remainingMove * (1.0f - toi);

    // 接地中の下方向移動を除去
	if (updateGroundState&&isWalkable&&sourceMove.y<0.0f)
	{
		leftover.y = 0.0f;
	}

    // 面の内部へ進む成分を除去
	float into =leftover.Dot(normal);

	if (into < 0.0f)
	{
		leftover -= normal * into;
	}

	remainingMove = leftover;
	
	UpdateCharacterSurfaceState(character,normal,sourceMove,updateGroundState);

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

	constexpr int MaxSweepIteration = 4;
	constexpr float moveEpsilon = 0.000001f;

	Math::Vector3 currentPos = startPos;
	Math::Vector3 remainingMove = move;

	// =====================================
	// 移動開始時のめり込みだけ最初に解消
	// =====================================
	ResolveAABBStartOverlap(character, currentPos, remainingMove);

	ResolveOBBStartOverlap(character, currentPos, remainingMove);

	// =====================================
	// 以降はSweepだけで移動を解決
	// =====================================
	for (int i = 0; i < MaxSweepIteration; i++)
	{

		if (remainingMove.LengthSquared() <= moveEpsilon)
		{
			break;
		}

		SweepResult sweepResult = FindSweepContacts(character, currentPos, remainingMove);

		// 何も当たらなかった
		if (!sweepResult.m_hit)
		{
			currentPos += remainingMove;
			break;
		}

		ResolveSweepContacts(
			character,
			sweepResult,
			currentPos,
			remainingMove,
			move,
			updateGroundState);

		continue;
		
	}


	return currentPos;
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