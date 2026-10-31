#include "CharacterMover.h"

#include"../../TimeManager/TimeManager.h"
#include"../CollisionMath/CollisionMath.h"
#include"../CollisionRegistry/CollisionRegistry.h"

#include"../../../../Framework/GameObject/KdGameObject.h"
#include"../IObstacle/IObstacle.h"
#include"../../../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include"../../../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"

#include"../../../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"

#include"../../../GameObject/Character/CharacterBase.h"


void CharacterMover::ApplyKnockBack(const std::shared_ptr<CharacterBase>& character)
{

	Math::Vector3 knockBack = character->GetKnockBack();

	if (knockBack.LengthSquared() <= 0.000001f)
	{
		return;
	}


	const float deltaTime = TimeManager::Instance().GetDeltaTime()*60;

	Math::Vector3 finalPos =
		ResolveCharacterDisplacement(character, character->GetPos(), knockBack*deltaTime, false);


	// 徐々に減衰
	knockBack *= std::pow(0.88f, deltaTime);

	if (knockBack.LengthSquared() <= 0.000001f)
	{
		knockBack = Math::Vector3::Zero;
	}

	character->SetPos(finalPos);

	character->SetKnockBack(knockBack);

}

void CharacterMover::CollectObstacles()
{
	m_obstacles.clear();

	for (const auto& aabb : AABBCollisionManager::Instance().GetAABBCollisionList())
	{
		if (!aabb)
		{
			continue;
		}

		m_obstacles.push_back(aabb.get());
	}

	for (const auto& obb : OBBCollisionManager::Instance().GetOBBCollisionList())
	{
		if (!obb)
		{
			continue;
		}

		m_obstacles.push_back(obb.get());
	}

	for (const auto& weakObj : m_registry.GetObjects(CollisionLayer::Gate))
	{
		auto obstacle = dynamic_cast<const IObstacle*>(weakObj.lock().get());

		if (!obstacle)
		{
			continue;
		}

		m_obstacles.push_back(obstacle);
	}

	for (const auto& weakObj : m_registry.GetObjects(CollisionLayer::Gimmick))
	{
		auto obstacle = dynamic_cast<const IObstacle*>(weakObj.lock().get());

		if (!obstacle)
		{
			continue;
		}

		m_obstacles.push_back(obstacle);
	}

}

void CharacterMover::ResolveSweepContacts(
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

CharacterMover::SweepResult CharacterMover::FindSweepContacts(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& currentPos, const Math::Vector3& remainingMove)
{
	SweepResult result;

	if (!character)
	{
		return result;
	}

	//============================================
	// Sweep判定の準備
	//============================================

	// 移動量の長さ
	const float moveLength = remainingMove.Length();

	// 判定を始める位置
	Math::Vector3 sweepStart = character->GetBumpSphereAt(currentPos).Center;

	const float radius = character->GetBumpSphereRadius();

	// 球の動く範囲
	const Math::Vector3 sweepEnd = sweepStart + remainingMove;

	const Math::Vector3 reach(radius + CullMargin, radius + CullMargin, radius + CullMargin);
	const Math::Vector3 sweepMin = Math::Vector3::Min(sweepStart, sweepEnd) - reach;
	const Math::Vector3 sweepMax = Math::Vector3::Max(sweepStart, sweepEnd) + reach;


	// ===========================================================
	// 判定開始
	// ===========================================================

	for (const auto& obstacle : m_obstacles)
	{
		
		if (!obstacle->IsActiveFor(character.get()))
		{
			continue;
		}

		// ボックスの範囲を求める
		Math::Vector3 boxMin = {};
		Math::Vector3 boxMax = {};

		obstacle->GetWorldBounds(boxMin, boxMax);

		// 重なっていなければ飛ばす
		if (!CollisionMath::AABBVsAABB(sweepMin,sweepMax,boxMin,boxMax))
		{
			continue;
		}

		float toi = {};
		Math::Vector3 normal = {};

		// 当たっていなければ飛ばす
		if (!obstacle->SweepSphere(sweepStart, remainingMove, radius, toi, normal))
		{
			continue;
		}

		bool isWalkable = false;

		if (ShouldSkipWalkableObbContact(obstacle->IsWalkableOnly(), normal, character, isWalkable))
		{
			continue;
		}

		AddSweepContact(result, toi, normal, moveLength);

	}

	return result;

}

void CharacterMover::AddSweepContact(SweepResult& result, float toi, const Math::Vector3& normal, float moveLength)
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

void CharacterMover::ResolveStartOverlapContact(const Math::Vector3& push, const Math::Vector3& normal, Math::Vector3& currentPos, Math::Vector3& remainingMove, bool isWalkable)
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
			CollisionMath::PreserveHorizontalSpeedOnSlope(preProjectMove, contactNormal, remainingMove);
		}
		else
		{
			remainingMove -= contactNormal * into;
		}
	}
}

