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

	// このフレームで使うキャラリストは1回だけ取得して使い回す
	const std::vector<std::shared_ptr<CharacterBase>> characters = GetCharacters();

	// まず全キャラクターを非接地状態にする
	for (const auto& character : characters)
	{
		if (!character)
		{
			continue;
		}

		character->SetIsGrounded(false);
	}

	ResolveCharacterMovement(characters);

	// ノックバック
	for (const auto& character : characters)
	{
		if (!character)
		{
			continue;
		}

		ApplyKnockBack(character);
	}

	// 最後にキャラ同士の判定をする
	ResolveCharacterCollision(characters);

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

	const float moveLength = remainingMove.Length();

	Math::Vector3 sweepStart = GetBumpSphereCenterAt(character, currentPos);

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
		bool isWalkableContact = false;
		const bool isObbTypeWalkable = (obbCollision->GetCollisionType() == OBBCollision::OBBCollisionType::Walkable);

		if (ShouldSkipWalkableObbContact(isObbTypeWalkable, normal, character, isWalkableContact))
		{
			continue;
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

void CollisionManager::ResolveStartOverlapContact(const Math::Vector3& push, const Math::Vector3& normal, Math::Vector3& currentPos, Math::Vector3& remainingMove, bool isWalkable)
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

	// 投影前の移動量を保持しておく(歩行可能面での速度補正に使う)
	const Math::Vector3 preProjectMove = remainingMove;

	const float into = remainingMove.Dot(contactNormal);

	if (into < 0.0f)
	{
		if (isWalkable)
		{
			// 坂に乗っている間は毎フレームここでめり込みが発生するが、
			// 歩行可能面では水平方向(X,Z)の移動量はそのまま保ち、
			// 坂の傾きに合わせて必要な高さだけを再計算する
			PreserveHorizontalSpeedOnSlope(preProjectMove, contactNormal, remainingMove);
		}
		else
		{
			remainingMove -= contactNormal * into;
		}
	}
}