void CharacterMover::ResolveObstacleOverlap(const std::shared_ptr<CharacterBase>& character, Math::Vector3& currentPos, Math::Vector3& remainingMove)
{

	const float radius = character->GetBumpSphereRadius();
	const Math::Vector3 reach(radius + CullMargin, radius + CullMargin, radius + CullMargin);

	for (const auto& obstacle : m_obstacles)
	{
		if (!obstacle->IsActiveFor(character.get()))
		{
			continue;
		}

		DirectX::BoundingSphere sphere = character->GetBumpSphereAt(currentPos);
		
		// 球の範囲を求める
		Math::Vector3 sphereMin = sphere.Center - reach;
		Math::Vector3 sphereMax = sphere.Center + reach;

		// ボックスの範囲を求める
		Math::Vector3 boxMin = {};
		Math::Vector3 boxMax = {};

		obstacle->GetWorldBounds(boxMin, boxMax);

		// 重なっているか調べる
		if (!CollisionMath::AABBVsAABB(sphereMin,sphereMax,boxMin,boxMax))
		{
			continue;
		}


		Math::Vector3 push = Math::Vector3::Zero;
		Math::Vector3 normal = Math::Vector3::Zero;

		if (!obstacle->OverlapSphere(sphere,push,normal))
		{
			continue;
		}

		bool isWalkable = false;

		// 歩ける面かどうか
		if (ShouldSkipWalkableObbContact(obstacle->IsWalkableOnly(), normal, character,isWalkable))
		{
			continue;
		}

		ResolveStartOverlapContact(push, normal, currentPos, remainingMove, isWalkable);
	}
}

void CharacterMover::ResolveMultipleSurfaceHit(
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
		CollisionMath::PreserveHorizontalSpeedOnSlope(remainingMove, normal, candidate);

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

void CharacterMover::UpdateCharacterSurfaceState(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& normal, const Math::Vector3& sourceMove, bool updateGroundState)
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
		m_groundedCurr.insert(character.get());
	}

	// 天井
	if (upDot < 0.0f && sourceMove.y > 0.0f)
	{
		character->SetGravity(0.0f);
	}
}

bool CharacterMover::IsWalkableSurface(const Math::Vector3& normal, const std::shared_ptr<CharacterBase>& character) const
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

bool CharacterMover::ShouldSkipWalkableObbContact(bool isObbTypeWalkable, const Math::Vector3& normal, const std::shared_ptr<CharacterBase>& character, bool& outIsWalkable) const
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

void CharacterMover::ResolveSweepHit(
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
			CollisionMath::PreserveHorizontalSpeedOnSlope(preProjectMove, normal, leftover);
		}
		else
		{
			leftover -= normal * into;
		}
	}

	remainingMove = leftover;
	
	UpdateCharacterSurfaceState(character,normal,sourceMove,updateGroundState);

}

void CharacterMover::BeginFrame(const std::vector<std::shared_ptr<CharacterBase>>& characters)
{
	// 前フレームの接地記録を引き継ぎ、今フレーム分を空にする
	// (SetIsGrounded(false)で消える前の接地状態を、吸い付き判定に使うため)
	m_groundedPrev = std::move(m_groundedCurr);
	m_groundedCurr.clear();

	// まず全キャラクターを非接地状態にする
	for (const auto& character : characters)
	{
		if (!character)
		{
			continue;
		}

		character->SetIsGrounded(false);
	}

	// 当たり判定オブジェクトを更新
	CollectObstacles();

}

void CharacterMover::ResolveCharacterMovement(const std::vector<std::shared_ptr<CharacterBase>>& characters)
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

Math::Vector3 CharacterMover::ResolveCharacterDisplacement(const std::shared_ptr<CharacterBase>& character, const Math::Vector3& startPos, const Math::Vector3& move, bool updateGroundState)
{

	constexpr int MaxSweepIteration = 4;
	constexpr float moveEpsilon = 0.000001f;

	Math::Vector3 currentPos = startPos;
	Math::Vector3 remainingMove = move;

	// =====================================
	// 移動開始時のめり込みだけ最初に解消
	// =====================================
	ResolveObstacleOverlap(character, currentPos, remainingMove);

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

	// =====================================
	// 下り坂などで地面から離れたら、真下の坂に吸い付かせる
	// (ノックバックやキャラ同士の押し戻しでは行わない)
	// =====================================
	if (updateGroundState)
	{
		SnapToGround(character, currentPos, move);
	}

	return currentPos;
}

void CharacterMover::SnapToGround(
	const std::shared_ptr<CharacterBase>& character,
	Math::Vector3& currentPos,
	const Math::Vector3& sourceMove)
{
	if (!character)
	{
		return;
	}

	// 前フレームに接地していなかった(ジャンプ中・落下中)なら吸い付かない
	if (m_groundedPrev.find(character.get()) == m_groundedPrev.end())
	{
		return;
	}

	// 今フレームすでに接地している
	if (m_groundedCurr.find(character.get()) != m_groundedCurr.end())
	{
		return;
	}

	// 上昇中(ジャンプ直後など)は吸い付かない
	if (sourceMove.y > 0.0f)
	{
		return;
	}

	constexpr float snapMargin = 0.05f;      // 吸い付く距離の余裕
	constexpr float maxSnapDistance = 0.5f;  // 吸い付く距離の上限(段差から飛び降りた時に引き戻さないため)
	constexpr float skin = 0.001f;           // 面からわずかに離す量

	// 登れる最大角度の坂を水平に進んだ時の高低差 = 水平移動量 * tan(最大歩行角度)
	const float horizontalLength =
		std::sqrt(sourceMove.x * sourceMove.x + sourceMove.z * sourceMove.z);

	const float maxSlopeRadian =
		DirectX::XMConvertToRadians(character->GetMaxWalkableSlopeAngle());

	const float snapDistance =
		std::min(horizontalLength * std::tan(maxSlopeRadian) + snapMargin, maxSnapDistance);

	const Math::Vector3 snapMove(0.0f, -snapDistance, 0.0f);

	// 今の位置から真下へバンプスフィアを飛ばして、地面を探す
	const SweepResult sweepResult = FindSweepContacts(character, currentPos, snapMove);

	if (!sweepResult.m_hit)
	{
		return;
	}

	for (const auto& normal : sweepResult.m_normals)
	{
		// 壁や急な面には吸い付かない
		if (!IsWalkableSurface(normal, character))
		{
			continue;
		}

		// 接触位置まで真下へ移動し、面からわずかに離す
		currentPos += snapMove * sweepResult.m_toi;
		currentPos.y += normal.y * skin;

		// 接地フラグONと重力リセット(snapMove.y < 0 なので接地扱いになる)
		UpdateCharacterSurfaceState(character, normal, snapMove, true);

		return;
	}
}