void CollisionManager::PreserveHorizontalSpeedOnSlope(const Math::Vector3& preProjectMove, const Math::Vector3& normal, Math::Vector3& move) const
{
	// 法線のY成分が無いと平面の方程式が解けないため補正しない
	// (歩行可能面はIsWalkableSurfaceで角度チェック済みのため通常は発生しない)
	if (std::abs(normal.y) <= 0.000001f)
	{
		return;
	}

	const float dx = preProjectMove.x;
	const float dz = preProjectMove.z;

	// 元々水平方向の移動が無かった(重力のみ等)場合は補正しない
	if (dx * dx + dz * dz <= 0.000001f)
	{
		return;
	}

	// 坂の平面上の移動ベクトルは法線と直交する
	// (normal.x*dx + normal.y*dy + normal.z*dz = 0)ので、
	// 元の水平移動量(dx, dz)をそのまま保ったまま
	// 坂の平面に乗るために必要な高さdyを直接求める
	const float slopeY = -(normal.x * dx + normal.z * dz) / normal.y;

	move.x = dx;
	move.y = slopeY;
	move.z = dz;
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
	// 3.5 坂+壁の角など、歩行可能面を含む複数面接触の場合、
	// 他の面へめり込まない範囲でだけ水平速度を保つ補正を試みる
	// =====================================

	for (size_t i = 0; i < sweepResult.m_normals.size(); i++)
	{
		const Math::Vector3& normal = sweepResult.m_normals[i];

		if (!IsWalkableSurface(normal, character))
		{
			continue;
		}

		Math::Vector3 candidate = remainingMove;
		PreserveHorizontalSpeedOnSlope(remainingMove, normal, candidate);

		constexpr float penetrationEpsilon = 0.0001f;

		bool violatesOtherSurface = false;

		for (size_t j = 0; j < sweepResult.m_normals.size(); j++)
		{
			if (j == i)
			{
				continue;
			}

			if (candidate.Dot(sweepResult.m_normals[j]) < -penetrationEpsilon)
			{
				violatesOtherSurface = true;
				break;
			}
		}

		// 他の面(壁など)へめり込む場合は補正前の結果を優先する
		if (!violatesOtherSurface)
		{
			remainingMove = candidate;
		}

		// 歩行可能面は基本1つのはずなので、最初に見つかったものだけ処理する
		break;
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

		DirectX::BoundingSphere sphere;
		sphere.Center = GetBumpSphereCenterAt(character, currentPos);
		sphere.Radius = character->GetBumpSphere().Radius;

		Math::Vector3 push = Math::Vector3::Zero;
		Math::Vector3 normal = Math::Vector3::Zero;

		if(!CollisionMath::SphereVsAABB(sphere,wall->GetBox(),push,normal))
		{
			continue;
		}

		// 壁(AABB)は歩行可能面としては扱わない
		ResolveStartOverlapContact(push,normal,currentPos,remainingMove,false);

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

		DirectX::BoundingSphere sphere;
		sphere.Center = GetBumpSphereCenterAt(character, currentPos);
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
		bool isWalkableContact = false;
		const bool isObbTypeWalkable = (obbCollision->GetCollisionType() == OBBCollision::OBBCollisionType::Walkable);

		if (ShouldSkipWalkableObbContact(isObbTypeWalkable, normal, character, isWalkableContact))
		{
			continue;
		}

		ResolveStartOverlapContact(push, normal, currentPos, remainingMove, isWalkableContact);

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

Math::Vector3 CollisionManager::GetBumpSphereCenterAt(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& pos) const
{
	// バンプスフィアはキャラの原点からオフセットした位置にあるため、
	// 「pos にキャラがいたとしたら」のスフィア中心を求める
	const Math::Vector3 sphereOffset = character->GetBumpSphere().Center - character->GetPos();

	return pos + sphereOffset;
}

bool CollisionManager::ShouldSkipWalkableObbContact(bool isObbTypeWalkable, const Math::Vector3& normal, const std::shared_ptr<CharacterBase>& character, bool& outIsWalkable) const
{
	outIsWalkable = false;

	// 坂用(Walkable)でないOBBは通常の障害物として常に対象にする
	if (!isObbTypeWalkable)
	{
		return false;
	}

	// 坂用OBBは、登れる角度の面(歩行可能面)だけを対象にする
	if (!IsWalkableSurface(normal, character))
	{
		return true;
	}

	outIsWalkable = true;
	return false;
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

	// 投影前の移動量を保持しておく(歩行可能面での速度補正に使う)
	const Math::Vector3 preProjectMove = leftover;

    // 面の内部へ進む成分を除去
	float into =leftover.Dot(normal);

	if (into < 0.0f)
	{
		if (isWalkable)
		{
			// 歩行可能面では水平方向(X,Z)の移動量はそのまま保ち、
			// 坂の傾きに合わせて必要な高さだけを再計算する
			PreserveHorizontalSpeedOnSlope(preProjectMove, normal, leftover);
		}
		else
		{
			leftover -= normal * into;
		}
	}

	remainingMove = leftover;
	
	UpdateCharacterSurfaceState(character,normal,sourceMove,updateGroundState);

}

void CollisionManager::ResolveCharacterMovement(const std::vector<std::shared_ptr<CharacterBase>>& characters)
{
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

	// デバッグ用ログ：ノックバック/プッシュ解決(updateGroundState==false)では
	// 毎キャラ分呼ばれて上書きされてしまい意味をなさないため、
	// 本来の移動解決(ResolveCharacterMovementから呼ばれる時)だけ表示する
	if (updateGroundState)
	{
		// 実際に移動した量
		Math::Vector3 actualMove = currentPos - startPos;

		// 入力された移動量の水平成分
		Math::Vector3 inputHorizontal = move;
		inputHorizontal.y = 0.0f;

		// 実際の移動量の水平成分
		Math::Vector3 actualHorizontal = actualMove;
		actualHorizontal.y = 0.0f;

		KdDebugGUI::Instance().ClearLog();

		KdDebugGUI::Instance().AddLog("Input  X: %.4f  Z: %.4f",
			inputHorizontal.x,
			inputHorizontal.z);
		KdDebugGUI::Instance().AddLog("\nActual X: %.4f  Z: %.4f",
			actualHorizontal.x,
			actualHorizontal.z);

		KdDebugGUI::Instance().AddLog(
			"\nActual Y: %.4f",
			actualMove.y);
	}

	return currentPos;
}

void CollisionManager::ResolveCharacterCollision(const std::vector<std::shared_ptr<CharacterBase>>& characters)
{
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